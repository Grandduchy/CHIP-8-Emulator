#!/bin/sh
mkdir build
cd build
qmake ..
make
rm *.o *.h *.cpp

if [ -d ./.qmake.stash ]; then
    rm -rf .qmake.stash;
fi
