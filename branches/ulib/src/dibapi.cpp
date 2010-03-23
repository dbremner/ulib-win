#include "StdAfx.h"
#include "dibapi.h"
#include <io.h>
#include <errno.h>
#include <math.h>
#include <direct.h>

//
#define DIB_HEADER_MARKER ((WORD)('M'<<8)|'B')

BOOL WINAPI PaintDIB(HDC hDC,
                     LPRECT lpDCRect, 
                     HDIB hDIB, 
                     LPRECT lpDIBRect, 
                     CPalette *pPal)
{
    LPSTR lpDIBHdr;
    LPSTR lpDIBBits;
    BOOL  bSuccess = FALSE;
    HPALETTE hPal = NULL;
    HPALETTE hOldPal = NULL;

    //
    if (hDIB == NULL)
    {
        return FALSE;
    }

    //
    lpDIBHdr = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
    lpDIBBits = FindDIBBits(lpDIBHdr);
    //
    if (pPal != NULL)
    {
        hPal = (HPALETTE)pPal->m_hObject;
        hOldPal = ::SelectPalette(hDC, hPal, TRUE);
    }

    //
    ::SetStretchBltMode(hDC, COLORONCOLOR);

    //
    if ((RECTWIDTH(lpDCRect) == RECTWIDTH(lpDIBRect)) &&
         (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
    {
        //
        bSuccess = ::SetDIBitsToDevice(hDC,
                                       lpDCRect->left,
                                       lpDCRect->top,
                                       RECTWIDTH(lpDCRect),
                                       RECTHEIGHT(lpDCRect),
                                       lpDIBRect->left,
                                       (int)DIBHeight(lpDIBHdr)-
                                       lpDIBRect->top-
                                       RECTHEIGHT(lpDIBRect),
                                       0,
                                       (WORD)DIBHeight(lpDIBHdr),
                                       lpDIBBits,
                                       (LPBITMAPINFO)lpDIBHdr,
                                       DIB_RGB_COLORS);
    }
    else
    {
        //
        bSuccess = ::StretchDIBits(hDC,
                                   lpDCRect->left,
                                   lpDCRect->top,
                                   RECTWIDTH(lpDCRect),
                                   RECTHEIGHT(lpDCRect),
                                   lpDIBRect->left,
                                   lpDIBRect->top,
                                   RECTWIDTH(lpDIBRect),
                                   RECTHEIGHT(lpDIBRect),
                                   lpDIBBits,
                                   (LPBITMAPINFO)lpDIBHdr,
                                   DIB_RGB_COLORS,
                                   SRCCOPY); 
    }

    ::GlobalUnlock((HGLOBAL)hDIB);

    //
    if (hOldPal != NULL)
    {
        ::SelectPalette(hDC, hOldPal, TRUE);
    }

    return bSuccess;
}

//
BOOL WINAPI CreateDIBPalette(HDIB hDIB, CPalette *pPal)
{
    //
    LPLOGPALETTE lpPal;

    //
    HANDLE hLogPal;

    //
    HPALETTE hPal = NULL;

    //
    int i;

    //
    WORD wNumColors;

    //
    LPSTR lpbi;

    //
    LPBITMAPINFO lpbmi;

    //
    LPBITMAPCOREINFO lpbmc;

    //
    BOOL bWinStyleDIB;

    //
    BOOL bResult = FALSE;

    //
    if (hDIB == NULL)
    {
        return FALSE;
    }

    //
    lpbi = (LPSTR)::GlobalLock((HGLOBAL)hDIB);

    //
    lpbmi = (LPBITMAPINFO)lpbi;

    //
    lpbmc = (LPBITMAPCOREINFO)lpbi;

    //
    wNumColors = ::DIBNumColors(lpbi);

    //
    if (wNumColors != 0)
    {
        //
        hLogPal = ::GlobalAlloc(GHND, sizeof(LPLOGPALETTE)
                                      + sizeof(PALETTEENTRY)
                                       * wNumColors);
        //
        if (hLogPal == 0)
        {
            //
            ::GlobalUnlock((HGLOBAL)hDIB);

            //
            return FALSE;
        }

        lpPal = (LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPal);

        //
        lpPal->palVersion = PALVERSION;

        //
        lpPal->palNumEntries = (WORD)wNumColors;

        //
        bWinStyleDIB = IS_WIN30_DIB(lpbi);

        //
        for (i=0; i<(int)wNumColors; i++)
        {
            if (bWinStyleDIB)
            {
                //
                lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;

                //
                lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;

                //
                lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;

                //
                lpPal->palPalEntry[i].peFlags = 0;
            }
            else
            {
                //
                lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
                
                //
                lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
                
                //
                lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
                
                //
                lpPal->palPalEntry[i].peFlags = 0;
            }
        }

        //
        bResult = pPal->CreatePalette(lpPal);

        //
        ::GlobalUnlock((HGLOBAL)hLogPal);

        //
        ::GlobalFree((HGLOBAL)hLogPal);
    }
    
    //
    ::GlobalUnlock((HGLOBAL)hDIB);

    return bResult;
}

//
LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
    return (lpbi + *(LPDWORD)lpbi+::PaletteSize(lpbi));
}

//
DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
    //
    LPBITMAPINFOHEADER lpbmi;

    //
    LPBITMAPCOREHEADER lpbmc;

    //
    lpbmi = (LPBITMAPINFOHEADER)lpDIB;
    lpbmc = (LPBITMAPCOREHEADER)lpDIB;

    //
    if (IS_WIN30_DIB(lpDIB))
    {
        //
        return lpbmi->biWidth;
    }
    else
    {
        //
        return (DWORD)lpbmc->bcWidth;
    }
}


