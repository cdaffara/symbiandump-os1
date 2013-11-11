// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tcmd_step_resourcereader.h"

#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmcommandlist.h>
#include <barsc2.h>
#include <barsread2.h>
#include "testcmdlistids.hrh"
#include "ssmcommandlistresourcereaderimpl.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"
#include "ssmcommandfactory.h"
#include "ssmatest_utils.h"

#include <e32debug.h>

static const TUint KSwpKey = {12};

class CActiveSchedulerWithRunLCount : public CActiveScheduler
	{
public:
	void Start();
	void Start(TInt aRunLCount);
private:
	void WaitForAnyRequest();
private:
	TInt iRunLCount;
	TBool iCanStopEarly;
	};

void CActiveSchedulerWithRunLCount::Start()
	{
	iCanStopEarly = EFalse;
	CActiveScheduler::Start();
	}

void CActiveSchedulerWithRunLCount::Start(TInt aRunLCount)
	{
	iCanStopEarly = ETrue;
	iRunLCount = aRunLCount;
	CActiveScheduler::Start();
	}

void CActiveSchedulerWithRunLCount::WaitForAnyRequest()
	{
	if (iCanStopEarly && !--iRunLCount)
		{
		Stop();
		}
	CActiveScheduler::WaitForAnyRequest();
	}

static TInt StopSchedulerCallBack(TAny* /*aNotUsed*/)
	{
	CActiveScheduler::Current()->Stop();
	return KErrNone;
	}

CResourceReaderTest::~CResourceReaderTest()
	{
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	iFs.Close();
	}

CResourceReaderTest::CResourceReaderTest()
	: iWhichPrepare(EPrepareNoCheck)
	{
	SetTestStepName(KTResourceReader);
	iMainThreadId = RThread().Id();
	}

/** */
TVerdict CResourceReaderTest::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("*** >CResourceReaderTest::doTestStepPreambleL"));
	iActiveScheduler = new(ELeave) CActiveSchedulerWithRunLCount;
	CActiveScheduler::Install(iActiveScheduler);
	
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopSchedulerCallBack, NULL);
	iAsyncStopScheduler->Set(stop);

	SsmCommandFactory::DelegateTo(this); // allows this test code to hook command construction

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareAuto()); // allows all threads in this process to use this file server session

	INFO_PRINTF1(_L("*** CResourceReaderTest::doTestStepPreambleL>"));
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CResourceReaderTest::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("*** >CResourceReaderTest::doTestStepPostambleL"));

	INFO_PRINTF1(_L("*** CResourceReaderTest::doTestStepPostambleL>"));
	return CTestStep::doTestStepPostambleL();
	}

TBool CResourceReaderTest::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
	{
	INFO_PRINTF1(_L("*** >CResourceReaderTest::ConditionalCommandAllowedL"));
	RResourceReader reader;
	reader.OpenLC(&aResourceFile, aResourceId);
	const TInt32 softwareReason = reader.ReadInt32L();
	const TInt32 hardwareReason = reader.ReadInt32L();
	CleanupStack::PopAndDestroy(&reader);

	TBool ret = EFalse;
	if (softwareReason == EUnconditionalSoftwareReason)
		{
		ret = ETrue;
		}
	INFO_PRINTF1(_L("*** CResourceReaderTest::ConditionalCommandAllowedL>"));
	return ret;
	}

TVerdict CResourceReaderTest::doTestStepL()
	{
	INFO_PRINTF1(_L("*** >CResourceReaderTest::doTestStepL"));
	
	// Launch the different test cases
	TestConstructCases();
	TestInitialiseCases();
	TestPrepareCases();
	TestCancelCases();
	TestDelayCases();
	TestGetCases();

	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();
	INFO_PRINTF1(_L("*** CResourceReaderTest::doTestStepL>"));
	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDRSC-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */

void CResourceReaderTest::TestConstructCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestConstruct1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestConstruct2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestOOM1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestOOM2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;	
	}
/**
Old Test CaseID 		AFSS-CMDRSC-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */

void CResourceReaderTest::TestInitialiseCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestInitialise1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise3L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise4L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise5L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise6L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise7L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise8L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise9L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise10L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise11L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise12L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise13L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise14L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestInitialise15L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestNoMappings1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestNoMappings2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	}

/**
Old Test CaseID 		AFSS-CMDRSC-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */

void CResourceReaderTest::TestPrepareCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestPrepare1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare3L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare4L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare5L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare6L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare7L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare8L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare9L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare10L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare11L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare12L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare13L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare14L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare15L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare16L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestPrepare17L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	}

/**
Old Test CaseID 		AFSS-CMDRSC-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */
void CResourceReaderTest::TestCancelCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestCancel1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestCancel2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	}

/**
Old Test CaseID 		AFSS-CMDRSC-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */
void CResourceReaderTest::TestDelayCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestDelay1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestDelay2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	}

void CResourceReaderTest::TestConstruct1L()
	{
	INFO_PRINTF1(_L("*** Starting TestConstruct1L"));
	CSsmCommandListResourceReaderImpl* reader = NULL;
	TRAPD(err, reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KNullDesC, *this));
	delete reader;
	TEST(err == KErrArgument);
	}

/**
Old Test CaseID 		AFSS-CMDRSC-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0016
 */

void CResourceReaderTest::TestGetCases()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestGet1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGet2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGet3L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGet4L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGet5L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGeneral1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestCommandType1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGetCommandListIds1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGetCommandListIds2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestGetCommandListIds3L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	}


// simple valid construct followed by delete, should not leave or leak memory
void CResourceReaderTest::TestConstruct2L()
	{
	INFO_PRINTF1(_L("*** Starting TestConstruct2L"));
	_LIT(KValidCommandListPath, "z:\\resource\\ssmatest\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KValidCommandListPath, *this);
	delete reader;
	}

// initialise should fail with KErrNotFound after passing a bad path to NewL
void CResourceReaderTest::TestInitialise1L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest directory that does not exist\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrPathNotFound);

	delete reader;
	}

// Call Init, fails with KErrNotFound as directory doesn't contain any command list resource files
void CResourceReaderTest::TestInitialise2L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNotFound);

	delete reader;
	}


// Call Init twice using the same TRequestStatus object without waiting for the first init to complete.
// 2nd init should panic with EInUse1.
// The only option for the resource reader when this happens is to panic, as it cannot
// return the error from the second init using the first init's request status.
void CResourceReaderTest::TestInitialise3L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise3L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadInitialise3);
	TEST(exitReason == EInUse1);
	}

void CResourceReaderTest::ThreadInitialise3L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	reader->Initialise(status); // panic is only thing to do when same status is used on 2 calls to init

	delete reader;
	}


// Successful init followed by another call to Init should not give error
void CResourceReaderTest::TestInitialise4L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise4L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// first init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone); // successful init

	// second init
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone); // successful init

	delete reader;
	}

// Init using a resource file with invalid version number should complete with error.
void CResourceReaderTest::TestInitialise5L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise5L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNotSupported);

	delete reader;
	}

// Prepare state list on uninitialised reader should panic
void CResourceReaderTest::TestInitialise6L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise6L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadInitialise6);
	TEST(exitReason == ENotInitialized);
	}

void CResourceReaderTest::ThreadInitialise6L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	TSsmState state;
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);

	delete reader;
	}

// Prepare swp list on uninitialised reader should panic
void CResourceReaderTest::TestInitialise7L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise7L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadInitialise7);
	TEST(exitReason == ENotInitialized2);
	}

void CResourceReaderTest::ThreadInitialise7L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	TSsmSwp swp(0, 0);
	reader->PrepareCommandList(*this, ETestCommandListId1, swp, status); // should panic with ENotInitialized2

	delete reader;
	}

// Get command list on uninitialised reader should panic
void CResourceReaderTest::TestInitialise8L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise8L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadInitialise8);
	TEST(exitReason == ENoPreparedCommandList);
	}

void CResourceReaderTest::ThreadInitialise8L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	reader->GetCommandList(); // should panic with ENoPreparedCommandList

	delete reader;
	}

// Prepare state list on uninitialised reader (after init fail) should panic
void CResourceReaderTest::TestInitialise9L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise9L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadInitialise9);
	TEST(exitReason == ENotInitialized);
	}

