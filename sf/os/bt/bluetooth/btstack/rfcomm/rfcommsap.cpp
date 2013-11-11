// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <es_prot.h>
#include <bt_sock.h>
#include "rfcommsap.h"
#include "rfcommconsts.h"
#include "rfcommstates.h"
#include "rfcomm.h"
#include "rfcommmuxer.h"
#include "rfcommflow.h"
#include "AsyncErrorKicker.h"
#include "IncomingConnListener.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

CRfcommSAP* CRfcommSAP::NewL(CRfcommProtocol& aProtocol)
	{
	CRfcommSAP* sap= new (ELeave) CRfcommSAP(aProtocol);
	CleanupStack::PushL(sap);
	sap->ConstructL();
	CleanupStack::Pop();
	return sap;
	}

CRfcommSAP::CRfcommSAP(CRfcommProtocol& aProtocol)
	: CBluetoothSAP(aProtocol.SecMan(), aProtocol.CodMan()),
	  iClearToSend(ETrue),
	  iClearToRecv(ETrue),
	  iSendBlocked(EFalse),
	  #ifdef _DEBUG
	  iLocalCreditsSupplied(0),
	  iLocalCreditsUsed(0),
	  iProxyForRemoteCreditsSupplied(0),
	  iProxyForRemoteCreditsUsed(0),
	  #endif
	  iSignals(KInitialModemStatus),
	  iProtocol(aProtocol)
	{
	iState=iProtocol.StateFactory()->GetState(CRfcommStateFactory::EClosed);

	// set default Port Parameters as per 07.10
	iLocalPortParams.SetBitRate(EBps9600);
	iLocalPortParams.SetDataBits(EData8);
	iLocalPortParams.SetStopBit(EStop1);
	iLocalPortParams.SetParity(EParityNone);
	iLocalPortParams.SetFlowCtrl(0);
	iLocalPortParams.SetXOnChar(0x11);	//DC1
	iLocalPortParams.SetXOffChar(0x13);	//DC3
	}

void CRfcommSAP::ConstructL()
	/**
	   Construct the SAP
	**/
	{
	CBluetoothSAP::ConstructL();
	TCallBack nnd(NotifyNewDataCallback,this);
	iNotifyNewDataCallback=new (ELeave) CAsyncCallBack(nnd, ECAsyncDeferredPriority);

	iErrorKicker=new (ELeave) CAsyncErrorKicker(*this, ECAsyncImmediatePriority);
	}

CRfcommSAP::~CRfcommSAP()
	{
	//	Assert that we aren't in the Listening state
	__ASSERT_DEBUG(iState!=iProtocol.StateFactory()->
		GetState(CRfcommStateFactory::EListening), Panic(ERfcommBadStateForDestruction));
	LOG1(_L("RFCOMM: ~CRfcommSAP 0x%08x"), this);
	delete iNotifyNewDataCallback;
	delete iErrorKicker;
	
	CancelAccessRequest();
	
	if (iProtocol.IsListening())
		iProtocol.Listener().CancelIoctl(*this);
	
	if(iProtocol.SAPIsBound(*this))
		{
		iProtocol.RemoveBoundSAP(*this);
		}
 
	if(iMux)
		{
		iMux->DetachSAP(*this);
		}
	}
	
// CServProviderBase functions
void CRfcommSAP::Start()
	{
	iState->Start(*this);
	}

void CRfcommSAP::LocalName(TSockAddr& aAddr) const
	/**
	Return the BT address of local host controller together with
	the channel ID and security provided by user at 'Bind'.
	@param holder for returned data .
	@see SetLocalName
	*/
	{
	// Construct with supplied address: future proof against additions to 
	// TRfcommSockAddr, also to balance with parallel functions in L2Cap.
	TRfcommSockAddr addr(aAddr);
	// Copy in BT address of local host controller
	addr.SetBTAddr(iProtocol.LocalBTAddr());
	// Copy in channel ID and security provided by user at 'Bind'
	addr.SetPort(iServerChannel);
	addr.SetSecurity(iSecurity);
	aAddr=addr;   // Convert back 
	}

