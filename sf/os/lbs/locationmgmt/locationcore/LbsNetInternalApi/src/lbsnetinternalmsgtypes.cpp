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
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32cmn.h>
#include "lbsnetinternalmsgtypes.h"


//
// Base Message Class
//


EXPORT_C TLbsNetInternalMsgBase::TLbsNetInternalMsgType TLbsNetInternalMsgBase::Type() const
	{
	return iType;
	}
	
EXPORT_C const TLbsNetSessionIdInt& TLbsNetInternalMsgBase::SessionId() const
	{
	return iSessionId;
	}
	
EXPORT_C TInt32 TLbsNetInternalMsgBase::Size() const
	{
	return iSize;
	}

EXPORT_C TLbsNetInternalMsgBase& TLbsNetInternalMsgBase::operator= (const TLbsNetInternalMsgBase& aSrc)
	{
	__ASSERT_ALWAYS(Type() == aSrc.Type(), User::Panic(_L("LbsNetInternal"), 2));
	Mem::Copy(this, &aSrc, aSrc.Size());
	return *this;
	}
	
EXPORT_C TLbsNetInternalMsgBase::TLbsNetInternalMsgBase() :
	iType(EUnknown),
	iSize(sizeof(TLbsNetInternalMsgBase))
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsNetInternal"), 1));
	}

EXPORT_C TLbsNetInternalMsgBase::TLbsNetInternalMsgBase(TLbsNetInternalMsgType aType, 
											   TInt aSize):
	iType(aType),
	iSize(aSize)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsNetInternal"), 1));
	}
	
EXPORT_C TLbsNetInternalMsgBase::TLbsNetInternalMsgBase(TLbsNetInternalMsgType aType, 
											   TInt aSize,
											   const TLbsNetSessionIdInt& aSessionId) :
	iType(aType),
	iSize(aSize),
	iSessionId(aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsNetInternal"), 1));
	}
