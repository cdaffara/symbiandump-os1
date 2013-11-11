// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TOwnNumbersManager implementation
// 
//

 #include "townnumbersmanager.h"
 #include "rdebuglogger.h"
 
 void TOwnNumbersManager::CacheOwnNumbersL()
 	{
	if(!iGotNumbers) //try retrieving numbers once only
		{
		//on demand retrieval of own numbers. This must be called on a thread other than the main server thread 
		//as an exec call is made to switch opening the RPhone session to the server thread.
		RDEBUGPRINTLOGGER1(_L("TOwnNumbersManager::CacheOwnNumbersL - attempting to get Own numbers"));
		// Create an active scheduler for this thread
	    CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	    CActiveScheduler::Install(sched);
	    CleanupStack::PushL(sched);
	    
	    //invoking TRPSOwnNumbers::RetrieveOwnNumbersL requires Active Scheduler to be present.
	    CAsyncCallBack* asyncCallBack = new (ELeave) CAsyncCallBack(CActive::EPriorityStandard);
	    CleanupStack::PushL(asyncCallBack);
	    
	    iRetrieveError = KErrNone;
	    asyncCallBack->Set(TCallBack(AsyncCallback, this));
	    asyncCallBack->CallBack();
	    CActiveScheduler::Start();
	    
	    RDEBUGPRINTLOGGER2(_L("TOwnNumbersManager::CacheOwnNumbersL - own numbers completed with %d"), iRetrieveError);
	   	TPtrC ptr = iOwnNumbers.OwnVoiceNumber();
		RDEBUGPRINTLOGGER2(_L("OwnVoiceNumber = %S"), &ptr);
		ptr.Set(iOwnNumbers.OwnDataNumber());
		RDEBUGPRINTLOGGER2(_L("OwnDataNumber = %S"), &ptr);
		ptr.Set(iOwnNumbers.OwnFaxNumber());
		RDEBUGPRINTLOGGER2(_L("OwnFaxNumber = %S"), &ptr);
		
	    CleanupStack::PopAndDestroy(asyncCallBack);
	    CleanupStack::PopAndDestroy(sched);
		iGotNumbers = ETrue;		
		}
 	}
 	
 TInt TOwnNumbersManager::AsyncCallback(TAny* aTOwnNumbersManagerInstance)
 	{
 	static_cast<TOwnNumbersManager*>(aTOwnNumbersManagerInstance)->CacheOwnNumbersAO();
	return KErrNone;
 	}
 	
 void TOwnNumbersManager::CacheOwnNumbersAO()
 	{
//  Ini file is being used for own number, instead of reading it from SIM because
//  Integration test suite over trp, was crashing on H4 board while Retrieving own number 
// 	TRAPD(err, iOwnNumbers.RetrieveOwnNumbersL(iEtelSessMgr->GetPhoneL(KMainServer, KMainPhone)));
// 	iRetrieveError = err;
 	CActiveScheduler::Stop();
 	}
