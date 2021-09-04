#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
struct Sample{
    signed int data : 24;
};

typedef struct RIFFWAVE
{
  char Riff[5];
  unsigned int chunkLength;
  char Wave[5];
} RIFFWAVE;

typedef struct WaveFileHeader{
  char SubChunkID[5];
  unsigned int SubChunkLength;
  uint16_t AudioFormat;
  uint16_t NumChannels;
  unsigned int SampleRate;
  unsigned int ByteRate;
  int16_t BlockAlign;
  int16_t BitsPerSample;
} WaveFileHeader;

typedef struct WaveDataChunk{
  char DataChunkID[5];
  unsigned int DataChunkLength;
  int NumSamples;
  struct Sample* Samples; // Pointer to an array pointer
} WaveDataChunk;

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
  fseek(filePointer, -8,SEEK_CUR);// Revert the file reader's position to before we checked for junk
  return false; 
  
}

static void ReadWaveDataChunkHeader(WaveDataChunk *waveDataChunk, WaveFileHeader *waveFileHeader, FILE *filepointer)
{
  fread(waveDataChunk->DataChunkID,4,1,filepointer); // Data 'data'
  fread(&waveDataChunk->DataChunkLength,4,1,filepointer); //Size Of Data Chunk
  waveDataChunk->NumSamples = waveDataChunk->DataChunkLength * 8  / waveFileHeader->BitsPerSample;
  waveDataChunk->Samples = calloc(waveDataChunk->NumSamples,sizeof(struct Sample));
}
int main(){
  FILE *fPointer = fopen("C:/Users/shann/Downloads/No turning back MASTER 8_15_21.wav","rb");
  FILE *outPointer = fopen("NewFile.wav","wb");
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
  //SkipJunkIfExists(fPointer);
  ReadWaveDataChunkHeader(&waveDataChunk,&waveFileHeader,fPointer);

  int tempField = 0;

  fwrite(riffWave.Riff,4,1,outPointer); // Write RIFF
  fwrite(&riffWave.chunkLength,4,1,outPointer); // Write Length
  fwrite(riffWave.Wave,4,1,outPointer); // Write WAVE

  fwrite(waveFileHeader.SubChunkID,4,1,outPointer); // FMT 
  fwrite(&waveFileHeader.SubChunkLength,4,1,outPointer); // SubchunkSize 16  
  fwrite(&waveFileHeader.AudioFormat,2,1,outPointer); // Format 1
  fwrite(&waveFileHeader.NumChannels,2,1,outPointer); // Num Channels 2
  fwrite(&waveFileHeader.SampleRate,4,1,outPointer); // Sample Rate 44100
  fwrite(&waveFileHeader.ByteRate,4,1,outPointer); // Byte Rate 264600
  fwrite(&waveFileHeader.BlockAlign,2,1,outPointer); // BlockAlign
  fwrite(&waveFileHeader.BitsPerSample,2,1,outPointer); // Bits Per Sample 24
  fwrite(waveDataChunk.DataChunkID,4,1,outPointer); // Data 'data'
  fwrite(&waveDataChunk.DataChunkLength,4,1,outPointer); //Size Of Data Chunk
  for(int i = 0 ; i < waveDataChunk.NumSamples ; i++){
    
    fread(&tempField,3,1,fPointer);
    //samples[i].data = tempField;
    
    waveDataChunk.Samples[i].data = tempField;
    fwrite(&tempField,3,1,outPointer);
  }
  //fread(chunkName,4,1,fPointer);
  //fread(chunkName,4,1,fPointer);


  


  fclose(outPointer);

  free(waveDataChunk.Samples);


  if(fclose(fPointer) == EOF){
    printf("Operation failed.\n");
    return 2;
  }

  printf("Operation succeeded.\n");
  return 0;
}