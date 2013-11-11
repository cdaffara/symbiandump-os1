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

#include <e32debug.h>

#include "mdfplayerengine.h"

#if defined (SYMBIAN_MDFPLAYERENGINE_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif // defined(SYMBIAN_MDFPLAYERENGINE_DEBUG)

const TInt KOneSecond = 1000000;

/* 
 Constructs a new instance of CMdfVideoPlayerEngine.
 @return    "CMdfVideoPlayerEngine*"
            A pointer to the newly constructed CMdfVideoPlayerEngine
 */
CMdfVideoPlayerEngine* CMdfVideoPlayerEngine::NewL(MMdfVideoPlayerHwDeviceObserver& aDeviceObserver)
	{
	CMdfVideoPlayerEngine* self = new (ELeave) CMdfVideoPlayerEngine(aDeviceObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}


/** 
 Constructs and initialises a new instance of CMdfVideoPlayerEngine.
 
 @param     "MMdfVideoPlayerHwDeviceObserver& aDeviceObserver"
            A reference to the HwDevice on which it will operate
 */
CMdfVideoPlayerEngine::CMdfVideoPlayerEngine(MMdfVideoPlayerHwDeviceObserver& aDeviceObserver)
	: iDeviceObserver(aDeviceObserver)
	{	
	}


void CMdfVideoPlayerEngine::ConstructL()
	{
	iTimer = CPeriodic::NewL(EPriorityHigh);		
	}

/** 
 Default destructor
 */
CMdfVideoPlayerEngine::~CMdfVideoPlayerEngine()
	{
	// stop if we need to
	Stop();
	}

/** 
 Starts the player engine.
 
 @param     "TUint aFrameRate"
            The actual frame rate for display, or KUnsynchronized
 @leave		May leave with KErrNotReady if the frame rate is not initialized.            
 */
void CMdfVideoPlayerEngine::StartL(TUint aFrameRate)
	{
	if(aFrameRate == 0) 
		{
		User::Leave(KErrNotReady);
		}
	
	iFrameRate = aFrameRate;
	
	// get current tick count, save this
	iStartTime = iDeviceObserver.Time();
	iFrameCount = 0;
	
	DEBUG_PRINT(_L("+++ PlayerEngine: Starting"));
    iTimer->Cancel();
    iTimer->Start(KMaxFrameRate, KMaxFrameRate, TCallBack(DisplayFrame, this));
	}
      
/** 
 Stops the player engine.
 */
void CMdfVideoPlayerEngine::Stop()
	{
	if(iTimer) 
		{
		DEBUG_PRINT(_L("+++ PlayerEngine: Stopping"));
	    iTimer->Cancel();
	    delete iTimer;
	    iTimer = NULL;
		}
	}
	
/**
 Returns the frame rate.
 
 @return The frame rate.
*/	
TInt CMdfVideoPlayerEngine::FrameRate()
	{
	return iFrameRate;
	}
	
/**
 Returns the frame count.
 
 @return The frame count.
*/	
TInt CMdfVideoPlayerEngine::FrameCount()
	{
	return iFrameCount;
	}

/** 
 Static callback method for the CPeriodic timer
 */
TInt CMdfVideoPlayerEngine::DisplayFrame(TAny* aObject)
	{
    // cast, and call non-static function
    (reinterpret_cast<CMdfVideoPlayerEngine*>(aObject))->DoDisplayFrame();
    return KErrNone;
	}

/** 
 Processes one frame if available.
 */
void CMdfVideoPlayerEngine::DoDisplayFrame()
	{
	if(iDeviceObserver.FrameAvailable()) 
		{
		// if we're playing unsynchronized, then return the frame
		// regardless of time
		if(iFrameRate == KUnsynchronized) 
			{
			iFrameCount++;	
			DEBUG_PRINT(_L("+++ PlayerEngine: Sending frame buffer to DevVideo"));
			iDeviceObserver.DisplayFrame();
			}
		else 
			{
			TUint curPlayTime = iDeviceObserver.Time();
		
			// if we've missed a tick, then discard the frame.
			// (unless it's the first frame; always display this)
			if(iFrameCount) 
				{
				TUint nextNextFrameTime = iStartTime + ((iFrameCount + 1) * (KOneSecond / iFrameRate));
				if(nextNextFrameTime <= curPlayTime) 
					{
					iFrameCount++;	
					DEBUG_PRINT(_L("+++ PlayerEngine: Discarding frame buffer"));
					iDeviceObserver.DiscardFrame();
					return;
					}
				}
			// else if we're past the expected display time
			// then display the frame.
			TUint nextFrameTime = iStartTime + (iFrameCount * (KOneSecond / iFrameRate));
			if(!iFrameCount || nextFrameTime <= curPlayTime) 
				{
				iFrameCount++;	
				DEBUG_PRINT(_L("+++ PlayerEngine: Sending frame buffer to DevVideo"));
				iDeviceObserver.DisplayFrame();
				}
			}
		}
	}

// end
