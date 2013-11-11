// TestTelephonyPktTestStepBase.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPktTestStepBase

// 	user include
#include "TestLtsyPktTestStepBase.h"
#include "te_integration_stltsysuitestepbase.h"
#include "te_integration_stltsysuitedefs.h"

_LIT8(KName, "internet");
/**
 * Constructor
 */
CTestLtsyPktTestStepBase::CTestLtsyPktTestStepBase(CTestLtsyModel& aModel)
	:iModel(aModel),iSharedPhone(aModel.SharedEnv().Phone())
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyPktTestStepBase::~CTestLtsyPktTestStepBase()
	{

	}

TVerdict CTestLtsyPktTestStepBase::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPktTestStepBase::doTestStepPreambleL()."));
	SetTestStepResult(EPass);
	return TestStepResult();
	}
TVerdict CTestLtsyPktTestStepBase::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPktTestStepBase::doTestStepPostambleL()"));
	return TestStepResult();
	}

/**
 * Get the shared phone
 * 
 */
RMobilePhone& CTestLtsyPktTestStepBase::SharedPhone() 
	{
	return iSharedPhone;
	}
/**
 * Check if the RPacketService has been opened
 * @return - the result
 */
TBool CTestLtsyPktTestStepBase::IsOpenedPktService()
	{
	return (NULL != iModel.PktSharedData().iPacket.SubSessionHandle());
	}
void CTestLtsyPktTestStepBase::OpenPacketobject()
	{
	TInt           returnStatus;
	INFO_PRINTF1(_L("Close packet\n"));
	// Open Packet object
	iModel.PktSharedData().iPacket.Close();
	INFO_PRINTF1(_L("packet closeed \n"));
	INFO_PRINTF1(_L("To open Packet Object\n"));

	returnStatus = iModel.PktSharedData().iPacket.Open(iModel.SharedEnv().Phone());
	if(returnStatus == KErrNone)
		{
		INFO_PRINTF1(_L("packet opened \n"));
		}
	else
		{
		INFO_PRINTF1(_L("Opened packet error\n"));
		INFO_PRINTF2(_L("Opened packet error = %d\n"), returnStatus);
		}
	}
void CTestLtsyPktTestStepBase::ClosePacketobject()
	{
	INFO_PRINTF1(_L("Close packet\n"));
	User::After(1000000);
	iModel.PktSharedData().iPacket.Close();
	INFO_PRINTF1(_L("packet closeed \n"));
	}
