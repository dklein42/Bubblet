/*
============================================================================
gfx_Engine v1.02 -- 06/22/99
Tools für eine 'Fenster-Only' DirectX-Grafik-Engine
Code: Daniel Klein
(c) 1998/99 House of Bytes Software
============================================================================
*/

#include "gfx.h"

/*
 * Creates a DirectDraw Surface of a specified size
 * This surface will be in video memory if enough is
 * available, otherwise it will be created in system memory
*/
IDirectDrawSurface * CreateOffScreenSurface(IDirectDraw *pdd, int dx, int dy)
{
  DDSURFACEDESC ddsd;
  IDirectDrawSurface *pdds;

  //
  // create a DirectDrawSurface for this bitmap
  //
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = dx;
  ddsd.dwHeight = dy;

  if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
  {
	 return NULL;
  } else
  {
	 return pdds;
  }
}

/*
 * This function copies a previously loaded bitmap into a DirectDraw surface.
 * Notice that we can obtain a GDI style device context for a
 * DirectDraw surface, with which to BitBlt the bitmap into the
 * surface.
*/
HRESULT DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int dx, int dy)
{
  HDC hdcImage;
  HDC hdc;
  HRESULT hr;
  HBITMAP hbmOld;

  //
  // select bitmap into a memoryDC so we can use it.
  //
  hdcImage = CreateCompatibleDC(NULL);
  hbmOld = (HBITMAP)SelectObject(hdcImage, hbm);

  if ((hr = pdds->GetDC(&hdc)) == DD_OK)
  {
	 BitBlt(hdc, 0, 0, dx, dy, hdcImage, 0, 0, SRCCOPY);
	 pdds->ReleaseDC(hdc);
  }

  SelectObject(hdcImage, hbmOld);
  DeleteDC(hdcImage);

  return hr;
}

/*
 * Function to create an offscreen surface and load a bitmap from
 * disk into it. The szBitmap field is the filename of the Bitmap.
*/
IDirectDrawSurface * DDLoadBitmap(IDirectDraw *pdd, LPCSTR szBitmap)
{
  HBITMAP hbm;
  BITMAP bm;
  IDirectDrawSurface *pdds;

	  hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP,
				0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

  if (hbm == NULL)
	  hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP,
				0, 0, LR_CREATEDIBSECTION);

  if (hbm == NULL)
	  return NULL;

  GetObject(hbm, sizeof(bm), &bm); // get size of bitmap

  /*
	* create a DirectDrawSurface for this bitmap
	* source to function CreateOffScreenSurface() follows immediately
  */

  pdds = CreateOffScreenSurface(pdd, bm.bmWidth, bm.bmHeight);
  if (pdds)
  {
	 DDCopyBitmap(pdds, hbm, bm.bmWidth, bm.bmHeight);
  }

  DeleteObject(hbm);

  if(pdds==NULL) Error_Message("Fehler beim laden eines Bildes!");
  return pdds;
}

HRESULT DDReLoadBitmap(IDirectDrawSurface *pdds, LPCSTR szBitmap)
{
	 HBITMAP             hbm;
	 HRESULT             hr;

	 hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	 if (hbm == NULL)
		 hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	 if (hbm == NULL)
	 {
		OutputDebugString("handle is null\n");
		return E_FAIL;
	 }

	 hr = DDCopyBitmap(pdds, hbm, 0, 0);
	 if (hr != DD_OK)
	 {
		OutputDebugString("ddcopybitmap failed\n");
	 }
	 DeleteObject(hbm);
	 return hr;
}

Surface LoadSurface(LPCSTR BMP_Name)
{
  return DDLoadBitmap(dd_Object, BMP_Name);
}

HRESULT ReLoadSurface(Surface surface, LPCSTR BMP_Name)
{
  return DDReLoadBitmap(surface, BMP_Name);
}

