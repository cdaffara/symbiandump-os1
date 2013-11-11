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
 @test
 @internalComponent - Internal Symbian test code
*/

#include "wsratelimitdrawer.h"

//Constant message handle, which is used to send count value between client and plug-in
const TUint8 KCmdCount=0;

/**
Creates new object of type CWsRateLimitGraphic based on existing plug-in id.
This interally constructs CWsGraphic object
@param	TUid	Test plug-in id
*/
EXPORT_C CWsRateLimitGraphic* CWsRateLimitGraphic::NewL(TUid aUid)
	{
	CWsRateLimitGraphic* self = new(ELeave) CWsRateLimitGraphic;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KRateLimitDrawerImplId,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

/**
Creates new object of type CWsRateLimitGraphic based on active screen number.
This interally constructs CWsGraphic object
*/
EXPORT_C CWsRateLimitGraphic* CWsRateLimitGraphic::NewL()
	{
	CWsRateLimitGraphic* self = new(ELeave) CWsRateLimitGraphic;
	CleanupStack::PushL(self);
	self->BaseConstructL(KRateLimitDrawerImplId,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsRateLimitGraphic::~CWsRateLimitGraphic()
	{
	}

CWsRateLimitGraphic::CWsRateLimitGraphic()
	{
	}

void CWsRateLimitGraphic::HandleMessage(const TDesC8& aData)
	{
	if (aData.Size()>1)
		{
		Mem::Copy(iAnimData, aData.Ptr(),(aData.Size()));
		}
	}

void CWsRateLimitGraphic::OnReplace()
	{
	}

/**
Query a message value from test plug-in.
@param	TAnimData	Drawing count for two animations.
*/
EXPORT_C TInt CWsRateLimitGraphic::QueryPluginForFrameRate(TAnimData& aAnimData)
	{
	TBuf8<1> cmd;
	cmd.Append(KCmdCount);
	SendMessage(cmd);
	TInt err = Flush();
	if (err!=KErrNone)
		return err;
	iAnimData=&aAnimData;
	return KErrNone;
	}
