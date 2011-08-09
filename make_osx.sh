#!/bin/bash
cp fl-cow/Makefile.in fl-cow/Makefile.in-bak
sed -i "" "s/-module/-dylib/g" fl-cow/Makefile.in
make
mv fl-cow/Makefile.in-bak fl-cow/Makefile.in