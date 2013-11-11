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
// The plug-in provides the client side CWsGraphic instance CWsListen, which is used in 
// GRAPHICS-WSERV-0023 and GRAPHICS-WSERV-0024. 
// This is also used for Test Case GRAPHICS-WSERV-0058 for INC094118: CWsGraphicDrawer::SendMessage panics window server. 
// The customer incident "INC094118" reports the failure of CWsGraphicDrawer::SendMessage(const TDesC8& aData)
// when the event message iEventMsg is not ready. The defect is reproduced by calling QueryPlugin(aInfo) 
// once again to wserv plug-in side in the CWsListen::HandleMessage(const TDesC8& aData), when the event 
// message has not yet been reset by CWsGraphic::CManager. Certainly in practice there are many other cases
// which could trigger the defect. 
// The fix is added in CWsGraphicMessageQueue::Queue(CMessage* aMessage), where we check whether the iEventMsg 
// is empty or not. As a result, if iEventMsg is not ready we do not signal event but keep message untouched in queue. 
// nce the iEventMsg is set ready, an event is signaleed immediately if there is a waiting event and message 
// will be popped out of the queue.
// The test of defect-fixing INC094118 can be enabled by initialising iTestFlag to ETrue. Setting iTestFlag to
// EFalse will not trigger the defect. A new test case is written to trigger the defect only once.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "wslisten.h"

const TUid KListenerInterfaceId = {0x10281fb6};
const TUid KListenerImplId = {0x10281fb7};
const TUid KNotifierInterfaceId1 = {0x102754c3};
const TUid KNotifierImplId1 = {0x102754c4};
const TUid KNotifierInterfaceId2 = {0x102754c5};
const TUid KNotifierImplId2 = {0x102754c6};

const TUint8 KCmdDisable 	= 0;
const TUint8 KCmdEnable  	= 1;
const TUint8 KCmdQuery		= 2;	

EXPORT_C CWsListen* CWsListen::NewL()
	{
	return NewL(EFalse);
	}

EXPORT_C CWsListen* CWsListen::NewL(TBool aDisableWin)
	{
	CWsListen* self = new(ELeave) CWsListen;
	CleanupStack::PushL(self);
	TBuf8<1> data;
	data.Append((TUint8)aDisableWin);
	self->BaseConstructL(KListenerInterfaceId, KListenerImplId, data);
	CleanupStack::Pop(self);
	//Initialise the test flag to EFalse if dont want to reproduce the INC094118 and test the fix
	self->iTestFlag = EFalse;
	self->iIsReady = ETrue;
	return self;
	}

EXPORT_C CWsListen::~CWsListen()
	{
	iIsReady = EFalse;
	}

EXPORT_C void CWsListen::SetTestFlag()
	{
	//Set the test flag to ETrue if want to reproduce the INC094118 and test the fix
	iTestFlag = ETrue;
	}

void CWsListen::HandleMessage(const TDesC8& aData)
	{
	if (aData[0]==KListenerInfoSig)
		Mem::Copy(iReq, aData.Ptr(), aData.Size());
	//The if-statement allows to call QueryPlugin() in HandleMessage() only once to reproduce the defect INC094118.
	//Otherwise HandleMessage() will behave as normal. 
	if (iTestFlag)
		{
	    TListenerInfo aInfo;
		QueryPlugin(aInfo) ;
		iTestFlag = EFalse;
		}
	else
		{
		iCallBack.CallBack();
		}
	}

void CWsListen::OnReplace()
	{
	}

EXPORT_C TInt CWsListen::Enable(TBool aEnabled)
	{
	if (!iIsReady)
		return KErrNotReady;
	
	TBuf8<1> cmd;
	cmd.Append(aEnabled? KCmdEnable : KCmdDisable);
	SendMessage(cmd);
	return Flush();
	}

EXPORT_C TInt CWsListen::QueryPlugin(TListenerInfo& aInfo)
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

EXPORT_C void CWsListen::SetCallBack(TCallBack aCallBack)
	{
	iCallBack = aCallBack;
	}

EXPORT_C CWsNotify* CWsNotify::NewL()
	{
	return NewL(EFalse);
	}

EXPORT_C CWsNotify* CWsNotify::NewL(TBool aPluginOrder)
	{
	CWsNotify* self = new(ELeave) CWsNotify;
	CleanupStack::PushL(self);
	TBuf8<1> data;
	data.Append(8);
	if (!aPluginOrder)				//Different IDs so 2 plugins may be registered at once
		self->BaseConstructL(KNotifierInterfaceId1, KNotifierImplId1, data);
	else
		self->BaseConstructL(KNotifierInterfaceId2, KNotifierImplId2, data);
	CleanupStack::Pop(self);
	self->iIsReady = ETrue;
	self->iResult = ETrue;
	return self;
	}

EXPORT_C CWsNotify::~CWsNotify()
	{
	iIsReady = EFalse;
	}

void CWsNotify::HandleMessage(const TDesC8& aData)
	{
	TBuf8<100> KTesting(_L8("Error:"));
	if(aData.Find(KTesting)!=KErrNotFound)
		{
		iError.Copy(aData);
		iResult = EFalse;
		}
	}

void CWsNotify::OnReplace()
	{
	}
 
EXPORT_C TInt CWsNotify::SetBehaviour(TInt aType)
	{
	if (!iIsReady)
		return KErrNotReady;
	TBuf8<1> cmd;
	cmd.Append(aType);
	SendMessage(cmd);
	return Flush();
	}
