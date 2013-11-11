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
// base class for CTSY component tests
// 
//

/**
 @file 
*/

#include "cctsycomponenttestbase.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <pcktcs.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/rmmcustomapi.h>
#include "cctsycustomipcfu.h"


_LIT(KDoubleColon, "::");

/**
To push a CleanupItem ont the cleanupstack to close the phone
*/
void CCtsyComponentTestBase::Cleanup(TAny* aSelf)
	{
	static_cast<CCtsyComponentTestBase*>(aSelf)->DoCleanup();
	}
	
/**
Close the Phone a the server connection
*/	
void CCtsyComponentTestBase::DoCleanup()
	{
	iPhone.Close();
	iMockLTSY.Close();
	
	TInt ret = iTelServer.UnloadPhoneModule(KMmTsyModuleName);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to unload phone module"));
		}

	iTelServer.Close();
	}

/**
Assert MockLtsy Terminated all handlings without errors
*/
void CCtsyComponentTestBase::AssertMockLtsyStatusL()
	{
	TBool hasWaitingExpect;
	TBool hasPendingComplete;
	TBool hasError;
	iMockLTSY.GetStatus(hasWaitingExpect,hasPendingComplete,hasError);
	if (hasWaitingExpect || hasPendingComplete)
		{
		ERR_PRINTF1(_L("MockLTsy still have pending/waiting events!"));
		}
	if (hasWaitingExpect || hasPendingComplete || hasError)
		{
		TBuf<255> log;
		do	{
			iMockLTSY.GetNextLogLine(log);
			INFO_PRINTF1(log);
			} 
		while(log.Length() > 0);
		User::Leave(KErrTEFUnitFail); // Fail the test
		}
	}

	
/**
Open Server session
*/
void CCtsyComponentTestBase::OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient)
	{
	TInt ret = iTelServer.Connect();

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect phone"));
		User::Leave(ret);
		}

	ret = iTelServer.LoadPhoneModule(KMmTsyModuleName);

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
		
	if(aSetExtendedErrorClient == EUseExtendedError)
		{
		iTelServer.SetExtendedErrorGranularity(	RTelServer::EErrorExtended );
		}
	}

/**
Standard function to open the phone up to the end of the boot sequence
*/
void CCtsyComponentTestBase::OpenPhoneL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	//Initialize local variables first:
	//EMobilePhoneGetHomeNetwork
    RMobilePhone::TMobilePhoneNetworkInfoV8 homeNetwork;
    homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
    homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
    homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
    homeNetwork.iCountryCode = _L("234");
    homeNetwork.iCdmaSID = _L("");
    homeNetwork.iAnalogSID = _L("");
    homeNetwork.iNetworkId = _L("23499");
    homeNetwork.iDisplayTag = _L("symbian");
    homeNetwork.iShortName = _L("symbian");
    homeNetwork.iLongName = _L("symbian mobile");
    homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
    homeNetwork.iEgprsAvailableIndicator = ETrue;
    homeNetwork.iHsdpaAvailableIndicator = ETrue;
    homeNetwork.iHsupaAvailableIndicator = ETrue;
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV8> homeNetworkData(homeNetwork);
    homeNetworkData.SerialiseL(data);

    TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	
	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// EMmTsyBootNotifyModemStatusReadyIPC
    ASSERT_EQUALS(KErrNone, iMockLTSY.PauseCompletion());
	iMockLTSY.CompleteL(EMmTsyBootNotifyModemStatusReadyIPC,KErrNone);

	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus,KErrNone,0);
	
	// EMmTsyBootNotifySimStatusReadyIPC
	iMockLTSY.ExpectL(EMmTsyBootNotifySimStatusReadyIPC);
	iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC,KErrNone,0);
                        
	// EMobilePhoneGetHomeNetwork
	iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
	iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone,data,0);

	// EMmTsyPhoneGetPin1DisableSupportedIPC
 	iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
    
 	// EMmTsySimRefreshRegisterIPC
 	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
 	
	// EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	data.Close();
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);
  
	// EMobilePhoneGetALSLine
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine);
  
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
  
	// EMobilePhoneGetIccMessageWaitingIndicators
    iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);

    //ECustomCheckAlsPpSupportIPC
    iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);

    //EMobilePhoneGetCustomerServiceProfile
    iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
    
	
	// Complete for EMmTsyPhoneGetPin1DisableSupportedIPC 
 	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	data.Close();
	pin1DisableSupportData.SerialiseL(data);
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC,KErrNone,data,0);

	// Complete for EMmTsySimRefreshRegisterIPC
	iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);		

	// Complete for EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	data.Close();
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable,KErrNone,data,0);
	
	
	// Complete for EMobilePhoneGetALSLine
	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine,KErrNone,data,0);

	
	
	// Complete for EMobilePhoneGetIccMessageWaitingIndicators
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
	data.Close();
	indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);
	
	//Complete for ECustomCheckAlsPpSupportIPC
	RMmCustomAPI::TAlsSupport alsSupport(RMmCustomAPI::EAlsSupportOff); 
	TMockLtsyData1< RMmCustomAPI::TAlsSupport > alsSupportData(alsSupport);
	data.Close();	
    alsSupportData.SerialiseL(data);
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, data);
	
	// Complete for EMobilePhoneGetCustomerServiceProfile
	RMobilePhone::TMobilePhoneCspFileV1 completeCsp;
	completeCsp.iCallOfferingServices = 1;
	completeCsp.iCallRestrictionServices = 2;
	completeCsp.iOtherSuppServices = 3;
	completeCsp.iCallCompletionServices = 4;
	completeCsp.iTeleservices = 5;
	completeCsp.iCphsTeleservices = 6;
	completeCsp.iCphsFeatures = 7;
	completeCsp.iNumberIdentServices = 8;
	completeCsp.iPhase2PlusServices = 9;
	completeCsp.iValueAddedServices = 10;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCspFileV1> completeLtsyData(completeCsp);
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrNone, data, 10);	

    ASSERT_EQUALS(KErrNone, iMockLTSY.ResumeCompletion());
	User::WaitForRequest(mockLtsyStatus);

	CleanupStack::PopAndDestroy(1); // data
	}

