
import csv
import json
from pprint import pprint


csvfile = open('bnatlas_tree.csv', 'r')
jsonfile = open('bnatlas_tree.json', 'w')

fieldnames = ("index","id","lobe","gyrus","area")
reader = csv.DictReader( csvfile, fieldnames)

lobes=[]
gyruses=[]
result=[]

# for row in reader:
#   json.dump(row, jsonfile)
#   jsonfile.write(',\n')

for row in reader:
  index=row['index']
  ID = row['id']
  lobe = row['lobe'].capitalize()
  gyrus = row['gyrus'].capitalize()
  area = row['area']
  if lobe not in lobes:
    lobes.append(lobe)
    result.append({'text':lobe})
    lidx=lobes.index(lobe)
    result[lidx]['children']=[]
  lidx=lobes.index(lobe)
  gyruses = [item['text'] for item in result[lidx]['children']]
  if gyrus not in gyruses:
    gyruses.append(gyrus)
    result[lidx]['children'].append({'text':gyrus})
    gidx=gyruses.index(gyrus)
    result[lidx]['children'][gidx]['children']=[]
  gidx=gyruses.index(gyrus)
  result[lidx]['children'][gidx]['children'].append({'text':ID,'id':index,'area':area})
  
  
# print lobe,gyrus
# pprint(result)
print json.dumps(result, indent=2, separators=(',',':'))
json.dump(result, jsonfile, indent=0, separators=(',',':'))
