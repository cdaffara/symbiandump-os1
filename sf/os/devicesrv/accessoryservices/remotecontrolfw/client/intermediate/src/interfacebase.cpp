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

#include <bluetooth/logger.h>
#include <remcon/remconinterfacebase.h>
#include <remconinterfaceselector.h>
#include "interfacebaseextension.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_IF_BASE);
#endif

EXPORT_C CRemConInterfaceBase::~CRemConInterfaceBase()
	{
	LOG_FUNC	
	
	delete iExtension;
	}

EXPORT_C CRemConInterfaceBase::CRemConInterfaceBase(TUid aInterfaceUid, 
													TUint aMaxLength,
													CRemConInterfaceSelector& aInterfaceSelector,
													TRemConClientType aType)
 :	iInterfaceUid(aInterfaceUid),
	iMaxLength(aMaxLength),
	iType(aType),
	iInterfaceSelector(aInterfaceSelector)
	{
	LOG_FUNC
	
	if ( aType != ERemConClientTypeController && aType != ERemConClientTypeTarget )
		{
		LOG(_L("Panicking with RemConIfSel / ERemConIfSelUndefinedClientType"));
		User::Panic(KRemConIfSelPanicCat, ERemConIfSelUndefinedClientType);
		}
	}

EXPORT_C void CRemConInterfaceBase::BaseConstructL()
	{
	iInterfaceSelector.RegisterInterfaceL(*this);
	}

EXPORT_C void CRemConInterfaceBase::BaseConstructL(RRemConInterfaceFeatures& aRemConInterfaceFeatures)
	{
	iInterfaceSelector.RegisterInterfaceL(*this, aRemConInterfaceFeatures);
	}

EXPORT_C void CRemConInterfaceBase::BaseConstructL(RRemConInterfaceFeatures& aRemConInterfaceFeatures, TBool aBulkInterface)
	{
	iExtension = new(ELeave) CRemConInterfaceBaseExtension();
	iExtension->SetBulk(aBulkInterface);
	BaseConstructL(aRemConInterfaceFeatures);
	}

EXPORT_C CRemConInterfaceSelector& CRemConInterfaceBase::InterfaceSelector()
	{
	return iInterfaceSelector;
	}

EXPORT_C TInt CRemConInterfaceBase::Cancel()
	{
	TRemConMessageType msgType = ERemConResponse;
	if ( iType == ERemConClientTypeController )
		{
		msgType = ERemConCommand;
		}
		
	//Ignore Return code because
	// a) It'll mostly be other than KErrNone because the server has terminated, in which
	//    case the original async request will have completed with the error anyway!
	// b) It's meaningless to the client whatever the return code is.
	(void)InterfaceSelector().SendCancel(msgType);
	
	return KErrNone;
	}

TUid CRemConInterfaceBase::InterfaceUid() const
	{
	return iInterfaceUid;
	}

TUint CRemConInterfaceBase::MaxLength() const
	{
	return iMaxLength;
	}

TRemConClientType CRemConInterfaceBase::Type() const
	{
	return iType;
	}

TBool CRemConInterfaceBase::Bulk() const
	{
	return iExtension && iExtension->Bulk();
	}

TBool CRemConInterfaceBase::Target(TRemConClientType aType)
	{
	return aType == ERemConClientTypeTarget;
	}

