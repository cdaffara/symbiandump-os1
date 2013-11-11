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

#include "T_BluetoothSynchronousLinkData.h"
#include "T_BTDevAddrData.h"
#include "T_BTSyncBandWidthData.h"
#include "T_BTUtil.h"
#include "T_TBTSockAddrChild.h"

/*@{*/
//Parameters
_LIT(KRSocketServ,												"socketserv");
_LIT(KTBTDevAddr,												"tbtdevaddr");
_LIT(KIntDevAddr,												"integerdevaddr");
_LIT(KPacketType,												"packettype");
_LIT(KSyncPacket,												"syncpacket");
_LIT(KData,														"data");
_LIT(KVoiceSetting,												"voicesetting");
_LIT(KSyncBandwidth,											"syncbandwidth");
_LIT(KLatency,													"latency");
_LIT(KRetransmissionType,										"retransmissiontype");
_LIT(KUseAlternateNotifier,										"usealternatenotifier");
_LIT(KUid,														"uid");
_LIT(KExpectNull,												"expectnull");
_LIT(KExpected, 												"expected");
_LIT(KErrForSetupConnection,									"errforsetupconnection");
_LIT(KErrForSend,												"errforsend");
_LIT(KErrForReceive,											"errforreceive");
_LIT(KErrForAcceptConnection,									"errforacceptconnection");
_LIT(KErrForDisconnect,											"errfordisconnect");
_LIT(KIncOutstanding,											"incoutstanding");

//Commands
_LIT(KCmdNewL,													"NewL");
_LIT(KCmdNewLC,													"NewLC");
_LIT(KCmdSetupConnection,										"SetupConnection");
_LIT(KCmdCancelSetup,											"CancelSetup");
_LIT(KCmdDisconnect,											"Disconnect");
_LIT(KCmdSend,													"Send");
_LIT(KCmdCancelSend,											"CancelSend");
_LIT(KCmdReceive,												"Receive");
_LIT(KCmdCancelReceive,											"CancelReceive");
_LIT(KCmdAcceptConnection,										"AcceptConnection");
_LIT(KCmdCancelAccept,											"CancelAccept");
_LIT(KCmdSetCoding,												"SetCoding");
_LIT(KCmdCoding,												"Coding");
_LIT(KCmdSetMaxBandwidth,										"SetMaxBandwidth");
_LIT(KCmdBandwidth,												"Bandwidth");
_LIT(KCmdSetMaxLatency,											"SetMaxLatency");
_LIT(KCmdLatency,												"Latency");
_LIT(KCmdSetRetransmissionEffort,								"SetRetransmissionEffort");
_LIT(KCmdRetransmissionEffort,									"RetransmissionEffort");
_LIT(KCmdRemoteName,											"RemoteName");
_LIT(KCmdSetNotifier,											"SetNotifier");
_LIT(KCmdDestructor,											"~");
_LIT(KCmdMBSLN_ExtensionInterfaceL,								"MBSLN_ExtensionInterfaceL");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BluetoothSynchronousLinkData* CT_BluetoothSynchronousLinkData::NewL()
	{
	CT_BluetoothSynchronousLinkData*	ret=new (ELeave) CT_BluetoothSynchronousLinkData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BluetoothSynchronousLinkData::CT_BluetoothSynchronousLinkData()
:	iData(NULL)
,	iErrForSetupConnection(0)
,	iErrForSend(0)
,	iErrForReceive(0)
,	iErrForAcceptConnection(0)
,	iErrForDisconnect(0)
,	ibtSyncPackets(NULL)
,	iSetupIndex(0)
,	iDisconnectionIndex(0)
,	iAcceptIndex(0)
,	iSendIndex(0)
,	iReceiveIndex(0)
	{
	}

void CT_BluetoothSynchronousLinkData::ConstructL()
	{
	}

CT_BluetoothSynchronousLinkData::~CT_BluetoothSynchronousLinkData()
	{
	DestroyData();
	}

//	Service methods
TAny* CT_BluetoothSynchronousLinkData::GetObject()
	{
	return iData;
	}

void CT_BluetoothSynchronousLinkData::SetObjectL(TAny* aObject)
	{
	//called when loading up persistent data
	DestroyData();
	iData = static_cast<CBluetoothSynchronousLink*>(aObject);
	}

void CT_BluetoothSynchronousLinkData::DisownObjectL()
	{
	iData=NULL;
	}

void CT_BluetoothSynchronousLinkData::DestroyData()
	{
	delete iData;
	iData = NULL;
	if ( ibtSyncPackets )
		{
		ibtSyncPackets = NULL;
		}
	}

inline TCleanupOperation CT_BluetoothSynchronousLinkData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BluetoothSynchronousLinkData::CleanupOperation(TAny* aAny)
	{
	CBluetoothSynchronousLink* arg=static_cast<CBluetoothSynchronousLink*>(aAny);
	delete arg;
	}

void CT_BluetoothSynchronousLinkData::HandleSetupConnectionCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothSynchronousLinkNotifier::HandleSetupConnectionCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForSetupConnection )
		{
		ERR_PRINTF2(_L("HandleSetupConnectionCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iSetupIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleSetupConnectionCompleteL successful" ));
		}
	}

