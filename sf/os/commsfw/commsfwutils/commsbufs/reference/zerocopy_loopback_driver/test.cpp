/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>

#include <e32def_private.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <comms-infras/zerocopy_loopback_driver.h>
#else
//this header is not exported, needs to be a user include
#include "zerocopy_loopback_driver.h"
#endif

#include <comms-infras/commsbuf.h>
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufpondop.h>

_LIT8(KTestSendData,"abcdefghijklmnopqrstuvwxyz");
static const TInt KMBuf_MaxAvail = 393216;
static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_MBufSizeBig = 1600;
static const TInt KMBuf_InitialAllocation = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;

static const TInt KOneSecond = 1000000;


class CZCDriverTestServer : public CTestServer
	{
public:
	static CZCDriverTestServer* NewL();
	~CZCDriverTestServer();

protected:
	const TPtrC ServerName() const;
	CTestStep* CreateTestStep(const TDesC& aStepName);
	};


_LIT(KZCDriverTestName, "zc_driver_test");

class CZCDriverTestStep : public CTestStep
	{
public:

	CZCDriverTestStep()
		{
		SetTestStepName(KZCDriverTestName);
		}

protected:
	enum TVerdict doTestStepL();

	enum TVerdict doTestStepPreambleL()
		{
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	};

#define TEST_VERDICT(test) if(test) {SetTestStepResult(EFail);}

enum TVerdict CZCDriverTestStep::doTestStepL()
	{
	CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);
	CActiveScheduler::Install(activeScheduler);

	SetTestStepResult(EFail);

	// Start of testing
	INFO_PRINTF1(_L("Load Physical Device"));
	TInt r;
	r=User::LoadPhysicalDevice(KZeroCopyLoopbackPddFileName);
	TEST_VERDICT(r==KErrNone || r==KErrAlreadyExists);

	INFO_PRINTF1(_L("Load Logical Device"));
	r=User::LoadLogicalDevice(KZeroCopyLoopbackLddFileName);
	TEST_VERDICT(r==KErrNone || r==KErrAlreadyExists);

    // Initialize a commsbuf pond
    RCommsBufPondOp commsBufPond;
    RArray<TCommsBufPoolCreateInfo> createInfos;
    TCommsBufPoolCreateInfo poolInfoForLargeBufs;
    poolInfoForLargeBufs.iBufSize = KMBuf_MBufSizeBig;
    poolInfoForLargeBufs.iInitialBufs = KMBuf_InitialAllocation;
    poolInfoForLargeBufs.iGrowByBufs = KMBuf_MinGrowth;
    poolInfoForLargeBufs.iMinFreeBufs = KMBuf_GrowthThreshold;
    poolInfoForLargeBufs.iCeiling = (KMBuf_MaxAvail/2) / KMBuf_MBufSize;    
    createInfos.AppendL(poolInfoForLargeBufs);
    TCommsBufPoolCreateInfo poolInfoForSmallBufs;
    poolInfoForSmallBufs.iBufSize = KMBuf_MBufSize;
    poolInfoForSmallBufs.iInitialBufs = KMBuf_InitialAllocation;
    poolInfoForSmallBufs.iGrowByBufs = KMBuf_MinGrowth;
    poolInfoForSmallBufs.iMinFreeBufs = KMBuf_GrowthThreshold;
    poolInfoForSmallBufs.iCeiling = (KMBuf_MaxAvail/2) / KMBuf_MBufSize;
    createInfos.AppendL(poolInfoForSmallBufs);
	User::LeaveIfError(commsBufPond.Open(createInfos));
	createInfos.Close();
	
	__KHEAP_MARK;

	INFO_PRINTF1(_L("Open Device"));
	RDevice device;
	r=device.Open(RZeroCopyLoopbackDriver::Name());
	TEST_VERDICT(r==KErrNone);

	INFO_PRINTF1(_L("Get Device Capabilities"));
	RZeroCopyLoopbackDriver::TCaps caps;
	TPckg<RZeroCopyLoopbackDriver::TCaps>capsPckg(caps);
	capsPckg.FillZ(); // Zero 'caps' so we can tell if GetCaps has really filled it
	device.GetCaps(capsPckg);
	TVersion expectedVer(RZeroCopyLoopbackDriver::VersionRequired());
	TEST_VERDICT(caps.iVersion.iMajor==expectedVer.iMajor);
	TEST_VERDICT(caps.iVersion.iMinor==expectedVer.iMinor);
	TEST_VERDICT(caps.iVersion.iBuild==expectedVer.iBuild);

	INFO_PRINTF1(_L("Close Device"));
	device.Close();

	INFO_PRINTF1(_L("Open Logical Channel"));
	RZeroCopyLoopbackDriver ldd;
	r=ldd.Open();
	TEST_VERDICT(r==KErrNone);

	INFO_PRINTF1(_L("GetConfig"));
	RZeroCopyLoopbackDriver::TConfigBuf configBuf;
	configBuf.FillZ();   // Zero 'config' so we can tell if GetConfig has really filled it
	r=ldd.GetConfig(configBuf);
	TEST_VERDICT(r==KErrNone);

	RZeroCopyLoopbackDriver::TConfig& config=configBuf();
	TEST_VERDICT(config.iPddBufferSize!=0);
	TEST_VERDICT(config.iMaxSendDataSize!=0);
	TEST_VERDICT(config.iMaxReceiveDataSize!=0);

	INFO_PRINTF1(_L("SetConfig"));
	TInt speed = configBuf().iSpeed+1;
	configBuf().iSpeed = speed;
	r=ldd.SetConfig(configBuf); // Use SetConfig to change speed
	TEST_VERDICT(r==KErrNone);

	configBuf.FillZ();
	r=ldd.GetConfig(configBuf);
	TEST_VERDICT(r==KErrNone);
	TEST_VERDICT(configBuf().iSpeed==speed);
	TRequestStatus status;

	INFO_PRINTF1(_L("Check access by wrong client"));
	RZeroCopyLoopbackDriver ldd2=ldd;
	r=ldd2.Duplicate(RThread(),EOwnerProcess);
	TEST_VERDICT(r==KErrAccessDenied);

	INFO_PRINTF1(_L("Check handle duplication"));
	ldd2=ldd;
	r=ldd2.Duplicate(RThread(),EOwnerThread);
	TEST_VERDICT(r==KErrNone);
	ldd2.Close();

	INFO_PRINTF1(_L("Load pond"));
	r = ldd.LoadPond(commsBufPond);
	TEST_VERDICT(r == KErrNone);

	INFO_PRINTF1(_L("Unload pond"));
	r = ldd.UnloadPond();
	TEST_VERDICT(r == KErrNone);
	
	INFO_PRINTF1(_L("Reload pond"));
	r = ldd.LoadPond(commsBufPond);
	TEST_VERDICT(r == KErrNone);

	INFO_PRINTF1(_L("SendData"));
	TCommsBufAllocator allocator = commsBufPond.Allocator();
	RCommsBuf* buf = RCommsBuf::Alloc(KTestSendData().Length(), allocator);
	TEST_VERDICT(buf != NULL);
	buf->Reset();
	buf->Append(KTestSendData);
	ldd.SendData(status, *buf);
	User::WaitForRequest(status);
	r = status.Int();
	TEST_VERDICT(r == KErrNone);
	buf = NULL;

	INFO_PRINTF1(_L("SendData"));
	buf = RCommsBuf::Alloc(KTestSendData().Length(), allocator);
	TEST_VERDICT(buf != NULL);
	buf->Reset();
	buf->Append(KTestSendData);
	ldd.SendData(status, *buf);
	User::WaitForRequest(status);
	r=status.Int();
	TEST_VERDICT(r == KErrNone);
	buf = NULL;

	INFO_PRINTF1(_L("ReceiveData"));
	RCommsBuf* rxbuf;
	ldd.ReceiveData(status);
	User::WaitForRequest(status);
	r=status.Int();
	TEST_VERDICT(r > 0);
	rxbuf = commsBufPond.FromHandle(r);
	TEST_VERDICT(rxbuf != 0);
	TEST_VERDICT(rxbuf->Length() == KTestSendData().Size());
	TEST_VERDICT(rxbuf->DesC8() == KTestSendData);
	TBuf8<256> bufdata;
	rxbuf->Read(bufdata);
	TEST_VERDICT(bufdata == KTestSendData);
	rxbuf->Free();
	rxbuf = NULL;

	INFO_PRINTF1(_L("ReceiveData"));
	ldd.ReceiveData(status);
	User::WaitForRequest(status);
	r=status.Int();
	TEST_VERDICT(r > 0);
	rxbuf = commsBufPond.FromHandle(r);
	TEST_VERDICT(rxbuf != 0);
	TEST_VERDICT(rxbuf->Length() == KTestSendData().Size());
	TEST_VERDICT(rxbuf->DesC8() == KTestSendData);
	rxbuf->Read(bufdata);
	TEST_VERDICT(bufdata == KTestSendData);
	rxbuf->Free();
	rxbuf = NULL;

	INFO_PRINTF1(_L("ReceiveDataCancel"));
	ldd.ReceiveData(status);
	User::After(KOneSecond);
	ldd.ReceiveDataCancel();
	User::WaitForRequest(status);
	r=status.Int();
	TEST_VERDICT(r == KErrCancel);

#if 0
	INFO_PRINTF1(_L("SendDataCancel"));
	ldd.SendDataCancel();
	User::WaitForRequest(status);
	r=status.Int();
	TEST_VERDICT(r == KErrNone);
	buf = NULL;
#endif
	
	INFO_PRINTF1(_L("Unload pond"));
	r = ldd.UnloadPond();
	TEST_VERDICT(r == KErrNone);
	
	INFO_PRINTF1(_L("Close Logical Channel"));
	ldd.Close();

	__KHEAP_MARKEND;

	INFO_PRINTF1(_L("Unload Logical Device"));
	r=User::FreeLogicalDevice(RZeroCopyLoopbackDriver::Name());
	TEST_VERDICT(r==KErrNone);

	INFO_PRINTF1(_L("Unload Physical Device"));
	TName pddName(RZeroCopyLoopbackDriver::Name());
	_LIT(KVariantExtension,".template");
	pddName.Append(KVariantExtension);
	r=User::FreePhysicalDevice(pddName);
	TEST_VERDICT(r==KErrNone);

	// Close the pond
	commsBufPond.Close();
	
	// End of testing
	CleanupStack::PopAndDestroy(activeScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


CZCDriverTestServer* CZCDriverTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CZCDriverTestServer* server = new (ELeave) CZCDriverTestServer();
	CleanupStack::PushL(server);
	server->StartL(server->ServerName());
	CleanupStack::Pop(server);
	return server;
	}


CZCDriverTestServer::~CZCDriverTestServer()
	{
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CZCDriverTestServer* server = NULL;

	// Create the CTestServer derived server
	TRAPD(err,server = CZCDriverTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}

	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CZCDriverTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName.Compare(KZCDriverTestName) == 0)
		testStep =new CZCDriverTestStep();

	return testStep;
	}

const TPtrC CZCDriverTestServer::ServerName() const
	{
#if (!defined EKA2)
	// On EKA1 our test server is loaded as a dll.
	// So we cannot rely on the process name
	return KServerName();
#else
	// On EKA2, test server runs in its own process.
	// So we arrive at the server name using the exe from which it is loaded.
	// This is useful when doing cap tests, as we create duplicate exe's using setcap then.
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
#endif
	}

