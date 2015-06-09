#!/usr/bin/env python

import os,sys,json

# custom parameters
datadir = 'BDf_FDR05'
filename_fmt = datadir+'/Cluster%d_BDf_FDR05.txt'
outfile = datadir+'.json'

# initialize
idx = 1
ncluster = 252

result = {}

for idx in range(1,ncluster+1):
  filename = filename_fmt % idx
  fp = open(filename)
  if not fp: break
  print 'processing %s ...' % filename

  thisdict = {}
  while 1:
    linestr = fp.readline()
    if not linestr: break;
    line = linestr.split()
    thisdict[line[0]] = float(line[-1])
    # print {line[0]:float(line[1])}
    label = line[0]
    value = line[1]
  print thisdict

  result[idx]=thisdict

fp = open(outfile,'wt')
fp.write(json.dumps(result, indent=4, sort_keys=True))
