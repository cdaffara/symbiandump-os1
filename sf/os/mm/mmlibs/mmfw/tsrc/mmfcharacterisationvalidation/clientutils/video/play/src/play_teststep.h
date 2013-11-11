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

#ifndef PLAY_TESTSTEP_H
#define PLAY_TESTSTEP_H

#include "char_videoplay_common.h"


// Open a video file
//MM-MMF-VCLNT-CHRTZ-I-0001

class RVPlyrOpenFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVPlyrOpenFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVPlyrOpenFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	};         

// Open a video file (Invalid file format)
//MM-MMF-VCLNT-CHRTZ-I-0002 , 0002a, 0002b, 0003

class RVidPlyrOpnFileInvFmtTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrOpnFileInvFmtTest* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpError = KErrNone);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
	TFileName iFilename1;
	
private:
	// private constructor
	RVidPlyrOpnFileInvFmtTest(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpError = KErrNone);
	TBuf<KNameBufSize> iKeyName;
	TInt iExpError;
	}; 


// Open a video file with controller Uid
//MM-MMF-VCLNT-CHRTZ-I-0004

class RVidPlyrOpnFileCntrlrUidTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrOpnFileCntrlrUidTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
	TFileName iFilename1;
	
private:
	// private constructor
	RVidPlyrOpnFileCntrlrUidTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Open a video file with invalid controller uid.
//MM-MMF-VCLNT-CHRTZ-I-0005

class RVidPlyrOpnFileInvCntrlrUidTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrOpnFileInvCntrlrUidTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrOpnFileInvCntrlrUidTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Play a video file
// MM-MMF-VCLNT-CHRTZ-I-0006
class RVideoPlayTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVideoPlayTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVideoPlayTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Play a video file while video is playing
//MM-MMF-VCLNT-CHRTZ-I-0007

class RVidPlyrPlayAgainTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayAgainTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;
		
private:
	// private constructor
	RVidPlyrPlayAgainTest(const TDesC& aStepName, const TDesC& aSectName);
	// Timer callback
	void DoTimerCallback();	
	};

// Play a video file without opening it
//MM-MMF-VCLNT-CHRTZ-I-0008

class RVidPlyrPlayFileWithoutOpenTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayFileWithoutOpenTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPlayFileWithoutOpenTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Play the video file without preparing clips
//MM-MMF-VCLNT-CHRTZ-I-0009

class RVidPlyrPlayWithoutPrepareTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayWithoutPrepareTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrPlayWithoutPrepareTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Play a video file with start and end points
//MM-MMF-VCLNT-CHRTZ-I-0010

class RVidPlyrPlayFileStEndptsTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayFileStEndptsTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPlayFileStEndptsTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Stop a video file while video is playing
//MM-MMF-VCLNT-CHRTZ-I-0011

class RVidPlyrStopVidPlayTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStopVidPlayTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrStopVidPlayTest(const TDesC& aStepName, const TDesC& aSectName);
	
	// Timer callback
	void DoTimerCallback();
	};

// Stop a video file in paused state
//MM-MMF-VCLNT-CHRTZ-I-0012

class RVidPlyrStopVidPlayInPauseTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStopVidPlayInPauseTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrStopVidPlayInPauseTest(const TDesC& aStepName, const TDesC& aSectName);	
	
	// Timer callback
	void DoTimerCallback();
	}; 

// Stop a video file after opening the video file
//MM-MMF-VCLNT-CHRTZ-I-0013

class RVidPlyrStopAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStopAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrStopAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Pause video play while file is playing
//MM-MMF-VCLNT-CHRTZ-I-0014

class RVidPlyrPauseVidPlayFilPlngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPauseVidPlayFilPlngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPauseVidPlayFilPlngTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
 
// Pause video play after opening the video file
//MM-MMF-VCLNT-CHRTZ-I-0015

class RVidPlyrPauseVidPlayAftrOpnFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPauseVidPlayAftrOpnFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPauseVidPlayAftrOpnFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Pause video play after stop
//MM-MMF-VCLNT-CHRTZ-I-0016

class RVidPlyrPauseVidPlayAftrStopTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPauseVidPlayAftrStopTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPauseVidPlayAftrStopTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Pause video play before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0017

