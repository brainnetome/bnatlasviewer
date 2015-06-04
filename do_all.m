function do_all

addpath(genpath('plugins'))

inputdir = 'content/';
cachedir = 'cache/';
outputdir = 'output/';

imagefile = 'mni152.nii.gz';
atlasfile = 'aal.nii.gz';
namefile = 'aal.nii.txt';

% imagefile = 'mni152_2mm.nii.gz';
% atlasfile = 'bnatlas_2mm.nii.gz';
% namefile = 'bnatlas_2mm.nii.txt';

if ~exist(cachedir,'dir'),mkdir(cachedir);end
if ~exist(outputdir,'dir'),mkdir(outputdir);end

genslice([inputdir imagefile],[inputdir atlasfile]);
genmaps([inputdir namefile]);
gencenters([inputdir atlasfile],[inputdir namefile]);
gencolors([inputdir namefile]);

copyfile('main.js', [outputdir 'main.js']);

%% create package
% system(['tar zcvf static-final-$(date +%F).tar.gz ' ...
%         'imgseq-x-bg.png imgseq-x.png imgseq-y-bg.png imgseq-y.png ' ...
%         'imgseq-z-bg.png imgseq-z.png ' ...
%         'mask-0-001.png mask-1-001.png mask-2-001.png ' ...
%         'main.js gv_center.json slice_BNatlas.html']);

