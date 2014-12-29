BUILD NOTES
===========

libpatts is designed for *nix operating systems, and it is built using the GNU
Build System.

To Build
--------

    ./autogen.sh
    ./configure
    make

Dependencies
------------

Library    | Description                                      | Purpose
---------- | ------------------------------------------------ | ---------------------------------------
libcquel   | MySQL C API wrapper with dynamic data structures | Database connectivity and UTF-8 support

Dependency Build Instructions: Debian and Trisquel
--------------------------------------------------

Build requirements:

    # apt-get install build-essential pkg-config
    # apt-get install libtool autotools-dev autoconf automake
    $ wget http://delwink.com/dl/libcquelX_Y.Z_ARCH.deb
    $ wget http://delwink.com/dl/libcquel-dev_Y.Z_all.deb
    # dpkg -i libcquelX_Y.Z_ARCH.deb libcquel-dev_Y.Z_all.deb

Dependency Build Instructions: Parabola
---------------------------------------

Build requirements:

    Install a cquel implementation (>= 4.1).

    # pacman -S base-devel