TInt CRfcommSAP::SetLocalName(TSockAddr& aAddr)
	/**
	Set the local address (port and security).
	(the port is specified in terms of server channels)
	@param contains server channel and security
	*/
	{
	TRfcommSockAddr rfcommAddr = TRfcommSockAddr::Cast(aAddr);
	TUint port = rfcommAddr.Port();
	TInt err = KErrNone;

	if (port == KRfcommPassiveAutoBind)
		{
		// user wishes for us to choose free ServerChannel
		err = PassiveAutoBind(rfcommAddr);
		}
	else
		{
		// explicit server channel provided
		if(IsValidServerChannel(port))
			{
			if (iProtocol.FindBoundSAP(rfcommAddr) ||
				iProtocol.FindIdleSAP(rfcommAddr) ||
				iProtocol.FindInboundConnectedSAP(rfcommAddr))
				{
				// not available!
				err = KErrInUse;
				}
			}
		else
			{
			// user provided bogus server channel
			err = KErrRfcommBadAddress;
			}
		}

	if (err == KErrNone)
		{ 
		// everything OK here, so store details
		iServerChannel=static_cast<TUint8>(rfcommAddr.Port());
		iSecurity = rfcommAddr.BTSecurity();
		iProtocol.AddBoundSAP(*this);
		}
	return err;
	}


TInt CRfcommSAP::PassiveAutoBind(TRfcommSockAddr& aAddress)
	{
// provide the next free server channel as local port
// ESOCK doesn't itself support Autobind for passive connections!
	TInt result = iProtocol.FindFreeServerChannel();

	if (result >= KMinRfcommServerChannel)
		{
		aAddress.SetPort(result);
		result = KErrNone;
		}

	else
		{
		// error - none left: convert to a more meaningful error
		// can't change error in FindFreeServerChannel for BC reasons
		result = KErrRfcommNoMoreServerChannels;
		}

	return result;
	}
		

void CRfcommSAP::RemName(TSockAddr& aAddr) const
	/**
	Return the ServerChannel/DLCI value and remote device address,
	provided by user at 'Connect' 
   	@param holder for returned data
	@see SetRemName()
	*/
	{
	// Construct with supplied address: future proof against additions to 
	// TRfcommSockAddr, also to balance with parallel functions in L2Cap.
	TRfcommSockAddr addr(aAddr);
	addr.SetBTAddr(iRemoteDev);
	addr.SetPort(iServerChannel);
	addr.SetSecurity(iSecurity);
	aAddr=addr;
	}


TInt CRfcommSAP::SetRemName(TSockAddr& aAddr)
	/**
	Set the remote device address and ServerChannel.
	@param contains values required
	*/
	{
	TRfcommSockAddr rfcommAddr = TRfcommSockAddr::Cast(aAddr);

	if(IsValidServerChannel(rfcommAddr.Port()))
		{
		iServerChannel=static_cast<TUint8>(rfcommAddr.Port());
		iRemoteDev=rfcommAddr.BTAddr();
		iSecurity = rfcommAddr.BTSecurity();
		return KErrNone;
		}
	else
		{
		return KErrRfcommBadAddress;
		}
	}


