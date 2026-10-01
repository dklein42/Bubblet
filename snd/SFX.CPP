////////////////////////////////////////////////////////////////////
//
//  sfx.cpp sfx-engine v1.00
//  coded by Daniel Klein
//  (c) 1999 House of Bytes Software
//
////////////////////////////////////////////////////////////////////
#include "sfx.h"

HWAVEOUT WaveOut_handle;

BOOL sfx_InitWaveOut()
{
  WAVEFORMATEX wfm;
  MMRESULT result;

  memset(&wfm, 0, sizeof(WAVEFORMATEX));
  wfm.wFormatTag= WAVE_FORMAT_PCM;
  wfm.nChannels= 1;
  wfm.nSamplesPerSec= 44100;
  wfm.wBitsPerSample= 16;
  wfm.nBlockAlign= wfm.wBitsPerSample / 8 * wfm.nChannels;
  wfm.nAvgBytesPerSec= wfm.nSamplesPerSec * wfm.nBlockAlign;

  result= waveOutOpen(&WaveOut_handle, WAVE_MAPPER , &wfm, 0, 0, 0);
  if(result== MMSYSERR_NOERROR) return FALSE;
  return TRUE;
}

BOOL sfx_ShutdownWaveOut()
{
  MMRESULT result;

  result= waveOutClose(WaveOut_handle);
  if(result== MMSYSERR_NOERROR) return FALSE;
  return TRUE;
}

BOOL sfx_PlayRAWFile(LPCSTR filename)
{
  int f;
  void *ptr;
  long size;
  WAVEHDR wavehdr;
  MMRESULT result;

  f= open(filename,O_RDONLY|O_BINARY);
  size= filelength(f); //Dateigröße
  ptr= malloc(size); //Speicher alloziieren
  read(f, &ptr, size);
  close(f);

  memset(&wavehdr, 0, sizeof(WAVEHDR));
  wavehdr.lpData= (char *)ptr;
  wavehdr.dwBufferLength= size;
  wavehdr.dwFlags= 0;

  result= waveOutPrepareHeader(WaveOut_handle, &wavehdr, sizeof(WAVEHDR));
  if(result!=MMSYSERR_NOERROR) return FALSE;
  //Beep(100,100);
  if(waveOutWrite(WaveOut_handle, &wavehdr,
						sizeof(WAVEHDR))!= 0) return FALSE;

  free(ptr);
  return TRUE;
}










//PlaySound-Stuff
void sfx_PlaySoundfile(LPCSTR file)
{
  PlaySound(file, NULL, SND_SYNC|SND_FILENAME);
}

void sfx_PlaySoundfile_Async(LPCSTR file)
{
  PlaySound(file, NULL, SND_ASYNC|SND_FILENAME);
}

//DirectSound-Stuff
/*
LPDIRECTSOUND lpds;

BOOL sfx_Init(HWND hwnd, HINSTANCE hinst)
{
  HRESULT hr;
  LPDIRECTSOUNDBUFFER lpDsb;
  DSBUFFERDESC dsbdesc;
  WAVEFORMATEX wfm;

  //DirectSound-Objekt anlegen
  if(DirectSoundCreate(NULL, &lpds, NULL)!= DS_OK) return FALSE;

  //Kooperationsebene festlegen
  if(lpds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)!= DS_OK) return FALSE;

  //DSBUFFERDESC-Struktur initialisieren
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
  dsbdesc.dwSize= sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags= DSBCAPS_PRIMARYBUFFER;
  dsbdesc.dwBufferBytes= 0; // muß beim primären Puffer 0 sein!
  dsbdesc.lpwfxFormat= NULL; // muß beim primären Puffer NULL sein!

  //beschreibung des gewünschten Formats
  memset(&wfm, 0, sizeof(WAVEFORMATEX));
  wfm.wFormatTag= WAVE_FORMAT_PCM;
  wfm.nChannels= 2;
  wfm.nSamplesPerSec= 44100;
  wfm.wBitsPerSample= 16;
  wfm.nBlockAlign= wfm.wBitsPerSample / 8 * wfm.nChannels;
  wfm.nAvgBytesPerSec= wfm.nSamplesPerSec * wfm.nBlockAlign;

  //Objekte für den primären Soundpuffer
  hr= lpds->CreateSoundBuffer(&dsbdesc, &lpDsb, NULL);
  if(hr== DS_OK)
  {
	 //Format des primären Soundpuffers setzen. Wenn das fehlschlägt,
	 //müssen wir mit dem Standardformat leben.
	 lpDsb->SetFormat(&wfm);
  }

  return TRUE;
}

void sfx_LoadRAWFile(LPDIRECTSOUNDBUFFER &lpdsb, LPCSTR filename )
{
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfm;
  LPVOID ptr= NULL;
  long size;
  LPVOID ds_ptr= NULL;
  DWORD ds_size;
  int f;

  f= open("yeah.wav",O_RDONLY|O_BINARY);
  size= filelength(f); //Dateigröße
  ptr= malloc(size); //Speicher alloziieren
  read(f, ptr, size);
  close(f);

  //WAVEFORMATEX erstellen (fest eingestelltes Format)
  memset(&wfm, 0, sizeof(WAVEFORMATEX));
  wfm.wFormatTag= WAVE_FORMAT_PCM;
  wfm.nChannels= 1;
  wfm.nSamplesPerSec= 22050;
  wfm.wBitsPerSample= 16;
  wfm.nBlockAlign= wfm.wBitsPerSample / 8 * wfm.nChannels;
  wfm.nAvgBytesPerSec= wfm.nSamplesPerSec * wfm.nBlockAlign;

  //DSBUFFERDESC erstellen
  memset(&dsbd, 0, sizeof(DSBUFFERDESC));
  dsbd.dwSize                 = sizeof(DSBUFFERDESC);
  dsbd.dwFlags                = DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC ;
  dsbd.dwBufferBytes          = size; //Größe des Puffers in Bytes
  dsbd.lpwfxFormat            = &wfm; //WAVEFORMATEX-Struktur mit dem Datenformat

  //Puffer erstellen
  lpds->CreateSoundBuffer(&dsbd, &lpdsb, NULL);

  lpdsb->Lock(0,size,&ds_ptr,&ds_size,NULL,NULL,DSBLOCK_ENTIREBUFFER);

  memcpy(ds_ptr, ptr, size);

  free(ptr);

  lpdsb->Unlock(ds_ptr, size, NULL, NULL);
}

void sfx_PlaySound(LPDIRECTSOUNDBUFFER lpdsb)
{
  HRESULT hr;

  if(lpdsb == NULL)
  {
	Beep(100,100);
	return;
  }

  lpdsb->SetCurrentPosition(0);
  lpdsb->Play(0, 0, 0);    // Mehrfach-Aufruf schadet nichts
}

*/
