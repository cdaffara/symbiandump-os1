// tsi_mvs_apr.h

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

#include "tsi_mvs_agents_stepbase.h"

/**
 *
 *		RTestMVSAgentAPRRegOnce4SameEvent
 *
 **/
class RTestMVSAgentAPRRegOnce4SameEvent:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRRegOnce4SameEvent* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);

private:
	RTestMVSAgentAPRRegOnce4SameEvent(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRRegB4Open
 *
 **/
class RTestMVSAgentAPRRegB4Open:public RTestStepMVSAgentsAPRBase
	{
public:
    static RTestMVSAgentAPRRegB4Open* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
	
private:
	RTestMVSAgentAPRRegB4Open(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRRegAfterOpenB4Play
 *
 **/
class RTestMVSAgentAPRRegAfterOpenB4Play:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRRegAfterOpenB4Play* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
		
private:
	RTestMVSAgentAPRRegAfterOpenB4Play(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRRegAftrHpClntCompltPlay
 *
 **/
class RTestMVSAgentAPRRegAftrHpClntCompltPlay:public RTestStepMVSAgentsAPRBase
	{
public:
	 static RTestMVSAgentAPRRegAftrHpClntCompltPlay* NewL(const TDesC& aStepName, const TDesC& aSectionName);
	 
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);	
		
private:
	RTestMVSAgentAPRRegAftrHpClntCompltPlay(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRRegSamePriorityPlayers
 *
 **/
class RTestMVSAgentAPRRegSamePriorityPlayers:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRRegSamePriorityPlayers* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
		
private:
	RTestMVSAgentAPRRegSamePriorityPlayers(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRRegAtPlayAndCancelNfn
 *
 **/
class RTestMVSAgentAPRRegAtPlayAndCancelNfn:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRRegAtPlayAndCancelNfn* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);

private:
	RTestMVSAgentAPRRegAtPlayAndCancelNfn(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};

/**
 *
 *		RTestMVSAgentAPRTestWillResumePlay
 *
 **/
class CMVSAPRWillResumeTimer; 
class RTestMVSAgentAPRTestWillResumePlay:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRTestWillResumePlay* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
	void Finished();
private:
	RTestMVSAgentAPRTestWillResumePlay(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	CMVSAPRWillResumeTimer  *iTimerWillResume;
	};

/**
 * 
 * CMVSAPRWillResumeTimer
 * 
 * - Timer for checking if no notification occurs in 5 seconds
 *   after calling WillResumePlay()
 *
 */
class CMVSAPRWillResumeTimer : public CTimer
    {
public:
    static CMVSAPRWillResumeTimer* NewL(RTestMVSAgentAPRTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    void RunL();
private:
    CMVSAPRWillResumeTimer(RTestMVSAgentAPRTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestMVSAgentAPRTestWillResumePlay* 	iParent;
    TTimeIntervalMicroSeconds32 			iWaitTime;
    };
    
    
/**
 *
 *		RTestMVSAgentAPRMulRegAtPlay
 *
 **/
class RTestMVSAgentAPRMulRegAtPlay:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRMulRegAtPlay* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
		
private:
	RTestMVSAgentAPRMulRegAtPlay(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};
    
    
/**
 *
 *		RTestMVSAgentAPRCancelNoResourceNfn
 *
 **/
class RTestMVSAgentAPRCancelNoResourceNfn:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRCancelNoResourceNfn* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
		
private:
	RTestMVSAgentAPRCancelNoResourceNfn(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	};
	
/**
 *
 *		RTestMVSAgentAPRTestWillResumePlayCancelNfn
 *
 **/
class CMVSAPRWillResumeCancelNfnTimer;
class RTestMVSAgentAPRTestWillResumePlayCancelNfn:public RTestStepMVSAgentsAPRBase
	{
public:
	static RTestMVSAgentAPRTestWillResumePlayCancelNfn* NewL(const TDesC& aStepName, const TDesC& aSectionName);

	// from RAsyncTestStep
	void DoKickoffTestL();  //-- for the derived class to implement

	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode, TInt aError);
	void Finished();
		
private:
	RTestMVSAgentAPRTestWillResumePlayCancelNfn(const TDesC& aStepName, const TDesC& aSectionName);						     	
protected:
	CMVSAPRWillResumeCancelNfnTimer 	*iTimerWillResume;
	};

 
/**
 * 
 * CMVSAPRWillResumeCancelNfnTimer
 * 
 * - Timer for checking if no notification occurs in 5 seconds
 *   after calling WillResumePlay()
 *
 */
class CMVSAPRWillResumeCancelNfnTimer : public CTimer
    {
public:
    static CMVSAPRWillResumeCancelNfnTimer* NewL(RTestMVSAgentAPRTestWillResumePlayCancelNfn* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    void RunL();
private:
    CMVSAPRWillResumeCancelNfnTimer(RTestMVSAgentAPRTestWillResumePlayCancelNfn* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestMVSAgentAPRTestWillResumePlayCancelNfn* 		iParent;
    TTimeIntervalMicroSeconds32 						iWaitTime;
    };
       