TInt CRfcommSAP::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
	// Get an option for user
	{
	LOG(_L("RFCOMM [rfcommsap.cpp]: CRfcommSAP::GetOption"));

	// Make sure it's for us
	if (aLevel!=KSolBtRFCOMM) 
		{
		//	What we do next depends on whether we have a muxer...
		if(iMux)
			{
			// Pass it down to our muxer
			return iMux->GetOption(aLevel, aName, aOption);
			}
		else
			{
			//	If we have no muxer, we can't do anything here.
			return KErrNotSupported;
			}
		}

	switch (aName)
		{
		case KRFCOMMLocalPortParameter: 
			{
			// Don't care what state RFCOMM is in, just send back 
			// our local settings
			TPckgBuf<TRfcommRemotePortParams> pckg(iLocalPortParams);
			aOption=pckg;
//			aOption = TPtr8((TUint8*)(&iLocalPortParams), 
//							sizeof(TRfcommRemotePortParams));
			return KErrNone;
			}
		case KRFCOMMGetAvailableServerChannel:
	 		{
 			TPckgBuf<TInt> intpckg(iProtocol.FindFreeServerChannel());
 			aOption=intpckg;
 			return KErrNone;
 			}
		case KRFCOMMGetDebug1:
			{
 			TPckgBuf<TInt> intpckg((TInt)(static_cast<SBtTls*>(Dll::Tls()))->iPort);
 			aOption=intpckg;
 			return KErrNone;
			}
		case KRFCOMMMaximumSupportedMTU:
    		{
			TPckgBuf<TUint16> intpckg(iUserDefinedMTU);
			aOption=intpckg;
 			return KErrNone;
			}
		case KRFCOMMGetRemoteModemStatus:
			{
			TPckgBuf<TUint8> intpckg(iRemoteModemStatus);
			aOption=intpckg;
			return KErrNone;
			}
		case KRFCOMMGetReceiveCredit:
			{
			TPckgBuf<TInt> intpckg(ProxyForRemoteCredit());
			aOption=intpckg;
			return KErrNone;
			}
		case KRFCOMMGetTransmitCredit:
			{
			TPckgBuf<TInt> intpckg(LocalCredit());
			aOption=intpckg;
			return KErrNone;
			}
		case KRFCOMMGetReUsedCount:
			{
			TInt value;
			if(!NegotiatedMTU())
				value = -1; //-1 indicates use of get option before meaningful
			else
				value = FreeCreditCalculation();
			TPckgBuf<TInt> intpckg(value);
			aOption=intpckg;
			return KErrNone;
			}
		case KRFCOMMFlowTypeCBFC:
			{
			if (iMux)
				{
				TPckgBuf<CRfcommFlowStrategyFactory::TFlowStrategies> intpckg(iMux->FlowStrategy()->FlowType());
				aOption=intpckg;
				return KErrNone;
				}
			else
				{
				return KErrDisconnected;
				}
			}
		case KRFCOMMLocalModemStatus:
			{
			TPckgBuf<TUint8> intpckg(iSignals);
			aOption=intpckg;
			return KErrNone;
			}
		case KBTSecurityDeviceOverride:
			{
			// aspectoriented programming would be nice :-)
			return GetDeviceOverride(aOption); 
			}
		case KRFCOMMMaximumMTU:
			{
			if (iMux)
				{
				// Allow for possible credit in header, 1 byte
				TPckgBuf<TUint> intpckg(iOptimalMTUForSending - 1);
				aOption=intpckg;
 				return KErrNone;
		 		}
			else
				{
				return KErrDisconnected;
				}	
			}
		default:
			return KErrNotSupported;
		}
	}


TInt CRfcommSAP::SAPSetOption(TUint aLevel, TUint aName, const TDesC8 &aOption)
	// Set option from user
	{
	return iState->SetOption(*this, aLevel, aName, aOption);
	}

void CRfcommSAP::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
/**
	   Perform an ioctl.

	   If it is not for RFCOMM it is simply pushed down to the
	   layer below. If it is for RFCOMM and it is valid, it is handled in the state machine
	   If it is not valid, an error occurs.
**/
	{
	__ASSERT_DEBUG(iIoctlLevel == 0, Panic(ERfcommSAPTwoIoctls));
	__ASSERT_DEBUG(iIoctlName == 0, Panic(ERfcommSAPTwoIoctls));

	iIoctlLevel=aLevel;
	iIoctlName=aName;

	if (aLevel == KSolBtRFCOMM)
		iState->Ioctl(*this, aLevel, aName, aOption);
	else
		{
		if (!iMux && iProtocol.IsListening())
			iProtocol.Listener().Ioctl(*this, aLevel, aName, aOption);
		else if (iMux)
			iMux->Ioctl(aLevel, aName, aOption);
		else
			{
			iSocket->Error(KErrNotReady, MSocketNotify::EErrorIoctl);
			}
			
		}
	}

void CRfcommSAP::CancelIoctl(TUint aLevel,TUint aName)
	{
	iState->CancelIoctl(*this, aLevel, aName);
	//Cancel the Ioctl which has gone into muxer 
	if(aLevel != KSolBtRFCOMM && iMux) 
		{ 
		iMux->CancelIoctl(aLevel, aName); 
		} 
	}

TUint CRfcommSAP::Write(const TDesC8& aDesc,TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	return iState->Send(*this, aDesc);
	}

void CRfcommSAP::GetData(TDes8& aDesc,TUint /*aOptions*/,TSockAddr* /*aAddr*/)
	{
	iState->Read(*this, aDesc);
	}

void CRfcommSAP::ActiveOpen()
	/**
	   Request the SAP to open a connection.
	**/
	{
	iState->ActiveOpen(*this);
	}

void CRfcommSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	LOG(_L("CRfcommSAP::ActiveOpen - with data called"));
	Panic(ERfcommConnectWithData);
	}

TInt CRfcommSAP::PassiveOpen(TUint aQueSize)
	{
	return iState->PassiveOpen(*this, aQueSize);
	}

TInt CRfcommSAP::PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/)
	{
	return KErrNotSupported;
	}

void CRfcommSAP::Shutdown(TCloseType aOption)
	{
	if(aOption!=EImmediate)
		{
		// Normal closedown, ESOCK will wait for us to say we can end
		LOG1(_L("RFCOMM: Shutdown, sap %08x"), this);
		iState->Shutdown(*this);
		}
	else
		{
		LOG1(_L("RFCOMM: Fast Shutdown, sap %08x"), this);
		iState->FastShutdown(*this);
		}
	}

void CRfcommSAP::Shutdown(TCloseType /*aOption*/,const TDesC8& /*aDisconnectionData*/)
	{
	LOG(_L("CRfcommSAP::Shutdown - with data"));
	Panic(ERfcommDisconnectDataNotSupported);
	}

void CRfcommSAP::AutoBind()
	/**
	   Pick an appropriate local port.
	   This is a no-op for RFCOMM since the local and remote ports
	   are the same.
	**/
	{
	}

void CRfcommSAP::Error(TInt aErrorCode, TErrorTypes aType)
	{
	iState->Error(*this, aErrorCode, aType);
	}

TUint16 CRfcommSAP::NegotiatedMTU() const
	{
	return iNegotiatedMTU;
	}

TUint16 CRfcommSAP::OptimalMTUForSending() const
	{
	return iOptimalMTUForSending;
	}


TUint16 CRfcommSAP::UsableMTU(TUint8 aCredit) const
	{
	return iMux->FlowStrategy()->UsableMTU(*this, aCredit);
	}

/*
  Query and util functions
*/

TUint8 CRfcommSAP::DLCI() const
	{
	return iDLCI;
	}

TUint16 CRfcommSAP::MaximumMTU() const
	/**
	Calculates the maximum MTU that we can possibly allow in terms of the underlying 
	L2CAP SAP MTU, and any user defined upper limit on the MTU.

	This is not a misnaming - the SAPs actual MTU might be negotiated down from 
	the Maximum Maximum Transfer Unit!
	**/
	{
	TInt frameSize=iMux->GetMaxDataSize();
	//	size is currently the raw L2CAP MTU at this point.
	//	Adjust it to allow for the header and tail fields.
	frameSize-=(frameSize>KMaxL2CAPMTUFor1LengthByte)?
		(KLongFrameHeaderLength+KFCSFieldLength):
		(KShortFrameHeaderLength+KFCSFieldLength);

	//Check the RFComm MTU negotiated in PN commands is
	//less than the TS 07.10 defined maximum.
	frameSize = (KRfcommMaxMTU<frameSize)?KRfcommMaxMTU:frameSize;
	
	if(iUserDefinedMTU)	//	i.e. not = 0
		frameSize=(iUserDefinedMTU<frameSize)?iUserDefinedMTU:frameSize;
	
	__ASSERT_ALWAYS(frameSize>0 && frameSize<=KRfcommMaxMTU,Panic(ERfcommBadCalculatedMTU));
	return STATIC_CAST(TUint16,frameSize);
	}

TRfcommChannel CRfcommSAP::ServerChannel() const
	{
	return iServerChannel;
	}

TBool CRfcommSAP::ServerChannelValid() const
	/**
	   Checks the ServerChannel for validity.
	   The DLCI must be in the range 2-30.
	**/
	{
	return IsValidServerChannel(iServerChannel);
	}

TBool CRfcommSAP::IsValidServerChannel(TInt aChan) const
	{
	return (aChan >= KMinRfcommServerChannel && aChan <= KMaxRfcommServerChannel);
	}

CRfcommMuxer* CRfcommSAP::Mux()
	{
	return iMux;
	}

CCirBuffer& CRfcommSAP::DataBuffer()
	{
	return iDataBuffer;
	}

TInt CRfcommSAP::UnusedBufferSpace() const
	{
	return iDataBuffer.Length() - iDataBuffer.Count();
	}

TInt CRfcommSAP::NotifyNewDataCallback(TAny* aSAP)
	{
	CRfcommSAP& sap = *static_cast<CRfcommSAP*>(aSAP);
	sap.iState->NotifyNewDataCallback(sap);
	return EFalse;
	}

