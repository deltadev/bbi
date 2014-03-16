#!/usr/bin/python

import env

import os, sys, argparse, inspect
import subprocess as sp
from os import path

parser = argparse.ArgumentParser(description="Generates many contig data.",
                                 formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument("--do-it", help="actually do it.", action='store_true')
args = parser.parse_args()


def run():
    tmpl = "contig_{cnum}\t{size}\n"
    f = open(path.join(env.data, 'many_contig_sizes'), 'w')
    for i in range(888):
        f.write(tmpl.format(cnum=i, size=1000+i))
    f.close()


    # +----------+  [a0, b0)
    #   +---------+ [a1, b1)
    #      +---+    [a2, b2)
    #       +-+     [a3, b3)
    #
    tmpl = "contig_{cnum}\t{start}\t{end}\n"
    f = open(path.join(env.data, 'many_contig_data'), 'w')
    for i in range(888):
        a0, a1, a2, a3 = 0, 10, 20, 30
        b0, b1, b2, b3 = 60, 70, 50, 40
        for offset in [100 * ii + i for ii in range(10)]:
            f.write(tmpl.format(cnum=i, start=a0 + offset, end=b0 + offset))
            f.write(tmpl.format(cnum=i, start=a1 + offset, end=b1 + offset))
            f.write(tmpl.format(cnum=i, start=a2 + offset, end=b2 + offset))
            f.write(tmpl.format(cnum=i, start=a3 + offset, end=b3 + offset))
    f.close()

if args.do_it:
    run()
else:
    print inspect.getsource(run)
    


