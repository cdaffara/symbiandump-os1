/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "t_data_rgavdp.h"
#include "T_BTUtil.h"
#include <e32des8.h>

using namespace SymbianBluetoothAV;

/*@{*/

//Parameters
_LIT(KFldSocketServ,						"socketserv");
_LIT(KFldBTDevAddr,							"btdevaddr");
_LIT(KFldSEPInfo,							"avdtpsepinfo");
_LIT(KFldServiceCapability,					"avdtpservicecapability");
_LIT(KFldSecurityData,						"securitydata");
_LIT(KFldSEPsCount,							"expected_sepcount");
_LIT(KFldRemoteSEPs,						"remotesep_%d");
_LIT(KFldServiceCategory,					"servicecategory");
_LIT(KFldRemoteCaps,						"expected_remotecap_%d");
_LIT(KFldSEID,								"seid");
_LIT(KFldLocalSEID,							"localseid");
_LIT(KFldRemoteSEID,						"remoteseid");
_LIT(KFldIgnored1,							"ignored1");
_LIT(KFldIgnored2,							"ignored2");
_LIT(KFldSendData,							"senddata");
_LIT(KFldExpectedData,						"expecteddata");
_LIT(KFldExpectedEvents,					"expectedevents");
_LIT(KFldExpectedResponseData,				"expectedresponsedata");
_LIT(KFldExpectedMaxLength,					"expectedmaxlength");
_LIT(KFldExistRemoteSEPs,					"exist_remote_sep");
_LIT(KFldConfigReturnCode,					"config_return_code");
_LIT(KFldConfigEndReturnCode,				"config_end_return_code");

_LIT(KEGAVDP_NoneEvent,						"EGAVDP_NoneEvent");
_LIT(KEGAVDP_ConnectConfirm,				"EGAVDP_ConnectConfirm");
_LIT(KEGAVDP_SEPDiscovered,					"EGAVDP_SEPDiscovered");
_LIT(KEGAVDP_SEPDiscoveryComplete,			"EGAVDP_SEPDiscoveryComplete");
_LIT(KEGAVDP_SEPCapability,					"EGAVDP_SEPCapability");
_LIT(KEGAVDP_SEPCapabilityComplete,			"EGAVDP_SEPCapabilityComplete");
_LIT(KEGAVDP_AbortStreamConfirm,			"EGAVDP_AbortStreamConfirm");
_LIT(KEGAVDP_StartStreamsConfirm,			"EGAVDP_StartStreamsConfirm");
_LIT(KEGAVDP_SuspendStreamsConfirm,			"EGAVDP_SuspendStreamsConfirm");
_LIT(KEGAVDP_SecurityControlConfirm,		"EGAVDP_SecurityControlConfirm");
_LIT(KEGAVDP_ConfigurationConfirm,			"EGAVDP_ConfigurationConfirm");
_LIT(KEGAVDP_ConfigurationStartIndication,	"EGAVDP_ConfigurationStartIndication");
_LIT(KEGAVDP_ConfigurationIndication,		"EGAVDP_ConfigurationIndication");
_LIT(KEGAVDP_ConfigurationEndIndication,	"EGAVDP_ConfigurationEndIndication");
_LIT(KEGAVDP_StartIndication,				"EGAVDP_StartIndication");
_LIT(KEGAVDP_SuspendIndication,				"EGAVDP_SuspendIndication");
_LIT(KEGAVDP_SecurityControlIndication,		"EGAVDP_SecurityControlIndication");
_LIT(KEGAVDP_AbortIndication,				"EGAVDP_AbortIndication");
_LIT(KEGAVDP_ReleaseIndication,				"EGAVDP_ReleaseIndication");
_LIT(KEGAVDP_BearerReady,					"EGAVDP_BearerReady");
_LIT(KEGAVDP_Error,							"EGAVDP_Error");

//Commands
_LIT(KCmdDestructorGeneral,					"~");
_LIT(KCmdDestructor,						"~RGavdp");
_LIT(KCmdConstructor,						"new");
_LIT(KCmdOpen,								"Open");
_LIT(KCmdListen,							"Listen");
_LIT(KCmdConnect,							"Connect");
_LIT(KCmdClose,								"Close");
_LIT(KCmdCancel,							"Cancel");
_LIT(KCmdDiscoverRemoteSEPs,				"DiscoverRemoteSEPs");
_LIT(KCmdGetRemoteSEPCapabilities,			"GetRemoteSEPCapabilities");
_LIT(KCmdRegisterSEP,						"RegisterSEP");
_LIT(KCmdBeginConfiguringRemoteSEP,			"BeginConfiguringRemoteSEP");
_LIT(KCmdBeginConfiguringLocalSEP,			"BeginConfiguringLocalSEP");
_LIT(KCmdAddSEPCapability,					"AddSEPCapability");
_LIT(KCmdCommitSEPConfiguration,			"CommitSEPConfiguration");
_LIT(KCmdStartStream,						"StartStream");
_LIT(KCmdSuspendStream,						"SuspendStream");
_LIT(KCmdAbortStream,						"AbortStream");
_LIT(KCmdSendSecurityControl,				"SendSecurityControl");
_LIT(KCmdCreateBearerSockets,				"CreateBearerSockets");
_LIT(KCmdMaxSecurityControlLength,			"MaxSecurityControlLength");
_LIT(KCmdShutdown,							"Shutdown");
//Utility
_LIT(KCmdIncOutstanding,					"IncOutstanding");
_LIT(KCmdRead,								"Read");
_LIT(KCmdSend,								"Send");
_LIT(KCmdCloseBearerSockets,				"CloseBearerSockets");

