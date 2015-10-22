
from pprint import pprint

import csv
import json

csvfile = open('bnatlas_tree.csv', 'r')
jsonfile = open('bnatlas_tree.json', 'w')

fieldnames = ("index","id","lobe","gyrus","area")
reader = csv.DictReader( csvfile, fieldnames)

lobes=[]
gyruses=[]
result=[]


for row in reader:
  index=row['index']
  ID = row['id']
  lobe = row['lobe'].strip()
  gyrus = row['gyrus'].strip()
  area = row['area']
  # if lobe not in lobes:
  #   lobes.append(lobe)
  #   result.append({'text':lobe})
  #   lidx=lobes.index(lobe)
  #   result[lidx]['children']=[]
  # lidx=lobes.index(lobe)
  # gyruses = [item['text'] for item in result[lidx]['children']]
  # if gyrus not in gyruses:
  #   gyruses.append(gyrus)
  #   result[lidx]['children'].append({'text':gyrus})
  #   gidx=gyruses.index(gyrus)
  #   result[lidx]['children'][gidx]['children']=[]
  # gidx=gyruses.index(gyrus)
  # data={'alias':area,}
  # result[lidx]['children'][gidx]['children'].append({'text':ID,'id':index,'data':data,'type':'file'})

  islobe=0
  isgyrus=0
  if lobe not in lobes:
    lobes.append(lobe)
    result.append({'text':lobe,'id':lobe,'parent':'#'})
  #   lidx=lobes.index(lobe)
  #   result[lidx]['id']=lobe #.lower().replace(' ','_')
  # lidx=lobes.index(lobe)
  gyruses = [item['text'] for item in filter(lambda x:x.get('parent')==lobe,result)]
  # print gyruses
  if gyrus not in gyruses:
    gyruses.append(gyrus)
    result.append({'text':gyrus,'id':gyrus,'parent':lobe})
    # gidx=gyruses.index(gyrus)
    # result[lidx]['id'][gidx]['children']=[]
  # gidx=gyruses.index(gyrus)
  data={'alias':area,}
  result.append({'text':ID,'id':index,'data':data,'type':'file','parent':gyrus})
  
  
print json.dumps(result, indent=2, separators=(',',':'))
json.dump(result, jsonfile, indent=0, separators=(',',':'))
