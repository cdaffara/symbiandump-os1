// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mmfswaudioinput.h
// 
//

#ifndef SWAUDIOINPUT_H
#define SWAUDIOINPUT_H

#include <e32base.h>
#include <d32soundsc.h>


//TAudioInputParams 
//- give sample rate and buffer size
class TAudioInputParams
	{
public:
	IMPORT_C TAudioInputParams();
	
	TInt iSampleRate;
		// The sample rate of the data past to the client. Ideally, but not
		// necessarily that used on the device
	TInt iNumChannels;
		// Number of channels to use. 1 = mono. 2 = (interleaved) stereo.
	TInt iNominalBufferSize;
		// "Full buffers" are to be of this size. (Only the penultimate can be shorter)
	TInt iInitialGain;
		// Gain to use on start.
	};

// Observer class for MAudioInput
class MAudioInputObserver
	{
public:
	virtual void InputBufferAvailable(const TDesC8& aBuffer)=0;
		// A buffer of data is available. Assume single buffering
		
	virtual void InputFinished()=0;
		// called following Pause() to say no more data left
		
	virtual void InputError(TInt aError)=0;
		// called on fatal(ish) error. Client should call Stop() and/or Close()
	};
	
// Interface to SwWrapper AudioInput
class MAudioInput
	{
public:
	IMPORT_C static MAudioInput* CreateL(MAudioInputObserver& aObserver);
		// Create new object
		
	virtual void Release()=0;
		// destructor call
		
	virtual TInt Initialize(const TAudioInputParams& aParams)=0;
		// Initialize with given properties. synchronous call.
		
	virtual void Close()=0;
		// undo of Initialize() - return to created state. Implied Stop() if required.
		
	virtual TInt Start()=0;
		// Start to record and supply buffers. Only valid if initialized
		// subsequently BufferToEmpty() will be called
		
	virtual void BufferAck()=0;
		// The buffer supplied by InputBufferAvailable has been read.
		// Client must have stopped reading from the buffer
		
	virtual TInt Pause()=0;
		// pause (temporarily stop) recording. When all buffers have been passed
		// InputFinished() is called.
		
	virtual TInt Resume()=0;
		// resume from paused mode. If InputFinished() has not been sent
		// already it won't be
		
	virtual TInt Flush()=0;
		// throw away any partially recorded buffers than have not been
		// received. Implied BufferAck(). Only legal when paused
		
	virtual void Stop()=0;
		// immediate stop, but does not close. Akin to Cancel() if we are running.
		
	virtual TAny* Interface(TUid aInterfaceUid)=0;
		// for standard extension pattern
	};

// Parameter access. Do as CI since API not so clear
const TUid KUidAIParamInterface = {0x10287080};
class MAIParamInterface
	{
public:
	virtual TInt SetGain(TInt aGain)=0;
		// set basic gain
		
	virtual TInt GetBufferSizes(TInt& aMinSize, TInt& aMaxSize)=0;
		// min and max buffer size supported (or at least recommended)	
	
	virtual TInt GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates)=0;
	};

#endif // SWAUDIOINPUT_H

