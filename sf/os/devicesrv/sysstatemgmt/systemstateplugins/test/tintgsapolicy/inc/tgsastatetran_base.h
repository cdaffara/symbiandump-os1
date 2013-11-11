// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 

#ifndef __TGSA_STATETRAN_H__
#define __TGSA_STATETRAN_H__

#include <ssm/ssmstateawaresession.h>
#include <e32base.h>
#include <s32file.h>

class CTGsaStateTransition : public CBase, public MStateChangeNotificationSubscriber
	{
public:
	CTGsaStateTransition();
	void ConstructL(const TDesC& aResultFile);
	~CTGsaStateTransition();

public:
	void CurrentSystemStateL();
	void CallStopSchedulerL();
	TBool DoStartStateTranTest();
	void CommitTestResultsL(TInt aTestCompletionReason);
	// from MStateChangeNotificationSubscriber
	void StateChanged(TSsmState aSsmState);

protected:
	RSsmStateAwareSession iSsmStateAwareSess;
	CAsyncCallBack* iAsyncCallbackForStartTest;
	CAsyncCallBack* iAsyncCallbackForStopScheduler;
	RFs iFs;
	RFileWriteStream iFileWriteStream;
	};

#endif	// __TGSA_STATETRAN_H__


