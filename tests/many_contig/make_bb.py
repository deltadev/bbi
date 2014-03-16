#!/usr/bin/python

import env

import os, sys, argparse
import subprocess as sp
from os import path

parser = argparse.ArgumentParser(
    description=("Generates a BigBed file from data and sizes files\n"
                 "in the example_data directory.\n"),
    formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument("--do-it", help="actually do it.", action='store_true')
args = parser.parse_args()

cmd = "bedToBigBed -type=bed3 -tab {data} {sizes} {data}.bb"

call = cmd.format(data=path.join(env.data, 'many_contig_data'),
                  sizes=path.join(env.data, 'many_contig_sizes'))
if args.do_it:
    sp.call(call, shell=True)
else:
    print call
