//
//  MidiSmoother.h
//  MidiSmoother
//
//  Created by Nathan Holmberg on 9/04/14.
//  Copyright (c) 2014 Nathan Holmberg. All rights reserved.
//
#include <time.h>
#include <stdint.h>

#ifndef MidiSmoother_MidiSmoother_h
#define MidiSmoother_MidiSmoother_h

class MidiSmoother
{
	/// <summary>
	/// Used to check the type of note coming in 
	/// https://midifile.sapp.org/class/MidiMessage/
	/// </summary>
	enum MidiStatus : uint8_t
	{
		NoteOff = 0x80,
		NoteOn = 0x90,
		Aftertouch = 0xA0,
		ContinuousController = 0xB0,
		PatchChange = 0xC0,
		ChannelPressure = 0xD0,
		PitchBend = 0xE0,
	};

public:
	MidiSmoother( int midi_values_per_revolution, double seconds_per_revolution );
	
	void NotifyMidiValue( char midi_value );
	
	double RequestMSToMoveValue( double ms_to_process ) const;
	
	void StartMidiProcessing();
	
	void StopMidiProcessing();
	
	bool MidiIsProcessing() const;


	// Uncompress the Midi value
	bool TryDecompressMidiValue(char& midi_value, uint32_t& outputValue);

	char* CharToBinary(unsigned char c);
private:
	// These variables should not be modified to ensure things continue as necessary
	const int mMidiValuesPerRevolution; // the number of midi values that would need to be recieved for an entire platter revolution to be expected
	const double mSecondsPerRevolution; // the number of seconds an entire platter revolution represents
	bool mbMidiIsProcessing;

	float mLastTimeSinceCheck = 0;
	time_t mStartTime;
	
	char mLastStoredNote;
	uint32_t mStoredMidiValue;

private:
	// This is an example of the absolute simplest way of providing velocity.
	double mLastVelocity;
};

#endif
