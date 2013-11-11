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
//

#include "Te_SimTestStepBase.h"
#include <testconfigfileparser.h>
#include <etelmm.h>
#include "SimConstants.h"

const TInt KUsimGranularity=2;		// < Granularity for sat list array

TVerdict CSimTestsTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iActiveScheduler=new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iActiveScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iActiveScheduler;
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimTestsTestStepBase::doTestStepPostambleL()
	{
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();
	delete iActiveScheduler;

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimTestsTestStepBase::SetTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Changing the Test Number to %d"),aTestNumber);
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber);
	TESTL(ret == KErrNone);
	ASSERT(RThread().RequestCount()==0);
  	}

void CSimTestsTestStepBase::CreateConfigFileL(const TDesC& aConfigFileName)
	{
	RFs fs;
	TInt ret=fs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to file system"));
		SetTestStepResult(EFail);
		User::LeaveIfError(ret);
		}
	CFileMan* fileMan=CFileMan::NewL(fs);
	ret = fileMan->Copy(aConfigFileName, KConfigFilename);
	delete fileMan;
	fs.Close();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to copy config file"));
		SetTestStepResult(EFail);
		User::Leave(ret);
		}
	}

void CSimTestsTestStepBase::DeleteConfigFileL()
	{
	RFs fs;
	TInt ret=fs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to file system"));
		SetTestStepResult(EFail);
		User::Leave(ret);
		}
	fs.Delete(KConfigFilename);
	fs.Close();
	}

TVerdict CPhBkAndPacketTestsTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CPhBkAndPacketTestsTestStepBase::doTestStepPostambleL()
	{
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CPhBkAndPacketTestsTestStepBase::SetTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Changing the Test Number to %d"),aTestNumber);
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber);
	TESTL(ret == KErrNone);
	ASSERT(RThread().RequestCount()==0);
  	}

void CPhBkAndPacketTestsTestStepBase::CreateConfigFileL(const TDesC& aConfigFileName)
	{
	RFs fs;
	TInt ret=fs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to file system"));
		SetTestStepResult(EFail);
		User::LeaveIfError(ret);
		}
	CFileMan* fileMan=CFileMan::NewL(fs);
	ret = fileMan->Copy(aConfigFileName, KConfigFilename);
	delete fileMan;
	fs.Close();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to copy config file"));
		SetTestStepResult(EFail);
		User::Leave(ret);
		}
	}
	
void CPhBkAndPacketTestsTestStepBase::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
/**
 * Timeout function
 */
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}
	
void CPhBkAndPacketTestsTestStepBase::SignalSimTsyToReduceTimerL(TInt aTimerEventId)
	{
	RProperty reduceTimersProperty;
	User::LeaveIfError(reduceTimersProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime));
	CleanupClosePushL(reduceTimersProperty);

	User::LeaveIfError(reduceTimersProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
												aTimerEventId));

	CleanupStack::PopAndDestroy(&reduceTimersProperty);
	}

LOCAL_C TInt NTRasThreadFunction(TAny* /*aThreadData*/)
	{
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName,"LOOPBACK::1");

	_LIT8(KServerText,"SERVER");

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;

	RCommServ commServ;
	TInt r=commServ.Connect();
	r=commServ.LoadCommModule(KCsyName);
	RComm comm;
	r=comm.Open(commServ,KPortName,ECommShared);

	TBuf8<6> readBuf;
	TRequestStatus stat0;
	comm.Read(stat0,readBuf);
	User::WaitForRequest(stat0) ;

	comm.Write(stat0,KServerText);
	User::WaitForRequest(stat0) ;


	delete cleanup;
	ASSERT(RThread().RequestCount()==0);
	return r;
	}

TInt StartNTRasSimulation()
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

	_LIT(KNtRasName,"NTRasEmulation");

	RThread t;
	TInt res=t.Create(KNtRasName,NTRasThreadFunction,
					KStackSize,KHeapSize,KMaxHeapSize,NULL);
	t.Resume();
	t.Close();
	ASSERT(RThread().RequestCount()==0);
	return res;
	}

