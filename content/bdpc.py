#!/usr/bin/env python

import os,sys,json

# custom parameters
datadir = 'BDPC'
typelist=['BDf_FDR05','PCf_FDR05']

for typeidx in range(2):
  datatype=typelist[typeidx]
  filename_fmt = datadir+'/Cluster%d_'+datatype+'.txt'
  outfile = datatype+'.json'

  # initialize
  idx = 1
  ncluster = 246

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
      label = '_'.join(line[0:len(line)-1])#line[0]
      value = float(line[-1])
      thisdict[label] = value
      # print {line[0]:float(line[1])}
    print thisdict

    result[idx]=thisdict
  #   break
  # break

  fp = open(outfile,'wt')
  fp.write(json.dumps(result, indent=0, sort_keys=True))
