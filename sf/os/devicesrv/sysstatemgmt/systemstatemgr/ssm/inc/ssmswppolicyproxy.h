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

#ifndef SSMSWPPOLICYPROXY_H
#define SSMSWPPOLICYPROXY_H

#include <e32base.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmswp.h>

#include "ssmswppolicycli.h"

/**
This class is used for connecting and communicating with the ssmswppolicy server.

@internalComponent
@released
*/
NONSHARABLE_CLASS (CSsmSwpPolicyCliSession) : public CBase
	{
public:
	~CSsmSwpPolicyCliSession();
	static CSsmSwpPolicyCliSession* NewL();

	void ConnectL();
	void Close();
	TBool IsConnected();

	void CallSetDllHandleAndInitialize(TInt aPolicyHandle, TRequestStatus& aStatus);
	TInt CallInitializeCancel();
	void CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	TInt CallPrepareCommandListCancel();
	CSsmCommandList* CallCommandList();
	void CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	TInt CallHandleCleReturnValueCancel();
	
private:
	CSsmSwpPolicyCliSession();

private:
	RSsmSwpPolicySession iSsmSwpPolicySession;
	};

#endif //SSMSWPPOLICYPROXY_H
