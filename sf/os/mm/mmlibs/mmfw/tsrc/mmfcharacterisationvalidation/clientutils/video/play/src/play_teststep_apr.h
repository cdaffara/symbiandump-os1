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

#ifndef PLAY_TESTSTEP_APR_H
#define PLAY_TESTSTEP_APR_H

#include "char_videoplay_common_apr.h"



/** 
 * Register (after Open and before Play of video) , receive notification of a resource available event.
 *
 * RTestVclntAprRegAftrOpenAndB4Play
 * MM-MMF-VCLNT-CHRTZ-I-0125
 * 
 */
class RTestVclntAprRegAftrOpenAndB4Play : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprRegAftrOpenAndB4Play* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprRegAftrOpenAndB4Play( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Register before opening the file
 *
 * RTestVclntAprRegAftrOpenAndB4Play
 * MM-MMF-VCLNT-CHRTZ-I-0127
 * 
 */
class RTestVclntAprRegBeforeOpen : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprRegBeforeOpen* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprRegBeforeOpen( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Register with unsupported event UID
 *
 * RTestVclntAprRegWithUnSupptdEvntUid
 * MM-MMF-VCLNT-CHRTZ-I-0128
 * 
 */
class RTestVclntAprRegWithUnSupptdEvntUid : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprRegWithUnSupptdEvntUid* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprRegWithUnSupptdEvntUid( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Register multiple times
 *
 * RTestVclntAprRegMultpleTimes
 * MM-MMF-VCLNT-CHRTZ-I-0129
 * 
 */
class RTestVclntAprRegMultpleTimes : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprRegMultpleTimes* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprRegMultpleTimes( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Cancel notification
 *
 * RTestVclntAprCancelNtfnAftrOpen
 * MM-MMF-VCLNT-CHRTZ-I-0130
 * 
 */
class RTestVclntAprCancelNtfnAftrOpen : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprCancelNtfnAftrOpen* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprCancelNtfnAftrOpen( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Cancel notification with Invalid event Uid
 *
 * RTestVclntAprCancelNtfnWthInvalidEvntUid
 * MM-MMF-VCLNT-CHRTZ-I-0132
 * 
 */
class RTestVclntAprCancelNtfnWthInvalidEvntUid : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprCancelNtfnWthInvalidEvntUid* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprCancelNtfnWthInvalidEvntUid( const TDesC& aTestName, const TDesC& aSectName );
	};
	
/** 
 * Test for WillResumePlay() using 3 clients.
 *
 * RTestVclntAprWillResumePlay
 * MM-MMF-VCLNT-CHRTZ-I-0133
 * 
 */
class RTestVclntAprWillResumePlay : public RTestBaseVideoPlayAPR
	{
public:
	static RTestVclntAprWillResumePlay* NewL( const TDesC& aTestName, const TDesC& aSectName );
    void DoKickoffTestL();
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	void Finished(); // for CVclntAviAprWillResumeTimer	


protected:
	RTestVclntAprWillResumePlay( const TDesC& aTestName, const TDesC& aSectName );
	CVclntAviAprWillResumeTimer* iWillResumePlayTimer;

	};

/**
 * 
 * CVclntAviAprWillResumeTimer
 * MM-MMF-VCLNT-CHRTZ-I-0133
 * - Timer for checking if no notification occurs in 5 seconds
 *   after calling WillResumePlay()
 *
 */
class CVclntAviAprWillResumeTimer : public CTimer
    {
public:
    static CVclntAviAprWillResumeTimer* NewL(RTestVclntAprWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    void RunL();
private:
    CVclntAviAprWillResumeTimer(RTestVclntAprWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestVclntAprWillResumePlay* iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };

/** 
 * Cancel notification without registration
 *
 * RTestVclntAprCancelNtfnWthRegnAftrOpen
 * MM-MMF-VCLNT-CHRTZ-I-0134
 * 
 */
class RTestVclntAprCancelNtfnWthRegnAftrOpen : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprCancelNtfnWthRegnAftrOpen* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprCancelNtfnWthRegnAftrOpen( const TDesC& aTestName, const TDesC& aSectName );
	};

/** 
 * Cancel notification multiple times
 *
 * RTestVclntAprCancelNtfnMultTimesAftrOpen
 * MM-MMF-VCLNT-CHRTZ-I-0135
 * 
 */
class RTestVclntAprCancelNtfnMultTimesAftrOpen : public RTestBaseVideoPlayAPR
	{
public:
	// static NewL() function to initialise
	static RTestVclntAprCancelNtfnMultTimesAftrOpen* NewL( const TDesC& aTestName, const TDesC& aSectName );
	// from RAsyncTestStep
	void DoKickoffTestL();
	// State machine function
	void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
private:
	// Private constructor
	RTestVclntAprCancelNtfnMultTimesAftrOpen( const TDesC& aTestName, const TDesC& aSectName );
	};
		
#endif // PLAY_TESTSTEP_APR_H
