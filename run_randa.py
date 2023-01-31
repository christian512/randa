"""
A script to execute RANDA and start/kill the GAP program.
In the future I hope to implement this functionality into the C++ package itself,
but this is currently simpler to setup.
"""

import argparse
import subprocess
import os
import signal

# Hardcoded name of the gap program that is generated within RANDA
gap_file = './gap_prg.g'
# TODO: Remove GAP file in case it already exists



# set inputs that are equivalent to inputs of RANDA
parser = argparse.ArgumentParser()
parser.add_argument(dest='input_file', help='Input file for Randa containing vertices/inequalities')
parser.add_argument('-t', type=int, help="Number of threads to use")

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
randa_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
print('Started RANDA')


# TODO: Wait for GAP file to be written
while False:
    print('Waiting')

# Start GAP
cmd = "gap --quitonbreak {}".format(gap_file)
gap_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
time.sleep(5)
print('Started GAP program.')


# define behavior for receiving a SIGINT (e.g. pressing ctrl+c)
def handler(signum, frame):
    print("\n Ctrl-c was pressed. Stopping GAP and RANDA.")
    os.killpg(os.getpgid(randa_process.pid), signal.SIGTERM)
    os.killpg(os.getpid(gap_process.pid), signal.SIGTERM)
    exit(1)


# add handler for sigint
signal.signal(signal.SIGINT, handler)


# Wait for processes
randa_process.wait()
gap_process.wait()

# TODO: to remove file

