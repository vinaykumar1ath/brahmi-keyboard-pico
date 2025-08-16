param (
  [Parameter(Mandatory = $true)]
  [string]$InputFile
)

Write-Host "Generating Layouts from layout_config.json"

$OutputFile = "generated_layouts.c"

# ==== Check input file exists ====
if (-not (Test-Path $InputFile)) {
  Write-Error "Error: File '$InputFile' not found."
  exit 1
}

# ==== Parse JSON ====
try {
  $json = Get-Content $InputFile -Raw | ConvertFrom-Json -ErrorAction Stop
} catch {
  Write-Error "Error: Could not parse input JSON (might be malformed)."
  exit 1
}

# ==== Validate header ====
if (-not $json.header) {
  Write-Error "Error: Missing or empty 'header' field."
  exit 1
}

# ==== Extract layout keys ====
$layoutKeys = $json.PSObject.Properties.Name | Where-Object { $_ -ne "header" }

if ($layoutKeys.Count -gt 6) {
  Write-Error "Error: Cannot have more than 6 layouts. Found $($layoutKeys.Count)."
  exit 1
}

# ==== Start output ====
Set-Content -Path $OutputFile -Value $json.header
Add-Content -Path $OutputFile -Value ""

$layoutVars = @()

# Modifier mapping
$MOD_KEYS = @{
  EMPTY = "MOD_NONE"
  CTRL  = "MOD_CTRL"
  SHIFT = "MOD_SHIFT"
  ALT   = "MOD_ALT"
}

# ==== Process each layout ====
foreach ($layoutName in $layoutKeys) {
  $layout = $json.$layoutName

  if (-not $layout.id) {
    Write-Error "Error: '$layoutName' missing 'id' field."
    exit 1
  }

  Add-Content -Path $OutputFile -Value "/* ===== Layout: $layoutName ===== */"

  foreach ($mod in $MOD_KEYS.Keys) {
    $keyName = "$mod-key"
    $modEnum = $MOD_KEYS[$mod]
    $varName = "${layoutName}_${mod}subLayout"

    $keyObj = $layout.$keyName

    if (-not $keyObj) {
      Add-Content $OutputFile @"
ModifierKeyboardSubLayout $varName = {
  .key = {[0x00 ... 0xff] = CHARCODE_NONE},
  .intervals = 0x00,
  .ranges = {[0x00 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }}
};

"@
      continue
    }

    Add-Content $OutputFile "ModifierKeyboardSubLayout $varName = {"
    Add-Content $OutputFile "  .key = {"

    foreach ($entry in $keyObj.key.PSObject.Properties) {
      Add-Content $OutputFile "    [$($entry.Name)] = $($entry.Value),"
    }

    Add-Content $OutputFile "  },"

    $intervals = "{0:x2}" -f [int]$keyObj.intervals
    Add-Content $OutputFile "  .intervals = 0x$intervals,"

    Add-Content $OutputFile "  .ranges = {"
    $rangeCount = 0
    foreach ($range in $keyObj.ranges) {
      $start = $range[0]
      $end = $range[1]
      $index = "{0:x2}" -f $rangeCount
      Add-Content $OutputFile "    [0x$index] = {$start, $end},"
      $rangeCount++
    }

    if ($rangeCount -le 15) {
      $index = "{0:x2}" -f $rangeCount
      Add-Content $OutputFile "    [0x$index ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }"
    }

    Add-Content $OutputFile "  }"
    Add-Content $OutputFile "};"
    Add-Content $OutputFile ""
  }

  # ====== KeyboardLayout =======
  $layoutVars += $layoutName

  Add-Content $OutputFile "KeyboardLayout $layoutName = {"
  Add-Content $OutputFile "  .id = `"$($layout.id)`","
  Add-Content $OutputFile "  .subLayoutFor = {"
  Add-Content $OutputFile "    [MOD_NONE] = &$layoutName`_EMPTYsubLayout,"
  Add-Content $OutputFile "    [MOD_CTRL] = &$layoutName`_CTRLsubLayout,"
  Add-Content $OutputFile "    [MOD_SHIFT] = &$layoutName`_SHIFTsubLayout,"
  Add-Content $OutputFile "    [MOD_ALT] = &$layoutName`_ALTsubLayout,"
  Add-Content $OutputFile "    [MOD_META] = &$layoutName`_EMPTYsubLayout"
  Add-Content $OutputFile "  },"

  # Handle VARGIYAdoubleKey
  if ($layout.enableVARGIYAdoubleKey -eq $true) {
    if (-not $layout.VARGIYAvyanjana -or -not $layout.VARGIYAvyanjana.varga -or -not $layout.VARGIYAvyanjana.prana) {
      Write-Error "Error: '$layoutName' has 'enableVARGIYAdoubleKey=true' but is missing 'VARGIYAvyanjana.varga' or 'prana'."
      exit 1
    }
    Add-Content $OutputFile "  .VARGIYAdoubleKey = {"
    foreach ($row in @("varga", "prana")) {
      $keys = $layout.VARGIYAvyanjana.$row
      $joined = ($keys -join ", ")
      Add-Content $OutputFile "    {$joined},"
    }
    Add-Content $OutputFile "  },"
  }

  # supportedScripts
  Add-Content $OutputFile "  .supportedScripts = {"
  $index = 0
  foreach ($script in $layout.supportedScripts) {
    Add-Content $OutputFile "    [$index] = $script,"
    $index++
  }

  if ($index -le 31) {
    Add-Content $OutputFile "    [$index ... 31] = NO_SCRIPT,"
  }

  Add-Content $OutputFile "  }"
  Add-Content $OutputFile "};"
  Add-Content $OutputFile ""
}

# ==== Final defined_layout[6] ====
Add-Content $OutputFile "KeyboardLayout* defined_layout[6] = {"
foreach ($name in $layoutVars) {
  Add-Content $OutputFile "  &$name,"
}
for ($i = $layoutVars.Count; $i -lt 6; $i++) {
  Add-Content $OutputFile "  NULL,"
}
Add-Content $OutputFile "};"

Write-Host "Successfully Generated Layouts from layout_config.json"
