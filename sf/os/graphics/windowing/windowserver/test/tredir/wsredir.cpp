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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "wsredir.h"

const TUint8 KCmdQuery			= 0;
const TUint8 KCmdSetGcFront 	= 1;
const TUint8 KCmdSetGcBack  	= 2;
const TUint8 KCmdResetGcFront	= 3;
const TUint8 KCmdResetGcBack 	= 4;
const TUint8 KCmdSetBackObject 	= 5;
const TUint8 KCmdResetBackObject= 6;

const TUid KRedirectorInterfaceId = {0x10281e1d};
const TUid KRedirectorImplId = {0x10281e1e};

EXPORT_C CWsRedir* CWsRedir::NewL()
	{
	return NewL(0);
	}

EXPORT_C CWsRedir* CWsRedir::NewL(TInt aScreenId)
	{
	return NewL(aScreenId, EFalse);
	}

EXPORT_C CWsRedir* CWsRedir::NewL(TInt aScreenId, TBool aDisableWin)
	{
	CWsRedir* self = new(ELeave) CWsRedir;
	CleanupStack::PushL(self);
	TBuf8<2> data;
	data.Append((TUint8)aScreenId);
	data.Append((TUint8)aDisableWin);
	self->BaseConstructL(KRedirectorInterfaceId, KRedirectorImplId, data);
	CleanupStack::Pop(self);
	self->iIsReady = ETrue;
	return self;
	}

EXPORT_C CWsRedir::~CWsRedir()
	{
	iIsReady = EFalse;
	}

void CWsRedir::HandleMessage(const TDesC8& aData)
	{
	if (aData.Size()>1 && aData[0]==KRedirectorInfoSig)
		Mem::Copy(iReq, aData.Ptr(), aData.Size());
	iCallBack.CallBack();
	}

void CWsRedir::OnReplace()
	{
	}

EXPORT_C TInt CWsRedir::Redirect(TBufferType aWhich, TBool aHow)
	{
	if (!iIsReady)
		return KErrNotReady;
	
	TBuf8<1> cmd;
	if (aWhich==EFrontBuffer)
		{
		if (iIsFrontRedirected && aHow)
			return KErrArgument;
		iIsFrontRedirected = aHow;
		cmd.Append(iIsFrontRedirected? KCmdSetGcFront : KCmdResetGcFront);
		}
	else
		{
		if (iIsBackRedirected && aHow)
			return KErrArgument;
		iIsBackRedirected = aHow;
		cmd.Append(iIsBackRedirected? KCmdSetGcBack : KCmdResetGcBack);
		}
		
	SendMessage(cmd);
	return Flush();
	}

EXPORT_C TInt CWsRedir::RedirectUsingWsBackBuffer(TBool aHow)
	{
	if (!iIsReady)
		return KErrNotReady;
	
	TBuf8<1> cmd;
	if (iIsBackRedirected && aHow)
		return KErrArgument;
	iIsBackRedirected = aHow;
	cmd.Append(iIsBackRedirected? KCmdSetBackObject : KCmdResetBackObject);
		
	SendMessage(cmd);
	return Flush();
	}
	
EXPORT_C TInt CWsRedir::QueryPlugin(TRedirectorInfo& aInfo)	
	{
	TBuf8<1> cmd;
	cmd.Append(KCmdQuery);
	SendMessage(cmd);
	TInt err = Flush();
	if (err!=KErrNone)
		return err;
	iReq = &aInfo;
	return KErrNone;
	}

EXPORT_C void CWsRedir::SetCallBack(TCallBack aCallBack)
	{
	iCallBack = aCallBack;
	}
