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
 @file
 @internalComponent
*/

#include <e32base.h>
#include <remconaddress.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_TYPES);
#endif

EXPORT_C TRemConAddress::TRemConAddress()
	{
	LOG_FUNC
	iBearerUid = KNullUid;
	iAddr.Zero();
	}

EXPORT_C TRemConAddress::~TRemConAddress()
	{
	LOG_FUNC
	}

EXPORT_C TBool TRemConAddress::IsNull() const
	{
	return ( iBearerUid == KNullUid );
	}

EXPORT_C TUid& TRemConAddress::BearerUid()
	{
	return iBearerUid;
	}

EXPORT_C TUid TRemConAddress::BearerUid() const
	{
	return iBearerUid;
	}

EXPORT_C TBuf8<TRemConAddress::KMaxAddrSize>& TRemConAddress::Addr()
	{
	return iAddr;
	}

EXPORT_C const TBuf8<TRemConAddress::KMaxAddrSize>& TRemConAddress::Addr() const
	{
	return iAddr;
	}
	
EXPORT_C TBool TRemConAddress::operator==(const TRemConAddress& aRhs) const
	{
	if ( iBearerUid == aRhs.iBearerUid && iAddr == aRhs.iAddr )
		{
		return ETrue;
		}
	return EFalse;
	}
