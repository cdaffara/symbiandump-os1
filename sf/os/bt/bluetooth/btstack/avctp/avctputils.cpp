// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include "avctputils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

namespace SymbianAvctp
{

/**
Panic AVCTP due to stack bug.

  @internalComponent
*/
void Panic(TPanic aPanic)
	{
	LOG_STATIC_FUNC
	User::Panic(KAvctpPanicName, aPanic);
	}
	
void Panic(TPanic aPanic, TInt aState)
	{
	LOG_STATIC_FUNC
	User::Panic(KAvctpPanicName, (aPanic*KStatePanicMultiplier) + KStatePanicDelimiter + aState);
	}
	
} // end of namespace SymbianAvctp
