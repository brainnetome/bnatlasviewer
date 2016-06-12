
from pprint import pprint

import csv
import json

csvfile = open('bnatlas_tree.csv', 'r')
outfile = open('bnatlas_ontology.csv', 'w')

fieldnames = ("index","id","lobe","gyrus","area")
reader = csv.DictReader( csvfile, fieldnames)

lobes=[]
gyruses=[]
result=[{'id':0,'name':'brain','acronym':'Br'},]

boff=0
loff=10000
goff=100

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
    lidx=lobes.index(lobe)
    result.append({'id':lobe,'name':lobe.lower(),'parent_structure_id':'0'})
    # result[lidx]['id']=lidx #.lower().replace(' ','_')
  lidx=lobes.index(lobe)
  gyruses = [item['id'] for item in filter(lambda x:x.get('parent_structure_id')==lobe,result)]
  # print gyruses
  if gyrus not in gyruses:
    gyruses.append(gyrus)
    gidx=gyruses.index(gyrus)
    result.append({'id':gyrus,'name':gyrus.split(',')[1].strip().lower(),'parent_structure_id':lobe})
    # result[lidx]['id'][gidx]['children']=[]
  gidx=gyruses.index(gyrus)
  orient='left, ' if int(index) % 2 == 1 else 'right, '
  name=lobe.lower()+', '+orient+gyrus.split(',')[1].strip().lower()+', '+area.split(',')[-1].strip()
  result.append({'name':name,'id':index,'acronym':ID,'parent_structure_id':gyrus})
  
# pprint(result)
gyruses=[]
for ridx,row in enumerate(result):
  if row.get('id') in lobes:
    lidx=lobes.index(row.get('id'))
    result[ridx]['id']=loff*(lidx+1)+boff
  elif row.get('parent_structure_id') in lobes:
    lidx=lobes.index(row.get('parent_structure_id'))
    result[ridx]['parent_structure_id']=loff*(lidx+1)+boff
    gyrus=result[ridx]['id']
    if gyrus not in gyruses: gyruses.append(gyrus)
    gidx=gyruses.index(gyrus)
    result[ridx]['id']=goff*(gidx+1)+loff*(lidx+1)+boff
  elif row.get('parent_structure_id') in gyruses:
    gyrus=result[ridx]['parent_structure_id']
    gidx=gyruses.index(gyrus)
    result[ridx]['parent_structure_id']=goff*(gidx+1)+loff*(lidx+1)+boff

writer=csv.DictWriter(outfile,fieldnames=['id','name','acronym','parent_structure_id'])
writer.writeheader()
[writer.writerow(r) for r in result]
outfile.close()





