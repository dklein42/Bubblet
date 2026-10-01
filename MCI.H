////////////////////////////////////////////////////////////////////////
//
// mci.h Header-File
// mci-library v1.00 by Daniel Klein 05-14-99
// (c) 1999 House of Bytes Software
//
// this library provides support for wave-file and cdaudio
// playback via the mci-engine
//
////////////////////////////////////////////////////////////////////////

extern MCIERROR mci_LastError;

extern BOOL mci_ExecStr(char* command_str);
extern BOOL mci_OpenWave(char* filename, char* alias);
extern BOOL mci_CloseWave(char* alias);
extern BOOL mci_PlayWave(char* alias);
extern BOOL mci_PlayWaveWait(char* alias);
extern BOOL mci_StopWave(char* alias);
extern BOOL mci_PlayCD();
extern BOOL mci_PlayCDTrack(int track);
extern BOOL mci_PlayCDFromTrack(int track);
extern BOOL mci_StopCD();
