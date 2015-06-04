function gencenters(atlasfile,mapfile)

outputdir = 'output/';

nim=load_nii(atlasfile);
d=nim.img;

%% load labels
fp=fopen(mapfile,'rt');
labels={};
while 1
l=fgets(fp);
data=strsplit(char(l),' ');
if size(data{1})<1,break;end
idval=str2num(data{1});
labels{idval}=data{2};
end
fclose(fp);

%% compute 
center=[];
jsonstr=sprintf('var GLOBAL_title2center={};\n');
for isoval=1:max(d(:))
if length(labels{isoval})==0,continue;end
idx=find(d==isoval);tmp=zeros(size(d));tmp(idx)=1;
centerprop=regionprops(tmp,'centroid');
center(isoval,:)=centerprop.Centroid;
tmpstr=[];
tmpstr=[tmpstr sprintf('%.0f,',center(isoval,2)) ];
tmpstr=[tmpstr sprintf('%.0f,',center(isoval,1)) ];
tmpstr=[tmpstr sprintf('%.0f',181-center(isoval,3)) ];
jsonstr=[jsonstr sprintf('GLOBAL_title2center["%s"]=[%s];\n',labels{isoval},tmpstr)];
end
jsonstr=[jsonstr sprintf('\n\n')];

jsonstr=[jsonstr sprintf('var GLOBAL_title2ind={};\n')];
for isoval=1:max(d(:))
jsonstr=[jsonstr sprintf('GLOBAL_title2ind["%s"]=%d',labels{isoval},isoval-1) sprintf(';\n')];
end

fp=fopen([outputdir 'gv_center.json'],'wt');fprintf(fp,'%s',jsonstr);fclose(fp);

