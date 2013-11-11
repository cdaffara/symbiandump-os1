// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TMESSAGE_H__
#define __TMESSAGE_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CMessageReceiver;

class CLowPriorityIdle : public CIdle
	{
public:
	CLowPriorityIdle();
	virtual TInt RunError(TInt aError);
	};

class CTMessage : public CTWsGraphicsBase
	{
public:
	CTMessage(CTestStep* aStep);
	~CTMessage();
	void ConstructL();
	void TestPanicsL();
	void MessageTests(TInt aNoOfMessages,TInt aMsgCountCheck,TBool aSyncSendReceiveFlag,TBool aTestNeedsExtraCheck);
	void OnMessageReceived(const TWsEvent &aEvent, TInt& aMsgCount);
	TInt SendSyncMessages();
	static TInt CallbackLowPriority(TAny *aMessageTest);
	TInt CreateAndSendMessages(TInt aCount,TInt aNoOfMessages);
	void TestMessageQueueOverflowL();
	
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	TBool iSyncSendReceiveFlag;
	TInt iCounter;
	HBufC8* iMsg;
	RPointerArray<CMessageReceiver>* iMessageReceiver;
	CLowPriorityIdle* iLowPriorityObject;
	TInt iState;
	};
	
	
class CMessageReceiver : public CTWindowGroup
	{
public:
	CMessageReceiver(CTClient *aClient, CTMessage *aTMessage);
	void MessageReady(const TWsEvent &aTime);
	inline TInt MessageCount()
		{return iMsgCount;}
	inline void ResetMessageCount()
		{iMsgCount=0;}
private:
	CTMessage *iTMessage; // CTMessage, to call it's OnMessageReceived when message is received
	TInt iMsgCount;	
	};

	

class CTMessageStep : public CTGraphicsStep
	{
public:
	CTMessageStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMessageStep,"TMessage");


#endif
