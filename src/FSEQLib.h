/*
 Name:		FSEQLib.h
 Created:	9/18/2018 5:04:31 PM
 Author:	Shaun Price
 Contact:	Via Github website below
 Copyright (C) 2018-2020 Shaun Price
 Website:	https://github.com/ShaunPrice/FSEQLib
 Version 	2.0.0

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#define FSEQ_VERSION "2.0.0"

#ifndef _FSEQLib_h
#define _FSEQLib_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#elif defined(ARDUINO) && ARDUINO < 100
#include "WProgram.h"
#else
#include <string>
#define String std::string
#endif

#endif

#pragma pack(push, 1)
typedef struct _headerData_v1_0
{
	uint8_t	  magic[4];			// Should be "PSEQ"
	uint16_t  dataOffset;		// Begining of the data
	uint8_t   minorVersion;		// Minor version of the file format = 0
	uint8_t   majorVersion;		// Major version of the file format = 1
	uint16_t  headerLength;		// Length of this header = 28
	uint32_t  channelsPerStep;  // Number of channels per step
	uint32_t  sequenseSteps;	// Number of steps in the sequence
	uint8_t   steptime;			// Milliseconds per step
	uint8_t   flags;			// Flags (unused/reserved) = 0
	uint16_t  universes;		// Not used by fpp
	uint16_t  sizeofUniverse;	// Not used by fpp
	uint8_t   gamma;			// Gamma - should be 1
	uint8_t   colorOrder;		// Color order (unused by fpp) = 2 
	uint16_t  reserved;			// Reserved, should be 0
} headerData_v1;
#pragma pop() 

#pragma pack(push, 1)
typedef struct _headerData_v2_0
{
	uint8_t	  magic[4];			// Should be "PSEQ"
	uint16_t  dataOffset;		// Begining of the data
	uint8_t   minorVersion;		// Minor version of the file format = 0
	uint8_t   majorVersion;		// Major version of the file format = 2
	uint16_t  headerLength;		// Length of this header = (32 bytes) + Compression Block Count * length of a Compression Block (8 bytes) + Channel Count * length of a Channel (12 bytes)
	uint32_t  channelsPerStep;  // Number of channels per step
	uint32_t  sequenseSteps;	// Number of steps in the sequence
	uint8_t   steptime;			// Milliseconds per step
	uint8_t   flags;			// Flags (unused/reserved) = 0
	uint8_t   compressionType;  // Bits 0-3 - compression type 0 for uncompressed, 1 for zstd, 2 for libz/gzip. Bits 4 - 7 - number of compression blocks, upper 4 bits - introduced in FSEQ 2.1.
	uint8_t   compressedBlocks; // Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
	uint8_t   sparseRanges;		// Number of sparse ranges  = 0 
	uint8_t   flags2;			// Flags (unused/reserved) = 0
	uint64_t  uuid;				// 64bit unique identifier, likely a timestamp or UUID 
} headerData_v2;
#pragma pop() 

typedef union _HeaderData
{
	_headerData_v1_0 headerData_v1;
	_headerData_v2_0 headerData, headerData_v2;
	char rawData[32];	    // Xlights FSEQ Header
} HeaderData;

class FSEQLib
{
private:
	HeaderData _header;
public:
	FSEQLib();
	FSEQLib(HeaderData header);

	// Magic number (PSEQ) for FSEQ files.
	String magic();

	// Offset of the lights data from the header file
	uint16_t dataOffset();
	
	// FSEQ major version
	uint8_t majorVersion();

	// FSEQ minor version
	uint8_t minorVersion();

	// Length of the FSEQ  header
	uint16_t headerLength();

	// Number of channels per step
	uint32_t channelsPerStep();

	// Lenght of the step
	uint32_t sequenseSteps();

	// Delay bewteen steps in milliseconds
	// (20 frames per second = 50, 50 frames per second = 20)
	uint8_t stepTime();

	// Number of universes (Not used by fpp. Returns 0)
	uint16_t universes();

	// Size of the universes (Not used by fpp. Returns 0)
	uint16_t sizeofUniverses();

	// Lights Gamma (Typically 1)
	uint8_t gamma();

	// Type of lights (RGB or UNKNOWN)
	String colorOrder();

	// V2 Compresion Type
	// Bits 0-3 - compression type 0 for uncompressed, 1 for zstd, 2 for libz/gzip
	uint8_t compressionType();

	// V2 String version of compressionType
	// Bits 0-3 - compression type 0 for uncompressed, 1 for zstd, 2 for libz/gzip
	String compressionTypeName();

	// V2 Compressed Blocks
	// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
	// Bits 4-7 of CompressionType - number of compression blocks, upper 4 bits - introduced in FSEQ 2.1
	uint8_t compressedBlocks();
	
	// V2 Sparse Ranges
	// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
	uint8_t sparseRanges();

	// V2 UUID
	// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
	uint64_t uuid();

};
