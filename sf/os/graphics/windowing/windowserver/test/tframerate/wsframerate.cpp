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

/**
 @file
 @test - Test client library used to communicate between framerate plug-in and twsgraphs.
 @internalComponent - Internal Symbian test code
*/

#include "wsframerate.h"

//Constant message handle, which is used to send count value between client and plug-in
const TUint8 KCmdCount=0;

LOCAL_C const TUid KUidWsGraphicTestFrameRate = {0x1028292A};

/**
Creates new object of type CGraphicTestFrameRate based on existing plug-in id.
This interally constructs CWsGraphic object

@param	TUid	Test plug-in id
*/

EXPORT_C CGraphicTestFrameRate* CGraphicTestFrameRate::NewL(TUid aUid)
	{
	CGraphicTestFrameRate* self = new(ELeave) CGraphicTestFrameRate;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KUidWsGraphicTestFrameRate,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

/**
Creates new object of type CGraphicTestFrameRate based on active screen number.
This interally constructs CWsGraphic object

@param	TInt	Screen number
*/
EXPORT_C CGraphicTestFrameRate* CGraphicTestFrameRate::NewL(TInt aScreen)
	{
	CGraphicTestFrameRate* self = new(ELeave) CGraphicTestFrameRate;
	CleanupStack::PushL(self);
	TBuf8<1> data;
	data.Append((TUint8)aScreen);
	self->BaseConstructL(KUidWsGraphicTestFrameRate,data);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CGraphicTestFrameRate::~CGraphicTestFrameRate()
	{
	}

CGraphicTestFrameRate::CGraphicTestFrameRate()
	{
	}

/**
Handles message between client and Twsgraphs test call.

@param	TDesc8	Descriptor values passed between client and plugin
*/
void CGraphicTestFrameRate::HandleMessage(const TDesC8& aData)
	{
	if (aData.Size()>1)
		{
		Mem::Copy(iCount, aData.Ptr(),(aData.Size()));
		}
	iCallBack.CallBack();
	}

void CGraphicTestFrameRate::OnReplace()
	{
	}

/**
Query a message value from test plug-in.

@param	TAnimRate	Drawing count for two animations.
*/
EXPORT_C TInt CGraphicTestFrameRate::QueryPlugin(TAnimRate& aCount)
	{
	TBuf8<1> cmd;
	cmd.Append(KCmdCount);
	SendMessage(cmd);
	TInt err = Flush();
	if (err!=KErrNone)
		return err;
	iCount=&aCount;
	return KErrNone;
	}

/**
Set the callback function

@param	TCallBack	Name of the callback function to invoke.
*/
EXPORT_C void CGraphicTestFrameRate::SetCallBack(TCallBack aCallBack)
	{
	iCallBack = aCallBack;
	}
