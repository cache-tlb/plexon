#pragma once

#ifdef USINGMATLAB
#include "Matlab/MatLib.h"
#endif
#include <vector>

class MatlabHelper {
public:
    static void row_major_to_column_major(double *src, double *dst, int rows, int cols);
    static void row_major_to_column_major(std::vector<double> &src, std::vector<double> &dst, int rows, int cols);
    static void save_data(const std::string &filename, int num_bin, int num_neuron_channel, int num_motion_channel, double *neuron_data, double *motion_data);
    static void train_ecog(const std::string &data_filename, int max_routes, int index_triger, int sample_rate_org, int n_down);
    static void test_spike(double *neuron_data, int num_bin, int num_neuron_channel, int num_motion_channel, const std::string &model_filename, std::vector<double> &res);
    static void test_ecog(double *ecog_data, int rows, int cols, const std::string &model_path, std::vector<double> &res);
    static void train_spike(const std::string &data_filename, int num_epo, int workmode);
};

