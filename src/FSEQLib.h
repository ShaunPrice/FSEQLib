/*
 Name:		FSEQLib.h
 Created:	9/18/2018 5:04:31 PM
 Author:	Shaun Price
 Contact:	Via Github website below
 Copyright (C) 2018 Shaun Price
 Website:	https://github.com/ShaunPrice/FSEQLib
 Version 	1.0.2

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
#define FSEQ_VERSION "1.0.2"

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
typedef struct _headerData_t
{
	char	  magic[4];	// Should be "PSEQ"
	uint16_t  dataOffset;
	uint8_t   minorVersion;
	uint8_t   majorVersion;
	uint16_t  headerLength;
	uint16_t  lchannelsPerStep;
	uint16_t  hchannelsPerStep;
	uint16_t  lstepLength;
	uint16_t  hstepLength;
	uint16_t  steptime;
	uint16_t  universes;
	uint16_t  sizeofUniverse;
	uint8_t   gamma;
	uint8_t   lightType;
} headerData_t;
#pragma pop() 


typedef union _HeaderData
{
	headerData_t headerData;
	char rawData[28];	    // Xlights FSEQ Header
} HeaderData;

class FSEQLib
{
private:
	headerData_t _header;
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
	uint32_t stepLength();

	// Delay bewteen steps in milliseconds
	// (20 frames per second = 50, 50 frames per second = 20)
	uint16_t stepTime();

	// Number of universes (often returns 0)
	uint16_t universes();

	// Sixe of the universes (often returns 0)
	uint16_t sizeofUniverses();

	// Lights Gamma (Typically 1)
	uint8_t gamma();

	// Type of lights (RGB or UNKNOWN)
	String lightType();
};