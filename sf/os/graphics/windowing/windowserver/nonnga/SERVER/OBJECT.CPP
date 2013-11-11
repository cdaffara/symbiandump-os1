// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common object code
// 
//

#include "OBJECT.H"
#include "CLIENT.H"

CWsObject::CWsObject(CWsClient* aOwner, WH_HANDLES aType)
	: iType(aType), iWsOwner(aOwner)
	{
	__DECLARE_NAME(_S("CWsObject"));
	}

CWsObject::~CWsObject()
	{
	RemoveFromIndex();
	}

void CWsObject::CloseObject()
	{
	delete this;
	}

void CWsObject::RemoveFromIndex()
	{
	if (iWsOwner)
		{
		iWsOwner->ObjectIndex()->Remove(this);
		iWsOwner=NULL;
		}
	}

void CWsObject::NewObjL()
	{
	if (iWsOwner)
		SetReply(iWsOwner->ObjectIndex()->AddL(this));
	}

TInt CWsObject::LogHandle() const
	{
	return(iWsOwner ? iWsOwner->ObjectHandle(this) : 0);
	}

void CWsObject::SetReply(TInt aReply)
	{
	iWsOwner->SetReply(aReply);
	}

void CWsObject::OwnerPanic(TClientPanic aPanic) const
	{
	if ( iWsOwner )
		{
		iWsOwner->PPanic(aPanic);
		}
	else
		{
		User::Leave(CWsClient::EPanicLeave);
		}
	}

