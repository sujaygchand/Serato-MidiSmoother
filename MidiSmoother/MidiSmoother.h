//
//  MidiSmoother.h
//  MidiSmoother
//
//  Created by Nathan Holmberg on 9/04/14.
//  Copyright (c) 2014 Nathan Holmberg. All rights reserved.
//
#include <time.h>

#ifndef MidiSmoother_MidiSmoother_h
#define MidiSmoother_MidiSmoother_h

class MidiSmoother
{
public:
	MidiSmoother( int midi_values_per_revolution, double seconds_per_revolution );
	
	void NotifyMidiValue( char midi_value );
	
	double RequestMSToMoveValue( double ms_to_process ) const;
	
	void StartMidiProcessing();
	
	void StopMidiProcessing();
	
	bool MidiIsProcessing() const;
private:
	// These variables should not be modified to ensure things continue as necessary
	const int mMidiValuesPerRevolution; // the number of midi values that would need to be recieved for an entire platter revolution to be expected
	const double mSecondsPerRevolution; // the number of seconds an entire platter revolution represents
	bool mbMidiIsProcessing;

	double lastTimeSinceCheck = 0;
	time_t startTime;
private:
	// This is an example of the absolute simplest way of providing velocity.
	double mLastVelocity;
};

#endif
