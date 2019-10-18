/*
Name:		FSEQLib.cpp
Created:	9/18/2018 5:04:31 PM
Author:	Shaun Price
Contact:	Via Github website below
Copyright (C) 2018 Shaun Price
Website:	https://github.com/ShaunPrice/FSEQLib

Version 1.0.3

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
/*
SD Card VSPI PINS (ESP32 - Wemos Lolin32 Lite)
==============================================
SCLK = 18
MISO = 19
MOSI = 23
SS = 5

SD Card SPI PINS (ESP8266 - Wemos D1 R2 mini)
==============================================
SCLK = D5/GPIO 5
MISO = D6/GPIO 12
MOSI = D7/GPIO 13
SS   = D8/GPIO 15 

*/

#if !defined ESP8266
#include <FS.h>
#endif
#include <FastLED.h>
#include <SD.h>
#include <SPI.h>
#include "FSEQLib.h"

#define DEBUG 0 // 0=OFF, 1=Serial

// Serial Debug Info
#if (DEBUG == 1)
#define SERIAL_BEGIN(x) Serial.begin(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define SERIAL_BEGIN
#define DEBUG_PRINT
#define DEBUG_PRINTLN
#endif


#if defined ESP8266
#define DATA_PIN_1 D1			// Data pin for universe 1.
#define CARD_DETECT_PIN  D2		// May require 10k pull-up
#elif defined ESP32
#define DATA_PIN_1 13			// Data pin for universe 1.
#define CARD_DETECT_PIN  17		// May require 10k pull-up
#else
#define DATA_PIN_1 13			// Data pin for universe 1.
#define CARD_DETECT_PIN  17		// May require 10k pull-up
#endif

// This is where the second universe would go if using more than one universe
//#define DATA_PIN_2 14	// Data pin for universe 2. 

#define FSEQ_FILE "/show.dat"	// Name of the FSEQ file to play
#define UNIVERSES 1				// Universes aren't really defined here but I use the term to define the 
								//  number of times I want to split up the sequence step channels into.
#define NUM_NODES 240			// Nodes/Pixels
#define NUM_CHANNELS_PER_NODE 3 // Number of channels/LEDs per Node/Pixel
#define LEDS_PER_UNIVERSE NUM_NODES * NUM_CHANNELS_PER_NODE	// LEDs/channels per universe
#define BUFFER_LENGTH (UNIVERSES * LEDS_PER_UNIVERSE) + ((4 - (UNIVERSES * LEDS_PER_UNIVERSE) % 4) % 4) // Buffer is 32 bit (4 byte) padded.
// Adjust the brightness of the display
// For best results leave brightness at 255 (100%)
// and correct the brightness with Xlights.
#define BRIGHTNESS 255

size_t bytesRead = 0;
File dataFile;

// SPI
char stepBuffer[BUFFER_LENGTH];
CRGB leds[UNIVERSES][NUM_NODES];
//uint8_t universeBuffer[LEDS_PER_UNIVERSE]; // If using uint8_t buffer rather than CRGB buffer

uint32_t sequenceDelay = 50; // This will get set from the FSEQ header
int32_t currentStep = 0;
HeaderData rawHeader;
FSEQLib header;

bool cardInitialised = false;
bool cardDetected = false;


