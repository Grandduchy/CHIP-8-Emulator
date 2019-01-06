# CHIP-8-Emulator
A simple CHIP-8 Emulator created in C++ &nbsp;&nbsp;&nbsp;&nbsp;[![Build Status](https://travis-ci.org/Grandduchy/CHIP-8-Emulator.svg?branch=master)](https://travis-ci.org/Grandduchy/CHIP-8-Emulator) <br />
This program uses QT for the GUI and QT for compilation through qmake. <br />
The emulator comes with the games UFO, Tetris, Space Invaders and Pong <br />

# Preparation
The project requires QT, install by:
```
 sudo apt-get build-essential qt5-default
```
Alternatively Qt can be installed through an installer [here](https://www.qt.io/download)

# Quick Start
First clone this repository
```
 git clone https://github.com/Grandduchy/CHIP-8-Emulator
```
Run configure
```
cd CHIP-8-Emulator
./build.sh
```
Incase you do not have permission, add executable permission by:
```
chmod +x build.sh
```
Configure creates the executable in build directory, to run the program
```
build/Chip-8
```
# License
[MIT License](https://github.com/Grandduchy/CHIP-8-Emulator/blob/master/LICENSE)
