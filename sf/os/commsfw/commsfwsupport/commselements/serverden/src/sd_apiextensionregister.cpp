// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SS_APIEXT_REGISTER.CPP
// 
//

#include <ecom/ecom.h>
#include "sd_std.h"
#include "sd_apiextension.h"
#include "sd_apiextensionregister.h"
#include <e32base.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenApiXRC, "ElemSvrDenApiXRC");
#endif

using namespace Den;
//using namespace Messages;

TInt CompareApiExtIdentifications(const TApiExtIdentification& aExtItfId1, const TApiExtIdentification& aExtItfId2)
	{
	return Mem::Compare(aExtItfId1.Ptr(), sizeof(TInt)+sizeof(TInt), aExtItfId2.Ptr(), sizeof(TInt)+sizeof(TInt));
	}

TApiExtRegister::~TApiExtRegister()
	{
	//We should not have anything to cleanup here.
	//Every session and subsession must close all
	//remaining extensions from their destructors.
	__ASSERT_DEBUG(iApiExtList.Count()==0, User::Panic(KSpecAssert_ElemSvrDenApiXRC, 1)); //Panic in debug
	iApiExtList.Close();
	}

void TApiExtRegister::CleanupInterfaceRegistration(TAny* aSelf)
	{
	TApiExtRegister* reg = static_cast<TApiExtRegister*>(aSelf);
	__ASSERT_DEBUG(reg->iClenupInfoExtItfId, User::Panic(KSpecAssert_ElemSvrDenApiXRC, 2)); //We have just added..
	TLinearOrder<TApiExtIdentification> order(CompareApiExtIdentifications);
	TInt index = reg->iApiExtList.FindInOrder(*reg->iClenupInfoExtItfId, order);
	__ASSERT_DEBUG(index!=KErrNotFound, User::Panic(KSpecAssert_ElemSvrDenApiXRC, 3)); //We have just added..
	reg->iApiExtList.Remove(index);
	}

void TApiExtRegister::CleanupInterfaceDeregistrationPushL(const TApiExtIdentification& aExtItfId)
	{
	//Never call this fn twice without a Pop() in between
	TLinearOrder<TApiExtIdentification> order(CompareApiExtIdentifications);
	TInt index = iApiExtList.FindInOrder(aExtItfId, order);
	__ASSERT_DEBUG(index!=KErrNotFound, User::Panic(KSpecAssert_ElemSvrDenApiXRC, 4)); //We have just added..
	iClenupInfoExtItfId = &iApiExtList[index];
	CleanupStack::PushL(TCleanupItem(TApiExtRegister::CleanupInterfaceRegistration, this));
	}

void TApiExtRegister::RegisterOpenInterfaceLC(const TApiExtIdentification& aExtItfId)
	{
	//Never call this fn twice without a Pop() in between
	TLinearOrder<TApiExtIdentification> order(CompareApiExtIdentifications);
	TInt index = iApiExtList.FindInOrder(aExtItfId, order);
	User::LeaveIfError(index!=KErrNotFound? KErrAlreadyExists : KErrNone);
	iApiExtList.InsertInOrderL(aExtItfId, order);
	CleanupInterfaceDeregistrationPushL(aExtItfId);
	}

void TApiExtRegister::DeregisterClosedInterface(const TApiExtIdentification& aExtItfId)
	{
	TLinearOrder<TApiExtIdentification> order(CompareApiExtIdentifications);
	TInt index = iApiExtList.FindInOrder(aExtItfId, order);
	__ASSERT_DEBUG(index!=KErrNotFound, User::Panic(KSpecAssert_ElemSvrDenApiXRC, 5)); //UDEB
	if (index!=KErrNotFound)
		{
		iApiExtList.Remove(index);
		}
	}

TBool TApiExtRegister::IsOpened(const TApiExtIdentification& aExtItfId)
	{
	TLinearOrder<TApiExtIdentification> order(CompareApiExtIdentifications);
	return KErrNotFound==iApiExtList.FindInOrder(aExtItfId, order)?
		EFalse : ETrue;
	}

const TApiExtIdentification& TApiExtRegister::operator[](TInt aIndex)
	{
	__ASSERT_DEBUG(aIndex>=0 && aIndex<iApiExtList.Count(), User::Panic(KSpecAssert_ElemSvrDenApiXRC, 6));
	return iApiExtList[aIndex];
	}


