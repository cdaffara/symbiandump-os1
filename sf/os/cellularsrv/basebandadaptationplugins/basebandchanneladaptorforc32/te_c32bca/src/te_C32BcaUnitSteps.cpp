// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// te_c32BcaUnitTestSteps.cpp
// Implements the unit test classes for C32Bca.
// 
//

/**
 @file
 @internalComponent
*/


#include "te_c32BcaUnitSteps.h"
#include "BcaTestProxy.h"
#include <c32comm.h> // uniquely for the call to StartC32WithCMISuppressions

#include <commsdattypesv1_1.h>  // required to create a record
using namespace CommsDat;       // in the CommDB database
using namespace te_c32BcaUnitTests; // all C32Bca tests



/** BCA version type */
typedef TUint32 TBcaVersion;

/** Special error code for test that require a leaving function.
This code is not used by any of Symbian system components */
static const TInt KNonSystemErrCode(-9876);
_LIT8(KWriteBuf1Lit, "WriteBuffer_1");
_LIT8(KWriteBuf2Lit, "WriteBuffer_2");

/**
 C++ constructor 
 */
CC32BcaTestBase::CC32BcaTestBase()
	: CTestStep(),
	iUut(NULL),
	iPeer(NULL),
	iWriteBuf1(KWriteBuf1Lit),
	iWriteBuf2(KWriteBuf2Lit)	
	{	
	// Ioctl packaging:
	iWritePort1BufPckg = iWritePort1Buf;
	iReadPort1BufPckg  = iReadPort1Buf;
	}

/**
C++ destructor */
CC32BcaTestBase::~CC32BcaTestBase()
	{
	iCommPort.Close();
	iCommServ.Close();
	
	if(iUut)
		{
		iUut->Release();
		}
		
	
	if(iPeer)
		{
		iPeer->Release();
		}
		
	}

/**
 Implements the pure virtual doTestStepPreambleL defined in CTestStep. 
 Used to disable the phone book synchronizer
 which may cause the tests to fail.
 
 
 @return EPass if successful.
 @leave If Phonebook synchronizer disabling fails. 
 */
TVerdict CC32BcaTestBase::doTestStepPreambleL()
	{	
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	TESTL(KErrNone == err || KErrAlreadyExists == err);
	INFO_PRINTF1(_L("Test Step Preamble: disabled Phonebook Synchronizer."));	
	return EPass;
	}
	
/**
 Implements the pure virtual doTestStepL defined in CTestStep. 
 Allows the base class to execute before any of the derived 
 tests is called.
 
 @leave If any of the called methods leaves.
 */
TVerdict CC32BcaTestBase::doTestStepL()
	{
	__UHEAP_MARK;
	TVerdict testResult = EFail;
	
	TRAPD(
		configErr, 
		LoadBcaSettingsFromConfigFileL();
		LoadBcaIapSettingsFromConfigFileL()
		);
	if(KErrNone != configErr)
		{
		ERR_PRINTF1(_L("Test Step Failure: Could not load Configuration Settings"));
		SetTestStepResult(EAbort);
		return TestStepResult();		
		}
	
	ASSERT(!CActiveScheduler::Current()); // We should not have an AS at this point.
	CActiveScheduler* testSched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(testSched);
	CActiveScheduler::Install(testSched);
			
	TRAPD(err, testResult = RunTestStepL());
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Test Step Completion."));
		SetTestStepResult(testResult);
		}
	else
		{
		ERR_PRINTF2(_L("Test Step Failure: left with [%d]"), err);
		TestEnvironmentCleanup(); // Make sure test code has released everything. 
		SetTestStepResult(EFail);
		}
	
	
	CActiveScheduler::Install(NULL); // uninstall the test scheduler
	CleanupStack::PopAndDestroy(testSched);
		
	// Heap checking targets incorrect memory handling in the UUT, 
	// rather than in the test code.
	// It is Ok for the test code to leave without releasing memory - 
	// cleanup is forced if the test step leaves.
	// Heap checking should fail if UUT was cleaned up, but not all
	// memory was released by the owned objects.		
	__UHEAP_MARKEND; // Check the Heap after test code is finished.
	
	return TestStepResult();	
	}
	
/**
Makes sure that the test environment has released all the resources */
void CC32BcaTestBase::TestEnvironmentCleanup()
	{
	if(iUut)
		{
		ERR_PRINTF1(_L("Test Environment cleanup: unloading UUT."));
		iUut->Release();
		iUut = NULL;
		}
	
	if(iPeer)
		{
		ERR_PRINTF1(_L("Test Environment cleanup: unloading Peer."));
		iPeer->Release();
		iPeer = NULL;
		}		 
	}


/** 
Read optional IAP settings from Config file 

@leave if settings could not be loaded.
*/
void CC32BcaTestBase::LoadBcaIapSettingsFromConfigFileL()
	{
	TInt uutIap(0); // 0 is an invalid value for IAP
	TInt peerIap(0);
	
	if(	!GetIntFromConfig(ConfigSection(), _L("UUT_IapId"), uutIap) ||
		!GetIntFromConfig(ConfigSection(), _L("Peer_IapId"), peerIap)) 
		{
		ERR_PRINTF2(_L("FAILED to read one of IAP Settings from config file. Leaving with KErrNotFound[%d]"), KErrNotFound);
		User::Leave(KErrNotFound);	
		}
	iUutIapId = static_cast<TUint32>(uutIap);
	iPeerIapId = static_cast<TUint32>(peerIap);
	}

/** 
Reads test configuration file and applies the settings 

@leave if the settings could not be loaded.
*/	
void CC32BcaTestBase::LoadBcaSettingsFromConfigFileL()
	{
	if(
		!GetStringFromConfig(ConfigSection(), _L("UUT_Name"),      iUutName) ||
		!GetStringFromConfig(ConfigSection(), _L("Peer_Name"), 	   iPeerName) ||
		!GetStringFromConfig(ConfigSection(), _L("UUT_PortName"),  iUutPortName) ||
		!GetStringFromConfig(ConfigSection(), _L("Peer_PortName"), iPeerPortName))
		{
		ERR_PRINTF2(_L("FAILED to read one of BCA Settings (BcaCSY_Name, UUT_PortName, etc) from config file. Leaving with KErrNotFound[%d]"), KErrNotFound);
		User::Leave(KErrNotFound);	
		}
	INFO_PRINTF5(_L("BCA Settings: %S Port[%S], %S Port[%S]"), &iUutName, &iUutPortName, &iPeerName, &iPeerPortName);
	}
	