TBool CRfcommSAP::IsCloned()
	{
	return iCloned;
	}
	
CRfcommSAP* CRfcommSAP::CloneMe()
	/**
	Provides a cloning service.
	
	The return value is either NULL (when the clone could not be created for lack of
	memory, or hitting the limitations set on the maximum number of child clones waiting
	for start as set in PassiveOpen aQue) or the address of a new SAP.
	**/
	{
	__ASSERT_DEBUG(iState==iProtocol.StateFactory()->
		GetState(CRfcommStateFactory::EListening), Panic(ERfcommBadStateForCloning));
	CRfcommSAP* clone = NULL;
	
	if(iClonedChildren.Count() + iClonesWaitingForStart>=iMaxClonesWaitingForStart)
		return NULL;	//	We want the listening queue to hold at iMaxClonesWaitingForStart.

	TRAPD(err, clone = NewL(iProtocol));
	if(err != KErrNone)
		return NULL;

	//  Mark new SAP as cloned
	clone->iCloned = ETrue;
	
	//	Ensure the clone knows it's parent
	clone->iParentSAP=this;

	//	Ensure the parent doesn't forget its clone
	iClonedChildren.Append(clone);	

	clone->SetCodServiceBits(iCodServiceBits);
	
	return clone;
	}

void CRfcommSAP::ChildConnected(CRfcommSAP& aRfcommSAP)
	/**
	Called by cloned child SAPs on their parent when they get hit by an incoming SABM.
	
	This is a cue for the parent (listening) SAP to increment iClonesWaitingForStart 
	and to call ConnectComplete on their MSocketNotify. At this point, the cloned child 
	SAP should also be removed from the Parent SAPs list of children.
	**/
	{
	TInt index=iClonedChildren.Find(&aRfcommSAP);
	if(index<KErrNone)
		{
		//	The child was not found!
		Panic(ERfcommUnknownChildConnectionNotified);
		}

	iClonedChildren.Remove(index);	//	We no longer have ownership of this SAP
	iClonesWaitingForStart++;
	iSocket->ConnectComplete(aRfcommSAP);
	}

void CRfcommSAP::ChildConnectFailed(CRfcommSAP& aRfcommSAP)
	/**
	One of our cloned child SAPs has failed to connect.

	This could be because of a RFCOMM shutdown of the connection
	before it completes, or because of a security check failure.
	Either way, We can delete the SAP with no ill effects since ESOCK
	has no knowledge of the clone.
	**/
	{
	TInt index=iClonedChildren.Find(&aRfcommSAP);

	__ASSERT_DEBUG(index>=KErrNone, Panic(ERfcommUnknownChildSecurityFailed)); //child not found!
	
	delete iClonedChildren[index];
	iClonedChildren.Remove(index);
	}

void CRfcommSAP::ChildStarted(CRfcommSAP& /*aRfcommSAP*/)
	/**
	Notification from a child that they have received a Start.
	**/
	{
	iClonesWaitingForStart--;
	}

void CRfcommSAP::AccessRequestComplete(TInt aResult)
	/**
	Callback from the BTSecurityRequester with result of security check
	**/
	{
	iState->AccessRequestComplete(*this, aResult);
	}

void CRfcommSAP::ParentClosed()
	/**
	Notification from our parent that it has been closed.
	**/
	{
	iState->ParentClosed(*this);
	}


TBool CRfcommSAP::ListeningTo(const TBTSockAddr& aBTSockAddr) const
	/**
	Is the current SAP listening for connections from the specified address?

	When called on a given SAP, this function returns ETrue if;
	a) The SAP is listening
	b) The SAP is listening for connections incoming from the given Server Channel
	**/
	{
	return	( iState==iProtocol.StateFactory()->GetState(CRfcommStateFactory::EListening) &&
			  aBTSockAddr.Port()==iServerChannel );
	}

TBool CRfcommSAP::BoundTo(const TBTSockAddr& aBTSockAddr) const
	{
	return aBTSockAddr.Port()==iServerChannel;
	}

