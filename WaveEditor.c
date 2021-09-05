#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "WaveIO.h"

int main(int argc , char * argv[]){
  printf("The argument supplied is %s\n", argv[1]);
  printf("The argument supplied is %s\n", argv[2]);

  FILE *fPointer = fopen(argv[1],"rb");
  FILE *outPointer = fopen(argv[2],"wb");
  
  RIFFWAVE riffWave;
  struct WaveFileHeader waveFileHeader;
  struct WaveDataChunk waveDataChunk;
  if(fPointer == NULL){
    printf("Operation failed unable to find file.\n");
    return 1;
  }

  ReadRiffWave(&riffWave,fPointer);
  SkipJunkIfExists(fPointer);
  ReadWaveFileHeader(&waveFileHeader,fPointer);
  SkipJunkIfExists(fPointer);
  ReadWaveDataChunkHeader(&waveDataChunk,&waveFileHeader,fPointer);
  ReadWaveDataChunkSamples(&waveDataChunk,&waveFileHeader,fPointer);

  // Edits to the original sound file
  waveFileHeader.SampleRate = 66150; // Dumb Hack To Make Music Sound Like Nightcore
  // Stop Edits 

  WriteRiffWave(&riffWave,outPointer);
  WriteWaveFileHeader(&waveFileHeader,outPointer);
  WriteWaveDataChunkHeader(&waveDataChunk,outPointer);
  WriteWaveDataChunkSamples(&waveDataChunk,outPointer);
  fclose(outPointer);


  free(waveDataChunk.Samples);
  // Cleanup
  if(fclose(fPointer) == EOF){
    printf("Operation failed.\n");
    return 2;
  }

  printf("Operation succeeded.\n");
  return 0;
}