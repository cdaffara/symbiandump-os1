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

/**
 @file
*/

#include "Te_SimPacketNotify2ThreadTest.h"

CSimPacketNotify2ThreadTest::CSimPacketNotify2ThreadTest() 
	{ 
	SetTestStepName(_L("Notify2ThreadTest"));
	}

TVerdict CSimPacketNotify2ThreadTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketNotify2ThreadTest"));
	CreateConfigFileL(_L("c:\\config3.txt"));
 	
 	SetTestNumberL(6);

	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

	_LIT(KGprsThreadName,"GprsThread");
	_LIT(KRel99ThreadName,"Rel99Thread");

	RThread gprs;
	RThread rel99;

	iGprsMutex.CreateLocal();

	gprs.Create(KGprsThreadName,GprsThreadEntry,
					KStackSize,KHeapSize,KMaxHeapSize,this);

	rel99.Create(KRel99ThreadName,Rel99ThreadEntry,
					KStackSize,KHeapSize,KMaxHeapSize,this);
					
	//start the threads
	gprs.Resume();
	rel99.Resume();
	
	while (iMainFlag==EFalse) //loop until threads have finished.
		User::After(5000);	

	iGprsMutex.Close();
	
	gprs.Close();
	rel99.Close();
	return TestStepResult();
	}

TInt CSimPacketNotify2ThreadTest::GprsThreadEntry(TAny* aThreadData)
	{
	CSimPacketNotify2ThreadTest* thisptr = static_cast<CSimPacketNotify2ThreadTest*>(aThreadData);
	thisptr->GprsNotifyFunction();
	ASSERT(RThread().RequestCount()==0);
	return(KErrNone);
	}

TInt CSimPacketNotify2ThreadTest::Rel99ThreadEntry(TAny* aThreadData)
	{
	CSimPacketNotify2ThreadTest* thisptr = static_cast<CSimPacketNotify2ThreadTest*>(aThreadData);
	thisptr->Rel99NotifyFunction();
	ASSERT(RThread().RequestCount()==0);
	return(KErrNone);
	}

void CSimPacketNotify2ThreadTest::GprsNotifyFunction()
	{
	TRequestStatus gprsReqStatus;
	TRequestStatus gprsNotifyStatus;
	RTelServer gprsServer;
	RMobilePhone gprsPhone;

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return;

	// Open Etel
	TESTL(gprsServer.Connect()==KErrNone);
	TESTL(gprsServer.LoadPhoneModule(KSimtsyName)==KErrNone);

	// Open phone
	TESTL(gprsPhone.Open(gprsServer,KPhoneName)==KErrNone); 

	// Open GPRS packet service
	TESTL(iGPRSPacketService.Open(gprsPhone)==KErrNone);

	// Open GPRS context
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iGPRSPacketContext.OpenNewContext(iGPRSPacketService, gprsContextName)==KErrNone);
	
    iNewContextOpened = ETrue;
    
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	
	// Configure GPRS context.
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);
	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = ETrue;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	iGPRSPacketContext.NotifyConfigChanged(gprsNotifyStatus, contextConfigPckgGPRS);
	User::After(500000);
	iGprsMutex.Wait();
			
	iGPRSPacketContext.SetConfig(gprsReqStatus, contextConfigPckgGPRS);
	User::WaitForRequest(gprsNotifyStatus);
	User::WaitForRequest(gprsReqStatus);
	iGprsMutex.Signal();

	// Close
	iGPRSPacketContext.Close();
	iGPRSPacketService.Close();
	gprsPhone.Close();
	gprsServer.UnloadPhoneModule(KSimtsyName);
	gprsServer.Close();	// disconnect from etel
	delete cleanup;
	iThreadFlag=ETrue;//marks that this thread has finished
	ASSERT(RThread().RequestCount()==0);
	}

void CSimPacketNotify2ThreadTest::Rel99NotifyFunction()
	{
	TRequestStatus rel99ReqStatus;
	TRequestStatus rel99NotifyStatus;
	RTelServer rel99Server;
	RMobilePhone rel99Phone;

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return;
		
	// Open Etel
		TESTL(rel99Server.Connect()==KErrNone);
		TESTL(rel99Server.LoadPhoneModule(KSimtsyName)==KErrNone);
		
	// Open phone
		TESTL(rel99Phone.Open(rel99Server,KPhoneName)==KErrNone); 
  // Open GPRS packet service
	TESTL(iRel99PacketService.Open(rel99Phone)==KErrNone);
 //Open Rel99 Context
	TName rel99ContextNameCompare;
	
	rel99ContextNameCompare.Append(KSimPrimaryPacketContextName);
	
	rel99ContextNameCompare.AppendNum(1);
	
	while(iNewContextOpened==EFalse)
		User::After(50000);		
	
	TESTL(iRel99PacketContext.OpenExistingContext(iRel99PacketService, rel99ContextNameCompare)==KErrNone);
	
	RDebug::Print(_L("ThreadEntry9"));
	
	//Configure Rel99 Context	
	RPacketContext::TContextConfigR99_R4 rel99ContextConfig;
	
	TESTL(rel99ContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgRel99(rel99ContextConfig);
	rel99ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel99ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel99ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	rel99ContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	rel99ContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	rel99ContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	iRel99PacketContext.NotifyConfigChanged(rel99ReqStatus, contextConfigPckgRel99);
	RDebug::Print(_L("ThreadEntry8"));
	iGprsMutex.Wait();	
	iRel99PacketContext.SetConfig(rel99ReqStatus, contextConfigPckgRel99);
	User::WaitForRequest(rel99NotifyStatus);
	User::WaitForRequest(rel99ReqStatus);	
	iGprsMutex.Signal();
	// Close
	iRel99PacketContext.Close();
	iRel99PacketService.Close();
	while(iThreadFlag==EFalse)	//wait until the other thread has finished.
		User::After(50000);
	rel99Phone.Close();
	rel99Server.UnloadPhoneModule(KSimtsyName);
	rel99Server.Close();	// disconnect from etel
	delete cleanup;
	iMainFlag=ETrue; //mark to say this thread (and hence the test) has finished.
	ASSERT(RThread().RequestCount()==0);
	}
	