void CT_BluetoothSynchronousLinkData::HandleDisconnectionCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothSynchronousLinkNotifier::HandleDisconnectionCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForDisconnect) 
		{
		 ERR_PRINTF2(_L("HandleDisconnectionCompleteL Call failed: aError = %d"),aError);
		 SetAsyncError(iDisconnectionIndex, aError);
		}
	else
		{
		 INFO_PRINTF1(_L("HandleDisconnectionCompleteL successful" ));
		}
	}

void CT_BluetoothSynchronousLinkData::HandleAcceptConnectionCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothSynchronousLinkNotifier::HandleAcceptConnectionCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForAcceptConnection) 
		{
		ERR_PRINTF2(_L("HandleAcceptConnectionCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iAcceptIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleAcceptConnectionCompleteL successful" ));
		}
	}

void CT_BluetoothSynchronousLinkData::HandleSendCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothSynchronousLinkNotifier::HandleSendCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForSend )
		{
		ERR_PRINTF2(_L("HandleSendCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iSendIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleSendCompleteL successful" ));
		}
	}

void CT_BluetoothSynchronousLinkData::HandleReceiveCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothSynchronousLinkNotifier::HandleReceiveCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForReceive )
		{
		ERR_PRINTF2(_L("HandleReceiveCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iReceiveIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleReceiveCompleteL successful" ));
		}
	}

