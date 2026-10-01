/*
===============================================================================
(Bescheuertes) Grundgerüst für eine DirectX-Fenster-Applikation
Code: Daniel Klein
(c) 1998 House of Bytes Software
===============================================================================
*/

#define WM_SURFACELOST 9999

#if defined( __BORLANDC__ ) && defined( __WIN32__ )
#define _WIN32
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "ddraw.h"
#include <stdlib.h>
#include <stdarg.h>
#include "game.h"
#include "gfx.h"
#include "bubblet.rh"

#define NAME "HOB"
#ifdef REGISTERED_BUILD
#define TITLE "Bubblet! (Registered)"
#else
#define TITLE "Bubblet! (Shareware)"
#endif

typedef LPDIRECTDRAWSURFACE Surface;  //Namen kann man sich besser merken ;-)

LPDIRECTDRAW         dd_Object;    // DirectDraw object
Surface              dd_Primary;   // DirectDraw primary surface
LPDIRECTDRAWCLIPPER  dd_Clipper;   // DirectDraw Clipper
BOOL                 bActive;      // is application active?
Surface              v_screen;     // Virtual Screen
POINT                Client_Pos;   // Position des Fenster-Client
HWND                 main_hwnd;    // Window-Handle
UINT                 TIMER_ID;     // Timer ID
BOOL                 game_initialized= FALSE; // Fertig initialisiert?
HINSTANCE            hInst;

