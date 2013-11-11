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

#ifndef __WSLISTEN_H__
#define __WSLISTEN_H__

#include <w32std.h>

const TUint8 KListenerInfoSig = 0xaa;

NONSHARABLE_STRUCT(TListenerInfo)
	{
	TUint8 iSignature;
	TInt iNumRect;
	TRect iRect;
	TInt iWindowGroupId;
	};

NONSHARABLE_CLASS(CWsListen): public CWsGraphic
	{
public:		
	IMPORT_C static CWsListen* NewL();
	IMPORT_C static CWsListen* NewL(TBool aDisableWin);	
	IMPORT_C ~CWsListen();
	IMPORT_C TInt Enable(TBool aEnabled);
	IMPORT_C void SetCallBack(TCallBack aCallBack);
	IMPORT_C TInt QueryPlugin(TListenerInfo& aInfo);
	IMPORT_C void SetTestFlag();
private:
	// override CWsGraphic
	virtual void HandleMessage(const TDesC8& aData);
	virtual void OnReplace();
private:
	TBool iIsReady;
	TCallBack iCallBack;
	TListenerInfo* iReq;
	TBool iTestFlag;
	};

NONSHARABLE_CLASS(CWsNotify): public CWsGraphic
	{
public:		
	IMPORT_C static CWsNotify* NewL();
	IMPORT_C static CWsNotify* NewL(TBool aPluginOrder);	
	IMPORT_C ~CWsNotify();
	IMPORT_C TInt SetBehaviour(TInt aType);
private:
	// override CWsGraphic
	virtual void HandleMessage(const TDesC8& aData);
	virtual void OnReplace();
private:
	TBool iIsReady;
public:
	TBool iResult;
	TBuf16<100> iError; 
	};

#endif
