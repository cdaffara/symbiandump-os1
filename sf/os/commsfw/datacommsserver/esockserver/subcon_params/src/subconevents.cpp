// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file subconevents.cpp
*/

#include <e32std.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <cs_subconevents.h>

#include "cs_subconeventsfactory.h"

// private event
const TUint32 KSubConGenericEventDataClientBase =  0x3;

START_ATTRIBUTE_TABLE( CSubConGenEventParamsGranted, KSubConnGenericEventsImplUid, KSubConGenericEventParamsGranted )
	REGISTER_ATTRIBUTE( CSubConGenEventParamsGranted, iFamily, TMetaNumber)
	REGISTER_ATTRIBUTE( CSubConGenEventParamsGranted, iGenericSet, TMetaPtr<CSubConGenericParameterSet>)
	REGISTER_ATTRIBUTE( CSubConGenEventParamsGranted, iExtensionSets, TMetaObject<RMetaDataEComContainer>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConGenEventParamsChanged, KSubConnGenericEventsImplUid, KSubConGenericEventParamsChanged )
	REGISTER_ATTRIBUTE( CSubConGenEventParamsChanged, iError, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CSubConGenEventParamsGranted, 0)

START_ATTRIBUTE_TABLE( CSubConGenEventParamsRejected, KSubConnGenericEventsImplUid, KSubConGenericEventParamsRejected )
	REGISTER_ATTRIBUTE( CSubConGenEventParamsRejected, iError, TMetaNumber)
	REGISTER_ATTRIBUTE ( CSubConGenEventParamsRejected, iFamilyId, TMetaNumber)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConGenEventDataClientBase, KSubConnGenericEventsImplUid, KSubConGenericEventDataClientBase)
	REGISTER_ATTRIBUTE( CSubConGenEventDataClientBase, iIap, TMetaNumber)
	REGISTER_ATTRIBUTE( CSubConGenEventDataClientBase, iSourceAddress, TMeta<TSockAddr>)
	REGISTER_ATTRIBUTE( CSubConGenEventDataClientBase, iDestAddress, TMeta<TSockAddr>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConGenEventDataClientJoined, KSubConnGenericEventsImplUid, KSubConGenericEventDataClientJoined)
END_ATTRIBUTE_TABLE_BASE(CSubConGenEventDataClientBase, 0)

START_ATTRIBUTE_TABLE( CSubConGenEventDataClientLeft, KSubConnGenericEventsImplUid, KSubConGenericEventDataClientLeft)
END_ATTRIBUTE_TABLE_BASE(CSubConGenEventDataClientBase, 0)

START_ATTRIBUTE_TABLE( CSubConGenEventSubConDown, KSubConnGenericEventsImplUid, KSubConGenericEventSubConDown)
	REGISTER_ATTRIBUTE( CSubConGenEventSubConDown, iError, TMetaNumber)
END_ATTRIBUTE_TABLE()

/**
Plugin Implementation
*/
CSubConNotificationEvent* CSubConGenEventsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (KSubConGenericEventParamsGranted):
		return new (ELeave) CSubConGenEventParamsGranted;
	case (KSubConGenericEventDataClientJoined):
		return new (ELeave) CSubConGenEventDataClientJoined;
	case (KSubConGenericEventDataClientLeft):
		return new (ELeave) CSubConGenEventDataClientLeft;
	case (KSubConGenericEventSubConDown):
		return new (ELeave) CSubConGenEventSubConDown;
	case (KSubConGenericEventParamsChanged):
		return new (ELeave) CSubConGenEventParamsChanged;
	case (KSubConGenericEventParamsRejected):
		return new (ELeave) CSubConGenEventParamsRejected;		
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}

CSubConGenEventParamsGranted::~CSubConGenEventParamsGranted()
/** (virtual) sub-connection parameter granted event destructor
*/
	{
	delete iGenericSet;
	iExtensionSets.ResetAndDestroy();
	}

CSubConGenEventParamsChanged::~CSubConGenEventParamsChanged()
/** Empty (virtual) sub-connection parameter changed event destructor
*/
	{
	}

CSubConGenEventParamsRejected::~CSubConGenEventParamsRejected()
/** Empty (virtual) sub-connection parameter rejected event destructor
*/
	{
	}

CSubConGenEventDataClientBase::~CSubConGenEventDataClientBase()
/** Empty (virtual) sub-connection data client base destructor
*/
	{
	}

CSubConGenEventDataClientJoined::~CSubConGenEventDataClientJoined()
/** Empty (virtual) sub-connection data client joined event destructor
*/
	{
	}

CSubConGenEventDataClientLeft::~CSubConGenEventDataClientLeft()
/** Empty (virtual) sub-connection data client left event destructor
*/
	{
	}

CSubConGenEventSubConDown::~CSubConGenEventSubConDown()
/** Empty (virtual) sub-connection down event destructor
*/
	{
	}


