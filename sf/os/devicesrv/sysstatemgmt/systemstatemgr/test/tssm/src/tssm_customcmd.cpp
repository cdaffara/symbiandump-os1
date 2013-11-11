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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32debug.h>
#include <e32property.h>
#include <s32mem.h> 
#include <f32file.h>
#include <s32file.h>

#include "tssm_customcmd.h"
#include "tssm_ssmclient.h"
#include "ssmdebug.h"

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcustomcommand.h>

/**
  Function expects no arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TCustomCmdDllFn1(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TCustomComdDllFn1 - entry: Used to test a panic in a custom cmd"));
	
	User::Panic(KSsmCustomCmdTest, ESsmCustomCmdTestPanic1);
	
	RDebug::Print(_L("TCustomComdDllFn1 - exit"));
 	return KErrNone;
	}

EXPORT_C void TCustomCmdDllFn2L(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TCustomComdDllFn2 - entry: Used to test a capability error in a custom cmd"));
	
	User::Leave(KErrPermissionDenied);
	
	RDebug::Print(_L("TCustomComdDllFn2 - exit"));
	}

EXPORT_C TInt TCustomCmdDllFn3(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TCustomComdDllFn3 - entry"));
	
	User::Panic(KSsmCustomCmdTest, ESsmCustomCmdTestPanic3);
	
	RDebug::Print(_L("TCustomComdDllFn3 - exit"));
 	return KErrNone;
	}

EXPORT_C TInt TCustomCmdDllFn4(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TCustomComdDllFn4 - entry"));
	
	User::Panic(KSsmCustomCmdTest, ESsmCustomCmdTestPanic4);
	
	RDebug::Print(_L("TCustomComdDllFn4 - exit"));
 	return KErrNone;
	}

/**
 * This custom command never finishes execution and
 * when cancelled will cancel an SwP transition
 * 
 * @internalComponent
 * @test
 */

NONSHARABLE_CLASS(CWaitingCancelSwPCustomCmd) : public CBase, public MSsmCustomCommand
	{
public:
	static CWaitingCancelSwPCustomCmd* NewL();
	
	// from MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/);
	void Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest);
	void Close();
	void Release();
	void ExecuteCancel();

	~CWaitingCancelSwPCustomCmd();
	
private:
	void ConstructL();
	
private:
	TRequestStatus* iRequestStatus;
	RSsmStateManager iStateManager;
	};

CWaitingCancelSwPCustomCmd* CWaitingCancelSwPCustomCmd::NewL()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::NewL() called"));
	CWaitingCancelSwPCustomCmd* self = new (ELeave) CWaitingCancelSwPCustomCmd();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

class RFakeSsmCreator : public RSessionBase
	{
public:
	TInt Connect()
		{
		return CreateSession(KSsmServerNameTest, TVersion(KSsmServMajorVersionNumber, KSsmServMinorVersionNumber, KSsmServBuildVersionNumber));
		}
	};

void CWaitingCancelSwPCustomCmd::ConstructL()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::ConstructL() called"));
	// To get connected to the testing SSM server first make a connection using the RFakeSsmCreator class
	RFakeSsmCreator base;
	User::LeaveIfError(base.Connect());
	// base is now connected to the testing SSM server
	// transfer ownership of base.Handle() handle ID to iStateManager
	iStateManager.SetHandle(base.Handle());
	// base is not closed before returning as ownership of the handle has been passed to iStateManager
	// this is just like transfering ownership of pointers
	}

CWaitingCancelSwPCustomCmd::~CWaitingCancelSwPCustomCmd()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::~CWaitingCancelSwPCustomCmd() called"));
	iStateManager.Close();
	}

TInt CWaitingCancelSwPCustomCmd::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::Initialize() called"));
	// Nothing to do
	return KErrNone;
	}

void CWaitingCancelSwPCustomCmd::Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest)
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::Execute() called"));
	iRequestStatus = &aRequest;
	aRequest = KRequestPending;
	}
	
