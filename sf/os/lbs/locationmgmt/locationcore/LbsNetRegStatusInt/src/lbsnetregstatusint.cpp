/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "lbsnetregstatusint.h"
#include "lbsrootcenrepdefs.h"
#include "lbspropertykeydefs.h"
#include <centralrepository.h>

//
// Network Registration Status
//

EXPORT_C void RLbsNetworkRegistrationStatus::InitializeL()
	{
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	
	TInt err = RProperty::Define(KNetworkRegistrationStatusKey, 
								 RProperty::EInt, 
								 readPolicy, 
								 writePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
										 
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
									  KNetworkRegistrationStatusKey, 
									  ENotRegistered));
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	SetCategoryUidL(KNetworkRegistrationStatusCategoryKey, 
	   			    RProcess().SecureId());	
	}
	
EXPORT_C void RLbsNetworkRegistrationStatus::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KNetworkRegistrationStatusKey));
	}

EXPORT_C void RLbsNetworkRegistrationStatus::OpenL()
	{
	__ASSERT_DEBUG(iNetworkRegistrationStatusProperty.Handle() == NULL, 
				   User::Invariant());
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	GetCategoryUidL(KNetworkRegistrationStatusCategoryKey, 
	  			    categoryUid);	
	
	User::LeaveIfError(iNetworkRegistrationStatusProperty.Attach(categoryUid, 
													 KNetworkRegistrationStatusKey));
	}
	
EXPORT_C void RLbsNetworkRegistrationStatus::Close()
	{
	iNetworkRegistrationStatusProperty.Close();
	}

EXPORT_C TInt RLbsNetworkRegistrationStatus::GetNetworkRegistrationStatus(TLbsNetworkRegistrationStatus& aStatus)
	{
	__ASSERT_DEBUG(iNetworkRegistrationStatusProperty.Handle() != NULL, User::Invariant());

	return iNetworkRegistrationStatusProperty.Get(reinterpret_cast<TInt&>(aStatus));
	}
	
EXPORT_C TInt RLbsNetworkRegistrationStatus::SetNetworkRegistrationStatus(TLbsNetworkRegistrationStatus aStatus)
	{
	__ASSERT_DEBUG(iNetworkRegistrationStatusProperty.Handle() != NULL, User::Invariant());

	return iNetworkRegistrationStatusProperty.Set(aStatus);
	}
	
	
EXPORT_C void RLbsNetworkRegistrationStatus::NotifyNetworkRegistrationStatusUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iNetworkRegistrationStatusProperty.Handle() != NULL, User::Invariant());
	iNetworkRegistrationStatusProperty.Subscribe(aStatus);	
	}

EXPORT_C void RLbsNetworkRegistrationStatus::CancelNotifyNetworkRegistrationStatusUpdate()
	{
	__ASSERT_DEBUG(iNetworkRegistrationStatusProperty.Handle() != NULL, User::Invariant());
	iNetworkRegistrationStatusProperty.Cancel();	
	}	

void RLbsNetworkRegistrationStatus::GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid)
	{
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(aKey, category));
	aCategoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);
	}
	
void RLbsNetworkRegistrationStatus::SetCategoryUidL(TUint32 aKey, TUid aCategoryUid)
	{
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(aKey, TInt(aCategoryUid.iUid)));
	CleanupStack::PopAndDestroy(rep);
	}