void PutSurface(int xpos, int ypos, Surface src_surface, Surface dest_surface)
{
  RECT Rect;
  DDSURFACEDESC src_desc, dest_desc;
  HRESULT result;
  DDBLTFX bltfx;

  ZeroMemory(&src_desc, sizeof(src_desc));
  src_desc.dwSize = sizeof(src_desc);
  src_surface->GetSurfaceDesc(&src_desc);

  ZeroMemory(&dest_desc, sizeof(dest_desc));
  dest_desc.dwSize = sizeof(dest_desc);
  dest_surface->GetSurfaceDesc(&dest_desc);

  bltfx.dwSize= sizeof(bltfx);

  if( ((src_desc.dwWidth+xpos) < dest_desc.dwWidth)
	  & ((src_desc.dwHeight+ypos) < dest_desc.dwHeight))
  {
	 SetRect(&Rect, xpos, ypos, (src_desc.dwWidth+xpos), (src_desc.dwHeight+ypos));
  } else
  {
	 SetRect(&Rect, xpos, ypos, dest_desc.dwWidth, dest_desc.dwHeight);
  }

  if(src_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);
  if(dest_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);

  result= dest_surface->Blt( &Rect, src_surface, NULL, DDBLT_WAIT, &bltfx);
  if(result==DDERR_INVALIDPARAMS  ) Beep(1000,100);
}

//PutSurface MIT ColorKey
void PutSurfaceCC(int xpos, int ypos, Surface src_surface, Surface dest_surface)
{
  RECT Rect;
  DDSURFACEDESC src_desc, dest_desc;
  HRESULT result;
  DDBLTFX bltfx;

  ZeroMemory(&src_desc, sizeof(src_desc));
  src_desc.dwSize = sizeof(src_desc);
  src_surface->GetSurfaceDesc(&src_desc);

  ZeroMemory(&dest_desc, sizeof(dest_desc));
  dest_desc.dwSize = sizeof(dest_desc);
  dest_surface->GetSurfaceDesc(&dest_desc);

  bltfx.dwSize= sizeof(bltfx);

  if( ((src_desc.dwWidth+xpos) < dest_desc.dwWidth)
	  & ((src_desc.dwHeight+ypos) < dest_desc.dwHeight))
  {
	 SetRect(&Rect, xpos, ypos, (src_desc.dwWidth+xpos), (src_desc.dwHeight+ypos));
  } else
  {
	 //Beep(1000,100);
	 //SetRect(&Rect, xpos, ypos, dest_desc.dwWidth, dest_desc.dwHeight);
	 return;
  }

  if(src_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);
  if(dest_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);

  result= dest_surface->Blt( &Rect, src_surface, NULL, DDBLT_WAIT|DDBLT_KEYSRC, &bltfx);
  if(result==DDERR_INVALIDPARAMS  ) Beep(1000,100);
}