void CTestLtsyPktTestStepBase::DisplayPacketServiceStatus()
/**
 * Test the TSY's functionality to retrieve the packet service status.
 */
	{
	TInt returnStatus = iModel.PktSharedData().iPacket.GetStatus(iModel.PktSharedData().iPacketStatus);
	if(returnStatus==KErrNone)
		{
		switch (iModel.PktSharedData().iPacketStatus)
			{
			case RPacketService::EStatusUnattached:
				INFO_PRINTF1(_L("Packet Service status = Unattached\n"));
				break;
			case RPacketService::EStatusAttached:
				INFO_PRINTF1(_L("Packet Service status = Attached\n"));
				break;
			case RPacketService::EStatusActive:
				INFO_PRINTF1(_L("Packet Service status = Active\n"));
				break;
			case RPacketService::EStatusSuspended:
				INFO_PRINTF1(_L("Packet Service status = Suspended\n"));
				break;
			default:
				INFO_PRINTF1(_L("Packet Service error\n"));
				break;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Packet Service error\n"));
		}
	}

void CTestLtsyPktTestStepBase::GprsNetWorkTest()
	{
	INFO_PRINTF1(_L("Open Packet Object"));

	TInt           returnStatus;
	TRequestStatus requestStatus;
	INFO_PRINTF1(_L("Close packet\n"));
	// Open Packet object
	iModel.PktSharedData().iPacket.Close();
	INFO_PRINTF1(_L("packet closeed \n"));
	INFO_PRINTF1(_L("To open Packet Object\n"));
	returnStatus = iModel.PktSharedData().iPacket.Open(iSharedPhone);
	if(returnStatus == KErrNone)
		{
		INFO_PRINTF1(_L("packet opened \n"));
		}
	else
		{
		INFO_PRINTF1(_L("Opened packet error\n"));
		}
	requestStatus = KErrNone;
	INFO_PRINTF1((_L("RPacket::GetNtwkRegStatus() invoked...\n")));
	iModel.PktSharedData().iPacket.GetNtwkRegStatus(requestStatus, iModel.PktSharedData().iRegistrationStatus);
	User::WaitForRequest(requestStatus);
	
	if(requestStatus == KErrNone)
		{
		switch (iModel.PktSharedData().iRegistrationStatus)
			{
			case RPacketService::ENotRegisteredNotSearching:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered Not Searching\n"));
				break;
			case RPacketService::ERegisteredOnHomeNetwork:
				INFO_PRINTF1(_L("Netw Reg status = Registered On Home Network\n"));
				break;
			case RPacketService::ENotRegisteredSearching:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered Searching\n"));
				break;
			case RPacketService::ERegistrationDenied:
				INFO_PRINTF1(_L("Netw Reg status = Registration Denied\n"));
				break;
			case RPacketService::EUnknown:
				INFO_PRINTF1(_L("Netw Reg status = Unknown\n"));
				break;
			case RPacketService::ERegisteredRoaming:
				INFO_PRINTF1(_L("Netw Reg status = RegisteredRoaming\n"));
				break;
			case RPacketService::ENotRegisteredButAvailable:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered But Available\n"));
				break;
			case RPacketService::ENotRegisteredAndNotAvailable:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered And Not Available\n"));
				break;
			default:
				INFO_PRINTF1(_L("Network Registration Status Error\n"));
				break;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Network Registration Status Error\n"));
		}
	iModel.PktSharedData().iPacket.Close();
	}
void CTestLtsyPktTestStepBase::DisplayNetworkRegistrationStatus()
/**
 * Test the TSY's functionality to retrieve the network registration status.
 */
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1((_L("RPacket::GetNtwkRegStatus() invoked...\n")));
	iModel.PktSharedData().iPacket.GetNtwkRegStatus(requestStatus, iModel.PktSharedData().iRegistrationStatus);
	User::WaitForRequest(requestStatus);
	
	if(requestStatus == KErrNone)
		{
		switch (iModel.PktSharedData().iRegistrationStatus)
			{
			case RPacketService::ENotRegisteredNotSearching:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered Not Searching\n"));
				break;
			case RPacketService::ERegisteredOnHomeNetwork:
				INFO_PRINTF1(_L("Netw Reg status = Registered On Home Network\n"));
				break;
			case RPacketService::ENotRegisteredSearching:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered Searching\n"));
				break;
			case RPacketService::ERegistrationDenied:
				INFO_PRINTF1(_L("Netw Reg status = Registration Denied\n"));
				break;
			case RPacketService::EUnknown:
				INFO_PRINTF1(_L("Netw Reg status = Unknown\n"));
				break;
			case RPacketService::ERegisteredRoaming:
				INFO_PRINTF1(_L("Netw Reg status = RegisteredRoaming\n"));
				break;
			case RPacketService::ENotRegisteredButAvailable:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered But Available\n"));
				break;
			case RPacketService::ENotRegisteredAndNotAvailable:
				INFO_PRINTF1(_L("Netw Reg status = Not Registered And Not Available\n"));
				break;
			default:
				INFO_PRINTF1(_L("Network Registration Status Error\n"));
				break;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Network Registration Status Error\n"));
		}
	}

void CTestLtsyPktTestStepBase::DisplayContextInfo()
/**
 * Test the TSY's functionality to retrieve the packet context information
 * using the packet service object.
 */
	{
	// Enumerate Contexts & Get Context Info
	INFO_PRINTF1(_L("Display the packet context information\n"));

	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketService::EnumerateContexts() invoked....\n"));
	iModel.PktSharedData().iPacket.EnumerateContexts(requestStatus, iModel.PktSharedData().iNumContexts, iModel.PktSharedData().iMaxContextsAllowed);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketService::EnumerateContexts() completed %d\n"), requestStatus.Int());
	INFO_PRINTF2(_L("Number of Contexts   = %d\n"), iModel.PktSharedData().iNumContexts);
	INFO_PRINTF2(_L("Max Contexts Allowed = %d\n"), iModel.PktSharedData().iMaxContextsAllowed);

	requestStatus = KErrNone;
	INFO_PRINTF1(_L("Sync RPacket::GetContextInfo() invoked...\n"));
	iModel.PktSharedData().iPacket.GetContextInfo(requestStatus, 0, iModel.PktSharedData().iContextInfo);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("Sync RPacket::GetContextInfo() completed %d\n"), requestStatus.Int());
	if (requestStatus == KErrNone)
		{
		INFO_PRINTF2(_L("Context Name   = %S\n"), &iModel.PktSharedData().iContextInfo.iName);
		DisplayContextStatus(iModel.PktSharedData().iContextInfo.iStatus);
		}


/********************************************************************
	console->Printf(TRefByValue<const TDesC>(_L("Execute Async RPacket::GetContextInfo()\n")));
	TRequestStatus requestStatus;
	iPacket.GetContextInfo(requestStatus, 0, iContextInfo2);
	User::WaitForRequest(requestStatus);
	console->Printf(TRefByValue<const TDesC>(_L("RPacketService::GetContextInfo() ret = %d\n")), requestStatus);
	if (requestStatus == KErrNone)
		{
		console->Printf(TRefByValue<const TDesC>(_L("Context Name   = %S\n")), iContextInfo2.iName);
		console->Printf(TRefByValue<const TDesC>(_L("Context Status = %d\n")), iContextInfo2.iStatus);
		}
*********************************************************************/
	}


void CTestLtsyPktTestStepBase::DisplayContextStatus(RPacketContext::TContextStatus aContextStatus)
	{
	switch (aContextStatus)
		{
		case RPacketContext::EStatusUnknown:
			INFO_PRINTF1(_L("Context Status = Unknown\n"));
			break;
		case RPacketContext::EStatusInactive:
			INFO_PRINTF1(_L("Context Status = Inactive\n"));
			break;
		case RPacketContext::EStatusActivating:
			INFO_PRINTF1(_L("Context Status = Activating\n"));
			break;
		case RPacketContext::EStatusActive:
			INFO_PRINTF1(_L("Context Status = Active\n"));
			break;
		case RPacketContext::EStatusDeactivating:
			INFO_PRINTF1(_L("Context Status = Deactivating\n"));
			break;
		case RPacketContext::EStatusSuspended:
			INFO_PRINTF1(_L("Context Status = Suspended\n"));
			break;
		case RPacketContext::EStatusDeleted:
			INFO_PRINTF1(_L("Context Status = Deleted\n"));
			break;
		default:
			INFO_PRINTF1(_L("Error - Context Status\n"));
			break;
		}
	}


void CTestLtsyPktTestStepBase::DisplayContextStatus(RPacketContext& aContext, RPacketContext::TContextStatus& aContextStatus)
	{
	TRequestStatus returnStatus = aContext.GetStatus(aContextStatus);
	if(returnStatus == KErrNone)
		{
		INFO_PRINTF1(_L("RPacketContext::GetStatus() OK\n"));
		}
	else
		{
		INFO_PRINTF1(_L("Error - Context Status\n"));
		}
	DisplayContextStatus(aContextStatus);
	}

void  CTestLtsyPktTestStepBase::GprsContextAttachTest()
	{
	INFO_PRINTF1(_L("GprsContextAttachTest\n"));
	TRequestStatus requestStatus = KErrNone;
	iModel.PktSharedData().iPacket.Attach(requestStatus);
	User::WaitForRequest(requestStatus);
	if(requestStatus == KErrNone)
		{
		INFO_PRINTF1(_L("GprsContextAttachTest OK \n"));
		}
	else
		{
		INFO_PRINTF1(_L("GprsContextAttachTest Error\n"));
		INFO_PRINTF2(_L("GprsContextAttachTest Error = %d\n"), requestStatus.Int());
		}

	}
void  CTestLtsyPktTestStepBase::GprsContextDetachTest()
	{
	INFO_PRINTF1(_L("GprsContextDetachTest\n"));
	TRequestStatus requestStatus = KErrNone;
	iModel.PktSharedData().iPacket.Detach(requestStatus);
	User::WaitForRequest(requestStatus);
	if(requestStatus == KErrNone)
		{
		INFO_PRINTF1(_L("GprsContextDetachTest OK \n"));
		}
	else
		{
		INFO_PRINTF1(_L("GprsContextDetachTest Error\n"));
		INFO_PRINTF2(_L("GprsContextDetachTest Error = %d\n"), requestStatus.Int());
		}

	}
void  CTestLtsyPktTestStepBase::DisplayStaticCapabilities()
	{
	// Static Capabilities
	TRequestStatus returnStatus = KErrNone;
	returnStatus = iModel.PktSharedData().iPacket.GetStaticCaps(iModel.PktSharedData().iStaticCaps, RPacketContext::EPdpTypeCDPD);
	INFO_PRINTF2(_L("RPacketService::GetStaticCaps() ret = %d\n"), returnStatus.Int());
	INFO_PRINTF2(_L("Static Caps = %d\n"), iModel.PktSharedData().iStaticCaps);
	INFO_PRINTF2(_L("Static Caps = %xH\n"), iModel.PktSharedData().iStaticCaps);
	}
void  CTestLtsyPktTestStepBase::DisplayDynamicCapabilities()
	{
	// Dynamic Capabilities
	TRequestStatus returnStatus = KErrNone;
	returnStatus = iModel.PktSharedData().iPacket.GetDynamicCaps(iModel.PktSharedData().iDynamicCaps);
	INFO_PRINTF2(_L("RPacketService::GetDynamicCaps() ret = %d\n"), returnStatus.Int());
	INFO_PRINTF2(_L("Dynamic Caps = %d\n"), iModel.PktSharedData().iDynamicCaps);
	INFO_PRINTF2(_L("Dynamic Caps = %xH\n"), iModel.PktSharedData().iDynamicCaps);
	}

void  CTestLtsyPktTestStepBase::GprsGetDefaultContextParamsTest()
	{
	TRequestStatus reqStatusContextParams = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig01;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg01(contextConfig01);
	INFO_PRINTF1(_L("RPacketContext::GetDefaultContextParams() invoked...\n"));
	iModel.PktSharedData().iPacket.GetDefaultContextParams(reqStatusContextParams, contextConfigPckg01);
	User::WaitForRequest(reqStatusContextParams);
	if(reqStatusContextParams == KErrNone)
		{
		INFO_PRINTF1(_L("RPacketContext::GetDefaultContextParams() complete\n"));
		}
	else
		{
		INFO_PRINTF1(_L("RPacketContext::GetDefaultContextParams() Error\n"));
		INFO_PRINTF2(_L("RPacketService::GprsgetDefaultContextParamsTest() completed %d\n"), reqStatusContextParams.Int());
		}
	}
void  CTestLtsyPktTestStepBase::GprsSetDefaultContextParamsTest()
	{
	// Set Context Config
	TRequestStatus reqStatusContextParams = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig02;
	//contextConfig2.iServiceOption = RPacketContext::KLowSpeedData;
	contextConfig02.iPdpType  = RPacketContext::EPdpTypeIPv6;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg02(contextConfig02);
	INFO_PRINTF1(_L("RPacketContext::SetDefaultContextParams() invoked...\n"));
	iModel.PktSharedData().iPacket.SetDefaultContextParams(reqStatusContextParams, contextConfigPckg02);
	User::WaitForRequest(reqStatusContextParams);
	if(reqStatusContextParams == KErrNone)
		{
		INFO_PRINTF1(_L("RPacketContext::GprsContextSetConfigTest() complete\n"));
		}
	else
		{
		INFO_PRINTF1(_L("RPacketContext::GprsContextSetConfigTest() Error\n"));
		INFO_PRINTF2(_L("RPacketService::GprsSetDefaultContextParamsTest() completed %d\n"), reqStatusContextParams.Int());
		}
	}
void  CTestLtsyPktTestStepBase::GprsContextActivateTest()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketContext::Activate() invoked...\n"));
	iModel.PktSharedData().iContext1.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::Activate() completed %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsContextDeactivateTest()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketContext::Deactivate() invoked...\n"));
	iModel.PktSharedData().iContext1.Deactivate(requestStatus);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::Deactivate() completed %d\n"), requestStatus.Int());
	}