TVerdict CSimUSimRel6TestsTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iActiveScheduler=new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iActiveScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iActiveScheduler;
		User::Leave(ret);
		}
	
	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(46);
	(void)User::LeaveIfError(iFs.Connect());
 	iConfigFile=CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilename);

	TInt testNumber;
	testNumber=46;
	iSectionName.Format(KSectionNameFormat,testNumber);
	if(iConfigFile->Section(iSectionName)==NULL)
		{
		CleanupStack::Pop();
		User::Leave(KErrNotFound);
		}
	CleanupStack::Pop();	// iConfigFile pointer is safely stored as a member variable

    iVoiceMailIds=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneVoicemailIdsV8>(KUsimGranularity);
	iMessageWaiting=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneMessageWaitingV8>(KUsimGranularity);
 	iWlanData=new(ELeave) CArrayFixFlat<RMobilePhone::TUsimWlanDataV8>(KUsimGranularity);
 	iWlanSid=new(ELeave) CArrayFixFlat<RMobilePhone::TWlanSIDV8>(KUsimGranularity);
 	iPreferredNetworks=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePreferredNetworkEntryV3>(KUsimGranularity);
 	TInt count;
	count=CfgFile()->ItemCount(KUsimMailBoxIdInfo);
	const CTestConfigItem* item=NULL;
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUsimMailBoxIdInfo,i);
		if(!item)
			break;

		TInt voice,data,fax,other,video;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,voice);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for voice identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,data);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for data identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,fax);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for fax identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,other);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for other identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,video);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for video identifier"));
			continue;
			}
		RMobilePhone::TMobilePhoneVoicemailIdsV8 mailboxIdInfo;
		
		mailboxIdInfo.iVoice=voice;
		mailboxIdInfo.iData=data;
		mailboxIdInfo.iFax=fax;
		mailboxIdInfo.iOther=other;	
		mailboxIdInfo.iVideo=video;
		iVoiceMailIds->AppendL(mailboxIdInfo);
		}

	count=CfgFile()->ItemCount(KUsimMessageWaitingIdInfo);
	item=NULL;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUsimMessageWaitingIdInfo,i);
		if(!item)
			break;

		TInt displayStatus,voiceMsgs,auxVoiceMsgs,dataMsgs,faxMsgs,emailMsgs,otherMsgs,videoMsgs;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,displayStatus);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Display Status identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,voiceMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Voice Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,auxVoiceMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Auxillary Voice Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,dataMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Data Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,faxMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Fax Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,emailMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Email Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,otherMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Other Messages identifier"));
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,videoMsgs);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Video Messages identifier"));
			continue;
			}
		RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaiting;
		
		messageWaiting.iDisplayStatus = RMobilePhone::TMobilePhoneIndicatorDisplay(displayStatus);
		messageWaiting.iVoiceMsgs = voiceMsgs;
		messageWaiting.iAuxVoiceMsgs = auxVoiceMsgs;
		messageWaiting.iDataMsgs = dataMsgs;
		messageWaiting.iFaxMsgs = faxMsgs;
		messageWaiting.iEmailMsgs = emailMsgs;
		messageWaiting.iOtherMsgs = otherMsgs;
		messageWaiting.iVideoMsgs = videoMsgs;
		iMessageWaiting->AppendL(messageWaiting);
		}
	
	count=CfgFile()->ItemCount(KUsimWlanDataInfo);
	item=NULL;
	RMobilePhone::TUsimWlanDataV8 wlanData;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUsimWlanDataInfo,i);
		if(!item)
			break;

		TPtrC8 FieldsUsed,Pseudonym,ReauthenticationId,MasterKey,Counter;
		if(i==0)
			{
			wlanData.iFieldsUsed=SIMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
			}
		else
			{
			wlanData.iFieldsUsed=SIMTSY_PHONE_WLAN_LIST_DATA_FLAGS1;	
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,Pseudonym);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Pseudonym identifier"));
			continue;
			}
		else
			{
			wlanData.iPseudonym.Copy(Pseudonym);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,ReauthenticationId);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Reauthentication identifier"));
			continue;
			}
		else
			{
			wlanData.iReauthenticationId.Copy(ReauthenticationId);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,MasterKey);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Master Key identifier"));
			continue;
			}
		else
			{
			wlanData.iMasterKey.Copy(MasterKey);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,Counter);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Counter identifier"));
			continue;
			}
		else
			{
			wlanData.iCounter.Copy(Counter);
			}
		iWlanData->AppendL(wlanData);
		}
	
	count=CfgFile()->ItemCount(KUsimWlanSidInfo);
	item=NULL;
	RMobilePhone::TWlanSIDV8 wlanSid;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUsimWlanSidInfo,i);
		if(!item)
			break;

		TPtrC8 wlanSidBuf;
		TInt userDef;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,wlanSidBuf);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Wlan SID identifier"));
			continue;
			}
		else 
			{
			wlanSid.iWSID.Copy(wlanSidBuf);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,userDef);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for User Defined identifier"));
			continue;
			}
		else
			{
			wlanSid.iUserDefined=userDef;
			}
		iWlanSid->AppendL(wlanSid);
		}
	
	count=CfgFile()->ItemCount(KUsimPreferredNetworksInfo);
	item=NULL;
	RMobilePhone::TMobilePreferredNetworkEntryV3 networkEntry;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUsimPreferredNetworksInfo,i);
		if(!item)
			break;

		TPtrC8 accessId,countryCode,networkId;
		TInt userDef;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,userDef);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for User Defined identifier"));
			continue;
			}
		else
			{
			networkEntry.iUserDefined=userDef;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,countryCode);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Country code identifier"));
			continue;
			}
		else
			{
			networkEntry.iCountryCode.Copy(countryCode);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,networkId);
		if(ret!=KErrNone)
			{
			INFO_PRINTF1(_L("Failed to get the value for Network Id identifier"));
			continue;
			}
		else
			{
			networkEntry.iNetworkId.Copy(networkId);
			}
		iPreferredNetworks->AppendL(networkEntry);
		}
			
	//Open the session
	ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
  	INFO_PRINTF1(_L("Opened phone object"));
	return TestStepResult();
	}

