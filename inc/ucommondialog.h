#ifndef U_COMMON_DIALOG_H
#define U_COMMON_DIALOG_H

#include "ulib.h"
#include "colors.h"

namespace UCommonDialog
{

/**
 * @class UColorDialog
 * @brief Choose an RGB color.
 */
class ULIB_API UColorDialog
{
public:
    UColorDialog(HWND hWnd, huys::Color clr = huys::white);
    ~UColorDialog();
    BOOL choose();
private:
    CHOOSECOLOR m_cc;
    HWND m_hWnd;
    huys::Color m_clr;
    huys::Color acrCustClr[16];
};


/**
 * @class UFileDialog
 * @brief Choose a filename.
 */
class ULIB_API UFileDialog
{
public:
    UFileDialog(HWND hWnd);
    ~UFileDialog();
    BOOL open();
    LPCTSTR getName() const {return szFile;}
private:
    OPENFILENAME m_ofn;
    HWND m_hWnd;
    TCHAR szFile[260];       // buffer for file name
    //HANDLE m_hFile;
};


/**
 * @class UFontDialog
 * @brief Choose a font.
 */
class ULIB_API UFontDialog
{
public:
    UFontDialog(HWND hwnd);
    ~UFontDialog();

    BOOL choose();

    const LOGFONT &logFont() const
    {
        return m_lf;
    }
protected:
private:
    CHOOSEFONT m_cf;
    LOGFONT m_lf;
    HWND m_hWnd;
};

/**
 * @class UPrinterDialog
 * @brief Choose a printer
 */
class UPrinterDialog
{
public:
    UPrinterDialog(HWND hwnd);
    ~UPrinterDialog();
    
    BOOL choose();
    
    HDC getHDC() const
    { return m_pd.hDC; }
private:
    PRINTDLG m_pd;
    HWND m_hWnd;
};

}; // namespace UCommonDialog

#endif // U_COMMON_DIALOG_H
