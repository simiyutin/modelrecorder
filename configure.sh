#!/usr/bin/env bash

#install GLM
sudo apt install libglm-dev

#install GLFW
git clone https://github.com/glfw/glfw.git
cd glfw || exit
cmake ./
make
sudo make install

cd ..
rm -rf glfw
