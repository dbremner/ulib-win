#include "resource.h"

#define  WINVER 0x0501

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <assert.h>

#include "udialogx.h"
#include "umenu.h"
#include "udlgapp.h"
#include "umsg.h"

using huys::UDialogBox;

class UDialogExt : public UDialogBox
{
    enum {
        IDM_HELP = 3333
    };
public:
    UDialogExt(HINSTANCE hInst, UINT nID)
    : UDialogBox(hInst, nID)
    {}

    virtual BOOL onInit()
    {
        umn.createPopup();

        umn.append(IDM_HELP, _T("&help"));

        MENUINFO mi = {0};
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS | MIIM_BITMAP;
        mi.dwStyle = MNS_AUTODISMISS;
        mi.hbrBack = (HBRUSH)::GetStockObject(GRAY_BRUSH);

        HMENU hMenu = ::GetMenu(m_hDlg);

        ::SetMenuInfo(hMenu, &mi);

        HBITMAP hbmp1;    // handle to pie chart bitmap
        HBITMAP hbmp2;    // handle to line chart bitmap
        HBITMAP hbmp3;    // handle to bar chart bitmap

        //hbmp1 = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
        //hbmp2 = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDI_ICON2));
        //hbmp3 = LoadBitmap(m_hInst, MAKEINTRESOURCE(IDI_ICON3));
        hbmp1 = (HBITMAP)::LoadImage( m_hInst, // small class icon
                   MAKEINTRESOURCE(IDB_ICON1),
                   IMAGE_BITMAP,
                   GetSystemMetrics(SM_CXSMICON),
                   GetSystemMetrics(SM_CYSMICON),
                   LR_DEFAULTCOLOR);

        hbmp2 = (HBITMAP)::LoadImage( m_hInst, // small class icon
                   MAKEINTRESOURCE(IDB_ICON2),
                   IMAGE_BITMAP,
                   GetSystemMetrics(SM_CXSMICON),
                   GetSystemMetrics(SM_CYSMICON),
                   LR_DEFAULTCOLOR);

        hbmp3 = (HBITMAP)::LoadImage( m_hInst, // small class icon
                   MAKEINTRESOURCE(IDB_ICON3),
                   IMAGE_BITMAP,
                   GetSystemMetrics(SM_CXSMICON),
                   GetSystemMetrics(SM_CYSMICON),
                   LR_DEFAULTCOLOR);

        assert(hbmp1 != 0);

        ::SetMenuItemBitmaps(hMenu, IDM_EXIT, MF_BYCOMMAND, hbmp1, hbmp2);

        //AppendMenu(umn.getHandle(), MF_BITMAP, IDM_ICON1, (LPCTSTR) hbmp1);
        //AppendMenu(umn.getHandle(), MF_BITMAP, IDM_ICON2, (LPCTSTR) hbmp2);
        //AppendMenu(umn.getHandle(), MF_BITMAP, IDM_ICON3, (LPCTSTR) hbmp3);
        umn.addSeparator();

        umn.append(IDM_ICON1, hbmp1);
        umn.append(IDM_ICON2, hbmp2);
        umn.append(IDM_ICON3, hbmp3);

        umn.addSeparator();

        upop.createPopup();
        upop.append(3335, _T("&quit"));

        umn.addSubMenu(upop, _T("&popup"));

        umn.checkItemByPos(0);

        return TRUE;
    }

    virtual BOOL onRButtonDown(WPARAM wParam, LPARAM lParam)
    {
        POINT pt;
        ::GetCursorPos(&pt);
        umn.attach(m_hDlg);
        //umn.trackPopup(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y);
        umn.trackPopup(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y);
        return FALSE;
    }

    virtual BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case IDM_HELP:
            showMsg("&Help!");
            break;
        case 3335:
        case IDM_EXIT:
            return this->onCancel();
        case IDM_COPY:
            showMsg("Not implement yet");
            break;
        case IDM_ABOUT:
            showMsg("About");
            break;
        case IDM_ICON1:
            showMsg("Icon1");
            break;
        case IDM_ICON2:
            showMsg("Icon2");
            break;
        case IDM_ICON3:
            showMsg("Icon3");
            break;
        default:
            ;
        }
        return UDialogBox::onCommand(wParam, lParam);
    }
private:
    UMenu umn;
    UMenu upop;
};

UDLGAPP_T(UDialogExt, IDD_TEST);