/**
Dial  call
@param aCall the call to use to dial
@param aService type of call (must match the line the call what created on)
@param aCallId callId ltsy must allocate for this call
*/
void CCtsyComponentTestBase::DialL(RCall& aCall, const TInt aCallId, const RMobilePhone::TMobileService aService)
	{
	DialL(aCall, aCallId, aService, _L("01632960000"));
	}

/**
Dial  call
@param aCall the call to use to dial
@param aCallId callId ltsy must allocate for this call
@param aService type of call (must match the line the call what created on)
@param aTelNumber A telephone number
*/
void CCtsyComponentTestBase::DialL(RCall& aCall,
	                                     const TInt aCallId,
	                                     const RMobilePhone::TMobileService aService,
	                                     const TDesC &aTelNumber)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	// EEtelCallDial
	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
    callParams.iInterval = 0;
    callParams.iCug.iCugIndex = 0xFFFF;
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(aTelNumber);
	callInfo.iService = aService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		dialData(0, aService, callParams, callInfo);
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallDial,data);

	// EMobileCallGetMobileCallInfo
	CallGetMobileCallInfoL(aCallId, aService, aTelNumber);

	// complete Dial
	TMockLtsyCallData0 dialCompleteData(aCallId,aService);
	data.Close();
	dialCompleteData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,data);

	TRequestStatus requestStatus ;
	aCall.Dial(requestStatus, callInfo.iDialledParty.iTelNumber);

    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(1);
	}

/**
Open packet service session.
@param aStartAttachStatus Starting attach status.
@param aStartAttachMode Starting atach mode.  The CTSY does not even cache this value so it does not really matter what it gets set to.
*/
void CCtsyComponentTestBase::OpenPacketServiceL
    (
    RPacketService& aRPacketService,     
    RPacketService::TStatus aStartAttachStatus,     
    RPacketService::TAttachMode aStartAttachMode,    
    TInt aMaxMonitorServices,   
    TInt aMaxServices
    )
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.ExpectL(EPacketNotifyStatusChange);
	
	TBool resumed = EFalse;
	TMockLtsyData2<RPacketService::TStatus , TBool> comNotifyStatus(aStartAttachStatus, resumed);
	comNotifyStatus.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketNotifyStatusChange,KErrNone, data);
	
	TBool expInitState = ETrue;	 
	TMockLtsyData1<TBool> expGetAttachMode(expInitState);
	data.Close();
	expGetAttachMode.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetAttachMode, data);
	
	TMockLtsyData1<RPacketService::TAttachMode> comGetAttachMode(aStartAttachMode);
	data.Close();
	comGetAttachMode.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetAttachMode,KErrNone, data);
	
	TMockLtsyData1< TInt > iniData(aMaxServices);
    iniData.SerialiseL(data); 
    iMockLTSY.ExpectL(EPacketEnumerateMbmsMonitorServiceList);	
	iMockLTSY.CompleteL( EPacketEnumerateMbmsMonitorServiceList, KErrNone, data );

	

	TMockLtsyData1< TInt > iniData2(aMaxMonitorServices);
    iniData2.SerialiseL(data); 	
    iMockLTSY.ExpectL(EPacketEnumerateMbmsActiveServiceList);
	iMockLTSY.CompleteL( EPacketEnumerateMbmsActiveServiceList, KErrNone, data );

	//this is actually a sync call to LTSY where return represents a context id offset rather than error code.	
	iMockLTSY.ExpectL(EPacketInitProxiesIPC, 3);	

	//this actually completes before all messages between CTSY<->LTSY have finished.
	TInt err = aRPacketService.Open(iPhone);
		
	// Wait for the end of initialisation
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(); // data
	}	

