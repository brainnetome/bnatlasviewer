
import sys
from pprint import pprint
import re
from xml.dom.minidom import parse
from xml.dom import Node
import json

jsonfilename = 'bnatlas_tree.json'

svgfile = open('data/connectogram/001.svg','r')
#jsonfile = open(jsonfilename, 'w')

def get_elements(elems):
  return filter(lambda elem:elem.nodeType==Node.ELEMENT_NODE,elems)
def get_element_by_id(elems,idname):
  return filter(lambda elem:elem.getAttribute('id')==idname,elems)[0]
def isfloat(value):
  try:
    float(value)
    return True
  except:
    return False
def tofloat(v):
  return float(v) if isfloat(v) else float(v[1:])
def toint(v):
  return int(round(float(v))) if isfloat(v) else int(round(float(v[1:])))
def get_points(d):
  s=d.split()
  retval=[]
  retval.append(s[0].split(','))
  retval.append(s[4].split(','))
  retval.append(s[5].split(','))
  retval.append(s[9].split(','))
  retval = map(lambda s:map(lambda x:toint(x),s),retval)
  retval = reduce(lambda a,b:a+b,retval)
  return retval #list(enumerate(retval))

dom = parse(svgfile)
svg = get_elements(dom.getElementsByTagName('svg')[0].childNodes)
ideograms = get_elements(get_element_by_id(svg,'ideograms').childNodes)

stage = 0
cgram = {}
idx=-1
idx_stage = 0
idx_count = 246

verbose=0

for tag in ideograms:
  if verbose: print '%s:%d:' % (tag.tagName,stage,),
  
  if tag.tagName=='path':
    points=get_points(tag.getAttribute('d'))
    if stage==1:
      if (idx+1)<idx_count and idx_stage==0:
        idx += 2
      elif (idx+1)>=idx_count and idx_stage==0:
        idx += 1
        idx_stage=1
      elif idx_stage==1:
        idx -= 2
      if verbose: print('%03d:' % idx),
      if verbose: print points
      cgram[idx]=points
    else:
      if verbose: print
  elif tag.tagName=='text':
    if verbose: print tag.firstChild.nodeValue

  if tag.tagName=='path' and stage==0:
    stage = 1
  elif tag.tagName=='path' and stage==1:
    stage = 1
  elif tag.tagName=='text' and stage==1:
    stage = 2
  elif tag.tagName=='path' and stage==2:
    stage = 3
  elif tag.tagName=='path' and stage==3:
    stage = 1

#pprint(cgram)


# post processing
result=json.load(open(jsonfilename, 'r'))
#pprint(result)

for info in cgram:
  for area in result:
    if str(area['id'])==str(info):
      area['data']['cgram']=cgram[info]

jsonfile = open(jsonfilename, 'w')
print json.dumps(result, indent=2, separators=(',',':'))
json.dump(result, jsonfile, indent=0, separators=(',',':'))

