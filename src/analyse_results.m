%% WARNING : 
% this analysis is made only for predefined scenarios that are in
% the repository.

LEGENDS = dictionary( ...
    "override_100_0_ding.csv", "Override attempt", ...
    "override_100_0_reference_ding.csv", "100 \rightarrow 0 %", ...
    "power_drop_100_0_ding.csv", "100 \rightarrow 0 %", ...
    "power_drop_80_0_ding.csv", "80 \rightarrow 0 %", ...
    "power_drop_60_0_ding.csv", "60 \rightarrow 0 %", ...
    "power_drop_40_0_ding.csv", "40 \rightarrow 0 %", ...
    "power_drop_20_0_ding.csv", "20 \rightarrow 0 %", ...
    "power_rise_0_100_ding.csv", "0 \rightarrow 100 %", ...
    "power_rise_0_75_ding.csv", "0 \rightarrow 75 %", ...
    "power_rise_0_50_ding.csv", "0 \rightarrow 50 %", ...
    "power_rise_0_25_ding.csv", "0 \rightarrow 25 %");

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
    % reduction cases 
    fileList = dir(fullfile(results_dir, filesep, "power_drop*.csv"));

    % start by plotting power_drop cases on the same graph.
    % plotting like Ding : 
    % fig1 = figure(); hold on
    % legends = strings(size(fileList));
    % for i = 1:1:numel(fileList)
    %     results = readtable( ...
    %         fullfile(fileList(i).folder, filesep, fileList(i).name) ...
    %     );
    %     semilogy( ...
    %         results.t/3600, ...
    %         utils.concentration2reactivity_pcm(results.X, data), ...
    %         LineStyle='-', ...
    %         LineWidth=1 ...
    %     ); 
    %     legends(i) = LEGENDS(fileList(i).name); 
    %     % so that TeX doesn't interpret '_' as subscript
    % end 
    % 
    % xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
    % legend(legends)
    % grid on 
    % grid minor    
    % saveas(fig1, fullfile(results_dir, 'power_drop_ding.jpg'))

    % plotting like Module 01 : 
    fig1 = figure(); hold on
    legends = strings(size(fileList));
    for i = 1:1:numel(fileList)
        results = readtable( ...
            fullfile(fileList(i).folder, filesep, fileList(i).name) ...
        );
        reactivities = utils.concentration2reactivity_pcm(results.X, data);
        semilogy( ...
            results.t/3600, ...
            reactivities - reactivities(1), ...
            LineStyle='-', ...
            LineWidth=1 ...
        ); 
        legends(i) = LEGENDS(fileList(i).name); 
        % so that TeX doesn't interpret '_' as subscript
    end 

    xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
    legend(legends)
    grid on 
    grid minor    
    saveas(fig1, fullfile(results_dir, 'power_drop_ding.jpg'))

    % power rises
    fileList = dir(fullfile(results_dir, filesep, "power_rise*.csv"));

    % start by plotting power_drop cases on the same graph.
    fig1 = figure(); hold on
    legends = strings(size(fileList));
    for i = 1:1:numel(fileList)
        results = readtable( ...
            fullfile(fileList(i).folder, filesep, fileList(i).name) ...
        );
        semilogy( ...
            results.t/3600, ...
            utils.concentration2reactivity_pcm(results.X, data), ...
            LineStyle='-', ...
            LineWidth=1 ...
        ); 
        legends(i) = LEGENDS(fileList(i).name); 
        % so that TeX doesn't interpret '_' as subscript
    end 

    xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
    legend(legends, Location="southeast")
    grid on 
    grid minor    
    saveas(fig1, fullfile(results_dir, 'power_rise_ding.jpg'))

    
    % mitigation strategies
    fileList = dir(fullfile(results_dir, filesep, "override*.csv"));
    fig2 = figure(); hold on
    legends = strings(size(fileList));
    for i = 1:1:numel(fileList)
        results = readtable( ...
            fullfile(fileList(i).folder, filesep, fileList(i).name) ...
        );
        semilogy( ...
            results.t/3600, ...
            utils.concentration2reactivity_pcm(results.X, data), ...
            LineStyle='-', ...
            LineWidth=1 ...
        ); 
        legends(i) = LEGENDS(fileList(i).name); 
        % so that TeX doesn't interpret '_' as subscript
    end 
    
    xlabel('t [hr]'); ylabel('- \rho_{Xe} [pcm]');
    legend(legends)
    grid on 
    grid minor    
    saveas(fig2, fullfile(results_dir, 'override_ding.jpg'))

else
    fprintf("Missing directory: results\n") 
end