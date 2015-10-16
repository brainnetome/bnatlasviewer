function gendensity()

[dirname,name,ext]=fileparts(mfilename('fullpath'))
if ~exist('strsplit'),addpath([dirname '/plugins']);end

mapfile = [dirname '/content/bnatlas.nii.txt'];
folder = [dirname '/content/data/den'];
volrender = [dirname '/plugins/volrender/volrender'];
surffile = [dirname '/content/avsurf.obj'];

% load labels                                      
labels = getlabels(mapfile);

% copy images to target location
for ii=1:length(labels)
    disp(sprintf('%03d:%s',ii,labels{ii}));
    targetdir = sprintf('%s/%s_pm_norm',folder,labels{ii});
    if ~exist(targetdir),mkdir(targetdir);end
    copyfile(sprintf('%s/%s_pm_norm.nii.gz',folder,labels{ii}),...
             sprintf('%s/%03d.nii.gz',targetdir,ii));
end

% execute volume render program to generate images
oldldpath=getenv('LD_LIBRARY_PATH');
setenv('LD_LIBRARY_PATH','');
for ii=1:length(labels)
    disp(sprintf('%03d:%s',ii,labels{ii}));
    targetdir = sprintf('%s/%s_pm_norm',folder,labels{ii});
    system(sprintf('%s -Jet -NIFTI %s -MESH %s',...
                   volrender,sprintf('%s/%03d.nii.gz',targetdir,ii),surffile));
end
setenv('LD_LIBRARY_PATH',oldldpath);

end


