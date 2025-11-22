
%% Get the data
fid = fopen("../data/data_paper.json",'r');
rawBytes = fread(fid,inf); 
strData = char(rawBytes'); % transposing: columns -> rows for jsondecode
fclose(fid); 
data = jsondecode(strData);


%% testing 
neutronFlux = 1e14;
%tests.iodine(neutronFlux, data)
scenarios.power_drop(neutronFlux,data)
% We don't observe the rise in concentration...