/**
Context initiation
@param aPacketContext context object
@param aContextName context name
*/
	
TInt CCtsyComponentTestBase::InitContextL(RPacketContext& aPacketContext, 
                                          TInfoName& aContextName, 
                                          TInfoName aHostCidName)
{
	RBuf8 data;
	CleanupClosePushL(data);

	// data for initialisation of the context 
	TRequestStatus initStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);	
    TMockLtsyData2< TInfoName, TInfoName > ltsyInit(aContextName, aHostCidName);
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, data);
	
	TMockLtsyData1< TInfoName > ltsyData(aContextName);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	//initialisation of the context
	aPacketContext.InitialiseContext(initStatus, pckgInit);
	User::WaitForRequest(initStatus);
	
	CleanupStack::PopAndDestroy(1); // data
	return initStatus.Int();
}

/**
Context initiation
@param aPacketContext context object
@param aContextName context name
*/
	
TInt CCtsyComponentTestBase::InitMbmsContextL(RPacketMbmsContext& aPacketContext, 
                                              TInfoName& aContextName )
{
	RBuf8 data;
	CleanupClosePushL(data);

	// data for initialisation of the context 
    TMockLtsyData1< TInfoName> ltsyInit(aContextName);
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, data);
	
	RPacketMbmsContext::TDataChannelV2 dataCh;
	TPckg<RPacketMbmsContext::TDataChannelV2> pckgInit(dataCh);
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TDataChannelV2 > ltsyData(aContextName, dataCh);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data);
	
	//initialisation of the context
	TRequestStatus initStatus;
	aPacketContext.InitialiseContext(initStatus, pckgInit);
	User::WaitForRequest(initStatus);
	
	CleanupStack::PopAndDestroy(1); // data
	return initStatus.Int();
}



/**
Context activation
@param aContextName context name
*/

TInt CCtsyComponentTestBase::SetContextActiveL(TInfoName& aContextName)
{
	RBuf8 data;
	CleanupClosePushL(data);

	//data for CompleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActive;
 	TMockLtsyData2 <TInfoName, TContextMisc>
    ltsyData1(aContextName, completeMisc);
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();
}


/**
Context activation
@param aContextName context name
*/

TInt CCtsyComponentTestBase::SetMbmsActiveL(RPacketMbmsContext& aMbmsContext,
                                            RPacketMbmsContext::TContextConfigMbmsV1& aConfig,
                                            TInfoName& aContextName)
{
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	

 	TMockLtsyData1 <TInfoName> expData( aContextName );
	expData.SerialiseL( expectData );
	
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1(aContextName, aConfig);
	ltsyData1.SerialiseL(data);	
	
 	iMockLTSY.ExpectL( EPacketContextActivate, expectData );
 	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
 	
 	TRequestStatus	requestStatus;
	aMbmsContext.Activate( requestStatus );
	User::WaitForRequest( requestStatus );	
	
	CleanupStack::PopAndDestroy(2); // data, expectData
	
	return requestStatus.Int();
}

TInt CCtsyComponentTestBase::SetMbmsDeactiveL(RPacketMbmsContext& aPacketContext, 
                                              TInfoName& aContextName)
{
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketMbmsContext::CMbmsSession* list=RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(list);
	TMockLtsyData2< TInfoName, RPacketMbmsContext::CMbmsSession* > ltsyData2(aContextName, list);
	ltsyData2.SerialiseL(data);	
	
 	iMockLTSY.ExpectL( EPacketContextDeactivate, data );

 	RBuf8 compData;
	CleanupClosePushL(compData);	
	TMockLtsyData1< TInfoName> ltsyData1(aContextName);
	ltsyData1.SerialiseL(compData);	
 	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, compData);
 	
 	TRequestStatus	requestStatus;
	aPacketContext.Deactivate( requestStatus );
	User::WaitForRequest( requestStatus );	
	
	CleanupStack::PopAndDestroy(3); // data, list, compData
	
	return requestStatus.Int();       
}

