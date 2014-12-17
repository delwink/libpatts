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
    $ wget http://delwink.com/dl/libcquel**X**\_**Y.Z**\_**ARCH**.deb
    $ wget http://delwink.com/dl/libcquel-dev\_**Y.Z**\_all.deb
    # dpkg -i libcquel**X**\_**Y.Z**\_**ARCH**.deb libcquel-dev\_**Y.Z**\_all.deb

Dependency Build Instructions: Parabola
---------------------------------------

Build requirements:

    # pacman -S base-devel
