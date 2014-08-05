

/*
HOOK_DEFINE(user32.dll, DWORD, GetTabbedTextExtentA, (HDC hdc, LPCSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions), (hdc, lpString, nCount, nTabPositions, lpnTabStopPositions))
HOOK_DEFINE(user32.dll, DWORD, GetTabbedTextExtentW, (HDC hdc, LPCWSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions), (hdc, lpString, nCount, nTabPositions, lpnTabStopPositions))

HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointA, (HDC hdc, LPCSTR lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, lpszStr, cchString, nMaxExtent, lpnFit, lpDx, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointW, (HDC hdc, LPCWSTR lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, lpszStr, cchString, nMaxExtent, lpnFit, lpDx, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointI, (HDC hdc, LPWORD pgiIn, int cgi, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, pgiIn, cgi, nMaxExtent, lpnFit, lpDx, lpSize))

HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointA, (HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointW, (HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointI, (HDC hdc, LPWORD pgiIn, int cgi, LPSIZE lpSize), (hdc, pgiIn, cgi, lpSize))
*/

HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32A, (HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize));
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32W, (HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize));
HOOK_DEFINE(gdi32.dll, HFONT, CreateFontA, (int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCSTR  lpszFace), (nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace));
HOOK_DEFINE(gdi32.dll, HFONT, CreateFontW, (int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCWSTR lpszFace), (nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace));
HOOK_DEFINE(gdi32.dll, HFONT, CreateFontIndirectA, (CONST LOGFONTA* lplf), (lplf))
HOOK_DEFINE(gdi32.dll, HFONT, CreateFontIndirectW, (CONST LOGFONTW* lplf), (lplf))

HOOK_DEFINE(gdi32.dll, BOOL, TextOutA, (HDC hdc, int nXStart, int nYStart, LPCSTR  lpString, UINT cbString), (hdc, nXStart, nYStart, lpString, cbString));
HOOK_DEFINE(gdi32.dll, BOOL, TextOutW, (HDC hdc, int nXStart, int nYStart, LPCWSTR lpString, UINT cbString), (hdc, nXStart, nYStart, lpString, cbString));
HOOK_DEFINE(gdi32.dll, BOOL, ExtTextOutA, (HDC hdc, int nXStart, int nYStart, UINT fuOptions, CONST RECT* lprc, LPCSTR  lpString, UINT cbString, CONST INT* lpDx), (hdc, nXStart, nYStart, fuOptions, lprc, lpString, cbString, lpDx));
HOOK_DEFINE(gdi32.dll, BOOL, ExtTextOutW, (HDC hdc, int nXStart, int nYStart, UINT fuOptions, CONST RECT* lprc, LPCWSTR lpString, UINT cbString, CONST INT* lpDx), (hdc, nXStart, nYStart, fuOptions, lprc, lpString, cbString, lpDx));
