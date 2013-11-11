// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TE_CONESTEPBASE__
#define __TE_CONESTEPBASE__


#include "te_graphicsperformanceSuiteStepBase.h"

#include <coemain.h>
#include <eikappui.h>
#include <eikenv.h>


/**
Base class for tests steps which require CONE. In addition to CTe_graphicsperformanceSuiteStepBase's virtuals,
subclasses must implement void InitUIL() in order to initialise CONE controls on the screen.

The execution scenario:
1. doTestStepPreambleL() is executed in main test step's thread - creates and starts new CONE thread.
1a. InitUIL() is run in CONE thread.
1b. CONE's active scheduler is started in CONE thread.
2. doTestStepL() is executed in main test step's thread.
3. doTestStepPostambleL() is executed in main test step's thread
3a. CONE thread is stopped
*/
class CTe_ConeStepBase : public CTe_graphicsperformanceSuiteStepBase
	{
	friend class CConeThreadStopper;
	
public:
	inline RThread* ConeThread();
	
protected:
	// From CTe_graphicsperformanceSuiteStepBase
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	virtual void InitUIL(CCoeEnv* aCoeEnv) = 0; // implementation must call aCoeEnv->SetAppUi(appUi)
	
private:
	static TInt ConeThreadFunction(TAny* aParams);
	
	CCoeEnv* ConstructControlEnvironment();
	TInt StartControlEnvironment();
	void StopControlEnvironment();
	
private:    
    RThread iConeThread;
	RSemaphore iInitUISemaphore;
	TRequestStatus* iTestCompletionStatus;
	};

/**
 * Stops CONE basing on given TRequestStatus. TRequestStatus is updated in doTestStepPostambleL, which
 * stops the CONE thread.
 */
class CConeThreadStopper : public CActive
    {
public:
    ~CConeThreadStopper();
    CConeThreadStopper(CTe_ConeStepBase& aTestStep);
    void Start(TRequestStatus *&aStopStatus);
    
private:
    // From CActive
    void DoCancel();
    void RunL();
    
private:
    CTe_ConeStepBase& iTestStep;
    };


inline RThread* CTe_ConeStepBase::ConeThread() 
    {
    return &iConeThread;
    }


#endif
