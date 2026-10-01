// sfx-engine Test-Program
// by Daniel Klein

#include<stdio.h>
#include<windows.h>
#include "sfx.h"

int main()
{
  BOOL result;

  result= sfx_InitWaveOut();
  if(!result) printf("- WaveOut-Init failed!\n");
  if(result) printf("- WaveOut-Init ..OK\n");

  result= sfx_PlayRAWFile("hehe.wav");
  if(!result) printf("- WaveOut-Play failed!\n");
  if(result) printf("- WaveOut-Play ..OK\n");

  result= sfx_ShutdownWaveOut();
  if(!result) printf("- WaveOut-Shutdown failed!\n");
  if(result) printf("- WaveOut-Shutdown ..OK\n");

  printf("- done\n");
  return 0;
}
