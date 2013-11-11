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
// lbssuplpushimplprops.h
// A class for the defining of the Lbs Supl Push Publish and Subscribe properties 
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef SUPL_PUSH_PROPS_H
#define SUPL_PUSH_PROPS_H


#include <e32base.h>
#include "lbspropertykeydefs.h"


/** The key for the SMS BUSY property*/
const TUint  KLbsSuplPushSmsBusyKey = KLbsSuplPushAPIBase;
/** The key for the SMS INIT property*/
const TUint  KLbsSuplPushSmsInitKey = KLbsSuplPushAPIBase+1;
/** The key for the SMS ACK property*/
const TUint  KLbsSuplPushSmsAckKey = KLbsSuplPushAPIBase+2;
/** The key for the WAP Push BUSY property*/
const TUint  KLbsSuplPushWapBusyKey = KLbsSuplPushAPIBase+3;
/** The key for the WAP Push INIT property*/
const TUint  KLbsSuplPushWapInitKey = KLbsSuplPushAPIBase+4;
/** The key for the WAP Push ACK property*/
const TUint  KLbsSuplPushWapAckKey = KLbsSuplPushAPIBase+5;


/**
The LbsSuplPushProps class provides the InitializeL function that allows the process-owner of 
the SUPL Push API RProperties to define them.

@internalTechnology
@released

@see CLbsSuplPush
*/
NONSHARABLE_CLASS(LbsSuplPushProps)
	{
public:
	IMPORT_C static void InitializeL();
	};

#endif //SUPL_PUSH_PROPS_H
