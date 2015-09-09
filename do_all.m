function do_all

addpath(genpath('plugins'))

inputdir = 'content/';
cachedir = 'cache/';
outputdir = 'output/';

% imagefile = 'mni152.nii.gz';
% atlasfile = 'aal.nii.gz';
% namefile = 'aal.nii.txt';

imagefile = 'mni152.nii.gz';
atlasfile = 'bnatlas.nii.gz';
namefile = 'bnatlas.nii.txt';

if exist(cachedir,'dir'),rmdir(cachedir,'s');end;mkdir(cachedir);
if exist(outputdir,'dir'),rmdir(outputdir,'s');end;mkdir(outputdir);

genslice([inputdir imagefile],[inputdir atlasfile]);
genmaps([inputdir namefile]);
gencenters([inputdir atlasfile],[inputdir namefile]);
gencolors([inputdir namefile]);

%% copy required files 
copyfile('loading.gif', [outputdir 'loading.gif']);

% javascript programs
copyfile([inputdir 'jquery.min.js'], [outputdir 'jquery.min.js']);
copyfile([inputdir 'jstree.min.js'], [outputdir 'jstree.min.js']);
copyfile([inputdir 'css'], [outputdir 'css']);
copyfile('Chart.HorizontalBar.js', [outputdir 'Chart.HorizontalBar.js']);
copyfile('Chart.js', [outputdir 'Chart.js']);
copyfile('bnatlasviewer.js', [outputdir 'bnatlasviewer.js']);

% json data files
copyfile([inputdir 'BDf_FDR05.json'], [outputdir 'BDf_FDR05.json']);
copyfile([inputdir 'PCf_FDR05.json'], [outputdir 'PCf_FDR05.json']);
copyfile([inputdir 'bnatlas_tree.json'], [outputdir 'bnatlas_tree.json']);

%% create package
% system(['tar zcvf static-final-$(date +%F).tar.gz ' ...
%         'imgseq-x-bg.png imgseq-x.png imgseq-y-bg.png imgseq-y.png ' ...
%         'imgseq-z-bg.png imgseq-z.png ' ...
%         'mask-0-001.png mask-1-001.png mask-2-001.png ' ...
%         'main.js gv_center.json slice_BNatlas.html']);

end

