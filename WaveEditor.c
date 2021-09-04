#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct Sample{
    signed int data : 24;
};

struct RIFFWAVE
{
  char Riff[5];
  unsigned int chunkLength;
  char Wave[5];
};

struct WaveFileHeader{
  char SubChunkID[5];
  unsigned int SubChunkLength;
  uint16_t AudioFormat;
  uint16_t NumChannels;
  unsigned int SampleRate;
  unsigned int ByteRate;
  int16_t BlockAlign;
  int16_t BitsPerSample;
};

struct WaveDataChunk{
  char DataChunkID[5];
  unsigned int DataChunkLength;
  struct Sample Samples[];
};

static struct RIFFWAVE* ReadRiffWave(struct RIFFWAVE *riffWave,FILE *filePointer)
{
  fread(riffWave->Riff,4,1,filePointer); // RIFF 
  fread(&riffWave->chunkLength,4,1,filePointer); //Size Of Riff Chunk
  fread(riffWave->Wave,4,1,filePointer); //WAVE
};

int main(){
  FILE *fPointer = fopen("C:/Users/shann/Downloads/No turning back MASTER 8_15_21.wav","rb");
  FILE *outPointer = fopen("NewFile.wav","wb");
  struct RIFFWAVE riffWave;
  struct WaveFileHeader waveFileHeader;
  struct WaveDataChunk waveDataChunk;
  if(fPointer == NULL){
    printf("Operation failed unable to find file.\n");
    return 1;
  }

  char junkBuffer[128]; // Used to store and then discard junk data
  char chunkName[5]; // Should only ever be 4 char
  unsigned int chunkLength = 0;
  signed int bits;
  //waveFileHeader.NumChannels = 0;
  // Read in RIFFWAVE
  ReadRiffWave(&riffWave,fPointer);
  //fread(riffWave.Riff,4,1,fPointer); // RIFF 
  //fread(&riffWave.chunkLength,4,1,fPointer); //Size Of Riff Chunk
  //fread(riffWave.Wave,4,1,fPointer); //WAVE
  

  // In this partiuclar file there is a junk chunk
  fread(chunkName,4,1,fPointer);  // 'JUNK'
  fread(&chunkLength,4,1,fPointer); // Length Of Chunk
  fread(junkBuffer,28,1,fPointer); // Read Junk 
  
  // Read WaveFileHeader chunk
  fread(waveFileHeader.SubChunkID,4,1,fPointer); // FMT 
  fread(&waveFileHeader.SubChunkLength,4,1,fPointer); // SubchunkSize 16  
  fread(&waveFileHeader.AudioFormat,2,1,fPointer); // Format 1
  fread(&waveFileHeader.NumChannels,2,1,fPointer); // Num Channels 2
  fread(&waveFileHeader.SampleRate,4,1,fPointer); // Sample Rate 44100
  fread(&waveFileHeader.ByteRate,4,1,fPointer); // Byte Rate 264600
  fread(&waveFileHeader.BlockAlign,2,1,fPointer); // BlockAlign
  fread(&waveFileHeader.BitsPerSample,2,1,fPointer); // Bits Per Sample 24

  // Done reading WaveFileHeader chunk

  // Read Wave Data Chunk
  fread(waveDataChunk.DataChunkID,4,1,fPointer); // Data 'data'
  fread(&waveDataChunk.DataChunkLength,4,1,fPointer); //Size Of Data Chunk
  
// Start reading samples 
  // waveDataChunk.DataChunkLength * 8  / waveFileHeader.NumChannels / waveFileHeader.BitsPerSample   == NumSamples 
  int numSamples = waveDataChunk.DataChunkLength * 8  / waveFileHeader.BitsPerSample;
  struct Sample* samples  = calloc(numSamples,sizeof(struct Sample));
  struct Sample dummySample = {.data=0};
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
  for(int i = 0 ; i < numSamples ; i++){
    
    fread(&tempField,3,1,fPointer);
    //samples[i].data = tempField;
    if(
     ((i /44100) % 30 < 10) && // If Seconds 
     ((i % 2) == 0)){ // Mute Left Audio 
      tempField = 0;
    }
    else if(((i /44100) % 30 > 20 ) && //
     ((i % 2) == 1)){ // Mute Right Audio
      tempField = 0;
    }
    else{

    }
    waveDataChunk.Samples[i].data = tempField;
    fwrite(&tempField,3,1,outPointer);
  }
  fread(chunkName,4,1,fPointer);
  fread(chunkName,4,1,fPointer);


  


  fclose(outPointer);

  free(samples);


  if(fclose(fPointer) == EOF){
    printf("Operation failed.\n");
    return 2;
  }

  printf("Operation succeeded.\n");
  return 0;
}