
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
// TestPlayerUtils.h
// Header file: Basic  tests.
// 
//

/**
 @file TestPlayerUtils.h
*/

#ifndef TESTPLAYERCAPSDEVSOUNDMULTIPROCESSBASE_H__
#define TESTPLAYERCAPSDEVSOUNDMULTIPROCESSBASE_H__

#include <testframework.h>
#include "TSI_MMF_SDEVSOUND_SUITE.h"
#include "TSI_MMF_SDEVSOUND_STEP.h"

const TInt KDevSoundPriorityLow=0;
const TInt KDevSoundPriorityHigh=100;


/**
 * Class "CSecureDevSoundMultiProcessTestStepBase"
 *
 * @class CSDSMPTSBase
 *
 */
class CSDSMPTSBase : public CTestStepSDevSound
	{
protected:
	enum InternalState 
		{
		EWaitingForServer=0,
		EInitPlayer,
		EPlaying
		};

public:
	static CSDSMPTSBase* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	~CSDSMPTSBase();
	void Close();
	virtual TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();

	virtual void DoProcess(TInt aError);

	class CServerMonitor : public CActive 
		{
	public:
		static CServerMonitor* NewL(CSDSMPTSBase* aParent);
		void ConstructL();
		void RunL();
		void DoCancel();
		
		TRequestStatus& ActiveStatus();
	private:
		CServerMonitor(CSDSMPTSBase* aParent);
	private:
		CSDSMPTSBase* iParent;
		};

protected:
	CSDSMPTSBase(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);

	void TestSetVolume(TInt aVol);
	void TestSetPriority(TInt aPriority);
	TVerdict TestInitialize(TMMFState aMode);
	void TestSetGain(TInt aGain);

	void ResetCallbacks();
	TInt GetCallbackTotal();

	CServerMonitor*		iServerMonitor;
	TVerdict 			iVerdict;

	TBuf<KNameBufSize>	iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize>	iKeyName;		// Key name for retrieving filename
	InternalState		iState;
	};

#endif // TESTPLAYERCAPSDEVSOUNDMULTIPROCESSBASE_H__