void CC32BcaTestBase::LoadCsyNameFromConfigFileL()
	{
	if(!GetStringFromConfig(ConfigSection(), _L("BcaCSY_Name"),   iCsyName)) 
		{
		ERR_PRINTF2(_L("FAILED to read CSY name from config file. Leaving with KErrNotFound[%d]"), KErrNotFound);
		User::Leave(KErrNotFound);	
		}
	INFO_PRINTF2(_L("BCA Settings: CSY[%S]"), &iCsyName);
	}


/**
Load the Comm Role to be used from test configuration file 

@return the Comm role 
@leave if the parameter is missing or cannot be loaded
*/
TCommRole CC32BcaTestBase::LoadExpectedCommRoleFromConfigFileL()
	{
	TPtrC expectedCommRoleStr;
	if(!GetStringFromConfig(ConfigSection(), _L("ExpectedCommRole"), expectedCommRoleStr))
		{
		INFO_PRINTF1(_L("Failed to read ExpectedCommRole from Config. Test is Aborted."));
		User::Leave(KErrNotFound);
		}
		
	TCommRole expectedCommRole(ECommRoleDTE);
	if(expectedCommRoleStr == _L("DTE"))
		{
		expectedCommRole = ECommRoleDTE;
		}
	else if(expectedCommRoleStr == _L("DCE"))
		{
		expectedCommRole = ECommRoleDCE;
		}
	else
		{
		INFO_PRINTF2(_L("Unrecognized comm role[%S]. Test is Aborted."), &expectedCommRoleStr);
		User::Leave(KErrArgument);
		}
		
	return expectedCommRole;	
	}

/**
Executes duplex packet exchange between the UUT and Peer:
UUT writes, Peer reads. Peer writes, UUT reads.

@param aIterations number of packet exchanges.
@leave if there is a failure on one of the operations.
*/
void CC32BcaTestBase::DoDuplexReadWriteL(TInt aIterations)
	{
	ASSERT(iUut);
	ASSERT(iPeer);
	
	for(TInt iter = 0; iter < aIterations; ++iter)
		{
		INFO_PRINTF2(_L("======Duplex Read-Write iteration[%d] begin"), iter);
		
		iReadBuf1.Zero();
		iUut->Read(iReadBuf1);
		iUut->Write(iWriteBuf2);
		
		iReadBuf2.Zero();
		iPeer->Read(iReadBuf2);
		iPeer->Write(iWriteBuf1);
		
		iPeer->WaitForWriteL(KErrNone);
		iUut->WaitForReadL(KErrNone);
		
		iUut->WaitForWriteL(KErrNone);
		iPeer->WaitForReadL(KErrNone);

		CheckBuffersEqualL(iReadBuf2, iWriteBuf2);	
		CheckBuffersEqualL(iReadBuf1, iWriteBuf1);
		
		INFO_PRINTF2(_L("======Duplex Read-Write iteration[%d] end"), iter);
		}
	}
	

