import subprocess
import sys

def shellcmd(command, echo=True):
    if echo: print '[cmd]', command

    if not isinstance(command, str):
        command = ' '.join(command)

    stdout_result = subprocess.check_output(command, shell=True)
    if echo: sys.stdout.write(stdout_result)
    return stdout_result
