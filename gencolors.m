function gencolors(mapfile)

%mapfile='MNI_BNatlas/MNI_BNatlas.txt';
cachedir = 'cache/';

%% load image {im0,im1,im2}
load([cachedir 'imgseq-xyz.mat'])

%% load labels
fp=fopen(mapfile,'rt');
labels={};
while 1
l=fgets(fp);
data=strsplit(l,' ');
if size(data{1})<1,break;end
idval=str2num(data{1});
labels{idval}=data{2};
tags{idval}=data{2};
end
fclose(fp);

% fp=fopen(mapfile,'rt');
% data=textread(mapfile,'%s','delimiter',',\n');
% fclose(fp);
% labels=data(2:4:end);
% tags=data(3:4:end);
% for ii=1:length(tags),tags{ii}=tags{ii}(1:3);end
[C,I,J]=unique(tags);
nC=length(tags)+1;%size(C,1);
% for ii=1:length(tags),tag2id(str2num(tags{ii}))=J(ii);end

%% generate random color map
hsvcm=randi(255,[nC,1]);
hsvcm=[hsvcm ones([nC,2])*255];
rgbcm=hsv2rgb(double(hsvcm)/255);
rgbcm(1,:)=[0,0,0]

% img=ind2rgb(1:116,rgbcm);imshow(img);return;

im0_new=ind2rgb(im0+1,rgbcm);
im1_new=ind2rgb(im1+1,rgbcm);
im2_new=ind2rgb(im2+1,rgbcm);

alphamap0=mat2gray(bitor(bitor(im0_new(:,:,1)>0,im0_new(:,:,2)>0),im0_new(:,:,3)>0));
alphamap1=mat2gray(bitor(bitor(im1_new(:,:,1)>0,im1_new(:,:,2)>0),im1_new(:,:,3)>0));
alphamap2=mat2gray(bitor(bitor(im2_new(:,:,1)>0,im2_new(:,:,2)>0),im2_new(:,:,3)>0));

if ~exist('output','dir'),mkdir('output');end

imwrite(im0_new,'output/imgseq-x.png','alpha',alphamap0);
imwrite(im1_new,'output/imgseq-y.png','alpha',alphamap1);
imwrite(im2_new,'output/imgseq-z.png','alpha',alphamap2);

% imwrite(im0_new,'output/imgseq-x.png','alpha',im0_new);
% imwrite(im1_new,'output/imgseq-y.png','alpha',im1_new);
% imwrite(im2_new,'output/imgseq-z.png','alpha',im2_new);

% im0_new=zeros([size(im0) 3]);
% for ii=1:size(im0,1)
% for jj=1:size(im0,2)
% if im0(ii,jj)==0,continue;end
% id=tag2id(str2num(tags{im0(ii,jj)}));
% im0_new(ii,jj,:)=rgbcm(id,:);
% end
% end


