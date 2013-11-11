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
// A unit test wrapper over the CLbsSuplPushImpl class
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushRProperty.h"
#include "Te_LbsSuplPushLbsSystemController.h"

TBool CTe_LbsSuplPushImpl::iEnableAsserts = ETrue;

/**
A public wrapper over the protected CActive:SetActive for the CLbsSuplPushImpl class.

@param aPushImpl [In] A reference on the object the method is called for.
*/
void CTe_LbsSuplPushImpl::SetActive(CLbsSuplPushImpl& aPushImpl)
  	{
  	CTe_LbsSuplPushImpl& pushImpl = static_cast<CTe_LbsSuplPushImpl&>(aPushImpl);
  	pushImpl.CLbsSuplPushImpl::SetActive();
  	}


/** Redefines the ASSERT_DEBUG macro to be able to turn ot onn/off at run-time. */
#undef __ASSERT_DEBUG
#define __ASSERT_DEBUG(c,p) (void)((c || !CTe_LbsSuplPushImpl::AssertsEnabled()) ||(p,0))

/** Replaces the RProperty class with its test wrapper - RTe_LbsSuplPushRProperty. */
#define RProperty RTe_LbsSuplPushRProperty
/** Replaces the RLbsSystemController class with its test wrapper - RTe_LbsSuplPushLbsSystemController. */
#define RLbsSystemController RTe_LbsSuplPushLbsSystemController


#include "lbssuplpushimpl.cpp"


void CLbsSuplPushImpl::CheckStateL(TInt aPossibleStates)
	{
	if((iState & aPossibleStates)==0)
		{
		User::Leave(KErrGeneral); //Leavescan, leavescan bug while redefining __ASSERT_DEBUG macro at line 30
		}
	
	switch(iState)
		{
		case ECreated:
			if(!iTimer)
				{
				User::Leave(KErrGeneral); 
				}
			break;
		case EInitialized:
		case EWaitingAck:
		case EWaitingBusy:
			if(!iTimer || iPropOwnerSecureId==TUid::Uid(0))
				{
				User::Leave(KErrGeneral); 
				}
			break;		
		default:
			__ASSERT_ALWAYS(0, User::Invariant());
			break;
		}
	}

