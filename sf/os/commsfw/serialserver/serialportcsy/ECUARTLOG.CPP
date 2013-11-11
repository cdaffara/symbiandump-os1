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

#include <e32std.h>
#include "ECUARTLOG.H"

#if (defined __FLOG_ACTIVE)

_LIT8(KLitEBadUnit,              "EBadUnit");
_LIT8(KLitECancelNotOutstanding, "ECancelNotOutstanding");
_LIT8(KLitEWrongRole,            "EWrongRole");
_LIT8(KLitENegativeCount,        "ENegativeCount");
_LIT8(KLitFaultUnspecified,      "**FaultUnspecified**");

/**
Returns a string for a TCommFault reason

@param aFault TCommFault reason
@return String name representation of aFault
*/
const TDesC8& TECUARTLOG::CommFaultStr(const TCommFault& aFault)
	{
	switch(aFault)
		{
	case EBadUnit:
		return KLitEBadUnit;
	case ECancelNotOutstanding:
		return KLitECancelNotOutstanding;
	case EWrongRole:
		return KLitEWrongRole;
	case ENegativeCount:
		return KLitENegativeCount;
	default:
		return KLitFaultUnspecified;
		}
	}
	
#endif // __FLOG_ACTIVE




