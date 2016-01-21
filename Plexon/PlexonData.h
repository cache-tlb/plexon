#pragma once

#include "Plexon.h"
#include <vector>
#include <map>

#define MAX_MAP_EVENTS_PER_READ 500000
#define MAX_SPIKE_CHANNEL_NUMBER 64
#define MAX_MOTION_CHANNEL_NUMBER 64
#define ANALOG_CHANNEL_BASE 192
#define TOTAL_CHANNEL_NUMBER 224

typedef struct BatchStatType {
    std::vector<unsigned int > val; 
    BatchStatType() {
        val.resize(MAX_SPIKE_CHANNEL_NUMBER + MAX_SPIKE_CHANNEL_NUMBER + MAX_MOTION_CHANNEL_NUMBER, 0);
    }
} BatchStatType;
typedef struct TimeValueType {
    unsigned long timestamp;
    short value;
    TimeValueType() : timestamp(0), value(0) {}
    TimeValueType(unsigned long t, short v) : timestamp(t), value(v) {}
    bool operator < (const TimeValueType & that) {
        return (timestamp < that.timestamp) || (timestamp == that.timestamp && value < that.value);
    }
} TimeValueType;

struct SpikeDataType {
    unsigned long TimeStamp;
    short Unit;
    short Channel;
    SpikeDataType() : TimeStamp(0), Unit(-1), Channel(-1) {}
    SpikeDataType(unsigned long _TimeStamp, short _Unit, short _Channel) 
        : TimeStamp(_TimeStamp), Unit(_Unit), Channel(_Channel) {}
};

class PlexonData {
public:
    PlexonData(CDialogEx *parent);
    ~PlexonData(void);

    bool Init();
    bool ConnectDataServer();
	inline void GetGain();
    inline void SetWindowSize(int ms);
    void SetupInitBatchNumber();
    inline void ClearData();
    int SaveData(const std::string &mat_filename, int neuron_channel_start, int neuron_channel_number, int motion_channel_start, int motion_channel_number);
    void Train(const std::string &data_filename, int num_epo, int workmode);
    bool Test(const std::string &model_filename, int neuron_channel_start, int neuron_channel_number, int motion_channel_number, int num_epo, std::vector<double> &res);
    int ReadSpike();
	inline int GetSampleRate() const { return mapSampleRate;}

	std::vector<int> gains;
    std::vector<std::vector<SpikeDataType> > spikeDataBatch;        // original spike data
    
    // for visualize
    std::vector<BatchStatType> sampleDataStatistics;
    std::vector<BatchStatType> sampleDataSum; 
    std::vector<BatchStatType> sampleDataCnt;

    // for save
    std::vector<std::vector<TimeValueType> > dataForSave;

protected:
    CDialogEx *mainWindow;
    PL_WaveLong *pServerEventBuffer;

    int windowSizeInMs;  // in millisecond, for vis
    int windowSizeInTimeStamp; // in timeStamp. time_in_second = TimeStamps / SampleRate
    int mapSampleRate;
    int initalBatchNumber;
};

inline void PlexonData::SetWindowSize(int ms) {
    windowSizeInMs = ms;
}

inline void PlexonData::ClearData() {
    spikeDataBatch.clear();
    sampleDataStatistics.clear();
    sampleDataSum.clear();
    sampleDataCnt.clear();

    dataForSave.clear();
    dataForSave.resize(TOTAL_CHANNEL_NUMBER);
}

inline void PlexonData::GetGain() {
	gains.resize(256);
	PL_GetGain(&gains[0]);
}
