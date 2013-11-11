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

#ifndef MDFPLAYERENGINE_H
#define MDFPLAYERENGINE_H

#include <e32base.h>

class MMdfVideoPlayerHwDeviceObserver
	{
public:
	/** Get system time. May come from a local or external source.
	@return The system time as a TUint.
	*/
	virtual TUint Time() const = 0;
		
	/** 
	Is a frame (a decoded picture) available.
	@return ETrue if a frame is available.
	*/
	virtual TBool FrameAvailable() const = 0;

	/** 
	Displays a frame.
	*/
	virtual void DisplayFrame() = 0;

	/** 
	Discards a frame.
	*/
	virtual void DiscardFrame() = 0;
	};

/**
Video player engine.
This manages the playing of decoded frames for the DevVideo hardware device.
@internalComponent
*/
class CMdfVideoPlayerEngine : public CBase
	{
public:
	static CMdfVideoPlayerEngine* NewL(MMdfVideoPlayerHwDeviceObserver& aDeviceObserver);
	~CMdfVideoPlayerEngine();

	void StartL(TUint aFrameRate);
	void Stop();
	TInt FrameRate();
	TInt FrameCount();

private:
	CMdfVideoPlayerEngine(MMdfVideoPlayerHwDeviceObserver& aDeviceObserver);
	void ConstructL();
	static TInt DisplayFrame(TAny* aObject);
	void DoDisplayFrame();
public:
	// indicate that player should run unsynchronized
	static const TInt KUnsynchronized = -1;
		
private:
	// the maximum frame rate of the engine, in fps
	// NB the actual maximum frame rate of the decoder may be higher.
	static const TInt KMaxFrameRate = 100;
	
	// tick timer to manage playback
	CPeriodic* iTimer;
	TUint iStartTime;
	
	// actual frame rate
	TInt iFrameRate;
	
	// count of frames processed by the engine
	TInt iFrameCount;
	
	// the HwDevice observer
	MMdfVideoPlayerHwDeviceObserver& iDeviceObserver;
	};

#endif // MDFPLAYERENGINE_H
