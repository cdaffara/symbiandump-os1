// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalComponent
*/

#ifndef PSYPANIC_H
#define PSYPANIC_H

#include <e32base.h>

#ifdef _DEBUG

//Panic reason
enum TPanicReason
    {
    EPanicInvalidCase,				// Invalid case in switch clause.
    EPanicProcessDefination,		// Invalid process defination during connection.
    EPanicUnknownPositioningClass,  // Unkown positioning class.
    EPanicSingletonNotConstructed,  // Singleton not constructed.
    EPanicIndexOutOfRange,			// Index out of range.
    EPanicQNotEmpty,				// CPositionerQ destroyed when not empty
    EPanicUnknownUpdateError,		// Error is not in the known range
    EPanicIStatusNotSet,			// iStatus used when it is not set
    EPanicPositionCopyFailed,		// Copy of poition info returned an error
    EPanicUnknownTimerEventId,		// An unknown timer event occurred
    EPanicModeMergeFailed,			// A merge of the positioning modes for existing requests failed
    EPanicNewRequestInactive		// An attempt was made to merge an inactive request
    };
    
//AdaptationPSY panic category
_LIT(KAdaptationPanicCategory, "AdaptationPSY");
    
#endif //_DEBUG

#endif
// End of File

