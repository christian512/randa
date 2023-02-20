# TODO: Read inputs from arguments
# TODO: Polish this file.
graph = graphs.CompleteMultipartiteGraph([1,3,5])
vertices = graph.vertices()
gens = graph.automorphism_group().gens()
print("Number of Graph Vertices: ", len(vertices))

# Define subgroups
vertices_set = sage.sets.set.Set_object(vertices)
subsets_list = []
for subset in vertices_set.subsets():
    diff = sorted(list(vertices_set.difference(subset)))
    if diff in subsets_list:
        continue
    subsets_list.append(sorted(list(subset)))

# Check that the number of subgroups is correct
print("Number of Polytope Vertices: ", len(subsets_list))
assert len(subsets_list) == 2**(len(vertices)-1)


# Open a file to write the output to
f = open('cut.ext', 'w+')


# Generate polytope vertices
f.write('Vertices: \n')
def metric(x,y, subset):
    """ Metric to define the cut polytope """
    if not (x,y,None) in graph.edges():
        return 0
    intersection = list(set(subset) & set([x,y]))
    if len(intersection) != 1:
        return 0
    return 1
poly_vertices = []
for subset in subsets_list:
    # create vertex
    vertex = [metric(x,y,subset) for x,y,_ in graph.edges()]
    poly_vertices.append(vertex)
    # write vertex to file
    s = ' '.join(str(x) for x in vertex)
    f.write(s + '\n')


# Generate symmetries from the graph symmetry group
f.write('Symmetries: \n')
list_of_all_permutations = []
# Iterate trough generators
for gen in gens:
    # Define permutation of vertices by generator
    perm_vertices = gen(vertices)
    # define an empty permutation on the subsets
    perm_subsets_list = []
    # iterate through subsets
    for i, subset in enumerate(subsets_list):
        perm_subset = sorted([perm_vertices[i] for i in subset])
        # Find index of subset in subsets_list
        new_index = -1
        if perm_subset in subsets_list:
            new_index = subsets_list.index(perm_subset)
        else:
            # Take complementary and find that index
            comp_perm_subset = sorted(list(vertices_set.difference(Set(perm_subset))))
            new_index = subsets_list.index(comp_perm_subset)
        if new_index == -1:
            print('no index')
        # We add here + 1 to the index because Permutation can only run on [1,....,n] and not start at 0
        perm_subsets_list.append(new_index + 1)
    # Convert the permutation on subsets to disjoint cycle notation
    perm = Permutation(perm_subsets_list)
    cycles = perm.to_cycles(singletons=False)
    # Convert cycles to string
    cycles_string = ''
    for c in cycles:
        cycles_string += str(c).replace(' ', '')
    list_of_all_permutations.append(cycles_string)
    cycles_string += '\n'
    # write to file
    # f.write(cycles_string)


# Define symmetries by subgroups to get restricted symmetry group
group = []
# iterate through subsets
for i, vertex_subset in enumerate(subsets_list):
    # set that belongs to the vertex
    vertex_subset = Set(vertex_subset)
    perm = []
    for j, permutation_subset in enumerate(subsets_list):
        # set that is used to generate the permutation
        permutation_subset = Set(permutation_subset)
        # calculate symmetric difference of the two sets (and sort)
        symm_diff = permutation_subset.symmetric_difference(vertex_subset)
        symm_diff = sorted(list(symm_diff))
        # find the vertex that is defined by this new set
        if not symm_diff in subsets_list:
            # get the complement of the symmetric difference
            symm_diff = sorted(list(vertices_set.difference(Set(symm_diff))))
        vertex_idx = subsets_list.index(list(symm_diff))
        perm.append(vertex_idx + 1)
    perm = Permutation(perm)
    cycles = perm.to_cycles(singletons=False)
    # Convert cycles to string
    cycles_string = ''
    for c in cycles:
        cycles_string += str(c).replace(' ', '')
    if len(cycles_string) > 0:
        list_of_all_permutations.append(cycles_string)
        cycles_string += '\n'
        # write to file
        # f.write(cycles_string)

# Reduce to generators using GAP
all_perms = ','.join(list_of_all_permutations)
gap.eval("GRP := Group(" + all_perms + ")")
gens = gap.Group("SmallGeneratingSet(GRP)")
gens = str(gens)[7:-1]
gens = gens.replace(' ', '').replace('[','').replace(']','').replace('\n', '')
gens = gens.replace('),(',')\n(')

# Write Generators to file
f.write(gens)

# close file
f.close()




