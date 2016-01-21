// GraphicsView.cpp : 实现文件
//

#include "stdafx.h"
#include "Plexon.h"
#include "GraphicsView.h"
#include "MemDC.h"

// GraphicsView

IMPLEMENT_DYNAMIC(GraphicsView, CWnd)

GraphicsView::GraphicsView() {
    RegisterWndClass();

    borderColor = RGB(0,0,0);

    imgLeft.Load(L"Images/left.png");
    imgRight.Load(L"Images/right.png");
    imgUp.Load(L"Images/up.png");
    imgDown.Load(L"Images/down.png");
}

GraphicsView::~GraphicsView() { }

BOOL GraphicsView::RegisterWndClass()  { 
    WNDCLASS windowclass; 
    HINSTANCE hInst = AfxGetInstanceHandle();   

    //Check weather the class is registered already  
    if (!(::GetClassInfo ( hInst, GRAPHICSVIEWCLASS, &windowclass))) {       
        //If not then we have to register the new class
        windowclass.style = CS_DBLCLKS; // | CS_HREDRAW | CS_VREDRAW;
        windowclass.lpfnWndProc = ::DefWindowProc;
        windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
        windowclass.hInstance = hInst;
        windowclass.hIcon = NULL;
        windowclass.hCursor = AfxGetApp()->LoadStandardCursor ( IDC_ARROW );
        windowclass.hbrBackground = ::GetSysColorBrush( COLOR_WINDOW );
        windowclass.lpszMenuName = NULL;
        windowclass.lpszClassName = GRAPHICSVIEWCLASS;

        if (!AfxRegisterClass(&windowclass)) {
            AfxThrowResourceException();
            return FALSE;
        }
    }
    return TRUE;
}

void GraphicsView::Init() {
    CRect rect;
    GetWindowRect(&rect);
    widgetRect = CRect(0, 0, rect.Width(), rect.Height());

    int w = rect.Width(), h = rect.Height();
    int margin = 10;
    int img_size = rect.Width() / 4;
    int x1 = margin, x2 = margin + img_size, x3 = w / 2 - img_size / 2, x4 = w / 2 + img_size / 2, x5 = w - margin - img_size, x6 = w - margin;
    int y1 = margin, y2 = margin + img_size, y3 = h / 2 - img_size / 2, y4 = h / 2 + img_size / 2, y5 = h - margin - img_size, y6 = h - margin;
    rectLeft = CRect(x1, y3, x2, y4);
    rectRight = CRect(x5, y3, x6, y4);
    rectUp = CRect(x3, y1, x4, y2);
    rectDown = CRect(x3, y5, x4, y6);

    showLeft = true;
    showRight = true;
    showUp = true;
    showDown = true;

}

BEGIN_MESSAGE_MAP(GraphicsView, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// GraphicsView 消息处理程序

void GraphicsView::OnPaint() {
    CPaintDC dc(this); // device context for painting
    MemDC pdc(&dc);
    Draw(&pdc);
}

void GraphicsView::Draw(CDC *dc) {
    DrawBorder(dc);
    DrawDirection(dc);
}

void GraphicsView::DrawBorder(CDC *dc) {
    CPen penBorderColor;
    penBorderColor.CreatePen(PS_SOLID, 1, borderColor);
    CPen *oldPen = dc->SelectObject(&penBorderColor);

    dc->Rectangle(widgetRect);

    dc->SelectObject(oldPen);
}

void GraphicsView::DrawDirection(CDC *dc) {
    HDC hdc = *dc;
    if (showLeft)
        imgLeft.Draw(hdc, rectLeft);
    if (showRight)
        imgRight.Draw(hdc, rectRight);
    if (showUp)
        imgUp.Draw(hdc, rectUp);
    if (showDown)
        imgDown.Draw(hdc, rectDown);
}


