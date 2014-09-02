import os
import subprocess
import sys

def shellcmd(command, echo=True):
    if echo: print '[cmd]', command

    if not isinstance(command, str):
        command = ' '.join(command)

    stdout_result = subprocess.check_output(command, shell=True)
    if echo: sys.stdout.write(stdout_result)
    return stdout_result

def FindBaseNaCl():
    """Find the base native_client/ directory."""
    nacl = 'native_client'
    path_list = os.getcwd().split(os.sep)
    if nacl not in path_list:
        return None
    last_index = len(path_list) - path_list[::-1].index(nacl)
    return os.sep.join(path_list[:last_index])
