////////////////////////////////////////////////////////////////////
//
//  sfx.h Header-File
//  coded by Daniel Klein
//  (c) 1999 House of Bytes
//
////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
//#include "dsound.h"


//waveout
extern BOOL sfx_InitWaveOut();
extern BOOL sfx_ShutdownWaveOut();
extern BOOL sfx_PlayRAWFile(LPCSTR filename);


//playsound
extern void sfx_PlaySoundfile(LPCSTR file);
extern void sfx_PlaySoundfile_Async(LPCSTR file);

//DirectSound
//extern HINSTANCE hInst;
//extern BOOL sfx_Init(HWND hwnd, HINSTANCE hinst);
//extern void sfx_LoadRAWFile(LPDIRECTSOUNDBUFFER &lpdsb, LPCSTR filename);
//extern void sfx_PlaySound(LPDIRECTSOUNDBUFFER lpdsb);

