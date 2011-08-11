#!/bin/sh
mac_lib=`dirname $0`/../fl-cow/.libs/libflcow.dylib
lnx_lib=`dirname $0`/../fl-cow/.libs/libflcow.so

export DYLD_INSERT_LIBRARIES=${mac_lib}
export DYLD_FORCE_FLAT_NAMESPACE=1
export LD_PRELOAD=${lnx_lib}

`dirname $0`/flcow-test



