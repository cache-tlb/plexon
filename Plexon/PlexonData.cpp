#include "stdafx.h"
#include "PlexonData.h"
#include "MatlabHelper.h"

#pragma comment (lib, "PlexClient.lib")

PlexonData::PlexonData(CDialogEx *parent):
    pServerEventBuffer(NULL), 
    windowSizeInMs(50),
    windowSizeInTimeStamp(50*40000)
{
    mainWindow = parent;
}


PlexonData::~PlexonData(void) {
    if (pServerEventBuffer) {
        free(pServerEventBuffer);
    }
    PL_CloseClient();
}

bool PlexonData::Init() {
    pServerEventBuffer = (PL_WaveLong*)malloc(sizeof(PL_WaveLong) * MAX_MAP_EVENTS_PER_READ);
    if (pServerEventBuffer == NULL) {
        MessageBox(mainWindow->GetSafeHwnd(), L"Couldn't allocate memory!", L"Error", MB_OK);
        return false;
    }
    return true;
}

int PlexonData::ReadSpike() {
    int num_events = MAX_MAP_EVENTS_PER_READ;
    int ServerDropped, MMFDropped, PollHigh, PollLow;
    PL_GetLongWaveFormStructuresEx2(&num_events, pServerEventBuffer, &ServerDropped, &MMFDropped, &PollHigh, &PollLow);
	/*CString txt;
	txt.Format(L"num_event %d" , num_events);
	MessageBox(mainWindow->GetSafeHwnd(), txt, L"INFO", MB_OK);*/

    int num_spike_timestamps = 0;
    for (int i = 0; i < num_events; i++) {
		unsigned long time_stamp = pServerEventBuffer[i].TimeStamp;
		int batch_index = time_stamp / windowSizeInTimeStamp - initalBatchNumber;
		if (spikeDataBatch.size() <= batch_index) {
            spikeDataBatch.resize(batch_index + 1);
            //int oldSize = sampleDataStatistics.size();
            sampleDataStatistics.resize(batch_index + 1);
            sampleDataSum.resize(batch_index + 1);
            sampleDataCnt.resize(batch_index + 1);
        }

        if (pServerEventBuffer[i].Type == PL_SingleWFType) {
            spikeDataBatch[batch_index].push_back(SpikeDataType(
                pServerEventBuffer[i].TimeStamp,
                pServerEventBuffer[i].Unit,
                pServerEventBuffer[i].Channel ));
            short channel = pServerEventBuffer[i].Channel;
            sampleDataStatistics[batch_index].val[channel]++;
            num_spike_timestamps ++;
        } else if (pServerEventBuffer[i].Type == PL_ADDataType && pServerEventBuffer[i].Channel >= ANALOG_CHANNEL_BASE) {
            short analog_channel = pServerEventBuffer[i].Channel - ANALOG_CHANNEL_BASE;
            /*int NumNIDAQSamples = pServerEventBuffer[i].NumberOfDataWords;
            for (int j = 0; j < NumNIDAQSamples; j++) {
                sampleDataSum[batch_index].val[analog_channel] += abs(pServerEventBuffer[i].WaveForm[j]);
                sampleDataCnt[batch_index].val[analog_channel] ++;
            }
            sampleDataStatistics[batch_index].val[analog_channel + MAX_SPIKE_CHANNEL_NUMBER] = 
                sampleDataCnt[batch_index].val[analog_channel] > 0 ? 
                (sampleDataSum[batch_index].val[analog_channel] / sampleDataCnt[batch_index].val[analog_channel]) : 0;*/
            sampleDataStatistics[batch_index].val[analog_channel + MAX_SPIKE_CHANNEL_NUMBER] = pServerEventBuffer[i].WaveForm[0];
        } else if (pServerEventBuffer[i].Type == PL_ADDataType && pServerEventBuffer[i].Channel < MAX_SPIKE_CHANNEL_NUMBER) {
			short channel = pServerEventBuffer[i].Channel;
			sampleDataStatistics[batch_index].val[channel + MAX_SPIKE_CHANNEL_NUMBER + MAX_MOTION_CHANNEL_NUMBER] = pServerEventBuffer[i].WaveForm[0];
		}

        // prepare data for save
        if (pServerEventBuffer[i].Type == PL_ADDataType) {
            short channel = pServerEventBuffer[i].Channel;
            assert(channel < TOTAL_CHANNEL_NUMBER);
            dataForSave[channel].push_back(TimeValueType(pServerEventBuffer[i].TimeStamp, pServerEventBuffer[i].WaveForm[0]));
        }
    }

    return num_spike_timestamps;
}

bool PlexonData::ConnectDataServer() {
    PL_CloseClient();
    int h = PL_InitClientEx3(0, NULL, mainWindow->GetSafeHwnd());
    int time_stamp_tick = PL_GetTimeStampTick();
    switch (time_stamp_tick) {
    case 25: 
        mapSampleRate = 40000;
    	break;
    case 40:
        mapSampleRate = 25000;
        break;
    case 50:
        mapSampleRate = 20000;
        break;
    default:
        MessageBox(mainWindow->GetSafeHwnd(), L"Unsupported MAP sampling time, I can't continue!\r\n", L"Error", MB_OK);
        mapSampleRate = 40000*25/time_stamp_tick;
        return false;
    }
    windowSizeInTimeStamp = windowSizeInMs * mapSampleRate / 1000;
    GetGain();
    // set the initial time stamp;
    // SetupInitBatchNumber();
    return true;
}

