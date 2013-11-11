// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//

#ifndef TEST_STEP_RESOURCE_NOTIFICATION_H
#define TEST_STEP_RESOURCE_NOTIFICATION_H

#include "mmfclientaudiorecorder.h"

/**
 *
 * DEF061822  aNotificationData in the MarncResourceAvailable() callback contains wrong data
 *
 */
class CTestStepResNotificationData :	public	CTestStepUnitMMFAudClient,
									    public	MMdaObjectStateChangeObserver,
									    public  MMMFAudioResourceNotificationCallback
	{
public:
	static CTestStepResNotificationData* NewL();
	~CTestStepResNotificationData();
	void Close();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	// from MMMFAudioResourceNotificationCallback
	void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
private:
	CTestStepResNotificationData();
	
	static TInt TimerCbGate(TAny* aPtr);
	void StartHighRecorder();
	
private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecorderUtilityLowP;
	CMdaAudioRecorderUtility* iRecorderUtilityHighP;
	CTimer* iDelayTimer;
	TBool iGotNotification;
	TInt iOpenCompletedCnt;
	};




#endif // ndef TEST_STEP_RESOURCE_NOTIFICATION_H

