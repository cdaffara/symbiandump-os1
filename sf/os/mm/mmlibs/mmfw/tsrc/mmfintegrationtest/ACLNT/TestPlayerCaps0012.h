
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

#ifndef TESTPLAYERCAPS0012_H__
#define TESTPLAYERCAPS0012_H__


#include "TSI_MMFACLNT.h"


/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntCaps0012
 *
 */         
class CTestMmfAclntCaps0012 :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
private:
	enum InternalState 
		{
		EWaitingForServer=0,
		EInitPlayer,
		EPlaying
		};
		
		
public:
	static CTestMmfAclntCaps0012* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	static CTestMmfAclntCaps0012* NewLC(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	
	~CTestMmfAclntCaps0012();
	void Close();
	virtual TVerdict DoTestStepL();

	void CreatePlayer();
	void BeginPlayback();
	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	virtual void MapcPlayComplete(TInt aError);
	
	void DoProcess(TInt aError);
	
	class CServerMonitor : public CActive 
		{
	public:
		static CServerMonitor* NewL(CTestMmfAclntCaps0012* aParent);
		void ConstructL();
		void RunL();
		void DoCancel();
		
		TRequestStatus& ActiveStatus();
	private:
		CServerMonitor(CTestMmfAclntCaps0012* aParent);
	private:
		CTestMmfAclntCaps0012* iParent;
		};

private:
	CTestMmfAclntCaps0012(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	
private:
	CMdaAudioPlayerUtility* iPlayer;	
	CServerMonitor* iServerMonitor;
	TVerdict iVerdict;

	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	InternalState iState;
	};
#endif	// TESTPLAYERCAPS0012_H__
