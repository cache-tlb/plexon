#include "stdafx.h"
#include "MatlabHelper.h"

#ifdef USINGMATLAB
#pragma comment(lib, "./MatLab/MatLib.lib")
#pragma comment(lib, "mclmcrrt.lib")
#endif


void MatlabHelper::row_major_to_column_major(double *src, double*dst, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[j*rows + i] = src[i*cols + j];
        }
    }
}

void MatlabHelper::row_major_to_column_major(std::vector<double> &src, std::vector<double> &dst, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[j*rows + i] = src[i*cols + j];
        }
    }
}

void MatlabHelper::save_data(const std::string &filename, int num_bin, int num_neuron_channel, int num_motion_channel, double *neuron_data, double *motion_data) {
	return;
#ifdef USINGMATLAB
    /*mwArray neuron_mwarray(num_neuron_channel, num_bin, mxDOUBLE_CLASS, mxREAL);
    mwArray motion_mwarray(num_motion_channel, num_bin, mxDOUBLE_CLASS, mxREAL);

    double *neuron_data_col_major = new double[num_bin * num_neuron_channel];
    double *motion_data_col_major = new double[num_bin * num_motion_channel];
    row_major_to_column_major(neuron_data, neuron_data_col_major, num_neuron_channel, num_bin);
    row_major_to_column_major(motion_data, motion_data_col_major, num_motion_channel, num_bin);

    neuron_mwarray.SetData(neuron_data_col_major, num_bin*num_neuron_channel);
    motion_mwarray.SetData(motion_data_col_major, num_bin*num_motion_channel);
    
    mwArray str(filename.c_str());
    mwArray ret;
    
    SaveCapturedData(1, ret, neuron_mwarray, motion_mwarray, str);

    delete [] neuron_data_col_major;
    delete [] motion_data_col_major;*/
#else
	std::ofstream fout(filename);
	for (int i = 0; i < num_bin; i++) {
		for (int j = 0; j < num_neuron_channel; j++) {
			fout << neuron_data[j*num_bin + i] << ' ';
		}
		for (int j = 0; j < num_motion_channel; j++) {
			fout << motion_data[j*num_bin + i] << ' ';
		}
		fout << std::endl;
	}
	fout.close();
#endif
}

void MatlabHelper::train_ecog(const std::string &data_filename, int max_routes, int index_triger, int sample_rate_org, int n_down) {
#ifdef USINGMATLAB
	mwArray vecStrFileName(data_filename.c_str());
	mwArray MAXROUTES(max_routes);
	mwArray indTriger(index_triger);
	mwArray spRateOri(sample_rate_org);
	mwArray nDown(n_down);
	std::string model_filename = data_filename + ".model.mat";
	mwArray strResultFileName(model_filename.c_str());
	mwArray matRet;
	Mat_ECOG_Train(1, matRet, vecStrFileName, MAXROUTES, indTriger, spRateOri, nDown, strResultFileName);
	int ret;
	matRet.GetData(&ret, 1);
	if (ret == 0) {
		MessageBox(NULL, L"OK", L"INFO", MB_OK);
	} else {
		MessageBox(NULL, L"FAIL", L"INFO", MB_OK);
	}
#endif
}

void MatlabHelper::train_spike(const std::string &data_filename, int num_epo, int workmode) {
#ifdef USINGMATLAB
    mwArray data_name(data_filename.c_str());
    mwArray nEpo(num_epo);
    mwArray nWorkMode(workmode);
    mwArray fErr;
    Entry_Train(1, fErr, data_name, nEpo, nWorkMode);
#endif // USINGMATLAB
}

void MatlabHelper::test_spike(double *neuron_data, int num_epo, int num_neuron_channel, int num_motion_channel, const std::string &model_filename, std::vector<double> &res) {
#ifdef USINGMATLAB
    int rows = num_neuron_channel, cols = num_epo;
    double *test_data = new double[rows * cols];
    row_major_to_column_major(neuron_data, test_data, rows, cols);

    mwArray matSpikeCount(rows, cols, mxDOUBLE_CLASS, mxREAL);
    matSpikeCount.SetData(test_data, rows * cols);
    mwArray strFileName(model_filename.c_str());
    mwArray ret;

    Entry_Test(1, ret, matSpikeCount, strFileName);
    res.resize(num_motion_channel);
    ret.GetData(&res[0], num_motion_channel);
    delete [] test_data;
#endif // USINGMATLAB
}

void MatlabHelper::test_ecog(double *ecog_data, int rows, int cols, const std::string &model_path, std::vector<double> &res) {
#ifdef USINGMATLAB
    double *test_data = new double[rows *cols];
    row_major_to_column_major(ecog_data, test_data, rows, cols);

    mwArray mat_ecog(rows, cols, mxDOUBLE_CLASS, mxREAL);
    mat_ecog.SetData(test_data, rows * cols);
    mwArray strFileName(model_path.c_str());
    mwArray ret;

    Mat_ECOG_Test(1, ret, mat_ecog, strFileName);
    res.resize(1);
    ret.GetData(&res[0], 1);
    delete [] test_data;
#endif // USINGMATLAB
}
