////////////////////////////////////////////////////////////////////////
//
// mci.cpp
// mci-library v1.00 by Daniel Klein 05-14-99
// (c) 1999 House of Bytes Software
//
// this library provides support for wave-file and cdaudio
// playback via the mci-engine
//
////////////////////////////////////////////////////////////////////////
// *BUG*= mci_PlayCDTrack() doesn't stop after track. Maybe the 'open'- and 'close'-calls have to be external-functions like waveaudio needs?
// *TODO*= mci_GetLastErrorStr() which delivers the last Error-Code as string!?

#include <windows.h>
#include "mci.h"

MCIERROR mci_LastError;

////////////////////////////////////////////////////////////////////////
//
// Executes any given mci-type command-string.
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_ExecStr(char* command_str)
{
  mci_LastError= mciSendString(command_str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Opens a wave file with the alias 'alias'
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_OpenWave(char* filename, char* alias)
{
  char str[255];

  wsprintf(str, "open %s type waveaudio alias %s", filename, alias);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Closes a wave file with the alias 'alias'
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_CloseWave(char* alias)
{
  char str[255];

  wsprintf(str, "close %s", alias);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Plays the wave file associated with 'alias'
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_PlayWave(char* alias)
{
  char str[255];

  wsprintf(str, "play %s from 0", alias);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Plays the wave file associated with 'alias' and waits until it's
// finnished. Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_PlayWaveWait(char* alias)
{
  char str[255];

  wsprintf(str, "play %s from 0 wait", alias);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Stops the wave file associated with 'alias'.
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_StopWave(char* alias)
{
  char str[255];

  wsprintf(str, "stop %s", alias);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Plays the CD from actual position to the end.
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_PlayCD()
{
  char str[255];

  wsprintf(str, "open cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "play cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "close cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Plays 'track' of the CD.
// Returns TRUE if succeed.
// *BUG*= doesn't stop after track. Maybe the 'open' and 'close'-call have to be external?
////////////////////////////////////////////////////////////////////////
BOOL mci_PlayCDTrack(int track)
{
  char str[255];

  wsprintf(str, "open cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "set cdaudio time format track");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "play cdaudio from %u to %u", track, track);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "close cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Plays from 'track' to the end of the CD.
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_PlayCDFromTrack(int track)
{
  char str[255];

  wsprintf(str, "open cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "set cdaudio time format track");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "play cdaudio from %u", track);
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "close cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Stops playback of CD.
// Returns TRUE if succeed.
//
////////////////////////////////////////////////////////////////////////
BOOL mci_StopCD()
{
  char str[255];

  wsprintf(str, "open cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "stop cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;

  wsprintf(str, "close cdaudio");
  mci_LastError= mciSendString(str, NULL, 0, NULL);
  if(mci_LastError!=0) return FALSE;
  return TRUE;
}