//
//
DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
    //
    LPBITMAPINFOHEADER lpbmi;
    
    //
    LPBITMAPCOREHEADER lpbmc;
    
    //
    lpbmi = (LPBITMAPINFOHEADER)lpDIB;
    lpbmc = (LPBITMAPCOREHEADER)lpDIB;
    
    //
    if (IS_WIN30_DIB(lpDIB))
    {
        //
        return lpbmi->biHeight;
    }
    else
    {
        //
        return (DWORD)lpbmc->bcHeight;
    }
}

//
WORD WINAPI PaletteSize(LPSTR lpbi)
{
    //
    if (IS_WIN30_DIB(lpbi))
    {
        //
        return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
    }
    else
    {
        //
        return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
    }
}

//
WORD WINAPI DIBNumColors(LPSTR lpbi)
{
    WORD wBitCount;

	//
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		//
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

		if (dwClrUsed != 0)
		{
			//
			return (WORD)dwClrUsed;
		}
	}

	//
	if (IS_WIN30_DIB(lpbi))
	{
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	}
	else
	{
		//
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	}

	switch(wBitCount)
	{
	case 1:
		return 2;
	case 4:
		return 16;
	case 8:
		return 256;
	default:
		return 0;
	}

}

//
HGLOBAL WINAPI CopyHandle(HGLOBAL h)
{
    if (h == NULL)
    {
        return NULL;
    }

    //
    DWORD dwLen = ::GlobalSize((HGLOBAL)h);

    //
    HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

    //
    if (hCopy != NULL)
    {
        //
		void *lpCopy = ::GlobalLock((HGLOBAL)hCopy);
		void *lp = ::GlobalLock((HGLOBAL)h);

		//
		memcpy(lpCopy, lp, dwLen);

		//
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
    }

    return hCopy;
}

//
BOOL WINAPI SaveDIB(HDIB hDib, CFile& file)
{
    //
    BITMAPFILEHEADER bmfHeader;

    //
    LPBITMAPINFOHEADER lpBI;

    //
    DWORD dwDIBSize;
	//
	//LPBYTE lpBits;              // memory pointer 

    if (hDib==NULL)
    {
        //
        return FALSE;
    }

    //
    lpBI = (LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)hDib);

    if (lpBI == NULL)
    {
        //
        return FALSE;
    }

    //
    if (!IS_WIN30_DIB(lpBI))
    {
        //
        //
        ::GlobalUnlock((HGLOBAL)hDib);

        //
        return FALSE;
    }

    //
    bmfHeader.bfType = DIB_HEADER_MARKER;

    //
    //
    //
    dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);

    //
    if ((lpBI->biCompression == BI_RLE8)||(lpBI->biCompression == BI_RLE4))
    {
        //
        dwDIBSize += lpBI->biSizeImage;
    }
    else
    {
        //
        DWORD dwBmBitsSize;
        //
        dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;
		//
		dwDIBSize += dwBmBitsSize;

        //
        lpBI->biSizeImage = dwBmBitsSize;
    }

    //
    bmfHeader.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);

    //
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
                            + PaletteSize((LPSTR)lpBI);

    //
    TRY {
        //
        file.Write((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));

        //
        file.Write(lpBI, dwDIBSize);

    }
    CATCH (CFileException, e)
    {
        ::GlobalUnlock((HGLOBAL)hDib);
        
        THROW_LAST();
    }
    END_CATCH
        
    ::GlobalUnlock((HGLOBAL)hDib);

    return TRUE;
}

