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
parser.add_argument('-r', type=int, help="Number of maximum recursive calls")
parser.add_argument('-d', type=int, help="Minimum number of vertices required for recursive calls")
parser.add_argument('-o', type=str, help='Path to output file', default='randa.out')
parser.add_argument('--sampling', action='store_true')

args = parser.parse_args()
# temporarily print arguments
args = args.__dict__


# Extract input/output file
input_file = args['input_file']
args.pop('input_file')
output_file = args['o']
args.pop('o')

# Extract probabilistic flag
sampling_flag = args['sampling']
args.pop('sampling')

# Define command for Randa
cmd = 'randa {}'.format(input_file)

# Add arguments to the randa command
for arg, val in args.items():
    # Continue if Argument is empty
    if not val:
        continue
    cmd += ' -{} {}'.format(arg, val)

if sampling_flag:
    cmd += ' --probabilistic'
# Add output file
cmd += ' > ' + output_file
# Execute RANDA
randa_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(3)

# Wait for GAP file to be created
while not os.path.exists(gap_file):
    print('Waiting for GAP file')
    time.sleep(0.5)

# Start GAP
cmd = "gap.sh --quitonbreak {}".format(gap_file)
gap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)


# define behavior for receiving a SIGINT (e.g. pressing ctrl+c)
def stop_handler(signum, frame):
    print("\n  Stopping GAP and RANDA.")
    os.killpg(os.getpgid(randa_process.pid), signal.SIGTERM)
    os.killpg(os.getpgid(gap_process.pid), signal.SIGTERM)
    os.remove(fromgap_pipe)
    os.remove(togap_pipe)
    os.remove(gap_file)
    exit(1)

# add handler for sigint
signal.signal(signal.SIGINT, stop_handler)

# Wait for RANDA process to finish and kill gap process
randa_process.wait()
os.killpg(os.getpgid(gap_process.pid), signal.SIGTERM)

# remove FIFO files and GAP program
os.remove(fromgap_pipe)
os.remove(togap_pipe)
os.remove(gap_file)