TInt CCtsyComponentTestBase::SetMbmsDeleteL(RPacketMbmsContext& aPacketContext, 
                                            TInfoName& aContextName)
{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TMockLtsyData1< TInfoName > ltsyData1(aContextName);
	ltsyData1.SerialiseL(data);	
	
 	iMockLTSY.ExpectL( EPacketContextDelete, data );
 	iMockLTSY.CompleteL(EPacketContextDelete, KErrNone, data);
 	
 	TRequestStatus	requestStatus;
	aPacketContext.Delete( requestStatus );
	User::WaitForRequest( requestStatus );	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return requestStatus.Int(); 

}

/**
Set context to new status
@param aContextName context name
@param aStatus status to set 
*/
TInt CCtsyComponentTestBase::SetContextStatusL(TInfoName& aContextName, RPacketContext::TContextStatus aStatus)
{
	RBuf8 data;
	CleanupClosePushL(data);

	//data for CompleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = aStatus;
 	TMockLtsyData2 <TInfoName, TContextMisc>
 														 ltsyData1(aContextName, completeMisc);
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();
}

/**
Set context to new status
@param aContextName context name
@param aStatus status to set 
*/
TInt CCtsyComponentTestBase::SetMbmsConfigL(
    RPacketMbmsContext& aMbmsContext, 
    TInfoName& aContextName, 
    RPacketMbmsContext::TContextConfigMbmsV1 aStatus)
{
    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(aStatus);     

	TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(aStatus, aContextName);
    expExpect.SerialiseL(expectData);
	
    TMockLtsyData1<TInfoName > contexName(aContextName);
    contexName.SerialiseL(data);
	TRequestStatus mockLtsyStatus;    
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    aMbmsContext.SetConfig(mockLtsyStatus, pckgConfig);	
	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
	
	CleanupStack::PopAndDestroy(2); // data, expectData
	
	return mockLtsyStatus.Int();
}

/**
Create QoS and set profile params
@param aPacketQoS packet qos object
@param aPacketContext context object
@param aRequested profile
*/

TInt CCtsyComponentTestBase::CreateQoSAndSetProfileParamLC(RPacketQoS& aPacketQoS, 
													       RPacketContext& aPacketContext, 
													       TInfoName& aContextName,
														   TPacketDataConfigBase& aRequested,
														   TInfoName aPrimaryContextName)
{
	// ------ create New QoS -------------------------------------------------
	TName qosName;
	aPacketQoS.OpenNewQoS(aPacketContext, qosName);
	CleanupClosePushL(aPacketQoS);	
	
	RBuf8 expectDataSPP;
	CleanupClosePushL(expectDataSPP);
	
	TInt ret(KErrArgument); 	
 	
 	// Get Context current status 
 	// test 1: context not inited
    RPacketContext::TContextStatus contextStatus;    
    TBool statusChanged = EFalse;
    aPacketContext.GetStatus(contextStatus);
    
    //check  Context current status 
    if ( RPacketContext::EStatusActivating != contextStatus &&
             RPacketContext::EStatusActive != contextStatus)
    	{ 	   		
    		// and change it to RPacketContext::EStatusActive     		
    		SetContextActiveL(aContextName);  
    		statusChanged = ETrue;  		
    	}  
 	
 	
	// ------ SetProfileParameters --------------------------------------------
	TRequestStatus requestStatus;	
	
	if (TPacketDataConfigBase::KConfigGPRS == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSGPRSRequested*  req;
		req = reinterpret_cast< RPacketQoS::TQoSGPRSRequested* > ( &aRequested );
		TPckg<RPacketQoS::TQoSGPRSRequested> pckg(*req);
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		AssertMockLtsyStatusL();
		// ... get  contextConfig
		RPacketContext::TContextConfigGPRS contextConfig;
		TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();	
		
		// data for SetProfileParameters's ExpectL		
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = aPrimaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
		                    expExpect(contextParams, *req);
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);					
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
	
	if (TPacketDataConfigBase::KConfigRel99Rel4 == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSR99_R4Requested* req;
		req = reinterpret_cast< RPacketQoS::TQoSR99_R4Requested* > ( &aRequested );		
		TPckg<RPacketQoS::TQoSR99_R4Requested> pckg(*req);
		
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		// ... get  contextConfig
		RPacketContext::TContextConfigR99_R4 contextConfig;
		TPckg< RPacketContext::TContextConfigR99_R4 > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfigR99_R4 > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);	
		
		// data for SetProfileParameters's ExpectL
		TInfoName primaryContextName;	
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = primaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSR99_R4Requested> 
		                    expExpect(contextParams, *req);
		expectDataSPP.Close();                    
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);		
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
	
	if (TPacketDataConfigBase::KConfigRel5 == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSR5Requested* req;
		req = reinterpret_cast< RPacketQoS::TQoSR5Requested* > ( &aRequested );		
		TPckg<RPacketQoS::TQoSR5Requested> pckg(*req);
		
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		// ... get  contextConfig
		RPacketContext::TContextConfig_R5 contextConfig;
		TPckg< RPacketContext::TContextConfig_R5 > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfig_R5 > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);	
		
		// data for SetProfileParameters's ExpectL
		TInfoName primaryContextName;	
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = primaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSR5Requested> 
		                    expExpect(contextParams, *req);
	    expectDataSPP.Close();
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);	
		
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
    
    
    // if context status was changed
    if (statusChanged)
    	{
    	// return to previous status
    	SetContextStatusL(aContextName, contextStatus);
    	}
    
    CleanupStack::PopAndDestroy(1);// expectDataSPP    
    // aPacketQoS is still in CleanupStack !!!	
	return ret;
}


