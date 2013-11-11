// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @prototype
*/

#include "dvbhhaitest.h"
#include "dvbhhaitestutil.h"
#include <mobiletv/hai/dvbh/dvbhtypes.h>

/**
* RDvbhHaiTest 
*	DVB-H HAI Unit Tests on stub implementations
*/
		
RDvbhHaiTest* RDvbhHaiTest::NewL(TBool aRequireReceiverInfo, const TDesC& aTestName)
	{
	RDvbhHaiTest* self = new (ELeave) RDvbhHaiTest(aRequireReceiverInfo, aTestName);
	return self;	
	}
	
RDvbhHaiTest::RDvbhHaiTest(TBool aRequireReceiverInfo, const TDesC& aTestName)
: RTestStep(), iDvbhTestDriverVersion(1, 2, 3), iDvbhTestTime(TDateTime(2007, EJuly, 30, 16, 35, 22, 18)),
  iDvbhTestPerformanceData(), iDvbhTestScanConfiguration(), iDvbhTestPlatform(), iDvbhTestHardwareInfo(),
  iDvbhTestNetwork(), iDvbhTestNetworkTime(), iDvbhTestScanResult(), iRequireReceiverInfo(aRequireReceiverInfo),
  iRxInfo(NULL), iTestUtils(NULL), iReceiverType(EDvbhReceiverBluetooth)
	{
	iTestStepName.Copy(aTestName);
	SetupParameterData();
	}

void RDvbhHaiTest::SetupParameterData()
	{
	// The members initialised here should be treated as constants
	// and not changed.
	iDvbhTestPerformanceData.iFilterCount = 1;
	iDvbhTestPerformanceData.iMaxFilters = 2;
	iDvbhTestPerformanceData.iDataCount = 3;
	iDvbhTestPerformanceData.iWriteCount = 4;
	iDvbhTestPerformanceData.iWriteCommandCount = 5;
	iDvbhTestPerformanceData.iReadCount = 6;
	iDvbhTestPerformanceData.iReadCommandCount = 7;
	
	iDvbhTestScanConfiguration.iScanStartFrequency = 100;
	iDvbhTestScanConfiguration.iScanEndFrequency = 200;
	iDvbhTestScanConfiguration.iSignalBandwidth = 10;
	iDvbhTestScanConfiguration.iScanOptions = EDvbhScanOptionFullScan;

	iDvbhTestPlatform.iPlatform.iId = KInitialPlatformId;
	iDvbhTestPlatform.iPlatform.iName = KInitialPlatformName;
	iDvbhTestPlatform.iEsgRoot = KDvbhTestIpV6Addr;
	
	iDvbhTestHardwareInfo = KDvbhTestHardwareInfo;
	
	iDvbhTestNetworkTime.iNetworkTime = iDvbhTestTime;
	iDvbhTestNetworkTime.iTerminalTimeStamp = KDvbhTestTerminalTimeStamp;
	iDvbhTestNetworkTime.iPlatformId = KInitialPlatformId;
	}

void RDvbhHaiTest::InfoPrintf3(const TDesC16& aString, TInt aInt1, TInt aInt2)
	{
	INFO_PRINTF3(aString, aInt1, aInt2);
	}

void RDvbhHaiTest::InfoPrintf4(const TDesC16& aString, TInt aInt1, TInt aInt2, TInt aInt3)
	{
	INFO_PRINTF4(aString, aInt1, aInt2, aInt3);
	}
		
TVerdict RDvbhHaiTest::DoTestStepPreambleL()
	{
	TRAPD(result, iTestUtils = CDvbhTestUtil::NewL(*this));
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d creating CDvbhTestUtil"), result);
		return EInconclusive;		
		}
	
	if (iRequireReceiverInfo)
		{
		TRAP(result, iRxInfo = CDvbhReceiverInfo::NewL())
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d creating CDvbhReceiverInfo"), result);
			return EInconclusive;
			}
		}
		
	if (iTestStepName.Compare(_L("MM-TV-DVBH-U-101")) == 0)		
		{
		result = iTestUtils->DefineAllDvbhProperties();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-103")) == 0)	
		{
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateInactive);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-104")) == 0)	
		{
		 // Receiver state must be EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReceiving);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeySignalQuality, KInitialSignalQuality);
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-105")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			TPckg<TDvbhPlatformProperty> initialPlatformPckg(iDvbhTestPlatform);
			result = iTestUtils->DefineDvbhProperty(EDvbhPropertyKeyPlatform, CDvbhTestUtil::GetType(EDvbhPropertyKeyPlatform), initialPlatformPckg);			
			}
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-106")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			TPckg<TDvbhNetworkTime> initialTimePckg(iDvbhTestNetworkTime);
			result = iTestUtils->DefineDvbhProperty(EDvbhPropertyKeyNetworkTime, CDvbhTestUtil::GetType(EDvbhPropertyKeyNetworkTime), initialTimePckg);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-107")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			TPckg<TDvbhPerformanceData> initialDataPckg(iDvbhTestPerformanceData);
			result = iTestUtils->DefineDvbhProperty(EDvbhPropertyKeyPerformanceData, CDvbhTestUtil::GetType(EDvbhPropertyKeyPerformanceData), initialDataPckg);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-108")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyFrequency, KInitialFrequency);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-109")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyCellId, KInitialCellId);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-110")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyNetworkId, KInitialNetworkId);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-309")) == 0)		
		{
		 // Receiver state must be EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReceiving);
		}

	return (result == KErrNone ? EPass : EInconclusive);				
	}
	
