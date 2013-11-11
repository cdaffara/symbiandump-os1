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
// nm_common.h
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_NM_COMMON_INTERNAL_H
#define SYMBIAN_NM_COMMON_INTERNAL_H

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessCmiH, "ElemNodeMessCmiH");
#endif


namespace Messages
{
class MTransportSender;
class MTransportReceiver;
class TNodeId;


class CGlobalsInternal : public CBase
	{
	friend class CGlobals;

public:
	MTransportSender& TransportSender()
		{
		__ASSERT_DEBUG(iSender, User::Panic(KSpecAssert_ElemNodeMessCmiH, 1));
		return *iSender;
		}

	MTransportReceiver& TransportReceiver()
		{
		__ASSERT_DEBUG(iReceiver, User::Panic(KSpecAssert_ElemNodeMessCmiH, 2));
		return *iReceiver;
		}

	void SetTransportSender(MTransportSender* aTransport)
		{
		__ASSERT_DEBUG(!(aTransport != NULL && iSender != NULL), User::Panic(KSpecAssert_ElemNodeMessCmiH, 3));
		iSender = aTransport;
		}

	void SetTransportReceiver(MTransportReceiver* aTransport)
		{
		__ASSERT_DEBUG(!(aTransport != NULL && iReceiver != NULL), User::Panic(KSpecAssert_ElemNodeMessCmiH, 4));
		iReceiver = aTransport;
		}

	Meta::CMetaDataVirtualCtorInPlace* VirtualCtor()
		{
		return iVirtCtor;
		}

	TInt PersistentItfCount() const
		{
		return iPersistentItfCount;
		}

	IMPORT_C void AddPersistentItf(const Messages::TNodeId& aNodeId);

private:
	IMPORT_C explicit CGlobalsInternal();
	IMPORT_C ~CGlobalsInternal();
	IMPORT_C void ConstructL();

private:
	Meta::CMetaDataVirtualCtorInPlace* iVirtCtor;
	MTransportSender* iSender;
	MTransportReceiver* iReceiver;
	TInt iPersistentItfCount;
	};
} //namespace Messages


#endif
//SYMBIAN_NM_COMMON_H

