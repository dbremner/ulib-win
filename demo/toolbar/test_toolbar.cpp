#include "resource.h"

#define _WIN32_IE 0x0400
#define WINVER    0x0500

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#include "udialogx.h"
#include "udlgapp.h"
#include "uimagelist.h"
#include "utoolbar.h"
#include "umsg.h"
#include "uedit.h"

using huys::UDialogBox;

const UINT ID_TOOLBAR = 13333;
const DWORD buttonStyles = TBSTYLE_AUTOSIZE; //BTNS_AUTOSIZE;

HWND hwndGoto = NULL;  // Window handle of dialog box

HWND hToolbar;

BOOL CALLBACK GoToProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //BOOL fError;

    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_CLOSE:
        DestroyWindow(hwndGoto);
        hwndGoto = NULL;
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            return TRUE;

        case IDCANCEL:
            DestroyWindow(hwndDlg);
            hwndGoto = NULL;
            return TRUE;
        }
    }
    return FALSE;
}

HWND hModalDlg = NULL;

BOOL CALLBACK ModalDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

class UModalDialog : public UDialogBox
{
public:
    UModalDialog(HINSTANCE hInst, UINT nID, HWND hParent)
        : UDialogBox(hInst, nID, ModalDlgProc)
    {
        this->setParentH(hParent);
        m_mode = U_DLG_MODAL;
    }
    virtual BOOL create()
    {
        return ::DialogBoxParam(m_hInst, MAKEINTRESOURCE(m_nDialogID), m_hParent, m_lpDialogFunc,
            (LPARAM)this);
    }

    virtual BOOL doModal()
    {
        return this->create();
    }
    void setHWND(HWND hDlg)
    {
        m_hDlg = hDlg;
    }
    BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch(LOWORD(wParam))
        {
        case IDM_EXIT:
            return this->destroy();
        default:
            return UDialogBox::onCommand(wParam, lParam);
        }
    }

    BOOL onLButtonDown(WPARAM, LPARAM)
    {
        showMsg(_T("xxxx"));
        return FALSE;
    }
};

BOOL CALLBACK ModalDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //BOOL fError;
    static UModalDialog *pDlg = NULL;

    switch (message)
    {
    case WM_INITDIALOG:
        pDlg = reinterpret_cast<UModalDialog *>(lParam);
        pDlg->setHWND(hwndDlg);
        return TRUE;
    case WM_SETFONT:
        return FALSE;
    default:
        return pDlg->DialogProc(message, wParam, lParam);
    }
}

class UDialogExt : public UDialogBox
{
public:
    UDialogExt(HINSTANCE hInst, UINT nID)
        : UDialogBox(hInst, nID)//, m_psub(0)//, uil(IDR_TOOLBAR1, hInst)
    {}

    ~UDialogExt()
    {
        CHECK_PTR(m_putl);
    }

    virtual BOOL onInit()
    {
        m_putl = new  UToolBar(m_hDlg, ID_TOOLBAR, m_hInst);
        m_putl->setStyles(TBSTYLE_FLAT );
        m_putl->create();

        static UImageList uil(IDR_TOOLBAR1, m_hInst);
        HIMAGELIST himl = uil.getHandle();
        m_putl->setImageList(himl);

        static UImageList uilhot(IDR_TOOLBAR1_HOT, m_hInst);
        m_putl->setHotImageList(uilhot.getHandle());


        m_putl->setExtendStyle(TBSTYLE_EX_DRAWDDARROWS);

        // Initialize button info.
        // IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.
        TBBUTTON tbButtons[4] =
        {
            { MAKELONG(0, 0), IDM_NEW, TBSTATE_ENABLED,
            buttonStyles | TBSTYLE_DROPDOWN, {0}, 0, (INT_PTR)"New" },
            { MAKELONG(1, 0), IDM_OPEN, TBSTATE_ENABLED,
            buttonStyles, {0}, 0, (INT_PTR)"Open"},
            { MAKELONG(2, 0), IDM_SAVE, 0,
            buttonStyles, {0}, 0, (INT_PTR)"Save"},
            { 100, 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, -1}
        };
        m_putl->addButtons(4, tbButtons);
        m_putl->autosize();
        m_putl->show();
        m_putl->enableButton(IDM_SAVE);

        hToolbar = m_putl->getHWND();

        DWORD dwSize = m_putl->getButtonSize();
        LONG lHeight = LOWORD(dwSize);
        LONG lWidth  = HIWORD(dwSize);

        UEdit uedt(hToolbar, 5433, m_hInst);
        uedt.create();
        RECT rcEdit = {
            lWidth*3+2,
            lHeight/4,
            lWidth*3+100,
            lHeight*3/4
        };
        uedt.setPosition(&rcEdit);

        //m_sub.setParentH(m_hDlg);
        //m_sub.create();
        //m_sub.show();

        MENUINFO mi = {0};
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
        mi.dwStyle = MNS_AUTODISMISS;
        mi.hbrBack = (HBRUSH)::GetStockObject(GRAY_BRUSH);

        HMENU hMenu = ::GetMenu(m_hDlg);

        ::SetMenuInfo(hMenu, &mi);

        return TRUE;
    }

