function genmha
% GENMHA 

%% settings
mapfile='MNI_BNatlas/MNI_BNatlas.txt';

%% load labels
fp=fopen(mapfile,'rt');
data=textread(mapfile,'%s','delimiter',',\n');
fclose(fp);
labels={};
for ii=1:size(data,1)/4
idval=str2num(data{(ii-1)*4+1});
labels{idval}=data{(ii-1)*4+2};
end

%% load .mha file template
fp=fopen('MNI_BNatlas/MNI_DTI/den/template.mha','rt');
mhatemplate=fread(fp,10000,'char=>char');
fclose(fp);

load('MNI_152.mat');
MNI_152=double(MNI_152);
minval=min(MNI_152(:));
maxval=max(MNI_152(:));
MNI_152=(MNI_152-minval)/(maxval-minval)*3000;

%% generate mha files
for iter=1:length(labels)
if length(labels{iter})==0,continue;end
% load as img variable
clear('img');
load(sprintf('MNI_BNatlas/MNI_DTI/den/mat/%d.mat',iter));
assert(exist('img')==1);
minval=min(img(:));
maxval=max(img(:));
img=(img-minval)/(maxval-minval)*3000-3000;
if ~exist(sprintf('MNI_BNatlas/MNI_DTI/den/%d',iter),'dir')
mkdir(sprintf('MNI_BNatlas/MNI_DTI/den/%d',iter));
end
% save img variable into .MHA as header
fp=fopen(sprintf('MNI_BNatlas/MNI_DTI/den/%d/%d.mha',iter,iter),'wt');
fwrite(fp,sprintf(mhatemplate,iter),'char');
fclose(fp);
% save img variable into .RAW file
fp=fopen(sprintf('MNI_BNatlas/MNI_DTI/den/%d/%d.raw',iter,iter),'wb');
fwrite(fp,MNI_152+img,'short');
fclose(fp);
end

%% run volume rendering program
for iter=1:length(labels)
if length(labels{iter})==0,continue;end
oldldpath=getenv('LD_LIBRARY_PATH');
setenv('LD_LIBRARY_PATH','')
system(sprintf('./gendensity -MHA MNI_BNatlas/MNI_DTI/den/%d/%d.mha -Jet',iter,iter));
setenv('LD_LIBRARY_PATH',oldldpath)
% break;
end