void setup()
{
	SERIAL_BEGIN(115200);

	pinMode(SS, OUTPUT); // SD Card VSPI SS
	pinMode(CARD_DETECT_PIN, INPUT_PULLUP); // SD Card Detected CD 
	
	FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds[0], NUM_NODES);
	// The second universe would be:
	//FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds[1], NUM_NODES);

	// Adjust the brightness of the display
	// For best results leave brightness at 255 (100%)
	// and correct the brightness with Xlights.
	FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
	unsigned long startMillis = millis();
	cardDetected = (digitalRead(CARD_DETECT_PIN) == 1) ? true : false;
	// Initialise SPI Master

	// See if the card is present and can be initialized:
	if (cardDetected && !cardInitialised)
	{
		DEBUG_PRINT("Initializing SD card...");

		if (SD.begin())
		{
			DEBUG_PRINTLN("card initialized.");

			// Open the file.
			// Note that only one file can be open at a time,
			// so you have to close this one before opening another.
			// Also, this only supports 8.3 format so we need to 
			// rename the file from xxx.fseq to something else.
			dataFile = SD.open(FSEQ_FILE);
			DEBUG_PRINTLN("File size: " + String(dataFile.size()));
			if (dataFile.size() > 0)
			{
				cardInitialised = true;

				dataFile.readBytes(rawHeader.rawData, 28);

				header = FSEQLib(rawHeader);
				
				// DEBUG code to print out the header details
				DEBUG_PRINTLN("======================");
				DEBUG_PRINTLN("== Xlights FSEQ Header");
				DEBUG_PRINTLN("======================");
				DEBUG_PRINTLN("Magic: " + header.magic());
				DEBUG_PRINTLN("Data Offset: " + String(header.dataOffset()));
				DEBUG_PRINTLN("Version: " + String(header.majorVersion())+"."+String(header.minorVersion()));
				DEBUG_PRINTLN("Header Length: " + String(header.headerLength()));
				DEBUG_PRINTLN("Channels per Step: " + String(header.channelsPerStep()));
				DEBUG_PRINTLN("Number of Steps: " + String(header.stepLength()));
				DEBUG_PRINTLN("Step Time (ms): " + String(header.stepTime()));
				DEBUG_PRINTLN("Universes: " + String((header.universes() == 0) ? "N/A" : String(header.universes())));
				DEBUG_PRINTLN("Size of Universe: " + String((header.sizeofUniverses() == 0) ? "N/A" : String(header.sizeofUniverses())));
				DEBUG_PRINTLN("Gamma: " + String(header.gamma()));
				DEBUG_PRINTLN("Light Type: " + String(header.lightType()));
				DEBUG_PRINTLN("======================");

				DEBUG_PRINTLN("done!");

				// Set the data offset
				dataFile.seek(header.dataOffset());

				sequenceDelay = header.stepTime();
			}
			else
			{
				cardInitialised = false;
				dataFile.close();
			}
		}
		else
		{
			DEBUG_PRINTLN("Card failed, or not present");
			delay(500);
		}
	}
	else if (!cardDetected && cardInitialised)
	{
		DEBUG_PRINTLN("Card removed");
		cardInitialised = false;
		dataFile.close();
		#if !defined ESP8266
		SD.end();
		#endif
	}
	else if (cardDetected && cardInitialised)
	{
		// Read the channels for the next step
		int datalen = dataFile.readBytes(stepBuffer, BUFFER_LENGTH);
		
		if (datalen != BUFFER_LENGTH)
		{
			DEBUG_PRINTLN("Buffer Failed to load. Closing File and SD card");
			cardInitialised = false;
			dataFile.close();
			#if !defined ESP8266
			SD.end();
			#endif
		}
		else
		{
			// Output data
			for (uint8_t current_universe = 0; current_universe < UNIVERSES; current_universe++)
			{
				// Copy the led values into the universe buffer
				memcpy(&leds[current_universe], &stepBuffer[current_universe * LEDS_PER_UNIVERSE], LEDS_PER_UNIVERSE);
				//memcpy(&universeBuffer[0], &stepBuffer[current_universe * LEDS_PER_UNIVERSE], LEDS_PER_UNIVERSE); // If using uint8_t buffer rather than CRGB buffer

				// Send the data
				FastLED.show();
			}
			
			currentStep++;

			// Reset to first step if we have gone past the last step
			if (currentStep == header.stepLength())
			{
				// Restart at first step
				currentStep = 0;
				// Restart file after header 
				//Corrected offset starting point or it will overlap with multiple universes
				dataFile.seek(header.dataOffset());
			}
		}
	}
	// Delay to make sure we send the number of times specified per second.
	// The delay is constrained between 1ms and 50ms
	delay(constrain(sequenceDelay - (millis() - startMillis), 1, 50));
}
