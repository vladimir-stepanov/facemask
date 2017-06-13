HOWTO build and run v8-hello
-----------

1. Make sure you've used horizon-meta or the very least cloned grapevine and silo to the same
   directory

1. Copy assets:
    execute copy_hello-v8_assets.sh under silo

2. Install/build v8:

    You have two options, either install V8 system wide so that v8 headers are available under
    /usr/include or /usr/local/include and lib under standard LD_LIBRARY_PATH *OR* install have it
    available only locally in which v8-hello needs to be specified where its located at. Local
    installation is recommended in order to avoid conflicts with possible previous system wide
    v8 installations.

    Please note that the minimum support version is 3.25.30. Later versions will likely also work.

    On Mac OSX you can install V8 globally via brew by executing
        brew install v8

    Under Ubuntu you can try
        sudo apt-get install libv8-dev

        NB! libv8-dev is too old still in Ubuntu 14.04

    If no prebuilt V8 is available you can build it as follows:

        1. make sure subversion is installed and is in path
        2. git clone https://github.com/v8/v8
        3. make builddeps
        4. make native library=shared i18nsupport=off -j4

    These steps build the shared library under v8/out/native/ or v8/out/native/lib.target
    depending on the host OS.

    NB! If you choose not to install globally, then take note of the path where the libv8.so or
    libv8.dylib is found under v8/out.

    Optionally you may choose to manually copy the headers from v8/include and the shared library to
    relevant location on your system such that they are globally available.

3. Build Grapevine
    - Linux: refer to INSTALL document at the root of Grapevine repo
    - OSX: type make in grapevine/osx

4. Build v8-hello

    a. Globally installed V8:
        make -j4

    b. Locally installed V8:
        CFLAGS=-I<path to v8/include> V8_LIB_DIR=<path to directory containing libv8> make

5. Run v8-hello:

    ./v8-hello hello.js

