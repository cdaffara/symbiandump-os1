// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "epos_cpospsymonitorthreadtest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMonitorThreadTest::CPosPSYMonitorThreadTest() : 
	iVerificationType(EFirstVerification)
    {
	}

// EPOC default constructor can leave.
void CPosPSYMonitorThreadTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYMonitorThreadTest* CPosPSYMonitorThreadTest::NewL()
    {
    CPosPSYMonitorThreadTest* self = new (ELeave) CPosPSYMonitorThreadTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMonitorThreadTest::~CPosPSYMonitorThreadTest()
    {
	delete iRequester;
	iRequester = NULL;
    }

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Monitor Thread Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& /*aTestParameters*/)
    {   	
	aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;

#ifndef __WINS__

    TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteTest();
		}

#else
	_LIT(KMonitorThreads, "Process monitoring is not performed on emulator.");
	TRAP(iTestStatus, AddTestResultL(KMonitorThreads, EInfoMessage));

	CompleteTest();

#endif
    }

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::CancelTest()
    {
#ifndef __WINS__
	
	iTestStatus = KErrCancel;
	iRequester->CancelRequest();
	CompleteTest(KErrCancel);

#endif
    }

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::HandlePositionCompleteL(
	TInt aCompleteCode, 
	TTimeIntervalMicroSeconds /*aRequestTime*/)
    {
#ifndef __WINS__
	iVerificationType = ELastVerification;
	VerifyProcessParametersL();

	if (iTestStatus == KErrCancel)
		{
		CompleteTest();
		return;
		}
	
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		UnexpectedCompletionCode(aCompleteCode);
		}

	CheckResultsL();
	
	iRequester->ClosePositioner();
	
    ResetResults();
    CompleteTest(KErrNone);

#else
	//Dummy calculation to remove build warning for wins
	aCompleteCode++;
	
#endif
	}

#ifndef __WINS__

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::ExecuteTestL()
	{
	// To be sure of the process name before opening PSY the 
	// name is set every time the test is executed
	// User::LeaveIfError(RProcess().Rename(KPSYTesterProcName));
	User::LeaveIfError(User::RenameProcess(KPSYTesterProcName));
	iNumberOfThreads = CheckProcessParametersL();
	
	if (iNumberOfThreads == 0)
		{
		_LIT(KNameChange, "Process name mismatch.");
		AddTestResultL(KNameChange, EErrorMessage);
		CompleteTest(KErrNotFound);
		}

	MakeRequestL();
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::MakeRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::MakeRequestL()
	{
	VerifyProcessParametersL();
	iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	VerifyProcessParametersL();
	iRequester->MakeRequest(iPosInfo);
	VerifyProcessParametersL();
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::VerifyProcessParametersL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::VerifyProcessParametersL()
	{
	TInt nrOfThreads = CheckProcessParametersL();

	if (nrOfThreads != iNumberOfThreads)
		{
		if (iVerificationType == EMiddleVerification)
			{
			iThreadsCreated = ETrue;
			}
		else if (iVerificationType == ELastVerification)
			{
			iThreadsNotClosed = ETrue;
			}
		}
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::CheckProcessParametersL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYMonitorThreadTest::CheckProcessParametersL()
	{
	TInt nrOfThreads = 0;
	TFullName processName;
	TProcessPriority processPriority;
	TFullName name;
	
	if (iVerificationType == EFirstVerification)
		{
		TInt threadCount = 0;
		TFindThread find(_L("*PSY Tester*"));
		while (find.Next(name) == KErrNone)
			{
			GetProcessNameAndPriorityL(processName, processPriority, name); 
		
			iProcessName = processName;
			iProcessPriority = processPriority;
			iVerificationType = EMiddleVerification;
			
			threadCount ++;
			}
		
		nrOfThreads = threadCount;
		}
	else
		{
		//TFindThread find(_L("Main::PSY Tester*"));
		TFindThread find(_L("*PSY Tester*::Main"));
		while (find.Next(name) == KErrNone)
			{
			GetProcessNameAndPriorityL(processName, processPriority, name); 
		
			if (processName == iProcessName)
				{
				if (processPriority != iProcessPriority)
					{
					iProcessPriorityChanged = ETrue;
					RProcess().SetPriority(iProcessPriority);
					}
				nrOfThreads++;
				}
			else
				{
				iProcessNameChanged = ETrue;
				nrOfThreads = 0;
				break;
				}
			}
		
		//Check if threads are created
		TFindThread findCreated(_L("PSY*Tester*T*"));
		while (findCreated.Next(name) == KErrNone)
			{
			nrOfThreads++;
			}
		}		
	return nrOfThreads;
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::GetProcessNameAndPriorityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::GetProcessNameAndPriorityL(
	TFullName& aProcessName,
	TProcessPriority& aProcessPriority,
	const TFullName& aThreadName)
	{
	RThread thread;
	RProcess process;

	User::LeaveIfError(thread.Open(aThreadName));
	User::LeaveIfError(thread.Process(process));
	
	_LIT(KDelim, "Main::");
	TInt startPosition = process.FullName().Find(KDelim); 
	if (startPosition != KErrNotFound)
		{	
		//Skip 'Main::' from process name
		startPosition += 6; 
		}
	else
		{
		_LIT(KPSY, "PSY");
		TInt psyPosition = process.FullName().Find(KPSY);
		if (psyPosition != 0)
			{
			iProcessNameChanged = ETrue;
			}
		startPosition = 0;
		}
	
	TLex input(process.FullName().Mid(startPosition));
	TInt processNameLength = 0;
	
	while (!input.Eos() && input.Peek() != '[' && input.Peek() != '(')
		{
		input.Inc();
		processNameLength++;
		}
	
	aProcessName = process.FullName().Mid(startPosition).Left(processNameLength);
	aProcessPriority = process.Priority();

	process.Close();
	thread.Close();
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::ResetResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::ResetResults()
	{
	iProcessName.Zero();
	iNumberOfThreads = 0;
	iVerificationType = EFirstVerification;
	iProcessNameChanged = EFalse;
	iThreadsCreated = EFalse;
	iThreadsNotClosed = EFalse;
	iProcessPriorityChanged = EFalse;
	}

// ---------------------------------------------------------
// CPosPSYMonitorThreadTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMonitorThreadTest::CheckResultsL()
	{
	if (iProcessNameChanged)
		{
		_LIT(KNameChange, "PSY has changed process name of PSY Tester. Not checking other process parameters.");
		AddTestResultL(KNameChange, EErrorMessage);
		}
	else
		{
		if (iThreadsCreated)
			{
			_LIT(KWarning, "PSY creates threads.");
			AddTestResultL(KWarning, EWarningMessage);
			}
		else
			{
			_LIT(KInfo, "PSY does not create any threads.");
			AddTestResultL(KInfo, EInfoMessage);
			}
	
		if (iThreadsNotClosed)
			{
			_LIT(KError, "PSY does not close all threads it creates.");
			AddTestResultL(KError, EErrorMessage);	
			}
		
		if (iProcessPriorityChanged)
			{
			_LIT(KError, "PSY has changed process priority of PSY Tester.");
			AddTestResultL(KError, EErrorMessage);
			}
		}
	}

#endif

//  End of File

