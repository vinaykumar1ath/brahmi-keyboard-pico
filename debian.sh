#!/bin/bash

check_dependancies (){
  local dependancies_installed=1

  if ! command -v cmake &> /dev/null ; then
    echo "CMAKE is not installed"
    dependancies_installed=0
  fi

  if ! command -v ninja &> /dev/null ; then
    echo "NINJA is not installed"
    dependancies_installed=0
  fi

  if ! command -v arm-none-eabi-gcc &> /dev/null ; then
    echo "gcc-arm-none-eabi toolchain is not installed"
    dependancies_installed=0
  fi

  if [  -z "$PICO_SDK_PATH"  ]; then
    echo "PICO_SDK_PATH is not set"
    dependancies_installed=0
  fi

  if [ $dependancies_installed -eq 0 ]; then
    return 1
  else
    return 0
  fi
}

if(check_dependancies); then
  echo "All dependancies are installed"
else
  echo "Installing dependancies"
  curl -L "https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh" -o pico_setup.sh
  chmod +x pico_setup.sh
  export SKIP_OPENOCD=1
  export SKIP_UART=1
  ./pico_setup.sh
  export PICO_SDK_PATH="${PWD}/pico/pico-sdk"
  echo "export PICO_SDK_PATH=\"$PICO_SDK_PATH\"" >> ~/.bashrc
  source ~/.bashrc
  rm pico_setup.sh
fi

if [ -d "pico_device_and_host" ] && [ -f "pico_device_and_host/CMakeLists.txt" ]; then
  echo "pico_device_and_host installed"
else
  echo "installing pico_device_and_host"
  curl -L -o pdah.zip "https://github.com/brendena/pico_device_and_host/archive/refs/heads/main.zip"
  tar -xvf pdah.zip
  mv pico_device_and_host-main pico_device_and_host
  rm pdah.zip
fi

if [ -d "pico_device_and_host/Pico-PIO-USB" ] && [ -f "pico_device_and_host/Pico-PIO-USB/CMakeLists.txt" ]; then
  echo "Pico-PIO-USB installed"
else
  echo "Installing Pico-PIO-USB"
  curl -L "https://github.com/sekigon-gonnoc/Pico-PIO-USB/archive/refs/heads/main.zip" -o ppu.zip
  tar -xvf ppu.zip
  rm -rf pico_device_and_host/Pico-PIO-USB
  mv Pico-PIO-USB-main pico_device_and_host/Pico-PIO-USB
  rm -rf Pico-PIO-USB-main
  rm ppu.zip
fi

chmod +x ./brahmi_keyboard/generate-layout.sh
./brahmi_keyboard/generate-layout.sh layout_config.json
if [ $? -eq 0 ]; then
  echo "Parsed layout"
else
  echo "Failed to parse layout"
  exit 1
fi
mv -f generated_layouts.c ./brahmi_keyboard/src/generated_layouts.c


if ! [ -d ./build ]; then
  mkdir build
fi

cd build

if ! [ -f ./build.ninja ]; then
  cmake  -G Ninja ../brahmi_keyboard/
fi

ninja
cd ..

exit 0
