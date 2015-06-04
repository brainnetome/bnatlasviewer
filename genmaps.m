function genmaps(mapfile)

%mapfile='MNI_BNatlas/MNI_BNatlas.txt';
% mapfile='MNI_AAL/MNI_AAL.txt';
cachedir = 'cache/';
outputdir = 'output/';

%% load labels
fp=fopen(mapfile,'rt');
labels={};
while 1
l=fgets(fp);
data=strsplit(char(l),' ');
if size(data{1})<1,break;end
idval=str2num(data{1})
labels{idval}=data{2};
end
fclose(fp);

numslices=[181,217,181];

maps=[]
%% parameters
for dimval=1:3
for sliceidx=1:numslices(dimval)
  disp(sprintf('processing map-%d-%d',dimval-1,sliceidx));    
  
  %% load raw data
  load(sprintf([cachedir '%d/%03d.mat'],dimval-1,sliceidx));
  if (max(im(:))==0),maps(dimval,sliceidx).html='';continue;end
  
  %% get area info
  C=unique(im);C=C(2:end);
  numC=size(C,1);
  
  %% extract contour 
  areas=[];areacounter=1;
  for ii=1:numC
    isoval=C(ii);
    contour=bwboundaries(im==isoval);
    for jj=1:size(contour,1)
      contourdesc=flipud((contour{jj})');
      poly=sprintf('%.0f,',reshape(contourdesc,1,[]));
      areas(areacounter).poly=poly(1:end-1);
      areas(areacounter).title=labels{C(ii)};
      areacounter=areacounter+1;
    end
  end

  %% generate map html
  maps(dimval,sliceidx).html=genmaphtml(areas,dimval,sliceidx);
end % sliceidx
end % dimval

mapshtml=[];
for dimval=1:3
for sliceidx=1:numslices(dimval)
  mapshtml=[mapshtml sprintf('%s',maps(dimval,sliceidx).html)];
end
end
fp=fopen('template.html','rt');
template=fread(fp,100000,'char=>char');
fclose(fp);
fp=fopen([outputdir 'bnatlas.html'],'wt');
fprintf(fp,template,mapshtml);
fclose(fp);