#!/usr/bin/env bash
# Fetch the latest version of the library
fetch() {
if [ -d "microui" ]; then return; fi
URL="https://github.com/rxi/microui/archive/refs/heads/master.zip"
ZIP="${URL##*/}"
DIR="microui-master"
mkdir -p .build
cd .build

# Download the release
if [ ! -f "$ZIP" ]; then
  echo "Downloading $ZIP from $URL ..."
  curl -L "$URL" -o "$ZIP"
  echo ""
fi

# Unzip the release
if [ ! -d "$DIR" ]; then
  echo "Unzipping $ZIP to .build/$DIR ..."
  cp "$ZIP" "$ZIP.bak"
  unzip -q "$ZIP"
  rm "$ZIP"
  mv "$ZIP.bak" "$ZIP"
  echo ""
fi
cd ..

# Copy the libs to the package directory
echo "Copying libs to microui/ ..."
rm -rf microui
mkdir -p microui
cp -rf  ".build/$DIR/src"/* microui/
echo ""
}


# Test the project
test() {
echo "Running 01-basic-window.c ..."
clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 01.exe examples/01-basic-window.c  && ./01.exe && echo -e "\n"
# echo "Running 02-slider-panel.c ..."
# clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 02.exe examples/02-slider-panel.c  && ./02.exe && echo -e "\n"
# echo "Running 03-popup-dialog.c ..."
# clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 03.exe examples/03-popup-dialog.c  && ./03.exe && echo -e "\n"
}


# Main script
if [[ "$1" == "test" ]]; then test
elif [[ "$1" == "fetch" ]]; then fetch
else echo "Usage: $0 {fetch|test}"; fi

# 01-basic-window.c
# 02-slider-panel.c
# 03-popup-dialog.c
