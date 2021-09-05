#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "WaveDataStructs.h"



// Read In The RIFFWAVE Starting Chunk
static void ReadRiffWave(struct RIFFWAVE *riffWave,FILE *filePointer)
{
  fread(riffWave->Riff,4,1,filePointer); // RIFF 
  fread(&riffWave->chunkLength,4,1,filePointer); //Size Of Riff Chunk
  fread(riffWave->Wave,4,1,filePointer); //WAVE
};

static void ReadWaveFileHeader(WaveFileHeader *waveFileHeader,FILE *filepointer){
  fread(waveFileHeader->SubChunkID,4,1,filepointer); // FMT 
  fread(&waveFileHeader->SubChunkLength,4,1,filepointer); // SubchunkSize 16  
  fread(&waveFileHeader->AudioFormat,2,1,filepointer); // Format 1
  fread(&waveFileHeader->NumChannels,2,1,filepointer); // Num Channels 2
  fread(&waveFileHeader->SampleRate,4,1,filepointer); // Sample Rate 44100
  fread(&waveFileHeader->ByteRate,4,1,filepointer); // Byte Rate 264600
  fread(&waveFileHeader->BlockAlign,2,1,filepointer); // BlockAlign
  fread(&waveFileHeader->BitsPerSample,2,1,filepointer); // Bits Per Sample 24
}
//Starts reading the next chunk of the file and skips over it in its entirety if the name is JUNK. 
//Returns a bool indicating if it skipped ahead or not.
static bool SkipJunkIfExists(FILE *filePointer){
  char chunkName[5];
  unsigned int chunkLength = 0;

  fread(chunkName,4,1,filePointer);  // Likely 'JUNK'
  if(strcmp(chunkName,"JUNK") == 0) // If the chunk is named junk ...
  {
    fread(&chunkLength,4,1,filePointer);  //Read in how long it is.
    fseek(filePointer,chunkLength, SEEK_CUR); // Skip over its contents 
    return true;
  }

  fseek(filePointer, -4,SEEK_CUR);// Revert the file reader's position to before we checked for junk
  return false; 
  
}

static void ReadWaveDataChunkHeader(WaveDataChunk *waveDataChunk, WaveFileHeader *waveFileHeader, FILE *filepointer)
{
  fread(waveDataChunk->DataChunkID,4,1,filepointer); // Data 'data'
  fread(&waveDataChunk->DataChunkLength,4,1,filepointer); //Size Of Data Chunk
  waveDataChunk->NumSamples = waveDataChunk->DataChunkLength * 8  / waveFileHeader->BitsPerSample;
  waveDataChunk->Samples = calloc(waveDataChunk->NumSamples,sizeof(struct Sample));
}

static void ReadWaveDataChunkSamples(WaveDataChunk * waveDataChunk,FILE *filePointer,FILE *outPointer)
{
  int tempField = 0;
  for(int i = 0 ; i < waveDataChunk->NumSamples ; i++){
    fread(&tempField,3,1,filePointer);    
    waveDataChunk->Samples[i].data = tempField;
  }
}