function genprobmaps(mapfile, fibdir, dendir, fundir, outdir)
% GENPROBMAPS generates concatenated image sets of both density and
% probabilistic DTI tracking results, which requires running GENMHA
% command to generate probabilistic DTI tracking results and store
% them under MNI_BNatlas/MNI_DTI/den.
%
% See Also:
%    GENMHA, GENMAPS
% 
% Copyright (C) Liangfu Chen (2013-2014) <liangfu.chen@nlpr.ia.ac.cn>
% Brainnetome Center, NLPR at CASIA

%% settings
angle_interval=10;
nc=240;
nr=200;
%mapfile='MNI_BNatlas/MNI_BNatlas.txt';

%% load labels
fp=fopen(mapfile,'rt');
data=textread(mapfile,'%s','delimiter',',\n');
fclose(fp);
labels={};
parfor ii=1:size(data,1)/4
idval=str2num(data{(ii-1)*4+1});
labels{idval}=data{(ii-1)*4+2};
end

%% for loop ??
for iter=1:length(labels)
if length(labels{iter})==0,continue;end
% idx=10;
idx=iter;

%% read all image into memory
disp(sprintf('[%03d]: reading all images into memory',iter));
folder=sprintf('%s/%d/',dendir,idx);
parfor ii=1:angle_interval:360
den{floor(ii/angle_interval)+1}.im=...
    imresize(imread([folder sprintf('%03d.png',ii-1)]),2/3,'cubic');
assert((size(den{floor(ii/angle_interval)+1}.im,1)==nr)&& ...
       (size(den{floor(ii/angle_interval)+1}.im,2)==nc));
end
folder=sprintf('%s/%d/',fibdir,idx);
parfor ii=1:angle_interval:360
fib{floor(ii/angle_interval)+1}.im=...
    imresize(imread([folder sprintf('%03d.png',ii-1)]),1/1,'cubic');
assert((size(fib{floor(ii/angle_interval)+1}.im,1)==nr)&& ...
       (size(fib{floor(ii/angle_interval)+1}.im,2)==nc));
end
folder=sprintf('%s/%02d/',fundir,idx);
parfor ii=1:angle_interval:360
fun{floor(ii/angle_interval)+1}.im=...
    imresize(imread([folder sprintf('%03d.png',ii-1)]),1/1,'cubic');
assert((size(fun{floor(ii/angle_interval)+1}.im,1)==nr)&& ...
       (size(fun{floor(ii/angle_interval)+1}.im,2)==nc));
end

%% initialize density and fiber maps
disp(sprintf('[%03d]: initializing density and fiber maps',iter));
nnc=5;
nnr=ceil(360./angle_interval/5.);
dencat=imconcat(den,nnr,nnc);
fibcat=imconcat(fib,nnr,nnc);
funcat=imconcat(fun,nnr,nnc);
% size(dencat)
imwrite(1-dencat(:,:,:)/255,sprintf('%s/den-%03d.jpg',outdir,idx));
imwrite(1-fibcat(:,:,:)/255,sprintf('%s/fib-%03d.jpg',outdir,idx));
imwrite(  funcat(:,:,:)/255,sprintf('%s/fun-%03d.jpg',outdir,idx));
end

end % function

function dst=imconcat(imlist,nnr,nnc)
nr=size(imlist{1}.im,1);
nc=size(imlist{1}.im,2);
dst=zeros(nr*nnr,nc*nnc,size(imlist{1}.im,3));
for ii=1:length(imlist)
% [ii mod(ii-1,nnc),floor((ii-1)/nnc)]
xloc=mod(ii-1,nnc)*nc+1;
yloc=floor((ii-1)/nnc)*nr+1;
dst(yloc:yloc+nr-1,xloc:xloc+nc-1,:)=imlist{ii}.im(:,:,:);
%imwrite(mat2gray(squeeze(dst(yloc:yloc+nr-1,xloc:xloc+nc-1,1))),'test.png');
end
end % function imconcat