//
HDIB WINAPI ReadDIBFile(CFile &file)
{
    BITMAPFILEHEADER bmfHeader;
    DWORD dwBitsSize;
    HDIB hDIB;
    LPSTR pDIB;

    //
    dwBitsSize = (DWORD)file.GetLength();

    //
    if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
    {
        //
        return NULL;
    }

    //
    if (bmfHeader.bfType != DIB_HEADER_MARKER)
    {
        //
        return NULL;
    }
    
    //
    hDIB = (HDIB)::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwBitsSize);
    if (hDIB == 0)
    {
        //
        return NULL;
    }

    //
    pDIB = (LPSTR)::GlobalLock((HGLOBAL)hDIB);

    //
    //if (file.ReadHuge(pDIB, dwBitsSize-sizeof(BITMAPFILEHEADER))!=
    //        dwBitsSize-sizeof(BITMAPFILEHEADER))
	if (file.Read(pDIB, dwBitsSize-sizeof(BITMAPFILEHEADER))!=
		        dwBitsSize-sizeof(BITMAPFILEHEADER))
    {
        //
        ::GlobalUnlock((HGLOBAL)hDIB);

        //
        ::GlobalFree((HGLOBAL)hDIB);

        //
        return NULL;
    }

    //
    ::GlobalUnlock((HGLOBAL)hDIB);

    return hDIB;
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal )
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 )
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL WriteDIB( LPTSTR szFile, HANDLE hDIB)
{
    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER	lpbi;
    
    if (!hDIB)
        return FALSE;
    
    CFile file;
    if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
        return FALSE;
    
    lpbi = (LPBITMAPINFOHEADER)hDIB;
    
    int nColors = 1 << lpbi->biBitCount;
    
    // Fill in the fields of the file header 
    hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
    hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
    hdr.bfReserved1 	= 0;
    hdr.bfReserved2 	= 0;
    hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
        nColors * sizeof(RGBQUAD));
    
    // Write the file header 
    file.Write( &hdr, sizeof(hdr) );
    
    // Write the DIB header and the bits 
    file.Write( lpbi, GlobalSize(hDIB) );
    
    return TRUE;
}
// TransparentBlt	- Copies a bitmap transparently onto the destination DC
// hdcDest		- Handle to destination device context 
// nXDest		- x-coordinate of destination rectangle's upper-left corner 
// nYDest		- y-coordinate of destination rectangle's upper-left corner 
// nWidth		- Width of destination rectangle 
// nHeight		- height of destination rectangle 
// hBitmap		- Handle of the source bitmap
// nXSrc		- x-coordinate of source rectangle's upper-left corner 
// nYSrc		- y-coordinate of source rectangle's upper-left corner 
// colorTransparent	- The transparent color
// hPal			- Logical palette to be used with bitmap. Can be NULL