//PutSurface MIT ColorKey MIT Clipping
void PutSurfaceClipCC(int xpos, int ypos, Surface src_surface, Surface dest_surface, RECT clip_rect)
{
  RECT Rect;
  DDSURFACEDESC src_desc, dest_desc;
  HRESULT result;
  DDBLTFX bltfx;
  RECT src_rect, src_clip_rect;

  ZeroMemory(&src_desc, sizeof(src_desc));
  src_desc.dwSize = sizeof(src_desc);
  src_surface->GetSurfaceDesc(&src_desc);

  ZeroMemory(&dest_desc, sizeof(dest_desc));
  dest_desc.dwSize = sizeof(dest_desc);
  dest_surface->GetSurfaceDesc(&dest_desc);

  bltfx.dwSize= sizeof(bltfx);

  src_rect.left= xpos;
  src_rect.top= ypos;
  src_rect.right= xpos+src_desc.dwWidth;
  src_rect.bottom= ypos+src_desc.dwHeight;

  //wenn alles im Clipping-Bereich ist, muß nix gemacht werden...
  if(  (src_rect.left > clip_rect.left)
	  & (src_rect.top > clip_rect.top)
	  & (src_rect.right < clip_rect.right)
	  & (src_rect.bottom < clip_rect.bottom) )
  {
	 SetRect(&Rect, xpos, ypos, (src_desc.dwWidth+xpos), (src_desc.dwHeight+ypos));

	 if(src_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);
	 if(dest_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);

	 result= dest_surface->Blt( &Rect, src_surface, NULL, DDBLT_WAIT|DDBLT_KEYSRC, &bltfx);
  } else
  {
	 //ansonsten müssen wir halt alles testen...
	 src_clip_rect.left= 0;
	 src_clip_rect.top= 0;
	 src_clip_rect.right= src_desc.dwWidth;
	 src_clip_rect.bottom= src_desc.dwHeight;

	 SetRect(&Rect, xpos, ypos, (src_desc.dwWidth+xpos), (src_desc.dwHeight+ypos));

	 if(src_rect.left < clip_rect.left)
	 {
		src_clip_rect.left+= clip_rect.left-src_rect.left;
		Rect.left+= clip_rect.left-src_rect.left;
	 }
	 if(src_rect.top < clip_rect.top)
	 {
		src_clip_rect.top+= clip_rect.top-src_rect.top;
		Rect.top+= clip_rect.top-src_rect.top;
	 }
	 if(src_rect.right > clip_rect.right)
	 {
		src_clip_rect.right-= src_rect.right-clip_rect.right;
		Rect.right-= src_rect.right-clip_rect.right;
	 }
	 if(src_rect.bottom > clip_rect.bottom)
	 {
		src_clip_rect.bottom-= src_rect.bottom-clip_rect.bottom;
		Rect.bottom-= src_rect.bottom-clip_rect.bottom;
	 }

	 if(src_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);
	 if(dest_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);

	 result= dest_surface->Blt( &Rect, src_surface, &src_clip_rect,
															 DDBLT_WAIT|DDBLT_KEYSRC, &bltfx);
  }

  if(result==DDERR_INVALIDPARAMS) Beep(1000,100);
}

// PutSurface nur auf Primary Surfaces (wegen verschiebung durch Fenster-Koordinaten)
void PutSurfacePrimary(int xpos, int ypos, Surface src_surface)
{
  RECT Rect;
  DDSURFACEDESC src_desc, dest_desc;
  HRESULT result;
  DDBLTFX bltfx;

  ZeroMemory(&src_desc, sizeof(src_desc));
  src_desc.dwSize = sizeof(src_desc);
  src_surface->GetSurfaceDesc(&src_desc);

  ZeroMemory(&dest_desc, sizeof(dest_desc));
  dest_desc.dwSize = sizeof(dest_desc);
  dd_Primary->GetSurfaceDesc(&dest_desc);

  bltfx.dwSize= sizeof(bltfx);

  if( ((src_desc.dwWidth+xpos) < dest_desc.dwWidth)
	  & ((src_desc.dwHeight+ypos) < dest_desc.dwHeight))
  {
	 SetRect(&Rect, xpos+Client_Pos.x, ypos+Client_Pos.y,
			  (src_desc.dwWidth+xpos)+Client_Pos.x, (src_desc.dwHeight+ypos)+Client_Pos.y);
  } else
  {
	 SetRect(&Rect, xpos+Client_Pos.x, ypos+Client_Pos.y,
			  dest_desc.dwWidth+Client_Pos.x, dest_desc.dwHeight+Client_Pos.y);
  }

  if(src_surface->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);
  if(dd_Primary->IsLost()==DDERR_SURFACELOST) PostMessage(main_hwnd, WM_SURFACELOST, 0, 0);

  result= dd_Primary->Blt( &Rect, src_surface, NULL, DDBLT_WAIT, &bltfx);
  if(result==DDERR_INVALIDPARAMS  ) Beep(1000,100);
}

