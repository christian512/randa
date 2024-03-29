###################################################################################
## Author: Stefan Lörwald, Universität Heidelberg                                ##
## License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode ##
###################################################################################

PANDA -- Parallel AdjaceNcy Decomposition Algorithm

Detailed information available at http://comopt.ifi.uni-heidelberg.de/software/PANDA

I. PREREQUISITES

1. Compiler

a) known compatibility

PANDA was tested on Linux and Mac OS X systems using the following compilers:
g++-4.7
g++-4.8
g++-4.9
g++-5
clang++-3.4
clang++-3.5
clang++-3.6

b) known incompatibility

PANDA cannot be compiled with the following compilers:
g++ up to 4.6
clang++ up to 3.3

2. Build system

Either gnumake or CMake is required to compile PANDA with the help of Makefiles.
Building manually, all files in the src directory have to be compiled and then linked.
Threading capability is required (i.e. "-pthread" on Linux systems).
Building manually with support for MPI requires the definition of the macro "MPI_SUPPORT".

3. MPI

MPI is optional. Even if installed, it can be deactivated by the user.
See below for instructions.



II. INSTALLATION

 1. using CMake
   a) create a folder called "build" and enter it (linux: mkdir build; cd build)
   b) call "cmake .."
   c) run "make"
   d) run "ctest" (optional)
   e) run "make install" (optional. to select the installation path, please refer to the cmake help)

   If you do not have ctest installed, PANDA will probably still work fine. If you want to execute the tests without ctest, simply execute every binary in the build directory (exluding PANDA itself).

   You will find an executable called "panda" in the build directory (or in the installation directory if you called "make install").

 2. without CMake (not recommended)
   a) run "./configure"
   b) run "make"



III. USAGE

 For information on how to use panda, see our website http://comopt.ifi.uni-heidelberg.de/software/PANDA
 Calling panda --help provides information on how to use command line parameters.
 Calling panda --version provides version information of your installation.



IV. TROUBLESHOOTING

 1. Debug build
  a) CMake: run "cmake -DCMAKE_BUILD_TYPE=Debug .." and re-compile.
  b) Manual build: edit "Makefile_configuration.mk" and re-compile.

 2. Further information
  If you experience any trouble, visit our website
  http://comopt.ifi.uni-heidelberg.de/software/PANDA/index.html

 3. Bug reports
  If you have a bug report, you may contact the developer (stefan.loerwald+panda@gmail.com).
  Please include the version information and a detailed description of the error (include data files if necessary).

Note that in general, we do not give any support for PANDA.
Usage is at own risk.



V. PUBLICATIONS

For details on the used algorithms and techniques, please refer to our publication "PANDA: a software for polyhedral transformation" by S. Lörwald and G. Reinelt. (to appear in EURO Journal on Computational Optimization, 2015).
For referencing PANDA, please use the paper as a citation rather than the official website.

