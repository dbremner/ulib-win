#include "resource.h"

#include <windows.h>
#include <tchar.h>

#include "udialogx.h"
#include "ucontrol.h"
#include "ustatic.h"
#include "udlgapp.h"

#include "colors.h"

#include "ulcd.h"
#include "uled.h"

using huys::UDialogBox;

BOOL g_bShow = FALSE;

class USubStatic : public UStatic
{
public:
    USubStatic(HWND hParent, UINT nResource, HINSTANCE hInst)
        : UStatic(hParent, nResource, hInst)
    {
        m_dwStyles &= ~SS_SIMPLE;
    }

    virtual BOOL onLButtonDown(WPARAM,LPARAM)
    {
        ::MessageBox(m_hSelf, "LBUTTONDOWN", "info", MB_OK);
        return TRUE;
    }

    virtual BOOL onRButtonDown(WPARAM,LPARAM)
    {
        ::MessageBox(m_hSelf, "RBUTTONDOWN", "info", MB_OK);
        return TRUE;
    }

    virtual BOOL onShow(WPARAM,LPARAM)
    {
        //::MessageBox(m_hSelf, "SHOWWINDOW", "info", MB_OK);
        return TRUE;
    }
};

class UIconStatic : public UStatic
{
public:
    UIconStatic(HWND hParent, UINT nResource, HINSTANCE hInst)
    : UStatic(hParent, nResource, hInst)
    {
         m_dwStyles &= ~SS_SIMPLE;
         m_dwStyles |= WS_BORDER | SS_ICON | SS_REALSIZEIMAGE;
    }

    BOOL setIcon(UINT nID)
    {
        this->sendMsg(STM_SETICON, (WPARAM)LoadIcon(m_hInstance, MAKEINTRESOURCE(nID)));
        return TRUE;
    }
};

class UTransStatic : public UStatic
{
public:
    UTransStatic(HWND hParent, UINT nResource, HINSTANCE hInst)
        : UStatic(hParent, nResource, hInst)
    {
        m_dwStyles &= ~SS_SIMPLE;
        m_dwStyles |= SS_CENTER | SS_CENTERIMAGE;
    }

    virtual BOOL create()
    {
        BOOL bRet = UStatic::create();
        this->subclassProc();
        return  bRet;
    }

    BOOL onMessage( UINT message, WPARAM wParam, LPARAM lParam )
    {
        BOOL bRet = UStatic::onMessage(message, wParam, lParam);

        if (WM_SETTEXT == message)
        {
            onSetText();
        }

        return bRet;
    }

    virtual BOOL onCtrlColor(WPARAM wParam, LPARAM lParam)
    {
        HDC hdc = (HDC)wParam;
        ::SetBkMode(hdc, TRANSPARENT);

        return (BOOL)(HBRUSH)GetStockObject(NULL_BRUSH);
    }
private:
    void onSetText()
    {
        RECT rc;
        ::GetWindowRect(m_hSelf, &rc);
        ::ScreenToClient(m_hParent,(LPPOINT) &rc);
		//::ScreenToClient(m_hParent,(LPPOINT) (&rc+1));
        ::InvalidateRect(m_hParent, &rc, TRUE);
        ::UpdateWindow(m_hParent);
    }
};

class UBevelLine : public UStatic
{
    enum {
        LineWidth   = 1,
        LineHeight  = 1
    };
public:
    UBevelLine(HWND hParent)
        : UStatic(hParent, IDC_STATIC, ::GetModuleHandle(NULL))
    {
        m_dwStyles &= ~SS_SIMPLE;
    }

    ~UBevelLine()
    {}

    virtual BOOL create()
    {
        BOOL bRet = UStatic::create();
        this->subclassProc();
        return  bRet;
    }

	virtual BOOL onPaint()
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(m_hSelf, &ps);
	
        RECT rc;

        this->getClientRect(&rc);

        DWORD hiCol = ::GetSysColor(!m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW);
        DWORD loCol = ::GetSysColor(m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW);

        if(rc.bottom > rc.right)
        {
            // vertical
            rc.right /= 2;
            rc.left = rc.right - LineWidth;
            rc.right += LineWidth;
        }
        else
        {
            // horizontal
            rc.bottom /= 2;
            rc.top = rc.bottom - LineHeight;
            rc.bottom += LineHeight;
        }

        // CPaintDC::Draw3dRect(hdc, &rc,hiCol,loCol);
        int x = rc.left;
        int y = rc.top;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        FillSolidRect(hdc, x, y, cx - 1, 1, hiCol);
        FillSolidRect(hdc, x, y, 1, cy - 1, hiCol);
        FillSolidRect(hdc, x + cx, y, -1, cy, loCol);
        FillSolidRect(hdc, x, y + cy, cx, -1, loCol);

		EndPaint(m_hSelf, &ps);

		return FALSE;

    }
