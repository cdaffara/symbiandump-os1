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
//

#ifndef RMOCKLTSY_H
#define RMOCKLTSY_H

#include <rmocksy.h>

/**
Specific MockSY API for LTSY plugin
*/

_LIT(KMockLtsyServerName,"MockLSTY");

class RMockLtsy : public RMockSY
	{
public:
	IMPORT_C void ExpectL(TInt aIpc, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);
	IMPORT_C void ExpectL(TInt aIpc, const TDesC8& aData, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);
	IMPORT_C void CompleteL(TInt aIpc, TInt aResultCode, TInt aDelay = 1);
	IMPORT_C void CompleteL(TInt aIpc, TInt aResultCode, const TDesC8& aData, TInt aDelay = 1);

protected:
	IMPORT_C virtual const TDesC& ServerName();
	};
#endif
