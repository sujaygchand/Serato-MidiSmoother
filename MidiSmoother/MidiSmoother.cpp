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

MidiSmoother::MidiSmoother(int midi_values_per_revolution, double seconds_per_revolution) :
	mMidiValuesPerRevolution(midi_values_per_revolution),
	mSecondsPerRevolution(seconds_per_revolution),
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
	// Starts timer
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


bool MidiSmoother::TryDecompressMidiValue(char& midi_value, uint32_t& output_value)
/*
 * Decompresses the midi value and bitwise adds the last stored value
 *
 * @param midi_value
 *		The number of midi values that has passed. This can be negative indicating reverse direction.
 * @param output_value
 *		The decompressed value of midi_value.
 * @return
 *		Whether the value was decompressed
 */
{
	output_value = midi_value;

	// Return if data decompression is not needed 
	if (!(output_value & MidiStatus::NoteOff))
		return true;

	// Gets lower 7 bits of byte
	output_value &= 0x7f;

	output_value = (output_value << 7) | (mStoredMidiValue & 0x7F);

	return !(output_value & MidiStatus::NoteOff);
}

char* MidiSmoother::CharToBinary(unsigned char c)
/*
 * Convert to binary.
 *
 * @param c
 *		input char.
 */
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
/*
 * Checks if note doesn't set velocity to off
 *
 * @param midi_value
 *		The number of ms we are calculating this step for.
 * @return
 *		Will note effect velocity
 */
{
	auto midiStatus = midi_value & 0xF0;

	return !(midiStatus == MidiStatus::NoteOff || midiStatus == MidiStatus::ChannelPressure);
}

void MidiSmoother::NotifyMidiValue(char midi_value)
/*
 * Notify the smoother that a new value has been received from the platter.
 *
 * @param midi_value
 *		The number of midi values that has passed. This can be negative indicating reverse direction.
 */
{
	// used to find time since last midi input
	mLastTimeSinceCheck = (difftime(time(0), mStartTime) * 1000) - mLastTimeSinceCheck;

	// Ignores duplicate values, which would represent the note being held down 
	if (midi_value == mLastStoredNote)
		return;

	mLastStoredNote = midi_value;

	uint32_t outputMidiValue;

	bool wasDecompressed = TryDecompressMidiValue(midi_value, outputMidiValue);

	// On success the previously stored value is no longer needed
	mStoredMidiValue = wasDecompressed ? outputMidiValue : 0;

	if (wasDecompressed == false)
		return;

	if (CanMidiValueAffectVelocity(outputMidiValue) == false)
	{
		outputMidiValue = 0;
	}

	mLastVelocity = outputMidiValue / (double)mMidiValuesPerRevolution * mSecondsPerRevolution * mLastTimeSinceCheck;
}

double MidiSmoother::RequestMSToMoveValue(double ms_to_process) const
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