/**
Set status to RPacketService
@param aStatus status to set
@param aIsResumed -- is resumed
*/
TInt CCtsyComponentTestBase::SetPacketServiceStatusL(RPacketService::TStatus& aStatus, TBool aIsResumed)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	// Data for CompleteL
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(aStatus, aIsResumed);
	ltsyData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();	
	}
	
/**
Add filter to RContext
@param aContext RPacketContext use to add filter 
@param aPacketFilterInfo -- Reference to a TPacketFilterV2 class packaged inside a TPckg<> class.
*/
TInt CCtsyComponentTestBase::AddContextPacketFilterL(RPacketContext& aContext, 
													TInfoName& aContextName, 
													RPacketContext::TPacketFilterV2& aFilter, 
													TBool aIsFailInCTSY)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
		
	TRequestStatus requestStatus;
	
	RPacketContext::TPacketFilterV2Pckg filterPckg (aFilter);

    // if request reaches LTSY
	if (!aIsFailInCTSY)
		{
		//data for ExpectL
		RPacketContext::TContextConfigGPRS configGPRS;
		CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );
	    CleanupStack::PushL(filterArray);

		filterArray->AppendL( aFilter );
		
		TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
					packetFilterData( configGPRS, 0, aContextName, filterArray );		
		packetFilterData.SerialiseL(data);		 
		iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);
		
		//data for CompleteL
		TMockLtsyData1<TInfoName > contexName(aContextName);
    	data.Close();
    	contexName.SerialiseL(data); 			
		iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
		
		CleanupStack::PopAndDestroy(1); // filterArray
		}    
		
	aContext.AddPacketFilter(requestStatus, filterPckg);		
	User::WaitForRequest(requestStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return requestStatus.Int();
	}

TInt CCtsyComponentTestBase::OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName)
    {
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(aLineName);
    name.Append(KDoubleColon);

   	return aCall.OpenNewCall(aLine, name);
    }

TInt CCtsyComponentTestBase::OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName, TName& aNewName)
    {
    aNewName = KMmTsyPhoneName;
    aNewName.Append(KDoubleColon);
    aNewName.Append(aLineName);
    aNewName.Append(KDoubleColon);

   	return aCall.OpenNewCall(aLine, aNewName);
    }

/**
Create incoming call on RLine
@param aLine RLine use to create call.
@param aCallId Call id for created call.
@param aLineName The name of the line. Deprecated, line should be opened in advance
@param aIncomingCallName On completion contains the name of the incoming call.
@param aMobileService Applicability of created call to a mobile service group.
@param aMobileCallStatus Call state of created call.
*/
TInt CCtsyComponentTestBase::CreateIncomingCallL(RLine& aLine, 
                                            const TInt aCallId, 
                                            const TDesC& /*aLineName*/,
                                            TName& aIncomingCallName,
                                            RMobilePhone::TMobileService aMobileService,
                                            RMobileCall::TMobileCallStatus aMobileCallStatus)
	{
    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus requestNotify;
    TRequestStatus mockLtsyStatus;

	aLine.NotifyIncomingCall(requestNotify, aIncomingCallName);

    // call CompleteNotifyIncomingCall for set iCallId = aCallId.
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = aMobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(aCallId, aMobileService, mobileCallInfo);
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestNotify);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestNotify.Int());
    
    // call CompleteNotifyStatusChange for set iMobileCallStatus = aMobileCallStatus.
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(aCallId, aMobileService, aMobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1);  
	
	return KErrNone;
	}