///	Logging
_LIT(KLogInfoCmdConstructor,				"execute new RGavdp::RGavdp()");
_LIT(KLogInfoCmdOpen,						"execute RGavdp::Open()");
_LIT(KLogInfoCmdDestructor1,				"execute ~RGavdp()");
_LIT(KLogInfoCmdClose,						"execute RGavdp::Close()");
_LIT(KLogInfoCmdListen,						"execute RGavdp::Listen()");
_LIT(KLogInfoCmdCancel,						"execute RGavdp::Cancel()");
_LIT(KLogInfoCmdConnnect,        			"execute RGavdp::Connect()");
_LIT(KLogInfoCmdDiscoverRemoteSEPs,			"execute RGavdp::DiscoverRemoteSEPs()");
_LIT(KLogInfoCmdGetRemoteSEPCapabilities,	"execute RGavdp::GetRemoteSEPCapabilities()");
_LIT(KLogInfoCmdRegisterSEP,				"execute RGavdp::RegisterSEP()");
_LIT(KLogInfoCmdBeginConfiguringRemoteSEP,	"execute RGavdp::BeginConfiguringRemoteSEP()");
_LIT(KLogInfoCmdBeginConfiguringLocalSEP,	"execute RGavdp::BeginConfiguringLocalSEP()");
_LIT(KLogInfoCmdAddSEPCapability,			"execute RGavdp::AddSEPCapability()");
_LIT(KLogInfoCmdCommitSEPConfiguration,		"execute RGavdp::CommitSEPConfiguration()");
_LIT(KLogInfoCmdStartStream,				"execute RGavdp::StartStream()");
_LIT(KLogInfoCmdSuspendStream,				"execute RGavdp::SuspendStream()");
_LIT(KLogInfoCmdAbortStream,				"execute RGavdp::AbortStream()");
_LIT(KLogInfoCmdSendSecurityControl,		"execute RGavdp::SendSecurityControl()");
_LIT(KLogInfoCmdCreateBearerSockets1,		"execute RGavdp::CreateBearerSockets(TSEID)");
_LIT(KLogInfoCmdCreateBearerSockets2,		"execute RGavdp::CreateBearerSockets(TSEID, TBool, TBool)");
_LIT(KLogInfoCmdMaxSecurityControlLength1,	"execute RGavdp::MaxSecurityControlLength()");
_LIT(KLogInfoCmdShutdown,					"execute RGavdp::Shutdown()");
_LIT(KLogInfoReading,						"Start reading");
_LIT(KLogInfoSending,						"Start sending");
_LIT(KLogInfoCmdMaxSecurityControlLength2,	"RGavdp::MaxSecurityControlLength = %d");

_LIT(KLogErrMissingParameter,				"Missing parameter '%S'");
_LIT(KLogErrCmdConstructor,					"**** RGavdp Constructor failed with error %d");
_LIT(KLogErrCmdOpen,						"**** RGavdp Open failed with error %d");
_LIT(KLogErrCmdListen,						"**** RGavdp Listen failed with error %d");
_LIT(KLogErrCmdRegisterSEP,					"**** RGavdp RegisterSEP failed with error %d");
_LIT(KLogErrCmdBeginConfiguringRemoteSEP,	"**** RGavdp BeginConfiguringRemoteSEP failed with error %d");
_LIT(KLogErrCmdBeginConfiguringLocalSEP,	"**** RGavdp BeginConfiguringLocalSEP failed with error %d");
_LIT(KLogErrCmdAddSEPCapability,			"**** RGavdp AddSEPCapability failed with error %d");
_LIT(KLogErrCmdCreateBearerSockets,			"**** RGavdp CreateBearerSockets failed with error %d");
_LIT(KLogErrCmdShutdown,					"**** RGavdp Shutdown failed with error %d");

//call back
_LIT(KLogInfoConnectConfirm,				"MGavdpUser::GAVDP_ConnectConfirm() call back");
_LIT(KLogInfoSEPDiscovered,					"MGavdpUser::GAVDP_SEPDiscovered() call back");
_LIT(KLogInfoSEPDiscoveryComplete,			"MGavdpUser::GAVDP_SEPDiscoveryComplete() call back.");
_LIT(KLogInfoSEPCapability,					"MGavdpUser::GAVDP_SEPCapability() call back.");
_LIT(KLogInfoSEPCapabilityComplete,			"MGavdpUser::GAVDP_SEPCapabilityComplete() call back.");
_LIT(KLogInfoAbortStreamConfirm,			"MGavdpUser::GAVDP_AbortStreamConfirm() call back.");
_LIT(KLogInfoStartStreamsConfirm,			"MGavdpUser::GAVDP_StartStreamsConfirm() call back.");
_LIT(SuspendStreamsConfirm,					"MGavdpUser::GAVDP_SuspendStreamsConfirm() call back.");
_LIT(KLogInfoSecurityControlConfirm,		"MGavdpUser::GAVDP_SecurityControlConfirm() call back");
_LIT(KLogInfoConfigurationConfirm,			"MGavdpUser::GAVDP_ConfigurationConfirm() call back.");
_LIT(KLogInfoConfigurationStartIndication,	"MGavdpUser::GAVDP_ConfigurationStartIndication() call back");
_LIT(KLogInfoConfigurationIndication1,		"MGavdpUser::GAVDP_ConfigurationIndication() call back");
_LIT(KLogInfoConfigurationIndication2,		"Category = %d");
_LIT(KLogInfoConfigurationEndIndication,	"MGavdpUser::GAVDP_ConfigurationEndIndication() call back.");
_LIT(KLogInfoStartIndication,				"MGavdpUser::GAVDP_StartIndication() call back");
_LIT(KLogInfoSuspendIndication,				"MGavdpUser::GAVDP_SuspendIndication() call back");
_LIT(KLogInfoSecurityControlIndication,		"MGavdpUser::GAVDP_SecurityControlIndication() call back");
_LIT(KLogInfoAbortIndication,				"MGavdpUser::GAVDP_AbortIndication() call back");
_LIT(KLogInfoReleaseIndication,				"MGavdpUser::GAVDP_ReleaseIndication() call back");
_LIT(KLogInfoBearerReady,					"MGavdpUser::GAVDP_BearerReady() call back");
_LIT(KLogInfoError1,						"MGavdpUser::GAVDP_Error() call back");
_LIT(KLogInfoError2,						"aError: %d, aErrorData: %s");

_LIT(KLogInfoSEPValue,						"SEID: %d");
_LIT(KLogInfoMediaTypeAudio,				"Audio");
_LIT(KLogInfoMediaTypeVideo,				"Video");
_LIT(KLogInfoMediaTypeMultimedia,			"Multimedia");
_LIT(KLogInfoAvdtpSink,						"Sink");
_LIT(KLogInfoAvdtpSource,					"Source");
_LIT(KLogInfoAvdtpInUse,					"[In use]");
_LIT(KLogInfoServiceCategoryMediaTransport,	"Category = EServiceCategoryMediaTransport");
_LIT(KLogInfoServiceCategoryReporting,		"Category = EServiceCategoryReporting");
_LIT(KLogInfoServiceCategoryRecovery,		"Category = EServiceCategoryRecovery");
_LIT(KLogInfoServiceCategoryContentProtection, "Category = EServiceCategoryContentProtection");
_LIT(KLogInfoBearerTypeEMedia,				"EMedia");
_LIT(KLogInfoBearerTypeEReporting,			"EReporting");
_LIT(KLogInfoBearerTypeERecovery,			"ERecovery");
_LIT(KLogInfoSendSucess,					"Send successfully");
_LIT(KLogInfoReceived,						"Recevied Data :%S");


