#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "WaveReader.h"


static void WriteWaveDataChunkSamples(WaveDataChunk * waveDataChunk,FILE *filePointer){
  int temp;
  for(int i = 0 ; i < waveDataChunk->NumSamples ; i++){    
      temp = waveDataChunk->Samples[i].data;
      //fwrite(&tempField,3,1,outPointer);
      fwrite(&temp,3,1,filePointer);
  }
}

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
  
  int tempField = 0;

  fwrite(riffWave.Riff,4,1,outPointer); // Write RIFF
  fwrite(&riffWave.chunkLength,4,1,outPointer); // Write Length
  fwrite(riffWave.Wave,4,1,outPointer); // Write WAVE

  unsigned int nightcoreVar = 66150; // Dumb Hack
  fwrite(waveFileHeader.SubChunkID,4,1,outPointer); // FMT 
  fwrite(&waveFileHeader.SubChunkLength,4,1,outPointer); // SubchunkSize 16  
  fwrite(&waveFileHeader.AudioFormat,2,1,outPointer); // Format 1
  fwrite(&waveFileHeader.NumChannels,2,1,outPointer); // Num Channels 2
  fwrite(&nightcoreVar,4,1,outPointer); // Sample Rate 44100
  fwrite(&waveFileHeader.ByteRate,4,1,outPointer); // Byte Rate 264600
  fwrite(&waveFileHeader.BlockAlign,2,1,outPointer); // BlockAlign
  fwrite(&waveFileHeader.BitsPerSample,2,1,outPointer); // Bits Per Sample 24
  fwrite(waveDataChunk.DataChunkID,4,1,outPointer); // Data 'data'
  fwrite(&waveDataChunk.DataChunkLength,4,1,outPointer); //Size Of Data Chunk
  
  ReadWaveDataChunkSamples(&waveDataChunk,fPointer,outPointer);
  WriteWaveDataChunkSamples(&waveDataChunk,outPointer);
  fclose(outPointer);

  free(waveDataChunk.Samples);


  if(fclose(fPointer) == EOF){
    printf("Operation failed.\n");
    return 2;
  }

  printf("Operation succeeded.\n");
  return 0;
}