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

#include "rmockhwrmplugin.h"
#include "tmockhwrmplugindata.h"

EXPORT_C void RMockHwrmPlugin::ExpectProcessCmdL(TInt aCmdId, const TDesC8& aData, TInt aErrorCode, TBool aLeave)
	{
	TMockHwrmPluginData data(aData);
	RBuf8 buffer;
	data.SerialiseL(buffer);
	DoExpect(aCmdId ,aErrorCode, aLeave, buffer);
	buffer.Close();
	}

EXPORT_C void RMockHwrmPlugin::ExpectProcessCmdL(TInt aCmdId, TUint8 aTransId, const TDesC8& aData, TInt aErrorCode, TBool aLeave)
	{
	TMockHwrmPluginData data(aTransId,aData);
	RBuf8 buffer;
	data.SerialiseL(buffer);
	DoExpect(aCmdId ,aErrorCode, aLeave, buffer);
	buffer.Close();
	}
	
EXPORT_C void RMockHwrmPlugin::ExpectCancelCmdL(TInt aCmdId, TInt aErrorCode, TBool aLeave)
	{
	TMockHwrmPluginData data;
	RBuf8 buffer;
	data.SerialiseL(buffer);
	// Uses negative cmdID for cancel
	DoExpect(-aCmdId ,aErrorCode, aLeave, buffer);
	buffer.Close();
	}

EXPORT_C void RMockHwrmPlugin::ExpectCancelCmdL(TInt aCmdId, TUint8 aTransId, TInt aErrorCode, TBool aLeave)
	{
	TMockHwrmPluginData data(aTransId);
	RBuf8 buffer;
	data.SerialiseL(buffer);
	// Uses negative cmdID for cancel
	DoExpect(-aCmdId ,aErrorCode, aLeave, buffer);
	buffer.Close();
	}
	
EXPORT_C void RMockHwrmPlugin::CompleteL(TInt aCmdId, TInt aDelay, const TDesC8& aData)
	{
	TMockHwrmPluginData data(aData);
	RBuf8 buffer;
	data.SerialiseL(buffer);
	DoComplete(aCmdId, KErrNone, aDelay, buffer);
	buffer.Close();
	}

EXPORT_C void RMockHwrmPlugin::CompleteL(TInt aCmdId, TUint8 aTransId, TInt aDelay, const TDesC8& aData)
	{
	TMockHwrmPluginData data(aTransId,aData);
	RBuf8 buffer;
	data.SerialiseL(buffer);
	DoComplete(aCmdId, KErrNone, aDelay, buffer);
	buffer.Close();
	}
