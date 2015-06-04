function maphtml=genmaphtml(areas,dimval,sliceidx)
outstr=[];
for ii=1:length(areas)
tmpstr=sprintf('<area shape="poly" title="%s" coords="%s">\n',...
               areas(ii).title,areas(ii).poly);
outstr=[outstr,tmpstr];
end
maphtml=sprintf('<map id="map-%d-%d" name="map-%d-%d">\n%s</map>\n\n',...
                dimval-1,sliceidx,dimval-1,sliceidx,outstr);