/**
Logs a message and leaves on error 

@param aErrCode error code to check
@param aMsg message to log 
@leave if aError is other than KErrNone
*/
void CC32BcaTestBase::LogAndLeaveIfErrorL(TInt aErrCode, const TDesC& aMsg)
	{	
	if(KErrNone == aErrCode)
		{
		INFO_PRINTF3(_L("[%S]. err[%d]. OK."), &aMsg, aErrCode);
		}
	else
		{
		ERR_PRINTF4(_L("Failed: [%S]. err[%d]. Leaving with [%d])."), &aMsg, aErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	}

/**
Loads UUT proxy 

@leave if UUT cannot be loaded */
void CC32BcaTestBase::LoadUutL()
	{
	INFO_PRINTF1(_L("Loading UUT"));
	ASSERT(NULL == iUut);
	
	iUut = CC32BcaProxy::NewL(*this, iUutName);
	
	ASSERT(iUut);
	INFO_PRINTF1(_L("UUT Loaded"));	
	}

/**
Unloads UUT 

*/
void CC32BcaTestBase::UnloadUut()
	{
	INFO_PRINTF1(_L("Unloading UUT"));
	ASSERT(iUut);
	
	iUut->Release();
	iUut = NULL;
	INFO_PRINTF1(_L("UUT Unloaded"));
	}

/**
Loads Peer (to the UUT )

@leave if Peer cannot be loaded */
void CC32BcaTestBase::LoadPeerL()
	{
	INFO_PRINTF1(_L("Loading Peer"));
	ASSERT(NULL == iPeer);
	
	iPeer = CC32BcaProxy::NewL(*this, iPeerName); 
	
	ASSERT(iPeer);
	INFO_PRINTF1(_L("Peer Loaded"));
	}
	
/**
Unloads Peer */
void CC32BcaTestBase::UnloadPeer()
	{
	INFO_PRINTF1(_L("Unloading Peer"));
	ASSERT(NULL != iPeer);
	
	iPeer->Release();
	iPeer = NULL;
	INFO_PRINTF1(_L("Peer Unloaded"));	
	}
	
/**
Checks if the two buffers are equal, for TDesC16

@param aBuf1 Buffer 1
@param aBuf2 Buffer 2

@leave if the buffers are not equal */ 	
void CC32BcaTestBase::CheckBuffersEqualL(const TDesC16& aBuf1,const TDesC16& aBuf2)
	{
	INFO_PRINTF3(_L("Buffer Equality: [%S]-[%S]"), &aBuf1, &aBuf2);
	if(aBuf1 != aBuf2)
		{
		User::Leave(KErrArgument);
		}
	INFO_PRINTF1(_L("Buffer Equality Check OK."));	
	}

/**
Checks if the two buffers are equal, for TDesC8

@param aBuf1 Buffer 1
@param aBuf2 Buffer 2

@leave if the buffers are not equal */ 		
void CC32BcaTestBase::CheckBuffersEqualL(const TDesC8& aBuf1,const TDesC8& aBuf2)
	{
	if(aBuf1 != aBuf2)
		{
		INFO_PRINTF1(_L("Buffer Equality Check Failed."));
		User::Leave(KErrArgument);
		}
	INFO_PRINTF1(_L("Buffer Equality Check OK."));		
	}


/**
Sets a CSY name in a BCA 

@param aProxy the BCA proxy 
@param aCsy CSY name
@leave on failure */
void CC32BcaTestBase::SetProxyCsyL(MBcaProxy& aProxy, const TDesC& aCsy)
	{
	// Ioctl: Set CSY Name
	TPckgBuf<TPortNameBuf> csyNamePckg(aCsy);
	aProxy.Ioctl(KBcaOptLevelExtSerial, KSerialSetCsyName, csyNamePckg);
	aProxy.WaitForIoctlL(KErrNone);	
	}

/**
Sets a CSY name in a BCA 

@param aProxy the BCA proxy 
@param aCsy CSY name
@leave on failure */
void CC32BcaTestBase::SetProxyIapIdL(MBcaProxy& aProxy, TUint32 aIapId)
	{
	// Ioctl: Set Iap Id
	TPckgBuf<TUint32> commDbIapIdPckg(aIapId);
	aProxy.Ioctl(KBcaOptLevelGeneric, KBCASetIapId, commDbIapIdPckg);
	aProxy.WaitForIoctlL(KErrNone);	
	}
	
/**
Executes BCA::Open

@param aProxy the BCA proxy
@param aPortName the port name 
@leave on failure */
void CC32BcaTestBase::OpenProxyL(MBcaProxy& aProxy, const TDesC& aPortName)
	{	
	SetProxyCsyL(aProxy, iCsyName);
	aProxy.Open(aPortName);
	aProxy.WaitForOpenL(KErrNone);	
	}
	
/**
Executes BCA::Open

@param aProxy the BCA proxy
@param aPortName the port name 
@leave on failure */
void CC32BcaTestBase::OpenProxyL(MBcaProxy& aProxy, TUint32 aIapId, const TDesC& aPortName)
	{
	SetProxyIapIdL(aProxy, aIapId);
	aProxy.Open(aPortName);
	aProxy.WaitForOpenL(KErrNone);	
	}

	
/**
Loads and opens UUT and Peer 

@leave on failure */	
void CC32BcaTestBase::LoadOpenUutAndPeerL()
	{
	LoadUutL();
	OpenProxyL(*iUut, iUutIapId, iUutPortName);
	
	LoadPeerL();
	OpenProxyL(*iPeer, iPeerIapId, iPeerPortName);
	}
	
/**
Closes and unloads UUT and Peer 

@leave on failure */
void CC32BcaTestBase::ShutdownUnloadUutAndPeerL()
	{
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();
	}

/**
Loads C32 related settings from test configuration file 

@leave if the parameters are missing
*/
void CC32BcaTestBase::LoadC32SettingsFromConfigFileL()
	{
	if(
	!GetStringFromConfig(ConfigSection(), _L("PDD_Name"),  iC32PddName) ||
	!GetStringFromConfig(ConfigSection(), _L("LDD_Name"),  iC32LddName) ||
	!GetStringFromConfig(ConfigSection(), _L("CSY_Name"),  iC32CsyName) ||
	!GetStringFromConfig(ConfigSection(), _L("Port_Name"), iC32PortName)
	)
		{
		ERR_PRINTF2(_L("FAILED to read one of C32 Settings (PDD_Name, LDD_Name, etc) from config file. Leaving with KErrNotFound[%d]"), KErrNotFound);
		User::Leave(KErrNotFound);
		}
	INFO_PRINTF5(_L("C32 Settings: PDD[%S], LDD[%S], CSY[%S], Port[%S]"), &iC32PddName, &iC32LddName, &iC32CsyName, &iC32PortName);
	}

/**
Opens a real C32 Comm Port, as opposed to the BCA's one 

@param aCommPort the port
@param aCsyName the CsyName to open the port on
@param aPortName the port name 
@leave on failure */
void CC32BcaTestBase::OpenRealCommPortL(RComm& aCommPort, const TDesC& aCsyName, const TDesC& aPortName, TCommRole aCommRole)
	{	
	INFO_PRINTF3(_L("Opening RCOMM: CSY name[%S], port name[%S]."), &aCsyName, &aPortName);
	
	if(aCsyName == _L("ECUART"))
		{
		ForceLoadC32LddAndPdd(iC32LddName, iC32PddName);
		}
		
	LogAndLeaveIfErrorL(iCommServ.Connect(), _L("Connection to RCommServ"));
	LogAndLeaveIfErrorL(iCommServ.LoadCommModule(aCsyName), _L("Loading Comms Module"));
	LogAndLeaveIfErrorL(aCommPort.Open(iCommServ, aPortName, ECommShared, aCommRole), _L("Opening Port"));
	}

/** Force loading of Device drivers, if necessary 

@param aLddName Logical Device Driver
@param aPddName Physical Device Driver */
void CC32BcaTestBase::ForceLoadC32LddAndPdd(const TDesC& aLddName, const TDesC& aPddName)
	{		
	TInt err = User::LoadLogicalDevice(aLddName);
	if(KErrNone != err && KErrAlreadyExists != err)
		{
		WARN_PRINTF2(_L("FAILED loading LDD [%S]. This is not fatal, the tests can succeed."), &aLddName);
		}
	else
		{
		INFO_PRINTF3(_L("Force loaded LDD [%S] with error [%d] "), &aLddName, err);
		}	
	
	err = User::LoadPhysicalDevice(aPddName);
	if(KErrNone != err && KErrAlreadyExists != err)
		{
		WARN_PRINTF2(_L("FAILED loading PDD [%S]. This is not fatal, the tests can succeed."), &aPddName);
		}
	else
		{
		INFO_PRINTF3(_L("Force loaded PDD [%S] with error [%d] "), &aPddName, err);	
		}
	}
	
/**	
Test C32 Bca when it is closed.	
*/
TVerdict CClosedState1::RunTestStepL()
	{
	LoadUutL();	
	
	// Ioctl: Set Iap ID for the proxy
	SetProxyIapIdL(*iUut, iUutIapId);
	
	// Open
	iUut->Open(iUutPortName);
	iUut->WaitForOpenL(KErrNone);
	
	// Shutdown
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	UnloadUut();

	
	return EPass;
	}


/** 
Tests C32 BCA when it is closed 
Verifies that BCA can be closed and reopened
*/
TVerdict CClosedState2::RunTestStepL()
	{
	
	LoadOpenUutAndPeerL();
		
	iReadBuf1.Zero();
	iUut->Write(iWriteBuf1);
	iPeer->Read(iReadBuf1);
	
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iReadBuf1, iWriteBuf1);
	
	
	// Close: must close peer, else loopback fails to rebind ports
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
		
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	iPeer->Open(iPeerPortName);
	iPeer->WaitForOpenL(KErrNone);
	
	
	iUut->Open(iUutPortName);
	iUut->WaitForOpenL(KErrNone);
	
	// Write - Read: did it really open? 
	iUut->Write(iWriteBuf1);
	iPeer->Read(iReadBuf1);
	
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
	
	
	ShutdownUnloadUutAndPeerL();	
	
	return EPass;
	}

/**
Test C32 BCA when it is shutting down 
Shutdown followed by Close
*/
enum TVerdict CShutdownState1::RunTestStepL()
	{
	LoadUutL();
	
	OpenProxyL(*iUut, iUutIapId, iUutPortName);
		
	iUut->Shutdown();
	iUut->Close(); // don't wait for shutdown to complete
	
	UnloadUut();
	
	return EPass;
	}


/**
Tests C32 BCA when it is open
*/
TVerdict COpenState1::RunTestStepL()
	{
	LoadUutL();
	LoadPeerL();
	
	// Set IAP Ids.	
	SetProxyIapIdL(*iUut, iUutIapId);	
	SetProxyIapIdL(*iPeer, iPeerIapId);
	
	// Open	
	iUut->Open(iUutPortName);
	iPeer->Open(iPeerPortName);
	
	iUut->WaitForOpenL(KErrNone);
	iPeer->WaitForOpenL(KErrNone);
	
	
	// Write - Read 
	iUut->Write(iWriteBuf1);
	iPeer->Read(iReadBuf1);
	
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
	
	
	// Read - Write
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iPeer->Write(iWriteBuf1);
	
	iUut->WaitForWriteL(KErrNone);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
		
	// Cancel Read, Write on no reads / writes outstanding
	iUut->CancelRead();
	iUut->CancelWrite();
	
	// cancel control: should not fail catastrophically
	iUut->CancelIoctl();
	
		
	ShutdownUnloadUutAndPeerL();


	return EPass;
	}
	

/**
Tests C32 BCA when it is opened: 
unloads BCA when it is in opened state.
*/
TVerdict COpenState2::RunTestStepL()
	{
	LoadUutL();
	
	OpenProxyL(*iUut, iUutIapId, iUutPortName);
	
	// Unload without closing
	UnloadUut();
	
	return EPass;
	}
	
/**
Tests C32 BCA when it reading
*/	
TVerdict CReadingState1::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	iUut->Read(iReadBuf1);
	
	// Peer: do not write
	
	iUut->CancelWrite(); 
	
	iUut->CancelIoctl();
	
	// Cancel Read:
	iUut->CancelRead();
	//iUut->WaitForReadL(KErrCancel);
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	
	// Close on pending read
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	
	// Verify that we did not crash anything, reopen:
	INFO_PRINTF1(_L("==========>Reopening UUT and Peer"));
	OpenProxyL(*iUut, iUutIapId, iUutPortName);
	OpenProxyL(*iPeer, iPeerIapId, iPeerPortName);
	DoDuplexReadWriteL(1);
	ShutdownUnloadUutAndPeerL();
		
	return EPass;
	}
	