    virtual BOOL DialogProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        BOOL result = UDialogBox::DialogProc(message, wParam, lParam);

        if (message == WM_NOTIFY)
        {

            LPNMTBCUSTOMDRAW lpNMCustomDraw = (LPNMTBCUSTOMDRAW) lParam;
            RECT rect;
            ::GetClientRect(hToolbar, &rect);
            ::FillRect(lpNMCustomDraw->nmcd.hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));

#define lpnm   ((LPNMHDR)lParam)
#define lpnmTB ((LPNMTOOLBAR)lParam)

            switch(lpnm->code)
            {
            case TBN_DROPDOWN:
                {
                    // Get the coordinates of the button.
                    RECT rc;
                    SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT,
                        (WPARAM)lpnmTB->iItem, (LPARAM)&rc);

                    // Convert to screen coordinates.
                    MapWindowPoints(lpnmTB->hdr.hwndFrom,
                        HWND_DESKTOP, (LPPOINT)&rc, 2);

                    // Get the menu.
                    HMENU hMenuLoaded = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_MAINMENU));

                    // Get the submenu for the first menu item.
                    HMENU hPopupMenu = GetSubMenu(hMenuLoaded, 0);

                    // Set up the popup menu.
                    // Set rcExclude equal to the button rectangle so that if the toolbar
                    // is too close to the bottom of the screen, the menu will appear above
                    // the button rather than below it.
                    TPMPARAMS tpm;
                    tpm.cbSize = sizeof(TPMPARAMS);
                    tpm.rcExclude = rc;

                    // Show the menu and wait for input.
                    // If the user selects an item, its WM_COMMAND is sent.
                    TrackPopupMenuEx(hPopupMenu,
                        TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
                        rc.left, rc.bottom, m_hDlg, &tpm);

                    DestroyMenu(hMenuLoaded);

                    return (FALSE);
                }
            }
        }

        return result;
    }

    virtual BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch(LOWORD (wParam))
        {
        case IDM_NEW:
            return onTbNew();
        case IDM_EXIT:
            return UDialogBox::onCancel();
        case IDM_SAVE:
            {
                UModalDialog dlg(m_hInst, IDD_TEST, m_hDlg);
                return dlg.doModal();
            }
        case IDM_OPEN:
            return onTbOpen();
        default:
            return UDialogBox::onCommand(wParam, lParam);
        }
    }

    virtual BOOL onPaint()
    {
        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(m_hDlg, &ps);
        RECT rt;
        GetClientRect(m_hDlg, &rt);
        ::FillRect(hdc, &rt, (HBRUSH)::GetStockObject(GRAY_BRUSH));
        rt.top = 5;
        ::SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, "Hello World!", strlen("Hello World!"), &rt, DT_SINGLELINE|DT_CENTER|DT_VCENTER );
        EndPaint(m_hDlg, &ps);

        return FALSE;
    }

    virtual BOOL onDestroy()
    {
        if (!IsWindow(hwndGoto))
        {
            DestroyWindow(hwndGoto);
            hwndGoto = NULL;
        }
        return UDialogBox::onDestroy();
    }
    /*
    virtual BOOL onCancel()
    {
        if (m_psub)
        {
            m_psub->destroy();
            delete m_psub;
            m_psub = 0;
        }
        UDialogBox::onCancel();
    }
    */
protected:

private:
    UToolBar *m_putl;
    //UDialogExt *m_psub;
    //UImageList uil;
    BOOL onTbNew()
    {
        m_putl->hideButton(IDM_OPEN);
        return FALSE;
    }

    BOOL onTbOpen()
    {
        /*
        if (!m_psub)
        {
        m_psub = new UDialogExt(m_hInst,IDD_TEST);
        m_psub->setParentH(m_hDlg);
        m_psub->create();
        //m_psub->show();
        }
        */

        if (!IsWindow(hwndGoto))
        {
            hwndGoto = ::CreateDialog(
                m_hInst,
                MAKEINTRESOURCE(IDD_TEST),
                m_hDlg,
                (DLGPROC)GoToProc);
            ::ShowWindow(hwndGoto, SW_SHOW);

        }
        return 0;
    }
};

UDLGAPP_T(UDialogExt, IDD_TEST);
