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

#include "T_CBluetoothSocketData.h"
#include "T_BTUtil.h"
#include "T_TBTSockAddrChild.h"
#include "T_BTDevAddrData.h"
#include "T_BTSockAddrData.h"

//  EPOC includes
#include <e32math.h>


/*@{*/
///	Constant Literals used.

///	Parameters
_LIT(KExpectedReply,							"expectedreply");
_LIT(KCBluetoothSocket,							"cbluetoothsocket");
_LIT(KConnectData,								"connectdata");
_LIT(KData,										"data");
_LIT(KDesOption,								"desoption");
_LIT(KDisconnectData, 							"disconnectData");
_LIT(KEquals,									"equals");
_LIT(KExpectNull,								"expectnull");
_LIT(KFlags,									"flags");
_LIT(KGetDes,									"getdes");
_LIT(KIntOption,								"intoption");
_LIT(KNamedIntOption,							"namedintoption");
_LIT(KNamedProtocol,							"named_protocol");
_LIT(KOptionLevel,								"optionlevel");
_LIT(KOptionName,								"optionname");
_LIT(KPort,										"port");
_LIT(KProtocol,									"protocol");
_LIT(KQueueSize,								"queuesize");
_LIT(KRSocket,									"rsocket");
_LIT(KServiceBits,								"servicebits");
_LIT(KSockType,									"socktype");
_LIT(KTBTSockAddr,								"tbtsockaddr");
_LIT(KTSockAddr,								"tsockaddr");
_LIT(KUid,										"uid");
_LIT(KUseAccept2,								"useaccept2");
_LIT(KUseAlternateNotifier,						"usealternatenotifier");
_LIT(KUseDeprecated,							"usedeprecated");
_LIT(KUseRecv2,									"userecv2");
_LIT(KUseSend2,									"usesend2");
_LIT(KHexBTAddrL,								"hex_btaddr_l");
_LIT(KHexBTAddrR,								"hex_btaddr_r");
_LIT(KTBTDevAddr,								"tbtdevaddr");
_LIT(KCallTwice,								"calltwice");
_LIT(KMTUSize,									"mtusize");
_LIT(KState,									"state");
_LIT(KConstructorType,							"constructortype");
_LIT(KSniffMode,								"sniffmode");
_LIT(KSniffDelay,								"sniffdelay");

///	CBluetoothSocket
_LIT(KCmdAccept,								"Accept");
_LIT(KCmdActivateBasebandEventNotifier,			"ActivateBasebandEventNotifier");
_LIT(KCmdActivateParkRequester,					"ActivateParkRequester");
_LIT(KCmdActivateSniffRequester,				"ActivateSniffRequester");
_LIT(KCmdAllowLowPowerModes,					"AllowLowPowerModes");
_LIT(KCmdAllowRoleSwitch,						"AllowRoleSwitch");
_LIT(KCmdBind,									"Bind");
_LIT(KCmdCancelAccept,							"CancelAccept");
_LIT(KCmdCancelAll,								"CancelAll");
_LIT(KCmdCancelBasebandEventNotifier,			"CancelBasebandEventNotifier");
_LIT(KCmdCancelConnect,							"CancelConnect");
_LIT(KCmdCancelIoctl,							"CancelIoctl");
_LIT(KCmdCancelLowPowerModeRequester,			"CancelLowPowerModeRequester");
_LIT(KCmdCancelRead,							"CancelRead");
_LIT(KCmdCancelRecv,							"CancelRecv");
_LIT(KCmdCancelSend,							"CancelSend");
_LIT(KCmdCancelWrite,							"CancelWrite");
_LIT(KCmdConnect,								"Connect");
_LIT(KCmdDestructor,							"~");
_LIT(KCmdGetDisconnectData,						"GetDisconnectData");
_LIT(KCmdGetOpt,								"GetOpt");
_LIT(KCmdInfo,									"Info");
_LIT(KCmdIoctl,									"Ioctl");
_LIT(KCmdListen,								"Listen");
_LIT(KCmdLocalName,								"LocalName");
_LIT(KCmdLocalPort,								"LocalPort");
_LIT(KCmdName,									"Name");
_LIT(KCmdNewL, 									"NewL");
_LIT(KCmdNewLC, 								"NewLC");
_LIT(KCmdPhysicalLinkState,						"PhysicalLinkState");
_LIT(KCmdPreventLowPowerModes,					"PreventLowPowerModes");
_LIT(KCmdPreventRoleSwitch,						"PreventRoleSwitch");
_LIT(KCmdRConnectionClose,						"RConnectionClose");
_LIT(KCmdRConnectionStart,						"RConnectionStart");
_LIT(KCmdRead,									"Read");
_LIT(KCmdRecv,									"Recv");
_LIT(KCmdRecvFrom,								"RecvFrom");
_LIT(KCmdRecvOneOrMore,							"RecvOneOrMore");
_LIT(KCmdRemoteName,							"RemoteName");
_LIT(KCmdRequestChangeSupportedPacketTypes,		"RequestChangeSupportedPacketTypes");
_LIT(KCmdRequestMasterRole,						"RequestMasterRole");
_LIT(KCmdRequestSlaveRole,						"RequestSlaveRole");
_LIT(KCmdRSocketServerClose,					"RSocketServerClose");
_LIT(KCmdRSocketServerConnect,					"RSocketServerConnect");
_LIT(KCmdSend,									"Send");
_LIT(KCmdSendTo,								"SendTo");
_LIT(KCmdSetAutomaticSniffMode,					"SetAutomaticSniffMode");
_LIT(KCmdSetLocalPort,							"SetLocalPort");
_LIT(KCmdSetOpt,								"SetOpt");
_LIT(KCmdSetOption,								"SetOption");
_LIT(KCmdSetRsocketServerFromCBTSocket,			"SetRsocketServerFromCBTSocket");
_LIT(KCmdSetTransferAble,						"SetTransferAble");
_LIT(KCmdShutdown,								"Shutdown");
_LIT(KCmdSetNotifier,							"SetNotifier");
_LIT(KCmdTestMBSN_ExtensionInterfaceL,			"TestMBSN_ExtensionInterfaceL");
_LIT(KCmdTransfer,								"Transfer");
_LIT(KCmdWrite,									"Write");

///	Other
_LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
/*@}*/

CT_CBluetoothSocketData* CT_CBluetoothSocketData::NewL()
	{
	CT_CBluetoothSocketData*	ret=new (ELeave) CT_CBluetoothSocketData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
* First phase construction
*/
CT_CBluetoothSocketData::CT_CBluetoothSocketData()
:	iData(NULL)
,	iHasConnectDataToExpect(EFalse)
,	iHasAcceptDataToExpect(EFalse)
,	iHasReceiveDataToExpect(EFalse)
,	iConnectIndex(0)
,	iAcceptIndex(0)
,	iShutdownIndex(0)
,	iIoctlIndex(0)
,	iNotifierIndex(0)
,	iReceiveIndex(0)
,	iSendIndex(0)
	{
	}

/**
* Second phase construction
*/
void CT_CBluetoothSocketData::ConstructL()
	{
	iData=new (ELeave) CT_BluetoothSocketNotifier();
	iData->SetServer(this);
	}

CT_CBluetoothSocketData::~CT_CBluetoothSocketData()
	{
	}

//	Service methods
TAny* CT_CBluetoothSocketData::GetObject()
	{
	return iData;
	}

void CT_CBluetoothSocketData::SetObjectL(TAny* aObject)
	{
	//called when loading up persistent data
	delete iData;
	iData = static_cast<CT_BluetoothSocketNotifier*>(aObject);
	iData->SetServer(this);
	}

void CT_CBluetoothSocketData::DisownObjectL()
 	{
	if ( iData )
		{
		iData->SetServer(NULL);
 		iData=NULL;
		}
 	}

inline TCleanupOperation CT_CBluetoothSocketData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_CBluetoothSocketData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	CT_BluetoothSocketNotifier* arg=static_cast<CT_BluetoothSocketNotifier*>(aAny);
	delete arg;
	}

