function do_all

addpath(genpath('plugins'))

inputdir = 'content/';
cachedir = 'cache/';
outputdir = 'output/';

% imagefile = 'mni152.nii.gz';
% atlasfile = 'aal.nii.gz';
% namefile = 'aal.nii.txt';

imagefile = [inputdir 'mni152.nii.gz'];
atlasfile = [inputdir 'bnatlas.nii.gz'];
namefile = [inputdir 'bnatlas.nii.txt'];
fibdir = [inputdir 'data/fib'];
dendir = [inputdir 'data/den'];
fundir = [inputdir 'data/fmri'];

% if exist(cachedir,'dir'),rmdir(cachedir,'s');end;mkdir(cachedir);
% if exist(outputdir,'dir'),rmdir(outputdir,'s');end;mkdir(outputdir);

% genslice(imagefile, atlasfile);
% genmaps(namefile);
% gencenters(atlasfile, namefile);
% gencolors(namefile);
genprobmaps(namefile, fibdir, dendir, fundir, [outputdir 'images'])

%% copy required files 
copyfile([inputdir 'loading.gif'], [outputdir 'loading.gif']);
copyfile(['./' 'template.html'], [outputdir 'bnatlas.html']);

% javascript programs
copyfile([inputdir 'css'], [outputdir 'css']);
copyfile([inputdir 'js'], [outputdir 'js']);

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

