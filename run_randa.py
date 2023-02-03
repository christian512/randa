"""
A script to execute RANDA and start/kill the GAP program.
In the future I hope to implement this functionality into the C++ package itself,
but this is currently simpler to setup.
"""

import argparse
import subprocess
import os
import signal
import time

# Hardcoded name of the gap program that is generated within RANDA
cwd = os.getcwd()
gap_file = cwd + '/gap_prg.g'
togap_pipe = cwd + '/togap.pipe'
fromgap_pipe = cwd + '/fromgap.pipe'

# Remove GAP file in case it already exists
if os.path.exists(gap_file):
    os.remove(gap_file)

if os.path.exists(togap_pipe):
    os.remove(togap_pipe)
if os.path.exists(fromgap_pipe):
    os.remove(fromgap_pipe)

os.mkfifo(togap_pipe)
os.mkfifo(fromgap_pipe)

# set inputs that are equivalent to inputs of RANDA
parser = argparse.ArgumentParser()
parser.add_argument(dest='input_file', help='Input file for Randa containing vertices/inequalities')
parser.add_argument('-t', type=int, help="Number of threads to use")
parser.add_argument('-r', type=int, help="Number of recursive calls")

args = parser.parse_args()
# temporarily print arguments
args = args.__dict__

# Extract input file
input_file = args['input_file']
args.pop('input_file')

# Define command for Randa
# TODO: Change this if you "make install" randa
cmd = './randa {}'.format(input_file)

# Add arguments to the randa command
for arg, val in args.items():
    # Continue if Argument is empty
    if not val:
        continue

    cmd += ' -{} {}'.format(arg, val)

# Execute RANDA
print(cmd)
randa_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
print('Started RANDA')
time.sleep(3)

# Wait for GAP file to be created
while not os.path.exists(gap_file):
    print('Waiting for GAP file')
    time.sleep(0.5)

# Start GAP
cmd = "gap.sh --quitonbreak {}".format(gap_file)
gap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)


# define behavior for receiving a SIGINT (e.g. pressing ctrl+c)
def stop_handler(signum, frame):
    print("\n  Stopping GAP and RANDA.")
    os.killpg(randa_process.pid, signal.SIGTERM)
    os.killpg(gap_process.pid, signal.SIGTERM)
    os.remove(fromgap_pipe)
    os.remove(togap_pipe)
    exit(1)

# add handler for sigint
signal.signal(signal.SIGINT, stop_handler)

# Wait for RANDA process to finish
randa_process.wait()
# kill GAP process
gap_process.kill()

# remove FIFO files
os.remove(fromgap_pipe)
os.remove(togap_pipe)