void CResourceReaderTest::ThreadInitialise9L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists1\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);
	CleanupStack::PushL(reader);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNotSupported);

	// init has failed, prepare a command list
	TSsmState state;
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status); // should panic with ENotInitialized
	
	CleanupStack::PopAndDestroy(reader);
	}

// delete after 1 Init RunL should cause no error
void CResourceReaderTest::TestInitialise10L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise10L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status); // this init should be split into 3 RunLs
	TEST(status == KRequestPending);

	Run(1);
	delete reader; // should complete status
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);
	}

// delete after 2 Init RunLs should cause no error
void CResourceReaderTest::TestInitialise11L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise11L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status); // this init should be split into 3 RunLs
	TEST(status == KRequestPending);

	Run(2);
	delete reader; // should complete status
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);
	}

// cancel init before it can RunL - check completes with KErrCancel and no error/mem leak on delete
void CResourceReaderTest::TestInitialise12L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise12L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	// cancel init, should complete status with KErrCancel
	reader->InitialiseCancel();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel init after 1 RunL - check completes with KErrCancel and no error/mem leak on delete
void CResourceReaderTest::TestInitialise13L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise13L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run(1);
	// cancel init after only 1 RunL (3 required to complete init), should complete status with KErrCancel
	reader->InitialiseCancel();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel init after 2 RunLs - check completes with KErrCancel and no error/mem leak on delete
void CResourceReaderTest::TestInitialise14L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise14L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run(2);
	// cancel init after only 2 RunLs (3 required to complete init), should complete status with KErrCancel
	reader->InitialiseCancel();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel init before it can RunL - check no error, then init again without cancel - should init ok
void CResourceReaderTest::TestInitialise15L()
	{
	INFO_PRINTF1(_L("*** Starting TestInitialise15L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	// cancel init, should complete status with KErrCancel
	reader->InitialiseCancel();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);

	// init again (without cancel) - should complete with KErrNone (even after previous cancel)
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	delete reader;
	}

// delete immediately after prepare should cause no error
void CResourceReaderTest::TestPrepare1L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);

	delete reader; // should complete status
	User::WaitForRequest(status);
	TEST(status == KErrCancel);
	}

// delete after only 1 prepare RunL - should cause no error
void CResourceReaderTest::TestPrepare2L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);

	Run(1);
	TEST(status == KRequestPending); // should be still pending as 1 RunL isn't enough to complete the prepare
	delete reader; // should complete status
	User::WaitForRequest(status);
	TEST(status == KErrCancel);
	}

// delete after only 2 prepare RunLs - should cause no error
void CResourceReaderTest::TestPrepare3L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare3L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);

	Run(2);
	TEST(status == KRequestPending); // should be still pending as 2 RunLs isn't enough to complete the prepare
	delete reader; // should complete status
	User::WaitForRequest(status);
	TEST(status == KErrCancel);
	}

// after 3 RunLs, prepare should be completed with KErrNone.
// deleting shouldn't complete again with KErrCancel.
void CResourceReaderTest::TestPrepare4L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare4L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);

	Run(3);
	User::WaitForRequest(status);
	TEST(status == KErrNone); // 1st RunL initial step, 2nd prepare 5 commands, 3rd prepares last 5 commands, so after 3 RunLs, a list of 10 commands should be complete
	delete reader; // should not complete status with KErrCancel
	TEST(status == KErrNone);
	}

// check command list is "ready" after a prepare and
// no longer ready after calling IsCommandListReady a second time
void CResourceReaderTest::TestPrepare5L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare5L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	// check command list status
	TEST(reader->IsCommandListReady());
	TEST(!reader->IsCommandListReady());
	TEST(!reader->IsCommandListReady());
	delete reader;
	}

