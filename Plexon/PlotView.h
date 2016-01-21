#pragma once

#include <vector>
// PlotView
#define MYWNDCLASS  L"PlotView"

class PlotView : public CWnd {
	DECLARE_DYNAMIC(PlotView)

public:
	PlotView();
	virtual ~PlotView();
    BOOL RegisterWndClass();

    inline void SetSize(int w, int ch, int mh, int totalChannel);
    inline void SetValue(int timeStamp, int channel, int value);
    inline void SetCurrentTimeStamp(int time);
    inline void SetDisplayChannelNumber(int c);
    inline void ScaleUp() {scale *= 2;}
    inline void ScaleDown() {scale = max(0.01, scale / 2.0);}

    inline int GetWidth() const;
    inline int GetHeight() const;
    inline int GetTotalChannelNumber() const;
    inline int GetDisplayChannelNumber() const;
    
    void Init();
    virtual void Draw(CDC * dc);			// Draw the entire plot
    virtual void DrawBasic(CDC * dc);		// Draw plot basics
    virtual void DrawSingleChannelPlot(int offsetX, int offsetY, int channel, CDC * dc);		// Draw the plot series
     virtual void DrawSerie(int offsetX, int offsetY, int channel, CDC *dc);
    virtual void DrawGrid(int offsetX, int offsetY, CDC * dc);		// Draw grids
    virtual void DrawFrame(int offsetX, int offsetY, CDC * dc);		// Draw border
    virtual void DrawCursor(int offsetX, int offsetY, CDC * dc);		// Draw cursor
    virtual void DrawLegend(int offsetX, int offsetY, int channel, CDC * dc);		// Draw legend


//     virtual void DrawYAxisGrid(CDC * dc);
//     virtual void DrawXAxisGrid(CDC * dc);
// 
//     virtual void Reset();

    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
    DECLARE_MESSAGE_MAP()

    int width, channelHeight, marginHeight;
    int totalChannelNumber;
    int displayChannelNumber;
    int cursorPos;
    double scale;
    std::vector<std::vector<int> > valueBuf;
    std::vector<std::vector<CPoint> > pointArray;

    CFont legendFont;
    COLORREF bgColor;
    COLORREF cursorColor;
    COLORREF serieColor;
    COLORREF gridColor;
    COLORREF frameColor;
    CRect widgetRect;
    bool hasWidgetBorder;
};

inline void PlotView::SetSize(int w, int ch, int mh, int totalChannel) {
    width = w;
    channelHeight = ch;
    marginHeight = mh;
    totalChannelNumber = totalChannel;

    valueBuf.resize(totalChannelNumber);
    pointArray.resize(totalChannelNumber);
    for (int i = 0; i < totalChannelNumber; i++) {
        valueBuf[i].resize(width);
        pointArray[i].resize(width);
    }
}

inline void PlotView::SetDisplayChannelNumber(int c) { displayChannelNumber = c;}

inline void PlotView::SetValue(int timeStamp, int channel, int value) {
    int index = timeStamp % width;
    valueBuf[channel][index] = value;
}

inline void PlotView::SetCurrentTimeStamp(int time) { cursorPos = time % width; }

inline int PlotView::GetHeight() const { return channelHeight; }

inline int PlotView::GetWidth() const { return width; }

inline int PlotView::GetTotalChannelNumber() const { return totalChannelNumber; }

inline int PlotView::GetDisplayChannelNumber() const { return displayChannelNumber; }

