//
//  MidiSmoother.cpp
//  MidiSmoother
//
//  Created by Nathan Holmberg on 9/04/14.
//  Copyright (c) 2014 Nathan Holmberg. All rights reserved.
//

#include "MidiSmoother.h"
#include <iostream>
#include <time.h>

MidiSmoother::MidiSmoother( int midi_values_per_revolution, double seconds_per_revolution ) :
mMidiValuesPerRevolution( midi_values_per_revolution ),
mSecondsPerRevolution( seconds_per_revolution ),
mbMidiIsProcessing(true),
mLastVelocity(0.0)
/*
 * Constructor for a Midi Smoother.
 *
 * @param midi_values_per_revolution
 *		The number of midi values that would need to be recieved for an entire platter revolution to be expected
 * @param seconds_per_revolution
 *		The number of seconds an entire platter revolution represents
 */
{
	
}


void MidiSmoother::StartMidiProcessing()
/*
 * Indicates that the midi processing is about to begin!
 */
{
	mStartTime = time(0);
	mbMidiIsProcessing = true;
}

void MidiSmoother::StopMidiProcessing()
/*
 * Indicates that there is no more midi coming!
 */
{
	mbMidiIsProcessing = false;
}

bool MidiSmoother::MidiIsProcessing() const
{
	return mbMidiIsProcessing;
}


bool MidiSmoother::TryDecompressMidiValue(char& midi_value, uint32_t& outputValue)
{
	outputValue = midi_value;

	// Return if data decompression is not needed 
	if (!(outputValue & MidiStatus::NoteOff))
		return true;

	// Gets lower 7 bits of byte
	outputValue &= 0x7f;

	outputValue = (outputValue << 7) | (mStoredMidiValue & 0x7F);

	return !(outputValue & MidiStatus::NoteOff);
}

char* MidiSmoother::CharToBinary(unsigned char c)
{
	static char bin[CHAR_BIT + 1] = { 0 };
	int i;

	for (i = CHAR_BIT - 1; i >= 0; i--)
	{
		bin[i] = (c % 2) + '0';
		c /= 2;
	}

	return bin;
}

bool MidiSmoother::CanMidiValueAffectVelocity(uint32_t midi_value)
{
	auto midiStatus = midi_value & 0xF0;

	return !(midiStatus == MidiStatus::NoteOff || midiStatus == MidiStatus::ChannelPressure);
}

void MidiSmoother::NotifyMidiValue( char midi_value )
/*
 * Notify the smoother that a new value has been received from the platter.
 *
 * @param midi_value
 *		The number of midi values that has passed. This can be negative indicating reverse direction.
 */
{
	// NB: This is incorrect! This assumes that we recieve a midi value every millisecond which isn't true

	mLastTimeSinceCheck = (difftime(time(0), mStartTime) * 1000) - mLastTimeSinceCheck;

	if (midi_value == mLastStoredNote)
		return;

	mLastStoredNote = midi_value;

	uint32_t outputMidiValue;

	bool wasDecompressed = TryDecompressMidiValue(midi_value, outputMidiValue);

	mStoredMidiValue = wasDecompressed ? outputMidiValue : 0;

	if(wasDecompressed == false)
		return;

	if (CanMidiValueAffectVelocity(outputMidiValue) == false)
	{
		outputMidiValue = 0;
	}

	mLastVelocity = outputMidiValue/(double)mMidiValuesPerRevolution * mSecondsPerRevolution * mLastTimeSinceCheck;
}

double MidiSmoother::RequestMSToMoveValue( double ms_to_process ) const
/*
 * Request a ms to move value from the smoother. This is so the audio engine would know how much to step forward to produce the appropriate pitch for what the platter is doing.
 *
 * @param ms_to_process
 *		The number of ms we are calculating this step for. 
 * @return
 *		The number of ms that should be moved during this process step
 */
{
	return mLastVelocity * ms_to_process;
}