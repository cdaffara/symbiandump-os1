// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestStepCsdAgt which is the base class for all 
// the Csd Agt suite test steps
// 
//

#if (!defined __CSDAGTTESTSTEP_H__)
#define __CSDAGTTESTSTEP_H__

#include "tnifnotify.h"		// MDummyNifToAgtHarnessNotify
#include "AgtNotify.h"      // AgtNotify - used to talk to dummy ETEL
#include "tdummynifagentref.h"	// Dummy version of CNifAgentRef

#include "CsdDialogServer.h" 

/**
 * Forward declarations.
 *
 */
class CTestSuite;
class CTestSuiteCsdAgt;

/**
 * CTestStepCsdAgt.
 *
 */
class CTestStepCsdAgt : public CTestStep, public MDummyNifToAgtHarnessNotify, public CDialogServer::MObserver
	{
public:
	CTestStepCsdAgt(TPtrC aName);
	void ConstructL();
	~CTestStepCsdAgt();

	// pointer to suite which owns this test 
	CTestSuiteCsdAgt * iCsdAgtSuite;

	// Delay
	void DelayL(TInt aMicroseconds);

	// MDummyNifToAgtHarnessNotify
	virtual TInt Notification(TAgentToNifEventType aEvent, TAny* aInfo);
	virtual void AgentProgress(TInt aStage, TInt aError);

	// Called by framework to do the test
	virtual enum TVerdict doTestStepL( void );

	// Pure Virtual, overridden by derived classes to implement a specific CSDAgt test
	virtual enum TVerdict doCsdAgtTestStepL( void )=0;

public:
	// From CDialogServer::MObserver
	virtual void DialogHasAppeared();

public:
	enum TVerdict iResult;

protected:

	// Functions to switch between modes
	void SetGSMModeL(MAgtNotify & aAgtNotify);
	void SetGPRSModeL(MAgtNotify & aAgtNotify);

	// Common Connection Routines
	void NormalConnectL();
	void ReConnectL();
	void ConnectExpectErrorL(TInt aError, TInt aStage);
	void ConnectAndWaitForAsyncronousRequestL();
	void NormalConnectDisconnectExpectsErrorL(TInt aError, TInt aStage);

	// Internal virtual functions called by the Notification() member
	// Should be overridden when required in base classes
	virtual void CsdAgtDisconnectRequest();

	CDialogServer* iDlgSvr;
	TInt iStage;			// Current stage in a test case

private:
	// Callback for DelayL() function
	static TInt TimerCallback(TAny *);
	};

#endif /* __CSDAGTTESTSTEP_H__ */
