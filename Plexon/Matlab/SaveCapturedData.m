function res = SaveCapturedData(matSpikeCount, matMov, strFileName)
save(strFileName, 'matSpikeCount', 'matMov', '-mat');
res = 0;
end
