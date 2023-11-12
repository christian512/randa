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

RANDA makes use of the computer algebra system GAP, which is used for computational group theory operations. You need to have gap installed on your system. 

However, I strongly encourage you to use the provided **Docker image** to run the software. This will take care of all dependencies required for *RANDA*.

## Docker setup

In the repository we provide a `run_dev_image.sh` file, which will automatically pull the latest version of the Docker image for RANDA and start a container of it. Download the script to your computer, make it executable and run it.

Connect to the shell of the running docker container using

```docker exec -it randa_dev bash```

Change the directory into the *user data* folder:

```cd user_data_randa```

We will clone this repository here by running: 

```git clone https://github.com/christian512/randa.git```

Now, we change into the *randa*  directory and build the software: 

```
cd randa
mkdir build 
cd build
cmake ..
make 
make install
```

This will make the `randa` command available globally.

## Executing RANDA
To make the execution of `randa` relatively simple I provide a Python script `run_randa.py` in this directory. 

You can pass multiple arguments to the script, which are described below. To see a full list of arguments run: 

`python3 run_randa.py --help`

To verify your installation you can run `randa` on the provided example file by executing:

`python3 run_randa.py example/example.ext`

This should create a `randa.out` file which contains to facets describing the facet classes of the cut polytope `K_{1,2,2}`.

## Options
TODO: Describe

## Defining Input files 

TODO: Rewrite this section

To enumerate any other polyhedron, you have to give a description of its vertices/rays and its combinatorial symmetry group.

The vertices need to be provided in a format as shown in `example/example_vertices.ext`. This format is also described [here](http://comopt.ifi.uni-heidelberg.de/software/PANDA/format.html).
The symmetry group, for the particular polyhedron, needs to be given in the *GAP* program. You can use the `example_stabilizer_program.g` as a template. Note that for recursive calls, this program
calculates the stabilizer as symmetry group for intermediate polyhedra. The symmetry group is defined in line 4 of the file: 

```
GRP_RED := ();
```
You have to give the symmetry group in disjoint cycles for the indices of the vertices (as given in the vertices file). For example, if the exchange of the first vertex with the second vertex is a valid symmetry of
the polyhedron, this is denoted by `(1,2)`. It is best to provide here only a minimal generating set of the symmetry group, as the calculations will make use of it. 