TBool CT_CBluetoothSocketData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	retVal=ETrue;

	//-------------------------------------------------------------------------
	// CBluetoothSocket
	//-------------------------------------------------------------------------
	if (aCommand==KCmdRSocketServerConnect)
		{
		DoCmdCloseSocketServer();
		User::LeaveIfError(iData->iSocketServer.Connect());
		iData->iSocketServerConnected=ETrue;
		}
	else if (aCommand==KCmdRSocketServerClose)
		{
		DoCmdCloseSocketServer();
		}
	else if (aCommand==KCmdRConnectionStart)
		{
		TInt err = iData->iConnection.Open(iData->iSocketServer, KBTAddrFamily);

		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("RConnection::Open() err=%d" ),err);
			SetError(err);
			}

		err = iData->iConnection.Start();

		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("RConnection::Start() err=%d" ),err);
			SetError(219);
			}
		}
	else if (aCommand==KCmdRConnectionClose)
		{
		iData->iConnection.Close();
		}
	else if (aCommand==KCmdDestructor)
		{
		delete iData->iBluetoothSocket;
		iData->iBluetoothSocket=NULL;
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection,EFalse);
		}
	else if ( aCommand==KCmdNewLC )
		{
		DoCmdNewL(aSection,ETrue);
		}
	else if ( aCommand==KCmdBind)
		{
		DoCmdBind(aSection);
		}
	else if ( aCommand==KCmdListen)
		{
		DoCmdListen(aSection);
		}
	else if ( aCommand==KCmdLocalName)
		{
		DoCmdLocalName(aSection);
		}
	else if ( aCommand==KCmdLocalPort)
		{
		DoCmdLocalPort(aSection);
		}
	else if ( aCommand==KCmdSetLocalPort)
		{
		DoCmdSetLocalPort(aSection);
		}
	else if ( aCommand==KCmdSetOpt)
		{
		DoCmdSetOpt(aSection);
		}
	else if ( aCommand==KCmdSetOption)
		{
		DoCmdSetOption(aSection);
		}
	else if ( aCommand==KCmdGetOpt)
		{
		DoCmdGetOpt(aSection);
		}
	else if ( aCommand==KCmdName)
		{
		DoCmdName();
		}
	else if ( aCommand==KCmdSetTransferAble)
		{
		DoCmdSetTransferAble();
		}
	else if ( aCommand==KCmdTransfer)
		{
		DoCmdTransfer(aSection);
		}
	else if ( aCommand==KCmdInfo)
		{
		DoCmdInfo(aSection);
		}
	else if ( aCommand==KCmdAccept)
		{
		DoCmdAccept(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelAccept)
		{
		DoCmdCancelAccept();
		}
	else if ( aCommand==KCmdConnect)
		{
		DoCmdConnect(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSend)
		{
		DoCmdSend(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecv)
		{
		DoCmdRecv(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecvOneOrMore)
		{
		DoCmdRecvOneOrMore(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdShutdown)
		{
		DoCmdShutdown(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetNotifier)
		{
		DoCmdSetNotifierL(aSection);
		}
	else if ( aCommand==KCmdCancelConnect)
		{
		DoCmdCancelConnect();
		}
	else if ( aCommand==KCmdSetRsocketServerFromCBTSocket)
		{
		DoCmdSetRsocketServerFromCBTSocket(aSection);
		}
	else if ( aCommand==KCmdIoctl)
		{
		DoCmdIoctl(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelIoctl)
		{
		DoCmdCancelIoctl();
		}
	else if ( aCommand==KCmdAllowLowPowerModes)
		{
		DoCmdAllowLowPowerModes(aSection);
		}
	else if ( aCommand==KCmdCancelLowPowerModeRequester)
		{
		DoCmdCancelLowPowerModeRequester(aSection);
		}
	else if ( aCommand==KCmdPreventLowPowerModes)
		{
		DoCmdPreventLowPowerModes(aSection);
		}
	else if ( aCommand==KCmdActivateBasebandEventNotifier)
		{
		DoCmdActivateBasebandEventNotifier(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelBasebandEventNotifier)
		{
		DoCmdCancelBasebandEventNotifier();
		}
	else if ( aCommand==KCmdPhysicalLinkState)
		{
		DoCmdPhysicalLinkState(aSection);
		}
	else if ( aCommand==KCmdPreventRoleSwitch)
		{
		DoCmdPreventRoleSwitch();
		}
	else if ( aCommand==KCmdAllowRoleSwitch)
		{
		DoCmdAllowRoleSwitch();
		}
	else if ( aCommand==KCmdRequestMasterRole)
		{
		DoCmdRequestMasterRole();
		}
	else if ( aCommand==KCmdRequestSlaveRole)
		{
		DoCmdRequestSlaveRole();
		}
	else if ( aCommand==KCmdActivateSniffRequester)
		{
		DoCmdActivateSniffRequester();
		}
	else if ( aCommand==KCmdActivateParkRequester)
		{
		DoCmdActivateParkRequester();
		}
	else if ( aCommand==KCmdCancelSend)
		{
		DoCmdCancelSend();
		}
	else if ( aCommand==KCmdCancelAll)
		{
		DoCmdCancelAll();
		}
	else if ( aCommand==KCmdWrite)
		{
		DoCmdWrite(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelWrite)
		{
		DoCmdCancelWrite();
		}
	else if ( aCommand==KCmdSendTo)
		{
		DoCmdSendTo(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdRecvFrom)
		{
		DoCmdRecvFrom(aSection,aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdGetDisconnectData)
		{
		DoCmdGetDisconnectData();
		}
	else if ( aCommand==KCmdRequestChangeSupportedPacketTypes)
		{
		DoCmdRequestChangeSupportedPacketTypes(aSection);
		}
	else if ( aCommand==KCmdCancelRecv)
		{
		DoCmdCancelRecv();
		}
	else if ( aCommand==KCmdRead)
		{
		DoCmdRead(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelRead)
		{
		DoCmdCancelRead();
		}
	else if ( aCommand==KCmdRemoteName)
		{
		DoCmdRemoteName(aSection);
		}
	else if ( aCommand==KCmdTestMBSN_ExtensionInterfaceL)
		{
		DoCmdTestMBSN_ExtensionInterfaceL(aSection);
		}
	else if ( aCommand==KCmdSetAutomaticSniffMode)
		{
		DoCmdSetAutomaticSniffMode(aSection);
		}
	else
		{
		retVal=EFalse;
		}
	return retVal;
	}


inline void CT_CBluetoothSocketData::DoCmdAccept(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iAcceptFlag = EFalse;
	iHasAcceptDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasAcceptDataToExpect )
		{
		iAcceptDataToExpect.Copy(expectedReply);
		}

	TBool	useAccept2 =EFalse;
	GetBoolFromConfig(aSection, KUseAccept2(), useAccept2);

	// Get the blank session socket
	TPtrC	bluetoothSocketName;
	CBluetoothSocket*	bluetoothSocket=NULL;
	if( GetStringFromConfig(aSection, KCBluetoothSocket(), bluetoothSocketName))
		{
		bluetoothSocket=(static_cast<CT_BluetoothSocketNotifier*>(GetDataObjectL(bluetoothSocketName))->iBluetoothSocket);
		if ( bluetoothSocket!=NULL )
			{
			TInt	status;
			if ( useAccept2 )
				{
				INFO_PRINTF1(_L("Using Accept 2"));
				status = iData->iBluetoothSocket->Accept(*bluetoothSocket, iDataToRead8);
				}
			else
				{
				INFO_PRINTF1(_L("Using Accept 1"));
				status = iData->iBluetoothSocket->Accept(*bluetoothSocket);
				}

			if(status==KErrNone)
				{
				IncOutstanding();
				iAcceptFlag = ETrue;
				iAcceptIndex = aAsyncErrorIndex;
				}
			else
				{
				ERR_PRINTF2(_L("Accept failed: %d"),status);
				SetError(status);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Bind TSockAddr is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdActivateBasebandEventNotifier(const TDesC& aSection,const TInt aAsyncErrorIndex)
	{
	// loop through all values in the ini file and add them up
	TUint32	flags=0;
	CT_BTUtil::GetLinkStateNotifierBits(*this, aSection, flags);

	TInt	errCode = iData->iBluetoothSocket->ActivateBasebandEventNotifier(flags);
	if ( errCode==KErrNone )
		{
		IncOutstanding();
		iNotifierIndex = aAsyncErrorIndex;
		}
	else
		{
		ERR_PRINTF2(_L("ActivateBasebandEventNotifier failed with error code: %d"), errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdActivateParkRequester()
	{
	TInt errCode = iData->iBluetoothSocket->ActivateParkRequester();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("ActivateParkRequester failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdActivateSniffRequester()
	{
	TInt errCode = iData->iBluetoothSocket->ActivateSniffRequester();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("ActivateSniffRequester failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdAllowLowPowerModes(const TDesC& aSection)
	{
	TUint32	mode=0;
	CT_BTUtil::GetLowPowerMode(*this, aSection, mode);

	TInt	errCode=iData->iBluetoothSocket->AllowLowPowerModes(mode);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("Send AllowLowPowerModes failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdAllowRoleSwitch()
	{
	TInt	errCode = iData->iBluetoothSocket->AllowRoleSwitch();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("AllowRoleSwitch failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdBind(const TDesC& aSection)
	{
	TPtrC	sockAddrName;
	if( GetStringFromConfig(aSection, KTSockAddr(), sockAddrName))
		{
		INFO_PRINTF2(_L("Binding with: %S TSockAddr"), &sockAddrName);
		TBTSockAddr*	sockAddr=static_cast<TBTSockAddr*>(GetDataObjectL(sockAddrName));
		if ( sockAddr!=NULL )
			{
			TInt	errCode = iData->iBluetoothSocket->Bind(*sockAddr);
			if(errCode!=KErrNone)
				{
				ERR_PRINTF2(_L("Bind failed: %d"), errCode);
				SetError(errCode);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Bind TSockAddr is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelAccept()
	{
	iData->iBluetoothSocket->CancelAccept();
	if(Outstanding() && iAcceptFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelAll()
	{
	iData->iBluetoothSocket->CancelAll();
	}

inline void CT_CBluetoothSocketData::DoCmdCancelBasebandEventNotifier()
	{
	iData->iBluetoothSocket->CancelBasebandEventNotifier();
	if(Outstanding() && iNotifierFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelConnect()
	{
	iData->iBluetoothSocket->CancelConnect();
	if(Outstanding() && iConnectFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelIoctl()
	{
	iData->iBluetoothSocket->CancelIoctl();
	if(Outstanding() && iIoctlFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelLowPowerModeRequester(const TDesC& /*aSection*/)
	{
	TInt	errCode=iData->iBluetoothSocket->CancelLowPowerModeRequester();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("Send CancelLowPowerModeRequester failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelRead()
	{
	iData->iBluetoothSocket->CancelRead();
	if (Outstanding() && iReadFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelRecv()
	{
	iData->iBluetoothSocket->CancelRecv();
	if (Outstanding() && iRecvFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelSend()
	{
	iData->iBluetoothSocket->CancelSend();
	if (Outstanding() && iSendFlag)
		{
		DecOutstanding();
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCancelWrite()
	{
	iData->iBluetoothSocket->CancelWrite();
	if (Outstanding() && iWriteFlag)
		{
		DecOutstanding();	
		}
	}

inline void CT_CBluetoothSocketData::DoCmdCloseSocketServer()
	{
	if(iData->iSocketServerConnected)
		{
		iData->iSocketServer.Close();
		}
	iData->iSocketServerConnected=EFalse;
	}

inline void CT_CBluetoothSocketData::DoCmdConnect(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iConnectFlag = EFalse;
	iHasConnectDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasConnectDataToExpect )
		{
		iConnectDataToExpect.Copy(expectedReply);
		}

	TPtrC	sockAddrName;
	if( GetStringFromConfig(aSection, KTBTSockAddr(), sockAddrName))
		{
		INFO_PRINTF2(_L("Connecting with: %S TBTSockAddr"), &sockAddrName);
		TBTSockAddr*	myTBTSockAddr=static_cast<TBTSockAddr*>(GetDataObjectL(sockAddrName));
		if ( myTBTSockAddr!=NULL )
			{
			TInt	errCode=KErrNone;

			TInt	serviceBits;
			if ( GetIntFromConfig(aSection, KServiceBits(), serviceBits) )
				{
				INFO_PRINTF1(_L("Using Connect 3"));

				TBTDevAddr	tempAddress = myTBTSockAddr->BTAddr();
				TBuf<KMaxSockAddrSize> tmpBuf;
				tempAddress.GetReadable(tmpBuf);
				INFO_PRINTF2(_L("Connecting to address (%S)"), &tmpBuf);

				errCode=iData->iBluetoothSocket->Connect(*myTBTSockAddr, serviceBits);
				}
			else
				{
				TPtrC	connectData;
				if ( GetStringFromConfig(aSection, KConnectData(), connectData) )
					{
					INFO_PRINTF1(_L("Using Connect 2"));
					TBuf8<DATASIZE>	connectData8(_L8(""));
					connectData8.Copy(connectData);

					TBTDevAddr	tempAddress = myTBTSockAddr->BTAddr();
					TBuf<KMaxSockAddrSize> tmpBuf;
					tempAddress.GetReadable(tmpBuf);
					INFO_PRINTF2(_L("Connecting to address (%S)"), &tmpBuf);

					errCode =iData->iBluetoothSocket->Connect(*myTBTSockAddr, iDataToRead8, connectData8);
					}
				else
					{
					INFO_PRINTF1(_L("Using Connect 1"));

					TBTDevAddr	tempAddress = myTBTSockAddr->BTAddr();
					TBuf<KMaxSockAddrSize> tmpBuf;
					tempAddress.GetReadable(tmpBuf);
					INFO_PRINTF2(_L("Connecting to address (%S)"), &tmpBuf);

					errCode =iData->iBluetoothSocket->Connect(*myTBTSockAddr);
					}
				}
			if ( errCode == KErrNone)
				{
        		IncOutstanding();
        		iConnectFlag = ETrue;
		        iConnectIndex = aAsyncErrorIndex;
				}
			else
				{
				ERR_PRINTF2(_L("Connect failed: %d"), errCode);
				SetError(errCode);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Connect TBTSockAddr is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdGetDisconnectData()
	{
	TInt	errCode = iData->iBluetoothSocket->GetDisconnectData(iDisconnectDes8);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("GetDisconnectData failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdGetOpt(const TDesC& aSection)
	{
	TBool								found;

	TBool	equals = ETrue;
	GetBoolFromConfig(aSection, KEquals(), equals);
	if (equals)
		{
		INFO_PRINTF1(_L("The comparison is expected to be equal."));
		}
	else
		{
		INFO_PRINTF1(_L("The comparison is not expected to be equal."));
		}

	TBool	dataOk=ETrue;
	TPtrC	optionName;
	TInt	optionNameValue=0;
	if ( GetStringFromConfig(aSection, KOptionName(), optionName) )
		{
		dataOk=CT_BTUtil::GetIntValue(optionName, optionNameValue);

		//Report error but continue as it may still make sense
		if (dataOk)
			{
			INFO_PRINTF3(_L("The name of the option to set: %S , its value: %d" ), &optionName, optionNameValue);
			}
		else
			{
			ERR_PRINTF2(_L("Option Level not found in lookup: %S"),&optionName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(_L("Data %S missing"), &KOptionName());
		SetBlockResult(EFail);
		}

	TPtrC	optionLevel;
	TInt	optionLevelValue;
	if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel))
		{
		found=CT_BTUtil::GetIntValue(optionLevel, optionLevelValue);
		//Report error but continue as it may still make sense
		if (found)
			{
			INFO_PRINTF3(_L("The name of the option level to set: %S , its value: %d" ),&optionLevel, optionLevelValue);
			}
		else
			{
			dataOk=EFalse;
			ERR_PRINTF2(_L("Option Level not found in lookup: %S"),&optionLevel);
			SetBlockResult(EFail);
			}
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(_L("Data %S missing"), &KOptionLevel());
		SetBlockResult(EFail);
		}

	if ( dataOk )
		{
		TBool	getDes=EFalse;
		GetBoolFromConfig(aSection, KGetDes(), getDes);

		if ( getDes )
			{
			TBuf8<DATASIZE>	tempData8(_L8(""));
			TInt err = iData->iBluetoothSocket->GetOpt(optionNameValue, optionLevelValue, tempData8);
			if( err != KErrNone )
				{
				INFO_PRINTF2(_L("GetOpt() return error %d" ), err);
				SetError(err);
				}

			TBuf<DATASIZE>	tempData;
			tempData.Copy(tempData8);
			INFO_PRINTF3(_L("The value of the option retrived as des: %S, descriptor length: %d" ), &tempData, tempData.Length());

			TPtrC	desOption; // descriptor to compare with if getdes is TRUE
			if( GetStringFromConfig(aSection, KDesOption(), desOption))
				{
				INFO_PRINTF2(_L("The expected descriptor value: %S" ), &desOption);
				if ( (desOption==tempData) == equals )
					{
					INFO_PRINTF1(_L("Option is as expected"));
					}
				else
					{
					ERR_PRINTF1(_L("Option is not as expected"));
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			TInt	val;
			TInt err = iData->iBluetoothSocket->GetOpt(optionNameValue, optionLevelValue, val);
			if( err != KErrNone )
				{
				INFO_PRINTF2(_L("GetOpt() return error %d" ), err);
				SetError(err);
				}

			INFO_PRINTF2(_L("The value of the option retrived: %d" ),val);

			TInt	intOption; // int value to compare with if getdes is FALSE or not defined
			if( GetIntFromConfig(aSection, KIntOption(), intOption))
				{
				INFO_PRINTF1(_L("Comparing results"));
				if ( (intOption==val) == equals )
					{
					INFO_PRINTF1(_L("Option is as expected"));
					}
				else
					{
					ERR_PRINTF1(_L("Option is not as expected"));
					SetBlockResult(EFail);
					}
				}
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdInfo(const TDesC& aSection)
	{
	TProtocolDesc	protocolDesc;
	TInt			err = iData->iBluetoothSocket->Info(protocolDesc);
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("iBluetoothSocket->Info error: %d"), err);
		SetError(err);
		}
		
	INFO_PRINTF2(_L("iBluetoothSocket->Info() = %S"), &protocolDesc.iName);
	
	TPtrC expectedValue;
	if( GetStringFromConfig(aSection, KExpectedReply(), expectedValue) )
		{
		if( expectedValue != protocolDesc.iName )
			{
			ERR_PRINTF3(_L("Expected Info (%S) != Actual Info (%S)"), &expectedValue, &protocolDesc.iName);
			SetBlockResult(EFail);
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdIoctl(const TDesC& aSection,const TInt aAsyncErrorIndex)
	{
	TBool	dataOk = ETrue;
	iIoctlFlag = EFalse;
	TBool	boolCallTwice = EFalse;
	GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

	TBool	useDeprecated=EFalse;
	GetBoolFromConfig(aSection, KUseDeprecated(), useDeprecated);

	TBool	found = EFalse;

	TPtrC	optionName;
	TInt 	optionNameValue=0;
	if( GetStringFromConfig(aSection, KOptionName(), optionName))
		{
		found=CT_BTUtil::GetIntValue(optionName, optionNameValue);
		//Report error but continue as it may still make sense
		if (found)
			{
			INFO_PRINTF3(_L("The name of the option to set: %S , its value: %d" ), &optionName, optionNameValue);
			}
		else
			{
			dataOk=EFalse;
			ERR_PRINTF2(_L("Option not found in lookup: %S"), &optionName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(_L("Missing: %S"), &KOptionName());
		SetBlockResult(EFail);
		}

	TPtrC	optionLevel;
	TInt	optionLevelValue;
	if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel))
		{
		found=CT_BTUtil::GetIntValue(optionLevel, optionLevelValue);
		//Report error but continue as it may still make sense
		if (found)
			{
			INFO_PRINTF3(_L("The name of the option level to set: %S , its value: %d" ), &optionLevel, optionLevelValue);
			}
		else
			{
			ERR_PRINTF2(_L("Option Level not found in lookup: %S"), &optionLevel);
			SetBlockResult(EFail);
			}
		}
	else
		{
		dataOk=EFalse;
		ERR_PRINTF2(_L("Missing: %S"), &KOptionLevel());
		SetBlockResult(EFail);
		}

	if ( dataOk )
		{
		TInt mtuOptionBufferSize = 0;
		GetIntFromConfig(aSection, KMTUSize(), mtuOptionBufferSize);

		if( mtuOptionBufferSize > DATASIZE )
			{
			mtuOptionBufferSize = DATASIZE;
			WARN_PRINTF3(_L("mtuOptionBufferSize %d is large, using %d"), mtuOptionBufferSize, DATASIZE);
			}

		iMTUOptionBuffer = TPckgBuf<TUint16>(mtuOptionBufferSize);

		TInt	errCode=KErrNone;
		if (useDeprecated)
			{
			errCode=iData->iBluetoothSocket->Ioctl(optionNameValue, &iMTUOptionBuffer, optionLevelValue);
			}
		else
			{
			errCode=iData->iBluetoothSocket->Ioctl(optionLevelValue, optionNameValue, &iMTUOptionBuffer);
			}

		if (errCode == KErrNone)
			{
       		IncOutstanding();
       		iIoctlFlag = ETrue;
	        iIoctlIndex = aAsyncErrorIndex;
			}
		else
			{
			ERR_PRINTF2(_L("Send Ioctl failed with error code: %d"), errCode);
			SetError(errCode);
			}

		if( boolCallTwice )
			{
			if (useDeprecated)
				{
				errCode=iData->iBluetoothSocket->Ioctl(optionNameValue, &iMTUOptionBuffer, optionLevelValue);
				}
			else
				{
				errCode=iData->iBluetoothSocket->Ioctl(optionLevelValue, optionNameValue, &iMTUOptionBuffer);
				}
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("Send Ioctl failed with error code: %d"),errCode);
				SetError(errCode);
				}	
			else
				{
           		IncOutstanding();
           		iIoctlFlag = ETrue;
           		//NB! May be not necessary, because the second call must call panics
	            iIoctlIndex = aAsyncErrorIndex;
				}
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdListen(const TDesC& aSection)
	{

	TInt	queueSize;
	if ( GetIntFromConfig(aSection, KQueueSize(), queueSize) )
		{
		TInt	errCode=KErrNone;

		TPtrC	connectData;
		if ( GetStringFromConfig(aSection, KConnectData(), connectData) )
			{
			iConnectData8 = _L8("");
			iConnectData8.Copy(connectData);
			errCode=iData->iBluetoothSocket->Listen(queueSize, iConnectData8);
			}
		else
			{
			TInt serviceBits;
			if ( GetIntFromConfig(aSection, KServiceBits(), serviceBits) )
				{
				INFO_PRINTF2(_L("Using Listen 3 Service bits is: %d"),serviceBits);
				errCode=iData->iBluetoothSocket->Listen(queueSize, serviceBits);
				}
			else
				{
				INFO_PRINTF1(_L("Using Listen 1"));
				errCode=iData->iBluetoothSocket->Listen(queueSize);
				}
			}
		if ( errCode!=KErrNone )
			{
			ERR_PRINTF2(_L("Listen failed: %d"),errCode);
			SetError(errCode);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing: %Sfailed"), &KServiceBits());
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdLocalName(const TDesC& aSection)
	{

	T_TBTSockAddrChild	addr;
	iData->iBluetoothSocket->LocalName(addr);

	TBTDevAddr				currentAddr = addr.BTAddr();
	TBuf<KMaxSockAddrSize>	tmpBuf;
	currentAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("LocalName() = 0x%S"), &tmpBuf);
	TPtrC	myTBTDevAddrName;
	if ( GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName) )
		{
		CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));
		TBTDevAddr btDevAddr = *myTBTDevAddrData->GetAddress();
		TBuf<KMaxSockAddrSize> tmpBuf2;
		btDevAddr.GetReadable(tmpBuf2);
		if (tmpBuf2 != tmpBuf)
			{
			ERR_PRINTF3(_L("Expected address (%S) != actual address (%S)"),&tmpBuf2,&tmpBuf);
			SetBlockResult(EFail);
			}
		}
	else
		{
		TInt	lhs;
		TInt	rhs;
		TBool	lhsGiven = EFalse;
		TBool	rhsGiven = EFalse;
		lhsGiven = GetHexFromConfig(aSection, KHexBTAddrL(), lhs );
		rhsGiven = GetHexFromConfig(aSection, KHexBTAddrR(), rhs );

		if (lhsGiven && rhsGiven)
			{
			TBTDevAddr				btDevAddr (MAKE_TINT64(lhs, rhs));
			TBuf<KMaxSockAddrSize>	tmpBuf2;
			btDevAddr.GetReadable(tmpBuf2);
			if (tmpBuf2 != tmpBuf)
				{
				ERR_PRINTF3(_L("Expected address (%S) != actual address (%S)"),&tmpBuf2,&tmpBuf);
				SetBlockResult(EFail);
				}
			}
		}

	// Set in TBTSockAddr Data object so that it can be verified
	TPtrC	myTSockAddrName;
	CT_BTSockAddrData* myTSockAddrData=NULL;
	if( GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName))
		{
		myTSockAddrData=static_cast<CT_BTSockAddrData*>(GetDataWrapperL(myTSockAddrName));
		if ( myTSockAddrData!=NULL )
			{
			myTSockAddrData->SetObject(&addr,EFalse);
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdLocalPort(const TDesC& aSection)
	{

	TInt	actualPort = iData->iBluetoothSocket->LocalPort();
	INFO_PRINTF2(_L("Actual port is: %d"), actualPort);

	TInt	port;
	if ( GetIntFromConfig(aSection, KPort(), port) )
		{
		INFO_PRINTF2(_L("Expected port is: %d"), port);
		if ( actualPort!=port )
			{
			ERR_PRINTF1(_L("LocalPort is not as expected"));
			SetBlockResult(EFail);
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdName()
	{
	TName	theName;
	TInt	err=iData->iBluetoothSocket->Name(theName);
	if ( err==KErrNone )
		{
		// good print the name
		INFO_PRINTF2(_L("The socket name is: (%S)" ), &theName);
		}
	else
		{
		ERR_PRINTF2(_L("Failed to get the Name of the Bluetoothsocket, error code %d returned"),err);
		SetError(err);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdNewL(const TDesC& aSection, TBool aUseLC)
	{

	TInt constructorType = 0;
	GetIntFromConfig(aSection, KConstructorType(), constructorType);

	if ((constructorType >0 )&&(constructorType < 6))
		{
		TBool								dataOk=ETrue;
		TBool								found = EFalse;

		delete iData->iBluetoothSocket;
		iData->iBluetoothSocket = NULL;		
		
		if( constructorType == 1 )
			{
			INFO_PRINTF1(_L("CBluetoothSocket Standard Constructor Call"));

			TInt 	intSockType=0;
			TPtrC	sockType;
			if( GetStringFromConfig(aSection, KSockType(), sockType) )
				{
				found=CT_BTUtil::GetIntValue(sockType, intSockType);
				if ( !found )
					{
					dataOk=EFalse;
					ERR_PRINTF2(_L("Socket type not found in lookup: %S"),&sockType);
					SetBlockResult(EFail);
					}
				}
			else
				{
				dataOk=EFalse;
				ERR_PRINTF2(_L("Missing :%S"), &KSockType());
				SetBlockResult(EFail);
				}

			TInt 	intProtocol;
			TPtrC	protocol;
			if ( GetStringFromConfig(aSection, KProtocol(), protocol) )
				{
				found=CT_BTUtil::GetIntValue(protocol, intProtocol);
				if ( !found )
					{
					dataOk=EFalse;
					ERR_PRINTF2(_L("Socket type not found in lookup: %S"),&sockType);
					SetBlockResult(EFail);
					}
				}
			else
				{
				dataOk=EFalse;
				ERR_PRINTF2(_L("Missing :%S"), &KProtocol());
				SetBlockResult(EFail);
				}

			if ( dataOk )
				{
				if(aUseLC)
					{
					INFO_PRINTF1(_L("CBluetoothSocket Standard Constructor NewLC" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewLC(*iData, iData->iSocketServer, intSockType, intProtocol); CleanupStack::Pop(););

					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket Standard Constructor NewLC err=%d" ),err);
						SetError(err);
						}
					}
				else	
					{
					INFO_PRINTF1(_L("CBluetoothSocket Standard Constructor NewL" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewL(*iData, iData->iSocketServer, intSockType, intProtocol));

					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket Standard Constructor NewL err=%d" ),err);
						SetError(err);
						}					
					}	
				}
			}
		else if( constructorType == 2 )
			{
			INFO_PRINTF1(_L("CBluetoothSocket RConnection Constructor Call"));

			TInt	theSockType=0;
			TPtrC	sockType;
			if ( GetStringFromConfig(aSection, KSockType(), sockType) )
				{
				found=CT_BTUtil::GetIntValue(sockType, theSockType);
				if ( !found )
					{
					dataOk=EFalse;
					ERR_PRINTF2(_L("Socket type not found in lookup: %S"), &sockType);
					SetBlockResult(EFail);
					}
				}
			else
				{
				dataOk=EFalse;
				ERR_PRINTF2(_L("Missing :%S"), &KSockType());
				SetBlockResult(EFail);
				}

			TInt	theProtocol;
			TPtrC	protocol;
			if ( GetStringFromConfig(aSection, KProtocol(), protocol) )
				{
				found=CT_BTUtil::GetIntValue(protocol, theProtocol);
				if (!found)
					{
					dataOk=EFalse;
					ERR_PRINTF2(_L("Protocol not found in lookup: %S"), &protocol);
					SetBlockResult(EFail);
					}
				}
			else
				{
				dataOk=EFalse;
				ERR_PRINTF2(_L("Missing :%S"), &KProtocol());
				SetBlockResult(EFail);
				}

			if ( dataOk )
				{
				if ( aUseLC )
					{
					INFO_PRINTF1(_L("CBluetoothSocket RConnection Constructor NewLC" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewLC(*iData, iData->iSocketServer, theSockType, theProtocol, iData->iConnection); CleanupStack::Pop(););
					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket RConnection Constructor NewLC err=%d" ),err);
						SetError(err);
						}
					}				
				else
					{
					INFO_PRINTF1(_L("CBluetoothSocket RConnection Constructor NewL" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewL(*iData, iData->iSocketServer, theSockType, theProtocol,iData->iConnection));

					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket RConnection Constructor NewL err=%d" ),err);
						SetError(err);
						}					
					}
				}
			}
		else if( constructorType == 3 )
			{
			INFO_PRINTF1(_L("CBluetoothSocket Named Protocol Constructor Call"));

			TPtrC	namedProtocol;
			if( GetStringFromConfig(aSection, KNamedProtocol(), namedProtocol))
				{
				if (aUseLC)
					{
					INFO_PRINTF1(_L("CBluetoothSocket Named Protocol Constructor NewLC" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewLC(*iData, iData->iSocketServer, namedProtocol); CleanupStack::Pop(););

					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket Named Protocol Constructor NewLC err=%d" ),err);
						SetError(err);
						}
					}
				
				else
					{
					INFO_PRINTF1(_L("CBluetoothSocket Named Protocol Constructor NewL" ));
					TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewL(*iData, iData->iSocketServer, namedProtocol));

					if( err!=KErrNone )
						{
						INFO_PRINTF2(_L("CBluetoothSocket Named Protocol Constructor NewL err=%d" ),err);
						SetError(err);
						}				
					}		
				}
			else
				{
				ERR_PRINTF2(_L("Missing :%s"), &KNamedProtocol());
				SetBlockResult(EFail);
				}
			}
		else if( constructorType == 4 )
			{
			if (aUseLC)
				{
				INFO_PRINTF1(_L("CBluetoothSocket Blank Constructor NewLC"));
				TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewLC(*iData, iData->iSocketServer); CleanupStack::Pop(););
		
				if( err!=KErrNone )
					{
					INFO_PRINTF2(_L("CBluetoothSocket Blank Constructor NewLC err=%d" ),err);
					SetError(err);
					}				
				}
			else
				{
				INFO_PRINTF1(_L("CBluetoothSocket Blank Constructor NewL"));
				TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewL(*iData, iData->iSocketServer));

				if( err!=KErrNone )
					{
					INFO_PRINTF2(_L("CBluetoothSocket Blank Constructor NewL err=%d" ),err);
					SetError(err);
					}				
				}
			}
		else if( constructorType == 5 )
			{
			INFO_PRINTF1(_L("CBluetoothSocket RSocket Constructor Call"));

			TPtrC		myRSocketName;
			if( GetStringFromConfig(aSection, KRSocket(), myRSocketName))
				{
				RSocket*	myRSocket=static_cast<RSocket*>(GetDataObjectL(myRSocketName));
				if ( myRSocket!=NULL )
					{
					if (aUseLC)
						{
						INFO_PRINTF1(_L("CBluetoothSocket RSocket Constructor NewLC" ));
						TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewLC(*iData, iData->iSocketServer, *myRSocket); CleanupStack::Pop(););

						if( err!=KErrNone )
							{
							INFO_PRINTF2(_L("CBluetoothSocket RSocket Constructor NewLC err=%d" ),err);
							SetError(err);
							}
						}		
					else
						{
						INFO_PRINTF1(_L("CBluetoothSocket RSocket Constructor NewL" ));
						TRAPD(err, iData->iBluetoothSocket = CBluetoothSocket::NewL(*iData, iData->iSocketServer, *myRSocket));

						if( err!=KErrNone )
							{
							INFO_PRINTF2(_L("CBluetoothSocket RSocket Constructor NewL err=%d" ),err);
							SetError(err);
							}						
						}
					}
				else
					{
					ERR_PRINTF2(_L("Failed to fetch RSocket named %S"),&myRSocketName);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("Missing :%S"), &KRSocket());
				SetBlockResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Incorrect constructorType = %d" ),constructorType);
			SetBlockResult(EFail);
			}		
		}
	else
		{
		ERR_PRINTF2(_L("Ilegal value for constructor type value in range 1-5 expected found: %d"),constructorType);
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdPhysicalLinkState(const TDesC& aSection)
	{

	TUint32	tmpstate;
	TInt errCode = iData->iBluetoothSocket->PhysicalLinkState(tmpstate);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("PhysicalLinkState failed with error code: %d"),errCode);
		SetError(errCode);
		}
		
	TInt state = static_cast<TInt>(tmpstate);	
	//Process the state we can do a nice report here as we know what the bits are
	// See CT_BTUtil as a starting point
	INFO_PRINTF2(_L("PhysicalLinkState: %d" ), state );
	
	TInt expectedState;
	if( GetIntFromConfig(aSection, KState(), expectedState) )
		{
		if( expectedState != state )
			{
			ERR_PRINTF3(_L("Expected PhysicalLinkState (%d) != Actual PhysicalLinkState (%d)"), state, expectedState);
			SetBlockResult(EFail);
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdPreventLowPowerModes(const TDesC& aSection)
	{
	TUint32	mode =0;
	CT_BTUtil::GetLowPowerMode(*this, aSection, mode);

	TInt	errCode=iData->iBluetoothSocket->PreventLowPowerModes(mode);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("Send PreventLowPowerModes failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdPreventRoleSwitch()
	{
	TInt errCode = iData->iBluetoothSocket->PreventRoleSwitch();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("PreventRoleSwitch failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRead(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iReadFlag = EFalse;
	iHasReceiveDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasReceiveDataToExpect )
		{
		iReceiveDataToExpect.Copy(expectedReply);
		}

	TBool	boolCallTwice = EFalse;
	GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

	TInt	errCode = iData->iBluetoothSocket->Read(iDataToRead8);
	if (errCode == KErrNone)
		{
		IncOutstanding();
		iReadFlag = ETrue;
		iReceiveIndex = aAsyncErrorIndex;
		}
	else
		{
		ERR_PRINTF2(_L("Read failed with error code: %d"),errCode);
		SetError(errCode);
		}

	if( boolCallTwice )
		{
		errCode = iData->iBluetoothSocket->Read(iDataToRead8);
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("Read failed with error code: %d"),errCode);
			SetError(errCode);	
			}
		else
			{
    		IncOutstanding();
    		iReadFlag = ETrue;
	    	iReceiveIndex = aAsyncErrorIndex;
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRecv(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iRecvFlag = EFalse;
	iHasReceiveDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasReceiveDataToExpect )
		{
		iReceiveDataToExpect.Copy(expectedReply);
		}

	TInt	flags=0;
	GetIntFromConfig(aSection, KFlags(), flags);
	TBool	useRecv2 = EFalse;
	GetBoolFromConfig(aSection, KUseRecv2(), useRecv2);

	TBool	boolCallTwice = EFalse;
	GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

	if (useRecv2)
		{
		INFO_PRINTF1(_L("Using Recv 2"));
		INFO_PRINTF2(_L("Flags: %d"),flags);
		TSockXfrLength	actuallengthSent;
		TInt errCode = iData->iBluetoothSocket->Recv(iDataToRead8, flags, actuallengthSent);
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("Recv failed: %d"),errCode);
			SetError(errCode);
			}
		else
			{
       		IncOutstanding();
       		iRecvFlag = ETrue;
	        iReceiveIndex = aAsyncErrorIndex;
			}

		if( boolCallTwice )
			{
			errCode = iData->iBluetoothSocket->Recv(iDataToRead8, flags, actuallengthSent);
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("Recv failed: %d"),errCode);
				SetError(errCode);
				}
			else
				{
        		IncOutstanding();
        		iRecvFlag = ETrue;
		        iReceiveIndex = aAsyncErrorIndex;
				}
			}
		TInt& lenSentInt=actuallengthSent();

		INFO_PRINTF2(_L("Returned actual length received: %d"), lenSentInt);
		if ( lenSentInt!=iDataToRead8.Length() )
			{
			ERR_PRINTF3(_L("Returned length received: %d != Actual length received: %d"), lenSentInt, iDataToRead8.Length());
			SetBlockResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Using Recv 1"));
		INFO_PRINTF2(_L("Flags: %d"),flags);
		TInt errCode = iData->iBluetoothSocket->Recv(iDataToRead8, flags);
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("Recv failed: %d"),errCode);
			SetError(errCode);
			}
		else
			{
       		IncOutstanding();
       		iRecvFlag = ETrue;
	        iReceiveIndex = aAsyncErrorIndex;
			}
		if( boolCallTwice )
			{
			errCode = iData->iBluetoothSocket->Recv(iDataToRead8, flags);
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("Recv failed: %d"),errCode);
				SetError(errCode);
				}
			else
				{
        		IncOutstanding();
        		iRecvFlag = ETrue;
		        iReceiveIndex = aAsyncErrorIndex;
				}
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRecvFrom(const TDesC& aSection,const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iHasReceiveDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasReceiveDataToExpect )
		{
		iReceiveDataToExpect.Copy(expectedReply);
		}

	TBool	useRecv2 = EFalse;
	GetBoolFromConfig(aSection, KUseRecv2(), useRecv2);

	TInt		flags = 0; // read in, if not found send 0
	GetIntFromConfig(aSection, KFlags(), flags);
	TPtrC	myTSockAddrName;
	TBTSockAddr* myTSockAddr=NULL;
	if( GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName))
		{
		INFO_PRINTF2(_L("Binding with: %S TSockAddr"), &myTSockAddrName);
		myTSockAddr=static_cast<TBTSockAddr*>(GetDataObjectL(myTSockAddrName));
		if ( myTSockAddr!=NULL )
			{
			if (!useRecv2)
				{
				TInt errCode = iData->iBluetoothSocket->RecvFrom(iDataToRead8, *myTSockAddr, flags);
				if (errCode != KErrNone)
					{
					ERR_PRINTF2(_L("RecvFrom failed with error code: %d"),errCode);
					SetError(errCode);
					}
				else
					{
            		IncOutstanding();
            	    iReceiveIndex = aAsyncErrorIndex;
					}
				}
			else
				{
				TSockXfrLength lenSent;
				TInt errCode = iData->iBluetoothSocket->RecvFrom(iDataToRead8, *myTSockAddr, flags, lenSent);
				if (errCode != KErrNone)
					{
					ERR_PRINTF2(_L("RecvFrom failed with error code: %d"),errCode);
					SetError(errCode);
					}
				else
					{
            		IncOutstanding();
            		iReceiveIndex = aAsyncErrorIndex;
					}
				TInt& lenSentInt=lenSent();

				INFO_PRINTF2(_L("Returned actual length received: %d"), lenSentInt);
				if ( lenSentInt!=iDataToRead8.Length() )
					{
					ERR_PRINTF3(_L("Returned length received: %d != Actual length received: %d"), lenSentInt, iDataToRead8.Length());
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF1(_L("RecvFrom TBTSockAddr is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRecvOneOrMore(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	expectedReply;
	iHasReceiveDataToExpect=GetStringFromConfig(aSection, KExpectedReply(), expectedReply);
	if ( iHasReceiveDataToExpect )
		{
		iReceiveDataToExpect.Copy(expectedReply);
		}

	TInt	flags=0;
	GetIntFromConfig(aSection, KFlags(), flags);

	TBool	boolCallTwice = EFalse;
	GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

	TSockXfrLength	actuallengthSent;
	TInt errCode = iData->iBluetoothSocket->RecvOneOrMore(iDataToRead8, flags, actuallengthSent);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("RecvOneOrMore failed: %d"),errCode);
		SetError(errCode);
		}
	else
		{
   		IncOutstanding();
   		iReceiveIndex = aAsyncErrorIndex;
		}

	if( boolCallTwice )
		{
		errCode = iData->iBluetoothSocket->RecvOneOrMore(iDataToRead8, flags, actuallengthSent);
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("RecvOneOrMore failed: %d"),errCode);
			SetError(errCode);
			}
		else
			{
       		IncOutstanding();
       	    iReceiveIndex = aAsyncErrorIndex;
			}
		}
	TInt& lenSentInt=actuallengthSent();
	INFO_PRINTF2(_L("Returned actual length received: %d"), lenSentInt);
	if ( lenSentInt!=iDataToRead8.Length() )
		{
		ERR_PRINTF3(_L("Returned length received: %d != Actual length received: %d"), lenSentInt, iDataToRead8.Length());
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRemoteName(const TDesC& aSection)
	{
	T_TBTSockAddrChild addr;
	iData->iBluetoothSocket->RemoteName(addr);

	TBTDevAddr currentAddr = addr.BTAddr();
	TBuf<KMaxSockAddrSize> tmpBuf;
	currentAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("RemoteName() = 0x%S"),&tmpBuf);


	TPtrC	myTBTDevAddrName;
	if( GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
		{
		CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));
		TBTDevAddr btDevAddr = *myTBTDevAddrData->GetAddress();
		TBuf<KMaxSockAddrSize> tmpBuf2;
		btDevAddr.GetReadable(tmpBuf2);
		if (tmpBuf2 != tmpBuf)
			{
			ERR_PRINTF3(_L("Expected address (%S) != actual address (%S)"),&tmpBuf2,&tmpBuf);
			SetBlockResult(EFail);
			}
		}
	else
		{
		TInt	lhs;
		TInt	rhs;
		TBool	lhsGiven = EFalse;
		TBool	rhsGiven = EFalse;
		lhsGiven = GetHexFromConfig(aSection, KHexBTAddrL(), lhs );
		rhsGiven = GetHexFromConfig(aSection, KHexBTAddrR(), rhs );

		if (lhsGiven && rhsGiven)
			{
			TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
			TBuf<KMaxSockAddrSize> tmpBuf2;
			btDevAddr.GetReadable(tmpBuf2);
			if (tmpBuf2 != tmpBuf)
				{
				ERR_PRINTF3(_L("Expected address (%S) != actual address (%S)"),&tmpBuf2,&tmpBuf);
				SetBlockResult(EFail);
				}
			}
		}

	// Set in TBTSockAddr Data object so that it can be verified
	TPtrC	myTSockAddrName;
	CT_BTSockAddrData* myTSockAddrData=NULL;
	if( GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName))
		{
		myTSockAddrData=static_cast<CT_BTSockAddrData*>(GetDataWrapperL(myTSockAddrName));
		if ( myTSockAddrData!=NULL )
			{
			myTSockAddrData->SetObject(&addr,EFalse);
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRequestChangeSupportedPacketTypes(const TDesC& aSection)
	{
	TInt	mask = 0;
	CT_BTUtil::GetTBTPacketType(*this, aSection, mask);
	TInt	errCode = iData->iBluetoothSocket->RequestChangeSupportedPacketTypes(mask);
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("RequestChangeSupportedPacketTypes failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRequestMasterRole()
	{
	TInt errCode = iData->iBluetoothSocket->RequestMasterRole();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("RequestMasterRole failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdRequestSlaveRole()
	{
	TInt errCode = iData->iBluetoothSocket->RequestSlaveRole();
	if (errCode != KErrNone)
		{
		ERR_PRINTF2(_L("RequestSlaveRole failed with error code: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSend(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TPtrC dataToSend;
	iSendFlag = EFalse;
	if( GetStringFromConfig(aSection, KData(), dataToSend))
		{
		INFO_PRINTF2(_L("About to send: [%S]"), &dataToSend);

		TBuf8<DATASIZE> 	dataToSend8(_L8(""));
		dataToSend8.Copy(dataToSend);

		for ( ;dataToSend8.Length()<DATASIZE;)
			{
			dataToSend8.Append(_L8("#"));
			}

		TBool boolCallTwice = EFalse;
		GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

		TInt flags=0;
		GetIntFromConfig(aSection, KFlags(), flags);
		TBool useSend2 =EFalse;
		GetBoolFromConfig(aSection, KUseSend2(), useSend2);
		if (useSend2!=EFalse)
			{
			INFO_PRINTF1(_L("Using Send 2"));
			INFO_PRINTF2(_L("Flags: %d"),flags);
			TSockXfrLength actuallengthSent;

			TInt errCode = iData->iBluetoothSocket->Send(dataToSend8,flags,actuallengthSent);
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("Send failed: %d"),errCode);
				SetError(errCode);
				}
			else
				{
        		IncOutstanding();
		        iSendIndex = aAsyncErrorIndex;
				}
			if( boolCallTwice )
				{
				errCode = iData->iBluetoothSocket->Send(dataToSend8,flags,actuallengthSent);
				if (errCode != KErrNone)
					{
					ERR_PRINTF2(_L("Send failed: %d"),errCode);
					SetError(errCode);
					}
				else
					{
            		IncOutstanding();
            		iSendFlag = ETrue;
	    	        iSendIndex = aAsyncErrorIndex;
					}
				}
			if (errCode == KErrNone)
				{
				TInt& lenSentInt=actuallengthSent();
				INFO_PRINTF2(_L("Returned length actual sent: %d"), lenSentInt);
				if (lenSentInt != dataToSend8.Length())
					{
					INFO_PRINTF3(_L("Returned length sent: %d != Actual length sent: %d"),lenSentInt,dataToSend8.Length());
					SetBlockResult(EFail);
					}	
				}
			}
		else
			{
			INFO_PRINTF1(_L("Using Send 1"));
			INFO_PRINTF2(_L("Flags: %d"),flags);
			TInt errCode = iData->iBluetoothSocket->Send(dataToSend8,flags);
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("Send failed: %d"),errCode);
				SetError(errCode);
				}
			else
				{
        		IncOutstanding();
        		iSendFlag = ETrue;
		        iSendIndex = aAsyncErrorIndex;
				}
			if( boolCallTwice )
				{
				errCode = iData->iBluetoothSocket->Send(dataToSend8,flags);
				if (errCode != KErrNone)
					{
					ERR_PRINTF2(_L("Send failed: %d"),errCode);
					SetError(errCode);
					}
				else
					{
            		IncOutstanding();
            		iSendFlag = ETrue;
	    	        iSendIndex = aAsyncErrorIndex;
					}
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSendTo(const TDesC& aSection,const TInt aAsyncErrorIndex)
	{
	TBool useSend2 =EFalse;
	GetBoolFromConfig(aSection, KUseSend2(), useSend2);

	TInt flags = 0; // read in, if not found send 0
	GetIntFromConfig(aSection, KFlags(), flags);
	TPtrC dataToSend;
	if( GetStringFromConfig(aSection, KData(), dataToSend))
		{
		INFO_PRINTF2(_L("About to send: [%S]"), &dataToSend);
    	TBool boolCallTwice = EFalse;
		GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

		TBuf8<DATASIZE> 	dataToSend8(_L8(""));
		dataToSend8.Copy(dataToSend);
		TPtrC	myTSockAddrName;
		TBTSockAddr* myTSockAddr=NULL;
		if( GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName))
			{
			INFO_PRINTF2(_L("Binding with: %S TSockAddr"), &myTSockAddrName);
			myTSockAddr=static_cast<TBTSockAddr*>(GetDataObjectL(myTSockAddrName));
			if ( myTSockAddr!=NULL )
				{
				if (!useSend2)
					{
					TInt errCode = iData->iBluetoothSocket->SendTo(dataToSend8,*myTSockAddr,flags);
					if (errCode != KErrNone)
						{
						ERR_PRINTF2(_L("SendTo failed with error code: %d"),errCode);
						SetError(errCode);
						}
					else
						{
                		IncOutstanding();
                	    iSendIndex = aAsyncErrorIndex;
						}
					if( boolCallTwice )
						{
						errCode = iData->iBluetoothSocket->SendTo(dataToSend8,*myTSockAddr,flags);
						if (errCode != KErrNone)
							{
							ERR_PRINTF2(_L("SendTo failed with error code: %d"),errCode);
							SetError(errCode);
							}
						else
							{
                    		IncOutstanding();
                    		iSendIndex = aAsyncErrorIndex;
							}
						}
					}
				else
					{
					TSockXfrLength lenSent;
					TInt errCode = iData->iBluetoothSocket->SendTo(dataToSend8,*myTSockAddr,flags,lenSent);
					if (errCode != KErrNone)
						{
						ERR_PRINTF2(_L("SendTo failed with error code: %d"),errCode);
						SetError(errCode);
						}
					else
						{
                		IncOutstanding();
                		iSendIndex = aAsyncErrorIndex;
						}
					if( boolCallTwice )
						{
						errCode = iData->iBluetoothSocket->SendTo(dataToSend8,*myTSockAddr,flags,lenSent);
						if (errCode != KErrNone)
							{
							ERR_PRINTF2(_L("SendTo failed with error code: %d"),errCode);
							SetError(errCode);
							}
						else
							{
                    		IncOutstanding();
                    	    iSendIndex = aAsyncErrorIndex;
							}
						}
					if (errCode == KErrNone)
						{
						TInt& lenSentInt=lenSent();
						INFO_PRINTF2(_L("Returned actual length sent: %d"), lenSentInt);
						if (lenSentInt != dataToSend8.Length())
							{
							INFO_PRINTF3(_L("Returned length sent: %d != Actual length sent: %d"),lenSentInt,dataToSend8.Length());
							}	
						}
				
					}
				}
			else
				{
				ERR_PRINTF1(_L("SendTo TBTSockAddr is NULL"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetStringFromConfig failed"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSetAutomaticSniffMode(const TDesC& aSection)
	{
	
	TBool	sniffMode = EFalse;
	GetBoolFromConfig(aSection, KSniffMode(), sniffMode);

	TInt errCode = KErrNone;

	TInt	sniffDelay = 0;
	if( GetIntFromConfig(aSection, KSniffDelay(), sniffDelay) )
		{
		INFO_PRINTF3(_L("SetAutomaticSniffMode(%d, %d)"), sniffMode, sniffDelay);
		errCode = iData->iBluetoothSocket->SetAutomaticSniffMode(sniffMode, sniffDelay);
		}
	else
		{
		INFO_PRINTF2(_L("SetAutomaticSniffMode(%d)"), sniffMode);
		errCode = iData->iBluetoothSocket->SetAutomaticSniffMode(sniffMode);
		}
	
	if (errCode!=KErrNone)
		{
		ERR_PRINTF2(_L("SetAutomaticSniffMode failed: %d"),errCode);
		SetError(errCode);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSetLocalPort(const TDesC& aSection)
	{
	TInt	port;

	if( GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF2(_L("Port to set is: %d"), port);
		TInt errCode = iData->iBluetoothSocket->SetLocalPort(port);
		if (errCode!=KErrNone)
			{
			ERR_PRINTF2(_L("SetLocalPort failed: %d"),errCode);
			SetError(errCode);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing :%S"), &KPort());
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSetNotifierL(const TDesC& aSection)
	{
	MBluetoothSocketNotifier* theNotifier = iData;

	TBool	useAlternate =EFalse;
	GetBoolFromConfig(aSection, KUseAlternateNotifier(), useAlternate);
	if (useAlternate)
		{
		theNotifier=this;
		}

	iData->iBluetoothSocket->SetNotifier(*theNotifier);
	}

inline void CT_CBluetoothSocketData::DoCmdSetOpt(const TDesC& aSection)
	{
	TPtrC optionName;
	TPtrC optionLevel;
	TInt optionNameValue;
	TInt optionLevelValue;
	TBool found = EFalse;
	if( GetStringFromConfig(aSection, KOptionName(), optionName))
		{
		found=CT_BTUtil::GetIntValue(optionName, optionNameValue);
		//Report error but continue as it may still make sense
		if (!found)
			{
			ERR_PRINTF2(_L("Option not found in lookup: %S"),&optionName);
			SetBlockResult(EFail);
			}
		INFO_PRINTF3(_L("The name of the option to set: %S , its value: %d" ),&optionName, optionNameValue);
		if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel))
			{
			found=CT_BTUtil::GetIntValue(optionLevel, optionLevelValue);
			//Report error but continue as it may still make sense
			if (!found)
				{
				ERR_PRINTF2(_L("Option Level not found in lookup: %S"),&optionLevel);
				SetBlockResult(EFail);
				}
			INFO_PRINTF3(_L("The name of the option level to set: %S , its value: %d" ),&optionLevel, optionLevelValue);
			TPtrC intOptionName;
			if( GetStringFromConfig(aSection, KNamedIntOption(), intOptionName))
				{
				INFO_PRINTF1(_L("Using SetOpt1 with named constant value"));
				TInt	intOption;
				found=CT_BTUtil::GetIntValue(intOptionName, intOption);
				//Report error but continue as it may still make sense
				if (!found)
					{
					ERR_PRINTF2(_L("Option not found in lookup: %S"),&optionLevel);
					SetBlockResult(EFail);
					}
				INFO_PRINTF3(_L("The name of the option level to set: %S its value: %d" ),&intOptionName, intOption);
				iData->iBluetoothSocket->SetOpt(optionNameValue,optionLevelValue,intOption);
				}
			else
				{
				TInt intOption;
				if( GetIntFromConfig(aSection, KIntOption(), intOption))
					{
					INFO_PRINTF1(_L("Using SetOpt1 with int value"));
					INFO_PRINTF2(_L("The value of the option level to set: %d" ),intOption);
					iData->iBluetoothSocket->SetOpt(optionNameValue,optionLevelValue,intOption);
					}
				else
					{
					TPtrC desOption;
					if( GetStringFromConfig(aSection, KDesOption(), desOption))
						{
						INFO_PRINTF2(_L("The option level to set: %S" ),&desOption);
						TBuf8<DATASIZE> desOption8(_L8(""));
						desOption8.Copy(desOption);
						INFO_PRINTF3(_L("desOption len %d, desOption8 len %d" ),desOption.Length(), desOption8.Length());
						INFO_PRINTF1(_L("Using Deprecated SetOpt2"));
						iData->iBluetoothSocket->SetOpt(optionNameValue,optionLevelValue,desOption8);
						}
					else
						{
						// Use default parameter for SetOpt i.e. call with two parameters only
						INFO_PRINTF1(_L("Using Deprecated SetOpt2 with default value"));
						iData->iBluetoothSocket->SetOpt(optionNameValue,optionLevelValue);
						}
					}
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetStringFromConfig failed"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}


inline void CT_CBluetoothSocketData::DoCmdSetOption(const TDesC& aSection)
	{
	TPtrC optionName;
	TPtrC optionLevel;
	TInt optionNameValue;
	TInt optionLevelValue;
	TBool found = EFalse;
	if( GetStringFromConfig(aSection, KOptionName(), optionName))
		{
		found=CT_BTUtil::GetIntValue(optionName, optionNameValue);
		//Report error but continue as it may still make sense
		if (!found)
			{
			ERR_PRINTF2(_L("Option not found in lookup: %S"),&optionName);
			SetBlockResult(EFail);
			}
		INFO_PRINTF3(_L("The name of the option to set: %S , its value: %d" ),&optionName, optionNameValue);
		if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel))
			{
			found=CT_BTUtil::GetIntValue(optionLevel, optionLevelValue);
			//Report error but continue as it may still make sense
			if (!found)
				{
				ERR_PRINTF2(_L("Option Level not found in lookup: %S"),&optionLevel);
				SetBlockResult(EFail);
				}

			TPtrC desOption;
			if( GetStringFromConfig(aSection, KDesOption(), desOption))
				{
				INFO_PRINTF2(_L("The option level to set: %S" ),&desOption);
				TBuf8<DATASIZE> desOption8(_L8(""));
				desOption8.Copy(desOption);

				INFO_PRINTF3(_L("desOption len %d, desOption8 len %d" ),desOption.Length(), desOption8.Length());
				INFO_PRINTF1(_L("Using SetOption"));
				iData->iBluetoothSocket->SetOption(optionNameValue,optionLevelValue,desOption8);
				}
			else
				{
				ERR_PRINTF1(_L("GetStringFromConfig failed"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetStringFromConfig failed"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdShutdown(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TBool	boolCallTwice = EFalse;
	GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

    RSocket::TShutdown	shutdown=RSocket::ENormal;
	TBool				dataOk=CT_BTUtil::GetShutdown(*this, aSection, shutdown);

	if ( dataOk )
		{
		TInt			err=KErrNone;
		TBuf8<DATASIZE>	dataToSend8(_L8(""));
		TPtrC			disconnectData;
		TBool			hasDisconnectData=GetStringFromConfig(aSection, KDisconnectData(), disconnectData);
		if( hasDisconnectData )
			{
			INFO_PRINTF1(_L("Using second shutdown"));
			dataToSend8.Copy(disconnectData);
			err = iData->iBluetoothSocket->Shutdown(shutdown, dataToSend8, iDataAtDisconnect8);
			}
		else
			{
			INFO_PRINTF1(_L("Using standard shutdown"));
			err = iData->iBluetoothSocket->Shutdown(shutdown);
			}
		if ( err==KErrNone )
			{
       		IncOutstanding();
	        iShutdownIndex = aAsyncErrorIndex;
			}
		else
			{
			ERR_PRINTF2(_L("Shutdown of iBluetoothSocket failed with error code %d"), err);
			SetError(err);
			}

		if( boolCallTwice )
			{
			if ( hasDisconnectData )
				{
				err = iData->iBluetoothSocket->Shutdown(shutdown, dataToSend8, iDataAtDisconnect8);
				}
			else
				{
				err = iData->iBluetoothSocket->Shutdown(shutdown);
				}
			if (err != KErrNone )
				{
				ERR_PRINTF2(_L("Shutdown of iBluetoothSocket failed with error code %d"),err);
				SetError(err);
				}
			}
		}
	}

inline void CT_CBluetoothSocketData::DoCmdTransfer(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Transfer the Bluetooth socket to this testddata object" ));

	TPtrC	myRSocketName;
	RSocket* myRSocket=NULL;
	if( GetStringFromConfig(aSection, KRSocket(), myRSocketName))
		{
		INFO_PRINTF2(_L("Transfering: %S to this RSocketServer"), &myRSocketName);
		myRSocket=static_cast<RSocket*>(GetDataObjectL(myRSocketName));
		if ( myRSocket!=NULL )
			{
			TName myRSockName;
			myRSocket->Name(myRSockName);

			if (myRSockName.Length()>0)
				{
				TInt err = iData->iBluetoothSocket->Transfer(iData->iSocketServer, myRSockName);
				if (err == KErrNone )
					{
					INFO_PRINTF1(_L("Transfer successful" ));
					}
				else
					{
					ERR_PRINTF2(_L("Transfer failed with error code %d"),err);
					SetError(err);
					}
				}
			else
				{
				ERR_PRINTF1(_L("Failed to fetch RSocket name"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Failed to fetch RSocket from %S"),&myRSocketName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdWrite(const TDesC& aSection,const TInt aAsyncErrorIndex)
	{

	TPtrC dataToSend;
	iWriteFlag = EFalse;
	if( GetStringFromConfig(aSection, KData(), dataToSend))
		{
		INFO_PRINTF2(_L("About to write: [%S]"), &dataToSend);

		TBool boolCallTwice = EFalse;
		GetBoolFromConfig(aSection, KCallTwice(), boolCallTwice);

		TBuf8<DATASIZE> dataToSend8(_L8(""));
		dataToSend8.Copy(dataToSend);

		for ( ;dataToSend8.Length()<DATASIZE;)
			{
			dataToSend8.Append(_L8("#"));
			}
		TInt errCode = iData->iBluetoothSocket->Write(dataToSend8);
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("ActivateBasebandEventNotifier failed with error code: %d"),errCode);
			SetError(errCode);
			}
		else
			{
      		IncOutstanding();
      		iWriteFlag = ETrue;
	        iSendIndex = aAsyncErrorIndex;
			}
		if( boolCallTwice )
			{
			errCode = iData->iBluetoothSocket->Write(dataToSend8);
			if (errCode != KErrNone)
				{
				ERR_PRINTF2(_L("ActivateBasebandEventNotifier failed with error code: %d"),errCode);
				SetError(errCode);
				}
			else
				{
        		IncOutstanding();
        		iWriteFlag = ETrue;
		        iSendIndex = aAsyncErrorIndex;
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdTestMBSN_ExtensionInterfaceL(const TDesC& aSection)
	{
	TInt	tmp;
	void* theExtensionObjectPtr;

	TBool expectNull = EFalse;
	GetBoolFromConfig(aSection, KExpectNull(), expectNull);

	if( GetIntFromConfig(aSection, KUid(), tmp))
		{
		TUid uid=TUid::Uid(tmp);
		TRAPD(err, iData->MBSN_ExtensionInterfaceL(uid,theExtensionObjectPtr));
		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("MBSN_ExtensionInterfaceL err=%d"),err);
			SetError(EFail);
			}

		if (expectNull)
			{
			//Fail if not NULL
			if (theExtensionObjectPtr!=NULL)
				{
				ERR_PRINTF1(_L("Expected Null pointer but object found"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			//Fail if NULL
			if (theExtensionObjectPtr!=NULL)
				{
				}
			else
				{
				ERR_PRINTF1(_L("Expected object but found NULL"));
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetIntFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSetRsocketServerFromCBTSocket(const TDesC& aSection)
	{

	// Get the blank session socket
	TPtrC	myCT_CBluetoothSocketDataName;
	CT_CBluetoothSocketData* myCT_CBluetoothSocketData=NULL;
	if( GetStringFromConfig(aSection, KCBluetoothSocket(), myCT_CBluetoothSocketDataName))
		{
		myCT_CBluetoothSocketData=static_cast<CT_CBluetoothSocketData*>(GetDataWrapperL(myCT_CBluetoothSocketDataName));
		if ( myCT_CBluetoothSocketData!=NULL )
			{
			iData->iSocketServer=myCT_CBluetoothSocketData->GetRServer();
			}
		else
			{
			ERR_PRINTF1(_L("CBluetoothSocket is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);
		}
	}

inline void CT_CBluetoothSocketData::DoCmdSetTransferAble()
	{
	iData->iBluetoothSocket->SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
	}

RSocketServ& CT_CBluetoothSocketData::GetRServer()
	{
	return iData->iSocketServer;
	}

//	MBluetoothSocketNotifier implementation
void CT_CBluetoothSocketData::HandleConnectCompleteL(TInt aErr)
	{
	INFO_PRINTF2(_L("HandleConnectCompleteL : Status (%d)" ), aErr);

	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleConnectCompleteL called with error code: %d"),aErr);
		SetAsyncError(iConnectIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleConnectCompleteL successful" ));
		}

	TBuf<DATASIZE>	dataToRead(_L(""));
	dataToRead.Copy(iDataToRead8);
	INFO_PRINTF2(_L("Retrived: [%S]"), &dataToRead);

	if ( iHasConnectDataToExpect )
		{
		TBuf<DATASIZE>	dataExpected;
		dataExpected.Copy(iConnectDataToExpect);

		// If dataExpected is shorter add filler chars as we did in send
		// We just need to use a TBuf so that we can change it
		if ( dataExpected.Length()<dataToRead.Length() )
			{
			for ( ;dataExpected.Length()<DATASIZE;)
				{
				dataExpected.Append(_L("#"));
				}
			}

		//Verify the data
		INFO_PRINTF2(_L("Expected: [%S]"), &dataExpected);
		if ( dataExpected != dataToRead )
			{
			ERR_PRINTF1(_L("Data is not as expected"));
			SetBlockResult(EFail);
			}
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleAcceptCompleteL(TInt aErr)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleAcceptCompleteL called with error code: %d"),aErr);
		SetAsyncError(iAcceptIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleAcceptCompleteL successful" ));
		}

	TBuf<DATASIZE>	dataToRead(_L(""));
	dataToRead.Copy(iDataToRead8);
	INFO_PRINTF2(_L("Retrived: [%S]"),&dataToRead);

	if ( iHasAcceptDataToExpect )
		{
		TBuf<DATASIZE>	dataExpected;
		dataExpected.Copy(iAcceptDataToExpect);

		// If dataExpected is shorter add filler chars as we did in send
		// We just need to use a TBuf so that we can change it
		if ( dataExpected.Length()<dataToRead.Length() )
			{
			for ( ;dataExpected.Length()<DATASIZE;)
				{
				dataExpected.Append(_L("#"));
				}
			}

		//Verify the data
		INFO_PRINTF2(_L("Expected: [%S]"),&dataExpected);
		if (dataExpected != dataToRead)
			{
			ERR_PRINTF1(_L("Data is not as expected"));
			SetBlockResult(EFail);
			}
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleShutdownCompleteL(TInt aErr)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleShutdownCompleteL called with error code: %d"),aErr);
		SetAsyncError(iShutdownIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleShutdownCompleteL successful" ));
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleSendCompleteL(TInt aErr)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleSendCompleteL called with error code: %d"),aErr);
		SetAsyncError(iSendIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleSendCompleteL successful" ));
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleReceiveCompleteL(TInt aErr)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleReceiveCompleteL called with error code: %d"),aErr);
		SetAsyncError(iReceiveIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleReceiveCompleteL successful" ));
		}

	TBuf<DATASIZE>	dataToRead(_L(""));
	dataToRead.Copy(iDataToRead8);
	INFO_PRINTF2(_L("Retrived: [%S]"), &dataToRead);

	if ( iHasReceiveDataToExpect )
		{
		TBuf<DATASIZE>	dataExpected;
		dataExpected.Copy(iReceiveDataToExpect);

		// If dataExpected is shorter add filler chars as we did in send
		// We just need to use a TBuf so that we can change it
		if ( dataExpected.Length()<dataToRead.Length() )
			{
			for ( ;dataExpected.Length()<DATASIZE;)
				{
				dataExpected.Append(_L("#"));
				}
			}

		//	Verify the data
		if (dataExpected != dataToRead)
			{
			ERR_PRINTF1(_L("Data is not as expected"));
			SetBlockResult(EFail);
			}
		INFO_PRINTF2(_L("Expected: [%S]"), &dataExpected);
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleIoctlCompleteL(TInt aErr)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleIoctlCompleteL called with error code: %d"),aErr);
		SetAsyncError(iIoctlIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleIoctlCompleteL successful" ));
		
		TBuf8<DATASIZE>	tempBuffer(_L8(""));
		tempBuffer.Copy(iMTUOptionBuffer);
		INFO_PRINTF2(_L("HandleIoctlCompleteL = %S" ), &tempBuffer);
		}
	DecOutstanding();
	}

void CT_CBluetoothSocketData::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification & aEventNotification)
	{
	if (aErr != KErrNone)
		{
		ERR_PRINTF2(_L("HandleActivateBasebandEventNotifierCompleteL called with error code: %d"),aErr);
		SetAsyncError(iNotifierIndex, aErr);
		}
	else
		{
		INFO_PRINTF1(_L("HandleActivateBasebandEventNotifierCompleteL successful" ));
		INFO_PRINTF4(_L("Even type: %d Error code: %d Symbian Error Code: %d"), aEventNotification.EventType(), aEventNotification.ErrorCode(), aEventNotification.SymbianErrorCode());


		}
	DecOutstanding();
	}
