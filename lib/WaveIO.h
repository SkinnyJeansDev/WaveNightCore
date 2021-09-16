#ifndef WAVEIO
#define WAVEIO

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "WaveDataStructs.h"


//READ

// Read In The RIFFWAVE Starting Chunk
void ReadRiffWave(struct RIFFWAVE *riffWave,FILE *filePointer);
void ReadWaveFileHeader(WaveFileHeader *waveFileHeader,FILE *filepointer);
//Starts reading the next chunk of the file and skips over it in its entirety if the name is JUNK. 
//Returns a bool indicating if it skipped ahead or not.
bool SkipJunkIfExists(FILE *filePointer);
void ReadWaveDataChunkHeader(WaveDataChunk *waveDataChunk, WaveFileHeader *waveFileHeader, FILE *filepointer);
void ReadWaveDataChunkSamples(WaveDataChunk * waveDataChunk,WaveFileHeader *waveFileHeader,FILE *filePointer);

// WRITE
void WriteRiffWave(RIFFWAVE* riffWave, FILE* outPointer);
void WriteWaveFileHeader(WaveFileHeader* waveFileHeader, FILE* outPointer);
void WriteWaveDataChunkHeader(WaveDataChunk* waveDataChunk, FILE* outPointer);
void WriteWaveDataChunkSamples(WaveDataChunk *waveDataChunk,WaveFileHeader *waveFileHeader,FILE *filePointer);

#endif
