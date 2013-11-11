// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of BCA Test Proxy
// The proxy encapsulates test logging and error checking.
// 
//

/**
 @file
*/

#include "bcatestproxy.h"
#include <test/testexecutelog.h>
// Note on log usage:
// We reuse the macros defined for TestExecute. 
// The macros rely on the objects' ::Logger method to return the logger instance


using namespace te_xxBcaUnitTests; // All xxBCA tests
CExecutor::CExecutor(MBca& aBca):
	CActive(EPriorityStandard),
	iBca(aBca)
	{
	CActiveScheduler::Add(this);
	}
		
CExecutor::~CExecutor()
	{
	Cancel();
	}
	
void CExecutor::Exec()
	{		
	CActiveScheduler::Start();
	}
		
TInt CExecutor::CompletionError()
	{
	return iCompletionError;
	}
	
void CExecutor::RunL()
	{
	iCompletionError = iStatus.Int();
	CActiveScheduler::Stop();
	}
		
// MUST be provided, else AS crashes when we force completion.
void CExecutor::DoCancel()
	{
	// Do nothing.
	}
		
void CExecutor::ForceCompletion(TInt err)
	{
	if(IsActive())
		{
		TRequestStatus* s = &iStatus;
		User::RequestComplete(s, err);
		}
	}


CWriter::CWriter(MBca& aBca):
	CExecutor(aBca)
	{}

CWriter::~CWriter()
	{
	}
	
void CWriter::Write(const TDesC8& aPkt)
	{
	iBca.Write(iStatus, aPkt);
	SetActive();
	}

void CWriter::DoCancel()
	{
	iBca.CancelWrite();
	}
	

CReader::CReader(MBca& aBca):
	CExecutor(aBca)
	{}
	
void CReader::Read(TDes8& aPkt)
	{
	iBca.Read(iStatus, aPkt);
	SetActive();
	}
	
void CReader::DoCancel()
	{
	iBca.CancelRead();
	}
	
CReader::~CReader()
	{
	}



/**
C++ Constructor 

@param aLogProvider test logger owner
@param aProxyName the name of this test proxy, to be used for logging purposes.
*/
CBcaTestProxy::CBcaTestProxy(CTestStep& aLogProvider, const TDesC& aProxyName):
	iLogProvider(aLogProvider),
	iProxyName(aProxyName)
	{
	}

/** Obtains TestStep logger
required for the logging macros to work. 

@return TestStep's logger */
CTestExecuteLogger& CBcaTestProxy::Logger()
	{
	return iLogProvider.Logger();
	}

/**
Verifies that an error code is equal to the expected one.
if aCodeToVerify != aExpectedErrorCode, leaves with KErrGeneral

@param aCodeToVerify error code 
@param aExpectedErrCode expected error code
@leave see above  */
void CBcaTestProxy::VerifyErrCodeL(TInt aCodeToVerify, TInt aExpectedErrCode)
	{
	if(aCodeToVerify == aExpectedErrCode)
		{
		INFO_PRINTF3(_L("[%S]::Error code[%d] as expected."), &iProxyName, aCodeToVerify);
		}
	else
		{
		ERR_PRINTF4(_L("[%S]: ERROR: Error code[%d], but expected[%d]"), &iProxyName, aCodeToVerify, aExpectedErrCode);
		User::Leave(KErrArgument);
		}
	}


void CBcaTestProxy::Release()
	{
	INFO_PRINTF2(_L("[%S]::Release"), &iProxyName);
	ASSERT(iBca);
	iBca->Release(); // delete the BCA
	iBca = NULL;
	// Don't delete our memory. Deriving class handles creation / deletion
	}

void CBcaTestProxy::Open(const TDesC& aChannelId) 
	{
	INFO_PRINTF3(_L("[%S]::Open on channel [%S]"), &iProxyName, &aChannelId);	
	ASSERT(iBca);	
	iBca->Open(iOpenRequest, aChannelId);
	}
	
void CBcaTestProxy::WaitForOpenL(TInt aExpectedErr)
	{	
	INFO_PRINTF3(_L("[%S]::WaitForOpenL, expecting error[%d]"), &iProxyName, aExpectedErr);
	User::WaitForRequest(iOpenRequest);
	VerifyErrCodeL(iOpenRequest.Int(), aExpectedErr);
	}

void CBcaTestProxy::Close()
	{
	INFO_PRINTF2(_L("[%S]::Close"), &iProxyName);
	ASSERT(iBca);
	iBca->Close();
	iReader->Cancel();
	iWriter->Cancel();
	}

void CBcaTestProxy::Shutdown()
	{
	INFO_PRINTF2(_L("[%S]::Shutdown"), &iProxyName);
	ASSERT(iBca);
	iBca->Shutdown(iShutdownRequest);
	}
	
void CBcaTestProxy::WaitForShutdownL(TInt aExpectedErr)
	{
	INFO_PRINTF3(_L("[%S]::WaitForShutdownL, expecting error[%d]"), &iProxyName, aExpectedErr);
	User::WaitForRequest(iShutdownRequest);
	VerifyErrCodeL(iShutdownRequest.Int(), aExpectedErr);
	iReader->Cancel();
	iWriter->Cancel();
	}
		
void CBcaTestProxy::Read(TDes8& aBuf)
	{
	INFO_PRINTF2(_L("[%S]::Read"), &iProxyName);
	ASSERT(iBca);
	iReader->Read(aBuf);
	}
	
