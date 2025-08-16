 function Check-Dependancies {
  $installed_SDK=$true

  try{
   $ninja_version=(ninja --version)
    $global:ninja=(where.exe ninja)
    Write-Host "ninja version ${ninja_version}"
  } catch {
    if( -not ( Test-Path -Path "../ninja" -PathType Leaf )) {
      Write-Host "NINJA is not installed"
      curl.exe -o ninja.zip -L "https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-win.zip"
      tar xf ninja.zip
      rm ninja.zip
      $global:ninja= Join-Path $PSScriptRoot "ninja.exe"
    }
  }

  if( ($Env:PICO_SDK_PATH ) -or (Test-Path -Path "C:\Programs Files\Raspberry Pi\Pico SDK v1.5.1\pico-sdk"
 -PathType Container)) {
    Write-Host "Pico SDK is installed"
  } else {
    Write-Host "Pico-SDK is not installed"
    $installed_SDK=$false
  }
  return $installed_SDK
}

if( -not (Check-Dependancies)){
 Write-Host "Dependancies not installed"
 exit 1
}

if((Test-Path -Path "./pico_device_and_host" -PathType Container) -and (Get-ChildItem -Path "./pico_device_and_host" -File -Force).Count -gt 0){
  Write-Host "pico_device_and_host is already installed"
}else{
  # download pico_device_and_host
  curl.exe -L -o pdah.zip https://github.com/brendena/pico_device_and_host/archive/refs/heads/main.zip
  tar -xf pdah.zip
  mv -Force pico_device_and_host-main pico_device_and_host
  rm pdah.zip
}

if((Test-Path -Path "./pico_device_and_host/Pico-PIO-USB" -PathType Container) -and ((Get-ChildItem -Path "./pico_device_and_host/Pico-PIO-USB" -File -Force).Count -gt 0)){
  Write-Host "Pico-PIO-USB is already installed"
}else{
  #download Pico-PIO-USB
  curl.exe -L -o ppu.zip https://github.com/sekigon-gonnoc/Pico-PIO-USB/archive/refs/heads/main.zip
  tar -xf ppu.zip
  mv Pico-PIO-USB-main/* ./pico_device_and_host/Pico-PIO-USB
  rmdir Pico-PIO-USB-main
  rm ppu.zip
}

# set the environment variables
if( -not ($Env:PICO_SDK_PATH )){
  $Env:PICO_SDK_PATH="C:\Programs Files\Pico SDK v1.5.1\pico-sdk"
}
if( -not ( $Env:PICO_TOOLCHAIN_PATH )){
  $Env:PICO_TOOLCHAIN_PATH= Join-Path ( Split-Path $Env:PICO_SDK_PATH) "gcc-arm-none-eabi"
  Write-Host $Env:PICO_TOOLCHAIN_PATH
}

# Generate the layout
./brahmi_keyboard/Generate-Layout.ps1 -InputFile layout_config.json
if ($?) {
  Write-Host "Parsed layout"
} else {
  Write-Host "Failed to parse layout"
  exit 1
}
mv -Force generated_layouts.c ./brahmi_keyboard/src/


if( -not (Test-Path -Path "./build" -PathType Container)){
  mkdir build
}
cd build

# build the project
if( -not (Test-Path -Path "./build.ninja" -PathType Leaf)){
  & "$Env:PICO_SDK_PATH\..\cmake\bin\cmake.exe" -G Ninja -DCMAKE_MAKE_PROGRAM="$ninja" ../brahmi_keyboard/
}

Write-Host $ninja
& $ninja
cd ..
exit 0