void FillSurface(Surface dest_surface, int x, int y, int ex, int ey, COLORREF rgb)
{
  RECT Rect;
  HRESULT result;
  DDBLTFX bltfx;

  bltfx.dwFillColor= rgb;
  bltfx.dwSize= sizeof(bltfx);
  SetRect(&Rect, x, y, ex, ey);
  result= dest_surface->Blt( &Rect, NULL/*surface*/, NULL, DDBLT_COLORFILL, &bltfx);
  if(result==DDERR_INVALIDPARAMS  ) Beep(1000,100);
}

void ShowText(Surface surface, int x, int y, LPCSTR Text, COLORREF text_col)
{
  HDC hdc;
  BOOL result;

 /*result=*/ surface->GetDC(&hdc);
 //if(result!=DD_OK) Beep(100,100);

 SetBkMode(hdc, TRANSPARENT);
 SetBkColor(hdc, TRANSPARENT);
 SetTextColor(hdc, text_col);
 SetTextAlign(hdc, TA_LEFT);
 TextOut(hdc, x, y, Text, lstrlen(Text));

 surface->ReleaseDC(hdc);
}

void ShowTextCenter(Surface surface, int x, int y, LPCSTR Text, COLORREF text_col)
{
  HDC hdc;

 surface->GetDC(&hdc);

 SetBkMode(hdc, TRANSPARENT);
 SetBkColor(hdc, TRANSPARENT);
 SetTextColor(hdc, text_col);
 SetTextAlign(hdc, TA_CENTER);
 TextOut(hdc, x, y, Text, lstrlen(Text));

 surface->ReleaseDC(hdc);
}


/*
 * ColorMatch
 *
 * convert a RGB color to a pysical color.
 *
 * we do this by leting GDI SetPixel() do the color matching
 * then we lock the memory and see what it got mapped to.
 */
DWORD ColorMatch(IDirectDrawSurface *pdds, COLORREF rgb)
{
	 COLORREF rgbT;
	 HDC hdc;
	 DWORD dw = CLR_INVALID;
	 DDSURFACEDESC ddsd;
	 HRESULT hres;

	 //
	 //  use GDI SetPixel to color match for us
	 //
	 if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	 {
	rgbT = GetPixel(hdc, 0, 0);             // save current pixel value
	SetPixel(hdc, 0, 0, rgb);               // set our value
	pdds->ReleaseDC(hdc);
	 }

	 //
	 // now lock the surface so we can read back the converted color
	 //
	 ddsd.dwSize = sizeof(ddsd);
	 while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
	;

	 if (hres == DD_OK)
	 {
	dw  = *(DWORD *)ddsd.lpSurface;                     // get DWORD
		  if(ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
				dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;  // mask it to bpp
	pdds->Unlock(NULL);
	 }

	 //
	 //  now put the color that was there back.
	 //
	 if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	 {
	SetPixel(hdc, 0, 0, rgbT);
	pdds->ReleaseDC(hdc);
	 }

	 return dw;
}

/*
 * SetColorKey
 *
 * set a color key for a surface, given a RGB.
 * if you pass CLR_INVALID as the color key, the pixel
 * in the upper-left corner will be used.
 */
HRESULT SetColorKey(IDirectDrawSurface *pdds, COLORREF rgb)
{
	 DDCOLORKEY          ddck;

	 ddck.dwColorSpaceLowValue  = ColorMatch(pdds, rgb);
	 ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
	 return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

//Draw Line from x1/y1 to x2/y2 via GDI
void DrawLine(Surface surface, int x1, int y1, int x2, int y2, COLORREF rgb)
{
  HDC hdc;
  BOOL result;
  HPEN pen, oldpen;

  result= surface->GetDC(&hdc);
  if(result!=DD_OK) Error_Message("Kein DC verfügbar!");

  pen= CreatePen(PS_SOLID, 0, rgb);
  oldpen= SelectObject(hdc, pen);

  MoveToEx(hdc, x1, y1, NULL);
  LineTo(hdc, x2, y2);

  SelectObject(hdc, oldpen);
  DeleteObject(pen);
  surface->ReleaseDC(hdc);
}


