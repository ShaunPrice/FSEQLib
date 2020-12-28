/*
Name:		FSEQLib.cpp
Created:	9/18/2018 5:04:31 PM
Author:		Shaun Price
Contact:	Via Github website below
Copyright (C) 2018-2020 Shaun Price
Website:	https://github.com/ShaunPrice/FSEQLib

Version 2.0.0

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

#include "FSEQLib.h"

/// XLights FSEQ file library

FSEQLib::FSEQLib()
{

}

FSEQLib::FSEQLib(HeaderData header)
{
	_header.headerData = header.headerData;
}

// Magic number (PSEQ) for FSEQ files.
String FSEQLib::magic()
{
	char l_buffMagic[5] = {0};

	memcpy(&l_buffMagic[0], &_header.headerData.magic[0], 4);
#if !defined(ARDUINO)
	return String(l_buffMagic, 4);
#else
	return String((const __FlashStringHelper*)l_buffMagic);
#endif
}

// Offset of the lights data from the header file
uint16_t FSEQLib::dataOffset()
{
	return _header.headerData.dataOffset;
}
	
// FSEQ major version
uint8_t FSEQLib::majorVersion()
{
	return _header.headerData.majorVersion;
}
	
// FSEQ minor version
uint8_t FSEQLib::minorVersion()
{
	return _header.headerData.minorVersion;
}
	
// Length of the FSEQ  header
uint16_t FSEQLib::headerLength()
{
	return (int)_header.headerData.headerLength;
}
	
// Number of channels per step
uint32_t FSEQLib::channelsPerStep()
{
	return _header.headerData.channelsPerStep;
}
	
// Lenght of the step
uint32_t FSEQLib::sequenseSteps()
{
	return _header.headerData.sequenseSteps;
}
	
// Delay bewteen steps in milliseconds
// (20 frames per second = 50, 50 frames per second = 20)
uint8_t FSEQLib::stepTime()
{
	return (int)_header.headerData.steptime;
}
	
// Number of universes (Not used by fpp. Returns 0)
uint16_t FSEQLib::universes()
{
	if (_header.headerData.majorVersion == 1)
		return _header.headerData_v1.universes;
	else
		return 0;
}
	
// Size of the universes (Not used by fpp. Returns 0)
uint16_t FSEQLib::sizeofUniverses()
{
	if (_header.headerData.majorVersion == 1)
		return _header.headerData_v1.sizeofUniverse;
	else
		return 0;
}
	
// Lights Gamma (Typically 1)
uint8_t FSEQLib::gamma()
{
	if (_header.headerData.majorVersion == 1)
		return _header.headerData_v1.gamma;
	else
		return 1;
}
	
// Type of lights (RGB or UNKNOWN)
String FSEQLib::colorOrder()
{
	if (_header.headerData.majorVersion == 1)
		return (_header.headerData_v1.colorOrder == 0) ? "RGB" : "UNKNOWN";
	else
		return "UNKNOWN";
}

// V2 Compresion Type
// Bits 0-3 - compression type 0 for uncompressed, 1 for zstd, 2 for libz/gzip
uint8_t FSEQLib::compressionType()
{
	 
	if (_header.headerData.majorVersion == 2)
		return (_header.headerData.compressionType & 0x0f);
	else
		return 0; // Uncompressed
}

// V2 Compresion Type Name
// Bits 0-3 - compression type 0 for uncompressed, 1 for zstd, 2 for libz/gzip
String FSEQLib::compressionTypeName()
{
	uint8_t l_type = (_header.headerData.compressionType & 0x0f);

	switch (l_type)
	{
		case 0:
			return "Uncompressed";
		case 1:
			return "ZStd";
		case 2:
			return "LibZ/Gzip";
		default:
			return "Unknown";
	}
}

// V2 Compressed Blocks
// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
uint8_t FSEQLib::compressedBlocks()
{
	if (_header.headerData.majorVersion == 2)
		return (_header.headerData.compressionType & 0xf0) * 256 + _header.headerData.compressedBlocks; // The top four bits are in the CompressionType field
	else
		return 0;
}

// V2 Sparse Ranges
// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
uint8_t FSEQLib::sparseRanges()
{
	if (_header.headerData.majorVersion == 2)
		return _header.headerData.sparseRanges;
	else
		return 0;
}

// V2 UUID
// Number of compression blocks, 0 if uncompressed, lower 8 bits. Total 12 bits.
uint64_t FSEQLib::uuid()
{
	if (_header.headerData.majorVersion == 2)
		return _header.headerData.uuid;
	else
		return 0;
}