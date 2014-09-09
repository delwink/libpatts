BUILD NOTES
===========

pattsd is designed for *nix operating systems, and it is built using the GNU
Build System.

To Build
--------

    ./autogen.sh
    ./configure
    ./make

Dependencies
------------

Library | Purpose     | Description
------- | ----------- | --------------------------------------
libssl  | SSL Support | Secure communications
MariaDB | Database    | Data Storage (MySQL may be compatible)

Dependency Build Instructions: Debian and *buntu
------------------------------------------------

Build requirements:

    # apt-get install build-essential pkg-config
    # apt-get install libtool autotools-dev autoconf automake
    # apt-get install libssl-dev mariadb-server