void CTestLtsyPktTestStepBase::GprsContextSetConfigTest()
	{
	// Set Context Config
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig2;
	//contextConfig2.iServiceOption = RPacketContext::KLowSpeedData;
	contextConfig2.iPdpType  = RPacketContext::EPdpTypeIPv6;
	contextConfig2.iAccessPointName = KName;
	//contextConfig2.iPdpAddress = ;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg2(contextConfig2);
	INFO_PRINTF1(_L("RPacketContext::SetConfig() invoked...\n"));
	iModel.PktSharedData().iContext1.SetConfig(reqStatusContextConfig, contextConfigPckg2);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::SetConfig() complete = %d\n"), reqStatusContextConfig.Int());
	}

void CTestLtsyPktTestStepBase::GprsContextGetConfigTest()
	{
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig3;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg3(contextConfig3);
	INFO_PRINTF1(_L("RPacketContext::GetConfig() invoked...\n"));
	iModel.PktSharedData().iContext1.GetConfig(reqStatusContextConfig, contextConfigPckg3);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::GetConfig() complete = %d\n"), reqStatusContextConfig.Int());
	contextConfig3 = contextConfigPckg3();
	}
void CTestLtsyPktTestStepBase::NotificationContextAddedCancel()
	{
	TName contextAddedContextId;
	TRequestStatus reqStatusContextAdded = KErrNone;
	INFO_PRINTF1(_L("Test Notify Context Added Cancel\n"));
	iModel.PktSharedData().iPacket.NotifyContextAdded(reqStatusContextAdded, contextAddedContextId);
	iModel.PktSharedData().iPacket.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(reqStatusContextAdded);
	if (reqStatusContextAdded == KErrCancel)
		{
		INFO_PRINTF1(_L("Notify Context Added is KErrCancel\n"));
		}
	else 
		{
		INFO_PRINTF1(_L("Error - Notify Context Added status\n"));
		INFO_PRINTF2(_L("Error - Notify Context Added status = %d\n"), reqStatusContextAdded.Int());
		}
	}
