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