//
//  MidiSmoother.cpp
//  MidiSmoother
//
//  Created by Nathan Holmberg on 9/04/14.
//  Copyright (c) 2014 Nathan Holmberg. All rights reserved.
//

#include "MidiSmoother.h"

#include <iostream>

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

void MidiSmoother::NotifyMidiValue( char midi_value )
/*
 * Notify the smoother that a new value has been received from the platter.
 *
 * @param midi_value
 *		The number of midi values that has passed. This can be negative indicating reverse direction.
 */
{
	// NB: This is incorrect! This assumes that we recieve a midi value every millisecond which isn't true
	mLastVelocity = midi_value/(double)mMidiValuesPerRevolution * mSecondsPerRevolution * 1000;
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