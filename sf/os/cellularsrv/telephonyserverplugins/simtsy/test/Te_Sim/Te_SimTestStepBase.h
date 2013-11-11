// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SIM_TESTSTEP_BASE__
#define __TE_SIM_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <etelpckt.h>

#include <e32test.h>
#include <c32comm.h>
#include <e32keys.h>
#include <f32file.h>

#include <etel.h>
#include <etelmm.h>
#include <etelmmerr.h>
#include <mmretrieve.h>
#include <pcktcs.h>
#include <etelqos.h>
#include <etelsat.h>
#include "SimTsy.h"
#include <satcs.h>
#include "Te_SimConstants.h"


class CTestConfig;
class CTestConfigSection;
class CSimPhone;

#define CHECKPOINTL(code,expected,msg){\
	if((code)!=(expected)){\
		INFO_PRINTF1(msg);\
		SetTestStepResult(EFail);\
		User::Leave(KErrGeneral);\
		}\
	}
#define CHECKPOINT_EXL(boolexpr,msg){\
	if(!(boolexpr)){\
		INFO_PRINTF1(msg);\
		SetTestStepResult(EFail);\
		User::Leave(KErrGeneral);\
		}\
	}

_LIT(KSimtsyName,"SIM");

class CSimTestsTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void SetTestNumberL(TInt aTestNumber);
	void CreateConfigFileL(const TDesC& aConfigFileName);
	void DeleteConfigFileL();

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;

private:
	CActiveScheduler* iActiveScheduler;
	};

class CPhBkAndPacketTestsTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void SetTestNumberL(TInt aTestNumber);
	void CreateConfigFileL(const TDesC& aConfigFileName);
   	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
	void SignalSimTsyToReduceTimerL(TInt aTimerEventId = ETimerIdAllTimers);
	
protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	};

class CSimUSimRel6TestsTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	const CTestConfigSection* CfgFile();		// < Accessor function for the configuration file.
	const CTestConfigSection* DefaultCfgFile();		// < Accessor function for the default configuration file.
	void TestSmartCardAuthenticate();
protected:
	void SetTestNumberL(TInt aTestNumber);
	void CreateConfigFileL(const TDesC& aConfigFileName);
	void DeleteConfigFileL();
	
protected:
	RTelServer iTelServer;
		
public:
	CActiveScheduler* iActiveScheduler;
	RMobilePhone iPhone;
	RFs											iFs;				//< Connection to file server for config file access
	CTestConfig*             					iConfigFile;        //< Pointer to the Configuration file reader
	TBuf8<KMaxName>          					iSectionName;
	CArrayFixFlat<RMobilePhone::TMobilePhoneMessageWaitingV8>* iMessageWaiting;	//< Array containing the Message Waiting Indicators
	CArrayFixFlat<RMobilePhone::TMobilePhoneVoicemailIdsV8>* iVoiceMailIds;     //< Array containing MailBox Numbers
	CArrayFixFlat<RMobilePhone::TUsimWlanDataV8>* iWlanData;					//< Array containing Wlan Data.
	CArrayFixFlat<RMobilePhone::TWlanSIDV8>* iWlanSid;
	CArrayFixFlat<RMobilePhone::TMobilePreferredNetworkEntryV3>* iPreferredNetworks;
	CMobilePhoneStoredWlanSIDList* iWlanList;
	CMobilePhoneStoredNetworkList* iNetworkList;
	};
	
#endif // __TE_SIM_TESTSTEP_BASE__
