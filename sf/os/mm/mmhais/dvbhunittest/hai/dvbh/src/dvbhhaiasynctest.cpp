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

#include "dvbhhaiasynctest.h"
#include <mobiletv/hai/dvbh/dvbhtypes.h>

/**
* RDvbhHaiAsyncTest 
*	DVB-H HAI Asynchronous Unit Tests on stub implementations
*/

		
RDvbhHaiAsyncTest* RDvbhHaiAsyncTest::NewL(TBool aRequireReceiverInfo, const TDesC& aTestName)
	{
	RDvbhHaiAsyncTest* self = new (ELeave) RDvbhHaiAsyncTest(aRequireReceiverInfo, aTestName);
	return self;	
	}
	
RDvbhHaiAsyncTest::RDvbhHaiAsyncTest(TBool aRequireReceiverInfo, const TDesC& aTestName)
: RAsyncTestStep(), iExpectedNetworkTime(TDateTime(2007, EJuly, 30, 16, 35, 22, 18)),
  iInitialNetworkTime(TDateTime(1997, EMay, 10, 10, 00, 34, 02)), iInitialPlatform(),
  iExpectedPlatform(), iPreTestRun(EFalse), iRequireReceiverInfo(aRequireReceiverInfo),
  iRxInfo(NULL), i2ndRxInfo(NULL), iTestUtils(NULL), i2ndStateObserver(NULL)
	{
	iStateCallbackVerdicts[E1stStateObserver] = KNoVerdict;
	iStateCallbackVerdicts[E2ndStateObserver] = KNoVerdict;
	iTestStepName.Copy(aTestName);
	SetupParameterData();
	}

void RDvbhHaiAsyncTest::SetupParameterData()
	{
	iInitialPlatform.iPlatform.iId = KInitialPlatformId;
	iInitialPlatform.iPlatform.iName = KInitialPlatformName;
	iInitialPlatform.iEsgRoot = KInet6AddrLinkLocal;
	
	iExpectedPlatform.iPlatform.iId = KExpectedPlatformId;
	iExpectedPlatform.iPlatform.iName = KExpectedPlatformName;	
	iExpectedPlatform.iEsgRoot = KDvbhTestIpV6Addr;
	}
	
void RDvbhHaiAsyncTest::InfoPrintf3(const TDesC16& aString, TInt aInt1, TInt aInt2)
	{
	INFO_PRINTF3(aString, aInt1, aInt2);
	}

void RDvbhHaiAsyncTest::InfoPrintf4(const TDesC16& aString, TInt aInt1, TInt aInt2, TInt aInt3)
	{
	INFO_PRINTF4(aString, aInt1, aInt2, aInt3);
	}
	