void CTestLtsyPktTestStepBase::NotificationContextAdded()
	{
	TName contextAddedContextId;
	TRequestStatus reqStatusContextAdded = KErrNone;
	// Notify Context Added
	reqStatusContextAdded = KErrNone;
	contextAddedContextId = _L("ABCDEFGHIJKLMNOP");
	iModel.PktSharedData().iPacket.NotifyContextAdded(reqStatusContextAdded, contextAddedContextId);
	if (reqStatusContextAdded == KRequestPending)
		{
		INFO_PRINTF1(_L("Notify Context Added is KRequestPending\n"));
		}
	else
		{
		INFO_PRINTF1(_L("Notify Context Added request status\n"));
		INFO_PRINTF2(_L("Error - Notify Context Added status = %d\n"), reqStatusContextAdded.Int());
		}
	}
void CTestLtsyPktTestStepBase::OpenNewContext()
	{
	TInt returnStatus;
	// check if the slot is free to open
	if (NULL != iModel.PktSharedData().iContext1.SubSessionHandle())
		{
		ERR_PRINTF1(_L("Can't open the packet context before it's closed, aborting..."));
		}
	else
		{
	    returnStatus = iModel.PktSharedData().iContext1.OpenNewContext(iModel.PktSharedData().iPacket, iModel.PktSharedData().iContextName);
		if (KErrNone != returnStatus)
			{
			INFO_PRINTF2(_L("RPacketContext::OpenNewContext() ret = %d\n"), returnStatus);
			}
	    INFO_PRINTF2(_L("RPacketContext::OpenNewContext() ret = %d\n"), returnStatus);
	    INFO_PRINTF2(_L("Context Name = %S\n"), &iModel.PktSharedData().iContextName);
		}
	}
