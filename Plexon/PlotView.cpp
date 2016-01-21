// PlotView.cpp : 实现文件
//

#include "stdafx.h"
#include "Plexon.h"
#include "PlotView.h"
#include "MemDC.h"

// PlotView

IMPLEMENT_DYNAMIC(PlotView, CWnd)

PlotView::PlotView() : CWnd() {
    RegisterWndClass();
    cursorPos = 0;
    scale = 1.0;

    legendFont.CreateFont(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    bgColor = RGB(255, 255, 255);
    cursorColor = RGB(0, 0, 255);
    serieColor = RGB(0, 255, 0);
    gridColor = RGB(0, 0, 0);
    frameColor = RGB(80, 80, 80);
    hasWidgetBorder = false;
}

PlotView::~PlotView(){}

void PlotView::Init() {
    CRect wndRect;
    this->GetWindowRect(&wndRect);
    widgetRect = CRect(0, 0, wndRect.Width(), wndRect.Height());
}

BEGIN_MESSAGE_MAP(PlotView, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// PlotView 消息处理程序

BOOL PlotView::RegisterWndClass()  {  
    WNDCLASS windowclass ;
    HINSTANCE hInst = AfxGetInstanceHandle ();

    //Check weather the class is registerd already
    if (!(::GetClassInfo ( hInst, MYWNDCLASS , &windowclass ))) {
        //If not then we have to register the new class
        windowclass .style = CS_DBLCLKS; // | CS_HREDRAW | CS_VREDRAW;
        windowclass .lpfnWndProc = :: DefWindowProc;
        windowclass .cbClsExtra = windowclass. cbWndExtra = 0;
        windowclass .hInstance = hInst;
        windowclass .hIcon = NULL;
        windowclass .hCursor = AfxGetApp ()->LoadStandardCursor ( IDC_ARROW);
        windowclass .hbrBackground = ::GetSysColorBrush (COLOR_WINDOW );
        windowclass .lpszMenuName = NULL;
        windowclass .lpszClassName = MYWNDCLASS;

        if (!AfxRegisterClass (& windowclass)) {
            AfxThrowResourceException ();
            return FALSE ;
        }
    }

    return TRUE ;
}  


void PlotView::OnPaint() {
    CPaintDC dc(this); // device context for painting
    MemDC  pdc(&dc);  // non flickering painting
    Draw(&pdc);
}

void PlotView::DrawBasic(CDC * dc) {
    CBrush brushBgColor(bgColor);
    CPen penFrameColor;
    penFrameColor.CreatePen(PS_SOLID, 1, frameColor);
    CBrush *oldBrush = dc->SelectObject(&brushBgColor);
    CPen *oldPen = dc->SelectObject(&penFrameColor);

    if (hasWidgetBorder) {
        dc->Rectangle(widgetRect);
    } else {
        dc->FillRect(widgetRect, &brushBgColor);
    }

    dc->SelectObject(oldBrush);
    dc->SelectObject(oldPen);
}

void PlotView::DrawGrid(int offsetX, int offsetY, CDC * dc) {
    
}

void PlotView::DrawCursor(int offsetX, int offsetY, CDC * dc) {
    CPen penCursorColor;
    penCursorColor.CreatePen(PS_SOLID, 1, cursorColor);
    CPen *oldPen = dc->SelectObject(&penCursorColor);

    dc->MoveTo(cursorPos + offsetX, offsetY);
    dc->LineTo(cursorPos + offsetX, channelHeight + offsetY);
    
    dc->SelectObject(oldPen);
}

void PlotView::DrawFrame(int offsetX, int offsetY, CDC * dc) {
    CRect frameRect(offsetX, offsetY, offsetX + width, offsetY + channelHeight);
    CPen penFrameColor;
    penFrameColor.CreatePen(PS_SOLID, 1, frameColor);
    CPen *oldPen = dc->SelectObject(&penFrameColor);
    dc->Rectangle(frameRect);
    dc->SelectObject(oldPen);
}

void PlotView::DrawSerie(int offsetX, int offsetY, int channel, CDC *dc) {
    // the data
    CPen penSerieColor;
    penSerieColor.CreatePen(PS_SOLID, 1, serieColor);
    CPen *oldPen = dc->SelectObject(&penSerieColor);

    std::vector<CPoint> &channelPoints = pointArray[channel];
    std::vector<int> &channelData = valueBuf[channel];
    for (int i = 0; i < channelPoints.size(); i++) {
        channelPoints[i].x = i + offsetX;
        channelPoints[i].y = - channelData[i]*scale + offsetY + channelHeight - 2;
    }

    if (cursorPos > 0 && cursorPos < channelPoints.size() - 1) {
        dc->Polyline(&channelPoints[0], cursorPos);
        dc->Polyline(&channelPoints[cursorPos + 1], channelPoints.size() - cursorPos -1);
    } else {
        dc->Polyline(&channelPoints[0], channelPoints.size());
    }

    dc->SelectObject(oldPen);
}

void PlotView::DrawLegend(int offsetX, int offsetY, int channel, CDC *dc) {
    
}

void PlotView::DrawSingleChannelPlot(int offsetX, int offsetY, int channel, CDC * dc) {
    DrawGrid(offsetX, offsetY, dc);
    DrawFrame(offsetX, offsetY, dc);
    if (channel < totalChannelNumber) {
        DrawSerie(offsetX, offsetY, channel, dc);
        DrawCursor(offsetX, offsetY, dc);
        DrawLegend(offsetX, offsetY, channel, dc);
    }
}

void PlotView::Draw(CDC * dc) {
    DrawBasic(dc);
    for (int i = 0; i < displayChannelNumber; i++) {
        int offsetY = marginHeight / 2 + i * (marginHeight + channelHeight) + 2;
        int offsetX = 10;
        DrawSingleChannelPlot(offsetX, offsetY, i, dc);
    }
    return;
    /*CBrush currentBrush(RGB(255, 255, 255));
    CBrush *oldBrush = dc->SelectObject(&currentBrush);

    CPen blackPen;
    blackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    CPen *oldPen = dc->SelectObject(&blackPen);
    CRect wndRect;
    this->GetWindowRect(&wndRect);
    CRect bbox(0, 0, wndRect.Width(), wndRect.Height());
    dc->Rectangle(&bbox);
    for (int i = 0; i < pointArray[0].size(); i++) {
        pointArray[0][i].x = i;
        pointArray[0][i].y = - valueBuf[0][i] + channelHeight;
    }

    CPen greenPen;
    greenPen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    dc->SelectObject(&greenPen);
    if (cursorPos > 0 && cursorPos < width - 1) {
        dc->Polyline(&pointArray[0][0], cursorPos);
        dc->Polyline(&pointArray[0][cursorPos + 1], pointArray[0].size() - cursorPos -1);
    } else {
        dc->Polyline(&pointArray[0][0], pointArray[0].size());
    }
    CPen bluePen;
    bluePen.CreatePen(PS_SOLID, 1, RGB(0,0,255));
    dc->SelectObject(&bluePen);
    dc->MoveTo(cursorPos, 0);
    dc->LineTo(cursorPos, channelHeight);

    dc->SelectObject(oldPen);
    dc->SelectObject(oldBrush);*/
}


BOOL PlotView::OnEraseBkgnd(CDC* pDC) {
    return FALSE;
}

