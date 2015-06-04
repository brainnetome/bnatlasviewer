% SLICE 
%    GENSLICE(IMAGEFILE,ATLASFILE)
function genslice(imagefile,atlasfile)

cachedir = 'cache/';
outputdir = 'output/';

%-------------------------------------------------------
% label images
%-------------------------------------------------------
nim = load_nii(atlasfile);
d = nim.img;

if ~exist(cachedir,'dir'),mkdir(cachedir),end
if ~exist([cachedir '0'],'dir'),mkdir([cachedir '0']),end
if ~exist([cachedir '1'],'dir'),mkdir([cachedir '1']),end
if ~exist([cachedir '2'],'dir'),mkdir([cachedir '2']),end

for ii=1:size(d,1),
im=flipud(reshape(d(ii,:,:),size(d,2),[])');
save(sprintf([cachedir '0/%03d.mat'],ii),'im');
end
for ii=1:size(d,2),
im=flipud(reshape(d(:,ii,:),size(d,1),[])');
save(sprintf([cachedir '1/%03d.mat'],ii),'im');
end
for ii=1:size(d,3),
im=flipud(reshape(d(:,:,size(d,3)+1-ii),size(d,1),[])');
save(sprintf([cachedir '2/%03d.mat'],ii),'im');
end

%-------------------------------------------------------
% background brain image
%-------------------------------------------------------
for isBG=0:1

if isBG
nim = load_nii(imagefile);
d = nim.img;
else
nim = load_nii(atlasfile);
d = nim.img;
end

divisor=10;
im0=zeros(181*ceil(181/divisor),217*divisor);
im1=zeros(181*ceil(217/divisor),181*divisor);
im2=zeros(217*ceil(181/divisor),181*divisor);

for ii=1:size(d,1)
yloc=floor((ii-1)/divisor)*181;
xloc=mod(ii-1,divisor)*217;
im0(yloc+1:yloc+181,xloc+1:xloc+217)=flipud(reshape(d(ii,:,:),size(d,2),[])');
end

for ii=1:size(d,2)
yloc=floor((ii-1)/divisor)*181;
xloc=mod(ii-1,divisor)*181;
im1(yloc+1:yloc+181,xloc+1:xloc+181)=flipud(reshape(d(:,ii,:),size(d,1),[])');
end

for ii=1:size(d,3)
yloc=floor((ii-1)/divisor)*217;
xloc=mod(ii-1,divisor)*181;
im2(yloc+1:yloc+217,xloc+1:xloc+181)=flipud(reshape(d(:,:,size(d,3)+1-ii),size(d,1),[])');
end

if isBG
imwrite(mat2gray(im0),[outputdir 'imgseq-x-bg.png']);
imwrite(mat2gray(im1),[outputdir 'imgseq-y-bg.png']);
imwrite(mat2gray(im2),[outputdir 'imgseq-z-bg.png']);
else
save([cachedir 'imgseq-xyz.mat'],'im0','im1','im2');
imwrite(mat2gray(im0),[outputdir 'imgseq-x.png']);
imwrite(mat2gray(im1),[outputdir 'imgseq-y.png']);
imwrite(mat2gray(im2),[outputdir 'imgseq-z.png']);
end

end

% imshow(im0,[]);
% imshow(im1,[]);
% imshow(im2,[]);
