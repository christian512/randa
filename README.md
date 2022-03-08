# RANDA

RANDA is an extension of [PANDA](http://comopt.ifi.uni-heidelberg.de/software/PANDA), which implements AdjaceNcy Decomposition Algorithm
for enumerating facets of convex polyhedra using parallel computing. This README describes the installation process of RANDA
and all tools needed to execute it. The instructions will be focussed on Ubuntu as operating system, but it is also possible
to set things up on other systems.

The extension adds the following features to the original functionality: 
* Enumeration of faces up to automorphism symmetries
  * Communication with GAP for group calculations
* Recursive calls of the Adjacency Decomposition Method (optional)
* A heuristic version of the Adjacency Decomposition Method (optional)

PANDA was written by Stefan Lörwald, Universität Heidelberg, and released under the CC BY-NC 4.0 license.
Accordingly, RANDA is released under CC BY-NC 4.0: http://creativecommons.org/licenses/by-nc/4.0/legalcode

## Important Note
RANDA makes use of the computer algebra system GAP, which is used for computational group theory operations. 
A running instance of a specific script in GAP, which compares and stores facets, is needed in order to execute RANDA properly.
If no such script is running in GAP, the enumeration using RANDA will not progress. Furthermore (as the name suggests),
the changes applied here to PANDA remove the parallel functionality. Thus RANDA can only run on a single-core.

## Docker Installation
For a simple installation process I provide a [Dockerfile](Dockerfile) that allows a quick installation using the Docker engine.
A guide for installing Docker engine on your computer can be found [here](https://docs.docker.com/engine/install/).
This guide assumes that you use Ubuntu, but the instructions should work similarly on other operating systems.

To install all software required to run the codes in this repository, you need to clone this repository and build a docker image from the Dockerfile:

```bash
git clone https://github.com/christian512/randa.git
cd randa
sudo docker build -t randa_environment .
```
The installation process might take a while. You can now spawn a container from the generated image, which allows you to run any code from this repository. Start the container by:

```bash
sudo docker run -it randa_environment
```

From this shell you can run both **RANDA** and **GAP** as described in the example section.

## Example
In the `/example/` directory I provide example input files for running RANDA and GAP. To run this 
examples change the directory: 

```bash
cd example
```

For the communication between RANDA and GAP you need to create a communication pipe between 
the two programs. This is done by the `mkfifo` command: 

```bash
mkfifo fromgap.pipe
mkfifo togap.pipe
```

You can start the **GAP** program in the background by running:

```bash
nohup gap --quitonbreak example_stabilizer_program.g
```
Now you can start RANDA which communicates to GAP and enumerates
all facet-classes of the polytope given by the vertices in `example_vertices.ext`.
To run RANDA execute:

```bash
randa example_vertices.ext
```

You can give any options to RANDA such as:
-r : Recursion level to use (integer)
-p : Flag for enabling the sampling method (1 for activating sampling method, 0 for not)

## Manual Installation
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

This is the complete list of commands to run in Ubuntu for the above actions:

```shell
mkdir build
cd build
cmake ..
make
ctest
sudo make install
```

### Install GAP
A detailed instruction on installing GAP can be found [here](https://github.com/gap-system/gap/blob/v4.11.1/INSTALL.md).

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

For executing the scripts to communicate with RANDA, you have to install the packages for GAP.
```shell
cd gap-4.11.1/pkg
../bin/BuildPackages.sh
```

In order to call GAP from any location, you have to add it to the *PATH* variable. 
Edit your *bashrc*-file using *nano*: 
```shell
nano ~/.bashrc
```
Add the following lines to the end of the file.
```shell
PATH=~/gap-4.11.1/bin:$PATH
alias gap="gap.sh"
```
To finish the setup, source the *.bashrc* file.

```shell
source ~/.bashrc
```
Now you should be able to call *gap* from any directory.