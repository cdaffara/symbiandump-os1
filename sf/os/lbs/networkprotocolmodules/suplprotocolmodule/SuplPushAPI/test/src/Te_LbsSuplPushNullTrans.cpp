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
// Creation transition for the CLbsSuplPushImpl class
// 
//

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushNullTrans.h"


/**
Creates a test state transition for the CSuplPushImpl to the ECreated state and puts it into 
the cleanup stack.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushNullNewL* CTe_LbsSuplPushNullNewL::NewLC()
	{
	CTe_LbsSuplPushNullNewL* self = new (ELeave) CTe_LbsSuplPushNullNewL();
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor
*/
CTe_LbsSuplPushNullNewL::CTe_LbsSuplPushNullNewL() 
	{
	//Intentionally left blank
	}	


/**
Runs the various scenarios for this transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushNullNewL::RunTransL()
	{
	CLbsSuplPushImpl* pushImpl = CLbsSuplPushImpl::NewL(ELbsSuplPushChannelSMS, *this);
	CleanupStack::PushL(pushImpl);
	CTe_LbsSuplPushImpl::DisableAsserts();
	pushImpl->OnTimerError(0,0); //Just to improve the code coverage
	CTe_LbsSuplPushImpl::EnableAsserts();
	pushImpl->CheckStateL(CLbsSuplPushImpl::ECreated);
	CleanupStack::PopAndDestroy(pushImpl);
	
	pushImpl = CLbsSuplPushImpl::NewL(ELbsSuplPushChannelWAP, *this);
	CleanupStack::PushL(pushImpl);
	pushImpl->CheckStateL(CLbsSuplPushImpl::ECreated);
	CleanupStack::PopAndDestroy(pushImpl);
	
	CTe_LbsSuplPushImpl::DisableAsserts();
	TRAPD(err, CLbsSuplPushImpl::NewL(ELbsSuplPushChannelUnknown, *this));
	if(err!=KErrGeneral)
		{
		User::Leave(KErrGeneral);
		}
	CTe_LbsSuplPushImpl::EnableAsserts();
	
	//Memory allocation failure test
	User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, 1);
	TInt* marker = new TInt(0);
	delete marker;
	
	if(!marker) //The OS is in the udeb mode
		{
		marker = new TInt(0);
		delete marker;
	
		for(TInt i=1; marker; i++)
			{
			User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, i);
			CTe_LbsSuplPushImpl::DisableAsserts();	
			pushImpl = 0;
			TRAP(err, pushImpl = CLbsSuplPushImpl::NewL(ELbsSuplPushChannelSMS, *this));
			CTe_LbsSuplPushImpl::EnableAsserts();	
			marker = new TInt(0);
			delete marker;
			if(!marker || (marker && err==KErrNone)) 
				{
				if(pushImpl==0)
					{
					User::Leave(KErrGeneral);
					}
				CleanupStack::PushL(pushImpl);
				User::LeaveIfError(err);
				pushImpl->CheckStateL(CLbsSuplPushImpl::ECreated);
				CleanupStack::PopAndDestroy(pushImpl);
				}
			}
		}
	}

/**
Receives notification when a SUPL INIT message was sent to the LBS sub-system or failed.
Overrides the pure virtual MLbsSuplPushObserver::OnSuplInitComplete.

@see MLbsSuplPushObserver
*/
void CTe_LbsSuplPushNullNewL::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, 
		TLbsSuplPushRequestId /*aReqId*/, TInt /*aError*/, TInt /*aReserved*/)
	{
	//Intentionally left blank
	}


