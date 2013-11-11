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
// te_xxBcaUnitTestSteps.cpp
// Implements the unit test classes for xxBca.
// 
//

/**
 @file
 @internalComponent
*/


#include "te_xxBcaUnitSteps.h"
#include "BcaTestProxy.h"
#include <c32comm.h> // uniquely for the call to StartC32WithCMISuppressions

using namespace te_xxBcaUnitTests; // all xxBca tests



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
CxxBcaTestBase::CxxBcaTestBase()
	: CTestStep(),
	iUut(NULL),
	iPeer(NULL),
	iWriteBuf1(KWriteBuf1Lit),
	iWriteBuf2(KWriteBuf2Lit)
	{	
	}

/**
C++ destructor */
CxxBcaTestBase::~CxxBcaTestBase()
	{
		
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
 Used to disable the phone book synchronizer which may cause the tests to fail.
 
 
 @return EPass if successful.
 @leave If Phonebook synchronizer disabling fails. 
 */
TVerdict CxxBcaTestBase::doTestStepPreambleL()
	{	
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	TESTL(KErrNone == err || KErrAlreadyExists == err);
	INFO_PRINTF1(_L("Test Step Preamble: disabled Phonebook Synchronizer."));	
	return EPass;
	}
	
/**
 Implements the pure virtual doTestStepL defined in CTestStep. 
 Allows the base class to execute before any of the derived tests is called.
 
 @leave If any of the called methods leaves.
 */
TVerdict CxxBcaTestBase::doTestStepL()
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
void CxxBcaTestBase::TestEnvironmentCleanup()
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
void CxxBcaTestBase::LoadBcaIapSettingsFromConfigFileL()
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
void CxxBcaTestBase::LoadBcaSettingsFromConfigFileL()
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


/**
Logs a message and leaves on error 

@param aErrCode error code to check
@param aMsg message to log 
@leave if aError is other than KErrNone
*/
void CxxBcaTestBase::LogAndLeaveIfErrorL(TInt aErrCode, const TDesC& aMsg)
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
void CxxBcaTestBase::LoadUutL()
	{
	INFO_PRINTF1(_L("Loading UUT"));
	ASSERT(NULL == iUut);
	
	iUut = CxxBcaProxy::NewL(*this, iUutName);
	
	ASSERT(iUut);
	INFO_PRINTF1(_L("UUT Loaded"));	
	}

/**
Unloads UUT 

*/
void CxxBcaTestBase::UnloadUut()
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
void CxxBcaTestBase::LoadPeerL()
	{
	INFO_PRINTF1(_L("Loading Peer"));
	ASSERT(NULL == iPeer);
	
	iPeer = CxxBcaProxy::NewL(*this, iPeerName); 
	
	ASSERT(iPeer);
	INFO_PRINTF1(_L("Peer Loaded"));
	}
	
/**
Unloads Peer */
void CxxBcaTestBase::UnloadPeer()
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
void CxxBcaTestBase::CheckBuffersEqualL(const TDesC16& aBuf1,const TDesC16& aBuf2)
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
void CxxBcaTestBase::CheckBuffersEqualL(const TDesC8& aBuf1,const TDesC8& aBuf2)
	{
	if(aBuf1 != aBuf2)
		{
		INFO_PRINTF1(_L("Buffer Equality Check Failed."));
		User::Leave(KErrArgument);
		}
	INFO_PRINTF1(_L("Buffer Equality Check OK."));		
	}



//Sets a CSY name in a BCA 

//@param aProxy the BCA proxy 
//@param aCsy CSY name
//@leave on failure 
void CxxBcaTestBase::SetProxyIapIdL(MBcaProxy& aProxy, TUint32 aIapId)
	{
	// Ioctl: Set Iap Id
	TPckgBuf<TUint32> commDbIapIdPckg(aIapId);
	aProxy.Ioctl(KBcaOptLevelGeneric, KBCASetIapId, commDbIapIdPckg);
	aProxy.WaitForIoctlL(KErrNone);	
	}
	

	

	
//CLOSED STATE
//



/**
Test open when BCA is closed
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



//

//SHUTTING DOWN STATE

/**
Close the BCA while it is trying to shutdown
*/

TVerdict CShutdownState1::RunTestStepL()
	{
	LoadUutL();
		
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName);
	iUut->WaitForOpenL(KErrNone);
		
	iUut->Shutdown();
	iUut->Close(); // don't wait for Shutdown to complete
	
	UnloadUut();

	
	return EPass;
	}
	
/**	
Unload BCA while shutting down
*/

TVerdict CShutdownState2::RunTestStepL()
	{
	LoadUutL();
		
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName);
	iUut->WaitForOpenL(KErrNone);
		
	iUut->Shutdown();		
	UnloadUut();

	
	return EPass;
	}


//
//OPENING STATE

/**
Shutdown while BCA is opening
*/

TVerdict COpeningState1::RunTestStepL()
	{
	LoadUutL();
		
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName); // don't wait for Open to complete
			
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	
	return EPass;
	}
	

