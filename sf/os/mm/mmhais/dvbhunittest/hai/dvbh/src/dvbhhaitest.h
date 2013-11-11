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

#ifndef DVBHHAITEST_H
#define DVBHHAITEST_H

#include <testframework.h>
#include "dvbhhaitestutil.h"
#include <e32property.h>
#include <mobiletv/hai/dvbh/dvbhreceiver.h>
#include <mobiletv/hai/dvbh/dvbhreceiverinfo.h>

class CDvbhTestUtil;

class RDvbhHaiTest : public RTestStep,
					 public MDvbhHaiTestLoggerInterface,
					 public MDvbhScanObserver,
					 public MDvbhDataObserver
	{
public:
	friend class CDvbhTestUtil;
public:
	static RDvbhHaiTest* NewL(TBool aRequireReceiverInfo, const TDesC& aTestName);
protected:
	//From RTestStep
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	
	//From MDvbhHaiTestLoggerInterface
	void InfoPrintf3(const TDesC16& aString, TInt aInt1, TInt aInt2);
	void InfoPrintf4(const TDesC16& aString, TInt aInt1, TInt aInt2, TInt aInt3);
	
	//RDvbhReceiver Positive Test Steps
	TVerdict DoCreateReceiverOnStackTestStep();
	TVerdict DoCreateReceiverOnHeapTestStep();
	TVerdict DoGetDriverVersionBeforeOpenTestStep();
	TVerdict DoReceiverOpenTestStep();
	TVerdict DoMultipleReceiverOpenTestStep();
	TVerdict DoGetDriverVersionAfterOpenTestStep();
	TVerdict DoPowerOnAfterOpenTestStep();
	TVerdict DoSetScanConfigurationAfterOpenTestStep();
	TVerdict DoGetScanConfigurationAfterOpenTestStep();
	TVerdict DoGetDvbhVersionAfterOpenTestStep();
	TVerdict DoGetHardwareInfoAfterOpenTestStep();
	TVerdict DoScanAfterOpenTestStep(); 				//
	TVerdict DoSetPlatformAfterOpenTestStep();  		//
	TVerdict DoCreateFilterAfterOpenTestStep();			//
	TVerdict DoCancelFilterAfterOpenTestStep();			//
	TVerdict DoReceiveIpDataAfterOpenTestStep();		//
	TVerdict DoUpdateNetworkTimeAfterOpenTestStep();	//
	TVerdict DoCustomAsyncAfterOpenTestStep();			//
	TVerdict DoCancelCustomAsyncAfterOpenTestStep();	//
	TVerdict DoCustomAfterOpenTestStep();
	TVerdict DoCloseAfterOpenTestStep();
	TVerdict DoPowerOffAfterOpenTestStep();
	TVerdict DoSetDisabledAfterOpenTestStep();
	TVerdict DoCancelScanAfterOpenTestStep();
	TVerdict DoCancelSetPlatformAfterOpenTestStep();
	TVerdict DoCancelReceiveIpDataAfterOpenTestStep();
	TVerdict DoCancelUpdateNetworkTimeAfterOpenTestStep();
	TVerdict DoMultipleReceiverCloseTestStep();
	
	//CDvbhReceiverInfo Positive Test Steps
	TVerdict DoCreateReceiverInfoTestStep();
	
	TVerdict DoGetStateTestStep();
	TVerdict DoGetSignalQualityTestStep();
	TVerdict DoGetPlatformTestStep();
	TVerdict DoGetNetworkTimeTestStep();
	TVerdict DoGetPerformanceDataTestStep();
	TVerdict DoGetFrequencyTestStep();
	TVerdict DoGetCellIdTestStep();
	TVerdict DoGetNetworkIdTestStep(); //110
	
	//RDvbhReceiver Negative Test Steps
	TVerdict DoReceiverOpenNoCloseTestStep();
	TVerdict DoPowerOnBeforeOpenTestStep();
	TVerdict DoSetScanConfigurationBeforeOpenTestStep();
	TVerdict DoGetScanConfigurationBeforeOpenTestStep();
	TVerdict DoGetDvbhVersionBeforeOpenTestStep();
	TVerdict DoGetHardwareInfoBeforeOpenTestStep();
	TVerdict DoScanBeforeOpenTestStep(); 				//
	TVerdict DoSetPlatformBeforeOpenTestStep();  		//
	TVerdict DoCreateFilterBeforeOpenTestStep();		//
	TVerdict DoCancelFilterBeforeOpenTestStep();		//
	TVerdict DoReceiveIpDataBeforeOpenTestStep();		//
	TVerdict DoUpdateNetworkTimeBeforeOpenTestStep();	//
	TVerdict DoCustomAsyncBeforeOpenTestStep();			//
	TVerdict DoCancelCustomAsyncBeforeOpenTestStep();	//
	TVerdict DoCustomBeforeOpenTestStep(); //215
	TVerdict DoCloseBeforeOpenTestStep(); //216
	TVerdict DoPowerOffBeforeOpenTestStep(); //217
	TVerdict DoSetDisabledBeforeOpenTestStep(); //218
	TVerdict DoCancelScanBeforeOpenTestStep(); //219
	TVerdict DoCancelSetPlatformBeforeOpenTestStep(); //220
	TVerdict DoCancelReceiveIpDataBeforeOpenTestStep(); //221
	TVerdict DoCancelUpdateNetworkTimeBeforeOpenTestStep(); //222
		
	//CDvbhReceiverInfo Negative Test Steps	
	TVerdict DoGetStateNoPropertiesTestStep(); //301
	TVerdict DoGetSignalQualityNoPropertiesTestStep(); //302
	TVerdict DoGetPlatformNoPropertiesTestStep(); //303
	TVerdict DoGetNetworkTimeNoPropertiesTestStep(); //304
	TVerdict DoGetPerformanceDataNoPropertiesTestStep(); //305
	TVerdict DoGetFrequencyNoPropertiesTestStep(); //306
	TVerdict DoGetCellIdNoPropertiesTestStep(); //307
	TVerdict DoGetNetworkIdNoPropertiesTestStep(); //308
	TVerdict DoGetTestsNoProperties(); //309
	
	//MDvbhScanObserver
	void DvbhScanProgress( const TDvbhScanResult& aResult );
	
	//MDvbhDataObserver
	void DvbhPacketReceived( const TDesC8& aPacket );
private:
	RDvbhHaiTest(TBool aRequireReceiverInfo, const TDesC& aTestName);
	
	//helpers
	TVerdict DoReceiverOpenAndClose(TUint aMultiples = 1);
	TVerdict DoGetDriverVersion(RDvbhReceiver& aReceiver);
	TVerdict OpenReceiver(RDvbhReceiver& aReceiver);
	TVerdict DoGetNetworkTime(TBool aExpectedValidity);
	
	void SetupParameterData();
private:
	TVersion 				iDvbhTestDriverVersion; //treat as a constant
	TTime					iDvbhTestTime; //treat as a constant
	TDvbhPerformanceData	iDvbhTestPerformanceData; //treat as a constant
	TDvbhScanConfiguration	iDvbhTestScanConfiguration; //treat as a constant
	TDvbhPlatformProperty	iDvbhTestPlatform; //treat as a constant
	TDvbhHardwareInfo		iDvbhTestHardwareInfo; //treat as a constant
	
	TDvbhNetwork			iDvbhTestNetwork; //Needed to run constructor (ccov)
	TDvbhNetworkTime		iDvbhTestNetworkTime; //For GetNetworkTime
	TDvbhScanResult			iDvbhTestScanResult; //Needed to run constructor (ccov)
	
	TBool					iRequireReceiverInfo;
	CDvbhReceiverInfo*		iRxInfo;
	CDvbhTestUtil*			iTestUtils;
	TDvbhReceiverType      iReceiverType;
	};


#endif // DVBHHAITEST_H
