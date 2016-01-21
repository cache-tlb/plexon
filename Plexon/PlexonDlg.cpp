
// PlexonDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PlexonApp.h"
#include "PlexonDlg.h"
#include "afxdialogex.h"
#include <cmath>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "ws2_32.lib")
// CPlexonDlg 对话框



CPlexonDlg::CPlexonDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CPlexonDlg::IDD, pParent),
    isSampling(false),
    isTesting(false), 
    num_epo(20),
    plotChannelStart(0),
    neuron_channel_start(0), 
    neuron_channel_number(0), 
    motion_channel_start(0), 
    motion_channel_number(0), 
    window_size(0),
    ipaddress(_T("127.0.0.1")), 
    port(4000)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    plotView = new PlotView;
    actualDirectionView = new GraphicsView;
    predictedDirectionView = new GraphicsView;
    count = 0;
}

CPlexonDlg::~CPlexonDlg() {
    closesocket(sock_client);
    WSACleanup();
}

void CPlexonDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NEURON_CHANNEL_START, neuron_channel_start);
    DDV_MinMaxInt(pDX, neuron_channel_start, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_NEURON_CHANNEL_NUMBER, neuron_channel_number);
    DDV_MinMaxInt(pDX, neuron_channel_number, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_MOTION_CHANNEL_START, motion_channel_start);
    DDV_MinMaxInt(pDX, motion_channel_start, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_MOTION_CHANNEL_NUMBER, motion_channel_number);
    DDV_MinMaxInt(pDX, motion_channel_number, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_WINDOW_SIZE, window_size);
    DDV_MinMaxInt(pDX, window_size, 0, 10000000);
    DDX_Text(pDX, IDC_EDIT_IPADDRESS, ipaddress);
    DDX_Text(pDX, IDC_EDIT_PORT, port);
}

BEGIN_MESSAGE_MAP(CPlexonDlg, CDialogEx)
	ON_WM_PAINT()
    ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CPlexonDlg::OnBnClickedButtonConnect)
    ON_BN_CLICKED(IDC_BUTTON_START_SAMPLE, &CPlexonDlg::OnBnClickedButtonStartSample)
    ON_BN_CLICKED(IDC_BUTTON_STOP_SAMPLE, &CPlexonDlg::OnBnClickedButtonStopSample)
    ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL_SET, &CPlexonDlg::OnCbnSelchangeComboChannelSet)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SAVE_AND_TRAIN, &CPlexonDlg::OnBnClickedButtonSaveAndTrain)
    ON_BN_CLICKED(IDC_BUTTON_SCALE_TWICE, &CPlexonDlg::OnBnClickedButtonScaleTwice)
    ON_BN_CLICKED(IDC_BUTTON_SCALE_HALF, &CPlexonDlg::OnBnClickedButtonScaleHalf)
    ON_BN_CLICKED(IDC_BUTTON_START_TEST, &CPlexonDlg::OnBnClickedButtonStartTest)
    ON_BN_CLICKED(IDC_BUTTON_END_TEST, &CPlexonDlg::OnBnClickedButtonEndTest)
    ON_BN_CLICKED(IDC_BUTTON_CONNECT_TCP, &CPlexonDlg::OnBnClickedButtonConnectTcp)
    ON_BN_CLICKED(IDC_BUTTON_TEST1, &CPlexonDlg::OnBnClickedButtonTest1)
    ON_BN_CLICKED(IDC_BUTTON_TEST2, &CPlexonDlg::OnBnClickedButtonTest2)
END_MESSAGE_MAP()


// CPlexonDlg 消息处理程序

BOOL CPlexonDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
    
	// TODO: 在此添加额外的初始化代码
    plotView->SubclassDlgItem(IDC_Canvas, this);
    actualDirectionView->SubclassDlgItem(IDC_ACTUAL_DIRECTION, this);
    predictedDirectionView->SubclassDlgItem(IDC_PREDICTED_DIRECTION, this);

    plotView->SetSize(800, 78, 0, 64 + 8);
    plotView->SetDisplayChannelNumber(8);
    plotView->Init();

    actualDirectionView->Init();
    predictedDirectionView->Init();

    dataProvider = new PlexonData(this);
    dataProvider->Init();
    dataProvider->SetWindowSize(50);

    plotTimerId = SetTimer(1,50, NULL);
    dataReadTimerId = SetTimer(2, 10, NULL);
    actualDirectionTimerId = SetTimer(3, 50, NULL);
    predictedDirectionTimerId = SetTimer(4, 50, NULL);
	debugTimerId = SetTimer(5, 1000, NULL);

    neuron_channel_start = 0;
    neuron_channel_number = 16;
    motion_channel_start = MAX_SPIKE_CHANNEL_NUMBER;
    motion_channel_number = 5;
    window_size = 50;