/**
Close while the BCA is openning	
*/

TVerdict COpeningState2::RunTestStepL()
	{
	LoadUutL();
		
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName); //don't wait for the Open to complete
	
	iUut->Close();		
	
	UnloadUut();
	
	return EPass;
	}
	
/**	
Release while the BCA is openning
*/

TVerdict COpeningState3::RunTestStepL()
	{
	LoadUutL();
		
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName); //don't wait for the Open to complete
			
	UnloadUut();
	
	return EPass;
	
	}





//
//OPEN STATE

/**
test for successful data tansfer between the 2 BCA's
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
	
	// Peer should have read what Uut wrote
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);
	
	
	// Read - Write
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iPeer->Write(iWriteBuf1);
	
	// Uut should have read what Peer wrote
	iUut->WaitForReadL(KErrNone);
	iUut->WaitForWriteL(KErrNone);
	CheckBuffersEqualL(iWriteBuf1, iReadBuf1);

	
	// normal Shutdown of Uut and Peer
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	iPeer->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	UnloadUut();
	UnloadPeer();	


	return EPass;
	}
	



/**
Close when the BCA is opened
*/

TVerdict COpenState2::RunTestStepL()
	{
	LoadUutL();
	
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName);	
	
	iUut->WaitForOpenL(KErrNone);
	
	iUut->Close();
	
	UnloadUut();
	
	return EPass;
	}
	
/**
Unload when the BCA is opened
*/

TVerdict COpenState3::RunTestStepL()
	{
	LoadUutL();
	
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName);	
	
	iUut->WaitForOpenL(KErrNone);
	
	UnloadUut();
	
	return EPass;
	}
	
/**
Peforming a IOClt operation when BCA is opened	
*/

TVerdict COpenState4::RunTestStepL()
	{

    //get BCA major and minor version numbers from config file
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
	
	SetProxyIapIdL(*iUut, iUutIapId);
	
	iUut->Open(iUutPortName);
	
	//get BCA major and minor version numbers from IOclt request
	TPckgBuf<TUint32> bcaVersionNumPckg;
	iUut->Ioctl(KBcaOptLevelGeneric, KVersionNumber, bcaVersionNumPckg);
	iUut->WaitForIoctlL(KErrNone);
	INFO_PRINTF3(_L("Retrieved BCA Version Number[%x], expecting [%x]."), bcaVersionNumPckg(), bcaVersionNum);
	
	//compare the version numbers
	if(bcaVersionNum != bcaVersionNumPckg())
		{
		User::Leave(KErrCorrupt);
		}
		
	UnloadUut();
	return EPass;
	
	}


//
//READING STATE

/**
Cancelread while the BCA is reading and sucessful Shutdown
*/

TVerdict CReadingState1::RunTestStepL()
	{
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);
		
	
	iUut->Read(iReadBuf1);
	
	// Peer: do not write
	
		
	// Cancel Read:
	iUut->CancelRead();

	
	// Start another read to get back into reading state
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	
	//Shutdown on pending read
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	

	UnloadUut();
	UnloadPeer();

	
	return EPass;
	}
	
/**
Close while the BCA is reading 
*/

TVerdict CReadingState2::RunTestStepL()
	{
	
    LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);

	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	
	//Close on a pending read	
	iUut->Close();
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();
	
	
	return EPass;
	}

/**
Unload while the BCA is reading 
*/

TVerdict CReadingState3::RunTestStepL()
	{
	
    LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iUut->Open(iPeerPortName);		
	iUut->WaitForOpenL(KErrNone);

	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	
	//Unload on a pending read
	UnloadUut();
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	UnloadPeer();
	
	
	return EPass;
	}



//
// WRITING STATE

/**
CancelWrite while BCA is writing 	
*/

TVerdict CWritingState1::RunTestStepL()
	{
	
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iUut->Open(iPeerPortName);		
	iUut->WaitForOpenL(KErrNone);


	
	iUut->Write(iWriteBuf1);
	
	//cancel the write
	iUut->CancelWrite();
	
	//start another write to get back in writing state
	iUut->Write(iWriteBuf1);
		
	//Unload the BCA while it is writing
	UnloadUut();
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadPeer();		
	
	
	return EPass;
	
	}
	
/**
Shutdown the BCA while is writing
*/

TVerdict CWritingState2::RunTestStepL()
	{
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);

	iUut->Write(iWriteBuf1);
	
	//shutdown the BCA while writing
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();
	
	return EPass;	
	
	}

/**
Close the BCA while it is writing
*/

TVerdict CWritingState3::RunTestStepL()
	{
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);

	iUut->Write(iWriteBuf1);
	
	//close the BCA while writing
	iUut->Close();
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();
	
	return EPass;	
	
	}


//
//READING/WRITING

/**
Successful data transfer between the 2 BCA's	
*/

