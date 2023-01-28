
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_GAP
#include "gap.h"
#undef COMPILE_TEMPLATE_GAP

#include "symmetries.h"
#include "algorithm_rotation.h"
#include <sys/stat.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace panda;

void panda::Gap::execute(){
    // executes the GAP command
    // starting a gap process
    std::string cmd = "gap.sh -q ";
    cmd.append(gap_prg_file);
    cmd.append(" &");
    const char *c = cmd.c_str();
    std::system(c);
}
template <typename Integer>
bool panda::Gap::initialize(Symmetries symmetries, const Vertices<Integer>& vertices){
    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // creating pipes
    mkfifo(fifo_to_gap, 0666);
    mkfifo(fifo_from_gap, 0666);
    //  Write GAP program with symmetries
    Gap::write_gap_prg(symmetries);
    // Execute GAP and wait for startup
    execute();
    std::chrono::seconds dura(10);
    std::this_thread::sleep_for(dura);

    // Generate the lookup table for the vertex
    fill_lookup_table(vertices);

    // set status to running
    running = true;
    return running;
}

bool panda::Gap::stop() const {
    if (!running){
        return true;
    }
    std::cout << "Shutting down GAP" << std::endl;
    std::chrono::seconds dura(2);
    std::this_thread::sleep_for(dura);
    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // Kill the gap command
    std::string cmd = "pkill -15 gap";
    const char * c = cmd.c_str();
    std::system(c);
    // Remove FIFO files.
    cmd = "rm ";
    cmd.append(fifo_from_gap);
    c = cmd.c_str();
    std::system(c);
    cmd = "rm ";
    cmd.append(fifo_to_gap);
    c = cmd.c_str();
    std::system(c);
    return true;
}

template <typename Integer>
std::vector<int> panda::Gap::equivalence(const Facets<Integer>& facets, const Vertices<Integer>& vertices) {
    // TODO: Implement
    // string to send to GAP
    std::string to_gap = "[";
    // iterate through facets
    for (const auto& facet: facets){
        // calculate vertex on facet
        const auto vertices_on_facet = algorithm::verticesWithZeroDistance(vertices, facet);
        to_gap.append("[");
        for (auto v : vertices_on_facet){
            int x = get_index(v);
            to_gap.append(std::to_string(x));
            to_gap.append(",");
        }
        to_gap.pop_back();
        to_gap.append("],");
    }
    to_gap.pop_back();
    to_gap.append("]");

    // temporary print out to_gap string
    std::cout << "String to GAP: " << to_gap << std::endl;

    // connect to GAP using the pipes
    std::ifstream in(fifo_from_gap);
    std::ofstream out(fifo_to_gap);
    std::string line;

    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Send string to GAP and read response
    // write string to out
    out << to_gap << std::endl;
    // get incoming line
    std::getline(in, line);

    std::cout << "Got line: " << line << std::endl;

    // temporary return
    std::vector<int> result;
    return result;
}

template <typename Integer>
std::string panda::Gap::vertex_to_string(const Vertex<Integer>& vertex) {
    std::string s;
    for (const auto v: vertex){
        s.append(std::to_string(v));
    }
    return s;
}

template <typename Integer>
bool panda::Gap::fill_lookup_table(const Vertices<Integer>& vertices) {
    // Iterate through vertices
    for ( int i = 0; i < vertices.size(); i++ ){
        const auto v = vertices[i];
        // Convert vertex to string
        std::string s = vertex_to_string(v);
        // add string to lookup table together with integer
        vertex_lookup[s] = i + 1;
    }
    return true;
}

template <typename Integer>
int panda::Gap::get_index(const Vertex<Integer>& vertex) {
    // Convert vertex to string and return lookup value
    std::string s = vertex_to_string(vertex);
    return vertex_lookup[s];
}

bool panda::Gap::write_gap_prg(Symmetries symmetries) {
    // Define GAP program
    std::string gap_prg;
    gap_prg.append("LoadPackage(\"json\");\n"
                   "#Define the Symmetry Group\n"
                   "GRP := Group(");

    for ( auto s : symmetries){
        gap_prg.append(s);
        gap_prg.append(",");
    }
    // remove last ,
    gap_prg.pop_back();
    // add closure for group
    gap_prg.append(");\n");

    // Add files to GAP
    gap_prg.append("# setup files\noutfile := IO_File(\"");
    gap_prg.append(fifo_from_gap);
    gap_prg.append("\", \"w\");\ninfile := IO_File(\"");
    gap_prg.append(fifo_to_gap);
    gap_prg.append("\", \"r\");\n");

    // Write the actual logic of the GAP file
    gap_prg.append("# List of all facets\n"
                   "# Print(\"STARTED GAP\")\n;"
                   "karr := [];\n"
                   "\n"
                   "\n"
                   "str := IO_ReadLine(infile);\n"
                   "while str <> \"break\" do\n"
                   "        # read command from input\n"
                   "        if str <> \"\" then\n"
                   "            # Print(\"GAP READ: \", str);\n"
                   "            # Convert to GAP Object\n"
                   "            tarr := JsonStringToGap(str);\n"
                   "\n"
                   "            # Response to return\n"
                   "            response := [];\n"
                   "            # Iterate through all arrays to test\n"
                   "            for i in [1..Length(tarr)] do\n"
                   "                tpoly := tarr[i];\n"
                   "                equiv := 0;\n"
                   "                for kpoly in karr do\n"
                   "                    res := RepresentativeAction(GRP, tpoly, kpoly, OnSets);\n"
                   "                    if res <> fail then\n"
                   "                        equiv := 1;\n"
                   "                        break;\n"
                   "                    fi;\n"
                   "                od;\n"
                   "                if equiv = 0 then\n"
                   "                    Add(karr, tpoly);\n"
                   "                    Add(response, i-1);\n"
                   "                fi;\n"
                   "            od;\n"
                   "            \n"
                   "            # Respond\n"
                   "            if Length(response) = 0 then\n"
                   "                IO_WriteLine(outfile, \"false\");\n"
                   "            else\n"
                   "                 Print(\"GAP FOUND: \", response);\n"
                   "                IO_WriteLine(outfile, response);\n"
                   "            fi;\n"
                   "        fi;\n"
                   "        str := IO_ReadLine(infile); \n"
                   "od;");


    // write gap program to file
    std::fstream file;
    file.open(gap_prg_file, std::ios::out);
    file << gap_prg;
    file.close();



    return true;
}