function labels = getlabels(mapfile)
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
end