TBool CT_BluetoothSynchronousLinkData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdNewLC )
		{
		DoCmdNewLC(aSection);
		}
	else if ( aCommand==KCmdSetupConnection )
		{
		DoCmdSetupConnection(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelSetup )
		{
		DoCmdCancelSetup();
		}
	else if ( aCommand==KCmdDisconnect )
		{
		DoCmdDisconnect(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSend )
		{
		DoCmdSend(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelSend )
		{
		DoCmdCancelSend();
		}
	else if ( aCommand==KCmdReceive )
		{
		DoCmdReceive(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelReceive )
		{
		DoCmdCancelReceive();
		}
	else if ( aCommand==KCmdAcceptConnection )
		{
		DoCmdAcceptConnection(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelAccept )
		{
		DoCmdCancelAccept();
		}
	else if ( aCommand==KCmdSetCoding )
		{
		DoCmdSetCoding(aSection);
		}
	else if ( aCommand==KCmdCoding )
		{
		DoCmdCoding(aSection);
		}
	else if ( aCommand==KCmdSetMaxBandwidth )
		{
		DoCmdSetMaxBandwidth(aSection);
		}
	else if ( aCommand==KCmdBandwidth )
		{
		DoCmdBandwidth();
		}
	else if ( aCommand==KCmdSetMaxLatency )
		{
		DoCmdSetMaxLatency(aSection);
		}
	else if ( aCommand==KCmdLatency )
		{
		DoCmdLatency(aSection);
		}
	else if ( aCommand==KCmdSetRetransmissionEffort )
		{
		DoCmdSetRetransmissionEffort(aSection);
		}
	else if ( aCommand==KCmdRetransmissionEffort )
		{
		DoCmdRetransmissionEffort(aSection);
		}
	else if ( aCommand==KCmdRemoteName )
		{
		DoCmdRemoteName(aSection);
		}
	else if ( aCommand==KCmdSetNotifier )
		{
		DoCmdSetNotifier(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdMBSLN_ExtensionInterfaceL )
		{
		DoCmdMBSLN_ExtensionInterfaceL(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test NewL()
*/
void CT_BluetoothSynchronousLinkData::DoCmdNewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink NewL Call"));
	DestroyData();
	
	TPtrC	socketServName;
	if(GetStringFromConfig(aSection, KRSocketServ(), socketServName))
		{
		RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		TRAPD(err, iData = CBluetoothSynchronousLink::NewL(*this, *socketServ));
		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("CBluetoothSynchronousLink Standard Constructor NewL err=%d" ),err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRSocketServ());
		SetBlockResult(EFail);
		}
	}


/**
Test NewLC()
*/
void CT_BluetoothSynchronousLinkData::DoCmdNewLC(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink NewLC Call"));
	DestroyData();
	
	TPtrC	socketServName;
	if(GetStringFromConfig(aSection, KRSocketServ(), socketServName))
		{
		RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		TRAPD(err, iData = CBluetoothSynchronousLink::NewLC(*this, *socketServ); CleanupStack::Pop(););
		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("CBluetoothSynchronousLink Standard Constructor NewLC err=%d" ),err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRSocketServ());
		SetBlockResult(EFail);
		}
	}


/**
Test SetupConnection()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetupConnection(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetupConnection Call"));
	
	TBool	foundTBTDevAddr = EFalse;
	TPtrC	tbtDevAddrName;
	CT_BTDevAddrData* tbtDevAddrData = NULL;
	if(GetStringFromConfig(aSection, KTBTDevAddr(), tbtDevAddrName))
		{
		foundTBTDevAddr = ETrue;
		tbtDevAddrData = static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tbtDevAddrName));
		}
	
	TBool	foundIntDevAddr = EFalse;
	TInt	intBtDevAddr = 0;
	if(GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundIntDevAddr = ETrue;
		}
	
	if( !foundTBTDevAddr & !foundIntDevAddr )
		{
		ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	
	TInt err = 0;
	//SetupConnection()2
	TBool	foundPackType = EFalse;
	TPtrC	packetTypeName;
	TInt	packetTypeValue;
	if(GetStringFromConfig(aSection, KPacketType(), packetTypeName))
		{
		if ( CT_BTUtil::GetPacketTypeIntValue(packetTypeName, packetTypeValue) )
			{
			INFO_PRINTF2(_L("CBluetoothSynchronousLink SetupConnection: packetTypeValue = %d"), packetTypeValue);
			foundPackType = ETrue;
			if( foundTBTDevAddr )
				{
				const TBTDevAddr* btDevAddr = tbtDevAddrData->GetAddress();
				err = iData->SetupConnection(*btDevAddr, packetTypeValue);
				}
			else
				{
				const TBTDevAddr intDevAddr(intBtDevAddr);
				iBTDeveAddr = intDevAddr;
				err = iData->SetupConnection(iBTDeveAddr, packetTypeValue);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Unknow input for PacketType"));
			SetBlockResult(EFail);
			}
		}
	
	//SetupConnection()3
	TBool	foundSyncPack = EFalse;
	TPtrC	syncPacketName;
	if (GetStringFromConfig(aSection, KSyncPacket(), syncPacketName) )
		{
		TBTSyncPackets*	tbtSyncPackets = static_cast<TBTSyncPackets*>(GetDataObjectL(syncPacketName));
		foundSyncPack = ETrue;
		if( foundTBTDevAddr )
			{
			const TBTDevAddr* btDevAddr = tbtDevAddrData->GetAddress();
			err = iData->SetupConnection(*btDevAddr, *tbtSyncPackets);
			}
		else
			{
			const TBTDevAddr intDevAddr(intBtDevAddr);
			iBTDeveAddr = intDevAddr;
			err = iData->SetupConnection(iBTDeveAddr, *tbtSyncPackets);
			}
		}
	
	//SetupConnection()1
	if( !foundPackType & !foundSyncPack )
		{
		if( foundTBTDevAddr )
			{
			const TBTDevAddr* btDevAddr = tbtDevAddrData->GetAddress();
			err = iData->SetupConnection(*btDevAddr);
			}
		else
			{
			const TBTDevAddr intDevAddr(intBtDevAddr);
			iBTDeveAddr = intDevAddr;
			err = iData->SetupConnection(iBTDeveAddr);
			}
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothSynchronousLink SetupConnection failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iSetupIndex = aAsyncErrorIndex;
		
		}
	
	if( !GetIntFromConfig(aSection, KErrForSetupConnection(), iErrForSetupConnection))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForSetupConnection());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelSetup()
*/
void CT_BluetoothSynchronousLinkData::DoCmdCancelSetup()
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink CancelSetup Call"));
	iData->CancelSetup();
	DecOutstanding();
	}


/**
Test Disconnect()
*/
void CT_BluetoothSynchronousLinkData::DoCmdDisconnect(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Disconnect Call"));
	
	TInt err = iData->Disconnect();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothSynchronousLink Disconnect failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iDisconnectionIndex = aAsyncErrorIndex;
		}
	
	if( !GetIntFromConfig(aSection, KErrForDisconnect(), iErrForDisconnect))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForDisconnect());
		SetBlockResult(EFail);
		}
	}


/**
Test Send()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSend(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Send Call"));
	
	TPtrC	dataToSend;
	if(GetStringFromConfig(aSection, KData(), dataToSend))
		{
		TBuf8<KDataSize>	dataToSend8(_L8(""));
		dataToSend8.Copy(dataToSend);
		
		TInt err = iData->Send(dataToSend8);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CBluetoothSynchronousLink Send failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KData());
		SetBlockResult(EFail);
		}
	IncOutstanding();
	iSendIndex = aAsyncErrorIndex;
	
	if( !GetIntFromConfig(aSection, KErrForSend(), iErrForSend))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForSend());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelSend()
*/
void CT_BluetoothSynchronousLinkData::DoCmdCancelSend()
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink CancelSend Call"));
	iData->CancelSend();
	DecOutstanding();
	}


