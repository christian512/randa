
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "gap.h"
#include <sys/stat.h>
#include <chrono>
#include <thread>
#include <iostream>

using namespace panda;

bool panda::Gap::initialize(){
    // TODO Only initialize when there are more than zero symmetries
    // lock the gap mutex
    std::lock_guard<std::mutex> lock(mutex);
    // creating pipes
    mkfifo(fifo_to_gap, 0666);
    mkfifo(fifo_from_gap, 0666);
    // TODO: Write GAP program using symmetries

    // starting a gap process
    // TODO add program file as argument to GAP
    std::string cmd = "gap.sh -q &";
    const char *c = cmd.c_str();
    std::system(c);
    // wait until gap is initialized

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
    std::string cmd = "pkill -15 gap.sh";
    const char * c = cmd.c_str();
    std::system(c);
    // Remove FIFO files.
    cmd = "rm ";
    cmd.append(fifo_from_gap);
    cmd.append(" ");
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
