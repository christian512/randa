# RANDA

RANDA is an extension of [PANDA](http://comopt.ifi.uni-heidelberg.de/software/PANDA), which implements AdjaceNcy Decomposition Algorithm
for enumerating facets of convex polyhedra using parallel computing.

The extension adds the following features to the original functionality: 
* Enumeration of faces up to automorphism symmetries
  * Communication with GAP for group calculations
* Recursive calls of the Adjacency Decomposition Method (optional)
* A heuristic version of the Adjacency Decomposition Method (optional)


## Important Note
RANDA makes use of the computer algebra system GAP, which is used for computational group theory operations. 
A running instance of a specific script in GAP, which compares and stores facets, is needed in order to execute RANDA properly.
If no such script is running in GAP, the enumeration using RANDA will not progress.

## Install RANDA
It is recommended to install RANDA using [CMake](www.cmake.org). On Ubuntu, you can install CMake using the command:
```shell
sudo apt-get install build-essential cmake
```
If you are on another platform take a look at the [CMake website](www.cmake.org) to find installation instructions.


After the CMake installation proceed with the following steps:
1. create a *build*-folder in the randa directory
2. change directory into the *build*-folder
3. call *cmake ..* and then *make*
4. optionally run tests that the installation was successful using *ctest*
5. *make install* to make randa accessible from any directory

This is the complete list of commands for the above actions:

```shell
mkdir build
cd build
cmake ..
make
ctest
sudo make install
```

## Install GAP


GAP requires external library [GMP](www.libgmp.org), which needs to be installed beforehand. 
On Ubuntu you can install it using the following command. 
```shell
sudo apt-get install libgmp-dev
```

Next download GAP from their Download webpage and follow these steps to install GAP:

1. unpack the GAP archive and change into the new directory
2. configure the installation using *./configure*
3. make the installation

On Ubuntu you can do this by the following commands:
```shell
tar -xf gap-4.11.1.tar.gz cd gap-4.11.1
./configure
make
```
GAP can now be executed by running *./bin/gap.sh*.

In order to call GAP from any location, you have to add an *Alias* to your *.bashrc* file. 
Edit the file using *nano*: 
```shell
nano ~/.bashrc
```
Add the Alias to the end of the file.
```shell
alias gap="cd ~/gap-4.11.1/bin && ./gap.sh"
```
To finish setting up the Alias, source the *.bashrc* file.

```shell
source ~/.bashrc
```
Now you should be able to call *gap* from any directory.


## Setup Communication Files

RANDA communicates with GAP via named pipes. These pipes can be created using *mkfifo*-command and act like files on the file system. 
The communication needs a pipe for each direction, i.e. two pipes, which are located in the directory where RANDA and GAP are executed.
For running the Example code navigate to the *example*-directory and create the pipes there.

```shell
mkfifo fromgap.pipe
mkfifo togap.pipe
```

## Run Example Enumeration
After creating the pipes in the *example*-directory you can run RANDA for the example files given in the directory.

First start execute the GAP script containing symmetry information on the polytope.

```shell
cd example 
gap gap_example.g
```

Now you can start RANDA by calling the file, that describes the vertices of the polytope.

```shell
randa vertices_example.ext
```


## Options for Running RANDA


The following flags are available:
-r : Recursion Depth (standard 0)
-p : Probabilistic Version (standard 0 for not, give 1 for activating this method.)
-t : Number of threads (standard given by system)

