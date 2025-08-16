#!/bin/bash

# Check for jq
if ! command -v jq &> /dev/null; then
  echo "Error: 'jq' is required but not installed." >&2
  exit 1
fi

# Check for input file
if [ $# -lt 1 ]; then
  echo "Usage: $0 <layout.json>" >&2
  exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="generated_layouts.c"

echo "Generating Layouts from layout_config.json"

# Validate JSON and ensure it parses
layout_count=$(jq 'to_entries | map(select(.key != "header")) | length' "$INPUT_FILE" 2>/dev/null)

if [ -z "$layout_count" ]; then
  echo "Error: Could not parse input JSON (might be malformed)." >&2
  exit 1
fi

# Enforce layout limit (max 6)
if [ "$layout_count" -gt 6 ]; then
  echo "Error: Cannot have more than 6 layouts. Found $layout_count." >&2
  exit 1
fi


# Keep track of all layouts
layout_vars=()

# Read header
HEADER=$(jq -r '.header // empty' "$INPUT_FILE")
if [ -z "$HEADER" ]; then
  echo "Error: Missing or empty 'header' field." >&2
  exit 1
fi

# Start output file
echo "$HEADER" > "$OUTPUT_FILE"
echo >> "$OUTPUT_FILE"


# Iterate over all layouts (excluding "header")
while read -r layout_name; do
  layout_path=".$layout_name"

  # ---- ID ----
  id=$(jq -r "$layout_path.id // empty" "$INPUT_FILE")
  if [ -z "$id" ]; then
    echo "Error: '$layout_name' missing 'id' field." >&2
    exit 1
  fi

  echo "/* ===== Layout: $layout_name ===== */" >> "$OUTPUT_FILE"

  # ========== SubLayouts ==========
  declare -A MOD_KEYS=(
    [EMPTY]="MOD_NONE"
    [CTRL]="MOD_CTRL"
    [SHIFT]="MOD_SHIFT"
    [ALT]="MOD_ALT"
  )

  for mod in "${!MOD_KEYS[@]}"; do
    key_name="${mod}-key"
    mod_enum="${MOD_KEYS[$mod]}"
    var_name="${layout_name}_${mod}subLayout"

    key_obj=$(jq "$layout_path.\"$key_name\"" "$INPUT_FILE")
    if [ "$key_obj" == "null" ]; then
      # Output default empty sublayout
      echo "ModifierKeyboardSubLayout ${var_name} = {" >> "$OUTPUT_FILE"
      echo "  .key = {[0x00 ... 0xff] = CHARCODE_NONE}," >> "$OUTPUT_FILE"
      echo "  .intervals = 0x00," >> "$OUTPUT_FILE"
      echo "  .ranges = {[0x00 ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }}" >> "$OUTPUT_FILE"
      echo "};" >> "$OUTPUT_FILE"
      echo >> "$OUTPUT_FILE"
      continue
    fi

    echo "ModifierKeyboardSubLayout ${var_name} = {" >> "$OUTPUT_FILE"

    # ---- .key ----
    echo "  .key = {" >> "$OUTPUT_FILE"
    key_entries=$(jq -r "$layout_path.\"$key_name\".key | to_entries[] | \"[\" + .key + \"] = \" + .value + \",\"" "$INPUT_FILE")
    if [ -n "$key_entries" ]; then
      echo "$key_entries" | sed 's/^/    /' >> "$OUTPUT_FILE"
    fi
    echo "  }," >> "$OUTPUT_FILE"

    # ---- .intervals ----
    intervals=$(jq -r "$layout_path.\"$key_name\".intervals" "$INPUT_FILE")
    echo "  .intervals = 0x$(printf '%02x' "$intervals")," >> "$OUTPUT_FILE"

    # ---- .ranges ----
    echo "  .ranges = {" >> "$OUTPUT_FILE"
    range_count=0
    while read -r range; do
      start=$(echo "$range" | jq -r '.[0]')
      end=$(echo "$range" | jq -r '.[1]')
      echo "    [0x$(printf '%02x' "$range_count")] = {$start, $end}," >> "$OUTPUT_FILE"
      ((range_count++))
    done < <(jq -c "$layout_path.\"$key_name\".ranges[]" "$INPUT_FILE")

    # Fill rest with EMPTY_RANGE
      if [ "$range_count" -le 15 ]; then
        printf "    [0x%02x ... 0x0f] = { EMPTY_RANGE, EMPTY_RANGE }\n" "$range_count" >> "$OUTPUT_FILE"
      fi

    echo "  }" >> "$OUTPUT_FILE"
    echo "};" >> "$OUTPUT_FILE"
    echo >> "$OUTPUT_FILE"
  done

  # ========== KeyboardLayout ==========
  layout_vars+=("$layout_name")
  echo "KeyboardLayout ${layout_name} = {" >> "$OUTPUT_FILE"
  echo "  .id = \"$id\"," >> "$OUTPUT_FILE"

  # ---- .subLayoutFor ----
  echo "  .subLayoutFor = {" >> "$OUTPUT_FILE"
  echo "    [MOD_NONE] = &${layout_name}_EMPTYsubLayout," >> "$OUTPUT_FILE"
  echo "    [MOD_CTRL] = &${layout_name}_CTRLsubLayout," >> "$OUTPUT_FILE"
  echo "    [MOD_SHIFT] = &${layout_name}_SHIFTsubLayout," >> "$OUTPUT_FILE"
  echo "    [MOD_ALT] = &${layout_name}_ALTsubLayout," >> "$OUTPUT_FILE"
  echo "    [MOD_META] = &${layout_name}_EMPTYsubLayout" >> "$OUTPUT_FILE"
  echo "  }," >> "$OUTPUT_FILE"

  # ---- .VARGIYAdoubleKey ----
  enable_vk=$(jq -r "$layout_path.enableVARGIYAdoubleKey" "$INPUT_FILE")
  if [ "$enable_vk" == "true" ]; then
    # Validate VARGIYAvyanjana, varga, prana
    vy_path="$layout_path.VARGIYAvyanjana"
    has_varga=$(jq "$vy_path.varga? // empty | length" "$INPUT_FILE")
    has_prana=$(jq "$vy_path.prana? // empty | length" "$INPUT_FILE")
    if [ -z "$has_varga" ] || [ -z "$has_prana" ]; then
      echo "Error: '$layout_name' has 'enableVARGIYAdoubleKey=true' but is missing 'VARGIYAvyanjana.varga' or 'prana'." >&2
      exit 1
    fi
    echo "  .VARGIYAdoubleKey = {" >> "$OUTPUT_FILE"
    for row in varga prana; do
      keys=$(jq -r "$layout_path.VARGIYAvyanjana.$row[]" "$INPUT_FILE")
      key_array=()
      while read -r key; do
        key_array+=("$key")
      done <<< "$keys"
      echo -n "    {" >> "$OUTPUT_FILE"
      comma=""
      while read -r key; do
        echo -n "$comma$key" >> "$OUTPUT_FILE"
        comma=", "
      done <<< "$keys"
      echo "}," >> "$OUTPUT_FILE"
    done

    echo "  }," >> "$OUTPUT_FILE"
  fi

  # ---- .supportedScripts ----
  echo "  .supportedScripts = {" >> "$OUTPUT_FILE"
  script_list=$(jq -r "$layout_path.supportedScripts[]" "$INPUT_FILE")
  index=0
  for script in $script_list; do
    echo "    [$index] = $script," >> "$OUTPUT_FILE"
    ((index++))
  done
  if [ $index -le 31 ]; then
  echo "    [$index ... 31] = NO_SCRIPT," >> "$OUTPUT_FILE"
  fi
  echo "  }" >> "$OUTPUT_FILE"
  echo "};" >> "$OUTPUT_FILE"
  echo >> "$OUTPUT_FILE"

done < <(jq -r 'to_entries | map(select(.key != "header")) | .[].key' "$INPUT_FILE")

# === Generate defined_layout[6] ===
echo "KeyboardLayout* defined_layout[6] = {" >> "$OUTPUT_FILE"
for i in "${!layout_vars[@]}"; do
  echo "  &${layout_vars[$i]}," >> "$OUTPUT_FILE"
done
# Pad remaining with NULLs
for ((i=${#layout_vars[@]}; i<6; i++)); do
  echo "  NULL," >> "$OUTPUT_FILE"
done
echo "};" >> "$OUTPUT_FILE"

echo "Successfully Generated Layouts from layout_config.json"