_LIT(KLogErrCallback,						"Called function is not as expected");
_LIT(KLogErrLocalSEID,						"SEID used for call back is not as expected, expected_SEID=%d actual_SEID=%d");
_LIT(KLogErrConnect,						"Remote device address is not as expected");
_LIT(KLogErrCapability,						"Received capability is not as expected");
_LIT(KLogErrExpectedEvents,					"Given expected events (%S) is invalid");
_LIT(KLogErrExpectedResponseData,			"Response security data is not as expected");
_LIT(KLogErrReceivedSecurityData,			"Received security data is not as expected");
_LIT(KLogErrExpectedMaxLength,				"MaxSecurityControlLength is not as expected");
_LIT(KLogErrExistRemoteSEPs,				"Given exist remote SEPs are invalid");
_LIT(KLogErrDiscoverRemoteSEPsCount,		"Discover remote SEPs fail expected_count=%d actual_count=%d");
_LIT(KLogErrReadData,						"Not received expected data");
_LIT(KLogErrSocket,							"Socket Error= %d");

//SecurityData
_LIT(KLogInfoSecurityDataUnchanged,			"Security data not changed");
_LIT(KLogInfoSecurityDataChanged,			"Have changed security data to: %S");
_LIT(KLogDataFixed,							"Do not change me");
_LIT(KLogDataChange,						"Change me");
_LIT(KLogDataChanged,						"Changed data");

/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataRGavdp* CT_DataRGavdp::NewL()
	{
	CT_DataRGavdp*	ret=new	(ELeave) CT_DataRGavdp();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRGavdp::CT_DataRGavdp()
:	iRGavdp(NULL),
	iRemoteAddr(NULL),
	iExpectedEventCount(0),
	iEventCount(0),
	iExpectedSEPsCount(0),
	iDiscoverdSEPsCount(0),
	iSrvCategory(EServiceCategoryNull),
	iReadBuffer(NULL),
	iReadBufferDes(NULL, NULL),
	iReadExpectBuffer(NULL),
	iSendBuffer(NULL),
	iExpectedResponseSecurityData(NULL),
	iActive(NULL),
	iCmdIndex(0),
	iConfigReturnCode(0),
	iConfigEndReturnCode(0)
	{
	}

void CT_DataRGavdp::ConstructL()
	{
	iActive = CActiveCallback::NewL(*this);
	}

CT_DataRGavdp::~CT_DataRGavdp()
	{
	DestroyData();

	delete iActive;
	iActive = NULL;
	}

void CT_DataRGavdp::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iRGavdp	= static_cast<RGavdp*> (aAny);
	}

void CT_DataRGavdp::DisownObjectL()
	{
	iRGavdp	= NULL;
	}

void CT_DataRGavdp::DestroyData()
	{
	DoCmdDestructor();
	
	iRemoteAddr=NULL;
	iExpectedEvent.Close();
	iExpectedEventCount=0;
	iEventCount=0;
	iExistRemoteSEIDs.Close();
	iExpectedSEPsCount=0;
	iDiscoverdSEPsCount=0;
	iRemoteSEPs.Close();
	iRemoteCapabilities.Close();
	iSrvCategory=EServiceCategoryNull;
	delete iReadBuffer;
	iReadBuffer = NULL;
	delete iReadExpectBuffer;
	iReadExpectBuffer = NULL;
	delete iExpectedResponseSecurityData;
	iExpectedResponseSecurityData = NULL;
	delete iSendBuffer;
	iSendBuffer = NULL;
	iConfigReturnCode = 0;
	iConfigEndReturnCode = 0;
	}

inline TCleanupOperation CT_DataRGavdp::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataRGavdp::CleanupOperation(TAny* aAny)
	{
	RGavdp* gavdp=static_cast<RGavdp*>(aAny);
	gavdp->Close();
	delete gavdp;
	}

