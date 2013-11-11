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
// clesessionproxy.cpp
// 
//

#include "clesessionproxy.h"
#include "ssmserverpanic.h"
#include <ssm/ssmcmd.hrh>

CCleSessionProxy* CCleSessionProxy::NewL()
	{
	CCleSessionProxy* self = new (ELeave) CCleSessionProxy;
	return self;
	}

void CCleSessionProxy::ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity)
	{
	iCleSession.ExecuteCommandList(aCmdList, aStatus, aSeverity);
	}

void CCleSessionProxy::ExecuteCommandListCancel()
	{
	iCleSession.ExecuteCommandListCancel();
	}

_LIT(KCleTestServerName, "TestSsmCleServer");

void CCleSessionProxy::ConnectL()
	{
	__ASSERT_DEBUG(!iConnected, PanicNow(KPanicSysStateMgr, ESsmCCleSessionProxy1));
	iCleSession.ConnectL(KCleTestServerName);
	iConnected = ETrue;
	}

void CCleSessionProxy::ReleaseCle()
	{
	delete this;
	}

void CCleSessionProxy::Close()
	{
	iCleSession.Close();
	iConnected = EFalse;
	}

TBool CCleSessionProxy::IsConnected()
	{
	return iConnected;
	}


CCleSessionProxy::~CCleSessionProxy()
	{
	if(iConnected)
		iCleSession.Close();
	}

MCleSessionProxy* CCleSessionProxy::CreateNewProxyL()
	{
	return CCleSessionProxy::NewL();
	}
