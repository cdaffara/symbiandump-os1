// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file
 @publishedPartner 
 @released
*/

#ifndef TTONEDATA_H
#define TTONEDATA_H

#include <e32base.h>

const TInt KMdaInfiniteDurationDTMFToneOnLength = -1;

/*
 *  Includes information about tone type, data, length.
 *  @lib ttonedata.lib
 */
class TToneData
{
public:

enum TToneType
	{
	/* Simple Tone */    
	ESimple = 0,
	/* Dual Tone */
	EDual = 1,
	/* TMF String Tone */
	EDtmfString = 2,
	/* Sequence */
	ESequence = 3,
	/* Fixed Sequence*/
	EFixedSequence = 4 
	};


	IMPORT_C TToneData();

	/*
	 * Sets type of the tone. (Simple, Dual, Sequence etc.)
	 * 
	 * @param aType Type of the tone.
	 */
	IMPORT_C void SetType(TToneType aType);

	/*
	 * Sets tone duration in micro seconds.
	 * 
	 * @param aDuration Tone duration in microseconds.
	 */
	IMPORT_C void SetDuration(TTimeIntervalMicroSeconds aDuration);

	/*
	 * Sets tone one freqyency.
	 *
	 * @param aFrequencyOne Frequency of the tone one.
	 */
	IMPORT_C void SetFrequencyOne(TInt aFrequencyOne);

	/*
	 * Sets tone two freqyency.
	 *
	 * @param aFrequencyOne Frequency of the tone two.
	 */
	IMPORT_C void SetFrequencyTwo(TInt aFrequencyTwo);

	/*
	 * Sets tone sequence number.
	 *
	 * @param aSequenceNumber Number of the sequence.
	 */
	IMPORT_C void SetSequenceNumber(TInt aSequenceNumber);

	/*
	 * Sets tone sequence data.
	 *
	 * @param aSequenceData Reference of the sequence data.
	 */
	IMPORT_C void SetSequenceData(TDesC8& aSequenceData);

	/*
	 * Sets tone repeat count, if count is KMdaRepeatForever, tone will be repeated indefinitely.
	 *
	 * @param aRepeatCount Repeat count of the tone.
	 */
	IMPORT_C void SetRepeatCount(TInt aRepeatCount);

	/*
	 * Sets tone trailing silence. 
	 * 
	 * @param aRepeatTrailingSilence An interval of silence which will be played after each tone. 
	 */
	IMPORT_C void SetRepeatTrailingSilence(TTimeIntervalMicroSeconds aRepeatTrailingSilence);

	/*
	 * Defines the duration of tone on, tone off and tone pause to be used during the DTMF tone playback operation.
	 *
	 * @param aToneOnLength The period over which the tone will be played. If this is set to zero, then the tone is not played.
	 * @param aToneOffLength The period over which the no tone will be played.
	 * @param aPauseLength The period over which the tone playing will be paused.
	 */
	IMPORT_C void SetDtmfLengths(TTimeIntervalMicroSeconds32 aToneOnLength, TTimeIntervalMicroSeconds32 aToneOffLength, TTimeIntervalMicroSeconds32 aPauseLength);

	/*
	 * Sets tone DTMF string.
	 *
	 * @param aDTMFString Reference of the DTMF string.
	 */
	IMPORT_C void SetDTMFString(TDesC& aDTMFString);

	/*
	 * Gets tone type. (Simple, Dual, Sequence etc.)
	 *
	 * @param aType Type of the tone.
	 */
	IMPORT_C void GetType(TToneType& aType);
	
	/*
	 * Gets tone duration in micro seconds.
	 *
	 * @param aDuration Tone duration in microseconds.
	 */
	IMPORT_C void GetDuration(TTimeIntervalMicroSeconds& aDuration);

	/*
	 * Gets frequency of the tone one.
	 *
	 * @return Frequency of the tone one.
	 */
	IMPORT_C TInt GetFrequencyOne();
	
	/*
	 * Gets frequency of the tone two.
	 *
	 * @return Frequency of the tone two.
	 */
	IMPORT_C TInt GetFrequencyTwo();
	
	/*
	 * Gets sequence number.
	 *
	 * @return Sequence number.
	 */
	IMPORT_C TInt GetSequenceNumber();
		  
	/*
	 * Gets sequence data of the tone.
	 * Client should not delete this pointer.
	 *
	 * @return Pointer to tone sequence data.
	 */
	IMPORT_C TDesC8* GetSequenceData();
			
	/*
	 * Gets tone repeat count.
	 *
	 * @return Repeat count.
	 */
	IMPORT_C TInt GetRepeatCount();
	
	/*
	 * Gets tone trailing silence duration.
	 *
	 * @param aRepeatTrailingSilence Trailing silence duration.
	 */
	IMPORT_C void GetRepeatTrailingSilence(TTimeIntervalMicroSeconds& aRepeatTrailingSilence);
			   
	/*
	 * Gets tone DTMF lengts duration.
	 *
	 * @param aToneOnLength The period over which the tone will be played.
	 * @param aToneOffLength The period over which the no tone will be played.
	 * @param aPauseLength The period over which the tone playing will be paused.
	 */
	IMPORT_C void GetDtmfLenghts(TTimeIntervalMicroSeconds32& aToneOnLength, TTimeIntervalMicroSeconds32& aToneOffLength, TTimeIntervalMicroSeconds32& aPauseLength);

	/*
	 * Gets DTMF string of the tone.
	 * Client should not delete this pointer
	 *
	 * @return Pointer to tone DTMF string.
	 */
	IMPORT_C TDesC* GetDTMFString();

	private: // data


	/* Type of the tone. */

	TToneType iType;

	
	/* Duration of the tone. */
	 
	TTimeIntervalMicroSeconds iDuration;

	
	/* Frequency of the tone one. */
	 
	TInt iFrequencyOne;

	
	/* Frequency of the tone two. */
	 
	TInt iFrequencyTwo;

	
	/* Sequence number of the tone. */
	 
	TInt iSequenceNumber;

	
	/* Repeat count of the tone. */
	 
	TInt iRepeatCount;

	
	/* Repeat traingling silence of the tone. */
	 
	TTimeIntervalMicroSeconds iRepeatTrainglingSilence;

	
	/* The period over which the tone will be played. */
	 
	TTimeIntervalMicroSeconds32 iToneOnLength;

	
	/* The period over which the no tone will be played. */
	 
	TTimeIntervalMicroSeconds32 iToneOffLength;

	
	/* The period over which the tone playing will be paused. */
	 
	TTimeIntervalMicroSeconds32 iPauseLength;

	
	/* Pointer to data of the tone sequence.
	 * Not own.
	 */
	 
	TDesC8 *iSequenceData;

	
	/* Pointer to string of the DTMF tone.
	 * Not own.
	 */
	 
	TDesC *iDTMFString;
	};

#endif // TTONEDATA_H
