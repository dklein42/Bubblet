/*
============================================================================
gfx_Engine v1.0 -- 13/10/98
Code: Daniel Klein
(c) 1998 House of Bytes Software
============================================================================
*/

#if defined( __BORLANDC__ ) && defined( __WIN32__ )
#define _WIN32
#endif

#define WIN32_LEAN_AND_MEAN
#include "ddraw.h"

#define WM_SURFACELOST 9999

typedef LPDIRECTDRAWSURFACE Surface;  //Namen kann man sich besser merken ;-)

extern LPDIRECTDRAW   dd_Object;    // DirectDraw object
extern Surface     dd_Primary;   // DirectDraw primary surface
extern BOOL bActive;
extern POINT Client_Pos;   // Position des Fenster-Client
extern void Error_Message(LPCSTR str);
extern HWND main_hwnd;    // Window-Handle


IDirectDrawSurface * CreateOffScreenSurface(IDirectDraw *pdd, int dx, int dy);

Surface LoadSurface(LPCSTR BMP_Name);
HRESULT ReLoadSurface(Surface surface, LPCSTR BMP_Name);
void PutSurface(int xpos, int ypos, Surface src_surface, Surface dest_surface);
void PutSurfacePrimary(int xpos, int ypos, Surface src_surface);
void PutSurfaceCC(int xpos, int ypos, Surface src_surface, Surface dest_surface);
void PutSurfaceClipCC(int xpos, int ypos, Surface src_surface, Surface dest_surface, RECT clip_rect);
void FillSurface(Surface dest_surface, int x, int y, int ex, int ey, COLORREF rgb);
void ShowText(Surface surface, int x, int y, LPCSTR Text, COLORREF text_col);
void ShowTextCenter(Surface surface, int x, int y, LPCSTR Text, COLORREF text_col);
void DrawLine(Surface surface, int x1, int y1, int x2, int y2, COLORREF rgb);

DWORD ColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
HRESULT SetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);