void PlexonData::SetupInitBatchNumber() {
    Sleep(10);
    int num_events = MAX_MAP_EVENTS_PER_READ;
    int ServerDropped, MMFDropped, PollHigh, PollLow;
    PL_GetLongWaveFormStructuresEx2(&num_events, pServerEventBuffer, &ServerDropped, &MMFDropped, &PollHigh, &PollLow);
    unsigned long init_time_stamp = pServerEventBuffer[num_events - 1].TimeStamp;
    initalBatchNumber = init_time_stamp / windowSizeInTimeStamp;
    Sleep(10);
}

TimeValueType linear_interpolation(const std::vector<TimeValueType> &v, double pos) {
    if (pos < 0) return v[0];
    if (pos >= v.size()) return v.back();
    int low = floor(pos), high = ceil(pos);
    double f = pos - low;
    TimeValueType tv1 = v[low], tv2 = v[high];
    TimeValueType res;
    res.timestamp = (f < 0.5 ? tv1.timestamp : tv2.timestamp);
    res.value = (1-f)*tv1.value + f*tv2.value;
    return res;
}

void linear_resample(std::vector<TimeValueType> &vec, int n) {
    int old_n = vec.size();
    std::vector<TimeValueType> res;
    res.resize(n);
    for (int i = 0; i < n; i++) {
        double pos = double(i*(old_n - 1)) / double(n - 1);
        res[i] = linear_interpolation(vec, pos);
    }
    vec.swap(res);
}

int PlexonData::SaveData(const std::string &mat_filename, 
                          int neuron_channel_start, int neuron_channel_number, 
                          int motion_channel_start, int motion_channel_number) {
    // convert the data to flat matrix
    // re-organize the data.
	/*CString txt(mat_filename.c_str());
	MessageBox(NULL, txt, L"INFO", MB_OK);
	return 1;*/
    int max_sample_number = 0;
    for (int i = 0; i < dataForSave.size(); i++) {
        std::sort(dataForSave[i].begin(), dataForSave[i].end());
        max_sample_number = std::max<int>(max_sample_number, dataForSave[i].size());
    }
    for (int i = 0; i < dataForSave.size(); i ++) {
        if (dataForSave[i].empty()) {
            dataForSave[i].resize(max_sample_number, TimeValueType(0,0));
        } else if (dataForSave[i].size() != max_sample_number) {
            linear_resample(dataForSave[i], max_sample_number);
        }
    }

    std::string ecog_save_name = mat_filename;
	std::string spike_save_name = mat_filename;
    ecog_save_name = ecog_save_name + "_ecog.txt";
	spike_save_name = spike_save_name + "_spike.txt";
    std::ofstream fout(ecog_save_name);
    for (int i = 0; i < dataForSave.size(); i++) {
        for (int j = 0; j < dataForSave[i].size(); j++) {
            fout << dataForSave[i][j].value << ' ';
        }
        fout << std::endl;
    }
    fout.close();

    // save the spike data
    int num_bin = sampleDataStatistics.size();
    if (num_bin == 0) {
        return -1;
    }
    std::vector<double> neuron_mat(num_bin * neuron_channel_number);
    std::vector<double> motion_mat(num_bin * motion_channel_number);
    for (int bin_index = 0; bin_index < num_bin; bin_index++) {
        for (int i = 0; i < neuron_channel_number; i++) {
            neuron_mat[i*num_bin + bin_index] = sampleDataStatistics[bin_index].val[neuron_channel_start + i];
        }
        for (int i = 0; i < motion_channel_number; i++) {
            motion_mat[i*num_bin + bin_index] = sampleDataStatistics[bin_index].val[motion_channel_start + i];
        }
    }
    fout.clear();
    fout.open(spike_save_name);
    for (int i = 0; i < neuron_channel_number; i++) {
        for (int j = 0; j < num_bin; j++) {
            fout << neuron_mat[i*num_bin + j] << ' ';
        }
        fout << std::endl;
    }
    for (int i = 0; i < motion_channel_number; i++) {
        for (int j = 0; j < num_bin; j++) {
            fout << motion_mat[i*num_bin + j] << ' ';
        }
        fout << std::endl;
    }
    fout.close();
    // MatlabHelper::save_data(mat_filename, num_bin, neuron_channel_number, motion_channel_number, &neuron_mat[0], &motion_mat[0]);
    return 0;
}

void PlexonData::Train(const std::string &data_filename, int num_epo, int workmode) {
    MatlabHelper::train_ecog(data_filename + "_ecog.txt", 64, 193, 1, 1);
    // MatlabHelper::train_spike(data_filename + "_ecog.txt", num_epo, workmode);
}

bool PlexonData::Test(const std::string &model_filename, int neuron_channel_start, int neuron_channel_number, int motion_channel_number, int num_epo, std::vector<double> &res) {
    // if successfully test , return true
    // else (the sampled data is too short) return false
    int sampled_bin_size = (int)sampleDataStatistics.size();
    int start_bin_index = sampled_bin_size - num_epo - 1;
    if (start_bin_index < 0) return false;
    std::vector<double> neuron_mat(num_epo * neuron_channel_number);
    std::vector<double> predicted_res(motion_channel_number);

    for (int bin_index = 0; bin_index < num_epo; bin_index++) {
        for (int i = 0; i < neuron_channel_number; i++){
            neuron_mat[i*num_epo + bin_index] = sampleDataStatistics[bin_index + start_bin_index].val[neuron_channel_start + i];
        }
    }
    MatlabHelper::test_spike(&neuron_mat[0], num_epo, neuron_channel_number, motion_channel_number, model_filename, predicted_res);
    return true;
}
