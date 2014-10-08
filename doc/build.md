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

Library        | Description                                  | Purpose
-------------- | -------------------------------------------- | ---------------------
MariaDB Client | Data Storage (MySQL may be compatible)       | Database Connectivity
ICU            | International Components for Unicode library | Unicode Support

Dependency Build Instructions: Debian and Trisquel
--------------------------------------------------

Build requirements:

    # apt-get install build-essential pkg-config
    # apt-get install libtool autotools-dev autoconf automake
    # apt-get install mariadb-client libicu-dev

Dependency Build Instructions: Parabola
---------------------------------------

Build requirements:

    # pacman -S base-devel
    # pacman -S mariadb-clients icu
