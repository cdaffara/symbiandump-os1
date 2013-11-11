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
// This contains CTestCasePsdAgt which is the base class for all 
// the Psd Agt suite test steps
// 
//

#if (!defined __PSDAGTTESTSTEP_H__)
#define __PSDAGTTESTSTEP_H__

#include <tnifnotify.h>  // MDummyNifToAgtHarnessNotify
#include "AgtNotify.h"   // AgtNotify - used to talk to dummy ETEL
#include "tdummynifagentref.h"   // CNifAgentRefN1

#include "PsdDialogServer.h" 
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

class CTestSuite;
class CTestSuitePsdAgt;

class CTestStepPsdAgt : public CTestStep, public MDummyNifToAgtHarnessNotify, public CDialogServer::MObserver
{
public:
	CTestStepPsdAgt(const TPtrC &aName);

	// tests may optionaly implement pre and post ambles
	virtual enum TVerdict doTestStepPreambleL( void );
	virtual enum TVerdict doTestStepPostambleL( void );

	// pointer to suite which owns this test 
	CTestSuitePsdAgt * iPsdAgtSuite;

	void DelayL(TInt aMicroseconds);
	
	// MDummyNifToAgtHarnessNotify
	virtual TInt Notification(TAgentToNifEventType aEvent, TAny* aInfo);
	virtual void AgentProgress(TInt aStage, TInt aError);

	// Called by framework to do the test
	virtual enum TVerdict doTestStepL( void );
	
	// Pure Virtual, overridden by derived classes to implement a specific PSDAGX test
	virtual enum TVerdict doPsdAgtTestStepL( void )=0;

public:
	// From CDialogServer::MObserver
	virtual void DialogHasAppeared();

public:
	enum TVerdict iResult;

protected:
	
	// Functions to switch between GPRS and CDMA modes
	void SetGPRSModeL(MAgtNotify & aAgtNotify);


	// Common Connection Routines
	void NormalConnectL();
	void ReConnectL();
	void NormalConnectOOML();
	void ReConnectOOML();
	void ConnectExpectErrorL(TInt aError, TInt aStage);
	void QoSChangeDisconnectL(MAgtNotify & aAgtNotify);
	void QoSChangeNoDisconnectL(MAgtNotify & aAgtNotify);
	void QoSChangeWithinTimeoutL(MAgtNotify & aAgtNotify);
	void ConnectAndWaitForAsyncronousRequestL();
	void ConnectAndWaitForEightySecondsL();
	void ConnectAndWaitForTwentySecondsL();


	// Internal virtual functions called by the Notification() member
	// Should be overridden when required in base classes
	virtual void PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived);
	virtual void PsdAgtDisconnectRequest();

	CDialogServer* iDlgSvr;
	TInt iStage;			// Current stage in a test case
	CActiveScheduler *iActiveScheduler;

private:
	// Callback for Delay() function
	static TInt TimerCallback(TAny *);
	
};

#endif /* __PSDAGTTESTSTEP_H__ */