TVerdict CSimUSimRel6TestsTestStepBase::doTestStepPostambleL()
	{
	iPhone.Close();
	delete iConfigFile;
	delete iVoiceMailIds;
	delete iMessageWaiting;
	iFs.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();
	delete iWlanData;
	delete iWlanSid;
	delete iPreferredNetworks;
	delete iActiveScheduler;
	delete iWlanList;
	delete iNetworkList;
	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimUSimRel6TestsTestStepBase::SetTestNumberL(TInt aTestNumber)
  	{
	INFO_PRINTF2(_L("Changing the Test Number to %d"),aTestNumber);
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber);
	TESTL(ret == KErrNone);
	ASSERT(RThread().RequestCount()==0);
  	}

void CSimUSimRel6TestsTestStepBase::CreateConfigFileL(const TDesC& aConfigFileName)
	{
	RFs fs;
	TInt ret=fs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to file system"));
		SetTestStepResult(EFail);
		User::LeaveIfError(ret);
		}
	CFileMan* fileMan=CFileMan::NewL(fs);
	ret = fileMan->Copy(aConfigFileName, KConfigFilename);
	delete fileMan;
	fs.Close();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to copy config file"));
		SetTestStepResult(EFail);
		User::Leave(ret);
		}
	}

void CSimUSimRel6TestsTestStepBase::DeleteConfigFileL()
	{
	RFs fs;
	TInt ret=fs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect to file system"));
		SetTestStepResult(EFail);
		User::Leave(ret);
		}
	fs.Delete(KConfigFilename);
	fs.Close();
	}
	
const CTestConfigSection* CSimUSimRel6TestsTestStepBase::CfgFile()
/**
*	Return a pointer to the Configuration File Section
*
* @return CTestConfigSection	pointer to the configuration file section
*/
	{
	return iConfigFile->Section(iSectionName);
	}

const CTestConfigSection* CSimUSimRel6TestsTestStepBase::DefaultCfgFile()
/**
*	Return a pointer to the Default Configuration File Section
*
* @return CTestConfigSection	pointer to the default configuration file section
*/
	{
	return iConfigFile->Section(KScriptDefaults);
	}	

void CSimUSimRel6TestsTestStepBase::TestSmartCardAuthenticate()
	{
   	TRequestStatus reqStatus;
   	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScWlanAuthAUTN();
	authenticateData.iRAND = KConfigScWlanAuthRAND();
	authenticateData.iCK = KConfigScWlan();
	
	RMobilePhone::TAID appAID = KWlanAid();
	iPhone.SmartCardAuthenticate(reqStatus,appAID,authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);
	TESTL(authenticateData.iRES==KConfigScWlanAuthRES());
	TESTL(authenticateData.iCK==KConfigScWlanAuthCK());
	TESTL(authenticateData.iIK==KConfigScWlanAuthIK());
	}
	
