%% Get the data
fid = fopen("../data/data_ding.json",'r');
rawBytes = fread(fid,inf); 
strData = char(rawBytes'); % transposing: columns -> rows for jsondecode
fclose(fid); 
data = jsondecode(strData);

%% Analyse the results of scenarios 
% parse the results folder and graph them 

% first find the results folder and available files
pwd = fileparts(mfilename("fullpath"));
root_dir = fileparts(pwd); 
results_dir = fullfile(root_dir, filesep, "results");

if exist(results_dir,"dir")
    fileList = dir(fullfile(results_dir, filesep, "*.csv"));

    for i = 1:1:numel(fileList)
        results = readtable(fullfile(fileList(i).folder, filesep, fileList(i).name));
        hold on
        fig = plot(results.t/3600, utils.concentration2reactivity_pcm(results.X, data), LineStyle='-', LineWidth=1.5, Color='black');
        xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
        grid on 
        grid minor    
        saveas(fig, fullfile(results_dir, strcat(fileList(i).name(1:end-4), '.jpg')) )                                                    
    end 

else
    fprintf("Missing directory: results\n") 
end






