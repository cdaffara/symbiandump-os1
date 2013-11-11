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

#ifndef __CLESESSIONPROXY_H__
#define __CLESESSIONPROXY_H__

#include <e32base.h>
#include "clecli.h"
#include "ssmserverpanic.h"
#include <ssm/ssmcmd.hrh>

class TRequestStatus;
class CSsmCommandList;

class MCleSessionProxy
	{
public:
	virtual void ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity) = 0;
	virtual void ExecuteCommandListCancel() = 0;
	virtual void ConnectL() = 0;
	virtual void ReleaseCle() = 0;
	virtual void Close() = 0;
	virtual TBool IsConnected() = 0;
	virtual MCleSessionProxy* CreateNewProxyL() = 0;
	};

class CCleSessionProxy : public CBase, public MCleSessionProxy
	{
public:
	static CCleSessionProxy* NewL();

	//from MCleSessionProxy
	void ExecuteCommandList(const CSsmCommandList& aCmdList, TRequestStatus& aStatus, TCmdErrorSeverity& aSeverity);
	void ExecuteCommandListCancel();
	void ConnectL();
	void ReleaseCle();
	void Close();
	TBool IsConnected();
	MCleSessionProxy* CreateNewProxyL();
	
private:
	inline CCleSessionProxy() {}
	~CCleSessionProxy();

private:
	RCleSession iCleSession;
	TBool iConnected;
	};

#endif