/**
* Allocs objects, defines properties, etc. as needed for each test step.
* Takes the role of the Preamble in synchornous tests.  It is called once
* when a test step is first kicked off.
*/
TVerdict RDvbhHaiAsyncTest::DoPreTest()
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
		
	if (iTestStepName.Compare(_L("MM-TV-DVBH-U-111")) == 0)		
		{
		result = iTestUtils->DefineDvbhStateProperty(KInitialState);		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-112")) == 0)		
		{
		 // Receiver state must be EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReceiving);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeySignalQuality, KInitialSignalQuality);			
			}				
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-113")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			TPckg<TDvbhPlatformProperty> initialPlatformPckg(iInitialPlatform);
			result = iTestUtils->DefineDvbhProperty(EDvbhPropertyKeyPlatform, CDvbhTestUtil::GetType(EDvbhPropertyKeyPlatform), initialPlatformPckg);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-114")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			TPckg<TTime> initialTimePckg(iInitialNetworkTime);
			result = iTestUtils->DefineDvbhProperty(EDvbhPropertyKeyNetworkTime, CDvbhTestUtil::GetType(EDvbhPropertyKeyNetworkTime), initialTimePckg);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-115")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyFrequency, KInitialFrequency);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-116")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyCellId, KInitialCellId);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-117")) == 0)		
		{
		 // Receiver state must be EDvbhStateReady or EDvbhStateReceiving before we can do this test.
		result = iTestUtils->DefineDvbhStateProperty(EDvbhStateReady);
		if (result == KErrNone)
			{
			result = iTestUtils->DefineDvbhIntegerProperty(EDvbhPropertyKeyNetworkId, KInitialNetworkId);			
			}		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-125")) == 0)		
		{
		result = iTestUtils->DefineDvbhStateProperty(KInitialState);
		if (result == KErrNone)
			{
			TRAP(result, i2ndStateObserver = CAlternativeStateObserver::NewL(*this));		
			}
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-126")) == 0)		
		{
		TRAP(result, i2ndStateObserver = CAlternativeStateObserver::NewL(*this));		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-127")) == 0)		
		{
		result = iTestUtils->DefineDvbhStateProperty(KInitialState);
		if (result == KErrNone)
			{
			TRAP(result, i2ndRxInfo = CDvbhReceiverInfo::NewL());
			if (result == KErrNone)
				{
				TRAP(result, i2ndStateObserver = CAlternativeStateObserver::NewL(*this));		
				}
			}
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-128")) == 0)		
		{
		TRAP(result, i2ndRxInfo = CDvbhReceiverInfo::NewL());
		if (result == KErrNone)
			{
			TRAP(result, i2ndStateObserver = CAlternativeStateObserver::NewL(*this));		
			}
		}
	else if (iTestStepName.Compare(_L("MM-TV-DVBH-U-401")) == 0)		
		{
		result = iTestUtils->DefineAllDvbhProperties();		
		}
	
	iPreTestRun = ETrue;	
	return (result == KErrNone ? EPass : EInconclusive);				
	}
	
void RDvbhHaiAsyncTest::CloseTest()
	{
	delete iRxInfo;
	iRxInfo = NULL;
	delete i2ndStateObserver;
	i2ndStateObserver = NULL;
	delete i2ndRxInfo;
	i2ndRxInfo = NULL;
	
	TInt result = KErrNone;

	if (iTestStepName.Compare(_L("MM-TV-DVBH-U-111")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-112")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeySignalQuality);	
		result = result2 != KErrNone ? result2 : result;		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-113")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyPlatform);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-114")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyNetworkTime);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-115")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyFrequency);	
		result = result2 != KErrNone ? result2 : result;		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-116")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyCellId);	
		result = result2 != KErrNone ? result2 : result;		
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-117")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		TInt result2 = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyNetworkId);
		result = result2 != KErrNone ? result2 : result;			
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-125")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-127")) == 0)		
		{
		result = iTestUtils->DeleteDvbhProperty(EDvbhPropertyKeyState);
		}	
	else if (iTestStepName.Compare(_L("MM-TV-DVBH-U-401")) == 0)		
		{
		result = iTestUtils->DeleteAllDvbhProperties();		
		}

	delete iTestUtils;
	iTestUtils = NULL;

	//Make sure we are not leaving anything laying around.  This is
	//to verify the integrity of the test code more than anything else.
	ASSERT(result == KErrNone);
	}
	