// cancel prepare before it can RunL - check no error
void CResourceReaderTest::TestPrepare6L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare6L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// cancel prepare before it can RunL, should complete status with KErrCancel
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	reader->PrepareCommandListCancel();
	User::WaitForRequest(status);
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel prepare after only 1 RunL - check no error
void CResourceReaderTest::TestPrepare7L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare7L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// cancel prepare after 1 RunL, should complete status with KErrCancel
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run(1);
	TEST(status == KRequestPending);
	reader->PrepareCommandListCancel();
	User::WaitForRequest(status);
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel prepare after only 2 RunLs - check no error
void CResourceReaderTest::TestPrepare8L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare8L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// cancel prepare after 2 RunLs, should complete status with KErrCancel
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run(2);
	TEST(status == KRequestPending);
	reader->PrepareCommandListCancel();
	User::WaitForRequest(status);
	TEST(status == KErrCancel);

	delete reader;
	}

// cancel once fully prepared shouldn't complete with KErrCancel
void CResourceReaderTest::TestPrepare9L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare9L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	reader->PrepareCommandListCancel();
	TEST(status == KErrNone); // still KErrNone, as request has already completed

	delete reader;
	}

// cancel during prepare followed by another prepare, should work ok
void CResourceReaderTest::TestPrepare10L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare10L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// first prepare, cancel part way through
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run(2);
	reader->PrepareCommandListCancel();
	User::WaitForRequest(status);
	TEST(status == KErrCancel);

	// second prepare, allow this one to complete normally
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone); // should complete ok, even though previous prepare was cancelled

	delete reader;
	}

// empty command list resource should return an empty command list
void CResourceReaderTest::TestPrepare11L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare11L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone); // preparing an empty command list is not an error

	CSsmCommandList* commandList = reader->GetCommandList();
	TEST(commandList->Count() == 0); // should be empty
	delete commandList;
	commandList = NULL;
	
	delete reader;
	}

// Prepare 2 command lists one after the other, check both are returned by GetCommandList without error
void CResourceReaderTest::TestPrepare12L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare12L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	CSsmCommandList* commandList = reader->GetCommandList();
	TEST(commandList->Count() == 0);
	delete commandList;
	commandList = NULL;

	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	commandList = reader->GetCommandList();
	TEST(commandList->Count() == 0);
	delete commandList;
	commandList = NULL;

	delete reader;
	}

// prepare for id not found, then prepare for an ID found, checks
// the notFound doesn't stop further prepares being attempted
void CResourceReaderTest::TestPrepare13L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare13L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId2, state, status); // ETestCommandListId2 not present in cmdlists5.rss
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNotFound);
	TEST(!reader->IsCommandListReady());

	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone); // prepare worked ok even though previous prepare failed
	TEST(reader->IsCommandListReady());

	delete reader;
	}

// prepare, cancel, then IsCommandListReady, should return false
void CResourceReaderTest::TestPrepare14L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare14L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// cancel prepare after 2 RunLs, should complete status with KErrCancel
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run(2);
	TEST(status == KRequestPending);
	reader->PrepareCommandListCancel();
	User::WaitForRequest(status);
	TEST(status == KErrCancel);

	TEST(!reader->IsCommandListReady());

	delete reader;
	}

// use factory callback to leave during list prepare, then prepare again to
// check it works 2nd time
void CResourceReaderTest::TestPrepare15L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare15L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	iWhichPrepare = EPrepareLeave;
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrGeneral);

	// prepare again to check it works after leaving during the last prepare
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	iWhichPrepare = EPrepareNoCheck;
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	delete reader;
	}

// Init, cancel before init starts processing, then prepare should panic with "NotInitialised"
void CResourceReaderTest::TestPrepare16L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare16L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadPrepare16);
	TEST(exitReason == ENotInitialized);
	}

void CResourceReaderTest::ThreadPrepare16L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	reader->InitialiseCancel();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrCancel);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status); // should panic
	Run();
	User::WaitForRequest(status);

	delete reader;
	}