void TransparentBlt( HDC hdcDest, int nXDest, int nYDest, int nWidth,
                    int nHeight, HBITMAP hBitmap, int nXSrc, int nYSrc,
                    COLORREF colorTransparent, HPALETTE hPal )
{
    CDC dc, memDC, maskDC, tempDC;
    dc.Attach( hdcDest );
    maskDC.CreateCompatibleDC(&dc);
    CBitmap maskBitmap;
    
    //add these to store return of SelectObject() calls
    CBitmap* pOldMemBmp = NULL;
    CBitmap* pOldMaskBmp = NULL;
    HBITMAP hOldTempBmp = NULL;
    
    memDC.CreateCompatibleDC(&dc);
    tempDC.CreateCompatibleDC(&dc);
    CBitmap bmpImage;
    bmpImage.CreateCompatibleBitmap( &dc, nWidth, nHeight );
    pOldMemBmp = memDC.SelectObject( &bmpImage );
    
    // Select and realize the palette
    if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && hPal )
    {
        ::SelectPalette( dc, hPal, FALSE );
        dc.RealizePalette();
        
        ::SelectPalette( memDC, hPal, FALSE );
    }
    
    hOldTempBmp = (HBITMAP) ::SelectObject( tempDC.m_hDC, hBitmap );
    
    memDC.BitBlt( 0,0,nWidth, nHeight, &tempDC, nXSrc, nYSrc, SRCCOPY );
    
    // Create monochrome bitmap for the mask
    maskBitmap.CreateBitmap( nWidth, nHeight, 1, 1, NULL );
    pOldMaskBmp = maskDC.SelectObject( &maskBitmap );
    memDC.SetBkColor( colorTransparent );
    
    // Create the mask from the memory DC
    maskDC.BitBlt( 0, 0, nWidth, nHeight, &memDC,
        0, 0, SRCCOPY );
    
    // Set the background in memDC to black. Using SRCPAINT with black 
    // and any other color results in the other color, thus making 
    // black the transparent color
    memDC.SetBkColor(RGB(0,0,0));
    memDC.SetTextColor(RGB(255,255,255));
    memDC.BitBlt(0, 0, nWidth, nHeight, &maskDC, 0, 0, SRCAND);
    
    // Set the foreground to black. See comment above.
    dc.SetBkColor(RGB(255,255,255));
    dc.SetTextColor(RGB(0,0,0));
    dc.BitBlt(nXDest, nYDest, nWidth, nHeight, &maskDC, 0, 0, SRCAND);
    
    // Combine the foreground with the background
    dc.BitBlt(nXDest, nYDest, nWidth, nHeight, &memDC,
        0, 0, SRCPAINT);
    
    
    if (hOldTempBmp)
        ::SelectObject( tempDC.m_hDC, hOldTempBmp);
    if (pOldMaskBmp)
        maskDC.SelectObject( pOldMaskBmp );
    if (pOldMemBmp)
        memDC.SelectObject( pOldMemBmp );
    
    dc.Detach();
}

HBITMAP GetSrcBit(HDC hDC,DWORD BitWidth, DWORD BitHeight)
{
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
	hBitTemp = (HBITMAP) SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight,hDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP) SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	return hBitmap;
}

HANDLE DDBToDIB( HBITMAP hbitmap, DWORD dwCompression, CPalette* pPal )
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	::GetObject(hbitmap, sizeof(BITMAP), &bm);

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Initialize the bitmapinfoheader
	bi.biSize			= sizeof(BITMAPINFOHEADER);
	bi.biWidth			= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 )
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, hbitmap, 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, hbitmap,
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

BOOL SaveBmp(HBITMAP hBitmap, CString FileName)
{
    //设备描述表
    HDC hDC;
    //当前分辨率下每象素所占字节数
    int iBits;
    //位图中每象素所占字节数
    WORD wBitCount;
    //定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数 
    DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0; 
    //位图属性结构 
    BITMAP Bitmap; 
    //位图文件头结构
    BITMAPFILEHEADER bmfHdr; 
    //位图信息头结构 
    BITMAPINFOHEADER bi; 
    //指向位图信息头结构 
    LPBITMAPINFOHEADER lpbi; 
    //定义文件，分配内存句柄，调色板句柄 
    HANDLE fh, hDib, hPal,hOldPal=NULL; 
    //计算位图文件每个像素所占字节数 
    hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
    DeleteDC(hDC); 
    if (iBits <= 1) wBitCount = 1; 
    else if (iBits <= 4) wBitCount = 4; 
    else if (iBits <= 8) wBitCount = 8; 
    else wBitCount = 24; 
    GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 0;
    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
    //为位图内容分配内存 
    hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER)); 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
    *lpbi = bi; 
    // 处理调色板 
    hPal = GetStockObject(DEFAULT_PALETTE); 
    if (hPal) 
    { 
        hDC = ::GetDC(NULL); 
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
        RealizePalette(hDC); 
    }
    // 获取该调色板下新的像素值 
    GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
        +dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS); 
    //恢复调色板 
    if (hOldPal) 
    { 
        ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
        RealizePalette(hDC); 
        ::ReleaseDC(NULL, hDC); 
    } 
    //创建位图文件 
    fh = CreateFile(FileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
    if (fh == INVALID_HANDLE_VALUE) return FALSE; 
    // 设置位图文件头 
    bmfHdr.bfType = 0x4D42; // "BM" 
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize; 
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0; 
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
    
    // 写入位图文件头
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 
    // 写入位图文件其余内容 
    // WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); //清除 
    GlobalUnlock(hDib); 
    GlobalFree(hDib); 
    CloseHandle(fh); 
    return TRUE;
}
