function [X, Y] = nex2mat(nexFileName, labelFileName, saveFileName)
    nex = readNexFile(nexFileName);
    labels = load(labelFileName);
    % each label is a row 
    nLabels = size(labels, 1);
    nChannels = 16*4;               % 16 SPK * 4 sorts
    X = zeros(nLabels, nChannels);
    Y = labels(:, 3);
    neurons = nex.neurons;
    for k = 1:length(neurons)
        neuron_name = neurons{k}.name;
        channel_id = (str2num(neuron_name(end-2:end-1)) - 1)*4 + (neuron_name(end) - 'a' + 1);
        ts = neurons{k}.timestamps;
        for i = 1:length(ts)
            for j = 1:nLabels
                % find a label
                if labels(j, 1) <= ts(i) && ts(i) <= labels(j, 2)
                    X(j, channel_id) = X(j, channel_id) + 1;
                    break;
                end
            end
        end
        % end for each neuron channel
    end
    % todo: save the result
    if ~isempty(saveFileName)
        save(saveFileName, 'X', 'Y');
    end
end