void CTestLtsyPktTestStepBase::OpenExistingContext()
	{
	// Open an existing context
	TInt returnStatus;
	returnStatus = iModel.PktSharedData().iContext3.OpenExistingContext(iModel.PktSharedData().iPacket, iModel.PktSharedData().iContextName);
	INFO_PRINTF2(_L("RPacketContext::OpenExistingContext() ret = %d\n"), returnStatus);
	INFO_PRINTF2(_L("Context3 Name = %S\n"), &iModel.PktSharedData().iContextName);
	}
void CTestLtsyPktTestStepBase::OpenContextUsingUnknownName()

{
	// Attempt to open a Context using an unknown name
	TInt returnStatus;
	TName bogusContextName;
	bogusContextName.Append(_L("Context"));
	bogusContextName.AppendNum(10);
	returnStatus = iModel.PktSharedData().iContext2.OpenExistingContext(iModel.PktSharedData().iPacket, bogusContextName);
	INFO_PRINTF2(_L("RPacketContext::OpenExistingContext() returnStatus = %d\n"), returnStatus);
}
void CTestLtsyPktTestStepBase::CloseExistContext()
	{
	// Close Exist Context
	User::After(1000000);
	INFO_PRINTF1(_L("CloseExistContext() invoked...\n"));
	iModel.PktSharedData().iContext1.Close();
	INFO_PRINTF1(_L("CloseExistContext() OK...\n"));
	}