void RDvbhHaiAsyncTest::KickoffTestL()
	{
	//Run the pre-test set-up if it has not been run already
	if (!iPreTestRun)
		{
		TVerdict preTestResult = DoPreTest();
		if (preTestResult != EPass)
			{
			INFO_PRINTF2(_L("DoPreTest failed with error %d."), preTestResult);
			StopTest(preTestResult);
			return;
			}
		}
		
	if (iTestStepName.Compare(_L("MM-TV-DVBH-U-111")) == 0)		
		{
		DoSetStateObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-112")) == 0)		
		{
		DoSetSignalQualityObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-113")) == 0)		
		{
		DoSetPlatformObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-114")) == 0)		
		{
		DoSetNetworkTimeObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-115")) == 0)		
		{
		DoSetFrequencyObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-116")) == 0)		
		{
		DoSetCellIdObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-117")) == 0)		
		{
		DoSetNetworkIdObserverTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-118")) == 0)		
		{
		DoSetStateObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-119")) == 0)		
		{
		DoSetSignalQualityObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-120")) == 0)		
		{
		DoSetPlatformObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-121")) == 0)		
		{
		DoSetNetworkTimeObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-122")) == 0)		
		{
		DoSetFrequencyObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-123")) == 0)		
		{
		DoSetCellIdObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-124")) == 0)		
		{
		DoSetNetworkIdObserverTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-125")) == 0)		
		{
		DoSetMultipleStateObserversTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-126")) == 0)		
		{
		DoSetMultipleStateObserversTestStep(EFalse);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-127")) == 0)		
		{
		DoCreateMultipleReceiverInfoTestStep(ETrue);
		}
	else if	(iTestStepName.Compare(_L("MM-TV-DVBH-U-128")) == 0)		
		{
		DoCreateMultipleReceiverInfoTestStep(EFalse);
		}
	else if (iTestStepName.Compare(_L("MM-TV-DVBH-U-401")) == 0)		
		{
		DoSetObserversNoMemory();		
		}
	else
		{
		StopTest(EInconclusive);
		}
	}

//
//
// CDvbhReceiverInfo Positive Async Tests
//
//

// 111 & 118
void RDvbhHaiAsyncTest::DoSetStateObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetStateObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetStateObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
	
	if (aProperyExists)
		{
		//DoPreTest will have initialised the state to KInitialState.
		//Now change it to KExpectedState.
		if (iTestUtils->SetStateProperty(KExpectedState) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}
	
//Called when the state property changes if this state observer has been registerd
void RDvbhHaiAsyncTest::DvbhStateChange( TDvbhState aNewState )
	{
	TVerdict verdict = EPass;

	// We have to make sure that this observer was not called for the wrong test.
	if ( iTestStepName.Compare(_L("MM-TV-DVBH-U-125")) == 0 ||
		 iTestStepName.Compare(_L("MM-TV-DVBH-U-126")) == 0 )		
		{
		INFO_PRINTF1(_L("RDvbhHaiAsyncTest::DvbhStateChange called for wrong test."));
		verdict = EFail;
		}
	else
		{
		if (aNewState != KExpectedState)
			{
			INFO_PRINTF2(_L("DvbhStateChange received unexpected value %d."), aNewState);
			verdict = EFail;
			}
		else
			{
			// Test case MM-TV-DVBH-U-127 is expecting two callbacks on two different observers
			if (iTestStepName.Compare(_L("MM-TV-DVBH-U-127")) == 0)
				{
				// Set the verdict for this observer
				iStateCallbackVerdicts[E1stStateObserver] = verdict;
				//Check if the 2nd observer has reported back yet. If not then we
				//do not want to stop the test yet.
				if (iStateCallbackVerdicts[E2ndStateObserver] == KNoVerdict)
					{
					return;
					}
				else
					{
					verdict = static_cast<TVerdict>(iStateCallbackVerdicts[E2ndStateObserver]);
					}
				}		
			}		
		}
		
	StopTest(verdict);
	}	

// 112 & 119
void RDvbhHaiAsyncTest::DoSetSignalQualityObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetSignalQualityObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetSignalQualityObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
	
	if (aProperyExists)
		{
		//DoPreTest will have initialised the signal quality to KInitialSignalQuality.
		//Now change it to KExpectedSignalQuality.
		if (iTestUtils->SetIntegerProperty(EDvbhPropertyKeySignalQuality, KExpectedSignalQuality) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

//Called when the signal quality property changes this a signal quality observer has been registerd
void RDvbhHaiAsyncTest::DvbhSignalQualityChange( const TDvbhSignalQuality& aNewSignalQuality )
	{
	TVerdict verdict = EPass;
	if (aNewSignalQuality != KExpectedSignalQuality)
		{
		INFO_PRINTF2(_L("DvbhSignalQualityChange received unexpected value %d."), aNewSignalQuality);
		verdict = EFail;
		}
	StopTest(verdict);
	}

// 113 & 120
void RDvbhHaiAsyncTest::DoSetPlatformObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetPlatformObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetPlatformObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
	
	if (aProperyExists)
		{
		//DoPreTest will have initialised the platform to iInitialPlatform.
		//Now change it to iExpectedPlatform.
		TPckg<TDvbhPlatformProperty> platformPckg(iExpectedPlatform);
		if (iTestUtils->SetByteArrayProperty(EDvbhPropertyKeyPlatform, platformPckg) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

//Called when the platform property changes if this platform observer has been registerd
void RDvbhHaiAsyncTest::DvbhPlatformChange( const TDvbhPlatform& aNewPlatform, const TIp6Addr& aESGRoot )
	{
	TVerdict verdict = EPass;
	if ( !CDvbhTestUtil::ComparePlatforms(aNewPlatform, iExpectedPlatform.iPlatform))
		{
		INFO_PRINTF1(_L("DvbhPlatformChangeDvbhPlatformChange received unexpected platform."));
		verdict = EFail;
		}
	if (!aESGRoot.IsEqual(iExpectedPlatform.iEsgRoot))
		{
		INFO_PRINTF1(_L("DvbhPlatformChangeDvbhPlatformChange received unexpected esgRoot."));
		verdict = EFail;		
		}
	StopTest(verdict);
	}
	
// 114 & 121
void RDvbhHaiAsyncTest::DoSetNetworkTimeObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetNetworkTimeObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetNetworkTimeObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
	
	if (aProperyExists)
		{
		//DoPreTest will have initialised the NetworkTime to iInitialNetworkTime.
		//Now change it to KExpectedNetworkTime.
		TPckg<TTime> timePckg(iExpectedNetworkTime);	
		if (iTestUtils->SetByteArrayProperty(EDvbhPropertyKeyNetworkTime, timePckg) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

//Called when the network time property changes if this network time observer has been registerd
void RDvbhHaiAsyncTest::DvbhNetworkTimeUpdate()
	{
	// Since this method has no parameters, there are no variables for us to verify.
	// The fact that this method was called at all means that CDvbhReceiverInfo::SetNetworkTimeObserver()
	// worked correctly, and thus we can now stop the test as passed.
	StopTest(EPass);
	}
	
// 115 & 122
void RDvbhHaiAsyncTest::DoSetFrequencyObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetFrequencyObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetFrequencyObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
	
	if (aProperyExists)
		{
		//DoPreTest will have initialised the frequency to KInitialFrequency.
		//Now change it to KExpectedFrequency.
		if (iTestUtils->SetIntegerProperty(EDvbhPropertyKeyFrequency, KExpectedFrequency) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

//Called when the frequency property changes if this frequency observer has been registerd
void RDvbhHaiAsyncTest::DvbhFrequencyChange( const TDvbhFrequency& aNewFrequency )
	{
	TVerdict verdict = EPass;
	if (aNewFrequency != KExpectedFrequency)
		{
		INFO_PRINTF2(_L("DvbhFrequencyChange received unexpected value %d."), aNewFrequency);
		verdict = EFail;
		}
	StopTest(verdict);
	}
	
// 116 & 123
void RDvbhHaiAsyncTest::DoSetCellIdObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetCellIdObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetCellIdObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}

	if (aProperyExists)
		{
		//DoPreTest will have initialised the CellId to KInitialCellId.
		//Now change it to KExpectedCellId.
		if (iTestUtils->SetIntegerProperty(EDvbhPropertyKeyCellId, KExpectedCellId) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}			
	}

//Called when the CellId property changes if this CellId observer has been registerd
void RDvbhHaiAsyncTest::DvbhCellIdChange( const TDvbhCellId& aNewCellId )
	{
	TVerdict verdict = EPass;
	if (aNewCellId != KExpectedCellId)
		{
		INFO_PRINTF2(_L("DvbhCellIdChange received unexpected value %d."), aNewCellId);
		verdict = EFail;
		}
	StopTest(verdict);
	}
	
// 117 & 124
void RDvbhHaiAsyncTest::DoSetNetworkIdObserverTestStep(TBool aProperyExists)
	{
	TInt result = iRxInfo->SetNetworkIdObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetNetworkIdObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}

	if (aProperyExists)
		{
		//DoPreTest will have initialised the NetworkId to KInitialNetworkId.
		//Now change it to KExpectedNetworkId.
		if (iTestUtils->SetIntegerProperty(EDvbhPropertyKeyNetworkId, KExpectedNetworkId) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

//Called when the NetworkId property changes if this NetworkId observer has been registerd
void RDvbhHaiAsyncTest::DvbhNetworkIdChange( const TDvbhNetworkId& aNewNetworkId )
	{
	TVerdict verdict = EPass;
	if (aNewNetworkId != KExpectedNetworkId)
		{
		INFO_PRINTF2(_L("DvbhNetworkIdChange received unexpected value %d."), aNewNetworkId);
		verdict = EFail;
		}
	StopTest(verdict);
	}
	
// 125 & 126
void RDvbhHaiAsyncTest::DoSetMultipleStateObserversTestStep(TBool aProperyExists)
	{
	// Set a state observer
	TInt result = iRxInfo->SetStateObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("1st SetStateObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}

	// Now try to set another one
	result = iRxInfo->SetStateObserver(*i2ndStateObserver);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("2nd SetStateObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
			
	if (aProperyExists)
		{
		//DoPreTest will have initialised the state to KInitialState.
		//Now change it.
		if (iTestUtils->SetStateProperty(KExpectedState) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}
	}

 //127 & 128
void RDvbhHaiAsyncTest::DoCreateMultipleReceiverInfoTestStep(TBool aProperyExists)
	{
	// Set a state observer on the first CDvbhReceiverInfo object
	TInt result = iRxInfo->SetStateObserver(*this);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("1st SetStateObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}

	// Now try to set another one on the 2nd CDvbhReceiverInfo object 
	result = i2ndRxInfo->SetStateObserver(*i2ndStateObserver);
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("2nd SetStateObserver returned error %d."), result);
		StopTest(EFail);
		return;
		}
			
	if (aProperyExists)
		{
		iStateCallbackVerdicts[E1stStateObserver] = KNoVerdict;
		iStateCallbackVerdicts[E2ndStateObserver] = KNoVerdict;
		//DoPreTest will have initialised the state to KInitialState.
		//Now change it.
		if (iTestUtils->SetStateProperty(KExpectedState) != KErrNone)
			{
			StopTest(EInconclusive);
			return;		
			}		
		}
	else
		{
		StopTest(EPass);
		}	
	}
	
//
//
// RDvbhHaiAsyncTest::CAlternativeStateObserver
//
//

RDvbhHaiAsyncTest::CAlternativeStateObserver* RDvbhHaiAsyncTest::CAlternativeStateObserver::NewL(RDvbhHaiAsyncTest& aTestStep)
	{
	CAlternativeStateObserver* self = new (ELeave) CAlternativeStateObserver(aTestStep);
	return self;	
	}

RDvbhHaiAsyncTest::CAlternativeStateObserver::CAlternativeStateObserver(RDvbhHaiAsyncTest& aTestStep)
: iTestStep(aTestStep)
	{
	}
	
void RDvbhHaiAsyncTest::CAlternativeStateObserver::DvbhStateChange( TDvbhState aNewState )
	{
	TVerdict verdict = EPass;
	if (aNewState != KExpectedState)
		{
		iTestStep.INFO_PRINTF2(_L("CAlternativeStateObserver::DvbhStateChange received unexpected value %d."), aNewState);
		verdict = EFail;
		}
	else
		{
		// Test case MM-TV-DVBH-U-127 is expecting two callbacks on two different observers
		if (iTestStep.iTestStepName.Compare(_L("MM-TV-DVBH-U-127")) == 0)
			{
			// Set the verdict for this observer
			iTestStep.iStateCallbackVerdicts[E2ndStateObserver] = verdict;
			//Check if the 1st observer has reported back yet. If not then we
			//do not want to stop the test yet.
			if (iTestStep.iStateCallbackVerdicts[E1stStateObserver] == KNoVerdict)
				{
				return;
				}
			else
				{
				verdict = static_cast<TVerdict>(iTestStep.iStateCallbackVerdicts[E1stStateObserver]);
				}
			}		
		}
		
	iTestStep.StopTest(verdict);
	}
	

//401
/**
* Test to improve conditional code coverage.  Tests KErrNoMemory failures
* with SetObserver methods.
*/
void RDvbhHaiAsyncTest::DoSetObserversNoMemory()
	{
	//SetStateObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	TInt result = iRxInfo->SetStateObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetStateObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetSignalQualityObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetSignalQualityObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetSignalQualityObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetPlatformObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetPlatformObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetPlatformObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetNetworkTimeObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetNetworkTimeObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetNetworkTimeObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetFrequencyObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetFrequencyObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetFrequencyObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetCellIdObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetCellIdObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetCellIdObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	//SetNetworkIdObserver
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);
	result = iRxInfo->SetNetworkIdObserver(*this);
	if (result != KErrNoMemory)
		{
		INFO_PRINTF2(_L("SetNetworkIdObserver returned %d."), result);
		StopTest(EInconclusive);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		return;
		}
		
	__UHEAP_SETFAIL(RHeap::ENone, 0);
	StopTest(EPass);
	}







