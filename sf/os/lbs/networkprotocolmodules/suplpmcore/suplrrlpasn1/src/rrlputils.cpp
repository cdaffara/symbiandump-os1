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
 @internalTechnology
 
*/
#include <e32base.h>
#include "rrlputils.h"
#include "supldevloggermacros.h" 
#include "rtxErrCodes.h"

/**
Translates error codes returned by the ASN1 runtime library to distinguish
from Symbian global error codes.

Errors are simply translated to positive error codes. They maintain their
meaning as described in rtxErrCodes.h and asn1ErrCodes.h.

Exceptions:
  RTERR_NOMEM is translated to global error code KErrNoMemory

@see rtxErrCodes.h
@see asn1ErrCodes.h
*/
TInt RrlpUtils::ProcessAsn1Error(TInt aError)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() Begin\n");
	if (aError == RTERR_NOMEM)
		{
		SUPLLOG(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() End (Out Of Memory)\n");
		return KErrNoMemory;
		}
	else
		{
		SUPLLOG2(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() End (ASN1Error runtime error %d)\n", aError);
		return aError * -1;
		}
	}