/**
test script	command	entry point
@internalAll
@return	ETrue if syncronous	command	otherwise EFalse
@param aParam1 descriptor containing the command value
@param aParam2 descriptor containing the command parameter
@pre N/A
@post N/A
*/
TBool CT_DataRGavdp::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdDestructorGeneral || aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdConstructor )
		{
		DoCmdConstructor();
		}
	else if( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection);
		}
	else if( aCommand==KCmdListen )
		{
		DoCmdListen(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if( aCommand==KCmdCancel )
		{
		DoCmdCancel();
		}
	else if( aCommand==KCmdConnect )
		{
		DoCmdConnect(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdDiscoverRemoteSEPs )
		{
		DoCmdDiscoverRemoteSEPs(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdGetRemoteSEPCapabilities )
		{
		DoCmdGetRemoteSEPCapabilities(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdRegisterSEP )
		{
		DoCmdRegisterSEP(aSection);
		}
	else if( aCommand==KCmdBeginConfiguringRemoteSEP )
		{
		DoCmdBeginConfiguringRemoteSEP(aSection);
		}
	else if( aCommand==KCmdBeginConfiguringLocalSEP )
		{
		DoCmdBeginConfiguringLocalSEP(aSection);
		}
	else if( aCommand==KCmdAddSEPCapability )
		{
		DoCmdAddSEPCapability(aSection);
		}
	else if( aCommand==KCmdCommitSEPConfiguration )
		{
		DoCmdCommitSEPConfiguration(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdStartStream )
		{
		DoCmdStartStream(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdSuspendStream )
		{
		DoCmdSuspendStream(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdAbortStream )
		{
		DoCmdAbortStream(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdSendSecurityControl )
		{
		DoCmdSendSecurityControl(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdCreateBearerSockets )
		{
		DoCmdCreateBearerSockets(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdMaxSecurityControlLength )
		{
		DoCmdMaxSecurityControlLength(aSection);
		}
	else if( aCommand==KCmdShutdown )
		{
		DoCmdShutdown();
		}
	else if( aCommand==KCmdRead )
		{
		Read(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdSend )
		{
		Send(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdCloseBearerSockets)
		{
		CloseBearerSockets();
		}
	else if( aCommand==KCmdIncOutstanding )
		{
		IncreaseOutstanding(aSection, aAsyncErrorIndex);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_DataRGavdp::DoCmdDestructor()
	{
	if (iRGavdp) 
		{
		INFO_PRINTF1(KLogInfoCmdDestructor1);
		delete iRGavdp;
		iRGavdp = NULL;
		}
	}

void CT_DataRGavdp::DoCmdConstructor()
	{
	DestroyData();

	INFO_PRINTF1(KLogInfoCmdConstructor);
	TRAPD(err,	iRGavdp = new (ELeave) RGavdp());
	if(err	!= KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdConstructor, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdOpen(const TTEFSectionName& aSection)
	{
	TBool	dataOk=ETrue;
	
	RSocketServ* socketServ;
	if ( !CT_BTUtil::GetSocketServL(*this, aSection, KFldSocketServ, socketServ) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSocketServ());
		SetBlockResult(EFail);		
		}
	
	TInt err = KErrNone;
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdOpen);
		err = iRGavdp->Open(*this,*socketServ);
		}

	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdOpen, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdListen(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(KLogInfoCmdListen);
	
	if(!GetIntFromConfig(aSection, KFldConfigReturnCode(), iConfigReturnCode))
		{
		iConfigReturnCode=KErrNone;
		}
	if(!GetIntFromConfig(aSection, KFldConfigEndReturnCode(), iConfigEndReturnCode))
		{
		iConfigEndReturnCode=KErrNone;
		}
		
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		SetBlockResult(EFail);
		}
	else
		{
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		TInt err = iRGavdp->Listen();
		if(err != KErrNone)
			{
			ERR_PRINTF2(KLogErrCmdListen, err);
			SetError(err);
			}
		}
	}

void CT_DataRGavdp::DoCmdClose()
	{
	INFO_PRINTF1(KLogInfoCmdClose);
	iRGavdp->Close();
	}

void CT_DataRGavdp::DoCmdCancel()
	{
	INFO_PRINTF1(KLogInfoCmdCancel);
	iRGavdp->Cancel();
	}

void CT_DataRGavdp::DoCmdConnect(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool dataOk = ETrue;
	TPtrC		objectName;
	if (!GetStringFromConfig(aSection, KFldBTDevAddr(), objectName) )
		{
		dataOk = EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldBTDevAddr());
		SetBlockResult(EFail);
		}
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	if (dataOk)
		{
		iRemoteAddr = static_cast<TBTDevAddr*>(GetDataObjectL(objectName));
		INFO_PRINTF1(KLogInfoCmdConnnect);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->Connect(*iRemoteAddr);
		}
	}

void CT_DataRGavdp::DoCmdDiscoverRemoteSEPs(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool dataOk = ETrue;
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	iExistRemoteSEIDs.Reset();
	TPtrC existSEIDstr(0,0);
	if(GetStringFromConfig(aSection, KFldExistRemoteSEPs(), existSEIDstr) 
		&& (existSEIDstr.Length() != 0))
		{
		if (!ConvertSEIDs(existSEIDstr, iExistRemoteSEIDs))
			{
			ERR_PRINTF1(KLogErrExistRemoteSEPs);
			SetBlockResult(EFail);
			}
		}
		
	iExpectedSEPsCount=0;
	iDiscoverdSEPsCount=0;
	iRemoteSEPs.Reset();

	TInt SEPCount;
	TAvdtpSEPInfo* SEPInfo = NULL;
	if(!GetIntFromConfig(aSection, KFldSEPsCount(), iExpectedSEPsCount))
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEPsCount());
		SetBlockResult(EFail);
		}
	else
		{
		for(SEPCount=1; SEPCount<=iExpectedSEPsCount; SEPCount++)
			{
			tempStore.Format(KFldRemoteSEPs, SEPCount);
			if ( CT_BTUtil::GetAvdtpSEPInfoL(*this, aSection, tempStore, SEPInfo) )
				{
				iRemoteSEPs.Append(SEPInfo);
				SEPInfo = NULL;
				}
			else
				{
				dataOk = EFalse;
				ERR_PRINTF2(KLogErrMissingParameter, &tempStore);
				SetBlockResult(EFail);
				break;
				}
			}
		}
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdDiscoverRemoteSEPs);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->DiscoverRemoteSEPs();
		}
	}

void CT_DataRGavdp::DoCmdGetRemoteSEPCapabilities(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	iSrvCategory=EServiceCategoryNull;
	iRemoteCapabilities.Reset();
	
	TInt capsCount;
	TAvdtpSEPInfo* SEPInfo;
	TAvdtpServiceCapability* capability;
	if ( !CT_BTUtil::GetAvdtpSEPInfoL(*this, aSection, KFldSEPInfo, SEPInfo) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEPInfo());
		SetBlockResult(EFail);		
		}

	if ( !CT_BTUtil::ReadAvdtpServiceCategory(*this, aSection, KFldServiceCategory(), iSrvCategory))
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldServiceCategory());
		SetBlockResult(EFail);
		}
	else
		{
		if(iSrvCategory == EAllServiceCategories)
			{
			for(capsCount=0; capsCount<ENumberOfServiceCategories; capsCount++)
				{
				iRemoteCapabilities.Append(NULL);
				}
			dataOk=EFalse;
			for(capsCount=0; capsCount<ENumberOfServiceCategories; capsCount++)
				{
				tempStore.Format(KFldRemoteCaps, capsCount);
				if ( CT_BTUtil::GetAvdtpServiceCapabilityL(*this, aSection, tempStore, capability) )
					{
					iRemoteCapabilities[capability->Category()] = capability;
					dataOk=ETrue;
					}
				}
			}
		else
			{
			iRemoteCapabilities.Append(NULL);
			tempStore.Format(KFldRemoteCaps, 1);
			if ( CT_BTUtil::GetAvdtpServiceCapabilityL(*this, aSection, tempStore, capability) )
				{
				iRemoteCapabilities[0] = capability;
				}
			else
				{
				dataOk=EFalse;
				}
			}
		if ( !dataOk )
			{
			tempStore.Format(KFldRemoteCaps, 1);
			ERR_PRINTF2(KLogErrMissingParameter, &tempStore);
			SetBlockResult(EFail);
			}		
		}
	
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdGetRemoteSEPCapabilities);
		TAvdtpServiceCategories caps;
		caps.SetCapability(iSrvCategory);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->GetRemoteSEPCapabilities(SEPInfo->SEID(), caps);
		}
	}

void CT_DataRGavdp::DoCmdRegisterSEP(const TTEFSectionName& aSection)
	{
	TBool	dataOk=ETrue;
	
	TAvdtpSEPInfo* SEPInfo;
	if ( !CT_BTUtil::GetAvdtpSEPInfoL(*this, aSection, KFldSEPInfo, SEPInfo) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEPInfo());
		SetBlockResult(EFail);		
		}
	
	TInt err = 0;
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdRegisterSEP);
		err = iRGavdp->RegisterSEP(*SEPInfo);
		}
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdRegisterSEP, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdBeginConfiguringRemoteSEP(const TTEFSectionName& aSection)
	{
	TBool	dataOk=ETrue;
	
	TSEID* remoteSEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldRemoteSEID, remoteSEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldRemoteSEID());
		SetBlockResult(EFail);		
		}
	TSEID* localSEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldLocalSEID, localSEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldLocalSEID());
		SetBlockResult(EFail);
		}
	
	TInt err = 0;
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdBeginConfiguringRemoteSEP);
		err = iRGavdp->BeginConfiguringRemoteSEP(*remoteSEID, *localSEID);
		}
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdBeginConfiguringRemoteSEP, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdBeginConfiguringLocalSEP(const TTEFSectionName& aSection)
	{
	TBool	dataOk=ETrue;
	
	TSEID* localSEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, localSEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);
		}
	
	TInt err = 0;
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdBeginConfiguringLocalSEP);
		err = iRGavdp->BeginConfiguringLocalSEP(*localSEID);
		}
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdBeginConfiguringLocalSEP, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdAddSEPCapability(const TTEFSectionName& aSection)
	{
	TBool	dataOk=ETrue;
	
	TAvdtpServiceCapability* capability;
	if ( !CT_BTUtil::GetAvdtpServiceCapabilityL(*this, aSection, KFldServiceCapability, capability) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldServiceCapability());
		SetBlockResult(EFail);		
		}
	
	TInt err = 0;
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdAddSEPCapability);
		err = iRGavdp->AddSEPCapability(*capability);
		}
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdAddSEPCapability, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdCommitSEPConfiguration(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(KLogInfoCmdCommitSEPConfiguration);
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		SetBlockResult(EFail);
		}
	else
		{
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->CommitSEPConfiguration();
		}
	}