private:
    bool m_bSunken;

    void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
    {
        //ASSERT(this);
        //ASSERT(hdc != NULL);

        ::SetBkColor(hdc, clr);
        RECT rect = { x, y, x + cx, y + cy};
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
    }
};

class UDialogExt : public UDialogBox
{
    enum {
        IDC_STATIC_SUB  = 10111,
        IDC_STATIC_ICON = 10112,
        IDC_STATIC_LCD  = 10113,
        IDC_STATIC_LED  = 10114,
        IDC_STATIC_TRANS = 10115
    };
public:
    UDialogExt(HINSTANCE hInst, UINT nID)
        : UDialogBox(hInst, nID),
          m_pSubStatic(NULL),
          m_pIconStatic(NULL),
          m_pBevelLine(NULL),
          m_pLCDCtrl(NULL),
          m_pLEDCtrl(NULL),
          m_pTrans(NULL)
    {
        m_hBkBrush = (HBRUSH)::CreateSolidBrush(huys::green);
    }

    ~UDialogExt()
    {
        CHECK_PTR(m_pSubStatic);
        CHECK_PTR(m_pIconStatic);
        CHECK_PTR(m_pBevelLine);
        CHECK_PTR(m_pLCDCtrl);
        CHECK_PTR(m_pLEDCtrl);
        CHECK_PTR(m_pTrans);

        if (m_hBkBrush)
            ::DeleteObject(m_hBkBrush);
    }

    virtual BOOL onInit()
    {
        m_pSubStatic = new USubStatic(m_hDlg, IDC_STATIC_SUB, m_hInst);
        RECT rc = {20, 20, 160, 160};
        m_pSubStatic->setStyles(WS_BORDER|SS_NOTIFY);
        m_pSubStatic->create();
        m_pSubStatic->setWindowText(_T("&Hello from STATIC New line"));
        m_pSubStatic->setPosition(&rc);
        m_pSubStatic->disable();
        m_pSubStatic->hide();
        m_pSubStatic->subclassProc();

        m_pIconStatic = new UIconStatic(m_hDlg, IDC_STATIC_ICON, m_hInst);
        RECT ico_rc = {200, 200, 250, 250};
        m_pIconStatic->create();
        m_pIconStatic->setPosition(&ico_rc);
        m_pIconStatic->setIcon(IDI_APP);

        RECT trans_rc = {270, 200, 320, 250};
        m_pTrans = new UTransStatic(m_hDlg, IDC_STATIC_TRANS, m_hInst);
        m_pTrans->create();
        m_pTrans->setPosition(&trans_rc);
        m_pTrans->setWindowText(_T("trans"));


        RECT rcLine = {320, 20, 330, 260};
        m_pBevelLine = new UBevelLine(m_hDlg);
        m_pBevelLine->create();
        m_pBevelLine->setPosition(&rcLine);

        RECT rcLCD = {360, 40, 460, 200};
        m_pLCDCtrl = new ULCDCtrl(m_hDlg, IDC_STATIC_LCD);
        m_pLCDCtrl->create();
        m_pLCDCtrl->setPosition(&rcLCD);


        RECT rcLED = {360, 220, 460, 260};
        m_pLEDCtrl = new ULEDCtrl(m_hDlg, IDC_STATIC_LED);
        m_pLEDCtrl->setStyles(WS_BORDER | WS_THICKFRAME);
        m_pLEDCtrl->create();
        m_pLEDCtrl->setPosition(&rcLED);

        return TRUE;
    }

    virtual BOOL onLButtonDown(WPARAM wParam, LPARAM lParam)
    {
        if(!g_bShow)
        {
            m_pSubStatic->enable();
            m_pSubStatic->show();
            g_bShow = TRUE;
        } else {
            m_pSubStatic->disable();
            m_pSubStatic->hide();
            g_bShow = FALSE;
        }

        return FALSE;
    }

    virtual BOOL DialogProc(UINT message, WPARAM wParam, LPARAM lParam)
    {

        //BOOL result = UDialogBox::DialogProc(message, wParam, lParam);
        //HBRUSH hBrushBK = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);
        switch (message)
        {
        case WM_CTLCOLORDLG:
                return (BOOL)m_hBkBrush;
        case WM_TCHANGE:
            return this->onTChange(wParam, lParam);
        default:
            return UDialogBox::DialogProc(message, wParam, lParam);
        }
    }

    BOOL onTChange(WPARAM wParam, LPARAM lParam)
    {
        switch(wParam)
        {
        case IDC_STATIC_LCD:
            break;
        default:
            return FALSE;
        }
        return FALSE;
    }
 private:
    USubStatic *m_pSubStatic;
    UIconStatic *m_pIconStatic;
    UBevelLine *m_pBevelLine;
    ULCDCtrl *m_pLCDCtrl;
    ULEDCtrl *m_pLEDCtrl;
    UTransStatic *m_pTrans;

    HBRUSH m_hBkBrush;
};

UDLGAPP_T(UDialogExt, IDD_TEST);

