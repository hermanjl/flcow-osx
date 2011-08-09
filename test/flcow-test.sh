#!/bin/sh
mac_lib=$(pwd)/../fl-cow/.libs/libflcow.dylib
lnx_lib=$(pwd)/../fl-cow/.libs/libflcow.so

export DYLD_INSERT_LIBRARIES=${mac_lib}
export DYLD_FORCE_FLAT_NAMESPACE=1
export LD_PRELOAD=${lnx_lib}

./flcow-test