TVerdict CReadingWritingState1::RunTestStepL()
	{
	
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);
		
	//Uut Read and Write
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
				
	//Peer Read and Write
	iReadBuf2.Zero();
	iPeer->Read(iReadBuf2);
	iPeer->Write(iWriteBuf2);
	
	//Check 1st exchange
	iUut->WaitForReadL(KErrNone);
	iPeer->WaitForWriteL(KErrNone);
	
	
	//Check 2nd exchange
	iUut->WaitForWriteL(KErrNone);
	iPeer->WaitForReadL(KErrNone);
	
	
	CheckBuffersEqualL(iReadBuf1, iWriteBuf2);
	CheckBuffersEqualL(iReadBuf2, iWriteBuf1);
	
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();
	
	
	return EPass;
	}


/**
Test various operations while the BCA is reading and writing	
*/

TVerdict CReadingWritingState2::RunTestStepL()
	{	
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);
	
	// BCA read and write
	iReadBuf1.Zero();
	iUut->Write(iWriteBuf1);
	iUut->Read(iReadBuf1);
	
	// Cancelwrite
	iUut->CancelWrite();
	
	// Start another write to get back to Read/Write State
	iUut->Write(iWriteBuf1);
		 
	// CancelRead	 
	iUut->CancelRead();
	
	// Start another Read to get back to Read/Write state
	iUut->Read(iReadBuf1);
	
	// Close the BCA while it is in Read/Write state			
	iUut->Close();
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();	
	
	return EPass;
	
	}

/**
Shutdown the BCA while it is in Read/Write state
*/

TVerdict CReadingWritingState3::RunTestStepL()
	{	
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);
	
	//BCA Read and Write
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	
	//Shutdown the BCA while it is in Reading/Wrting state
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	
	iPeer->Shutdown();
	iPeer->WaitForShutdownL(KErrNone);
	
	UnloadUut();
	UnloadPeer();	
	
	
	return EPass;	
	
	}

/**
Unload the BCA while it is Read/Write state	
*/

TVerdict CReadingWritingState4::RunTestStepL()
	{
	
	LoadUutL();	
	SetProxyIapIdL(*iUut, iUutIapId);	
	iUut->Open(iUutPortName);		
	iUut->WaitForOpenL(KErrNone);
	
	LoadPeerL();	
	SetProxyIapIdL(*iPeer, iPeerIapId);	
	iPeer->Open(iPeerPortName);		
	iPeer->WaitForOpenL(KErrNone);
	
	//BCA Read and Write
	iReadBuf1.Zero();
	iUut->Read(iReadBuf1);
	iUut->Write(iWriteBuf1);
	
	// Unload the BCA while it is Read/Write state
	UnloadUut();
	UnloadPeer();	
	
	return EPass;
	
	
	}






//
//IOCLTING STATE

/**
Unload the BCA while is performing IOClt 
*/

TVerdict CIoctlingState1::RunTestStepL()
	{
	
	LoadUutL();		
	
	//Setup IAP ID via IOClt
	TPckgBuf<TUint32> commDbIapIdPckg(iUutIapId);
	iUut->Ioctl(KBcaOptLevelGeneric, KBCASetIapId, commDbIapIdPckg);	
	
	// do not wait for IOClt to complete
	
	// Unload the BCA while it is IOCltling
	UnloadUut();
	
	return EPass;
	
	}


/**
CancelIoctl and shutdown while in IOClt state
*/


TVerdict CIoctlingState2::RunTestStepL()
	{
	
	LoadUutL();		
	
	//Setup IAP ID via IOClt
	TPckgBuf<TUint32> commDbIapIdPckg(iUutIapId);
	iUut->Ioctl(KBcaOptLevelGeneric, KBCASetIapId, commDbIapIdPckg);	
	
	// Do not wait for the IOClt to complete
	
	// Cancel the IOCtl 
	// This step is will not pass unless IOClt is asynchronous, refer to Test Spec
	// BCA IOClt api is synchronous - cancel does not work  
	iUut->CancelIoctl(); 	
	iUut->WaitForIoctlL(KErrNone);	
	
	// Start another one to get back into IOCtling state
	iUut->Ioctl(KBcaOptLevelGeneric, KBCASetIapId, commDbIapIdPckg);
	iUut->WaitForIoctlL(KErrNone);
	
	// Shutdown the BCA while it is IOClting state
	iUut->Shutdown();
	iUut->WaitForShutdownL(KErrNone);
	

	UnloadUut();
	
	return EPass;
	
	}
	
	

//



/**
Creates a new BCA Factory using the provided DLL.

@param aBcaDll The host BCA DLL
@return the BCA factory object
@leave if the factory object could not be created 
*/
MBcaFactory* CxxBcaTestBase::NewBcaFactoryL(TDllAutoClose& aBcaDll)
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
void CxxBcaTestBase::CreateBcaAndLeaveL(TDllAutoClose& aBcaDll)
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
	_LIT(KxxBcaDllName, "C32Bca");
	TDllAutoClose bcaDll;	
	LogAndLeaveIfErrorL(bcaDll.iObj.Load(KxxBcaDllName), _L("Loading BCA Library"));
	
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
	