void CTestLtsyPktTestStepBase::GprsInitialiseContext()
	{
	TRequestStatus requestStatusInit = KErrNone;
	INFO_PRINTF1(_L("RPacketContext::InitialiseContext() invoked...\n"));
    RPacketContext::TDataChannelV2 dataChannel;
    TPckg<RPacketContext::TDataChannelV2> dataChannelPckg(dataChannel);
	//iContext1.LoanCommPort(requestStatusLoanCommPort, commPort);
    iModel.PktSharedData().iContext1.InitialiseContext(requestStatusInit,dataChannelPckg);
	User::WaitForRequest(requestStatusInit);
	INFO_PRINTF2(_L("RPacketContext::InitialiseContext() completed %d \n"), requestStatusInit.Int());
	}

void CTestLtsyPktTestStepBase::NotificationContextStatusChangeCancel()
	{
	// cancel the context status change notification request
	TRequestStatus requestStatusContextStatusChange;
	INFO_PRINTF1(_L("RPacketContext::NotifyStatusChangeCancel() invoked...\n"));
	iModel.PktSharedData().iContext1.CancelAsyncRequest(EPacketContextNotifyStatusChange);
	User::WaitForRequest(requestStatusContextStatusChange);
	if (requestStatusContextStatusChange == KErrCancel)
		{
		INFO_PRINTF1(_L("Notify Context Status Change is KErrCancel\n"));
		}
	else
		{
		INFO_PRINTF2(_L("Error - Notify Context Status Change status  = %d\n"), requestStatusContextStatusChange.Int());
		}
	}

void CTestLtsyPktTestStepBase::NotificationContextStatusChange()
	{
	// request notification when context status changes
	INFO_PRINTF1(_L("RPacketContext::NotifyStatusChange() invoked...\n"));
	TRequestStatus requestStatusContextStatusChange;
	RPacketContext::TContextStatus newContextStatus;
	iModel.PktSharedData().iContext1.NotifyStatusChange(requestStatusContextStatusChange, newContextStatus);
	if (requestStatusContextStatusChange == KErrNone)
		{
		INFO_PRINTF1(_L("Context Status Changed\n"));
		}
	else
		{
		INFO_PRINTF2(_L("Error - Notify Context Status Change status  = %d\n"), requestStatusContextStatusChange.Int());
		}
	}
void CTestLtsyPktTestStepBase::OpenNewContextQos()
	{
	// Create QoS
	TInt returnStatus;
	INFO_PRINTF1(_L("OpenNewQoS() invoked...\n"));
	returnStatus = iModel.PktSharedData().iQoS.OpenNewQoS(iModel.PktSharedData().iContext1, iModel.PktSharedData().iQoSName);
	INFO_PRINTF2(_L("RPacketQoS::OpenNewQoS() ret = %d\n"), returnStatus);
	INFO_PRINTF2(_L("Context Name = %S\n"), &iModel.PktSharedData().iQoSName);
	}
