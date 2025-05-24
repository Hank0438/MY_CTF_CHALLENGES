from subprocess import Popen, PIPE
from base64 import b64encode

log = open("./output", "wb")

with open("./flag.png", "rb") as f:
    flag = f.read()
    flag = b64encode(flag)

with Popen(["./lazy7", flag], stdout=PIPE, stdin=PIPE, stderr=PIPE) as proc:
    output = proc.stdout.read()
    log.write(output)
