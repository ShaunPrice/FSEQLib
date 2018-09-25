/*
Name:		FSEQLib.cpp
Created:	9/18/2018 5:04:31 PM
Author:		Shaun Price
Contact:	Via Github website below
Copyright (C) 2018 Shaun Price
Website:	https://github.com/ShaunPrice/FSEQLib

Version 1.0.2

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
	_header = header.headerData;
}

// Magic number (PSEQ) for FSEQ files.
String FSEQLib::magic()
{
#if !defined(ARDUINO)
	return String(_header.magic, 4);
#else
	return String((const __FlashStringHelper*)_header.magic);
#endif
}

// Offset of the lights data from the header file
uint16_t FSEQLib::dataOffset()
{
	return _header.dataOffset;
}
	
// FSEQ major version
uint8_t FSEQLib::majorVersion()
{
	return _header.majorVersion;
}
	
// FSEQ minor version
uint8_t FSEQLib::minorVersion()
{
	return _header.minorVersion;
}
	
// Length of the FSEQ  header
uint16_t FSEQLib::headerLength()
{
	return (int)_header.headerLength;
}
	
// Number of channels per step
uint32_t FSEQLib::channelsPerStep()
{
	return (uint32_t)_header.lchannelsPerStep + (uint32_t)(_header.hchannelsPerStep * 65536);
}
	
// Lenght of the step
uint32_t FSEQLib::stepLength()
{
	return (uint32_t)_header.lstepLength + (uint32_t)(_header.hstepLength * 65536);
}
	
// Delay bewteen steps in milliseconds
// (20 frames per second = 50, 50 frames per second = 20)
uint16_t FSEQLib::stepTime()
{
	return (int)_header.steptime;
}
	
// Number of universes (often returns 0)
uint16_t FSEQLib::universes()
{
	return _header.universes;
}
	
// Sixe of the universes (often returns 0)
uint16_t FSEQLib::sizeofUniverses()
{
	return _header.sizeofUniverse;
}
	
// Lights Gamma (Typically 1)
uint8_t FSEQLib::gamma()
{
	return _header.gamma;
}
	
// Type of lights (RGB or UNKNOWN)
String FSEQLib::lightType()
{
	return (_header.lightType == 0) ? "RGB" : "UNKNOWN";
}