/**
Test Receive()
*/
void CT_BluetoothSynchronousLinkData::DoCmdReceive(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Receive Call"));
	
	TPtrC	dataToReceive;
	if(GetStringFromConfig(aSection, KData(), dataToReceive))
		{
		TBuf8<KDataSize>	dataToReceive8(_L8(""));
		dataToReceive8.Copy(dataToReceive);
		
		TInt err = iData->Receive(dataToReceive8);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CBluetoothSynchronousLink Receive failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KData());
		SetBlockResult(EFail);
		}
	IncOutstanding();
	iReceiveIndex = aAsyncErrorIndex;

	if( !GetIntFromConfig(aSection, KErrForReceive(), iErrForReceive))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForReceive());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelReceive()
*/
void CT_BluetoothSynchronousLinkData::DoCmdCancelReceive()
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink CancelReceive Call"));
	iData->CancelReceive();
	DecOutstanding();
	}


/**
Test AcceptConnection()
*/
void CT_BluetoothSynchronousLinkData::DoCmdAcceptConnection(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink AcceptConnection Call"));
	
	TInt err = 0;
	TPtrC	syncPacketName;
	if (GetStringFromConfig(aSection, KSyncPacket(), syncPacketName) )
		{
		ibtSyncPackets = static_cast<TBTSyncPackets*>(GetDataObjectL(syncPacketName));
		err = iData->AcceptConnection(*ibtSyncPackets);
		}
	else
		{
		err = iData->AcceptConnection();
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothSynchronousLink AcceptConnection failed with error %d"), err);
		SetError(err);
		}
	else
		{
        TBool incoutstanding = ETrue;
        // if in ini file, incoutstanding = false, this case is a unconnected case and will not expect a callback, so do not IncOutstanding().
        if ( (GetBoolFromConfig(aSection, KIncOutstanding(), incoutstanding)) &&  (incoutstanding == EFalse) ) 
            {
            INFO_PRINTF1(_L("incoutstanding is false, do not IncOutstanding()"));
            }
        else
            {
            IncOutstanding();
    		iAcceptIndex = aAsyncErrorIndex;
            }
		}
	
	if( !GetIntFromConfig(aSection, KErrForAcceptConnection(), iErrForAcceptConnection))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForAcceptConnection());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelAccept()
