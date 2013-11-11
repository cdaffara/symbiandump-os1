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

#ifndef SSMSWPPOLICYCLI_H
#define SSMSWPPOLICYCLI_H

#include <e32std.h>

#include <ssm/ssmswppolicy.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmcommandlist.h>
#include "ssmdebug.h"

const TInt KSsmSwpPolicySrvMsgSlots = 1; // This value was fixed during design.
const TInt KSsmSwpPolicyCliVersionMajor = 0; // 0-127
const TInt KSsmSwpPolicyCliVersionMinor = 0; // 0-99
const TInt KSsmSwpPolicyCliVersionBuild = 0; // 0-32767

/**
 The SsmSwpPolicy server client session.
 
 @internalComponent
 @released
 */
NONSHARABLE_CLASS(RSsmSwpPolicySession) : public RSessionBase
	{
public: 
	RSsmSwpPolicySession();
	void ConnectL();

	// For testing purposes
#ifdef TEST_SSM_SERVER
	void ConnectL(const TDesC& aServerName);
#endif // TEST_SSM_SERVER

	void Close();
	
	void CallSetDllHandleAndInitialize(TInt aPolicyHandle, TRequestStatus& aStatus);
	TInt CallInitializeCancel();
	void CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	TInt CallPrepareCommandListCancel();
	CSsmCommandList* CallCommandListL();
	void CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	TInt CallHandleCleReturnValueCancel();
	
	TBool IsConnected();
private:
	RBuf8 iCmdListBuf;
	TVersion iVersion;
	};

#endif //SSMSWPPOLICYCLI_H