/**
Tests C32 BCA when it is reading 

*/
TVerdict CReadingState2::RunTestStepL()
	{
    LoadOpenUutAndPeerL();

	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	// Peer: do not write
	
	iUut->CancelWrite();
	
	iPeer->Write(iWriteBuf1);
	iPeer->WaitForWriteL(KErrNone);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	// Don't complete the read
	
	ShutdownUnloadUutAndPeerL();

	// Allow C32 to recover. If we do not allow that,
	// the test may fail on hardware with KErrNotFound, when
	// attempting to re-open RCommServ
	User::After(3 * 1000000);
	
	// Make sure we did not crash anything
	INFO_PRINTF1(_L("==========>Begin Second Read Sequence"));
	LoadOpenUutAndPeerL();
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iPeer->Write(iWriteBuf1);
	
	iPeer->WaitForWriteL(KErrNone);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
	
	iUut->Close();
	iPeer->Close();
	
	UnloadUut();
	UnloadPeer();
	
	
	return EPass;
	}

/**
Tests C32 BCA when it is reading 

Unload UUT while read is pending
*/
TVerdict CReadingState3::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	iUut->Read(iReadBuf1);	
	// Peer: do not write
	
	UnloadUut(); // Unload while read is pending
		
	// Shutdown the peer
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	UnloadPeer();
	
	return EPass;
	}
	
/**
Tests C32 BCA when it is writing 
*/	
TVerdict CWritingState1::RunTestStepL()
	{
	LoadOpenUutAndPeerL();

	
	iUut->Write(iWriteBuf1);
	// Peer: do not read
	
	iUut->CancelRead();
	
	iUut->CancelWrite();
	
	iUut->Write(iWriteBuf1);
	
	ShutdownUnloadUutAndPeerL(); // close on pending write
		
	return EPass;
	}
	
/**
Tests C32 BCA when it is writing 

*/		
TVerdict CWritingState2::RunTestStepL()
	{
	LoadOpenUutAndPeerL();

	
	iUut->Write(iWriteBuf1);
	// Peer: do not read
	

	iUut->CancelRead();
	
	iReadBuf1.Zero();
	iPeer->Read(iReadBuf1);
	
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iReadBuf1, iWriteBuf1);
	
	
	iUut->Write(iWriteBuf1);
	iPeer->Read(iReadBuf1);
	// at this point the write buffer at the writing CSY
	// should be empty.
	
	
	ShutdownUnloadUutAndPeerL();
	
	return EPass;
	}



/**
Tests C32 BCA when it is writing 
Unload UUT while read is pending
*/		
TVerdict CWritingState3::RunTestStepL()
	{
    LoadOpenUutAndPeerL();
	
	iUut->Write(iReadBuf1);	
	// Peer: do not read
	
	UnloadUut(); // Unload while read is pending
		
	// Shutdown the peer
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	UnloadPeer();
	
	return EPass;
	}



/**
Tests C32 BCA when it is reading and writing
*/	
TVerdict CReadingWritingState1::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	// peer: do nothing
	
	// cancel and write again
	iUut->CancelWrite();
	iUut->Write(iWriteBuf1);
	
	// cancel read and start again
	iUut->CancelRead();
	//iUut->WaitForReadL(KErrCancel);
	iUut->Read(iReadBuf1);
	
	iUut->Close(); // close on pending read and write
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
		
	// Make sure we did not crash anything: restart
	INFO_PRINTF1(_L("Reopening UUT and Proxy"));
	
	OpenProxyL(*iUut, iUutIapId, iUutPortName);
	OpenProxyL(*iPeer, iPeerIapId, iPeerPortName);
	
	DoDuplexReadWriteL(1);
		
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Close();
	
	UnloadUut();
	UnloadPeer();
	
	
	
	return EPass;
	}

/**
Tests C32 BCA when it is reading and writing
Shuts down BCA after cancelling read and write

*/	
TVerdict CReadingWritingState2::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	// 1st read-write sequence
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	
	// Cancel and read again 
	iUut->CancelRead();
	iUut->Read(iReadBuf1);
				
	iReadBuf2.Zero();
	iPeer->Read(iReadBuf2);
	iPeer->Write(iWriteBuf2);
	
	iPeer->WaitForWriteL(KErrNone);
	iUut->WaitForReadL(KErrNone);
	
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	
	CheckBuffersEqualL(iReadBuf2, iWriteBuf1);
	CheckBuffersEqualL(iReadBuf1, iWriteBuf2);
	
	
	
	// 2nd-read write sequence
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
		
	iUut->CancelRead();
	iUut->CancelWrite();
	
	// Don't wait for completion of cancelled read and writes, shutdown now.
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();	
	
	
	return EPass;
	}


