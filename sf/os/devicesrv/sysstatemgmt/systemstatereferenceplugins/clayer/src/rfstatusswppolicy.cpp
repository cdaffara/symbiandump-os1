// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#include "rfstatusswppolicy.h"
#include "ssmdebug.h"
#include "clayerpanic.h"
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmpatchableconstants.h>
#include <ssm/clayerswp.hrh>


//Path for RF status SwP command list resource file
_LIT(KRFStatusSwPCmdListPath, "z:\\private\\2000d75b\\rfstatusswpcmdlist\\");

_LIT_SECURE_ID(KSsmSecureId , 0x2000D75B);

EXPORT_C MSsmSwpPolicy* CRFStatusSwPPolicy ::NewL()
	{
	CRFStatusSwPPolicy * self = new (ELeave) CRFStatusSwPPolicy ;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CRFStatusSwPPolicy ::CRFStatusSwPPolicy ()
	{
	}

CRFStatusSwPPolicy ::~CRFStatusSwPPolicy ()
	{
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CRFStatusSwPPolicy ::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KRFStatusSwPCmdListPath(), *this);
	}

void CRFStatusSwPPolicy ::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerInvalidResourceReader));

	//Initialise the command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CRFStatusSwPPolicy ::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerInvalidResourceReader));

	iCommandListResourceReader->InitialiseCancel();
	}

void CRFStatusSwPPolicy ::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CRFStatusSwPPolicy ::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	MSsmSwpPolicy::TResponse isTransitionAllowed = ENotAllowed;   
	TBool hasCapability = aMessage.HasCapability(ECapabilityWriteDeviceData, ECapabilityPowerMgmt,
	                    __PLATSEC_DIAGNOSTIC_STRING( "Platsec violation, RF status SwP transition" ));
	
	if(hasCapability)
        {        
        if (IsSsmGracefulOffline())
            {
            //RF ON/OFF is implemented as a substate in Normal instead of SwP for graceful offline notification.
            //Hence the SwP RF OFF/RF ON is restricted for the request which has SSM SID. Only BtSap SwP transition is 
            //allowed to any client with appropriate capability.
            if (aMessage.SecureId() == KSsmSecureId || aSwp.Value() == ESsmBtSap )
                {
                //Allow SwP change only if request has SsmSecureId or the SwP request for BtSap
                isTransitionAllowed = EAllowed;
                }
            }
        else
            {
            isTransitionAllowed = EAllowed;
            }
        }	
	return isTransitionAllowed;
	}

void CRFStatusSwPPolicy ::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerInvalidResourceReader));
	
	iCommandListResourceReader->PrepareCommandList(aSwp.Value(), aSwp, aStatus);
	}

void CRFStatusSwPPolicy ::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerInvalidResourceReader));
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CRFStatusSwPPolicy ::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerInvalidResourceReader));
	return iCommandListResourceReader->GetCommandList();
	}

void CRFStatusSwPPolicy ::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt /*aSeverity*/, TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	(void)aSwp;
	(void)aError;

#if defined(_DEBUG)
	if(KErrNone != aError)
		{
		DEBUGPRINT3A("ERROR: Request SwP change for RF status (SwP key %d) completed with error %d", aSwp.Key(), aError);
		}
	//Complete the status with the error which inturn panics SSM server
	User::RequestComplete(rs, aError);
#else
	//Completing the status with KErrNone to avoid panic in SSM server
	User::RequestComplete(rs, KErrNone);
#endif
	}


void CRFStatusSwPPolicy ::HandleCleReturnValueCancel()
	{
	// Nothing to do as HandleCleReturnValue always completes immediately
	}

TBool CRFStatusSwPPolicy ::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*aResourceId*/)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, CLAYER_PANIC(ECLayerConditionalInfoNotImplemented));
	return EFalse;
	}
