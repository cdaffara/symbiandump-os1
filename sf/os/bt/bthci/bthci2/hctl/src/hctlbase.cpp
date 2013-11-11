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
 @internalComponent
*/

#include "hctlbase.h"
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/logger.h>
#include <bluetooth/hci/hctlbase.hrh>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_BASE);
#endif

/** 
@param aImplementationUid is used by ECom to load the implementation.
@return Ownership of a new HCTL plugin.
*/
EXPORT_C CHCTLBase* CHCTLBase::NewL(TUid aImplementationUid)
	{
	LOG_STATIC_FUNC

	CHCTLBase* self = reinterpret_cast<CHCTLBase*> (CHciUtil::LoadImplementationL(TUid::Uid(KHCTLBaseEcomInterfaceUid), 
																				  aImplementationUid, 
													                              _FOFF(CHCTLBase, iInstanceId)));	
	return self;
	}

/** 
@return Ownership of a new HCTL plugin.
*/
EXPORT_C CHCTLBase* CHCTLBase::NewL()
	{
	LOG_STATIC_FUNC

	CHCTLBase* self = reinterpret_cast<CHCTLBase*> (CHciUtil::LoadImplementationL(TUid::Uid(KHCTLBaseEcomInterfaceUid), 
									  											  _FOFF(CHCTLBase, iInstanceId)));	
	return self;
	}

EXPORT_C CHCTLBase::~CHCTLBase()
    {
	LOG_FUNC

 	CHciUtil::DestroyedImplementation(iInstanceId);
 	CLOSE_LOGGER
    }

EXPORT_C CHCTLBase::CHCTLBase()
    {
	CONNECT_LOGGER
	LOG_FUNC
    }
