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

#ifndef RMOCKHWRMPLUGIN_H
#define RMOCKHWRMPLUGIN_H

#include <rmocksy.h>

/**
Specific MockSY API for HWRM service plugin
*/
class RMockHwrmPlugin : public RMockSY
	{
public:
	IMPORT_C void ExpectProcessCmdL(TInt aCmdId, TUint8 aTransId, const TDesC8& aData, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);
	IMPORT_C void ExpectProcessCmdL(TInt aCmdId, const TDesC8& aData, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);

	IMPORT_C void ExpectCancelCmdL(TInt aCmdId, TUint8 aTransId, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);
	IMPORT_C void ExpectCancelCmdL(TInt aCmdId, TInt aErrorCode = KErrNone, TBool aLeave = EFalse);

	IMPORT_C void CompleteL(TInt aCmdId, TUint8 aTransId, TInt aDelay, const TDesC8& aData);
	IMPORT_C void CompleteL(TInt aCmdId, TInt aDelay, const TDesC8& aData);
	};

#endif
