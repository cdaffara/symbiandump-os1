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
//


#include <e32property.h>
#include "blocker.h"


//----
void StartBlockerL(TCFModuleInfo* apModuleInfo)
	{
    //-- Create and install the active scheduler.
    CActiveScheduler * pA = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(pA);
    
    CBlockerChannelHandler* pChanHandler = CBlockerChannelHandler::NewL(apModuleInfo->iRxQueues, apModuleInfo->iTxQueues);

	pChanHandler->MaybeBlockStartup();
    
    // Signal to the RootServer that init is completed successfully
    RThread::Rendezvous(KErrNone);
	
    
    CActiveScheduler::Start();
    delete pChanHandler;
	
	}

//--- CPM thread entry point
EXPORT_C TInt CpmThreadEntryPoint(TAny* aArg)
	{
	TCFModuleInfo* pModuleInfo = (TCFModuleInfo*)aArg;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAP_IGNORE(StartBlockerL(pModuleInfo));
	delete cleanup;    
	
	return 0;
	
	}



//---- CBlockerChannelHandler handler implementation

CBlockerChannelHandler::CBlockerChannelHandler()
:CCFModuleChannelHandler(CActive::EPriorityStandard)
	{
	}

//----

CBlockerChannelHandler* CBlockerChannelHandler::NewL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues)
	{
	CBlockerChannelHandler* pSelf = new (ELeave) CBlockerChannelHandler;
	
	CleanupStack::PushL(pSelf);
	pSelf->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pSelf);
	
	return pSelf;
	}

//----

void CBlockerChannelHandler::MaybeBlockStartup()
	{
    // If we have been told to block before rendezvous then wait until we're signalled to proceed
	RProperty blockProp;
	TInt err = blockProp.Attach(TBlockerSID, EBlockingStateKey);
	if(err == KErrNone)
		{
		TRequestStatus status;
		do
			{
			blockProp.Subscribe(status);
			TInt blockState;
			err = blockProp.Get(blockState);
			if(err != KErrNone || blockState == EStateDontBlock || blockState >= EStateRelease)
				{
				blockProp.Cancel();
				}
			else if(blockState == EStateBlock)
				{
				blockProp.Set(EStateBlocking);
				}
			User::WaitForRequest(status);
			} while(status == KErrNone);
		blockProp.Close();
		}
	}	


void CBlockerChannelHandler::CFMessageShutdown(const CommsFW::TCFShutdownMsg& /*aMessage*/)
	{
	CActiveScheduler::Stop();
	}


