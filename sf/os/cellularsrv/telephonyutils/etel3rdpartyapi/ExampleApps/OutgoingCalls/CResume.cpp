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


#include "CResume.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CResume class
*/
CResume* CResume::NewL(MExecAsync* aController)
	{
	CResume* self = new(ELeave) CResume(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CResume::~CResume()
	{
	Cancel();
	}

/**
Resumes the specified call if the dynamic call capabilities allow it.

@param aCallId Identifier of the call to resume
*/
void CResume::DoStartRequestL(CTelephony::TCallId aCallId)
	{
	_LIT(KDummyAnswerPanic, "CResume Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
   iRequestNotify = EFalse;
	CTelephony::TCallCapsV1 callCapsV1;
   CTelephony::TCallCapsV1Pckg callCapsV1Pckg(callCapsV1);
   
   // Retrieves the dynamic call capabilities for calls you dialled or answered with CTelephony.
   iTelephony->GetCallDynamicCaps(aCallId, callCapsV1Pckg);

   if( callCapsV1.iControlCaps & CTelephony::KCapsResume )
      {
      // The call represented by aCallId can be resumed.
      iTelephony->Resume(iStatus, aCallId);
      SetActive();
      }
   else
      {
      // The call cannot be resumed.
      }
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CResume::CResume(MExecAsync* aController)
	: CISVAPIAsync(aController, KResume)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CResume::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and calls ExampleComplete() to notify
the menu object that the example has finished.
*/
void CResume::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the output to console if there is no error
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::Resume()
*/
void CResume::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EResumeCancel);
	}

