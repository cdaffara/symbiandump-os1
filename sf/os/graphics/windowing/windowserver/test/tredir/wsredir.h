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

#ifndef __WSREDIR_H__
#define __WSREDIR_H__

#include <w32std.h>

const TUint8 KRedirectorInfoSig = 0x7e;

NONSHARABLE_STRUCT(TRedirectorInfo)
	{
	TUint8 iSignature;
	TAny* iFrontBufferInterface;
	TAny* iBackBufferInterface;
	TAny* iScreenConfigInterface;
	TInt iScreenBitmapHandle;
	TInt iFlickerBitmapHandle; 
	TInt iUpdateCounter;
	};

NONSHARABLE_CLASS(CWsRedir): public CWsGraphic
	{
public:
	enum TBufferType
		{
		EFrontBuffer,
		EBackBuffer
		};
public:		
	IMPORT_C static CWsRedir* NewL();
	IMPORT_C static CWsRedir* NewL(TInt aScreenId);
	IMPORT_C static CWsRedir* NewL(TInt aScreenId, TBool aDisableWin);	
	IMPORT_C ~CWsRedir();
	IMPORT_C TInt Redirect(TBufferType aWhich, TBool aHow);
	IMPORT_C TInt RedirectUsingWsBackBuffer(TBool aHow);
	IMPORT_C TInt QueryPlugin(TRedirectorInfo& aInfo);
	IMPORT_C void SetCallBack(TCallBack aCallBack);
private:
	// override CWsGraphic
	virtual void HandleMessage(const TDesC8& aData);
	virtual void OnReplace();
private:
	TBool iIsFrontRedirected;
	TBool iIsBackRedirected;
	TBool iIsReady;
	TRedirectorInfo* iReq;
	TCallBack iCallBack;
	};

#endif