void CRfcommSAP::StopListening()
	/**
	Called on destruction, or on Shutdown or FastShutdown of a listening SAP.

	Removes this SAP from the Protocol's list of listening SAPs, and also informs all
	the cloned child SAPs of their parent's imminent destruction.
	**/
	{
	TInt index=0;
	for(;index<iClonedChildren.Count();index++)
		{
		iClonedChildren[index]->ParentClosed();
		}

	iClonedChildren.ResetAndDestroy();
	iClonedChildren.Close();
	iProtocol.DecrementListeners();
	iProtocol.RemoveIdleSAP(*this);	
	}

/*
  Events coming in from the muxer/protocol
*/
  

void CRfcommSAP::MuxUp()
	/**
	   Notification that a mux has been started.
	**/
	{
	iState->MuxUp(*this);
	}

void CRfcommSAP::LinkDown()
	/**
	   Notification that the link has gone down.
	**/
	{
	iState->LinkDown(*this);
	}

void CRfcommSAP::DISC()
	/**
	   A DISC frame has come in for this SAP
	**/
	{
	iState->DISC(*this);
	}

void CRfcommSAP::UA()
	/**
	   A UA frame has come in for this SAP
	**/
	{
	iState->UA(*this);
	}

void CRfcommSAP::DM()
	/**
	   A DM frame has come in for this SAP
	**/
	{
	iState->DM(*this);
	}


void CRfcommSAP::RPN(const TRfcommRPNTransaction* aRPNTransactionPtr, CRfcommMuxer& aMuxer, TUint8 aDLCI)
	/**
		An RPN command has come in from the remote end for this SAP

		This is either:
		1) A negotiation command or
		2) A request for our local port parameters
	**/
	{
	// If there's transaction data it be either a negotiation or a request(query) (having been through state machine)
 	if(aRPNTransactionPtr)
		{
		LOG(_L("RPN command received - Passing onto RFCOMM state machine\n"));
		iState->RPN(*this, *aRPNTransactionPtr, aMuxer, aDLCI);
		return;
		}
	
	// Otherwise it's a request (query)
	LOG(_L("RPN Request received - Sending our local settings\n"));
	
	TRfcommRPNTransaction localRPNTransaction;
	// Copy the local port params into the transaction structure
	localRPNTransaction.iPortParams = iLocalPortParams;

	// Set the parameter mask to whatever is valid in the port params
	TUint16& paramMask = localRPNTransaction.iParamMask;
	paramMask = 0;
	
	if(iLocalPortParams.IsValid() & EVMBitRate)
		paramMask |= EPMBitRate;
		
	if(iLocalPortParams.IsValid() & EVMDataBits)
		paramMask |= EPMDataBits;

	if(iLocalPortParams.IsValid() & EVMStopBit)
		paramMask |= EPMStopBit;

	if(iLocalPortParams.IsValid() & EVMParity)
		{
		paramMask |= EPMParity;
		paramMask |= EPMParityType;
		}
	
	if(iLocalPortParams.IsValid() & EVMFlowCtrl)
		{
		// Not sure which *parts* of Flow control are valid 
		//  - assume all of it is
		paramMask |= EPMXOnOffInput;
		paramMask |= EPMXOnOffOutput;
		paramMask |= EPMRTRInput;
		paramMask |= EPMRTROutput;
		paramMask |= EPMRTCInput;
		paramMask |= EPMRTCOutput;
		}

	if(iLocalPortParams.IsValid() & EVMXOnChar)
		paramMask |= EPMXOnChar;
	
	if(iLocalPortParams.IsValid() & EVMXOffChar)
		paramMask |= EPMXOffChar;
	
	//Call the state machine to process this RPN, including local transaction
	iState->RPN(*this, localRPNTransaction, aMuxer, aDLCI);

	}

void CRfcommSAP::RPNRsp(const TRfcommRPNTransaction& aRPNTransaction)
	/**
	   A response to a RPN command has come in for this SAP
	**/
	{
	// Depends on what state we're in so pass it on
	iState->RPNRsp(*this, aRPNTransaction);
	}

void CRfcommSAP::PNResp(TRfcommPortParams& aParams)
	/**
	   A response to a PN command has come in for this SAP
	**/
	{
	iState->PNResp(*this, aParams);
	}

void CRfcommSAP::MSC(TUint8 aSignals)
	/**
	   A MSC command or response has come in for this SAP
	   from the remote end
	**/
	{
	iState->MSC(*this, aSignals);
	}

