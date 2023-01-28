
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "gap.h"
#include "symmetries.h"
#include <sys/stat.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <pthread.h>

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

bool panda::Gap::initialize(Symmetries symmetries){
    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // creating pipes
    mkfifo(fifo_to_gap, 0666);
    mkfifo(fifo_from_gap, 0666);
    //  Write GAP program with symmetries
    Gap::write_gap_prg(symmetries);

    // wait until gap is initialized
    // std::thread t(&panda::Gap::execute, this);
    // thread_handle = t.native_handle();
    // t.detach();
    execute();
    // TODO: Use the following example to kill the thread
    // https://www.bo-yang.net/2017/11/19/cpp-kill-detached-thread
    std::chrono::seconds dura(10);
    std::this_thread::sleep_for(dura);

    // set status to running
    running = true;
    return running;
}

bool panda::Gap::stop() const {
    if (!running){
        return true;
    }
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

std::vector<int> panda::Gap::equivalence() const {
    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // temporary return
    std::vector<int> result;
    return result;
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
    gap_prg.append("# setup files\noutfile := IO_File(Concatenation(IO_getcwd(), \"/");
    gap_prg.append(fifo_from_gap);
    gap_prg.append("\"), \"w\");\ninfile := IO_File(Concatenation(IO_getcwd(), \"/");
    gap_prg.append(fifo_to_gap);
    gap_prg.append("\"), \"r\");\n");

    // Write the actual logic of the GAP file
    gap_prg.append("# List of all facets\n"
                   "karr := [];\n"
                   "\n"
                   "\n"
                   "str := IO_ReadLine(infile);\n"
                   "while str <> \"break\" do\n"
                   "        # read command from input\n"
                   "        if str <> \"\" then\n"
                   "            #Print(\"GAP READ: \", str);\n"
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