/**
Close incoming call
@param aCall RCall to close.
@param aCallId Call id of the call.
@param aMobileService Applicability of the call to a mobile service group.
*/
void CCtsyComponentTestBase::CloseIncomingCallL(RCall& aCall, const TInt aCallId,
                                           const RMobilePhone::TMobileService aMobileService)
	{
	CloseCallL(aCall, aCallId, aMobileService);
	}

	
/**
Close call
@param aCall RCall to close.
@param aCallId Call id of the call.
@param aMobileService Applicability of the call to a mobile service group.
*/

void CCtsyComponentTestBase::CloseCallL(RCall& aCall,
         const TInt aCallId,
         const RMobilePhone::TMobileService aMobileService)
	{
	PrepareCallCloseL(aCallId, aMobileService);
	
	aCall.Close();
	AssertMockLtsyStatusL();
	}

/**
Prepare call close
@param aCallId Call id of the call.
@param aMobileService Applicability of the call to a mobile service group.
*/

void CCtsyComponentTestBase::PrepareCallCloseL(const TInt aCallId,
												const RMobilePhone::TMobileService aMobileService)
	{
    RBuf8 expectData;
    CleanupClosePushL(expectData);

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(aCallId, aMobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);

    CleanupStack::PopAndDestroy(1); // expectData 
	}
	
/**
Close line
@param aCall RLine to close.
@param aCallId Call id of connected call on the line
@param aMobileService Applicability of the call to a mobile service group.
*/
void CCtsyComponentTestBase::CloseLineL(RLine& aLine, TInt aCallId, RMobilePhone::TMobileService aMobileService)
	{
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	// Get Call Status	
	RCall::TStatus callStatus;
	aLine.GetStatus(callStatus);

	// Hang up call if call is still connecting 
	// so in this case we have to prepare expectData
	if ( !( RMobileCall::EStatusIdle == callStatus ||
            RMobileCall::EStatusDisconnecting == callStatus ||
            RMobileCall::EStatusDisconnectingWithInband == callStatus )
           && aCallId !=0 && aMobileService != RMobilePhone::EServiceUnspecified)
		{
		// prepare data for close line
		TInt symbianHangUpCause( KErrGsmReleaseByUser );
		TBool autoStChangeDisable = EFalse;   
		TMockLtsyCallData2<TInt, TBool> expData(aCallId, aMobileService, symbianHangUpCause, autoStChangeDisable);
		expData.SerialiseL(expectData); 
		iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
		}	

    // close line
   	aLine.Close();   
   	
   	CleanupStack::PopAndDestroy(1); // expectData 	
	}

/**
Put complete data of CallGetMobileCallInfo IPC to MockLTSY
@param aCallId Call id of the call
@param aService Applicability of the call to a mobile service group.
@param aTelNumber Telephone number of the call.
*/
void CCtsyComponentTestBase::CallGetMobileCallInfoL(TInt aCallId, 
                                               RMobilePhone::TMobileService aService, 
                                               const TDesC &aTelNumber)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(aTelNumber);
	callInfo.iService = aService;
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(aCallId, aService, callInfo);
	callInfoData.SerialiseL(data);
		
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    
    CleanupStack::PopAndDestroy(1);
	}

/**
Change call status
@param aCallId Call id of the call
@param aMobileService Applicability of the call to a mobile service group.
@param aCallStatus Call status.
*/
void CCtsyComponentTestBase::ChangeCallStatusL(const TInt aCallId,
								const RMobilePhone::TMobileService aMobileService,
								const RMobileCall::TMobileCallStatus aCallStatus,
								const TInt aError)
	{
	RBuf8 data;
	data.CleanupClosePushL();
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	
	RMobileCall::TMobileCallStatus callStatus = aCallStatus;
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData(aCallId, aMobileService, callStatus);
	data.Close();
	completeCallStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, aError, data);

	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);
	}

/**
Gets next main service group enumeration value
@param aServiceGroup current service group
*/
void CCtsyComponentTestBase::GetNextServiceGroup(RMobilePhone::TMobileService &aServiceGroup)
	{
	switch ( aServiceGroup )
		{
		case RMobilePhone::EVoiceService:
			aServiceGroup = RMobilePhone::EAuxVoiceService;
			break;
		case RMobilePhone::EAuxVoiceService:
			aServiceGroup = RMobilePhone::ECircuitDataService;
			break;
		case RMobilePhone::ECircuitDataService:
			aServiceGroup = RMobilePhone::EFaxService;
			break;
		default:
			aServiceGroup = RMobilePhone::EVoiceService;
			break;
		}
	}

