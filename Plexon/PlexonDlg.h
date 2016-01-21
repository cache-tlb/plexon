
// PlexonDlg.h : 头文件
//

#pragma once
#include <vector>
#include "PlotView.h"
#include "GraphicsView.h"
#include "PlexonData.h"
#include "MatlabHelper.h"
#include<WinSock2.h>

// CPlexonDlg 对话框
class CPlexonDlg : public CDialogEx
{
// 构造
public:
	CPlexonDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CPlexonDlg();
// 对话框数据
	enum { IDD = IDD_PLEXON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
    PlotView *plotView;
    GraphicsView *actualDirectionView;
    GraphicsView *predictedDirectionView;
    PlexonData *dataProvider;

    UINT_PTR plotTimerId;
    UINT_PTR dataReadTimerId;
	UINT_PTR debugTimerId;
    UINT_PTR actualDirectionTimerId;
    UINT_PTR predictedDirectionTimerId;

    bool isSampling;
    bool isTesting;
    std::string model_filename;
    int num_epo;
    int count;
    int plotChannelStart;
    SOCKET sock_client;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonConnect();
    afx_msg void OnBnClickedButtonStartSample();
    afx_msg void OnBnClickedButtonStopSample();
    afx_msg void OnCbnSelchangeComboChannelSet();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedButtonSaveAndTrain();
    int neuron_channel_start;
    int neuron_channel_number;
    int motion_channel_start;
    int motion_channel_number;
    int window_size;
    afx_msg void OnBnClickedButtonScaleTwice();
    afx_msg void OnBnClickedButtonScaleHalf();
    afx_msg void OnBnClickedButtonStartTest();
    afx_msg void OnBnClickedButtonEndTest();
    CString ipaddress;
    short port;
    afx_msg void OnBnClickedButtonConnectTcp();
    afx_msg void OnBnClickedButtonTest1();
    afx_msg void OnBnClickedButtonTest2();
};
