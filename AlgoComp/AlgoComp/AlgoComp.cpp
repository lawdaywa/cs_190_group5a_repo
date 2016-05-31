#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "MidiEvent.h"
#include "MidiEventList.h"
#include "MidiFile.h"
#include "MidiMessage.h"
#include "Options.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <time.h>
#include <vector>

//the number of selections available for each instrument in each portion of the song
const int INTRO_BASS = 5;
const int INTRO_SNARE = 5;
const int INTRO_CRASH = 1;
const int INTRO_HAT = 4;

const int VERSE_BASS = 7;
const int VERSE_SNARE = 7;
const int VERSE_CRASH = 0;
const int VERSE_HAT = 7;

const int CHORUS_BASS = 7;
const int CHORUS_SNARE = 7;
const int CHORUS_CRASH = 3;
const int CHORUS_HAT = 6;

//enum for composing a song structure
//used to index base paths and 1st dimension of availableTracks
enum SECTION {
	INTRO,
	VERSE,
	CHORUS
};


enum INSTRUMENT {
	BASS,
	SNARE,
	CRASH,
	HAT
};

const std::string basePaths[]{ "Drums\\Intro\\" , "Drums\\Verse\\" ,"Drums\\Chorus\\" };

const int availableTracks[3][4] = { {INTRO_BASS,	INTRO_SNARE,	INTRO_CRASH,	INTRO_HAT},
									{VERSE_BASS,	VERSE_SNARE,	VERSE_CRASH,	VERSE_HAT},
									{CHORUS_BASS,	CHORUS_SNARE,	CHORUS_CRASH,	CHORUS_HAT} 
								  };


//Assumes 120 ticks per quarter note as default
int barToTicks(int bar) {
	return bar * 480;
}


//Appends a single bar of midi data
//input - the MidiFile that contains the drum pattern
//output - the destination MidiFile
//bar - the bar in the output to write the events 
void appendBar(const MidiFile& input, MidiFile& output, const int& bar) {
	MidiFile in = input;
	int count = in.getEventCount(1);
	for (int i = 0; i < count; ++i) {
		MidiEvent me = in.getEvent(1, i);
		me.tick = me.tick / 4 + barToTicks(bar); // / 4 is a magic number~
		output.addEvent(me);
	}
}

//Appends an entire section of a song ie verse, chorus, etc.
//basePath - the location of the files to randomly select instruments from
//available - an array containing the number of options available for that instrument in this song section
//barsToAdd - how long this song section is
//bar - the current bar of the song
//output - the destination MidiFile
void appendSongSection(std::string basePath, const int* available, int barsToAdd, int& bar,  MidiFile& output ) {
	MidiFile snare, crash;
	std::vector<MidiFile> instruments;

	if (available[BASS] > 0) {
		MidiFile bass;
		bass.read(basePath + "bass" + std::to_string((std::rand() % available[BASS]) + 1) + ".mid");
		instruments.push_back(bass);
	}
	if (available[SNARE] > 0) {
		MidiFile snare;
		snare.read(basePath + "snare" + std::to_string((std::rand() % available[SNARE]) + 1) + ".mid");
		instruments.push_back(snare);
	}

	if(std::rand() % 4 > 2)
		if (available[CRASH] > 0) {
			MidiFile crash;
			crash.read(basePath + "crash" + std::to_string((std::rand() % available[CRASH]) + 1) + ".mid");
			instruments.push_back(crash);
		}

	if (available[HAT] > 0) {
		MidiFile hat;
		hat.read(basePath + "hat" + std::to_string((std::rand() % available[HAT]) + 1) + ".mid");
		instruments.push_back(hat);
	}
	
	//make sure all tracks use same ticks per quarter note as output file
	for (MidiFile i : instruments)
		i.setTicksPerQuarterNote(output.getTicksPerQuarterNote());

	for (int i = 0; i < barsToAdd; ++i) {
		for (MidiFile i : instruments)
			appendBar(i, output, bar);
		++bar;
	}
}

int main() {
	int currentBar = 0;
	MidiFile output;
	output.addTrack();
	srand(time(NULL));
	SECTION songStructure[] { INTRO, CHORUS, VERSE };
	int barsPerSection[] { 2, 4, 4 };

	for (int i = 0; i < 3; ++i) {
		appendSongSection(basePaths[songStructure[i]], availableTracks[songStructure[i]], barsPerSection[i], currentBar, output);
	}

	output.write("out.mid");
}