// Check ConditionalCommandAllowedL is not executed when cmds get constructed
void CResourceReaderTest::TestPrepare17L()
	{
	INFO_PRINTF1(_L("*** Starting TestPrepare17L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists6\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare - only 2 of the 3 commands would be executed
	// however all 3 should be included in the command list
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	iWhichPrepare = EConditional;
	iCommandConstructCount = 0;
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	TEST(iCommandConstructCount == 3);
	
	iWhichPrepare = EPrepareNoCheck;

	delete reader;
	}

// 2 cancels one after the other on uninitialised reader
void CResourceReaderTest::TestCancel1L()
	{
	INFO_PRINTF1(_L("*** Starting TestCancel1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	reader->InitialiseCancel();
	reader->InitialiseCancel();

	delete reader;
	}

// 2 cancels one after the other on initialised reader
void CResourceReaderTest::TestCancel2L()
	{
	INFO_PRINTF1(_L("*** Starting TestCancel2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// cancel with no operation in progress
	reader->InitialiseCancel();
	TEST(status == KErrNone);
	reader->InitialiseCancel();
	TEST(status == KErrNone);

	delete reader;
	}

// GetCommandList before init
void CResourceReaderTest::TestGet1L()
	{
	INFO_PRINTF1(_L("*** Starting TestGet1L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadGet1);
	TEST(exitReason == ENoPreparedCommandList);
	}

void CResourceReaderTest::ThreadGet1L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	CSsmCommandList* commandList = reader->GetCommandList(); // should panic as a command list hasn't been prepared
	commandList = NULL;
	delete reader;
	}

// get cmd list after prepare, then get again should panic
void CResourceReaderTest::TestGet2L()
	{
	// NOTE - This test (and others)use an empty command list resource (provided by cmdlists5.rss) to ensure the reader
	// doesn't try to construct any commands. Any attempt to construct commands
	// in this test code using CSsmCommandListResourceReader would result in
	// NULL being returned by CResourceReaderTest::ConstructCommandFromResourceLC
	// and the NULL being passed to the "real" command list AppendL method, ie. CSsmCommandListImpl::AppendL
	// CSsmCommandListImpl::AppendL expects a non-NULL pointer to be passed to it.
	INFO_PRINTF1(_L("*** Starting TestGet2L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadGet2);
	TEST(exitReason == ENoPreparedCommandList);
	}

void CResourceReaderTest::ThreadGet2L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);
	
	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	CSsmCommandList* commandList = reader->GetCommandList(); // should be ok
	delete commandList;
	commandList = NULL;

	commandList = reader->GetCommandList(); // should panic as GetCommandList has already returned ownership of the command list
	commandList = NULL;
	delete reader;
	}

// prepare ok, then start prepare again, but before complete, try to get - should panic
void CResourceReaderTest::TestGet3L()
	{
	INFO_PRINTF1(_L("*** Starting TestGet3L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadGet3);
	TEST(exitReason == ENoPreparedCommandList);
	}

void CResourceReaderTest::ThreadGet3L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	// prepare again before getting command list
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);

	CSsmCommandList* commandList = reader->GetCommandList(); // should panic as a new prepare has just been initiated
	delete commandList;
	commandList = NULL;

	delete reader;
	}

// get cmd list after prepare - should work ok
void CResourceReaderTest::TestGet4L()
	{
	INFO_PRINTF1(_L("*** Starting TestGet4L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	CSsmCommandList* commandList = reader->GetCommandList(); // should return prepared command list
	TEST(commandList->Count() == 0);
	delete commandList;
	commandList = NULL;

	delete reader;
	}

// Init using 1 resource file that contains an empty mapping array.
// Init should complete with KErrNotFound.
void CResourceReaderTest::TestNoMappings1L()
	{
	INFO_PRINTF1(_L("*** Starting TestNoMappings1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists2\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNotFound);

	delete reader;
	}

// Init using 1 resource file whose mapping LLINK is zero.
// Init should complete with KErrNotFound.
void CResourceReaderTest::TestNoMappings2L()
	{
	INFO_PRINTF1(_L("*** Starting TestNoMappings2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists4\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNotFound);

	delete reader;
	}

// check 2 resource files are loaded by requesting a list from each
// checks state and swp are passed to factory method correctly
void CResourceReaderTest::TestGeneral1L()
	{
	INFO_PRINTF1(_L("*** Starting TestGeneral1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// Prepare cmd list from first resource file
	TSsmState state(0,23);
	iWhichPrepare = EPrepareState;
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	// Prepare cmd list from second resource file
	TSsmSwp swp(KSwpKey, 24);
	iWhichPrepare = EPrepareSwp;
	reader->PrepareCommandList(*this, ETestCommandListId2, swp, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	delete reader;
	iWhichPrepare = EPrepareNoCheck;
	}

// check default delay value is read from resource ok
void CResourceReaderTest::TestDelay1L()
	{
	INFO_PRINTF1(_L("*** Starting TestDelay1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// Prepare cmd list with no delay specified
	const TInt KNonZeroValue = 1;
	iDelay = KNonZeroValue;
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	const TInt KExpectedDelay = 0;
	TEST(iDelay == KExpectedDelay);

	delete reader;
	}

// check specific delay value is read from resource ok
// uses a "real" reader and command list to check the command list contains the correct value
void CResourceReaderTest::TestDelay2L()
	{
	INFO_PRINTF1(_L("*** Starting TestDelay2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists5\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	CSsmCommandList* commandList = reader->GetCommandList(); // should return prepared command list
	const TInt KExpectedDelay = 12;
	const TInt actualDelay = commandList->DelayBetweenCommands();
	TEST(actualDelay == KExpectedDelay);
	delete commandList;
	commandList = NULL;

	delete reader;
	}

// check the expected command type is readable by the RResourceReader provided to ConstructCommandFromResourceLC via the TSsmCommandParameters parameter
void CResourceReaderTest::TestCommandType1L()
	{
	INFO_PRINTF1(_L("*** Starting TestCommandType1L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	TSsmState state(0,23);
	iWhichPrepare = EPrepareType;
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	delete reader;
	iWhichPrepare = EPrepareNoCheck;
	}

// Get command list ids before init should cause a panic
void CResourceReaderTest::TestGetCommandListIds1L()
	{
	INFO_PRINTF1(_L("*** Starting TestGetCommandListIds1L"));
	const TInt exitReason = StartTestInThreadL(TThreadParams::EThreadGetCommandListIds1);
	TEST(exitReason == ENotInitialized3);
	}

void CResourceReaderTest::ThreadGetCommandListIds1L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);
	CleanupStack::PushL(reader);

	RArray<TInt> array;
	CleanupClosePushL(array);
	reader->GetCommandListIdsL(array); // should panic as reader hasn't been initialised
	CleanupStack::PopAndDestroy(&array);

	CleanupStack::PopAndDestroy(reader);
	}

// check calling GetCommandListIdsL passing a non-empty array leaves with KErrArgument
void CResourceReaderTest::TestGetCommandListIds2L()
	{
	INFO_PRINTF1(_L("*** Starting TestGetCommandListIds2L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);
	CleanupStack::PushL(reader);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	RArray<TInt> array;
	CleanupClosePushL(array);
	array.AppendL(ETestCommandListId1); // make array non-empty
	TRAPD(err, reader->GetCommandListIdsL(array)); // should leave with KErrArgument
	TEST(err == KErrArgument);
	CleanupStack::PopAndDestroy(&array);

	CleanupStack::PopAndDestroy(reader);
	}

// check GetCommandListIdsL returns the correct command list ids
void CResourceReaderTest::TestGetCommandListIds3L()
	{
	INFO_PRINTF1(_L("*** Starting TestGetCommandListIds3L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);
	CleanupStack::PushL(reader);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	RArray<TInt> array;
	CleanupClosePushL(array);
	reader->GetCommandListIdsL(array);
	array.FindL(ETestCommandListId1); // should not leave with KErrNotFound
	array.FindL(ETestCommandListId2); // should not leave with KErrNotFound
	TRAPD(err, array.FindL(ETestCommandListId3)); // ETestCommandListId3 should not be found
	TEST(err == KErrNotFound);
	CleanupStack::PopAndDestroy(&array);

	CleanupStack::PopAndDestroy(reader);
	}

// check attempt to construct reader under low memory conditions doesn't leak memory
void CResourceReaderTest::TestOOM1L()
	{
	INFO_PRINTF1(_L("*** Starting TestOOM1L"));
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, DoOOM1L());
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for CSsmCommandListResourceReader::NewL: %d."),failRate-1);
	}

void CResourceReaderTest::DoOOM1L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReader* reader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);
	delete reader;
	}

// check out of memory conditions during command list reader init/prepare don't cause memory to be leaked
void CResourceReaderTest::TestOOM2L()
	{
	INFO_PRINTF1(_L("*** Starting TestOOM2L"));
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, DoOOM2L());
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for CSsmCommandListResourceReaderImpl init + 2 prepares: %d."),failRate-1);
	}

void CResourceReaderTest::DoOOM2L()
	{
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists3\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);
	CleanupStack::PushL(reader);

	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);

	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	if (status == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	TEST(status == KErrNone);

	// Prepare cmd list from first resource file
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	if (status == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	TEST(status == KErrNone);

	// Prepare cmd list from second resource file
	TSsmSwp swp(KSwpKey, 24);
	reader->PrepareCommandList(*this, ETestCommandListId2, swp, status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status);
	if (status == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	TEST(status == KErrNone);

	CleanupStack::PopAndDestroy(reader);
	}

void CResourceReaderTest::Run()
	{
	if (RThread().Id() != iMainThreadId)
		{
		iThreadAsyncStopScheduler->CallBack();
		iThreadActiveScheduler->Start();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		iActiveScheduler->Start();
		}
	}

void CResourceReaderTest::Run(TInt aRunLCount)
	{
	if (RThread().Id() != iMainThreadId)
		{
		iThreadActiveScheduler->Start(aRunLCount);
		}
	else
		{
		iActiveScheduler->Start(aRunLCount);
		}
	}

TInt CResourceReaderTest::StartTestInThreadL(TThreadParams::TFunction aFunction)
	{
	// Tests that should panic, are run in a separate thread so we can check the panic category and code
	RThread thread;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNamePrefix, "SsmTestThread");
	RBuf threadName;
	CleanupClosePushL(threadName);
	threadName.CreateL(KThreadNamePrefix().Length() + 4); // 4 digit thread number
	threadName = KThreadNamePrefix;
	threadName.AppendNumFixedWidth(aFunction, EDecimal, 4);
	TThreadParams params(*this, aFunction);
	const TInt KMinHeapSize = 0xc800; // 50k - NOTE just an arbitrary value, please feel free to change it
	const TInt KMaxHeapSize = 0x19000; // 100k - NOTE just an arbitrary value, please feel free to change it
	User::LeaveIfError(thread.Create(threadName, ThreadStartFn, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, &params));
	CleanupStack::PopAndDestroy(&threadName);
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TExitCategoryName category = thread.ExitCategory();
	RDebug::Print(_L("***** Exit Category = %S *****"),&category);
	INFO_PRINTF2(_L("***** Exit Category = %S *****"), &category);
	TEST(category.Compare(KPanicCmdResourceReader) == 0); // always expecting a resource reader panic
	const TInt exitReason = thread.ExitReason();
	thread.Close();
	return exitReason;
	}

TInt CResourceReaderTest::ThreadStartFn(TAny* aPtr)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}
	
	TThreadParams& threadParams = *(reinterpret_cast<TThreadParams*>(aPtr));
	TRAPD(err, threadParams.iTestStep.ThreadDoTestL(threadParams.iFunction));

	delete trapCleanup;
	return err;
	}

void CResourceReaderTest::ThreadDoTestL(TThreadParams::TFunction aFunction)
	{
	CActiveSchedulerWithRunLCount* activeScheduler = new(ELeave) CActiveSchedulerWithRunLCount;
	CleanupStack::PushL(activeScheduler);
	CActiveScheduler::Install(activeScheduler);
	CAsyncCallBack* threadAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopSchedulerCallBack, NULL);
	threadAsyncStopScheduler->Set(stop);
	CleanupStack::Pop(activeScheduler);

	iThreadActiveScheduler = activeScheduler;
	iThreadAsyncStopScheduler = threadAsyncStopScheduler;

	TRAPD(err, ThreadDispatchFunctionL(aFunction));
	
	delete iThreadAsyncStopScheduler;
	iThreadAsyncStopScheduler = NULL;
	delete iThreadActiveScheduler;
	iThreadActiveScheduler = NULL;
	
	User::LeaveIfError(err);
	}

void CResourceReaderTest::ThreadDispatchFunctionL(TThreadParams::TFunction aFunction)
	{
	switch (aFunction)
		{
		case TThreadParams::EThreadInitialise3:
			ThreadInitialise3L();
			break;
		case TThreadParams::EThreadInitialise6:
			ThreadInitialise6L();
			break;
		case TThreadParams::EThreadInitialise7:
			ThreadInitialise7L();
			break;
		case TThreadParams::EThreadInitialise8:
			ThreadInitialise8L();
			break;
		case TThreadParams::EThreadInitialise9:
			ThreadInitialise9L();
			break;
		case TThreadParams::EThreadGet1:
			ThreadGet1L();
			break;
		case TThreadParams::EThreadGet2:
			ThreadGet2L();
			break;
		case TThreadParams::EThreadGet3:
			ThreadGet3L();
			break;
		case TThreadParams::EThreadPrepare16:
			ThreadPrepare16L();
			break;
		case TThreadParams::EThreadGetCommandListIds1:
			ThreadGetCommandListIds1L();
			break;
		default:
			TEST(EFalse); // invalid TFunction provided
			break;
		}
	}


void CResourceReaderTest::AppendL(MSsmCommand* /*aCmd*/)
	{
	}

TArray<MSsmCommand*> CResourceReaderTest::PendingDeferred() const
	{
	RPointerArray<MSsmCommand> array;
	return array.Array();
	}

void CResourceReaderTest::SetDelayBetweenCommands(TInt aDelayBetweenCommands)
	{
	iDelay = aDelayBetweenCommands;
	}

void CResourceReaderTest::SetResourceFileNameL(const TDesC& /*aFileName*/)
	{
	
	}

void CResourceReaderTest::SetConditionalCallback(MSsmConditionalCallback& /*aCallBack*/)
	{
	
	}

CSsmCommandBase* CResourceReaderTest::ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters)
	{
	switch (iWhichPrepare)
		{
	case EPrepareNoCheck: // don't perform any checks
		break;
	case EPrepareState:
		TEST(aCommandParameters.StateL().Int() == 23);
		break;
	case EPrepareSwp:
		TEST(aCommandParameters.SwpL() == TSsmSwp(KSwpKey, 24));
		break;
	case EPrepareLeave:
		INFO_PRINTF1(_L("Leaving from CResourceReaderTest::ConstructCommandFromResourceLC with KErrGeneral"));
		User::Leave(KErrGeneral);
		break;
	case EConditional:
		++iCommandConstructCount;
		break;
	case EPrepareType:
		{
		RResourceReader& reader = aCommandParameters.MainReader();
		const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
		TEST(type == aType);
		}
		break;
	default:
		User::Invariant(); // should never get here
		break;
		}
	
	CSsmCommandBase* cmd = NULL;
	CleanupStack::PushL(cmd);
	return NULL;
	}

/*
It tests for the scenario where first file have no commands(i.e. cmdlists7a) and second file cmdlists7b have two commands and
third file have no commands(i.e cmdlists7c)

CommandList should be created and iCommandConstructCount should be two.
*/
void CResourceReaderTest::TestGet5L()
	{
	INFO_PRINTF1(_L("*** Starting TestGet5L"));
	_LIT(KCommandListPath, "z:\\resource\\ssmatest\\cmdlists7\\");
	CSsmCommandListResourceReaderImpl* reader = CSsmCommandListResourceReaderImpl::NewL(iFs, KCommandListPath, *this);

	// init
	TRequestStatus status;
	reader->Initialise(status);
	TEST(status == KRequestPending);
	Run();
	User::WaitForRequest(status); // mop up the completed initialisation request
	TEST(status == KErrNone);

	// prepare
	TSsmState state(0,23);
	reader->PrepareCommandList(*this, ETestCommandListId1, state, status);
	iWhichPrepare = EConditional;
	iCommandConstructCount = 0;
	Run();
	User::WaitForRequest(status);
		
	TBool commandListReady =  reader->IsCommandListReady();
	
	TEST((status == KErrNone) && (commandListReady == TRUE) && (iCommandConstructCount == 2));
		
	iWhichPrepare = EPrepareNoCheck;
	delete reader;
	}