/**
Tests C32 BCA when it is reading and writing
Shuts down BCA when it has read and write outstanding

*/	
TVerdict CReadingWritingState3::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	
	static const TInt K4iterations(4);
	DoDuplexReadWriteL(K4iterations);	
	
	// Queue read and write, and then shutdown.
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	// peer: do nothing
	
	
	iUut->Shutdown();
	iPeer->Shutdown();
	
	iUut->WaitForShutdownL(KErrNone);
	iPeer->WaitForShutdownL(KErrNone);
	
	
	UnloadUut();
	UnloadPeer();	
	
	
	// Make sure we did not crash anything
	INFO_PRINTF1(_L("Reopening UUT and Peer"));
	
	LoadOpenUutAndPeerL();
	DoDuplexReadWriteL(1);
	ShutdownUnloadUutAndPeerL();
	
	return EPass;
	}



/**
Tests C32 BCA when it is reading and writing
Closes BCA when it has read and write outstanding
*/	
TVerdict CReadingWritingState4::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	DoDuplexReadWriteL(2);
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	// peer: do nothing
	
	iUut->Close();
	iPeer->Close();
	
	iUut->Shutdown();
	iPeer->Shutdown();
	
	iUut->WaitForShutdownL(KErrNone);
	iPeer->WaitForShutdownL(KErrNone);

	UnloadUut();
	UnloadPeer();	
	
	return EPass;
	}


/**
Tests C32 BCA when it is reading and writing
Unloads BCA when it has read and write outstanding
*/	
TVerdict CReadingWritingState5::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	// peer: do nothing
	
	iUut->Close();
	iPeer->Close();

	iUut->Shutdown();
	iPeer->Shutdown();
	
	iUut->WaitForShutdownL(KErrNone);
	iPeer->WaitForShutdownL(KErrNone);

	UnloadUut();
	UnloadPeer();	
	
	
	return EPass;
	}


/**
Tests C32 BCA when it is Ioctling
Uses invalid option name to try and confuse the BCA before
attempting to shut it down.
*/
TVerdict CIoctlingState1::RunTestStepL()
	{
	LoadCsyNameFromConfigFileL();
	LoadUutL();
	OpenProxyL(*iUut, iUutPortName);
	// Try illegal option to see if confuses the BCA
	static const TUint KInvalidOpt(777);
	iUut->Ioctl(KBcaOptLevelGeneric, KInvalidOpt, iControlBufPckg1);
	
	// Don't wait for completion, shutdown now
			
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	UnloadUut();
	
	return EPass;
	}


/**
Tests C32 BCA when it is Ioctling
Unload BCA in IoctlLing state
*/
TVerdict CIoctlingState2::RunTestStepL()
	{
	LoadUutL();
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialPortName, iControlBufPckg1); 
	UnloadUut(); // unload with pending ioctl
	
	return EPass;
	}



/**
Tests C32 BCA Packet Boundary preservation
*/
TVerdict CPacketBoundary1::RunTestStepL()
	{
	LoadOpenUutAndPeerL();
	
	static const TChar KChar('A');
	// Packets with different sizes:
	static const TInt KBasePacketSize(32);
	static const TInt KBaseUnusedBufferSpace(10);
	
	TBuf8<KBasePacketSize + KBaseUnusedBufferSpace> wPacket1;
	wPacket1.AppendFill(KChar, KBasePacketSize);
	
	TBuf8<KBasePacketSize * 2 + KBaseUnusedBufferSpace> wPacket2;
	wPacket2.AppendFill(KChar, KBasePacketSize * 2);
	
	
	TBuf8<KBasePacketSize * 4 + KBaseUnusedBufferSpace> wPacket3;
	wPacket3.AppendFill(KChar, KBasePacketSize * 4);
	
	
	iPeer->Write(wPacket1);
	iPeer->WaitForWriteL(KErrNone);
	
	iPeer->Write(wPacket2);
	iPeer->WaitForWriteL(KErrNone);
	
	iPeer->Write(wPacket3);
	iPeer->WaitForWriteL(KErrNone);
	
	
	TBuf8<KBasePacketSize * 12> rPacket;
	
	rPacket.Zero();
	iUut->Read(rPacket);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(wPacket1, rPacket);
	
	
	rPacket.Zero();
	iUut->Read(rPacket);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(wPacket2, rPacket);
	
	rPacket.Zero();
	iUut->Read(rPacket);
	iUut->WaitForReadL(KErrNone);
	CheckBuffersEqualL(wPacket3, rPacket);
	
	ShutdownUnloadUutAndPeerL();	
	
	return EPass;
	}


/** 
Test correct reading & use of Comm role parameter from CommDB 
*/
TVerdict CCommDbModemCommRole::RunTestStepL()
	{	
	TCommRole expectedCommRole = LoadExpectedCommRoleFromConfigFileL();	
	
				
	LoadC32SettingsFromConfigFileL();
	
	LoadCsyNameFromConfigFileL();
	OpenRealCommPortL(iCommPort, iC32CsyName, iC32PortName, expectedCommRole);
	
	LoadUutL();
	SetProxyIapIdL(*iUut, iUutIapId); // Specify IAP ID to use

	iUut->Open(_L(""));
	iUut->WaitForOpenL(KErrNone);
	
	// Check Modem CommRole
	TCommRole actualCommRole(ECommRoleDTE);
	iCommPort.GetRole(actualCommRole);
	
	
	INFO_PRINTF3(_L("Expected Comm Role[%d], actual Comm Role [%d]"), expectedCommRole, actualCommRole);
	if(actualCommRole != static_cast<TCommRole>(expectedCommRole))
		{
		User::Leave(KErrArgument);
		}
		
	UnloadUut();	
	return EPass;
	}
	
/** Tests C32Bca behaviour when channel ID is not specified, 
no valid commdb is provided, but use of CommDB is mandatated by provided IAP ID 
*/
TVerdict CCommDbNoChannelId::RunTestStepL()
	{
	TInt expectedOpenErrCode(KErrNone);
	
	if(!GetIntFromConfig(ConfigSection(), _L("ExpectedOpenErrorCode"), expectedOpenErrCode))
		{
		INFO_PRINTF1(_L("Failed to read expected Open Error Code from Config. Test is Aborted."));
		return EAbort;
		}
	
	LoadUutL();
	SetProxyIapIdL(*iUut, iUutIapId); // Specify IAP ID to use
	
	iUut->Open(_L(""));
	iUut->WaitForOpenL(expectedOpenErrCode);
	
	// Don't bother closing, since we should have failed opening. 
	
	UnloadUut();
	
	return EPass;
	}

