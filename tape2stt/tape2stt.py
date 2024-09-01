#!/usr/bin/python3

import os
import sys
import subprocess
import asyncio

tasks = []
sem = asyncio.Semaphore(12)
data = [[0]*5 for i in range(5)]


async def mainCompile(tasks):
    return await asyncio.gather(*tasks)


async def myrun(command, slot, subslot):
	async with sem:
		proc = await asyncio.create_subprocess_exec(
            *command,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE)

		stdout, stderr = await proc.communicate()
		
		data[slot][subslot] = stderr


scriptDir = os.path.dirname(os.path.realpath(__file__))
openMSXDir = scriptDir + "/openmsx"

os.environ["OPENMSX_HOME"] = openMSXDir
os.environ["OPENMSX_USER_DATA"] = openMSXDir
os.environ["OPENMSX_SYSTEM_DATA"] = openMSXDir

openMSX = openMSXDir + "/openmsx"
compress = scriptDir + "/compress/compress"

setting = openMSXDir + "/empty.xml"
script = scriptDir + "/tape2stt.tcl"


for slot in range(1, 4):
    for subslot in range(0, 4):
        arguments = [openMSX, "-machine", str(slot) + str(subslot), "-cassetteplayer", sys.argv[1], "-script", script, "-setting", setting]
        tasks.append(asyncio.ensure_future(myrun(arguments, slot, subslot)))

loop = asyncio.get_event_loop()

try:
	loop.run_until_complete(mainCompile(tasks))
finally:
	loop.run_until_complete(loop.shutdown_asyncgens())
	loop.close()


ba = bytearray()
ba += data[1][0]

for slot in range(1, 4):
    for subslot in range(0, 4):

        if (len(data[slot][subslot]) != 81985):
            if (len(data[slot][subslot]) == 0):
                exit(0)
            else:
                print("Conversion error in slot " + str(slot) + "-" + str(subslot))
                print(data[slot][subslot])
                exit(1)

        previousDifferent = False
        diff = []

        for i in range(0, len(data[slot][subslot])):

            newDifferent = data[slot][subslot][i] != data[1][0][i]

            if (newDifferent):
                diff.append(data[slot][subslot][i])

            if (previousDifferent and not newDifferent):
                ba += (len(diff)).to_bytes(3, byteorder='little')
                ba += (i-len(diff)).to_bytes(3, byteorder='little')
                ba += bytes(diff)
                diff = []
                
            previousDifferent = newDifferent

        ba += (0).to_bytes(3, byteorder='little')

res = subprocess.run(compress, input=ba, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

if (res.returncode != 0):
    print("Compression error")
    exit(res.returncode)

file = open(sys.argv[2], mode='wb')
file.write(res.stdout)
file.close()

exit(0)

