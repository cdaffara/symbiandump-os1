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
// Client that calls the plugin that does the actual coverage tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian coverage test code,
*/

#include "wsgdcov.h"

const TUid KGraphicDrawerCoverageInterfaceId = {0x10285D5A};
const TUid KGraphicDrawerCoverageImplId = {0x10285D5B};

EXPORT_C CWsGdCoverage* CWsGdCoverage::NewL()
	{
	return NewL(0);
	}

EXPORT_C CWsGdCoverage* CWsGdCoverage::NewL(TInt aScreenId)
	{
	CWsGdCoverage* self = new(ELeave) CWsGdCoverage;
	CleanupStack::PushL(self);
	TBuf8<1> data;
	data.Append((TUint8)aScreenId);
	self->BaseConstructL(KGraphicDrawerCoverageInterfaceId, KGraphicDrawerCoverageImplId, data);
	CleanupStack::Pop(self);
	self->iIsReady = ETrue;
	return self;
	}

EXPORT_C CWsGdCoverage::~CWsGdCoverage()
	{
	iIsReady = EFalse;
	}

void CWsGdCoverage::HandleMessage(const TDesC8& aData)
	{
	if (aData.Size()>1 && aData[0]==KGdCoverageInfoSig)
		{
		if (aData.Size() <= sizeof(TGdCoverageInfo))
			{
			Mem::Copy(iReq, aData.Ptr(), aData.Size());
			}
		}
	iCallBack.CallBack();
	}

void CWsGdCoverage::OnReplace()
	{
	}

EXPORT_C TInt CWsGdCoverage::RunTest(TInt aCode)
	{
	if (!iIsReady)
		return KErrNotReady;
	
	TBuf8<1> cmd;
	cmd.Append(aCode);
	
	SendMessage(cmd);
	return Flush();
	}

EXPORT_C TInt CWsGdCoverage::QueryPlugin(TGdCoverageInfo& aInfo)
	{
	TBuf8<1> cmd;
	cmd.Append(KGdCoverageCmdQuery);
	SendMessage(cmd);
	TInt err = Flush();
	if (err!=KErrNone)
		return err;
	iReq = &aInfo;
	return KErrNone;
	}

EXPORT_C void CWsGdCoverage::SetCallBack(TCallBack aCallBack)
	{
	iCallBack = aCallBack;
	}