class RVidPlyrPauseVidPlayB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPauseVidPlayB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrPauseVidPlayB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set volume after opening video file
//MM-MMF-VCLNT-CHRTZ-I-0018

class RVidPlyrSetVolAftrOpngVidFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolAftrOpngVidFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetVolAftrOpngVidFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};

// Set volume during play
//MM-MMF-VCLNT-CHRTZ-I-0019

class RVidPlyrSetVolDuringPlayTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolDuringPlayTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrSetVolDuringPlayTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
 
 // Set volume during play
//MM-MMF-VCLNT-CHRTZ-I-0020

class RVidPlyrSetVolInPauseTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolInPauseTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrSetVolInPauseTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 

// Set volume in stopped state
//MM-MMF-VCLNT-CHRTZ-I-0021

class RVidPlyrSetVolInStopTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolInStopTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrSetVolInStopTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Set volume after opening a video only file
//MM-MMF-VCLNT-CHRTZ-I-0022

class RVidPlyrSetVolAftrOpngVidOnlyFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolAftrOpngVidOnlyFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;		
		
private:
	// private constructor
	RVidPlyrSetVolAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set volume with invalid parameters
//MM-MMF-VCLNT-CHRTZ-I-0023

class RVidPlyrSetVolWithInvPmtsTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVolWithInvPmtsTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
		
private:
	// private constructor
	RVidPlyrSetVolWithInvPmtsTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 

// Get maximum volume in different states 
//MM-MMF-VCLNT-CHRTZ-I-0024

class RVidPlyrMaxVolInDfrntStTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrMaxVolInDfrntStTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
    TBool iValid1;		
		
private:
	// private constructor
	RVidPlyrMaxVolInDfrntStTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Query audio enabled or not in different(playing and stopped) states 
//MM-MMF-VCLNT-CHRTZ-I-0025

class RVidPlyrAudEnbldInDfrntStTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrAudEnbldInDfrntStTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
    TBool iValid1;
    TBool iValid2;
	void DoTimerCallback();
		
private:
	// private constructor
	RVidPlyrAudEnbldInDfrntStTest(const TDesC& aStepName, const TDesC& aSectName);	
	
	}; 		

// Query audio enabled or not in paused state 
//MM-MMF-VCLNT-CHRTZ-I-0025a

class RVidPlyrAudEnbldInPausedStTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrAudEnbldInPausedStTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
    TBool iValid1;
	void DoTimerCallback();
		
private:
	// private constructor
	RVidPlyrAudEnbldInPausedStTest(const TDesC& aStepName, const TDesC& aSectName);	
	
	}; 		

// Query audio enabled or not before opening the file 
//MM-MMF-VCLNT-CHRTZ-I-0026

class RVidPlyrAudEnbldBfrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrAudEnbldBfrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrAudEnbldBfrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Get volume without setting any volume after opening file
//MM-MMF-VCLNT-CHRTZ-I-0027

class RVidPlyrGetVolAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetVolAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetVolAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get volume before opening file
//MM-MMF-VCLNT-CHRTZ-I-0028

class RVidPlyrSetNGetVolB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetVolB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetNGetVolB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Get the duration of video file after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0029

class RVidPlyrGetDrnAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetDrnAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetDrnAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 									     	 		

// Get the duration of video file in playing state
//MM-MMF-VCLNT-CHRTZ-I-0030

class RVidPlyrGetDrnPlyngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetDrnPlyngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetDrnPlyngTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 								     

// Get the duration of video file in paused state
//MM-MMF-VCLNT-CHRTZ-I-0031

class RVidPlyrGetDrnPausedTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetDrnPausedTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetDrnPausedTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 	

// Get the duration of video file in stopped state
//MM-MMF-VCLNT-CHRTZ-I-0032

class RVidPlyrGetDrnStpdTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetDrnStpdTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetDrnStpdTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	};
	
// Get the duration of video file before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0033

class RVidPlyrGetDrnB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetDrnB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetDrnB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get position of playback after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0034

class RVidPlyrSetNGetPosAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;

private:
	// private constructor
	RVidPlyrSetNGetPosAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get position of playback in playing state
//MM-MMF-VCLNT-CHRTZ-I-0035

class RVidPlyrSetNGetPosInPlyngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosInPlyngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrSetNGetPosInPlyngTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Set and get position of playback in paused state
//MM-MMF-VCLNT-CHRTZ-I-0036

class RVidPlyrSetNGetPosPausedTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosPausedTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrSetNGetPosPausedTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	};
	
// Set and get position of playback in stopped state
//MM-MMF-VCLNT-CHRTZ-I-0037

class RVidPlyrSetNGetPosInStpdStTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosInStpdStTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;

private:
	// private constructor
	RVidPlyrSetNGetPosInStpdStTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	};
	
// Set and get position of playback before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0038

class RVidPlyrSetNGetPosB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;

private:
	// private constructor
	RVidPlyrSetNGetPosB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set position with some invalid value and get the position of playback after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0039

class RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TBool iValid;
	TBool iValid1;

private:
	// private constructor
	RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Query the audio type in video file
//MM-MMF-VCLNT-CHRTZ-I-0040

class RVidPlyrQryAudTypVidFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudTypVidFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryAudTypVidFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Query the audio type in video file before opening it
//MM-MMF-VCLNT-CHRTZ-I-0044

class RVidPlyrQryAudTypB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudTypB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryAudTypB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Query the audio type in video only file before opening it
//MM-MMF-VCLNT-CHRTZ-I-0045

class RVidPlyrQryAudTypVidOnlyFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudTypVidOnlyFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrQryAudTypVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Query the audio bitrate in video file 
//MM-MMF-VCLNT-CHRTZ-I-0046

class RVidPlyrQryAudBitRateVidFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudBitRateVidFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryAudBitRateVidFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Query the audio bitrate in video file before opening it
//MM-MMF-VCLNT-CHRTZ-I-0050

class RVidPlyrQryAudBitRateB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudBitRateB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryAudBitRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Query the audio bitrate in video only file after opening it
//MM-MMF-VCLNT-CHRTZ-I-0051

class RVidPlyrQryAudBitRateVidOnlyFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryAudBitRateVidOnlyFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrQryAudBitRateVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Query the video bitrate in video file after opening it
//MM-MMF-VCLNT-CHRTZ-I-0052

class RVidPlyrQryVidBitRateVidFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryVidBitRateVidFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryVidBitRateVidFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Query the video bitrate in video file before opening it
//MM-MMF-VCLNT-CHRTZ-I-0056

class RVidPlyrQryVidBitRateB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryVidBitRateB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryVidBitRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Query the video format MIME type
//MM-MMF-VCLNT-CHRTZ-I-0057

class RVidPlyrQryVidFmtMimeTypTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryVidFmtMimeTypTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrQryVidFmtMimeTypTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Query the video frame size
//MM-MMF-VCLNT-CHRTZ-I-0060

class RVidPlyrQryVidFrameSizTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryVidFrameSizTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TSize iFrameSize;

private:
	// private constructor
	RVidPlyrQryVidFrameSizTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Query the video frame size
//MM-MMF-VCLNT-CHRTZ-I-0063

class RVidPlyrQryVidFrameSizB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrQryVidFrameSizB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TSize iFrameSize;

private:
	// private constructor
	RVidPlyrQryVidFrameSizB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set video frame rate after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0064

class RVidPlyrSetVidFrameRateTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVidFrameRateTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetVidFrameRateTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Get video frame rate after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0065

class RVidPlyrGetVidFrameRateTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetVidFrameRateTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetVidFrameRateTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get video frame rate  before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0069

class RVidPlyrGetVidFrameRateB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetVidFrameRateB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetVidFrameRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Redraw the current frame in playing state
//MM-MMF-VCLNT-CHRTZ-I-0070

class RVidPlyrRfrshFrmPlyngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrRfrshFrmPlyngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrRfrshFrmPlyngTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Redraw the current frame before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0072

class RVidPlyrRfrshFrmB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrRfrshFrmB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrRfrshFrmB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 								     								     
  
// Get the current frame in playing state
//MM-MMF-VCLNT-CHRTZ-I-0073

class RVidPlyrGetCurrentFrmPlyngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCurrentFrmPlyngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetCurrentFrmPlyngTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Get the current frame before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0075

class RVidPlyrGetCurrentFrmB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCurrentFrmB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetCurrentFrmB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get priority of video play after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0076

class RVidPlyrSetNGetPrtyAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPrtyAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetNGetPrtyAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get priority of video play after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0078

class RVidPlyrSetNGetPrtyB4OpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPrtyB4OpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetNGetPrtyB4OpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get priority of video play after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0081

class RVidPlyrSetNGetBalanceAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetBalanceAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetNGetBalanceAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get balance before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0085

class RVidPlyrSetNGetBalanceB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetBalanceB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrSetNGetBalanceB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get balance with a video only file after opening it
//MM-MMF-VCLNT-CHRTZ-I-0086

class RVidPlyrSetBalanceAftrOpngVidOnlyFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetBalanceAftrOpngVidOnlyFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrSetBalanceAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get balance with out setting any balance after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0087

class RVidPlyrGetBalanceWthoutSetAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetBalanceWthoutSetAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetBalanceWthoutSetAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get balance before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0091

class RVidPlyrGetBalanceB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetBalanceB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();

private:
	// private constructor
	RVidPlyrGetBalanceB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get balance with a video only file after opening it
//MM-MMF-VCLNT-CHRTZ-I-0092

class RVidPlyrGetBalanceAftrOpngVidOnlyFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetBalanceAftrOpngVidOnlyFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;

private:
	// private constructor
	RVidPlyrGetBalanceAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get the current rotation
//MM-MMF-VCLNT-CHRTZ-I-0093

class RVidPlyrSetNGetRotationAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetRotationAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetRotationAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get the current rotation before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0096

class RVidPlyrSetNGetRotationB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetRotationB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetRotationB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get the rotation in stopped state
//MM-MMF-VCLNT-CHRTZ-I-0096

class RVidPlyrSetNGetRotationInStopTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetRotationInStopTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetRotationInStopTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Set and get the scale factor after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0097

class RVidPlyrSetNGetScaleFactorAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetScaleFactorAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetScaleFactorAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get the scale factor in stopped state
//MM-MMF-VCLNT-CHRTZ-I-0100

class RVidPlyrSetNGetScaleFactorInStopTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetScaleFactorInStopTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetScaleFactorInStopTest(const TDesC& aStepName, const TDesC& aSectName);	
	void DoTimerCallback();
	}; 
	
// Get the scale factor without setting any factor after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0101

class RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get the scale factor before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0102

class RVidPlyrSetNGetScalingFactorB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetScalingFactorB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetScalingFactorB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get the crop region after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0103

class RVidPlyrSetNGetCropRegionAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetCropRegionAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetCropRegionAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get the crop region before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0104

class RVidPlyrSetNGetCropRegionB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetCropRegionB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetCropRegionB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Get the crop region without setting any value
//MM-MMF-VCLNT-CHRTZ-I-0105

class RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Get the crop region without setting any value before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0106

class RVidPlyrGetCropRegionWthOutSetB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCropRegionWthOutSetB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetCropRegionWthOutSetB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 							 					 							 					 							 					 							 																	 											 					 							 										 						 							 								     	 		 								     	 				 								     	 								     		 			 								     		 			 								     		 											     		 								     						   				   					   				 					   					  						  								 				   								 				  								 				  	  								 				  								 				 								 												 									     	  	 										   									     	  	 										     	  	 									     	  									     	 										     	 											     	 											     	 									     									        	  										        	  			        	      		       		        	      		        	         		        	        

// Get the number of metadata after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0107

class RVidPlyrGetNumMetaDataAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetNumMetaDataAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetNumMetaDataAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Get the number of metadata before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0111

class RVidPlyrGetNumberOfMetadataB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetNumberOfMetadataB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetNumberOfMetadataB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get metadata entry  after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0112

class RVidPlyrGetMetaDataEntryAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetMetaDataEntryAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetMetaDataEntryAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Get metadata entry before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0116

class RVidPlyrGetMetadataEntryB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetMetadataEntryB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetMetadataEntryB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get the controller info after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0117

class RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Get the controller info before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0121

class RVidPlyrGetCntrlrInfoB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrGetCntrlrInfoB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrGetCntrlrInfoB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Start direct screen access after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0122

class RVidPlyrStartDrctScrnAcsAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStartDrctScrnAcsAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrStartDrctScrnAcsAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Stop and start direct screen access before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0123

class RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set display window after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0124

class RVidPlyrSetDsplyWndowAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetDsplyWndowAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetDsplyWndowAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

// Set display window after opening the file with invalid display window
//MM-MMF-VCLNT-CHRTZ-I-0136

class RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	void CloseTest();
	
protected:
    void InitWservL();
    void DeInitWserv();

	
protected:
    // Window server related objects
    RWsSession iWs1;
    CWsScreenDevice* iScreenDevice1;
    RWindow* iWindow1;
    RWindowGroup iRootWindow1;
	
private:
	// private constructor
	RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 

//Create video player multiple times
//MM-MMF-VCLNT-CHRTZ-I-0150

class RVPlyrCrtMltpleTimesTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVPlyrCrtMltpleTimesTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	void CloseTest();
	
	// State machine functions
	void Fsm();
	
	// CVideoPlayerUtility instance to check intitialise for 2nd time
	CVideoPlayerUtility* iUtility2;
	
private:
	// private constructor
	RVPlyrCrtMltpleTimesTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Open the video file multiple times
//MM-MMF-VCLNT-CHRTZ-I-0151

class RVidPlyrOpnFileMultTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrOpnFileMultTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrOpnFileMultTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	

// Close a video file without opening the file.
//MM-MMF-VCLNT-CHRTZ-I-0152

class RVidPlyrCloseFileWithoutopngTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrCloseFileWithoutopngTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrCloseFileWithoutopngTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Prepare the video clips multiple times
//MM-MMF-VCLNT-CHRTZ-I-0153

class RVidPlyrPrepareMultTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPrepareMultTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
	
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrPrepareMultTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Play a corrupted video file 
//MM-MMF-VCLNT-CHRTZ-I-0154

class RVidPlyrPlayCorruptedFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayCorruptedFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;		
	
private:
	// private constructor
	RVidPlyrPlayCorruptedFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Open a video file without extension 
//MM-MMF-VCLNT-CHRTZ-I-0155

class RVidPlyrOpnFileWthOutExtnTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrOpnFileWthOutExtnTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;		
	
private:
	// private constructor
	RVidPlyrOpnFileWthOutExtnTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Play a video file with invalid start and end points
//MM-MMF-VCLNT-CHRTZ-I-0156

class RVidPlyrPlayFileInvStEndptsTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayFileInvStEndptsTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrPlayFileInvStEndptsTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Call stop after completion of video play
//MM-MMF-VCLNT-CHRTZ-I-0157

class RVidPlyrStopPlayAfterComplnOfPlayTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrStopPlayAfterComplnOfPlayTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrStopPlayAfterComplnOfPlayTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set video frame rate with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0158

class RVidPlyrSetVidFrameRateInvValTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetVidFrameRateInvValTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetVidFrameRateInvValTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get priority with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0159

class RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	};
	
// Set and get playback balance with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0160

class RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Play an audio file
//MM-MMF-VCLNT-CHRTZ-I-0161

class RVidPlyrPlayAudioFileTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrPlayAudioFileTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	TFileName iFilename1;			
	
private:
	// private constructor
	RVidPlyrPlayAudioFileTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 	
	
// Set and get rotation with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0162

class RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 	
	
// Set and get scale factor with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0163

class RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 
	
// Set and get crop region with some invalid value
//MM-MMF-VCLNT-CHRTZ-I-0164

class RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest : public RTestBaseVideoPlay
	{
public:
	// static NewL() function to initialise
	static RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest* NewL(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	void DoKickoffTestL();
			
	// State machine functions
	void Fsm();
	
private:
	// private constructor
	RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName);	
	}; 										 											 									 		 	 									 		 	 							 		 								 		 							 					 							 								 			 	 			 													 			 	 			 							 			 	 								 			 	 							 			 	 							 			 							 					 							 					 							 												 					 												 					 							 					 							 					 							 																	 											 					 							 										 						 							 								     	 		 								     	 				 								     	 								     		 			 								     		 			 								     		 											     		 								     						   				   					   				 					   					  						  								 				   								 				  								 				  	  								 				  								 				 								 												 									     	  	 										   									     	  	 										     	  	 									     	  									     	 										     	 											     	 											     	 									     									        	  										        	  			        	      		       		        	      		        	         		        	        
#endif // PLAY_TESTSTEP_H
