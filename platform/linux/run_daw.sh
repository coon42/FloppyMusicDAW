#!/bin/bash

export WXDIR=$PWD/../../src/lib/wxWidgets/gtk-build

if [ -d $WXDIR ]
then
    echo "Directory $WXDIR exists."
else
    echo "Error: Directory $WXDIR does not exists."

    mkdir ../../src/lib/wxWidgets/gtk-build
    pushd ../../src/lib/wxWidgets/gtk-build
    ../configure --prefix=$PWD/../lib/gtk3_so --with-gtk=3 --with-opengl
    make -j4
    make install
    popd
fi

export PATH="$PATH:$PWD/../../src/lib/wxWidgets/gtk-build"

if make; then
    bin/FloppyMusicDAW.elf
else
    echo build failed.
fi

