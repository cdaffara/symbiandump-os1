
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

#ifndef TESTPLAYERCAPS0014_H__
#define TESTPLAYERCAPS0014_H__


#include "TSI_MMFACLNT.h"


/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntCaps0014
 *
 */
class CTestMmfAclntCaps0014 :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
private:
	enum InternalState 
		{
		EWaitingForServer=0,
		EInitPlayer,
		EPlaying
		};
		
		
public:
	static CTestMmfAclntCaps0014* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	static CTestMmfAclntCaps0014* NewLC(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	
	~CTestMmfAclntCaps0014();
	void Close();
	virtual TVerdict DoTestStepL();

	void CreatePlayer();
	void BeginPlayback();
	// from MMdaAudioPlayerCallback
//	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
//	virtual void MapcPlayComplete(TInt aError);
	
	void DoProcess(TInt aError);
	
	class CServerMonitor : public CActive 
		{
	public:
		static CServerMonitor* NewL(CTestMmfAclntCaps0014* aParent);
		void ConstructL();
		void RunL();
		void DoCancel();
		
		TRequestStatus& ActiveStatus();
	private:
		CServerMonitor(CTestMmfAclntCaps0014* aParent);
	private:
		CTestMmfAclntCaps0014* iParent;
		};

	// from MMdaObjectStateChangeObserver	
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntCaps0014(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	
private:
	CMdaAudioRecorderUtility* iPlayer;	
	CServerMonitor* iServerMonitor;
	TVerdict iVerdict;

	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	InternalState iState;
	};
#endif	// TESTPLAYERCAPS0014_H__