/** 
Tests C32Bca behaviour when channel ID is not specified, and no valid IAP is provided 
*/
TVerdict CNoChannelIdNoIapId::RunTestStepL()
	{
	TInt expectedOpenErrCode(KErrNone);
	
	if(!GetIntFromConfig(ConfigSection(), _L("ExpectedOpenErrorCode"), expectedOpenErrCode))
		{
		INFO_PRINTF1(_L("Failed to read expected Open Error Code from Config. Test is Aborted."));
		return EAbort;
		}
	
	LoadUutL();
	// Do not specify IAP ID.
	
	iUut->Open(_L(""));
	iUut->WaitForOpenL(expectedOpenErrCode);
	
	// Don't bother closing, since we should have failed. Otherwise, test cleanup will take care of that
	
	UnloadUut();
	
	return EPass;
	}

/** 
Tests C32Bca can read hidden records from the Comms database.
The test creates an entry in CommsDat for a valid CSY using an invalid (unknown) Port Id.
The C32Bca reads this entry and tries to connect to the Comms Server, load the packet loopback csy and open channel UNKNOWN:0.
The Comms Server returns -46, which indicates that it managed to load the packetloop bca csy but returns -46 indicating that
port is invalid.         
*/
TVerdict CHiddenIAPRecord::RunTestStepL()
	{
		
	_LIT(KCsyName, "PKTLOOPBACK");
	_LIT(KCsyPortName, "UNKNOWN::0");
	_LIT(KCsyRecordName, "DummyRecordName");
	
	_LIT(KIAPRecordName, "DummyHiddenIAP");
	_LIT(KModemBearerType, "ModemBearer");
	
	TInt error = KErrNone;

	// setup our BCA object from field passed by config file
	LoadUutL();
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* iCommsDat = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* iCommsDat = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(iCommsDat);
	
	// create a dummy Modem Bearer record
	CCDModemBearerRecord* csyField = static_cast<CCDModemBearerRecord*> (CCDRecordBase::RecordFactoryL(KCDTIdCsyName));
	CleanupStack::PushL(csyField);

	csyField->SetRecordId(KCDNewRecordRequest);
	
	csyField->iRecordName.SetMaxLengthL(15);
	csyField->iRecordName = KCsyRecordName;
	
	csyField->iCsyName.SetMaxLengthL(15);
	csyField->iCsyName = KCsyName;
	
	csyField->iPortName.SetMaxLengthL(15);
	csyField->iPortName = KCsyPortName;
	
	// set field to a modem
	csyField->iCommRole = 1;
			
	TRAP(error, csyField->StoreL(*iCommsDat));
	
	if(error != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		UnloadUut();
		return EFail;		
		}
	
	TInt32 csyRecordId = csyField->RecordId();
	
	// ensure the database can read hidden records also required to store hidden record
	iCommsDat->SetAttributeMask(ECDHidden | ECDPrivate);
	
	CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
  	CleanupStack::PushL(iapRecord);
     	
  	// create a new request
  	iapRecord->SetRecordId(KCDNewRecordRequest);
  	  	
  	iapRecord->iRecordName.SetMaxLengthL(15);
  	iapRecord->iRecordName = KIAPRecordName;
  	iapRecord->iNetworkWeighting = 14;    // dummy value
  	iapRecord->iBearer = csyRecordId;     // point to the ModemBearer record created above
    //CommsDat has now a more strict validation functionality and it's not possible to insert
  	//a bearer field without a bearerType field...
  	iapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	iapRecord->iBearerType = KModemBearerType;
  	
  	// Give the record a hidden status
  	iapRecord->SetAttributes(ECDHidden);
  	
  	TRAP(error,iapRecord->StoreL(*iCommsDat));
  	  	  	
  	if(error != KErrNone)
  		{
  		// cleanup stack and return a failure
  		CleanupStack::PopAndDestroy(3);
  		UnloadUut();
  		return EFail;
  		}
  	
  	// the index into the IAP area of database
  	TInt32 iapRecordId = iapRecord->RecordId();
  	 
  	SetProxyIapIdL(*iUut, iapRecordId); // Specify IAP ID to use for opening 
  	                                    // a port
    
    // follow the process to open a connection. Note this is just a test
    // to run 90% of the validation checks when opening a connection.
    iUut->Open(_L(""));
	
    // On successful reading of a hidden IAP record the Open method will  
    // return KErrPermissionDenied because of our dummy records in the database
    TRAP(error,iUut->WaitForOpenL(KErrPermissionDenied));
   
	// if we don't get the expected return code from the waitForOpenL method 
	// error is set to KErrArgument
	if(error == KErrArgument)
		{
		CleanupStack::PopAndDestroy(3);
		UnloadUut();
		return EFail;	
		}
	
	// now delete the record from the database	
	TRAP(error, iapRecord->DeleteL(*iCommsDat));
	TRAP(error, csyField->DeleteL(*iCommsDat));
	
	if(error != KErrNone)
		{
		 // cleanup the stack and return failure		
		CleanupStack::PopAndDestroy(3);
		UnloadUut();
  		return EFail;	
		}
	
	// pop and destroy the last to objects on the cleanup stack
	CleanupStack::PopAndDestroy(3);

	UnloadUut();
	return EPass; 
	}

/**
Tests C32 BCA Ioctl option: Serial Capability
*/
TVerdict CIoctlOption1::RunTestStepL()
	{
	LoadUutL();
	TPckgBuf<TInt> capsPckg;
	iUut->Ioctl(KBcaOptLevelGeneric, KBCACaps, capsPckg);
	iUut->WaitForIoctlL(KErrNone);
	if(0x01 != capsPckg())
		{
		INFO_PRINTF3(_L("Error: Expected KSerialCaps[%d], got[%d]."), KSerialCaps, capsPckg());
		User::Leave(KErrArgument);
		}
	UnloadUut();
	return EPass;
	}
	
