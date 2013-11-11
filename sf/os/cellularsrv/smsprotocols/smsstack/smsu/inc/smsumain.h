// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMSUMAIN_H__
#define __SMSUMAIN_H__

#include <e32base.h>

/**
 *  @internalComponent
 */
enum TSmsuPanic
	{
//  SMSUADDR.CPP
	ESmsuPanicAddrFamilyNotAllowed,
	ESmsuPanicAddrTextTooLong,
	ESmsuPanicWrongSmsAddressFamily,
//  SMSUSTRM.CPP
	KSsmuPanicStreamReadUnavailable,
	KSsmuPanicStreamWriteUnavailable,
//  SMSULOG.CPP
	KSmsuPanicUnexpectedState,
	ESmsuUnexpectedSmsPDUType,
	KSmsuPanicWrongLogServerId,
	ESmsuAlreadyActive,
	ESmsuTimeoutNull,
	ESmsuCompleteMyselfAlreadyRequested
	};

/**
 *  @internalComponent
 */
GLDEF_C void SmsuPanic(TSmsuPanic aPanic);

#endif