void CBcaTestProxy::WaitForReadL(TInt aExpectedErr)
	{
	INFO_PRINTF3(_L("[%S]::WaitForReadL, expecting error[%d]"), &iProxyName, aExpectedErr);
	
	iReader->Exec();
	VerifyErrCodeL(iReader->CompletionError(), aExpectedErr);
	}

void CBcaTestProxy::Write(const TDesC8& aBuf)
	{
	INFO_PRINTF2(_L("[%S]::Write"), &iProxyName);
	ASSERT(iBca);
	iWriter->Write(aBuf);
	}

void CBcaTestProxy::WaitForWriteL(TInt aExpectedErr)
	{
	INFO_PRINTF3(_L("[%S]::WaitForWriteL, expecting error[%d]"), &iProxyName, aExpectedErr);
	
	iWriter->Exec();
	VerifyErrCodeL(iWriter->CompletionError(), aExpectedErr);
	}
		
void CBcaTestProxy::CancelRead() 
	{
	INFO_PRINTF2(_L("[%S]::CancelRead"), &iProxyName);
	ASSERT(iBca);
	iReader->Cancel();
	}

void CBcaTestProxy::CancelWrite()
	{
	INFO_PRINTF2(_L("[%S]::CancelWrite"), &iProxyName);
	iWriter->Cancel();
	} 

void CBcaTestProxy::Ioctl(TInt aOptLevel, TInt aOptName, TDes8& aOpt)
	{
	INFO_PRINTF4(_L("[%S]::Ioctl Level=[%d] Name=[%d]"), &iProxyName, aOptLevel, aOptName);
	ASSERT(iBca);
	iBca->Ioctl(iIoctlRequest, aOptLevel, aOptName, aOpt);
	}
	
void CBcaTestProxy::WaitForIoctlL(TInt aExpectedErr)
	{
	INFO_PRINTF3(_L("[%S]::WaitForIoctL, expecting error[%d]"), &iProxyName, aExpectedErr);
	User::WaitForRequest(iIoctlRequest);
	VerifyErrCodeL(iIoctlRequest.Int(), aExpectedErr);
	}
	
	
void CBcaTestProxy::CancelIoctl()
	{
	INFO_PRINTF2(_L("[%S]::CancelIoctl"), &iProxyName);
	ASSERT(iBca);
	iBca->CancelIoctl();
	}
	
/** 
C++ constructor */
CxxBcaProxy::CxxBcaProxy(CTestStep& aLogger, const TDesC& aProxyName):
	CBcaTestProxy(aLogger, aProxyName)
	{
	}
/** 
Standard construction 

@param aLogger TestStep logger
@param aProxyName proxy name for logging
@leave if construction fails. */
CxxBcaProxy* CxxBcaProxy::NewL(CTestStep& aLogger, const TDesC& aProxyName)
	{
	CxxBcaProxy* self = new (ELeave)CxxBcaProxy(aLogger, aProxyName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Standard construction */	
void CxxBcaProxy::ConstructL()
	{
	_LIT(KxxBcaDllName, "C32Bca");
	TInt loadErr = iBcaDll.iObj.Load(KxxBcaDllName);
	if(KErrNone != loadErr)
		{
		ERR_PRINTF4(_L("[%S]::ConstructL: Failed to load library[%S], error[%d]. Leaving."), &iProxyName, KxxBcaDllName, loadErr);	
		User::Leave(loadErr);
		}
	
	static const int KNewBcaFactoryProcOrdinal(1);
	TNewBcaFactoryProcL newBcaFactoryProcL = 
		(TNewBcaFactoryProcL)iBcaDll.iObj.Lookup(KNewBcaFactoryProcOrdinal);
	if(!newBcaFactoryProcL) // can't use User::LeaveIfNull, becuase it returns KErrNoMemory
		{
		ERR_PRINTF2(_L("[%S]::ConstructL: Failed to locate NewBcaFactoryL entry point."), &iProxyName);	
		User::Leave(KErrBadLibraryEntryPoint);
		}
	
	MBcaFactory* bcaFactory = NULL; // We need to create only one BCA
	TRAPD(err, bcaFactory = newBcaFactoryProcL());
	if(KErrNone != err)
		{
		ERR_PRINTF3(_L("[%S]::ConstructL: NewBcaFactoryL left with[%d]."), &iProxyName, err);	
		User::Leave(KErrCompletion);
		}
		
	ASSERT(bcaFactory); // we should not get NULL. The factory function should leave instead.
	
	CleanupReleasePushL(*bcaFactory);	
	iBca = bcaFactory->NewBcaL();
	CleanupStack::PopAndDestroy(bcaFactory);
	ASSERT(iBca);
	iReader = new (ELeave)CReader(*iBca);
	iWriter = new (ELeave)CWriter(*iBca);
	}
	
void CxxBcaProxy::Release()
	{
	INFO_PRINTF2(_L("[%S]::CxxBcaProxy::Release: deleting proxy."), &iProxyName);
	CBcaTestProxy::Release();
	if(iReader->IsActive())
		{
		iReader->ForceCompletion(KErrCancel);
		}
	
	if(iWriter->IsActive())
		{
		iWriter->ForceCompletion(KErrCancel);
		}
		
	delete iReader;
	delete iWriter;
	delete this;
	// BCA host Dll is AutoClosed.		
	}
	


