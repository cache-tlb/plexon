function fErr = Entry_Train(data_name, nEpo, nWorkMode)
%% load txt data
xs = load(data_name);
strFileName = strcat(data_name(1:end-4), '_model.mat');
fErr =ImplElec_Train(xs.matSpikeCount, xs.matMov, nEpo, strFileName, nWorkMode);
end