/**
Opens a line
@param aLine The line to open
@param aLineName The name of the line
*/
void CCtsyComponentTestBase::OpenLineLC(RLine& aLine, const TDesC& aLineName)
	{
	TInt errorCode = aLine.Open(iPhone, aLineName);
	ASSERT_EQUALS(KErrNone, errorCode)
	CleanupClosePushL(aLine);
	}

/**
Opens a call
@param aLine A line to open the call from
@param aCall The call to open
*/
void CCtsyComponentTestBase::OpenNewCallLC(RLine& aLine, RCall& aCall)
	{
	TInt errorCode = aCall.OpenNewCall(aLine);
	ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(aCall);
	}

void CCtsyComponentTestBase::OpenIncomingCallLC(
		RLine& aLine,
		RCall& aCall,
		TInt aCallId,
		RMobilePhone::TMobileService aMobileService,
		RMobileCall::TMobileCallStatus aMobileCallStatus)
	{
	
	TName incomingCallName;

	TInt err = CreateIncomingCallL(aLine, aCallId, KNullDesC, incomingCallName, 
	                               aMobileService, aMobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);

    err = aCall.OpenExistingCall(aLine, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(aCall);
	AssertMockLtsyStatusL();
	
	}

/**
Opens RMobilePhoneBookStore
@param aStore phoneBookStore
@param aName phoneBookStore name
@param RMobilePhone phone for phoneBookStore.
*/

void CCtsyComponentTestBase::OpenPhoneBookStoreL(RMobilePhoneBookStore &aStore, TName &aName, RMobilePhone &aPhone)
	{

    if ( !aName.CompareF( KETelIccAdnPhoneBook ) ||
            !aName.CompareF( KETelIccFdnPhoneBook ))
        {
    	RBuf8 data;
    	CleanupClosePushL(data);
    	
    	RBuf8 data2;
    	CleanupClosePushL(data2);
    	

    	TRequestStatus mockLtsyStatus;
    	iMockLTSY.NotifyTerminated(mockLtsyStatus);

                
        //EMmTsyPhoneBookStoreInitIPC
        TMockLtsyPhoneBookData0 storeInitData(aName);
        storeInitData.SerialiseL(data);
        
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	


        CStorageInfoData storageData;

		SetStorageInfoData(storageData);
            
    	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(aName, storageData); 
    	retStoreInitC.SerialiseL(data2);
    	
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);


    	//EMmTsyPhoneBookStoreCacheIPC
        CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
        CleanupStack::PushL(cache);
        
        TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(aName, cache);
        data2.Close();
        storeCacheData.SerialiseL(data2);	
        data.Close();
        storeInitData.SerialiseL(data);
        
                      
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);
    	
    	//Open
    	TInt ret = aStore.Open(aPhone, aName);
    	ASSERT_EQUALS(KErrNone, ret);	    	

        User::WaitForRequest(mockLtsyStatus);        	
    	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    	AssertMockLtsyStatusL();
    	
    		
    	CleanupStack::PopAndDestroy(3); //data, data2, cache

        }
    else
      	{
    	RBuf8 data;
    	CleanupClosePushL(data);
    	
    	RBuf8 data2;
    	CleanupClosePushL(data2);
    	

    	TRequestStatus mockLtsyStatus;
    	iMockLTSY.NotifyTerminated(mockLtsyStatus);

                
        //EMmTsyPhoneBookStoreInitIPC
        TMockLtsyPhoneBookData0 storeInitData(aName);
        storeInitData.SerialiseL(data);
        
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	


        CStorageInfoData storageData;

		SetStorageInfoData(storageData);
            
    	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(aName, storageData); 
    	retStoreInitC.SerialiseL(data2);
    	
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    	//Open
    	TInt ret = aStore.Open(aPhone, aName);
    	ASSERT_EQUALS(KErrNone, ret);	    	

        User::WaitForRequest(mockLtsyStatus);        	
    	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    	AssertMockLtsyStatusL();
    	    		
    	CleanupStack::PopAndDestroy(2); //data, data2
      	}
	}

void CCtsyComponentTestBase::ChangeUsedSimCardTypeL(TICCType aType)
	{
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	TICCType myICCType(aType);
	
	TMockLtsyData1<TICCType> iccData(myICCType);
	completeData.Close();
	iccData.SerialiseL(completeData);

	TPckg<TInt> dataPckg(aType);
	iMockLTSY.CompleteL(EMmTsySimGetICCType, KErrNone, completeData);
	
	iMockLTSY.ExpectL(EMmTsySecurityGetSimActivePinStateIPC);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(1);
	}