/**
Tests C32 BCA Ioctl option: Version
*/
TVerdict CIoctlOption2::RunTestStepL()
	{
	TInt majorFromConfig(0);
	TInt minorFromConfig(0);
	if(!GetIntFromConfig(ConfigSection(), _L("BcaVersionNumMajor"), majorFromConfig) ||
	   !GetIntFromConfig(ConfigSection(), _L("BcaVersionNumMinor"), minorFromConfig))
		{
		INFO_PRINTF1(_L("Error: BcaVersionNum is not specified in configuration file. Leaving with KErrNotFound"));
		User::Leave(KErrNotFound);
		}
	
	TBcaVersion bcaVersionNum(0x0);
	bcaVersionNum |= static_cast<TBcaVersion>(majorFromConfig) << 16; 
	bcaVersionNum |= static_cast<TBcaVersion>(minorFromConfig);
	if(bcaVersionNum <= 0)
		{
		INFO_PRINTF2(_L("Error: BcaVersionNum [%d] must be positive. Leaving with KErrCorrupt"), bcaVersionNum);
		User::Leave(KErrCorrupt);
		}
	
	
	LoadUutL();
	TPckgBuf<TUint32> bcaVersionNumPckg;
	iUut->Ioctl(KBcaOptLevelGeneric, KVersionNumber, bcaVersionNumPckg);
	iUut->WaitForIoctlL(KErrNone);
	INFO_PRINTF3(_L("Retrieved BCA Version Number[%x], expecting [%x]."), bcaVersionNumPckg(), bcaVersionNum);
	
	if(bcaVersionNum != bcaVersionNumPckg())
		{
		User::Leave(KErrCorrupt);
		}
		
	UnloadUut();
	return EPass;
	}

/**
Tests C32 BCA handling of invalid Ioctl option level and names.
*/
TVerdict CIoctlInvalidOption::RunTestStepL()
	{
	LoadUutL();
	
	static const TInt KInvalidOptionLevel(999);
	static const TInt KInvalidOptionName(999);
	
	//
	// Different combinations of invalid option level and name
	//
	
	// Invalid level, invalid name
	iUut->Ioctl(KInvalidOptionLevel, KInvalidOptionName, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNotSupported);
	
	// Invalid level, valid name
	iUut->Ioctl(KInvalidOptionLevel, KSerialPortName, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNotSupported);
	
	// Valid level, invalid (non-existent) name 
	iUut->Ioctl(KBcaOptLevelExtSerial, KInvalidOptionName, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNotSupported);
	
	// Valid level, invalid name for the level (but valid for other level)
	iUut->Ioctl(KBcaOptLevelExtSerial, KBCACaps, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNotSupported);
	
	// Valid level, invlid name for the level (but valid of other level)
	iUut->Ioctl(KBcaOptLevelGeneric, KSerialPortName, iControlBufPckg1);
	iUut->WaitForIoctlL(KErrNotSupported);
	
	//
	// Sanity check: valid level, valid name
	//
	
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialPortName, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNone);
	
	iUut->Ioctl(KBcaOptLevelGeneric, KBCACaps, iControlBufPckg1); 
	iUut->WaitForIoctlL(KErrNone);
	
	
	UnloadUut();
	
	return EPass;
	}
	
/**
Tests C32 BCA Ioctl option: Getting COMM Config
*/	
TVerdict CIoctlSerialPort1::RunTestStepL()
	{
	LoadC32SettingsFromConfigFileL();
    
	LoadCsyNameFromConfigFileL();
	OpenRealCommPortL(iCommPort, iC32CsyName, iC32PortName);
    
	LoadUutL();
	SetProxyCsyL(*iUut, iC32CsyName);
    
	iUut->Open(iC32PortName);
	iUut->WaitForOpenL(KErrNone);
    
	TCommConfig actualCommConfig;
	iCommPort.Config(actualCommConfig);
	actualCommConfig().iTerminatorCount = 1;
	actualCommConfig().iTerminator[0] = 0x7E;

	LogAndLeaveIfErrorL(iCommPort.SetConfig(actualCommConfig), _L("Setting COMM Config on RComm"));    
    
	TCommConfig balCommConfig;
	// sanity check
	TRAPD(err, CheckBuffersEqualL(balCommConfig, actualCommConfig));
	if(KErrNone == err)
	{
	INFO_PRINTF1(_L("Santify Check Failure: buffers were equal before TCommConfig was retrieved from BCA."));
	return EInconclusive;
	}

	TPckgBuf<TCommConfig> argPckg(balCommConfig);
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialConfig, argPckg);
	iUut->WaitForIoctlL(KErrNone);
	balCommConfig = argPckg();

	CheckBuffersEqualL(balCommConfig, actualCommConfig);

	// Now check that we can accept a V2 structure, although only the V1 attributes are configured for use.
	TCommConfig2 actualCommConfig2;
	actualCommConfig2.FillZ();
	actualCommConfig2().iTerminatorCount = 2;
	actualCommConfig2().iTerminator[0] = 0x7E;    
	LogAndLeaveIfErrorL(iCommPort.SetConfig(actualCommConfig2), _L("Setting COMM Config using V2 on RComm"));

	// Now check that we can retrieve a V1 structure correctly.
	TCommConfig balCommConfig1;
	balCommConfig1.FillZ();
	TPckgBuf<TCommConfig> argPckg1(balCommConfig1);
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialConfig, argPckg1);
	iUut->WaitForIoctlL(KErrNone);
	balCommConfig1 = argPckg1();

	actualCommConfig().iTerminatorCount = 2;    //Modify Comparison Structure to be aligned to balCommConfig1          
	CheckBuffersEqualL(balCommConfig1, actualCommConfig);
    
	iCommPort.Close();
    
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
    
	UnloadUut();
    
	return EPass;    
	}

/**
Tests C32 BCA Ioctl option: Setting COMM Config
*/
TVerdict CIoctlSerialPort2::RunTestStepL()
	{
	LoadC32SettingsFromConfigFileL();
	
	OpenRealCommPortL(iCommPort, iC32CsyName, iC32PortName);
	
	LoadUutL();
	SetProxyCsyL(*iUut, iC32CsyName);
	
	iUut->Open(iC32PortName);
	iUut->WaitForOpenL(KErrNone);
	
	TCommConfig bcaCommConfig;
	iCommPort.Config(bcaCommConfig); // Make sure initial value is valid.
	bcaCommConfig().iTerminatorCount = 1;
	bcaCommConfig().iTerminator[0] = 0x7E;

	TPckgBuf<TCommConfig> argPckg(bcaCommConfig);
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialSetConfig, argPckg);
	iUut->WaitForIoctlL(KErrNone);

	TCommConfig actualCommConfig;
	iCommPort.Config(actualCommConfig);
	
	CheckBuffersEqualL(bcaCommConfig, actualCommConfig);
	
	iCommPort.Close();
	
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	return EPass;
	}