*/
void CT_BluetoothSynchronousLinkData::DoCmdCancelAccept()
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink CancelAccept Call"));
	iData->CancelAccept();
	DecOutstanding();
	}


/**
Test SetCoding()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetCoding(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetCoding Call"));
	
	TInt	voiceSetting;
	if(GetIntFromConfig(aSection, KVoiceSetting(), voiceSetting))
		{
		iData->SetCoding(voiceSetting);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KVoiceSetting());
		SetBlockResult(EFail);
		}
	}


/**
Test Coding()
*/
void CT_BluetoothSynchronousLinkData::DoCmdCoding(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Coding Call"));
	
	TUint16 voiceSetting = iData->Coding();
	INFO_PRINTF2(_L("CBluetoothSynchronousLink Coding result: actual = %d"), voiceSetting);
	
	TInt	expected;
	if(GetIntFromConfig(aSection, KExpected(), expected))
		{
		if( expected != voiceSetting )
			{
			ERR_PRINTF1(_L("Coding not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test SetMaxBandwidth()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetMaxBandwidth(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetMaxBandwidth Call"));
	
	TPtrC	bandwidthName;
	if(GetStringFromConfig(aSection, KSyncBandwidth(), bandwidthName))
		{
		TBTSyncBandwidth*	tbtSyncBandwidth = static_cast<TBTSyncBandwidth*>(GetDataObjectL(bandwidthName));
		iData->SetMaxBandwidth(*tbtSyncBandwidth);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KSyncBandwidth());
		SetBlockResult(EFail);
		}
	}


/**
Test Bandwidth()
*/
void CT_BluetoothSynchronousLinkData::DoCmdBandwidth()
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Bandwidth Call"));
	
	TBTSyncBandwidth bandwidth = iData->Bandwidth();
	
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Bandwidth result"));
	INFO_PRINTF2(_L("iTransmit = %d"), bandwidth.iTransmit);
	INFO_PRINTF2(_L("iReceive = %d"), bandwidth.iReceive);
	}