TVerdict RDvbhHaiTest::DoTestStepPostambleL()
	{
	delete iRxInfo;
	iRxInfo = NULL;
	
	TInt result = KErrNone;
	if (iTestStepName.Compare(_L("MM-TV-DVBH-U-101")) == 0)		
		{
		result = iTestUtils->DeleteAllDvbhProperties();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-103")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-104")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeySignalQuality);
		result = result2 != KErrNone ? result2 : result; 		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-105")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyPlatform);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-106")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyNetworkTime);
		result = result2 != KErrNone ? result2 : result;					
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-107")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyPerformanceData);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-108")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyFrequency);	
		result = result2 != KErrNone ? result2 : result;		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-109")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState );
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyCellId);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-110")) == 0)	
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState );
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyNetworkId);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-309")) == 0)	
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState );			
		}
		
	delete iTestUtils;
	iTestUtils = NULL;
	return (result == KErrNone ? EPass : EInconclusive);
	}
	
TVerdict RDvbhHaiTest::DoTestStepL()
	{
	if(iTestStepName.Compare(_L("MM-TV-DVBH-U-001")) == 0)
		{
		return DoCreateReceiverOnStackTestStep();
		}
	else if(iTestStepName.Compare(_L("MM-TV-DVBH-U-002")) == 0)		
		{
		return DoCreateReceiverOnHeapTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-003")) == 0)		
		{
		return DoGetDriverVersionBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-004")) == 0)		
		{
		return DoReceiverOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-005")) == 0)		
		{
		return DoMultipleReceiverOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-006")) == 0)		
		{
		return DoGetDriverVersionAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-007")) == 0)		
		{
		return DoPowerOnAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-008")) == 0)		
		{
		return DoSetScanConfigurationAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-009")) == 0)		
		{
		return DoGetScanConfigurationAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-010")) == 0)		
		{
		return DoGetDvbhVersionAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-011")) == 0)		
		{
		return DoGetHardwareInfoAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-012")) == 0)		
		{
		return DoScanAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-013")) == 0)		
		{
		return DoSetPlatformAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-014")) == 0)		
		{
		return DoCreateFilterAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-015")) == 0)		
		{
		return DoCancelFilterAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-016")) == 0)		
		{
		return DoReceiveIpDataAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-017")) == 0)		
		{
		return DoUpdateNetworkTimeAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-018")) == 0)		
		{
		return DoCustomAsyncAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-019")) == 0)		
		{
		return DoCancelCustomAsyncAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-020")) == 0)		
		{
		return DoCustomAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-022")) == 0)		
		{
		return DoPowerOffAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-023")) == 0)		
		{
		return DoSetDisabledAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-024")) == 0)		
		{
		return DoCancelScanAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-025")) == 0)		
		{
		return DoCancelSetPlatformAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-026")) == 0)		
		{
		return DoCancelReceiveIpDataAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-027")) == 0)		
		{
		return DoCancelUpdateNetworkTimeAfterOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-101")) == 0)		
		{
		return DoCreateReceiverInfoTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-102")) == 0)		
		{
		return DoCreateReceiverInfoTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-103")) == 0)		
		{
		return DoGetStateTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-104")) == 0)		
		{
		return DoGetSignalQualityTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-105")) == 0)		
		{
		return DoGetPlatformTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-106")) == 0)		
		{
		return DoGetNetworkTimeTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-107")) == 0)		
		{
		return DoGetPerformanceDataTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-108")) == 0)		
		{
		return DoGetFrequencyTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-109")) == 0)		
		{
		return DoGetCellIdTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-110")) == 0)		
		{
		return DoGetNetworkIdTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-202")) == 0)		
		{
		return DoPowerOnBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-203")) == 0)		
		{
		return DoSetScanConfigurationBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-204")) == 0)		
		{
		return DoGetScanConfigurationBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-205")) == 0)		
		{
		return DoGetDvbhVersionBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-206")) == 0)		
		{
		return DoGetHardwareInfoBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-207")) == 0)		
		{
		return DoScanBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-208")) == 0)		
		{
		return DoSetPlatformBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-209")) == 0)		
		{
		return DoCreateFilterBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-210")) == 0)		
		{
		return DoCancelFilterBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-211")) == 0)		
		{
		return DoReceiveIpDataBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-212")) == 0)		
		{
		return DoUpdateNetworkTimeBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-213")) == 0)		
		{
		return DoCustomAsyncBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-214")) == 0)		
		{
		return DoCancelCustomAsyncBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-215")) == 0)		
		{
		return DoCustomBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-216")) == 0)		
		{
		return DoCloseBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-217")) == 0)		
		{
		return DoPowerOffBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-218")) == 0)		
		{
		return DoSetDisabledBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-219")) == 0)		
		{
		return DoCancelScanBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-220")) == 0)		
		{
		return DoCancelSetPlatformBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-221")) == 0)		
		{
		return DoCancelReceiveIpDataBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-222")) == 0)		
		{
		return DoCancelUpdateNetworkTimeBeforeOpenTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-301")) == 0)		
		{
		return DoGetStateNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-302")) == 0)		
		{
		return DoGetSignalQualityNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-303")) == 0)		
		{
		return DoGetPlatformNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-304")) == 0)		
		{
		return DoGetNetworkTimeNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-305")) == 0)		
		{
		return DoGetPerformanceDataNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-306")) == 0)		
		{
		return DoGetFrequencyNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-307")) == 0)		
		{
		return DoGetCellIdNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-308")) == 0)		
		{
		return DoGetNetworkIdNoPropertiesTestStep();
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-309")) == 0)		
		{
		return DoGetTestsNoProperties();
		}
		
	return EInconclusive;
	}

TVerdict RDvbhHaiTest::DoCreateReceiverOnStackTestStep()
	{
	__MM_HEAP_MARK;
		{
		//Create RDvbhReceiver on stack
		RDvbhReceiver receiver;
		}
	//RDvbhReceiver out-of-scope, verify no heap leaks
	__MM_HEAP_MARKEND;
	return EPass;
	}
	
TVerdict RDvbhHaiTest::DoCreateReceiverOnHeapTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
	
	RDvbhReceiver* receiver = new RDvbhReceiver;
	if (receiver == NULL)
		{
		verdict = EInconclusive;
		}
	else
		{
		delete receiver;
		receiver = NULL;
		}

	__MM_HEAP_MARKEND;
	return verdict;
	}
	

TVerdict RDvbhHaiTest::DoGetDriverVersion(RDvbhReceiver& aReceiver)
	{
	TVerdict verdict = EPass;
	TVersion version = iDvbhTestDriverVersion;
	
	TInt result = aReceiver.GetDriverVersion(version);
	if (result != KErrNotSupported)
		{
		INFO_PRINTF2(_L("RDvbhReceiver::GetDriverVersion returned %d"), result);
		verdict = EFail;
		}
	else if (!CDvbhTestUtil::CompareVersions(version, iDvbhTestDriverVersion))
		{
		INFO_PRINTF1(_L("RDvbhReceiver::GetDriverVersion modified its arguments."));
		verdict = EFail;
		}
	return verdict;	
	}
	
TVerdict RDvbhHaiTest::DoGetDriverVersionBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		verdict = DoGetDriverVersion(receiver);
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}

/**
* @param aMultiples the number of times to call open on a single RDvbhReceiver instance.  Must be greater than 0.
*
* Creates a RDvbhReceiver instance, calls Open aMultiples of times on it, then Closes it.
*/
TVerdict RDvbhHaiTest::DoReceiverOpenAndClose(TUint aMultiples)
	{
	TVerdict verdict = EPass;
	RDvbhReceiver receiver;
	
	if (aMultiples == 0)
		{
		verdict = EInconclusive;
		}
	else
		{
		for (TUint i=0;i < aMultiples;++i)
			{
			TVerdict openVerdict = OpenReceiver(receiver);
			verdict = (openVerdict == EPass) ? verdict : openVerdict;
			}
		}
	receiver.Close();
	return verdict;
	}

/**
* Helper used to open a receiver before testing other methods.  
* @return EInconclusive if the Open call did not behave as expected so it shouldn't be used to test the Open() method itself.
*/	
TVerdict RDvbhHaiTest::OpenReceiver(RDvbhReceiver& aReceiver)
	{
	TVerdict verdict = EPass;
	TInt result = aReceiver.Open(iReceiverType);
	if (result != KErrNotSupported)
		{
		INFO_PRINTF2(_L("RDvbhReceiver::Open returned %d"), result);
		verdict = EInconclusive;
		}
	return verdict;	
	}

TVerdict RDvbhHaiTest::DoReceiverOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = DoReceiverOpenAndClose();
	__MM_HEAP_MARKEND;
	return verdict;
	}


TVerdict RDvbhHaiTest::DoMultipleReceiverOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = DoReceiverOpenAndClose(3);
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoGetDriverVersionAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;	
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			verdict = DoGetDriverVersion(receiver);
			receiver.Close();
			}
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}


TVerdict RDvbhHaiTest::DoPowerOnAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;	
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TRequestStatus status;
			TInt result = receiver.PowerOn(status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::PowerOn returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoSetScanConfigurationAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TInt result = receiver.SetScanConfiguration(iDvbhTestScanConfiguration);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::SetScanConfiguration returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoGetScanConfigurationAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TDvbhScanConfiguration scanConfig;
			scanConfig.iScanStartFrequency = iDvbhTestScanConfiguration.iScanStartFrequency;
			scanConfig.iScanEndFrequency = iDvbhTestScanConfiguration.iScanEndFrequency;
			scanConfig.iSignalBandwidth = iDvbhTestScanConfiguration.iSignalBandwidth;
			scanConfig.iScanOptions = iDvbhTestScanConfiguration.iScanOptions;
	
			TInt result = receiver.GetScanConfiguration(scanConfig);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::GetScanConfiguration returned %d"), result);
				verdict = EFail;
				}
			else
				{
				if (!CDvbhTestUtil::CompareScanConfigurations(scanConfig, iDvbhTestScanConfiguration))
					{
					INFO_PRINTF1(_L("RDvbhReceiver::GetScanConfiguration modified its arguments."));
					verdict = EFail;					
					}
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoGetDvbhVersionAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TVersion version = iDvbhTestDriverVersion;
			TInt result = receiver.GetDvbhVersion(version);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::GetDvbhVersion returned %d"), result);
				verdict = EFail;
				}
			else
				{
				if (!CDvbhTestUtil::CompareVersions(version, iDvbhTestDriverVersion))
					{
					INFO_PRINTF1(_L("RDvbhReceiver::GetDvbhVersion modified its arguments."));
					verdict = EFail;					
					}
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoGetHardwareInfoAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TDvbhHardwareInfo info = iDvbhTestHardwareInfo;
			TInt result = receiver.GetHardwareInfo(info);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::GetHardwareInfo returned %d"), result);
				verdict = EFail;
				}
			else
				{
				if (info.Compare(iDvbhTestHardwareInfo) != 0) // Compare returns 0 if they are the same
					{
					INFO_PRINTF1(_L("RDvbhReceiver::GetHardwareInfo modified its arguments."));
					verdict = EFail;					
					}
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoScanAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TRequestStatus status;
			TInt result = receiver.Scan(*this, status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::Scan returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoSetPlatformAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TRequestStatus status;
			TInt result = receiver.SetPlatform(iDvbhTestNetwork, iDvbhTestPlatform.iPlatform, status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::SetPlatform returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCreateFilterAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TInt filterId = KDvbhTestFilterId;
			TRequestStatus status;
			TInt result = receiver.CreateFilter(KDvbhTestIpV6Addr, filterId, status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::CreateFilter returned %d"), result);
				verdict = EFail;
				}
			else
				{
				if (filterId != KDvbhTestFilterId)
					{
					INFO_PRINTF1(_L("RDvbhReceiver::CreateFilter modified its arguments."));
					verdict = EFail;					
					}
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelFilterAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TInt filterId = KDvbhTestFilterId;
			TInt result = receiver.CancelFilter(filterId);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::CancelFilter returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoReceiveIpDataAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TInt result = receiver.ReceiveIPData(*this);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::ReceiveIPData returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoUpdateNetworkTimeAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TRequestStatus status;
			TInt result = receiver.UpdateNetworkTime(status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::UpdateNetworkTime returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCustomAsyncAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TBuf8<32> output;
			output = KDvbhTestCustomCommandOutput;
			TRequestStatus status;
			TInt result = receiver.CustomCommand(KDvbhTestCustomCommand, KDvbhTestCustomCommandInput, output, status);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::CustomCommand returned %d"), result);
				verdict = EFail;
				}
			else
				{
				if (output.Compare(KDvbhTestCustomCommandOutput) != 0) // Compare returns 0 if they are the same
					{
					INFO_PRINTF1(_L("RDvbhReceiver::CustomCommand modified its arguments."));
					verdict = EFail;					
					}
				}
			receiver.Close();
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelCustomAsyncAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TRequestStatus status;
			receiver.CancelCustomCommand(status);
			receiver.Close();
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCustomAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			TInt result = receiver.CustomCommand(KDvbhTestCustomCommand, KDvbhTestCustomCommandInput);
			if (result != KErrNotSupported)
				{
				INFO_PRINTF2(_L("RDvbhReceiver::CustomCommand returned %d"), result);
				verdict = EFail;
				}
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
		
TVerdict RDvbhHaiTest::DoPowerOffAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
            TRequestStatus status;
			receiver.PowerOff(status);
			//User::WaitForRequest(status);
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoSetDisabledAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
            TRequestStatus status;
			receiver.SetDisabled(ETrue, status);
            //User::WaitForRequest(status);
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelScanAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			receiver.CancelScan();
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelSetPlatformAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			receiver.CancelSetPlatform();
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelReceiveIpDataAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			receiver.CancelReceiveIPData();
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
TVerdict RDvbhHaiTest::DoCancelUpdateNetworkTimeAfterOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		if ((verdict = OpenReceiver(receiver))  == EPass)
			{
			receiver.CancelUpdateNetworkTime();
			receiver.Close();
			}	
		} //end of receiver's scope
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
//
//
// CDvbhReceiverInfo Positive Tests
//
//

// 101 & 102
TVerdict RDvbhHaiTest::DoCreateReceiverInfoTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	delete iRxInfo; 
	iRxInfo = NULL;
	TRAPD(result, iRxInfo = CDvbhReceiverInfo::NewL())
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d creating CDvbhReceiverInfo"), result);
		verdict = EFail;
		}
	delete iRxInfo; 
	iRxInfo = NULL;
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
// 103
TVerdict RDvbhHaiTest::DoGetStateTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the state to a known value (expectedState) so that we know 
 	//what we are trying to Get.
 	const TInt expectedState = EDvbhStateInactive;
	if (iTestUtils->SetStateProperty(expectedState) != KErrNone)
		{
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhState state = EDvbhStateNoHardware;
		TInt result = iRxInfo->GetState(state);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting state"), result);
			verdict = EFail;				
			}
		else
			{
			if (state != expectedState)
				{
				INFO_PRINTF2(_L("Got unexpected state %d"), state);
				verdict = EFail;
				}
			}
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
// 104
TVerdict RDvbhHaiTest::DoGetSignalQualityTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the signal quality to a known value (expectedQuality) so that we know 
 	//what we are trying to Get.
	const TInt expectedQuality = EDvbhSignalQualityFair;
	TInt result = iTestUtils->SetIntegerProperty(EDvbhPropertyKeySignalQuality, expectedQuality);
	if (result != KErrNone)
		{
		INFO_PRINTF3(_L("Error %d setting signal quality property to %d"), result, expectedQuality);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhSignalQuality quality = EDvbhSignalQualityNoSignal;
		result = iRxInfo->GetSignalQuality(quality);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting signal quality"), result);
			verdict = EFail;				
			}
		else
			{
			if (quality != expectedQuality)
				{
				INFO_PRINTF2(_L("Got unexpected quality %d"), quality);
				verdict = EFail;
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

// 105	
TVerdict RDvbhHaiTest::DoGetPlatformTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the platform property to a known value (iDvbhTestPlatform) so that we know 
 	//what we are trying to Get.
	TPckg<TDvbhPlatformProperty> platformPckg(iDvbhTestPlatform);
	TInt result = iTestUtils->SetByteArrayProperty(EDvbhPropertyKeyPlatform, platformPckg);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting platform property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhPlatform platform;
		TIp6Addr esgRoot;
		result = iRxInfo->GetPlatform(platform, esgRoot);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting platform."), result);
			verdict = EFail;				
			}
		else
			{
			if (!CDvbhTestUtil::ComparePlatforms(platform, iDvbhTestPlatform.iPlatform))
				{
				INFO_PRINTF1(_L("Got unexpected platform."));
				verdict = EFail;
				}
			if (!esgRoot.IsEqual(iDvbhTestPlatform.iEsgRoot))
				{
				INFO_PRINTF1(_L("Got unexpected esgRoot."));
				verdict = EFail;				
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

//Helper for test 106
TVerdict RDvbhHaiTest::DoGetNetworkTime(TBool aExpectedValidity)
	{
	TVerdict verdict = EPass;

	//Expected result is based on whether or not the platformId in the network time we
	//set is valid.
 	TInt expectedResult = KErrNone;
 	if (iDvbhTestNetworkTime.iPlatformId == KDvbhInvalidPlatform)
 		{
 		expectedResult = KErrNotReady;
 		}
 	
 	//Set the time to a known value (iDvbhTestNetworkTime) so that we know 
 	//what we are trying to Get.
	TPckg<TDvbhNetworkTime> timePckg(iDvbhTestNetworkTime);	
	TInt result = iTestUtils->SetByteArrayProperty(EDvbhPropertyKeyNetworkTime, timePckg);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting network time property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TTime initialTime(TInt64(0));
		TTime time = initialTime;
		TBool valid = EFalse;
		result = iRxInfo->GetNetworkTime(time, valid);
		if (result != expectedResult)
			{
			INFO_PRINTF2(_L("Unexptected result %d getting network time."), result);
			verdict = EFail;				
			}
		else
			{
			if (result == KErrNone)
				{
				//Check if what we got was as expected
				if (time != iDvbhTestNetworkTime.iNetworkTime || (valid != aExpectedValidity))
					{
					INFO_PRINTF1(_L("Got unexpected network time or validity."));
					verdict = EFail;
					}				
				}
			else
				{
				//Check that the arguments weren't updated
				if (time != initialTime || (valid != EFalse))
					{
					INFO_PRINTF1(_L("GetNetworkTime modified its args on failure."));
					verdict = EFail;
					}
				}
			}				
		}
	return verdict;
	}

// 106
TVerdict RDvbhHaiTest::DoGetNetworkTimeTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;

	//The 1st two tests are positive tests, so we do not want to get KErrNotReady
	//so we leave iDvbhTestNetworkTime.iPlatformId != KDvbhInvalidPlatform
	//(which is how it is set by default).
	
	//Test one: Set state to Receiving or Ready so that we expect a valid network time
	TDvbhState state = EDvbhStateReady;
	TInt result = iTestUtils->SetStateProperty(state);
	if (result != KErrNone)
		{
		verdict =  EInconclusive;
		}
	else
		{
		verdict = DoGetNetworkTime((state == EDvbhStateReady) || (state == EDvbhStateReceiving));
		}
	
	if (verdict != EPass)
		{
		__MM_HEAP_MARKEND;
		return verdict;		
		}
		
	//Test two: Set state to something other than Ready or Receiving so we expect
	//and invalid network time
	state = EDvbhStateScanning;
	result = iTestUtils->SetStateProperty(state);
	if (result != KErrNone)
		{
		verdict =  EInconclusive;
		}
	else
		{
		verdict = DoGetNetworkTime((state == EDvbhStateReady) || (state == EDvbhStateReceiving));
		}
	
	if (verdict != EPass)
		{
		__MM_HEAP_MARKEND;
		return verdict;		
		}
		
	//Test three: Set iDvbhTestNetworkTime.iPlatformId = KDvbhInvalidPlatform
	//which indicates the receiver is not ready to give network time, regardless
	//of the state it is in.  Will result in KErrNotReady being returned from
	//CDvbhReceiverInfo::GetNetworkTime()
	iDvbhTestNetworkTime.iPlatformId = KDvbhInvalidPlatform;
	verdict = DoGetNetworkTime((state == EDvbhStateReady) || (state == EDvbhStateReceiving));
			
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
// 107 
TVerdict RDvbhHaiTest::DoGetPerformanceDataTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the data to a known value (expectedData) so that we know 
 	//what we are trying to Get.
	TPckg<TDvbhPerformanceData> dataPckg(iDvbhTestPerformanceData);	
	TInt result = iTestUtils->SetByteArrayProperty(EDvbhPropertyKeyPerformanceData, dataPckg);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting performance data property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhPerformanceData data; //Is initialised to 0s by constructor.
		result = iRxInfo->GetPerformanceData(data);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting performance data."), result);
			verdict = EFail;				
			}
		else
			{
			if (!CDvbhTestUtil::ComparePerformanceDatas(data, iDvbhTestPerformanceData))
				{
				INFO_PRINTF1(_L("Got unexpected performance data."));
				verdict = EFail;
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

// 108
TVerdict RDvbhHaiTest::DoGetFrequencyTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the data to a known value (expectedFrequency) so that we know 
 	//what we are trying to Get.
	const TDvbhFrequency expectedFrequency = 150000;	
	TInt result = iTestUtils->SetIntegerProperty(EDvbhPropertyKeyFrequency, expectedFrequency);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting frequency property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhFrequency frequency = 0;
		result = iRxInfo->GetFrequency(frequency);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting frequency."), result);
			verdict = EFail;				
			}
		else
			{
			if (frequency != expectedFrequency)
				{
				INFO_PRINTF2(_L("Got unexpected frequency %d."), frequency);
				verdict = EFail;
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

// 109
TVerdict RDvbhHaiTest::DoGetCellIdTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the data to a known value (expectedId) so that we know 
 	//what we are trying to Get.
	const TDvbhCellId expectedId = 1001;	
	TInt result = iTestUtils->SetIntegerProperty(EDvbhPropertyKeyCellId , expectedId);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting cellId property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhCellId id = 0;
		result = iRxInfo->GetCellId(id);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting cellId."), result);
			verdict = EFail;				
			}
		else
			{
			if (id != expectedId)
				{
				INFO_PRINTF2(_L("Got unexpected cellId %d."), id);
				verdict = EFail;
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

// 110
TVerdict RDvbhHaiTest::DoGetNetworkIdTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
 	//First set the data to a known value (expectedId) so that we know 
 	//what we are trying to Get.
	const TDvbhNetworkId expectedId = 9876;	
	TInt result = iTestUtils->SetIntegerProperty(EDvbhPropertyKeyNetworkId , expectedId);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("Error %d setting networkId property."), result);
		verdict = EInconclusive;			
		}
	else
		{
		TDvbhNetworkId id = 0;
		result = iRxInfo->GetNetworkId(id);
		if (result != KErrNone)
			{
			INFO_PRINTF2(_L("Error %d getting networkId."), result);
			verdict = EFail;				
			}
		else
			{
			if (id != expectedId)
				{
				INFO_PRINTF2(_L("Got unexpected networkId %d."), id);
				verdict = EFail;
				}
			}				
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}
	
//
//
// RDvbhReceiver negative tests
//
//
	
//202
TVerdict RDvbhHaiTest::DoPowerOnBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
        TRequestStatus status;
		TInt result = receiver.PowerOn(status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::PowerOn returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}

//203
TVerdict RDvbhHaiTest::DoSetScanConfigurationBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TInt result = receiver.SetScanConfiguration(iDvbhTestScanConfiguration);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::SetScanConfiguration returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//204
TVerdict RDvbhHaiTest::DoGetScanConfigurationBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TDvbhScanConfiguration config;
		config.iScanStartFrequency = iDvbhTestScanConfiguration.iScanStartFrequency;
		config.iScanEndFrequency = iDvbhTestScanConfiguration.iScanEndFrequency;
		config.iSignalBandwidth = iDvbhTestScanConfiguration.iSignalBandwidth;
		config.iScanOptions = iDvbhTestScanConfiguration.iScanOptions;
		
		TInt result = receiver.GetScanConfiguration(config);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::GetScanConfiguration returned %d"), result);
			verdict = EFail;
			}
		else
			{
			if (!iTestUtils->CompareScanConfigurations(iDvbhTestScanConfiguration, config))
				{
				INFO_PRINTF1(_L("RDvbhReceiver::GetScanConfiguration modified its arg on failure."));
				verdict = EFail;			
				}			
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//205
TVerdict RDvbhHaiTest::DoGetDvbhVersionBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TVersion version = iDvbhTestDriverVersion;
		
		TInt result = receiver.GetDvbhVersion(version);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::GetDvbhVersion returned %d"), result);
			verdict = EFail;
			}
		else
			{
			if (!iTestUtils->CompareVersions(version, iDvbhTestDriverVersion))
				{
				INFO_PRINTF1(_L("RDvbhReceiver::GetDvbhVersion modified its arg on failure."));
				verdict = EFail;			
				}			
			}

		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//206
TVerdict RDvbhHaiTest::DoGetHardwareInfoBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TDvbhHardwareInfo info = iDvbhTestHardwareInfo;
		
		TInt result = receiver.GetHardwareInfo(info);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::GetHardwareInfo returned %d"), result);
			verdict = EFail;
			}
		else
			{
			if (info.Compare(iDvbhTestHardwareInfo) != 0) // Compare returns 0 if they are the same
				{
				INFO_PRINTF1(_L("RDvbhReceiver::GetHardwareInfo modified its arg on failure."));
				verdict = EFail;			
				}			
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;
	}

//207	
TVerdict RDvbhHaiTest::DoScanBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TRequestStatus status;
		TInt result = receiver.Scan(*this, status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::Scan returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//208
TVerdict RDvbhHaiTest::DoSetPlatformBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TRequestStatus status;
		TInt result = receiver.SetPlatform(iDvbhTestNetwork, iDvbhTestPlatform.iPlatform, status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::SetPlatform returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//209
TVerdict RDvbhHaiTest::DoCreateFilterBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TInt filterId = 100;
		TInt origFilterId = filterId;
		TRequestStatus status;
		TInt result = receiver.CreateFilter(KDvbhTestIpV6Addr, filterId, status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::CreateFilter returned %d"), result);
			verdict = EFail;
			}
		else
			{
			if (filterId != origFilterId)
				{
				INFO_PRINTF1(_L("RDvbhReceiver::CreateFilter modified its arg on failure."));
				verdict = EFail;			
				}			
			}			
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//210
TVerdict RDvbhHaiTest::DoCancelFilterBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TInt filterId = 7;
		TInt result = receiver.CancelFilter(filterId);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::CancelFilter returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//211
TVerdict RDvbhHaiTest::DoReceiveIpDataBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TInt result = receiver.ReceiveIPData(*this);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::ReceiveIPData returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//212
TVerdict RDvbhHaiTest::DoUpdateNetworkTimeBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TRequestStatus status;
		TInt result = receiver.UpdateNetworkTime(status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::UpdateNetworkTime returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//213
TVerdict RDvbhHaiTest::DoCustomAsyncBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TBuf8<32> output = _L8("Output");
		TBuf8<32> origOutput = output;
		TRequestStatus status;
		TInt result = receiver.CustomCommand(KDvbhTestCustomCommand, KDvbhTestCustomCommandInput, output, status);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::CustomCommand returned %d"), result);
			verdict = EFail;
			}
		else
			{
			if (output.Compare(origOutput) != 0) //Compare returns 0 if they are the same
				{
				INFO_PRINTF1(_L("RDvbhReceiver::CustomCommand modified its arg on failure."));
				verdict = EFail;			
				}			
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;		
	}
	
//214
TVerdict RDvbhHaiTest::DoCancelCustomAsyncBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TRequestStatus status;
		receiver.CancelCustomCommand(status);
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//215
TVerdict RDvbhHaiTest::DoCustomBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		TInt result = receiver.CustomCommand(KDvbhTestCustomCommand, KDvbhTestCustomCommandInput);
		if (result != KErrNotReady)
			{
			INFO_PRINTF2(_L("RDvbhReceiver::CustomCommand returned %d"), result);
			verdict = EFail;
			}
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//216
TVerdict RDvbhHaiTest::DoCloseBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		receiver.Close();
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;		
	}

//217
TVerdict RDvbhHaiTest::DoPowerOffBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
        TRequestStatus status;
		receiver.PowerOff(status);
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//218
TVerdict RDvbhHaiTest::DoSetDisabledBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
        TRequestStatus status;
		receiver.SetDisabled(ETrue, status);
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//219
TVerdict RDvbhHaiTest::DoCancelScanBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		receiver.CancelScan();
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//220
TVerdict RDvbhHaiTest::DoCancelSetPlatformBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		receiver.CancelSetPlatform();
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//221
TVerdict RDvbhHaiTest::DoCancelReceiveIpDataBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		receiver.CancelReceiveIPData();
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//222
TVerdict RDvbhHaiTest::DoCancelUpdateNetworkTimeBeforeOpenTestStep()
	{
	__MM_HEAP_MARK;
	TVerdict verdict = EPass;
		{
		RDvbhReceiver receiver;
		receiver.CancelUpdateNetworkTime();
		} //end of receiver's scope	
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
	
						
// 301	
TVerdict RDvbhHaiTest::DoGetStateNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhState state = EDvbhStateReady;
	TInt result = iRxInfo->GetState(state);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting state but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (state != EDvbhStateReady)
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetState modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;
	}

//302	
TVerdict RDvbhHaiTest::DoGetSignalQualityNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhSignalQuality quality = EDvbhSignalQualityVeryGood;
	TInt result = iRxInfo->GetSignalQuality(quality);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting signal quality but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (quality != EDvbhSignalQualityVeryGood)
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetSignalQuality modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//303	
TVerdict RDvbhHaiTest::DoGetPlatformNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhPlatform platform;
	platform.iId = iDvbhTestPlatform.iPlatform.iId;
	platform.iName = iDvbhTestPlatform.iPlatform.iName;
	
	TIp6Addr esgRoot = iDvbhTestPlatform.iEsgRoot;

	TInt result = iRxInfo->GetPlatform(platform, esgRoot);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting platform but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (!CDvbhTestUtil::ComparePlatforms(platform, iDvbhTestPlatform.iPlatform)  ||
			!esgRoot.IsEqual(iDvbhTestPlatform.iEsgRoot) )
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetPlatform modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//304	
TVerdict RDvbhHaiTest::DoGetNetworkTimeNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;	
	TTime time = iDvbhTestTime;
	TBool valid = ETrue;
	
	TInt result = iRxInfo->GetNetworkTime(time, valid);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting network time but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (time != iDvbhTestTime  || !valid )
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetNetworkTime modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;	
	}

//305	
TVerdict RDvbhHaiTest::DoGetPerformanceDataNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhPerformanceData data;
	data.iFilterCount = iDvbhTestPerformanceData.iFilterCount;
	data.iMaxFilters = iDvbhTestPerformanceData. iMaxFilters;
	data.iDataCount = iDvbhTestPerformanceData.iDataCount;
	data.iWriteCount = iDvbhTestPerformanceData.iWriteCount;
	data.iWriteCommandCount = iDvbhTestPerformanceData.iWriteCommandCount;
	data.iReadCount = iDvbhTestPerformanceData.iReadCount;
	data.iReadCommandCount = iDvbhTestPerformanceData.iReadCommandCount;
	
	TInt result = iRxInfo->GetPerformanceData(data);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting performance data but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (!CDvbhTestUtil::ComparePerformanceDatas(data, iDvbhTestPerformanceData))
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetPerformaceData modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;		
	}

//306	
TVerdict RDvbhHaiTest::DoGetFrequencyNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhFrequency frequency = KInitialFrequency;
	
	TInt result = iRxInfo->GetFrequency(frequency);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting frequency but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (frequency != KInitialFrequency)
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetFrequency modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;		
	}

//307	
TVerdict RDvbhHaiTest::DoGetCellIdNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhCellId cellId = KInitialCellId;
	
	TInt result = iRxInfo->GetCellId(cellId);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting cellId but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (cellId != KInitialCellId)
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetCellId modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	
//308
TVerdict RDvbhHaiTest::DoGetNetworkIdNoPropertiesTestStep()
	{
	TVerdict verdict = EPass;
	__MM_HEAP_MARK;
	TDvbhNetworkId networkId = KInitialNetworkId;
	
	TInt result = iRxInfo->GetNetworkId(networkId);
	if (result != KErrNotFound)
		{
		INFO_PRINTF2(_L("Got error %d getting networkId but expected KErrNotFound"), result);
		verdict = EFail;				
		}
	else
		{
		if (networkId != KInitialNetworkId)
			{
			INFO_PRINTF1(_L("RDvbhReceiverInfo::GetNetworkId modified its arg on failure."));
			verdict = EFail;			
			}			
		}
	__MM_HEAP_MARKEND;
	return verdict;	
	}
	

//309
/**
* Test added to improve conditional code coverage.  It tries to get all the properties
* that depend on state when the state property is defined, but no other properties are
* defined.
*/
TVerdict RDvbhHaiTest::DoGetTestsNoProperties()
	{
	TVerdict verdict = EPass;
		
	//Preamble will have defined the state property and set it to EDvbhStateReceiving.  So
	//we just have to try to get each (non-defined) property that depends on state.

	//CellId
	verdict = DoGetCellIdNoPropertiesTestStep();
	if (verdict != EPass)
		{
		return verdict;
		}
		
	//Frequency
	verdict = DoGetFrequencyNoPropertiesTestStep();
	if (verdict != EPass)
		{
		return verdict;
		}

	//NetworkId
	verdict = DoGetNetworkIdNoPropertiesTestStep();
	if (verdict != EPass)
		{
		return verdict;
		}
		
	//Platform
	verdict = DoGetPlatformNoPropertiesTestStep();
	if (verdict != EPass)
		{
		return verdict;
		}

	//Signal Quality
	verdict = DoGetSignalQualityNoPropertiesTestStep();
	if (verdict != EPass)
		{
		return verdict;
		}
	
	return verdict;	
	}
	
//MDvbhScanObserver
void RDvbhHaiTest::DvbhScanProgress( const TDvbhScanResult& /*aResult*/ )
	{	
	//Should never be called
	ASSERT(EFalse);
	}
	
//MDvbhDataObserver
void RDvbhHaiTest::DvbhPacketReceived( const TDesC8& /*aPacket*/ )
	{
	//Should never be called
	ASSERT(EFalse);	
	}
	
