#pragma once


// GraphicsView
#define GRAPHICSVIEWCLASS  L"GraphicsView"

class GraphicsView : public CWnd {
	DECLARE_DYNAMIC(GraphicsView)

public:
	GraphicsView();
	virtual ~GraphicsView();

    BOOL RegisterWndClass();
    void Init();
    void Draw(CDC *dc);
    void DrawBorder(CDC *dc);
    void DrawDirection(CDC *dc);

    COLORREF borderColor;
    CRect widgetRect;
    
    CImage imgLeft, imgRight, imgUp, imgDown;
    CRect rectLeft, rectRight, rectUp, rectDown;
    bool showLeft, showRight, showUp, showDown;

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
};