/**
Test SetMaxLatency()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetMaxLatency(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetMaxLatency Call"));
	
	TInt	latency = 0;
	if(GetIntFromConfig(aSection, KLatency(), latency))
		{
		iData->SetMaxLatency((TUint16)latency);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KLatency());
		SetBlockResult(EFail);
		}
	}


/**
Test Latency()
*/
void CT_BluetoothSynchronousLinkData::DoCmdLatency(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink Latency Call"));
	
	TUint16 latency = iData->Latency();
	INFO_PRINTF2(_L("CBluetoothSynchronousLink Latency result: actual = %d"), latency);
	
	TInt	expected;
	if(GetIntFromConfig(aSection, KExpected(), expected))
		{
		if( expected != latency )
			{
			ERR_PRINTF1(_L("Latency not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test SetRetransmissionEffort()
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetRetransmissionEffort(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetRetransmissionEffort Call"));
	
	TBTeSCORetransmissionTypes	retransmissionType;
	if ( CT_BTUtil::ReadTBTeSCORetransmissionTypes(*this, retransmissionType, aSection, KRetransmissionType()) )
		{
		INFO_PRINTF2(_L("CBluetoothSynchronousLink SetRetransmissionEffort: retransmissionType = %d"), retransmissionType);
		iData->SetRetransmissionEffort(retransmissionType);
		}
	else 
		{
		ERR_PRINTF1(_L("Missing parameter, TBTeSCORetransmissionTypes"));
		SetBlockResult(EFail);
		}
	}


/**
Test RetransmissionEffort()
*/
void CT_BluetoothSynchronousLinkData::DoCmdRetransmissionEffort(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink RetransmissionEffort Call"));
	
	TUint8 retransmissionType = iData->RetransmissionEffort();
	INFO_PRINTF2(_L("CBluetoothSynchronousLink RetransmissionEffort result: actual = %d"), retransmissionType);

	TBTeSCORetransmissionTypes	expected;
	if ( CT_BTUtil::ReadTBTeSCORetransmissionTypes(*this, expected, aSection, KExpected()) )
		{
		if( expected != retransmissionType )
			{
			ERR_PRINTF1(_L("RetransmissionEffort not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else 
		{
		ERR_PRINTF1(_L("Missing parameter, TBTeSCORetransmissionTypes"));
		SetBlockResult(EFail);		
		}
	}


/**
Test RemoteName()
*/
void CT_BluetoothSynchronousLinkData::DoCmdRemoteName(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink RemoteName Call"));
	
	T_TBTSockAddrChild addr;
	iData->RemoteName(addr);
	
	TBTDevAddr currentAddr = addr.BTAddr();
	TBuf<KMaxSockAddrSize> tmpBuf;
	currentAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("RemoteName() = 0x%S"),&tmpBuf);

	TPtrC	myTBTDevAddrName;
	if(GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
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
		ERR_PRINTF2(KLogMissingParameter, &KTBTDevAddr());
		SetBlockResult(EFail);
		}
	}


/**
Test SetNotifier
*/
void CT_BluetoothSynchronousLinkData::DoCmdSetNotifier(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothSynchronousLink SetNotifier Call"));
	
	MBluetoothSynchronousLinkNotifier* theNotifier = this;
	TBool	useAlternate =EFalse;
	if(GetBoolFromConfig(aSection, KUseAlternateNotifier(), useAlternate) )
		{
		if (useAlternate)
			{
			theNotifier=this;
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KUseAlternateNotifier());
		SetBlockResult(EFail);
		}
	iData->SetNotifier(*theNotifier);
	}


/**
Test Destructor
*/
void CT_BluetoothSynchronousLinkData::DoCmdDestructor()
	{
	DestroyData();
	}


/**
Test MBSLN_ExtensionInterfaceL()
*/
void CT_BluetoothSynchronousLinkData::DoCmdMBSLN_ExtensionInterfaceL(const TDesC& aSection)
	{
	TInt tmp = 0;
	void* theExtensionObjectPtr;

	if(GetIntFromConfig(aSection, KUid(), tmp))
		{
		TUid uid=TUid::Uid(tmp);
		
		TBool expectNull = EFalse;
		if(GetBoolFromConfig(aSection, KExpectNull(), expectNull))
			{
			TRAPD(err, this->MBSLN_ExtensionInterfaceL(uid, theExtensionObjectPtr));
			if( err!=KErrNone )
				{
				ERR_PRINTF2(_L("MBSLN_ExtensionInterfaceL err=%d"),err);
				SetError(err);
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
				if (theExtensionObjectPtr==NULL)
					{
					ERR_PRINTF1(_L("Expected object but found NULL"));
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(KLogMissingParameter, &KExpectNull());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KUid());
		SetBlockResult(EFail);
		}
	}