void CT_DataRGavdp::DoCmdStartStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	
	TSEID* SEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, SEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);		
		}
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdStartStream);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->StartStream(*SEID);
		}
	}

void CT_DataRGavdp::DoCmdSuspendStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	
	TSEID* SEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, SEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);		
		}
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdSuspendStream);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->SuspendStream(*SEID);
		}
	}

void CT_DataRGavdp::DoCmdAbortStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	
	TSEID* SEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, SEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);
		}
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdAbortStream);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->AbortStream(*SEID);
		}
	}

void CT_DataRGavdp::DoCmdSendSecurityControl(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	
	TSEID* SEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, SEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);		
		}
	
	TPtrC 	inputStr;
	HBufC8 *securityData = HBufC8::NewL(512);
	if( GetStringFromConfig(aSection, KFldSecurityData, inputStr))
		{
		TPtr8 des = securityData->Des();
		des.Copy(inputStr);
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSecurityData());
		SetBlockResult(EFail);		
		}
	
	if( GetStringFromConfig(aSection, KFldExpectedResponseData, inputStr))
		{
		if (!iExpectedResponseSecurityData)
			iExpectedResponseSecurityData = HBufC8::NewL(512);
		TPtr8 des = iExpectedResponseSecurityData->Des();
		des.Copy(inputStr);
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldExpectedResponseData());
		SetBlockResult(EFail);		
		if (iExpectedResponseSecurityData)
			{
			delete iExpectedResponseSecurityData;
			iExpectedResponseSecurityData = NULL;
			}
		}
	
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	
	if ( dataOk )
		{
		INFO_PRINTF1(KLogInfoCmdSendSecurityControl);
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		iRGavdp->SendSecurityControl(*SEID, *securityData);
		}
	
	delete 	securityData;
	}

void CT_DataRGavdp::DoCmdCreateBearerSockets(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	TInt    err = 0;
	
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		dataOk = EFalse;
		SetBlockResult(EFail);
		}
	
	TSEID* SEID;
	if ( !CT_BTUtil::GetSEIDL(*this, aSection, KFldSEID, SEID) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSEID());
		SetBlockResult(EFail);		
		}

	TBool	ignored1;
	TBool	ignored2;
	if ( (!GetBoolFromConfig(aSection, KFldIgnored1, ignored1))
		|| (!GetBoolFromConfig(aSection, KFldIgnored2, ignored2)) )
		{
		if ( dataOk )
			{
			// Execute command and log parameters
			INFO_PRINTF1(KLogInfoCmdCreateBearerSockets1);
			SetEventOutstanding();
			iCmdIndex = aAsyncErrorIndex;
			err = iRGavdp->CreateBearerSockets(*SEID);
			}
		}
	else
		{
		if ( dataOk )
			{
			// Execute command and log parameters
			INFO_PRINTF1(KLogInfoCmdCreateBearerSockets2);
			SetEventOutstanding();
			iCmdIndex = aAsyncErrorIndex;
			err = iRGavdp->CreateBearerSockets(*SEID, ignored1, ignored2);
			}
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdCreateBearerSockets, err);
		SetError(err);
		}
	}