#ifdef USINGMATLAB
    MatLibInitialize();
#endif
    UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CPlexonDlg::OnPaint() {
	if (IsIconic()) {}
	else {
		CDialogEx::OnPaint();
	}
    
}

void CPlexonDlg::OnTimer(UINT nIDEvent) {
    if (nIDEvent == plotTimerId && isSampling){
        if (dataProvider->sampleDataStatistics.size() > 1){
            count++;
            int n = dataProvider->sampleDataStatistics.back().val.size();
            for (int i = 0; i < plotView->GetDisplayChannelNumber(); i++) {
                int channel_index = i + plotChannelStart;
                int val = (channel_index >= n ? 0 : dataProvider->sampleDataStatistics[dataProvider->sampleDataStatistics.size() - 2].val[channel_index] );
                plotView->SetValue(count, i, val);
            }
            plotView->SetCurrentTimeStamp(count);
            plotView->Invalidate(FALSE);

            // set show left/right/up/down based on the sample_data
            actualDirectionView->showUp = false;
            actualDirectionView->showDown = false;
            actualDirectionView->showLeft = false;
            actualDirectionView->showRight = false;
            unsigned int max_val = dataProvider->sampleDataStatistics[dataProvider->sampleDataStatistics.size() - 2].val[motion_channel_start + 0];
            int max_pos = 0;
            for (int i = 1; i < 3; i++) {
                if (dataProvider->sampleDataStatistics[dataProvider->sampleDataStatistics.size() - 2].val[motion_channel_start + i] > max_val) {
                    max_val = dataProvider->sampleDataStatistics[dataProvider->sampleDataStatistics.size() - 2].val[motion_channel_start + i];
                    max_pos = i;
                }
            }
            switch (max_pos) {
            case 0:
                actualDirectionView->showUp = true;
                break;
            case 1:
                actualDirectionView->showDown = true;
                break;
            case 2:
                actualDirectionView->showLeft = true;
                break;
            case 3:
                actualDirectionView->showRight = true;
                break;
            default:
                break;
            }
            actualDirectionView->Invalidate(FALSE);
        }
    } else if (nIDEvent == dataReadTimerId && isSampling) {
        dataProvider->ReadSpike();
	} else if (nIDEvent == debugTimerId && isSampling){
		//CString txt;
		//txt.Format(L"sampleDataStatistics.size is %d", dataProvider->sampleDataStatistics.size());
		//MessageBox(txt);
	} else if (nIDEvent == predictedDirectionTimerId && isSampling && isTesting) {
        // run predict procedure
        std::vector<double> res;
        bool success = dataProvider->Test(model_filename, neuron_channel_start, neuron_channel_number, motion_channel_number, num_epo, res);
        if (!success) return;
        // update the graph
        double max_val = res[0];
        int max_pos = 0;
        for (int i = 1; i < res.size(); i++) {
            if (res[i] > max_val) {
                max_val = res[i];
                max_pos = i;
            }
        }
        predictedDirectionView->showLeft = false;
        predictedDirectionView->showRight = false;
        predictedDirectionView->showUp = false;
        predictedDirectionView->showDown = false;

        switch (max_pos) {
        case 0:
            predictedDirectionView->showUp = true; break;
        case 1:
            predictedDirectionView->showDown= true; break;
        case 2:
            predictedDirectionView->showLeft = true; break;
        case 3:
            predictedDirectionView->showRight = true; break;
        case 4:
            break;
        default:
            break;
        }
        predictedDirectionView->Invalidate(FALSE);
    } else{
        //MessageBox(L"Error");
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CPlexonDlg::OnDestroy() {
    KillTimer(plotTimerId);
#ifdef USINGMATLAB
    MatLibTerminate();
#endif
}


void CPlexonDlg::OnBnClickedButtonConnect()
{
    dataProvider->ConnectDataServer();
	CString txt;
	txt.Format(L"连接成功,采样率为: %d", dataProvider->GetSampleRate());
	MessageBox(txt);
}


void CPlexonDlg::OnBnClickedButtonStartSample() {
    UpdateData(TRUE);
    dataProvider->ClearData();
    dataProvider->SetWindowSize(window_size);
    dataProvider->SetupInitBatchNumber();

    GetDlgItem(IDC_BUTTON_START_SAMPLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_WINDOW_SIZE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_SAVE_DATA)->EnableWindow(FALSE);
    isSampling = true;
}


void CPlexonDlg::OnBnClickedButtonStopSample() {
    isSampling = false;

    GetDlgItem(IDC_BUTTON_START_SAMPLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_WINDOW_SIZE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_SAVE_DATA)->EnableWindow(TRUE);
}


void CPlexonDlg::OnCbnSelchangeComboChannelSet() {
    int index = ((CComboBox*)GetDlgItem(IDC_COMBO_CHANNEL_SET))->GetCurSel();
    plotChannelStart = index * plotView->GetDisplayChannelNumber();
}


void CPlexonDlg::OnSize(UINT nType, int cx, int cy) {
    CDialogEx::OnSize(nType, cx, cy);
}


void CPlexonDlg::OnBnClickedButtonSaveAndTrain()
{
    //MessageBox(L"这个功能还没完成");
    OnBnClickedButtonStopSample();
    UpdateData(TRUE);
    // get the save file name

    CString ext("");
    CString filter("All Files (*.*)|*.*");
    CFileDialog dlgFile(FALSE, ext, NULL, 6UL, filter);
    
    if (dlgFile.DoModal() == IDOK) {
        CString path = dlgFile.GetPathName();
        CT2CA temp(path);
        std::string path_str(temp);
        /*if (path_str.substr(path_str.length() - 4, 4) != std::string(".mat")) {
            path_str = path_str + ".mat";
        }*/
        dataProvider->SaveData(path_str, neuron_channel_start, neuron_channel_number, motion_channel_start, motion_channel_number);
        // then run train procedure
        dataProvider->Train(path_str, num_epo, 1);
        model_filename = path_str.substr(0, path_str.length() - 4) + "_model.mat";
    }
}


void CPlexonDlg::OnBnClickedButtonScaleTwice() {
    plotView->ScaleUp();
}


void CPlexonDlg::OnBnClickedButtonScaleHalf() {
    plotView->ScaleDown();
}


void CPlexonDlg::OnBnClickedButtonStartTest() {
    if (!isSampling) {
        MessageBox(L"请先开始采样");
    } else if (!PathFileExists((CString)model_filename.c_str())) {
        MessageBox(L"模型文件不存在");
    } else {
        UpdateData(TRUE);
        isTesting = true;
    }
}


void CPlexonDlg::OnBnClickedButtonEndTest() {
    isTesting = false;
}


void CPlexonDlg::OnBnClickedButtonConnectTcp()
{
    UpdateData(TRUE);
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1,1);
    err=WSAStartup(wVersionRequested,&wsaData);
    if(err!=0){
        MessageBox(L"连接失败");
        return;
    }
    if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1) {
        WSACleanup();
        MessageBox(L"连接失败");
        return;
    }

    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    CT2CA pszConvertedAnsiString (ipaddress);
    std::string ip_str(pszConvertedAnsiString);
    addrSrv.sin_addr.S_un.S_addr = inet_addr(ip_str.c_str());
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    if (connect(sock_client,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR))) {
        MessageBox(L"连接失败");
    }
    else {
        MessageBox(L"连接成功");
    }
}


void CPlexonDlg::OnBnClickedButtonTest1()
{
    // TODO: 在此添加控件通知处理程序代码
    std::string send_buf = "1";
    send(sock_client,send_buf.c_str(),send_buf.length() + 1,0);
}


void CPlexonDlg::OnBnClickedButtonTest2()
{
    // TODO: 在此添加控件通知处理程序代码
    std::string send_buf = "2";
    send(sock_client,send_buf.c_str(),send_buf.length() + 1,0);
}
