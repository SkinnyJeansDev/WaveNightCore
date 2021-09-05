#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "WaveDataStructs.h"


//READ

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
  waveDataChunk->Samples = calloc(waveDataChunk->NumSamples,sizeof(union Sample));
}

static void ReadWaveDataChunkSamples(WaveDataChunk * waveDataChunk,WaveFileHeader *waveFileHeader,FILE *filePointer)
{
  int tempField = 0;
  int sizeOfSampleInBytes = waveFileHeader->BitsPerSample / 8; // Should always be an integer in a valid wave file

  // I could invert the for and conditionals (if,if else) to be more performant but this is more readable for now...
  for(int i = 0 ; i < waveDataChunk->NumSamples ; i++){
    fread(&tempField,sizeOfSampleInBytes,1,filePointer);
    if(sizeOfSampleInBytes == 3){
      waveDataChunk->Samples[i].Data24Bit = tempField;
    } 
    else if(sizeOfSampleInBytes == 2){
      waveDataChunk->Samples[i].Data16Bit = tempField;
    } 
    else if((sizeOfSampleInBytes == 1))
    {
      waveDataChunk->Samples[i].Data8Bit = tempField;
    }
    else{
      printf("You've provided a file with an unexpected ByteRate not currently supported by this application. Shutting down...");
      exit(3); //I've read this is OK as the OS will reclaim memory I allocated on the heap
    }  
  }
}


// WRITE


static void WriteRiffWave(RIFFWAVE* riffWave, FILE* outPointer){
  fwrite(riffWave->Riff,4,1,outPointer); // Write RIFF
  fwrite(&riffWave->chunkLength,4,1,outPointer); // Write Length
  fwrite(riffWave->Wave,4,1,outPointer); // Write WAVE
}

static void WriteWaveFileHeader(WaveFileHeader* waveFileHeader, FILE* outPointer)
{
  fwrite(waveFileHeader->SubChunkID,4,1,outPointer); // FMT 
  fwrite(&waveFileHeader->SubChunkLength,4,1,outPointer); // SubchunkSize 16  
  fwrite(&waveFileHeader->AudioFormat,2,1,outPointer); // Format 1
  fwrite(&waveFileHeader->NumChannels,2,1,outPointer); // Num Channels 2
  fwrite(&waveFileHeader->SampleRate,4,1,outPointer); // Sample Rate 44100
  fwrite(&waveFileHeader->ByteRate,4,1,outPointer); // Byte Rate 264600
  fwrite(&waveFileHeader->BlockAlign,2,1,outPointer); // BlockAlign
  fwrite(&waveFileHeader->BitsPerSample,2,1,outPointer); // Bits Per Sample 24
}

static void WriteWaveDataChunkHeader(WaveDataChunk* waveDataChunk, FILE* outPointer){
  fwrite(waveDataChunk->DataChunkID,4,1,outPointer); // Data 'data'
  fwrite(&waveDataChunk->DataChunkLength,4,1,outPointer); //Size Of Data Chunk

}

static void WriteWaveDataChunkSamples(WaveDataChunk *waveDataChunk,WaveFileHeader *waveFileHeader,FILE *filePointer){
  int temp;
  int sizeOfSampleInBytes = waveFileHeader->BitsPerSample / 8;
  for(int i = 0 ; i < waveDataChunk->NumSamples ; i++){
    if(sizeOfSampleInBytes == 3)
    {
      temp = waveDataChunk->Samples[i].Data24Bit;
    }
    else if(sizeOfSampleInBytes == 2)
    {
      temp = waveDataChunk->Samples[i].Data16Bit;
    }
    
    else if(sizeOfSampleInBytes == 1)
    {
      temp = waveDataChunk->Samples[i].Data8Bit;
    }
    fwrite(&temp,sizeOfSampleInBytes,1,filePointer);
      
  }
}