void CTestLtsyPktTestStepBase::CloseExistContextQos()
	{
	// Close QoS
	INFO_PRINTF1(_L("CloseExistContextQos() invoked...\n"));
	iModel.PktSharedData().iQoS.Close();
	User::After(1000000);
	INFO_PRINTF1(_L("CloseExistContextQos() OK...\n"));
	}
void CTestLtsyPktTestStepBase::GprsContextDeleteTest()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("GprsContextDeleteTest() invoked...\n"));
	iModel.PktSharedData().iContext1.Delete(requestStatus);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketQoS::OpenNewQoS() ret = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked...\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput200000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput200000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	}
void  CTestLtsyPktTestStepBase::GprsGetQoSProfileTest()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::GetProfileParameters() invoked...\n"));
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosNegotiatedPckg(iModel.PktSharedData().iQoSNegotiated);   // pack
	iModel.PktSharedData().iQoS.GetProfileParameters(requestStatus, qosNegotiatedPckg);
	User::WaitForRequest(requestStatus);
	// iQoSNegotiated = qosNegotiatedPckg();   // unpack necessary ???
	INFO_PRINTF2(_L("    MinPrecedence       = %d\n"), iModel.PktSharedData().iQoSRequested.iMinPrecedence);
	INFO_PRINTF2(_L("    ReqPrecedence           = %d\n"), iModel.PktSharedData().iQoSRequested.iReqPrecedence);
	INFO_PRINTF2(_L("    ReqDelay           = %d\n"), iModel.PktSharedData().iQoSRequested.iReqDelay);
	INFO_PRINTF2(_L("    MinDelay        = %d\n"), iModel.PktSharedData().iQoSRequested.iMinDelay);
	INFO_PRINTF2(_L("    ReqReliability      = %d\n"), iModel.PktSharedData().iQoSRequested.iReqReliability);
	INFO_PRINTF2(_L("    MinReliability = %d\n"), iModel.PktSharedData().iQoSRequested.iMinReliability);
	INFO_PRINTF2(_L("    ReqPeakThroughput = %d\n"), iModel.PktSharedData().iQoSRequested.iReqPeakThroughput);
	INFO_PRINTF2(_L("    MinPeakThroughput      = %d\n"), iModel.PktSharedData().iQoSRequested.iMinPeakThroughput);
	INFO_PRINTF2(_L("    ReqMeanThroughput      = %d\n"), iModel.PktSharedData().iQoSRequested.iReqMeanThroughput);
	INFO_PRINTF2(_L("    MinMeanThroughput      = %d\n"), iModel.PktSharedData().iQoSRequested.iMinMeanThroughput);
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest1()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.1..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EPriorityHighPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EPriorityHighPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass1;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass1;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass1;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass1;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput100;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput100;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest2()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.2..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EPriorityMediumPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EPriorityMediumPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass2;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass2;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput2000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput2000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput200;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput200;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest3()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.3..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EPriorityLowPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EPriorityLowPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass3;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass3;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass3;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass3;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput4000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput4000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput500;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput500;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest4()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.4..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass4;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass4;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput8000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput8000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput1000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest5()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.5..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass5;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass5;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput16000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput16000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput2000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput2000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest6()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.6..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput32000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput32000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput5000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput5000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest7()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.7..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput64000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput64000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput10000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput10000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest8()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.8..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput128000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput128000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput20000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput20000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest9()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.9..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput256000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput256000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput50000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput50000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest10()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.10..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput100000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput100000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest11()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.11..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput200000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput200000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest12()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.12..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput500000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput500000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest13()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.13..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput1000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput1000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest14()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.14..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput2000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput2000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest15()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.15..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput5000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput5000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest16()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.16..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput10000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput10000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest17()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.17..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput20000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput20000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest18()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.18..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput50000000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput50000000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest19()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.29..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));
	iModel.PktSharedData().iQoSRequested.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	iModel.PktSharedData().iQoSRequested.iReqDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iMinDelay = RPacketQoS::EDelayClass2;
	iModel.PktSharedData().iQoSRequested.iReqReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iMinReliability = RPacketQoS::EReliabilityClass4;
	iModel.PktSharedData().iQoSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iMinPeakThroughput = RPacketQoS::EPeakThroughput1000;
	iModel.PktSharedData().iQoSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput200000;
	iModel.PktSharedData().iQoSRequested.iMinMeanThroughput = RPacketQoS::EMeanThroughput200000;

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void  CTestLtsyPktTestStepBase::GprsSetQoSProfileTest20()
	{
	TRequestStatus requestStatus = KErrNone;
	INFO_PRINTF1(_L("RPacketQoS::SetProfileParameters() invoked.20..\n"));
	INFO_PRINTF1(_L("  Use Assured Mode = True\n"));

	TPckg<RPacketQoS::TQoSGPRSRequested> qosRequestedPckg(iModel.PktSharedData().iQoSRequested);   // pack
	iModel.PktSharedData().iQoS.SetProfileParameters(requestStatus, qosRequestedPckg);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), requestStatus.Int());
	}
