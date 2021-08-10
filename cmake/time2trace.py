#!/usr/bin/env python3
""" used to parse the output of compiling a program, or other tasks appended by /usr/bin/time -v"""
import argparse 
import logging
import datetime

# add the following lines to cmake to produce trace data 
#set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "/usr/bin/time -v")
#set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "/usr/bin/time -v")

logger= logging.getLogger("time2trace")
goodParts = {
        "cpuTime": "User time (seconds)",
        "memory(kB)": "Maximum resident set size (kbytes)",
        "pageFaults": "Minor (reclaiming a frame) page faults",
        "inputFileIO": "File system inputs",
        "cmd": "Command being timed",
        }
sections = dict()


def parseLine(line):
    tokens = line.split(": ")
    tokens[0] = tokens[0].strip()
    if goodParts['cmd'] == tokens[0]:
        logger.info("new item:")
        try:
            items = [f'{sections[name].split()[-1]}' for label, name in goodParts.items()]
            print (', '.join(items))
        except KeyError as e:
            logger.error(f'exception: missing field {e}')
        sections.clear()
    elif 'Command' in tokens[0]:
        logger.error(f'not new "{tokens[0]}"')
    if tokens[0] == goodParts['cpuTime']:
        # parse time as mm:ss asuming no hours
        try:
           t = sum(float(x)*60**i for i,x in enumerate(tokens[1].split(":")))
        except ValueError:
            logger.error(f"expected time, got {tokens[1]}")
        #t = datetime.timedelta(minutes=int(m), seconds=float(s)).total_seconds()
        token = f"{t}s"
    if len(tokens)>2:
        # put last element in second position.
        # this may be hepfule if the command has colons in it.
        tokens[1] = tokens[-1]
    if len(tokens) >= 2:
        sections[tokens[0]] = tokens[1]
    else:
        #logger.error("unparsed line")
        pass


def main():
    """ start parsing time results"""
    print(', '.join(goodParts.keys()))
    arg = argparse.ArgumentParser()
    #arg.add_help("parse the output of time -vv, and export  as atrace")
    arg.add_argument("--input", default="arm-compile-log.txt")
    args = arg.parse_args()
    inputFileName = args.input
    with open(inputFileName) as inputFile:
        for line in inputFile:
            parseLine(line)


if __name__ == "__main__":
    main()