void CT_DataRGavdp::DoCmdMaxSecurityControlLength(const TTEFSectionName& aSection)
	{
	TInt actual = 0;
	INFO_PRINTF1(KLogInfoCmdMaxSecurityControlLength1);
	actual = iRGavdp->MaxSecurityControlLength();
	INFO_PRINTF2(KLogInfoCmdMaxSecurityControlLength2, actual);
	
	TInt	expected;
	if ( GetIntFromConfig(aSection, KFldExpectedMaxLength(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(KLogErrExpectedMaxLength);
			SetBlockResult(EFail);
			}
		}
	}

void CT_DataRGavdp::DoCmdShutdown()
	{
	INFO_PRINTF1(KLogInfoCmdShutdown);
	TInt err = iRGavdp->Shutdown();
	if(err != KErrNone)
		{
		ERR_PRINTF2(KLogErrCmdShutdown, err);
		SetError(err);
		}
	}


void CT_DataRGavdp::GAVDP_ConnectConfirm(const TBTDevAddr& aDeviceAddr)
	{
	INFO_PRINTF1(KLogInfoConnectConfirm);
	CheckEvent(EGAVDP_ConnectConfirm);
	if ( (iRemoteAddr)
		&&( aDeviceAddr != *iRemoteAddr ) )
		{
		ERR_PRINTF1(KLogErrConnect);
		SetAsyncError(iCmdIndex, KErrGeneral);
		}
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_SEPDiscovered(const TAvdtpSEPInfo& aSEP)
	{
	INFO_PRINTF1(KLogInfoSEPDiscovered);
	
	INFO_PRINTF2(KLogInfoSEPValue, aSEP.SEID().Value());

	switch (aSEP.MediaType())
		{
		case EAvdtpMediaTypeAudio:
			INFO_PRINTF1(KLogInfoMediaTypeAudio);
			break;
		case EAvdtpMediaTypeVideo:
			INFO_PRINTF1(KLogInfoMediaTypeVideo);
			break;
		case EAvdtpMediaTypeMultimedia:
			INFO_PRINTF1(KLogInfoMediaTypeMultimedia);
			break;
		}
	
	if (aSEP.IsSink())
		INFO_PRINTF1(KLogInfoAvdtpSink);
	else 
		INFO_PRINTF1(KLogInfoAvdtpSource);
	
	if (aSEP.InUse())
		INFO_PRINTF1(KLogInfoAvdtpInUse);
	
	if (!isExistSEID( aSEP.SEID().Value()))
		{
		iDiscoverdSEPsCount += 1;
		if (iDiscoverdSEPsCount <= iExpectedSEPsCount)
			{
			*(iRemoteSEPs[iDiscoverdSEPsCount-1])=aSEP;
			}
		
		CheckEvent(EGAVDP_SEPDiscovered);
		DecOutstanding();
		}
	
	}
void CT_DataRGavdp::GAVDP_SEPDiscoveryComplete()
	{
	INFO_PRINTF1(KLogInfoSEPDiscoveryComplete);
	//verify
	CheckEvent(EGAVDP_SEPDiscoveryComplete);
	if (iDiscoverdSEPsCount != iExpectedSEPsCount)
		{
		ERR_PRINTF3(KLogErrDiscoverRemoteSEPsCount, iExpectedSEPsCount, iDiscoverdSEPsCount);
		SetAsyncError(iCmdIndex, KErrGeneral);
		}
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_SEPCapability(TAvdtpServiceCapability* aCapability)
	{
	INFO_PRINTF1(KLogInfoSEPCapability);

	//verify
	TAvdtpServiceCategory category = aCapability->Category();
	switch(category)
		{
		case EServiceCategoryMediaTransport:
			INFO_PRINTF1(KLogInfoServiceCategoryMediaTransport);
			if (!iRemoteCapabilities[category])
				{
				ERR_PRINTF1(KLogErrCapability);
				SetAsyncError(iCmdIndex, KErrGeneral);
				}
			break;
		case EServiceCategoryReporting:
			INFO_PRINTF1(KLogInfoServiceCategoryReporting);
			if (!iRemoteCapabilities[category])
				{
				ERR_PRINTF1(KLogErrCapability);
				SetAsyncError(iCmdIndex, KErrGeneral);
				}			
			break;
		case EServiceCategoryRecovery:
			{
			INFO_PRINTF1(KLogInfoServiceCategoryRecovery);
			TAvdtpRecoveryCapabilities* received = (TAvdtpRecoveryCapabilities*)aCapability;
			TAvdtpRecoveryCapabilities* expected = (TAvdtpRecoveryCapabilities*)iRemoteCapabilities[category];
			if ( (received->MaxWindowSize() != expected->MaxWindowSize())
					||(received->MinWindowSize() != expected->MinWindowSize())
					||(received->RecoveryType() != expected->RecoveryType()))
				{
				ERR_PRINTF1(KLogErrCapability);
				SetAsyncError(iCmdIndex, KErrGeneral);
				}
			}
			break;
		case EServiceCategoryContentProtection:
			{
			INFO_PRINTF1(KLogInfoServiceCategoryContentProtection);
			TAvdtpContentProtectionCapabilities* received = (TAvdtpContentProtectionCapabilities*)aCapability;
			TAvdtpContentProtectionCapabilities* expected = (TAvdtpContentProtectionCapabilities*)iRemoteCapabilities[category];
			if ( ( received->ContentProtectionType() != expected->ContentProtectionType() )
				||( received->ContentProtectionData().Compare(expected->ContentProtectionData())!=0 ) )
				{
				ERR_PRINTF1(KLogErrCapability);
				SetAsyncError(iCmdIndex, KErrGeneral);
				}
			}
			break;
		default:
			break;
		}
	delete aCapability;
	
	CheckEvent(EGAVDP_SEPCapability);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_SEPCapabilityComplete()
	{
	INFO_PRINTF1(KLogInfoSEPCapabilityComplete);
	//verify
	CheckEvent(EGAVDP_SEPCapabilityComplete);
	DecOutstanding();
	
	}
void CT_DataRGavdp::GAVDP_AbortStreamConfirm()
	{
	INFO_PRINTF1(KLogInfoAbortStreamConfirm);
	//verify
	CheckEvent(EGAVDP_AbortStreamConfirm);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_StartStreamsConfirm()
	{
	INFO_PRINTF1(KLogInfoStartStreamsConfirm);
	//verify
	CheckEvent(EGAVDP_StartStreamsConfirm);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_SuspendStreamsConfirm()
	{
	INFO_PRINTF1(SuspendStreamsConfirm);
	//verify
	CheckEvent(EGAVDP_SuspendStreamsConfirm);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_SecurityControlConfirm(const TDesC8& aResponseData)
	{
	INFO_PRINTF1(KLogInfoSecurityControlConfirm);

	//verify
	if (!iExpectedResponseSecurityData)
		{
		ERR_PRINTF2(KLogErrMissingParameter, &KFldExpectedResponseData());
		SetBlockResult(EFail);		
		}
	else if (iExpectedResponseSecurityData->Compare(aResponseData)!=0)
		{
		ERR_PRINTF1(KLogErrExpectedResponseData);
		SetAsyncError(iCmdIndex, KErrGeneral);
		}
	
	CheckEvent(EGAVDP_SecurityControlConfirm);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_ConfigurationConfirm()
	{
	INFO_PRINTF1(KLogInfoConfigurationConfirm);
	//verify
	CheckEvent(EGAVDP_ConfigurationConfirm);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_ConfigurationStartIndication(TSEID aLocalSEID, TSEID aRemoteSEID)
	{
	INFO_PRINTF1(KLogInfoConfigurationStartIndication);
	iLocalSEID = aLocalSEID;
	iRemoteSEID = aRemoteSEID;
	//verify
	CheckEvent(EGAVDP_ConfigurationStartIndication);
	DecOutstanding();
	}
TInt CT_DataRGavdp::GAVDP_ConfigurationIndication(TAvdtpServiceCapability* aCapability)
	{
	INFO_PRINTF1(KLogInfoConfigurationIndication1);
	INFO_PRINTF2(KLogInfoConfigurationIndication2, aCapability->Category());
	delete aCapability;
	//verify
	CheckEvent(EGAVDP_ConfigurationIndication);
	DecOutstanding();
	return iConfigReturnCode;
	}
TInt CT_DataRGavdp::GAVDP_ConfigurationEndIndication()
	{
	INFO_PRINTF1(KLogInfoConfigurationEndIndication);
	//verify
	CheckEvent(EGAVDP_ConfigurationEndIndication);
	DecOutstanding();
	return iConfigEndReturnCode;
	}
TInt CT_DataRGavdp::GAVDP_StartIndication(TSEID aSEID)
	{
	INFO_PRINTF1(KLogInfoStartIndication);
	//verify
	CheckEvent(EGAVDP_StartIndication);
	CheckLocalSEID(aSEID);
	DecOutstanding();
	return KErrNone;
	}
TInt CT_DataRGavdp::GAVDP_SuspendIndication(TSEID aSEID)
	{
	INFO_PRINTF1(KLogInfoSuspendIndication);
	//verify
	CheckEvent(EGAVDP_SuspendIndication);
	CheckLocalSEID(aSEID);
	DecOutstanding();
	return KErrNone;
	}
TInt CT_DataRGavdp::GAVDP_SecurityControlIndication(TSEID /*aSEID*/, TDes8& aSecurityDataInOut)
	{
	INFO_PRINTF1(KLogInfoSecurityControlIndication);

	//verify
	CheckEvent(EGAVDP_SecurityControlIndication);
	
	HBufC16* securityDataInOut = HBufC16::NewL(aSecurityDataInOut.Length());
	securityDataInOut->Des().Copy(aSecurityDataInOut);
	if (KLogDataFixed().Compare(*securityDataInOut)==0)
		{
		INFO_PRINTF1(KLogInfoSecurityDataUnchanged);
		}
	else if(KLogDataChange().Compare(*securityDataInOut)==0)
		{
		aSecurityDataInOut.Copy(KLogDataChanged());
		INFO_PRINTF2(KLogInfoSecurityDataChanged, &KLogDataChanged());
		}
	else
		{
		ERR_PRINTF1(KLogErrReceivedSecurityData);
		SetAsyncError(iCmdIndex, KErrGeneral);
		}
	delete securityDataInOut;
	
	DecOutstanding();
	return KErrNone;
	}
void CT_DataRGavdp::GAVDP_AbortIndication(TSEID aSEID)
	{
	INFO_PRINTF1(KLogInfoAbortIndication);
	//verify
	CheckEvent(EGAVDP_AbortIndication);
	CheckLocalSEID(aSEID);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_ReleaseIndication(TSEID aSEID)
	{
	INFO_PRINTF1(KLogInfoReleaseIndication);
	//verify
	CheckEvent(EGAVDP_ReleaseIndication);
	CheckLocalSEID(aSEID);
	DecOutstanding();
	}
void CT_DataRGavdp::GAVDP_BearerReady(RSocket aSocket, const TAvdtpSockAddr& aAddress)
	{
	INFO_PRINTF1(KLogInfoBearerReady);
		
	switch(aAddress.Session())
		{
		case EMedia:
			INFO_PRINTF1(KLogInfoBearerTypeEMedia);
			iSockets[0] = aSocket;
			break;
		case EReporting:
			INFO_PRINTF1(KLogInfoBearerTypeEReporting);
			iSockets[1] = aSocket;
			break;
		case ERecovery:
			INFO_PRINTF1(KLogInfoBearerTypeERecovery);
			iSockets[2] = aSocket;
			break;
		default:
			break;
		}
	
	//verify
	CheckEvent(EGAVDP_BearerReady);
	DecOutstanding();
	
	}
void CT_DataRGavdp::GAVDP_Error(TInt aError, const TDesC8& aErrorData)
	{
	INFO_PRINTF1(KLogInfoError1);
	INFO_PRINTF3(KLogInfoError2, aError, &aErrorData);
	SetAsyncError(iCmdIndex, aError);
	//verify
	CheckEvent(EGAVDP_Error);
	DecOutstanding();
	}

void CT_DataRGavdp::CloseBearerSockets()
	{
	iSockets[0].Close();
	iSockets[1].Close();
	iSockets[2].Close();
	}

void CT_DataRGavdp::Read(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TPtrC 	inputStr;
	if( GetStringFromConfig(aSection, KFldExpectedData, inputStr))
		{
		if (!iReadExpectBuffer)
			iReadExpectBuffer = HBufC8::NewL(512);
		TPtr8 des = iReadExpectBuffer->Des();
		des.Copy(inputStr);
		}
	else
		{
		if (iReadExpectBuffer)
			{
			delete iReadExpectBuffer;
			iReadExpectBuffer = NULL;
			}
		}
	
	INFO_PRINTF1(KLogInfoReading);
	iReadBuffer = HBufC8::NewL(512);
	iReadBuffer->Des().SetMax();
	iReadBufferDes.Set(iReadBuffer->Des());
	
	iSockets[0].Read(iReadBufferDes,iActive->iStatus);
	iActive->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}
void CT_DataRGavdp::RunL(CActive* aActive, TInt aIndex)
	{
    TInt err = aActive->iStatus.Int();
    SetAsyncError(aIndex, err);
    DecOutstanding();
    
    if (iReadBuffer)
    	{
    	if (aActive->iStatus.Int()==KErrNone)
    		{
    		if (iReadExpectBuffer)
    			{
    			if (iReadExpectBuffer->Compare(*iReadBuffer)!=0)
    				{
    				ERR_PRINTF1(KLogErrReadData);
    				SetError(EFail);
    				}
    			}
    		HBufC16* readbufdes = HBufC16::NewL(iReadBuffer->Length());
    		readbufdes->Des().Copy(*iReadBuffer);
    		INFO_PRINTF2(KLogInfoReceived,readbufdes);
    		delete readbufdes;
    		}
    	else
    		{
    		ERR_PRINTF2(KLogErrSocket, aActive->iStatus.Int());
    		}
   	
    	delete iReadBuffer;
    	iReadBuffer = NULL;
    	}

    if (iSendBuffer)
    	{
    	if (aActive->iStatus.Int()!=KErrNone)
    		{
    		ERR_PRINTF2(KLogErrSocket, aActive->iStatus.Int());
    		}
    	else
    		INFO_PRINTF1(KLogInfoSendSucess);
    	delete iSendBuffer;
    	iSendBuffer = NULL;
    	}

	}
void CT_DataRGavdp::Send(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TPtrC 	inputStr;
	if( GetStringFromConfig(aSection, KFldSendData, inputStr))
		{
		INFO_PRINTF1(KLogInfoSending);
		iSendBuffer = HBufC8::NewL(512);
		TPtr8 des = iSendBuffer->Des();
		des.Copy(inputStr);
		
		iSockets[0].Write(*iSendBuffer, iActive->iStatus);
		iActive->Activate(aAsyncErrorIndex);
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF2(KLogErrMissingParameter, &KFldSendData());
		SetBlockResult(EFail);		
		}
	}

void CT_DataRGavdp::SetEventOutstanding()
	{
	TInt count = iExpectedEventCount;
	for (;count>0;count--)
		{
		IncOutstanding();
		}
	}

void CT_DataRGavdp::IncreaseOutstanding(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	if (!ReadExpectedEventsFromConfig(aSection))
		{
		SetBlockResult(EFail);
		}
	else
		{
		SetEventOutstanding();
		iCmdIndex = aAsyncErrorIndex;
		}
	}

TBool CT_DataRGavdp::ReadExpectedEventsFromConfig(const TTEFSectionName& aSection)
	{
	TBool ret = ETrue;
	TPtrC inputStr;
	
	if( GetStringFromConfig(aSection, KFldExpectedEvents, inputStr))
		{
		iExpectedEvent.Reset();
		iExpectedEventCount = 0;
		iEventCount = 0;
		if ( ConvertEvents(inputStr, iExpectedEvent) )
			{
			if (iExpectedEvent[0] != EGAVDP_NoneEvent)
				iExpectedEventCount = iExpectedEvent.Count();
			}
		else
			{
			iExpectedEvent.Reset();
			ERR_PRINTF2(KLogErrExpectedEvents, &inputStr);
			ret = EFalse;
			}
		}

	return ret;
	}

TBool CT_DataRGavdp::ConvertEvents(TDesC &aEventStr, RArray<TUint>& aEvent)
	{
	
	TBool ret = ETrue;

	if (aEventStr == KEGAVDP_NoneEvent)
		{
		aEvent.Append(EGAVDP_NoneEvent);
		}
	else if (aEventStr == KEGAVDP_ConnectConfirm)
		{
		aEvent.Append(EGAVDP_ConnectConfirm);
		}
	else if (aEventStr == KEGAVDP_SEPDiscovered)
		{
		aEvent.Append(EGAVDP_SEPDiscovered);
		}
	else if (aEventStr == KEGAVDP_SEPDiscoveryComplete)
		{
		aEvent.Append(EGAVDP_SEPDiscoveryComplete);
		}
	else if (aEventStr == KEGAVDP_SEPCapability)
		{
		aEvent.Append(EGAVDP_SEPCapability);
		}
	else if (aEventStr == KEGAVDP_SEPCapabilityComplete)
		{
		aEvent.Append(EGAVDP_SEPCapabilityComplete);
		}
	else if (aEventStr == KEGAVDP_AbortStreamConfirm)
		{
		aEvent.Append(EGAVDP_AbortStreamConfirm);
		}
	else if (aEventStr == KEGAVDP_StartStreamsConfirm)
		{
		aEvent.Append(EGAVDP_StartStreamsConfirm);
		}
	else if (aEventStr == KEGAVDP_SuspendStreamsConfirm)
		{
		aEvent.Append(EGAVDP_SuspendStreamsConfirm);
		}
	else if (aEventStr == KEGAVDP_SecurityControlConfirm)
		{
		aEvent.Append(EGAVDP_SecurityControlConfirm);
		}
	else if (aEventStr == KEGAVDP_ConfigurationConfirm)
		{
		aEvent.Append(EGAVDP_ConfigurationConfirm);
		}
	else if (aEventStr == KEGAVDP_ConfigurationStartIndication)
		{
		aEvent.Append(EGAVDP_ConfigurationStartIndication);
		}
	else if (aEventStr == KEGAVDP_ConfigurationIndication)
		{
		aEvent.Append(EGAVDP_ConfigurationIndication);
		}
	else if (aEventStr == KEGAVDP_ConfigurationEndIndication)
		{
		aEvent.Append(EGAVDP_ConfigurationEndIndication);
		}
	else if (aEventStr == KEGAVDP_StartIndication)
		{
		aEvent.Append(EGAVDP_StartIndication);
		}
	else if (aEventStr == KEGAVDP_SuspendIndication)
		{
		aEvent.Append(EGAVDP_SuspendIndication);
		}
	else if (aEventStr == KEGAVDP_SecurityControlIndication)
		{
		aEvent.Append(EGAVDP_SecurityControlIndication);
		}
	else if (aEventStr == KEGAVDP_AbortIndication)
		{
		aEvent.Append(EGAVDP_AbortIndication);
		}
	else if (aEventStr == KEGAVDP_ReleaseIndication)
		{
		aEvent.Append(EGAVDP_ReleaseIndication);
		}
	else if (aEventStr == KEGAVDP_BearerReady)
		{
		aEvent.Append(EGAVDP_BearerReady);
		}
	else if (aEventStr == KEGAVDP_Error)
		{
		aEvent.Append(EGAVDP_Error);
		}
	else
		{
		TInt location = aEventStr.Match(_L("*|*"));
		if( location != KErrNotFound )
			{
			//Converting Left part of the data
			TPtrC	tempStr = aEventStr.Left(location);
			ret = ConvertEvents(tempStr, aEvent);
			if(ret)
				{
				//Converting right data can be with another "|"
				tempStr.Set(aEventStr.Mid(location + 1));
				ret = ConvertEvents(tempStr, aEvent);
				}
			}
		else
			{
			ret = EFalse;
			}
		}

	return ret;
	}

void CT_DataRGavdp::CheckEvent(TUint aEvent)
	{
	iEventCount++;
	if ((iExpectedEventCount == 0)
			||(iEventCount > iExpectedEventCount)
			||(iExpectedEvent[iEventCount-1] != aEvent) )
		{
		ERR_PRINTF1(KLogErrCallback);
		if (aEvent == EGAVDP_Error)
			SetBlockResult(EFail);
		else
			SetAsyncError(iCmdIndex, KErrGeneral);
		}
	};
void CT_DataRGavdp::CheckLocalSEID(TSEID aSEID)
	{
	if (aSEID != iLocalSEID )
		{
		ERR_PRINTF3(KLogErrLocalSEID, iLocalSEID.SEID(), aSEID.SEID());
		SetAsyncError(iCmdIndex, KErrGeneral);
		}
	}

TBool CT_DataRGavdp::ConvertSEIDs(TDesC &aSEIDsStr, RArray<TUint>& aSEIDs)
	{
	
	TBool ret = ETrue;

	TLex lex(aSEIDsStr);
	TInt tempSEID;
	if (lex.Val(tempSEID) == KErrNone)
		{
		aSEIDs.Append(tempSEID);
		}
	else
		{
		TInt location = aSEIDsStr.Match(_L("*,*"));
		if( location != KErrNotFound )
			{
			//Converting Left part of the data
			TPtrC	tempStr = aSEIDsStr.Left(location);
			ret = ConvertSEIDs(tempStr, aSEIDs);
			if(ret)
				{
				//Converting right data can be with another ","
				tempStr.Set(aSEIDsStr.Mid(location + 1));
				ret = ConvertSEIDs(tempStr, aSEIDs);
				}
			}
		else
			{
			ret = EFalse;
			}
		}

	return ret;
	}

TBool CT_DataRGavdp::isExistSEID(TUint aSEID)
	{
	TInt i;
	for (i=0; i<iExistRemoteSEIDs.Count(); i++)
		{
		if (iExistRemoteSEIDs[i] == aSEID)
			return ETrue;
		}
	return EFalse;
	}