void CTestLtsyPktTestStepBase::GprsContextSetConfigTest1()
	{
	// Set Context Config
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig2;
	//contextConfig2.iServiceOption = RPacketContext::KLowSpeedData;
	contextConfig2.iPdpType  = RPacketContext::EPdpTypeIPv4;
	contextConfig2.iAccessPointName = KName;
	//contextConfig2.iPdpAddress = ;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg2(contextConfig2);
	INFO_PRINTF1(_L("RPacketContext::SetConfig1() invoked...\n"));
	iModel.PktSharedData().iContext1.SetConfig(reqStatusContextConfig, contextConfigPckg2);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::SetConfig1() complete = %d\n"), reqStatusContextConfig.Int());
	}
void CTestLtsyPktTestStepBase::GprsContextSetConfigTest2()
	{
	// Set Context Config
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig2;
	//contextConfig2.iServiceOption = RPacketContext::KLowSpeedData;
	contextConfig2.iPdpType  = RPacketContext::EPdpTypeIPv6;
	contextConfig2.iAccessPointName = KName;
	//contextConfig2.iPdpAddress = ;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg2(contextConfig2);
	INFO_PRINTF1(_L("RPacketContext::SetConfig2() invoked...\n"));
	iModel.PktSharedData().iContext1.SetConfig(reqStatusContextConfig, contextConfigPckg2);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::SetConfig2() complete = %d\n"), reqStatusContextConfig.Int());
	}
void CTestLtsyPktTestStepBase::GprsContextSetConfigTest3()
	{
	// Set Context Config
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig2;
	//contextConfig2.iServiceOption = RPacketContext::KLowSpeedData;
	contextConfig2.iPdpType  = RPacketContext::EPdpTypeX25;
	contextConfig2.iAccessPointName = KName;
	//contextConfig2.iPdpAddress = ;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg2(contextConfig2);
	INFO_PRINTF1(_L("RPacketContext::SetConfig3() invoked...\n"));
	iModel.PktSharedData().iContext1.SetConfig(reqStatusContextConfig, contextConfigPckg2);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::SetConfig3() complete = %d\n"), reqStatusContextConfig.Int());
	}
void CTestLtsyPktTestStepBase::GprsContextSetConfigTest4()
	{
	// Set Context Config
	TRequestStatus reqStatusContextConfig = KErrNone;
	RPacketContext::TContextConfigGPRS contextConfig2;
	contextConfig2.iAccessPointName = KName;
	//contextConfig2.iPdpAddress = ;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg2(contextConfig2);
	INFO_PRINTF1(_L("RPacketContext::SetConfig4() invoked...\n"));
	iModel.PktSharedData().iContext1.SetConfig(reqStatusContextConfig, contextConfigPckg2);
	User::WaitForRequest(reqStatusContextConfig);
	INFO_PRINTF2(_L("RPacketContext::SetConfig4() complete = %d\n"), reqStatusContextConfig.Int());
	}
// end of file
