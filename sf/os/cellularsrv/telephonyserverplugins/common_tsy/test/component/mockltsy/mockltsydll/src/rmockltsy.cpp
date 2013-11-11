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

#include "rmockltsy.h"


EXPORT_C void RMockLtsy::ExpectL(TInt aIpc,TInt aErrorCode, TBool aLeave)
	{
	DoExpect(aIpc,aErrorCode,aLeave,KNullDesC8);
	}

EXPORT_C void RMockLtsy::ExpectL(TInt aIpc, const TDesC8& aData, TInt aErrorCode, TBool aLeave)
	{
	DoExpect(aIpc,aErrorCode,aLeave,aData);
	}
	
EXPORT_C void RMockLtsy::CompleteL(TInt aIpc, TInt aResultCode, TInt aDelay)
	{
	DoComplete(aIpc,aResultCode,aDelay,KNullDesC8);
	}
	
EXPORT_C void RMockLtsy::CompleteL(TInt aIpc, TInt aResultCode, const TDesC8& aData, TInt aDelay)
	{
	DoComplete(aIpc,aResultCode,aDelay,aData);
	}
	

EXPORT_C const TDesC& RMockLtsy::ServerName()
	{
	return KMockLtsyServerName;
	}
