# DND Dice Roller Repository

## Introduction

This repository is for a DND Dice Roller, which was made to be able to roll large amounts of dice that would normally take up a lot of time during a session.

## Installation and Compilation

To compile and run this program, cmake, make or ninja and a C++ compile with support for C++23 needs to be installed.

After these are installed, go into the directory and create a subdirectory called build and go into it.
```
mkdir build
cd build
```

Then, CMake and make need to be run.
```
cmake ..
make
```
When cross compiling from linux to windows, use the following commands:
```
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-toolchain.cmake ..
make
```

At this point, the program can be run.
```
./DndDiceRoller
```
Or on Windows
```
./bundle/DndDiceRoller.exe
```


## Current Limitations

The program currently does not support all types of syntax for dice.<br>
Currently supported syntax is for example:
```
1d6+2 Piering + 2d10 -2 Force + 1d4 Acid
```
But not:
```
1d4+2 + 6d6 Slashing
```

## Copyright information

All code written in this project by the contributors is subject to copyright laws, with the ownership of the copyright lying with the contributors. The used copyright is [Creative Commons BY-CN-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/).