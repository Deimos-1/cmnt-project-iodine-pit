
%% Get the data
fid = fopen("../data/minimal_data.json",'r');
rawBytes = fread(fid,inf); 
strData = char(rawBytes'); % transposing: columns -> rows for jsondecode
fclose(fid); 
data = jsondecode(strData);


%% testing 
neutronFlux = 4.42e20;
tests.iodine(neutronFlux, data)