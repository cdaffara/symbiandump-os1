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

#include <remcon/bearerparams.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

EXPORT_C TBearerParams::TBearerParams(TUid aImplementationUid, MRemConBearerObserver& aObserver)
:	iImplementationUid(aImplementationUid),
	iObserver(aObserver)
	{
	LOG_FUNC
	}

EXPORT_C TBearerParams::~TBearerParams()
	{
	LOG_FUNC
	}

EXPORT_C TUid TBearerParams::ImplementationUid() const
	{
	return iImplementationUid;
	}

EXPORT_C MRemConBearerObserver& TBearerParams::Observer() const
	{
	return iObserver;
	}
