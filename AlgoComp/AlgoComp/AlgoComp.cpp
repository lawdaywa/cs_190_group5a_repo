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

//the number of selections available for each instrument in each portion of the song
const int INTRO_BASS = 1;
const int INTRO_SNARE = 1;
const int INTRO_CRASH = 1;

const int VERSE_BASS = 1;
const int VERSE_SNARE = 1;
const int VERSE_CRASH = 1;

const int CHORUS_BASS = 1;
const int CHORUS_SNARE = 1;
const int CHORUS_CRASH = 1;

//enum for composing a song structure
//used to index 1st dimension of availableTracks
enum SECTION {
	INTRO,
	VERSE,
	CHORUS
};

enum INSTRUMENT {
	BASS,
	SNARE,
	CRASH
};

const int availableTracks[3][3] = { {INTRO_BASS,	INTRO_SNARE,	INTRO_CRASH},
									{VERSE_BASS,	VERSE_SNARE,	VERSE_CRASH},
									{CHORUS_BASS,	CHORUS_SNARE,	CHORUS_CRASH} 
								  };


//Assume 120bpm tempo in 4/4 time -> 2000ms per measure
//use to update the base time of the current measure
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
	std::cout << count << std::endl;
	for (int i = 0; i < count; ++i) {
		MidiEvent me = in.getEvent(1, i);
		me.tick = me.tick / 4 + barToTicks(bar);
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
	MidiFile bass, snare, crash;
	bass.read(basePath	+ "bass"  + std::to_string((std::rand() % available[BASS]) + 1)  + ".mid");
	snare.read(basePath + "snare" + std::to_string((std::rand() % available[SNARE]) + 1) + ".mid");
	crash.read(basePath + "crash" + std::to_string((std::rand() % available[CRASH]) + 1) + ".mid");
	
	bass.setTicksPerQuarterNote(output.getTicksPerQuarterNote());
	snare.setTicksPerQuarterNote(output.getTicksPerQuarterNote());
	crash.setTicksPerQuarterNote(output.getTicksPerQuarterNote());

	for (int i = 0; i < barsToAdd; ++i) {
		appendBar(bass, output, bar);
		appendBar(snare, output, bar);
		appendBar(crash, output, bar);
		++bar;
	}
}

int main() {
	int currentBar = 0;
	MidiFile output;
	output.addTrack();

	SECTION songStructure[] { INTRO};
	int barsPerSection[]{ 4 };
	std::string basePaths[]{ "Drums\\Intro\\" };//, "Drums\\Verse\\", "Drums\\Chorus\\"};

	for (SECTION i : songStructure) {
		appendSongSection(basePaths[i], availableTracks[i], barsPerSection[i] ,currentBar, output);
	}

	output.write("out.mid");
}