/**
Tests C32 BCA Ioctl option: Getting COMM Caps
*/
TVerdict CIoctlSerialPort3::RunTestStepL()
	{
	LoadC32SettingsFromConfigFileL();
	
	OpenRealCommPortL(iCommPort, iC32CsyName, iC32PortName);
	
	LoadUutL();
	SetProxyCsyL(*iUut, iC32CsyName);
	
	iUut->Open(iC32PortName);
	iUut->WaitForOpenL(KErrNone);
	
	TCommCaps2 actualCommCaps;
	iCommPort.Caps(actualCommCaps);	
	
	TCommCaps2 bcaCommCaps;
	// sanity check
	TRAPD(err, CheckBuffersEqualL(bcaCommCaps, actualCommCaps));
	if(KErrArgument != err)
		{
		INFO_PRINTF1(_L("Sanity check failure: buffers were equal before TCommCaps was retrieved from BCA."));
		return EInconclusive;
		}
	
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialCaps, bcaCommCaps);
	iUut->WaitForIoctlL(KErrNone);
	
	CheckBuffersEqualL(bcaCommCaps, actualCommCaps);
	
	iCommPort.Close();
	
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	return EPass;
	}
	
/**
Tests C32 BCA Ioctl option: Setting Comm Port Role (DTE / DCE)
*/
TVerdict CIoctlSerialPort4::RunTestStepL()
	{
	LoadC32SettingsFromConfigFileL();	
	TCommRole expectedCommRole = LoadExpectedCommRoleFromConfigFileL();
		
	LoadUutL();
	SetProxyIapIdL(*iUut, iUutIapId); // Specify IAP ID to use
	
	TPckgBuf<TCommRole> commRolePckgBuf(expectedCommRole);
	iUut->Ioctl(KBcaOptLevelExtSerial, KSerialSetCommRole, commRolePckgBuf);
	iUut->WaitForIoctlL(KErrNone);
	
	iUut->Open(_L(""));
	iUut->WaitForOpenL(KErrNone);
	
	OpenRealCommPortL(iCommPort, iC32CsyName, iC32PortName, expectedCommRole);
	// If UUT was not opened in expected comm role, the above call failes with KErrLocked
	
	// Pure sanity check
	TCommRole actualCommRole;
	iCommPort.GetRole(actualCommRole);
	
	ERR_PRINTF3(_L("Actual Comm Role [%d]. Expected Comm Role[%d]"), actualCommRole, expectedCommRole);
	if(actualCommRole != expectedCommRole)
		{
		User::Leave(KErrArgument);
		}
	
	iCommPort.Close();
	
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	return EPass;
	}
	

/**
Creates a new BCA Factory using the provided DLL.

@param aBcaDll The host BCA DLL
@return the BCA factory object
@leave if the factory object could not be created 
*/
MBcaFactory* CC32BcaTestBase::NewBcaFactoryL(TDllAutoClose& aBcaDll)
	{
	static const int KNewBcaFactoryProcOrdinal(1);
	TNewBcaFactoryProcL newBcaFactoryProcL = 
		(TNewBcaFactoryProcL)aBcaDll.iObj.Lookup(KNewBcaFactoryProcOrdinal);
	if(!newBcaFactoryProcL) // can't use User::LeaveIfNull, because it returns KErrNoMemory
		{
		ERR_PRINTF1(_L("Failed to located NewBcaFactoryL entry point."));	
		User::Leave(KErrBadLibraryEntryPoint);
		}
		
	MBcaFactory* bcaFactory = NULL; 
	TRAPD(err, bcaFactory = newBcaFactoryProcL());
	if(KErrNone != err)
		{
		ERR_PRINTF2(_L("NewBcaFactoryL left with[%d]."), err);	
		User::Leave(KErrCompletion);
		}
		
	return bcaFactory;
	}
	
/**
Creates an instance of BCA and Leaves

@param aBcaDll the BCA host Dll.
@leave always with KErrNonSystemErrCode
*/	
void CC32BcaTestBase::CreateBcaAndLeaveL(TDllAutoClose& aBcaDll)
	{
	MBcaFactory* bcaFactory = NewBcaFactoryL(aBcaDll);
	CleanupReleasePushL(*bcaFactory);
	MBca* bca = bcaFactory->NewBcaL();
	CleanupReleasePushL(*bca);
	User::Leave(KNonSystemErrCode);	
	}
	
	

/**
Tests the Release & Cleanup Stack functionality of the MBcaFactory
@panic on Heap checks when the test fails.
*/
TVerdict CCleanupBcaRelease::RunTestStepL()
	{
	_LIT(KC32BcaDllName, "C32Bca");
	TDllAutoClose bcaDll;	
	LogAndLeaveIfErrorL(bcaDll.iObj.Load(KC32BcaDllName), _L("Loading BCA Library"));
	
	__UHEAP_MARK;	
	TRAPD(err, CreateBcaAndLeaveL(bcaDll));
	__UHEAP_MARKEND; 
	
	INFO_PRINTF3(_L("CreateBcaAndLeave left with [%d]. Expected error[%d]."), err, KNonSystemErrCode);
	if(KNonSystemErrCode != err)// sanity check
		{
		return EInconclusive;
		}
	return EPass;
	}
	
/** Test C32 BCA open functionality with the different combinations of ChannelId*/
TVerdict CValidChannelId::RunTestStepL()
	{
    TPtrC channelId;
		
	if(!GetStringFromConfig(ConfigSection(), _L("Port_Name1"), channelId))
		{
		INFO_PRINTF1(_L("Failed to read Port Name Config. Test is Aborted."));
		return EAbort;
		}
		
	// Pass channelId in the form of:
	// csyname::portname (ECUART::COMM::0) to the BCA
	// where csyname  = ECUART
	//       portname = COMM::0
	// OR alternatively
	// csyname::portname (PKTLOOPBACK::PKTLOOPBACK::501) 
	// where csyname   = PKTLOOPBACK
	//       portname  = PKTLOOPBACK::501
	//       (PKTLOOPBACK port names contain a seemingly redundant PKTLOOPBACK in their specification).
	LoadUutL();

	// Scenario 1 - the CSY name is specified in the descriptor argument passed to the Open()
	
	iUut->Open(channelId);
	iUut->WaitForOpenL(KErrNone);
	
	UnloadUut();
	
	if(!GetStringFromConfig(ConfigSection(), _L("Port_Name2"), channelId))
		{
		INFO_PRINTF1(_L("Failed to read Port Name Config. Test is Aborted."));
		return EAbort;
		}
		
	// Pass channelId in the form of portname (COMM::0) to the BCA
	//     OR
	//                                         (PKTLOOPBACK::501) 

	// Scenario 2 - the CSY name is not specified in the descriptor argument passed to the Open()
	// In this case, the CSY name is retrieved from CommsDat, based on the IAP.
	// The client specifies the IAP by call SetProxyIapIdL();
	
	LoadUutL();

	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(channelId);
	iUut->WaitForOpenL(KErrNone);
	
	UnloadUut();
	
	return EPass;
	}