extern void CALLBACK animate(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

static void Shutdown()
{
	 if( dd_Object != NULL )
	 {
		  if( dd_Primary != NULL )
		  {
				dd_Primary->Release();
				dd_Primary = NULL;
		  }
		  dd_Object->Release();
		  dd_Object = NULL;
	 }
}

void Error_Message(LPCSTR str)
{
  //Shutdown();
  MessageBox( 0, str, "ERROR", MB_OK );
}

long FAR PASCAL WindowProc( HWND hWnd, UINT message,
									 WPARAM wParam, LPARAM lParam )
{
  switch( message )
  {
	 case WM_ACTIVATEAPP:
		  bActive = wParam;
		  break;

	 case WM_MOVE:
		RECT rect;

		rect.left= 0;
		rect.top= 0;
		ClientToScreen(main_hwnd, (LPPOINT)&rect);
		Client_Pos.x= rect.left;
		Client_Pos.y= rect.top;
		if(game_initialized) Update_Screen();
		break;

	 case WM_DESTROY:
		Shutdown();
		PostQuitMessage( 0 );
		break;

	 case WM_SURFACELOST:  //DDERR_SURFACELOST-Error von gfx-engine ausgelöst!
      break;
  }

  Game_Message(message, wParam, lParam);

  return DefWindowProc(hWnd, message, wParam, lParam);

} /* WindowProc */

/*
 * Init - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL DirectDrawInit( HINSTANCE hInstance, int nCmdShow )
{
	 HWND                hwnd;
	 WNDCLASS            wc;
	 DDSURFACEDESC       ddsd;
	 HRESULT             ddrval;
	 RECT  client_rect;
	 RECT rect;
	 int x, y;

	 /*
	  * set up and register window class
	  */
	 wc.style = CS_HREDRAW | CS_VREDRAW;
	 wc.lpfnWndProc = WindowProc;
	 wc.cbClsExtra = 0;
	 wc.cbWndExtra = 0;
	 wc.hInstance = hInstance;
	 wc.hIcon = LoadIcon( hInstance, "BUBBLET_ICON"); /*IDI_APPLICATION*/
	 wc.hCursor = LoadCursor( hInstance, MAKEINTRESOURCE(2));
	 wc.hbrBackground = NULL;
	 wc.lpszMenuName = NAME;
	 wc.lpszClassName = NAME;
	 RegisterClass( &wc );

	 /*
	  * create a window
	  */
	 hwnd = CreateWindowEx(
		  0, //WS_EX_TOPMOST
		  NAME,
		  TITLE,
		  WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		  0, 0,
		  0,
		  0,
		  NULL,
		  NULL,
		  hInstance,
		  NULL );

	 if( !hwnd )
	 {
		return FALSE;
	 }

	 main_hwnd= hwnd;

	 //Größe der Client-Area
	 SetRect(&client_rect, 0, 0, 493, 419); //599, 399 neu: 494, 420

	 //Gesamtgröße für das ganze Fenster ermitteln
	 AdjustWindowRectEx(&client_rect, GetWindowStyle(hwnd), /*GetMenu(hwnd)!= NULL*/FALSE,
							GetWindowExStyle(hwnd) );

	 //Komplette Fenstergrröße einstellen

	 x= (GetSystemMetrics(SM_CXSCREEN)/2) - ( (client_rect.right-client_rect.left)/2 );
	 y= (GetSystemMetrics(SM_CYSCREEN)/2) - ( (client_rect.bottom-client_rect.top)/2 );

	 MoveWindow(hwnd, x, y, //x+y-Position des Fensters
					client_rect.right-client_rect.left,
					client_rect.bottom-client_rect.top, FALSE);

//	 GetSystemMetrics(SM_CXSCREEN);
	 rect.left= 0;
	 rect.top= 0;
	 ClientToScreen(hwnd, (LPPOINT)&rect);
	 Client_Pos.x= rect.left;
	 Client_Pos.y= rect.top;

	 ShowWindow( hwnd, nCmdShow );
	 UpdateWindow( hwnd );

	 /*
	  * create the main DirectDraw object
	  */
	 ddrval = DirectDrawCreate( NULL, &dd_Object, NULL );
	 if( ddrval == DD_OK )
	 {
		ddrval = dd_Object->SetCooperativeLevel( hwnd, DDSCL_NORMAL );
		if(ddrval == DD_OK )
		{
		  // Create the primary surface
		  ddsd.dwSize = sizeof( ddsd );
		  ddsd.dwFlags = DDSD_CAPS;
		  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		  ddrval = dd_Object->CreateSurface( &ddsd, &dd_Primary, NULL );
		  if( ddrval == DD_OK )
		  {
			 ddrval= dd_Object->CreateClipper( 0, &dd_Clipper, NULL );
			 if(ddrval==DD_OK)
			 {
				ddrval= dd_Clipper->SetHWnd(0, hwnd);
				if(ddrval==DD_OK)
				{
				  ddrval= dd_Primary->SetClipper(dd_Clipper);
				  if(ddrval==DD_OK)
				  {
					 dd_Clipper->Release();

					 return TRUE;
				  }
				}
			 }
		  }
		}
	 }

	 Error_Message("Direct Draw Initialisierung fehlgeschlagen!");
	 DestroyWindow( hwnd );
	 return FALSE;
} /* Init */

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
								LPSTR lpCmdLine, int nCmdShow)
{
  MSG         msg;
  UINT timer_rate;
  MMRESULT mm_res;

  hInst= hInstance;

  if( !DirectDrawInit( hInstance, nCmdShow ) )
  {
	 return FALSE;
  }
  /*
  timer_rate= 50; //Milliseconds (50!/300/250?)
  SetTimer(main_hwnd, TIMER_ID, timer_rate, NULL);
  */
  TIMER_ID= timeSetEvent(25, 0, &animate, 0, TIME_PERIODIC);
//  if(TIMER_ID==NULL) ErrorMessage("Couldn't initialize multimedia timer!");

  v_screen= CreateOffScreenSurface(dd_Object, 494, 420);

	 /*
	 cursor= LoadCursor(hInstance, "BUBBLET_CURSOR");
	 if(cursor== NULL) Beep(100,100);
	 SetCursor(cursor);
	 ShowCursor(TRUE);
	 */

  Game_Init(); // Initialisiert Spiel

  while (GetMessage(&msg, NULL, 0, 0))
  {
	 TranslateMessage(&msg);
	 DispatchMessage(&msg);
  }

  return msg.wParam;
} /* WinMain */