void CWaitingCancelSwPCustomCmd::Close()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::Close() called"));
	// Nothing to do
	}

void CWaitingCancelSwPCustomCmd::Release()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::Release() called"));
	delete this;
	}

void CWaitingCancelSwPCustomCmd::ExecuteCancel()
	{
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::ExecuteCancel() called"));
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::ExecuteCancel() - calling iStateManager.RequestSwpChangeCancel()"));
	// Cancel a change which hasn't been issued
	iStateManager.RequestSwpChangeCancel();
	DEBUGPRINT1(_L("CWaitingCancelSwPCustomCmd::ExecuteCancel() - completing request"));
	User::RequestComplete(iRequestStatus, KErrCancel);
	}

EXPORT_C MSsmCustomCommand* TCustomCmdDllFn5L()
	{
	return CWaitingCancelSwPCustomCmd::NewL();
	}

/**
 * This custom command waits until the property specified in
 * the parameters to change value before continuing.
 * 
 * @internalComponent
 * @test
 */

NONSHARABLE_CLASS(CWaitingPnsCustomCmd) : public CBase, public MSsmCustomCommand
	{
public:
	static CWaitingPnsCustomCmd* NewL();
	
	// from MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void Close();
	void Release();
	void ExecuteCancel();

private:
	void ConstructL();
	~CWaitingPnsCustomCmd();
	
private:
	RProperty iProperty;
	};

CWaitingPnsCustomCmd* CWaitingPnsCustomCmd::NewL()
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::NewL() called"));
	CWaitingPnsCustomCmd* self = new (ELeave) CWaitingPnsCustomCmd();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CWaitingPnsCustomCmd::ConstructL()
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::ConstructL() called"));
	}

CWaitingPnsCustomCmd::~CWaitingPnsCustomCmd()
	{
	if(iProperty.Handle() != KNullHandle)
		{
		iProperty.Cancel();
		}
	iProperty.Close();
	}

TInt CWaitingPnsCustomCmd::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::Initialize() called"));
	// Nothing to do
	return KErrNone;
	}

void CWaitingPnsCustomCmd::Execute(const TDesC8& aParams, TRequestStatus& aRequest)
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::Execute() called"));
	RDesReadStream stream;
	stream.Open(aParams);
	// Read the key value for the property to read
	TUint32 key = 0;
	TRAPD(err, key = stream.ReadUint32L());
	stream.Close();
	
	if (err != KErrNone)
		{
		DEBUGPRINT2(_L("CWaitingPnsCustomCmd::Execute() - failure reading P+S value to wait on: %d"), err);
		TRequestStatus* trs = &aRequest;
		User::RequestComplete(trs, err);
		return;
		}
	else
		{
		DEBUGPRINT2(_L("CWaitingPnsCustomCmd::Execute() - P+S value to wait on: 0x%x"), key);
		err = iProperty.Attach(RProcess().SecureId(), key);
		if (err != KErrNone)
			{
			DEBUGPRINT2(_L("CWaitingPnsCustomCmd::Execute() - error attaching to P+S key: %d"), err);
			TRequestStatus* trs = &aRequest;
			User::RequestComplete(trs, err);
			return;
			}
		// Successfully attached so subscribe
		iProperty.Subscribe(aRequest);
		}
	}
	
void CWaitingPnsCustomCmd::Close()
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::Close() called"));
	// Nothing to do
	}

void CWaitingPnsCustomCmd::Release()
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::Release() called"));
	delete this;
	}

void CWaitingPnsCustomCmd::ExecuteCancel()
	{
	DEBUGPRINT1(_L("CWaitingPnsCustomCmd::ExecuteCancel() called"));
	if(iProperty.Handle() != KNullHandle)
		{
		iProperty.Cancel();
		}
	}

EXPORT_C MSsmCustomCommand* TCustomCmdDllFn6L()
	{
	return CWaitingPnsCustomCmd::NewL();
	}