void CCtsyComponentTestBase::SetStorageInfoData(CStorageInfoData& aStorage, TInt aVal)
	{
        aStorage.iADNNumberLengthMax = 
         aStorage.iADNTextLengthMax = 
          aStorage.iFDNNumberLengthMax = 
           aStorage.iFDNTextLengthMax =
            aStorage.iSDNNumberLengthMax =
             aStorage.iSDNTextLengthMax =
              aStorage.iVMBXNumberLengthMax =
               aStorage.iVMBXTextLengthMax =
                aStorage.iMBDNNumberLengthMax =
                 aStorage.iMBDNTextLengthMax = aVal;	
	}

/*
 * CompleteDialStatusNotificationsL completes the correct call notifications to get the call into the
   connected state simulating the remote party answering the dialled call.
 * @param aCall A call which has been dialled.
 * @param aCallId callId ltsy must allocate for this call
 * @param aMobileService type of call.
 */
void CCtsyComponentTestBase::CompleteDialStatusNotificationsL(RCall &aCall, TInt aCallId, RMobilePhone::TMobileService aMobileService )
	{
	
    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
	TRequestStatus mockLtsyStatus;
	TRequestStatus reqState1, reqState2, reqState3;
	RCall::TStatus status;
	
	aCall.NotifyStatusChange(reqState1, status);
	
	RMobileCall::TMobileCallStatus mobileCallCurrentStatus = RMobileCall::EStatusDialling;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(aCallId, aMobileService, mobileCallCurrentStatus);
    completeData.Close();
    mockCallData4.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
				
    User::WaitForRequest(reqState1);            
    aCall.NotifyStatusChange(reqState2, status);
    
    mobileCallCurrentStatus = RMobileCall::EStatusConnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData5(aCallId, aMobileService, mobileCallCurrentStatus);
    completeData.Close();
    mockCallData5.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
                 
    User::WaitForRequest(reqState2);
    aCall.NotifyStatusChange(reqState3, status);
    
    mobileCallCurrentStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData6(aCallId, aMobileService, mobileCallCurrentStatus);
    completeData.Close();
    mockCallData6.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // status call becomes 'connected' here...   
            
    User::WaitForRequest(reqState3);    
	
    CleanupStack::PopAndDestroy(1);
	}

/*
 * CompleteCallDisconnectNotificationsL completes the necessary call status notification to disconnect a connected call.
 * @param aCallId callId ltsy must allocate for this call
 * @param aMobileService type of call.
 */
void CCtsyComponentTestBase::CompleteCallDisconnectNotificationsL(TInt aCallId, RMobilePhone::TMobileService aMobileService)
	{
	
	RBuf8 expectData;
    CleanupClosePushL(expectData);
	
	RBuf8 completeData;
    CleanupClosePushL(completeData);
    
	TRequestStatus mockLtsyStatus;
	
	// call CMmVoiceCallTsy::CompleteNotifyStatusChange for setting 
	// iMobileCallStatus = RMobileCall::EStatusDisconnecting.
	RMobileCall::TMobileCallStatus mobileCallSt = RMobileCall::EStatusDisconnecting; 
   	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus1(aCallId, aMobileService, mobileCallSt);
	completeData.Close();
	mockCallDataStatus1.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// call CMmVoiceCallTsy::CompleteNotifyStatusChange for setting 
	// iMobileCallStatus = RMobileCall::EStatusIdle.
	mobileCallSt = RMobileCall::EStatusIdle; 
   	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus2(aCallId, aMobileService, mobileCallSt);
	completeData.Close();
	mockCallDataStatus2.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
	
	// when call becomes idle, remaining duration of the call is added to life time param in LTSY:		
	TUint32 duration2 = 4;	// this is a dummy value, which won't be checked by mocksy engine
	TMockLtsyData1<TUint32> ltsyData2( duration2 );
	expectData.Close();
	ltsyData2.SerialiseL(expectData);	
	iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, expectData);	    
					
	User::WaitForRequest(mockLtsyStatus);
	// we did set a sleep time for 15sec, but after stopping timer it may give us 14 sec or 15 sec
	// we can not determine the correct value for the mockLTSY;
	// we tell mocksy engine to ignore duration param value	
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
    
    TRequestStatus mockLtsyStatus3;
	iMockLTSY.NotifyTerminated(mockLtsyStatus3);
	iMockLTSY.CompleteL(EMmTsyUpdateLifeTimeIPC, KErrNone);	
	
	User::WaitForRequest(mockLtsyStatus3);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus3.Int());
		
	CleanupStack::PopAndDestroy(2);
		
	}
	
