"""
Generates a bipartite Bell Polytope for given number of inputs and outputs.
"""
from itertools import product
import numpy as np


def get_deterministic_behaviors_two_party(inputs_a, inputs_b, outputs_a, outputs_b):
    """ Returns all deterministic behaviors corresponding to inputs/outputs for two parties"""
    dim = len(inputs_a) * len(inputs_b) * len(outputs_a) * len(outputs_b)
    # hidden variables
    lhvs_a = product(outputs_a, repeat=len(inputs_a))
    lhvs_b = product(outputs_b, repeat=len(inputs_b))
    deterministics = []
    for lhv_a, lhv_b in product(lhvs_a, lhvs_b):
            # initialize empty behavior
            counter = 0
            d = np.zeros(dim)
            # iterate over possible outcomes and check whether its defined by the LHV
            for a, b in product(range(len(outputs_a)), range(len(outputs_b))):
                for x, y in product(range(len(inputs_a)), range(len(inputs_b))):
                    if lhv_a[x] == a and lhv_b[y] == b:
                        d[counter] = 1.0
                    counter += 1
            deterministics.append(d)
    # check length
    assert len(deterministics) == (len(outputs_a) ** len(inputs_a)) * (len(outputs_b) ** len(inputs_b)), deterministics
    return np.array(deterministics)

def get_relabelling_generators(inputs_a, inputs_b, outputs_a, outputs_b):
    """ Returns a set of relabelling generators for the symmetry group """
    generators = []
    # setup configurations
    configurations = [(a, b, x, y) for a, b, x, y in product(outputs_a, outputs_b, inputs_a, inputs_b)]
    # set the first inputs
    first_x, first_y = inputs_a[0], inputs_b[0]
    # set first outputs
    first_a, first_b = outputs_a[0], outputs_b[0]
    # relabel each input x to the first input
    for curr_x in inputs_a:
        if curr_x == first_x:
            continue
        perm = list(range(len(configurations)))
        for a, b, x, y in configurations:
            if x == curr_x:
                # configuration of current setting
                idx_old = configurations.index((a, b, x, y))
                # same configuration where x is first_x
                idx_new = configurations.index((a, b, first_x, y))
                perm[idx_old] = idx_new
                perm[idx_new] = idx_old
        generators.append(perm)
    # relabel each input y to the first input
    for curr_y in inputs_b:
        if curr_y == first_y:
            continue
        perm = list(range(len(configurations)))
        for a, b, x, y in configurations:
            if y == curr_y:
                # configuration of current setting
                idx_old = configurations.index((a, b, x, y))
                # configuration where y is first_y
                idx_new = configurations.index((a, b, x, first_y))
                perm[idx_old] = idx_new
                perm[idx_new] = idx_old
        generators.append(perm)
    # relabel the first output of first_x with every other output
    for curr_a in outputs_a:
        if curr_a == first_a:
            continue
        perm = list(range(len(configurations)))
        for a, b, x, y in configurations:
            if x == first_x and a == curr_a:
                # current index
                idx_old = configurations.index((a, b, x, y))
                # idx where a is replaced with the first index
                idx_new = configurations.index((first_a, b, x, y))
                # change perm
                perm[idx_old] = idx_new
                perm[idx_new] = idx_old
        generators.append(perm)
    # relabel the first output of first_y with every other output
    for curr_b in outputs_b:
        if curr_b == first_b:
            continue
        perm = list(range(len(configurations)))
        for a, b, x, y in configurations:
            if y == first_y and b == curr_b:
                # current index
                idx_old = configurations.index((a, b, x, y))
                # idx where b is replaced with the first output
                idx_new = configurations.index((a, first_b, x, y))
                # change perm
                perm[idx_old] = idx_new
                perm[idx_new] = idx_old
        generators.append(perm)
    # exchange of parties if number of inputs and outputs is equal on both sides
    if len(inputs_a) == len(inputs_b) and len(outputs_a) == len(outputs_b):
        perm = list(range(len(configurations)))
        for a, b, x, y in configurations:
            idx_old = configurations.index((a, b, x, y))
            idx_new = configurations.index((b, a, y, x))
            perm[idx_old] = idx_new
            perm[idx_new] = idx_old
        generators.append(perm)
    return np.array(generators, dtype=int)

def get_relabels_dets(dets, allowed_perms, show_progress=0):
    """ Gets a list of which deterministic transforms to which under each relabelling """
    # list for relabels
    relabels_dets = []
    # iterate through possible relabels
    for j, perm in enumerate(allowed_perms):
        if show_progress:
            print('perm: {} / {}'.format(j, len(allowed_perms)))
        tmp_relabel_d = np.zeros(dets.shape[0])
        for i in range(dets.shape[0]):
            d = dets[i]
            idx = np.argmin(np.sum(np.abs(dets - d[perm]), axis=1))
            tmp_relabel_d[i] = idx
        relabels_dets.append(tmp_relabel_d)
    return np.array(relabels_dets, dtype=int)


def relabels_dets_to_disjoint_cycles(relabels_dets, show_progress = 0):
    """ Converts the relabellings on deterministic points to disjoint cycles """
    all_cycles = []

    for i, relabel in enumerate(relabels_dets):
        if show_progress:
            print('Get cycle {} / {}'.format(i, len(relabels_dets)))
        # convert to dict
        d = {}
        for i in range(relabel.shape[0]):
            d[i] = relabel[i]
        # generate cycles
        cycles = []
        while len(d) > 0:
            cycle = []
            key = list(d)[0]
            val = d.pop(key)
            if val != key:
                cycle.append(key)
                cycle.append(val)
            while val in list(d):
                key = val
                val = d.pop(key)
                if val not in cycle:
                    cycle.append(val)
            if cycle:
                cycles.append(cycle)
        if cycles:
            all_cycles.append(cycles)

    # Store the cycles
    out = ""
    for cycles in all_cycles:
        for cycle in cycles:
            out = out + "("
            for val in cycle:
                out += str(val + 1) + ","
            out = out[:-1] + ")"
        out = out + "\n"
    out = out[:-1]
    return out

def write_vertices_and_symmetries_to_file(vertices, symmetries: str, filename: str = 'bell.ext'):
    """ Writes Vertices and Symmetries to the file """
    f = open(filename, 'w+', encoding='utf-8')
    f.write('Vertices: \n')
    for vertex in vertices:
        s = ' '.join(str(int(x)) for x in vertex)
        f.write(s + '\n')
    
    f.write('Symmetries: \n')
    f.write(symmetries)
    f.close()

if __name__ == '__main__':
    inputs_a = list(range(3))
    inputs_b = list(range(3))
    outputs_a = list(range(4))
    outputs_b = list(range(2))
    filename = '3342.ext'
    vertices = get_deterministic_behaviors_two_party(inputs_a, inputs_b, outputs_a, outputs_b)
    vertices = vertices[np.lexsort(np.rot90(vertices))]
    relabeling_generators = get_relabelling_generators(inputs_a, inputs_b, outputs_a, outputs_b)
    automorphisms_on_vertices = get_relabels_dets(vertices, relabeling_generators)
    automorphisms_as_disjoint_cycles = relabels_dets_to_disjoint_cycles(automorphisms_on_vertices)

    print(vertices)
    print(automorphisms_as_disjoint_cycles)

    write_vertices_and_symmetries_to_file(vertices, automorphisms_as_disjoint_cycles, filename)