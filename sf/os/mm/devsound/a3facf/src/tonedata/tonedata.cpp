//tonedata.cpp

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



#include <a3f/tonedata.h>
#include <a3f/a3f_trace_utils.h>


// ---------------------------------------------------------------------------
// TToneData::TToneData
// ---------------------------------------------------------------------------
//
EXPORT_C TToneData::TToneData()
	{
	DP_CONTEXT(TToneData::TToneData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// Default values
	iRepeatCount = 0;
	iType = ESimple;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetType
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetType(TToneType aType)
	{
	DP_CONTEXT(TToneData::SetType *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aType=%d", aType);
	iType = aType;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetDuration
// ---------------------------------------------------------------------------
//    
EXPORT_C void TToneData::SetDuration(TTimeIntervalMicroSeconds aDuration)
	{
	DP_CONTEXT(TToneData::SetDuration *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iDuration = aDuration;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetFrequencyOne
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetFrequencyOne(TInt aFrequencyOne)
	{
	DP_CONTEXT(TToneData::SetFrequencyOne *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aFrequencyOne=%d", aFrequencyOne);
	iFrequencyOne = aFrequencyOne;    
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetFrequencyTwo
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetFrequencyTwo(TInt aFrequencyTwo)
	{
	DP_CONTEXT(TToneData::SetFrequencyTwo *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aFrequencyTwo=%d", aFrequencyTwo);
	iFrequencyTwo = aFrequencyTwo;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetSequenceNumber
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetSequenceNumber(TInt aSequenceNumber)
	{
	DP_CONTEXT(TToneData::SetSequenceNumber *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aSequenceNumber=%d", aSequenceNumber);
	iSequenceNumber = aSequenceNumber;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetSequenceData
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetSequenceData(TDesC8& aSequenceData)
	{
	DP_CONTEXT(TToneData::SetSequenceData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iSequenceData = &aSequenceData;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetRepeatCount
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetRepeatCount(TInt aRepeatCount)
	{
	DP_CONTEXT(TToneData::SetRepeatCount *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aRepeatCount=%d", aRepeatCount);
	iRepeatCount = aRepeatCount;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetRepeatTrailingSilence
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::SetRepeatTrailingSilence(TTimeIntervalMicroSeconds aRepeatTrainglingSilence)
	{
	DP_CONTEXT(TToneData::SetRepeatTrailingSilence *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iRepeatTrainglingSilence = aRepeatTrainglingSilence;    
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetDtmfLengths
// ---------------------------------------------------------------------------
//    
EXPORT_C void TToneData::SetDtmfLengths(TTimeIntervalMicroSeconds32 aToneOnLength,
		TTimeIntervalMicroSeconds32 aToneOffLength, TTimeIntervalMicroSeconds32 aPauseLength)
	{
	DP_CONTEXT(TToneData::SetDtmfLengths *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iToneOnLength = aToneOnLength;
	iToneOffLength = aToneOffLength;
	iPauseLength = aPauseLength;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::SetDTMFString
// ---------------------------------------------------------------------------
//    
EXPORT_C void TToneData::SetDTMFString(TDesC& aDTMFString)
	{
	DP_CONTEXT(TToneData::SetDTMFString *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iDTMFString = &aDTMFString;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::GetType
// ---------------------------------------------------------------------------
//    
EXPORT_C void TToneData::GetType(TToneType& aType)
	{
	DP_CONTEXT(TToneData::GetType *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aType = iType;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::GetDuration
// ---------------------------------------------------------------------------
//    
EXPORT_C void TToneData::GetDuration(TTimeIntervalMicroSeconds& aDuration)
	{
	DP_CONTEXT(TToneData::GetDuration *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aDuration = iDuration;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::GetFrequencyOne
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TToneData::GetFrequencyOne()
	{
	DP_CONTEXT(TToneData::GetFrequencyOne *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iFrequencyOne, "%d");    
	}

// ---------------------------------------------------------------------------
// TToneData::GetFrequencyTwo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TToneData::GetFrequencyTwo()
	{
	DP_CONTEXT(TToneData::GetFrequencyTwo *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iFrequencyTwo, "%d");
	}

// ---------------------------------------------------------------------------
// TToneData::GetSequenceNumber
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TToneData::GetSequenceNumber()
	{
	DP_CONTEXT(TToneData::GetSequenceNumber *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iSequenceNumber, "%d");
	}


// ---------------------------------------------------------------------------
// TToneData::GetSequenceData
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8* TToneData::GetSequenceData()
	{
	DP_CONTEXT(TToneData::GetSequenceData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iSequenceData;    
	}

// ---------------------------------------------------------------------------
// TToneData::GetRepeatCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TToneData::GetRepeatCount()
	{
	DP_CONTEXT(TToneData::GetRepeatCount *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iRepeatCount, "%d");
	}

// ---------------------------------------------------------------------------
// TToneData::GetRepeatTrailingSilence
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::GetRepeatTrailingSilence(TTimeIntervalMicroSeconds& aRepeatTrainglingSilence)
	{
	DP_CONTEXT(TToneData::GetRepeatTrailingSilence *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aRepeatTrainglingSilence = iRepeatTrainglingSilence;    
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::GetDtmfLenghts
// ---------------------------------------------------------------------------
//
EXPORT_C void TToneData::GetDtmfLenghts(TTimeIntervalMicroSeconds32& aToneOnLength,
		TTimeIntervalMicroSeconds32& aToneOffLength, TTimeIntervalMicroSeconds32& aPauseLength)
	{
	DP_CONTEXT(TToneData::GetDtmfLenghts *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aToneOnLength = iToneOnLength;
	aToneOffLength = iToneOffLength;
	aPauseLength = iPauseLength;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// TToneData::GetDTMFString
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC* TToneData::GetDTMFString()
	{
	DP_CONTEXT(TToneData::GetDTMFString *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iDTMFString;
	}