void CRfcommSAP::RLS(TUint8 aStatus)
	/**
	   A RLS command or response has come in for this SAP
	   from the remote end
	**/
	{
	iState->RLS(*this, aStatus);
	}

void CRfcommSAP::Data(const TDesC8& aData)
	/**
	   Data has arrived for this SAP
	**/
	{
	LOG1(_L("RFCOMM: Reading 0x%x"), aData[0]);
	iState->NewData(*this, aData);
	}

void CRfcommSAP::CanSend()
	/**
	   Clear to send from the mux
	**/
	{
	iState->CanSend(*this);
	}

//TRY_CBFC
void CRfcommSAP::DisallowCBFC()
	{
	iProtocol.DisallowCBFC();
	}

void CRfcommSAP::AllowCBFC()
	{
	iProtocol.AllowCBFC();
	}

TUint8 CRfcommSAP::FreeCredit()
	{
	return iState->FreeCredit(*this);
	}

TUint8 CRfcommSAP::FreeCreditCalculation() const
	{
	__ASSERT_DEBUG(NegotiatedMTU(), Panic(ERfcommBadCalculatedMTU));
	TInt value = UnusedBufferSpace()/NegotiatedMTU() - iProxyForRemoteCredit;
	#ifdef _DEBUG
	TInt divResult = 0; //two lines here to avoid compiler warning
	divResult = UnusedBufferSpace()/NegotiatedMTU();

	LOG(_L("RFCOMM: Calculating the maximum number of extra credits we can send to the remote ....."));
	LOG6(_L("RFCOMM: ..... %d/%d - %d = %d - %d = %d"), 
									UnusedBufferSpace(),
									NegotiatedMTU(),
									iProxyForRemoteCredit,
									divResult, 
									iProxyForRemoteCredit, 
									value);
	#endif
	value = Max(value, 0); //ignore negative values...FIXME
	return STATIC_CAST(TUint8, Min(value, KMaxTUint8));
	}

TInt CRfcommSAP::ProxyForRemoteCredit() const
	{
	return iState->ProxyForRemoteCredit(*this);
	}

void CRfcommSAP::SetProxyForRemoteCredit(TInt aCredit)
	{
	iState->SetProxyForRemoteCredit(*this, aCredit);
	}

void CRfcommSAP::ProxyForRemoteCreditDecrement()
	{
	iState->ProxyForRemoteCreditDecrement(*this);
	}

void CRfcommSAP::ProxyForRemoteCreditAddCredit(TUint8 aCredit)
	{
	iState->ProxyForRemoteCreditAddCredit(*this, aCredit);
	}

TInt CRfcommSAP::LocalCredit() const
	{
	return iState->LocalCredit(*this);
	}

void CRfcommSAP::SetInitialLocalCredit(TInt aCredit)
	{
	iState->SetInitialLocalCredit(*this, aCredit);
	}
void CRfcommSAP::LocalCreditDecrement()
	{
	iState->LocalCreditDecrement(*this);
	}

void CRfcommSAP::LocalCreditAddCredit(TUint8 aCredit)
	{
	iState->LocalCreditAddCredit(*this, aCredit);
	}



TBool CRfcommSAP::HandleFrameResponseTimeout()
	/**
	There has been a timeout while waiting for a response to a previously sent frame.
	**/
	{
	return iState->HandleFrameResponseTimeout(*this);
	}

void CRfcommSAP::IoctlComplete(TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf)
	{
	if (iIoctlLevel!=aLevel || iIoctlName!=aName)
		return;	// wasn't for us

	if (iIoctlLevel == KSolBtRFCOMM)
		iState->IoctlComplete(*this, aErr, aLevel, aName, aBuf); // the state machine is the best place to find out what to do
	else
		{
		iIoctlLevel = 0;
		iIoctlName = 0;

		if (aErr==KErrNone)
			iSocket->IoctlComplete(aBuf);
		else if(iSocket)
			iSocket->Error(aErr, MSocketNotify::EErrorIoctl);
		}
	}

void CRfcommSAP::PN(TRfcommPortParams& aParams, CRfcommMuxer& aMuxer, TUint8 aDLCI)
	{
	iState->PN(*this, aParams, aMuxer, aDLCI);
	}

void CRfcommSAP::SABM(CRfcommMuxer& aMuxer, TUint8 aDLCI)
	{
	iState->SABM(*this, aMuxer, aDLCI);
	}

