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

#include <badesca.h>
#include "RpsSFlogger.h"
#include "rpscommon.h"
#include "RespBuilder.h"
#include "rpsasciirqstdefs.h"
#include <e32debug.h>
	
CRespBuilder* CRespBuilder::NewL()
	{
	CRespBuilder* self = new(ELeave)CRespBuilder();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CRespBuilder::~CRespBuilder()
	{
	}
	
void CRespBuilder::ConstructL()
	{
	}
	
//
// BuildResponseL
// Builds an appropriate reply msg based on the error code.
// aErrorCode - the error code from parsing the reqst msg
// aRespBuff  - an uninstantiated buffer - caller assumes ownership
// aRespArgArray   - data to be sent as part of the response.
//	
void CRespBuilder::BuildResponseL(TInt aErrorCode, TUint aMsgNum, HBufC8*& aRespBuff, CDesCArrayFlat* aRespArgArray)
	{
	LOGLINE1(_L(">>CRespBuilder::BuildResponseL"));
	
	// Using CRpsMsg to build the response
	
	// Create new (build independent) argument array from the argument array passed in
	CDesCArrayFlat* argArray = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(argArray);
	
	for(TInt a=0; a < aRespArgArray->Count(); ++a)
		{
		if(aRespArgArray->MdcaPoint(a).Length() > 0)
			{
			
			TBuf<KMaxElementSize> tempBuf;  // Use build independent arg value
			tempBuf.Copy(aRespArgArray->MdcaPoint(a));
			
			argArray->AppendL(tempBuf);
			}
		}

	CRpsMsg* respMsg = CRpsMsg::NewLC(KMsgTypeResp, ErrorCodeAscii(aErrorCode), aMsgNum, *argArray);
	
	respMsg->ExternalizeL(aRespBuff);

	CleanupStack::PopAndDestroy(respMsg);	
	CleanupStack::PopAndDestroy(argArray);
	LOGLINE1(_L("<<CRespBuilder::BuildResponseL"));
	}
	
	
const TDesC8& CRespBuilder::ErrorCodeAscii(TInt aErrorCode)
	{
	if(aErrorCode == KErrNone)
		{
		return KOk;
		}
	else if(aErrorCode == KErrCorrupt)
		{
		return KErrCorruptMsg;
		}
	else
		{
		return KErrUnknownMsg;
		}
	}
