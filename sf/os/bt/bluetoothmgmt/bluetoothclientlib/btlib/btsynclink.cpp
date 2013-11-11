// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic functions associated with all Bluetooth socket addresses
// 
//

#include "bt_sock.h"
#include "internaltypes.h"
#include "btsocketpanic.h"
#include "btsynclinkhelpers.h"
#include <bluetooth/logger.h>


EXPORT_C TBTSyncPackets::TBTSyncPackets(TBTSyncPacketTypes aPacketTypes)
	: iPacketTypes(aPacketTypes)
	{}


EXPORT_C TBTSyncPacketTypes TBTSyncPackets::operator()() const
	{
	return iPacketTypes;
	}

EXPORT_C TBTSyncBandwidth::TBTSyncBandwidth()
	{}

EXPORT_C TBTSyncBandwidth::TBTSyncBandwidth(TUint aBandwidth)
	: iTransmit(aBandwidth), iReceive(aBandwidth)
	{}

TBTeSCOLinkParams::TBTeSCOLinkParams(TUint aBandwidth, TUint16 aCoding, TUint16 aLatency, TUint8 aRetransmission)
	: iBandwidth(aBandwidth), iCoding(aCoding), iLatency(aLatency), iRetransmissionEffort(aRetransmission)
	{}

/** Allocate and open a socket sub-session for Bluetooth SCO

@param aNotifier Notifier object which will receive callbacks after asynchronous requests
@param aServer Handle to a currently connected session on the socket server
@return Newly allocated CBluetoothSynchronousLink object
@capability LocalServices
*/
EXPORT_C CBluetoothSynchronousLink* CBluetoothSynchronousLink::NewL(MBluetoothSynchronousLinkNotifier& aNotifier,
														RSocketServ& aServer)
	{
	CBluetoothSynchronousLink* self = NewLC(aNotifier, aServer);
	CleanupStack::Pop();
	return self;
	}


/**  Allocate and open a socket sub-session for Bluetooth SCO

@param aNotifier Notifier object which will receive callbacks after asynchronous requests
@param aServer Handle to a currently connected session on the socket server
@return Newly allocated CBluetoothSynchronousLink object
@capability LocalServices
*/
EXPORT_C CBluetoothSynchronousLink* CBluetoothSynchronousLink::NewLC(MBluetoothSynchronousLinkNotifier& aNotifier,
														 RSocketServ& aServer)
	{
	CBluetoothSynchronousLink* self=new (ELeave) CBluetoothSynchronousLink(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/**
Destructor
*/
EXPORT_C CBluetoothSynchronousLink::~CBluetoothSynchronousLink()
	{
	FTRACE(FPrint(_L("Destroying CBTSyncLink 0x%08x"), this));
	delete iBTSynchronousLinkSenderSCO;
	delete iBTSynchronousLinkSenderESCO;
	delete iBTSynchronousLinkReceiverSCO;
	delete iBTSynchronousLinkReceiverESCO;
	delete iBTSynchronousLinkAccepterSCO;
	delete iBTSynchronousLinkAccepterESCO;
	delete iBTSynchronousLinkAttacherSCO;
	delete iBTSynchronousLinkAttacherESCO;
	delete iBTSynchronousLinkDetacherSCO;
	delete iBTSynchronousLinkDetacherESCO;
	delete iBTSynchronousLinkBaseband;
	
	FLOG(_L("Closing sockets"));
	ListeningSCOSocket().Close();
	ListeningESCOSocket().Close();	
	SCOSocket().Close();
	ESCOSocket().Close();
	}


void CBluetoothSynchronousLink::ConstructL()
	{
	iBTSynchronousLinkSenderSCO = CBTSynchronousLinkSender::NewL(*this,ESCO);
	iBTSynchronousLinkSenderESCO = CBTSynchronousLinkSender::NewL(*this,EeSCO);
	iBTSynchronousLinkReceiverSCO = CBTSynchronousLinkReceiver::NewL(*this,ESCO);
	iBTSynchronousLinkReceiverESCO = CBTSynchronousLinkReceiver::NewL(*this,EeSCO);
	iBTSynchronousLinkAccepterSCO = CBTSynchronousLinkAccepter::NewL(*this,ESCO);
	iBTSynchronousLinkAccepterESCO = CBTSynchronousLinkAccepter::NewL(*this,EeSCO);
	iBTSynchronousLinkAttacherSCO = CBTSynchronousLinkAttacher::NewL(*this,ESCO);
	iBTSynchronousLinkAttacherESCO = CBTSynchronousLinkAttacher::NewL(*this,EeSCO);
	iBTSynchronousLinkDetacherSCO = CBTSynchronousLinkDetacher::NewL(*this,ESCO);
	iBTSynchronousLinkDetacherESCO = CBTSynchronousLinkDetacher::NewL(*this,EeSCO);
	iBTSynchronousLinkBaseband = CBTSynchronousLinkBaseband::NewL(*this);
	}


CBluetoothSynchronousLink::CBluetoothSynchronousLink(MBluetoothSynchronousLinkNotifier& aNotifier, 
										             RSocketServ& aServer)
	: iNotifier(&aNotifier), iSockServer(aServer),
	  iRequestedLink(KBTSync64KBit, 0, 25, EeSCORetransmitDontCare),
	  iNegotiatedLink(0, 0, 0, 0)
	{
	FTRACE(FPrint(_L("Constructing CBTSyncLink 0x%08x"), this));
	}


/**
Create a synchronous link (e.g. voice) on an EXISTING physical link.

@param aBDAddr Bluetooth address of remote device (specifying existing physical link)
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::SetupConnection(const TBTDevAddr& aBDAddr)
	{
	return SetupConnection(aBDAddr, TBTSyncPackets(TBTSyncPackets::ESyncAnySCOPacket));
	}
	
	
/**
Create a synchronous link (e.g. voice) on an EXISTING physical link.
@see TBTPacketType
@param aBDAddr Bluetooth address of remote device (specifying existing physical link)
@param aPacketTypes Bitmask of the SCO packet types required on this connection
Use only the three SCO specific packet type enums
E.g. EPacketsHV1|EPacketsHV2|EPacketsHV3 (EAnySCOPacket) specifies all the three packet
types are allowed.
OR if ESCO is supported
three ESCO specific packet type enums
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::SetupConnection(const TBTDevAddr& aBDAddr, const TUint16 aPacketTypes)
	{
	return SetupConnection(aBDAddr, TBTSyncPackets((aPacketTypes & EAnySCOPacket) >> KSCOvsSyncHVOffset));
	}


// This needs to be a macro or the 'return' won't return properly
#define CLOSE_RETURN_IF_ERROR(error) if (error) { LinkDown(); SCOSocket().Close(); ESCOSocket().Close(); return error; }

EXPORT_C TInt CBluetoothSynchronousLink::SetupConnection(const TBTDevAddr& aBDAddr, const TBTSyncPackets& aPacketTypes)
	{
	TBool openSCO = EFalse;
	TBool openESCO = EFalse;

	FTRACE(FPrint(_L("SetupConnection on CBTSyncLink 0x%08x"), this));

	TBTSyncPacketTypes packets = aPacketTypes();
	
	__ASSERT_ALWAYS(packets, Panic(EBadSyncPacketTypes));
	
	packets &= (TBTSyncPackets::ESyncAnySCOPacket | TBTSyncPackets::ESyncAnyESCOPacket);
	if (!packets)
		{
		FLOG(_L("Bad packets specified at initial mask"));
		return KErrNotSupported;
		}
		
	TBTSyncPacketTypes packetsSCO = packets & TBTSyncPackets::ESyncAnySCOPacket;
    if (packetsSCO)
        {
        iSCOTypes |= ESCO;
        openSCO = ETrue;
        }

	TBTSyncPacketTypes packetsESCO = packets & TBTSyncPackets::ESyncAnyESCOPacket;
	if (packetsESCO) 
		{
		iSCOTypes |= EeSCO;
		openESCO = ETrue;
		}
		
	// but must be one		
	__ASSERT_ALWAYS(packetsSCO || packetsESCO, Panic(EBadSyncPacketTypes));

	if (iBTSynchronousLinkAttacherSCO->IsActive() || iBTSynchronousLinkAttacherESCO->IsActive())
		{
		FLOG(_L("Link attacher already active"));
		return KErrInUse;
		}
		
	TInt linkState = LinkUp(aBDAddr);
	if (linkState != KErrNone)
		{
		FLOG(_L("Baseband link not available"));
		return KErrDisconnected;
		}
	
	TInt err = KErrNone;			   
	
	if (openSCO && openESCO)
		{
		/* open both socket types */
		/* first try ESCO */
		err = ESCOSocket().Open(iSockServer, 
						        KBTAddrFamily,
						        KSockBluetoothTypeESCO,
						        KBTLinkManager);							        
		if(!err)
			{
			err = ESCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packets);
			
			if (!err)
				{
				TPckgBuf<TBTeSCOLinkParams> options;
				options() = iRequestedLink;
				err = ESCOSocket().SetOpt(EeSCOExtOptions, KSolBtESCO, options);
				}
			}
		if (err)
			{	// ESCO failed so try only SCO
			ESCOSocket().Close();
			openESCO = EFalse;
			}
			
    	err = SCOSocket().Open(iSockServer, 
								   KBTAddrFamily,
							       KSockBluetoothTypeSCO,
							       KBTLinkManager);						       							    
		if(!err)
			{
	        err = SCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packetsSCO);
			}
		else
			{// SCO failed
			openSCO = EFalse;
			}		

		CLOSE_RETURN_IF_ERROR(err);	// this will sort out if both have failed
		}							    
	else if (openSCO)
        {
    	err = SCOSocket().Open(iSockServer, 
							   KBTAddrFamily,
						       KSockBluetoothTypeSCO,
						       KBTLinkManager);						       							    
		if(err)
			{
			return err;
			}
        err = SCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packetsSCO);
        CLOSE_RETURN_IF_ERROR(err);	
        }      
	else if (openESCO) 
		{
		err = ESCOSocket().Open(iSockServer, 
						        KBTAddrFamily,
						        KSockBluetoothTypeESCO,
						        KBTLinkManager);							        
		if(err)
			{
			SCOSocket().Close();
			return err;
			}					
		err = ESCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packetsESCO);
			
		TPckgBuf<TBTeSCOLinkParams> options;
		options() = iRequestedLink;
		err = ESCOSocket().SetOpt(EeSCOExtOptions, KSolBtESCO, options);
		CLOSE_RETURN_IF_ERROR(err);	
		}							    
							    
	iSockAddr.SetBTAddr(aBDAddr);

	iBTSynchronousLinkBaseband->PreventPark();
	
	if(openESCO)
		{
		iOpeningESCO = ETrue;
		iBTSynchronousLinkAttacherESCO->AttachSCOLink(iSockAddr);
		iBTSynchronousLinkBaseband->CatchEvents();
		}

	if(openSCO)
		{
		iOpeningSCO = ETrue;
		iBTSynchronousLinkAttacherSCO->AttachSCOLink(iSockAddr);
		iBTSynchronousLinkBaseband->CatchEvents();
		}

	return KErrNone;							     
	}


/**
Cancel creating a synchronous link.
This cancels the asynchronous request to prevent the callback reaching the client.
It makes no claims about the resultant state of the CBluetoothSynchronousLink.  If
a client wishes to continue using the CBluetoothSynchronousLink it is strongly 
recommended to follow a call to CancelSetup with a call to Disconnect, which will
ensure that any established connection is tidied up.
@see CBluetoothSynchronousLink::Disconnect
*/
EXPORT_C void CBluetoothSynchronousLink::CancelSetup()
	{
	iBTSynchronousLinkAttacherSCO->Cancel();
	iBTSynchronousLinkAttacherESCO->Cancel();
	iBTSynchronousLinkBaseband->StopAll();
	}


/**
Disconnect a synchronous link.

The physical link will remain unless no other services are running on it.
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::Disconnect()
	{
	if (!SCOSocket().SubSessionHandle() && !ESCOSocket().SubSessionHandle())
		{
		return KErrDisconnected;
		}

	if (iBTSynchronousLinkDetacherSCO->IsActive() || iBTSynchronousLinkDetacherESCO->IsActive())
		{
		return KErrInUse;
		}
	
	__ASSERT_ALWAYS(iSCOTypes, Panic(EBadSyncPacketTypes));
	if (iSCOTypes & ESCO)
		{
		iBTSynchronousLinkDetacherSCO->DetachSCOLink(iDummySCOShutdownDescriptor, iDummySCOShutdownDescriptor);
		}
	
	if (iSCOTypes & EeSCO)
		{
		iBTSynchronousLinkDetacherESCO->DetachSCOLink(iDummySCOShutdownDescriptor, iDummySCOShutdownDescriptor);
		}
	
	return KErrNone;
	}


/**
Send data over synchronous link.  It is worth noting that Bluetooth hardware
may have dedicated pins to supply synchronous data, and the RSocket interface
cannot provide the synchronous requirements of the SCO link.  As a result of
this, the CBluetoothSynchronousLink class may only provide the control plane of
a SCO connection.  In this situation, Receive returns KErrNotSupported via the
MBluetoothSynchronousLinkNotifier::HandleSendCompleteL callback.

@param const aData Data to be sent.
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::Send(const TDesC8& aData)
	{
	__ASSERT_ALWAYS(SCOSocket().SubSessionHandle() || ESCOSocket().SubSessionHandle(), Panic(EInvalidSubSession));
	if (!SCOSocket().SubSessionHandle())
		{
		if(!ESCOSocket().SubSessionHandle())
			{
			return KErrDisconnected;
			}
		}
	
	if (iBTSynchronousLinkSenderSCO->IsActive() ||
	    iBTSynchronousLinkSenderESCO->IsActive())
		{
		return KErrInUse;
		}

	__ASSERT_ALWAYS(iSCOTypes, Panic(EBadSyncPacketTypes));
	if (iSCOTypes & ESCO)
		{
		iBTSynchronousLinkSenderSCO->SendSCOData(aData);
		}
	
	if (iSCOTypes & EeSCO)
		{
		iBTSynchronousLinkSenderESCO->SendSCOData(aData);
		}

	return KErrNone;
	}


/**
Cancel sending data.
*/
EXPORT_C void CBluetoothSynchronousLink::CancelSend()
	{
	iBTSynchronousLinkSenderSCO->Cancel();
	iBTSynchronousLinkSenderESCO->Cancel();
	}


/**
Receive data over synchronous link.  It is worth noting that Bluetooth hardware
may have dedicated pins to supply synchronous data, and the RSocket interface
cannot provide the synchronous requirements of the SCO link.  As a result of
this, the CBluetoothSynchronousLink class may only provide the control plane of
a SCO connection.  In this situation, Receive returns KErrNotSupported via the
MBluetoothSynchronousLinkNotifier::HandleReceiveCompleteL callback.

@param const aData Buffer for data to be received.
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::Receive(TDes8& aData)
	{
	__ASSERT_ALWAYS(SCOSocket().SubSessionHandle() || ESCOSocket().SubSessionHandle(), Panic(EInvalidSubSession));
	if (!SCOSocket().SubSessionHandle())
		{
		if(!ESCOSocket().SubSessionHandle())
			{
			return KErrDisconnected;
			}
		}

	if (iBTSynchronousLinkSenderSCO->IsActive() ||
	    iBTSynchronousLinkSenderESCO->IsActive())
		{
		return KErrInUse;
		}

	__ASSERT_ALWAYS(iSCOTypes, Panic(EBadSyncPacketTypes));
	if (iSCOTypes & ESCO)
		{
		iBTSynchronousLinkReceiverSCO->ReadSCOData(aData);
		}
	
	if (iSCOTypes & EeSCO)
		{
		iBTSynchronousLinkReceiverESCO->ReadSCOData(aData);
		}

	return KErrNone;
	}


/**
Cancel receiving data.
*/
EXPORT_C void CBluetoothSynchronousLink::CancelReceive()
	{
	iBTSynchronousLinkReceiverSCO->Cancel();
	iBTSynchronousLinkReceiverESCO->Cancel();
	}


/**
Prepare for a remote device to set up a synchronous link on the local device.

Whilst this facility is set, a response can be sent to a remote device trying to 
set up a synchronous link, allowing that synchronous link to be brought up.
This object will represent that synchronous link locally when/if it does come up.
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::AcceptConnection()
	{
	return AcceptConnection(TBTSyncPackets(TBTSyncPackets::ESyncAnySCOPacket));
	}


/**
Prepare for a remote device to set up a synchronous link on the local device.

Whilst this facility is set, a response can be sent to a remote device trying to 
set up a synchronous link, allowing that synchronous link to be brought up.
This object will represent that synchronous link locally when/if it does come up.

@see TBTPacketType
@param aPacketTypes Bitmask of supported packets.
@return Error code
*/
EXPORT_C TInt CBluetoothSynchronousLink::AcceptConnection(const TBTSyncPackets& aPacketTypes)
	{
	TRAPD(err, AcceptConnectionL(aPacketTypes));
	return err;
	}
	
void CBluetoothSynchronousLink::AcceptConnectionL(const TBTSyncPackets& aPacketTypes)
	{
	TBool listenForSCO = EFalse;
	TBool listenForESCO = EFalse;

	FTRACE(FPrint(_L("AcceptConnection on CBTSyncLink 0x%08x"), this));
	
	TBTSyncPacketTypes packets = aPacketTypes();
	
	__ASSERT_ALWAYS(packets, Panic(EBadSyncPacketTypes));
	packets &= (TBTSyncPackets::ESyncAnySCOPacket | TBTSyncPackets::ESyncAnyESCOPacket);
	if (!packets)
		{
		User::Leave(KErrNotSupported);
		}

	if (iBTSynchronousLinkAccepterSCO->IsActive() || iBTSynchronousLinkAccepterESCO->IsActive())
		{
		User::Leave(KErrInUse);
		}
	
	User::LeaveIfError(ListeningSCOSocket().Open(iSockServer, KBTAddrFamily, KSockBluetoothTypeSCO, KBTLinkManager));
	CleanupClosePushL(ListeningSCOSocket());
		
	User::LeaveIfError(ListeningESCOSocket().Open(iSockServer, KBTAddrFamily, KSockBluetoothTypeESCO, KBTLinkManager));
	CleanupClosePushL(ListeningESCOSocket());
	
	CleanupStack::PushL(TCleanupItem(StaticResetScoTypes, this)); // we want to clear any setting of SCO types upon leaving
	
	TBTSyncPacketTypes packetsSCO = packets & TBTSyncPackets::ESyncAnySCOPacket;
	if (packetsSCO)
		{
		TInt err = ListeningSCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packetsSCO);
		if(!err)
			{
			iSCOTypes |= ESCO;
			listenForSCO = ETrue;
			}
		}
	
	TBTSyncPacketTypes packetsESCO = packets & TBTSyncPackets::ESyncAnyESCOPacket;
	if (packetsESCO) 
		{
		TInt err = ListeningESCOSocket().SetOpt(ESyncUserPacketTypes, KSolBtSCO, packetsESCO);
		if (!err)
			{
			iSCOTypes |= EeSCO;
			listenForESCO = ETrue;
			
			TPckgC<TBTeSCOLinkParams> options(iRequestedLink);
			User::LeaveIfError(ListeningESCOSocket().SetOpt(EeSCOExtOptions, KSolBtESCO, options));
			}
		}

	__ASSERT_ALWAYS(listenForSCO || listenForESCO, Panic(EBadSyncPacketTypes));

	TBTSockAddr sa;
	TBTServiceSecurity sec;

	//security settings
	sec.SetAuthentication(EMitmNotRequired);
	sec.SetEncryption(EFalse);
	sec.SetAuthorisation(EFalse);
	sec.SetDenied(EFalse);

	sa.SetBTAddr(TBTDevAddr()); // I don't think this matters which address is being used
	sa.SetSecurity(sec);
	
	if (listenForSCO)
		{
		User::LeaveIfError(ListeningSCOSocket().Bind(sa));
		User::LeaveIfError(ListeningSCOSocket().Listen(KSCOListenQueSize));
		User::LeaveIfError(SCOSocket().Open(SocketServer()));
		CleanupClosePushL(SCOSocket());
		}
	
	if(listenForESCO)
		{
		User::LeaveIfError(ListeningESCOSocket().Bind(sa));
		User::LeaveIfError(ListeningESCOSocket().Listen(KSCOListenQueSize));
		User::LeaveIfError(ESCOSocket().Open(SocketServer()));
		}

	// Now we can't fail synchronously, so we're ready to begin the accept.
	if(listenForESCO)
		{
		iBTSynchronousLinkAccepterESCO->Accept();
		}
		
	if (listenForSCO)
		{
		CleanupStack::Pop(&SCOSocket());
		iBTSynchronousLinkAccepterSCO->Accept();
		}
	
	CleanupStack::Pop(3); // StaticResetScoTypes, ListeningESCOSocket(), ListeningSCOSocket()
	}
	
void CBluetoothSynchronousLink::StaticResetScoTypes(TAny* aThis)
	{
	static_cast<CBluetoothSynchronousLink*>(aThis)->iSCOTypes = 0;
	}


/**
Cancel ability to respond to a remote request to set up a synchronous link.

It is possible for a race condition to mean that a connection has been established
but the notifier has not yet received the call-back.  In this case no call-back will
be received and the link (if established) will be immediately shutdown.
*/
EXPORT_C void CBluetoothSynchronousLink::CancelAccept()
	{
	iBTSynchronousLinkAccepterSCO->Cancel();
	iBTSynchronousLinkAccepterESCO->Cancel();
	iBTSynchronousLinkBaseband->StopAll();
	
	iSCOTypes = 0;
	
	LinkDown();
	
	ListeningSCOSocket().Close();
	ListeningESCOSocket().Close();
	SCOSocket().Close();
	ESCOSocket().Close();
	}


/**
 * Specify Voice Setting.
 * See section 6.12 in Bluetooth Core Specification v1.2, Vol. 2, Part E.
 */
EXPORT_C void CBluetoothSynchronousLink::SetCoding(TUint16 aVoiceCoding)
	{
	iRequestedLink.iCoding = aVoiceCoding;
	}


/**
 * Specify maximum bandwidths in octets/second.
 */
EXPORT_C void CBluetoothSynchronousLink::SetMaxBandwidth(TBTSyncBandwidth aBandwidth)
	{
	iRequestedLink.iBandwidth = aBandwidth;
	}


/**
 * Specify maximum acceptable latency in milliseconds.
 */
EXPORT_C void CBluetoothSynchronousLink::SetMaxLatency(TUint16 aLatency)
	{
	iRequestedLink.iLatency = Max(aLatency, KMinESCOLatency);
	}


/**
 * Specify link retransmission policy.
 */
EXPORT_C void CBluetoothSynchronousLink::SetRetransmissionEffort(TBTeSCORetransmissionTypes aRetransmissionEffort)
	{
	iRequestedLink.iRetransmissionEffort = static_cast<TUint8>(aRetransmissionEffort);
	}


/**
 * Return the air coding portion only of the coding specified on the link.
 * See section 6.12 in Bluetooth Core Specification v1.2, Vol. 2, Part E.
 */
EXPORT_C TUint16 CBluetoothSynchronousLink::Coding()
	{
	return iNegotiatedLink.iCoding & KAirCodingFormatBits;
	}


/**
 * Return the negotiated bandwidth.
 */
EXPORT_C TBTSyncBandwidth CBluetoothSynchronousLink::Bandwidth()
	{
	return iNegotiatedLink.iBandwidth;
	}



/**
 * Return the negotiated latency in miliseconds (rounded up if non-integer on the link).
 */
EXPORT_C TUint16 CBluetoothSynchronousLink::Latency()
	{
	return iNegotiatedLink.iLatency;
	}


/**
 * Return an estimate of the retransmission policy on the link.
 * Estimate is calculated by taking the size of the retransmission window,
 * estimating the packet type based on packet length and calculating the
 * number of packet retransmissions which are possible.  This number is
 * then returned by this function.
 */
EXPORT_C TUint8 CBluetoothSynchronousLink::RetransmissionEffort()
	{
	return iNegotiatedLink.iRetransmissionEffort;
	}


/** Gets the socket address of the remote Bluetooth device.
@panic ESock_client 17 Raised when the link is not connected so there is no valid remote name to return.
@param aAddr The socket address.
*/
EXPORT_C void CBluetoothSynchronousLink::RemoteName(TSockAddr& aAddr)
	{
	__ASSERT_ALWAYS(iSCOTypes, Panic(EBadSyncPacketTypes));
	if (iSCOTypes & ESCO)
		{
		SCOSocket().RemoteName(aAddr);
		}
		
	if (iSCOTypes & EeSCO)
		{
		ESCOSocket().RemoteName(aAddr);
		}
	}

/**
Set the object to be notified of synchronous link events.  This does
not take ownership.  This notifier will replace the current one.

@param aNotifer The new object to inform of synchronous link events.
*/
EXPORT_C void CBluetoothSynchronousLink::SetNotifier(MBluetoothSynchronousLinkNotifier& aNotifier)
	{
	iNotifier = &aNotifier;
	}

void CBluetoothSynchronousLink::HandleSetupConnectionCompleteL(TInt aErr, TSCOType aSCOType)
	{
	TBool notify = EFalse;
	FTRACE(FPrint(_L("CBluetoothSynchronousLink::HandleSetupConnectionCompleteL(aErr %d)"), aErr));
	
	/* we can be in a situation where we are trying:
	  	SCO connection only
	  	ESCO connection only
	  	either SCO or ESCO
	  	
	  	If the connection is successful and we are trying one type then we accept it
	  	If the connection isn't successful and we are trying one type then clean up
	  	
	  	If we are trying both and
	  	a) only one is successful then clean up the other one
	  	b) neither are successful then clean up
	  	c) ESCO and SCO are both successful take the ESCO one and close SCO.
	  	
	  	
	  	What ever case we are dealing with only notify when we are sure of a finished 
	  	result
	 */	
	if (aErr==KErrNone)
		{
		iSCOTypes = (TUint8)aSCOType;	
		if (EeSCO == aSCOType)
			{
			if (iOpeningSCO)
				{
				// if we are trying both then close the SCO Socket
				// we only want one open
				SCOSocket().Close();
				iOpeningSCO = EFalse;
				iOpenedSCO = EFalse;
				}
			else //we can update and notify as we are only trying ESCO
				{
				UpdateLinkParams(aSCOType);
				}
			notify = ETrue;
			}
		if (ESCO == aSCOType)
			{
			if (!iOpeningESCO)
				{// we are only trying SCO so we can update and notify
				UpdateLinkParams(aSCOType);
				notify = ETrue;
				}
			else
				{
				iOpenedSCO = ETrue; // if the ESCO is rejected then we need to notify SCO was opened
				}
			}
		}
	else
		{
		if (EeSCO == aSCOType)
			{
			if (!iOpeningSCO)
				{
				// if we are trying both and the other socket might be accepted (or has 
				// been) we don't want to close this all down, but if we're not trying
				// to open SCO then close it all down and tell the user
				iNegotiatedLink = TBTeSCOLinkParams(0, 0, 0, 0);
				iBTSynchronousLinkBaseband->StopAll();
				LinkDown();
				notify = ETrue;
				}
			else if (iOpenedSCO)
				{
				// successfully opened SCO connection, ESCO failed so tell the user
				// about the SCO
				notify = ETrue;
				}
			// no longer trying to open ESCO
			iOpeningESCO = EFalse;
			ESCOSocket().Close();
			}
		else  // ESCO = aSCOType - plain SCO 
			{			
			if (!iOpeningESCO)
				{
				// if we are trying both and the other socket might be accepted (or has 
				// been) we don't want to close this all down but if we're not trying
				// to open ESCO then close it all down and tell the called			
				iNegotiatedLink = TBTeSCOLinkParams(0, 0, 0, 0);
				iBTSynchronousLinkBaseband->StopAll();
				LinkDown();
				notify = ETrue;
				}
			else
				{// no longer trying to open SCO
				iOpeningSCO = EFalse;
				}
			SCOSocket().Close();
			}
		}
	
	if (notify)
		{
#ifdef __FLOGGING__
		TRAPD(err, Notifier().HandleSetupConnectionCompleteL(aErr));
		FTRACE(FPrint(_L("Setup upcall to link owner returned %d"), err));
		User::LeaveIfError(err);
#else
		Notifier().HandleSetupConnectionCompleteL(aErr);
#endif
		}
	}


void CBluetoothSynchronousLink::HandleDisconnectionCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSynchronousLink::HandleDisconnectionCompleteL(aErr %d)"), aErr));
	
	iNegotiatedLink = TBTeSCOLinkParams(0, 0, 0, 0);
	
	iBTSynchronousLinkBaseband->StopAll();
	LinkDown();
	SCOSocket().Close();
	ESCOSocket().Close();
	
#ifdef __FLOGGING__
	TRAPD(err, Notifier().HandleDisconnectionCompleteL(aErr));
	FTRACE(FPrint(_L("Disconnect upcall to link owner returned %d"), err));
	User::LeaveIfError(err);
#else
	Notifier().HandleDisconnectionCompleteL(aErr);
#endif
	
	}


void CBluetoothSynchronousLink::HandleAcceptConnectionCompleteL(TInt aErr, TSCOType aSCOType)
	{
	FTRACE(FPrint(_L("CBluetoothSynchronousLink::HandleAcceptConnectionCompleteL(aErr %d)"), aErr));
	
	if (aErr==KErrNone)
		{
		iSCOTypes = (TUint8)aSCOType;
		
		TSockAddr sockAddr;
		if (aSCOType & ESCO)
			{
			iBTSynchronousLinkAccepterESCO->Cancel();
			ListeningESCOSocket().Close();
			ESCOSocket().Close();
			
			SCOSocket().RemoteName(sockAddr);
			}
		else
			{
			iBTSynchronousLinkAccepterSCO->Cancel();
			ListeningSCOSocket().Close();
			SCOSocket().Close();
			
			ESCOSocket().RemoteName(sockAddr);
			}
		
		if(sockAddr.Family() == KBTAddrFamily)
			{
			TBTSockAddr& btSockAddr = static_cast<TBTSockAddr&>(sockAddr);
			TBTDevAddr da = btSockAddr.BTAddr();
			aErr = LinkUp(da);
			if(aErr == KErrNone)
				{
				iBTSynchronousLinkBaseband->PreventPark();
				iBTSynchronousLinkBaseband->CatchEvents();
				UpdateLinkParams(aSCOType);
				}
			else
				{
				FTRACE(FPrint(_L("Failed to \"LinkUp\" the synchronous link (aErr %d)"), aErr));
				}
			}
		else
			{
			// reading RemoteName has failed, probably socket state is already closed
			// for example after quick disconnection initiated from remote side
			aErr = KErrDisconnected;
			}
		}
	
	if(aErr != KErrNone)
		{
		iNegotiatedLink = TBTeSCOLinkParams(0, 0, 0, 0);
		CancelAccept(); // makes sure everything is cleaned up.
		}

#ifdef __FLOGGING__
	TRAPD(err, Notifier().HandleAcceptConnectionCompleteL(aErr));
	FTRACE(FPrint(_L("Accept upcall to link owner returned %d"), err));
	User::LeaveIfError(err);
#else
	Notifier().HandleAcceptConnectionCompleteL(aErr);
#endif
	}


void CBluetoothSynchronousLink::HandleSendCompleteL(TInt aErr)
	{
	Notifier().HandleSendCompleteL(aErr);
	}


void CBluetoothSynchronousLink::HandleReceiveCompleteL(TInt aErr)
	{
	Notifier().HandleReceiveCompleteL(aErr);
	}


MBluetoothSynchronousLinkNotifier& CBluetoothSynchronousLink::Notifier()
	{
	return *iNotifier;
	}


RSocket& CBluetoothSynchronousLink::ListeningSCOSocket()
	{
	return iListeningSCOSocket;
	}
	
	
RSocket& CBluetoothSynchronousLink::ListeningESCOSocket()
	{
	return iListeningESCOSocket;
	}

RSocket& CBluetoothSynchronousLink::SCOSocket()
	{
	return iSCOSocket;
	}
	
RSocket& CBluetoothSynchronousLink::ESCOSocket()
	{
	return iESCOSocket;
	}

RSocketServ& CBluetoothSynchronousLink::SocketServer() 
	{
	return iSockServer;
	}


RBTBaseband& CBluetoothSynchronousLink::Baseband()
	{
	return iBaseband;
	}


void CBluetoothSynchronousLink::UpdateLinkParams(TSCOType aSCOType)
	{
	TPckgBuf<TBTeSCOLinkParams> options;
	
	TInt err = KErrNotSupported;
	if(aSCOType == EeSCO)
		{
		err = ESCOSocket().GetOpt(EeSCOExtOptions, KSolBtESCO, options);
		}
	if (err) return;
	
	iNegotiatedLink.iLatency = options().iLatency;
	iNegotiatedLink.iBandwidth = options().iBandwidth;
	iNegotiatedLink.iRetransmissionEffort = options().iRetransmissionEffort;
	iNegotiatedLink.iCoding = options().iCoding;
	}


TInt CBluetoothSynchronousLink::LinkUp(TBTDevAddr aAddr)
	{
	__ASSERT_ALWAYS((iBaseband.SubSessionHandle() == 0), Panic(EBasebandAlreadyConnected));
	return iBaseband.Open(SocketServer(), aAddr);
	}


void CBluetoothSynchronousLink::LinkDown()
	{
	iBaseband.Close();
	__ASSERT_ALWAYS((iBaseband.SubSessionHandle() == 0), Panic(EBasebandFailedDisconnect));
	}


//==========================================================
//Active Object Helpers
//

//
//for CBluetoothSynchronousLink
//
CBTSynchronousLinkBaseband* CBTSynchronousLinkBaseband::NewL(CBluetoothSynchronousLink& aParent)
	{
	CBTSynchronousLinkBaseband* self = new(ELeave) CBTSynchronousLinkBaseband(aParent);
	return self;
	}


CBTSynchronousLinkBaseband::CBTSynchronousLinkBaseband(CBluetoothSynchronousLink& aParent)
	: CActive(CActive::EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkBaseband::~CBTSynchronousLinkBaseband()
	{
	StopAll();
	}

void CBTSynchronousLinkBaseband::PreventPark()
	{
	iParent.Baseband().PreventLowPowerModes(EParkMode);
	}


void CBTSynchronousLinkBaseband::CatchEvents()
	{
	if (!IsActive())
		{
		iParent.Baseband().ActivateNotifierForOneShot(iEvent, iStatus, ENotifySCOLinkDown|ENotifyPhysicalLinkDown);
		SetActive();
		}
	}


void CBTSynchronousLinkBaseband::RunL()
	{
	FLOG(_L("CBTSynchronousLinkBaseband caught disconnection event"));
	iParent.HandleDisconnectionCompleteL(iStatus.Int());
	}


TInt CBTSynchronousLinkBaseband::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}


void CBTSynchronousLinkBaseband::StopAll()
	{
	iParent.Baseband().AllowLowPowerModes(EParkMode);
	Cancel();
	}

	
void CBTSynchronousLinkBaseband::DoCancel()
	{
	iParent.Baseband().CancelNextBasebandChangeEventNotifier();
	}


//--
CBTSynchronousLinkAttacher* CBTSynchronousLinkAttacher::NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	{
	CBTSynchronousLinkAttacher* self = new (ELeave) CBTSynchronousLinkAttacher(aParent, aSCOType);
	return self;
	}

CBTSynchronousLinkAttacher::CBTSynchronousLinkAttacher(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iSCOType(aSCOType)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkAttacher::~CBTSynchronousLinkAttacher()
	{
	Cancel();
	}

void CBTSynchronousLinkAttacher::AttachSCOLink(TBTSockAddr& aSockAddr)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	FLOG(_L("CBTSynchronousLinkAttacher attaching sync link"));
	
	if(iSCOType == ESCO)
		{
		iParent.SCOSocket().Connect(aSockAddr, iStatus);
		}
	else
		{
		iParent.ESCOSocket().Connect(aSockAddr, iStatus);
		}
		
	SetActive();
	}

void CBTSynchronousLinkAttacher::RunL()
	{
	iParent.HandleSetupConnectionCompleteL(iStatus.Int(), iSCOType);
	}

TInt CBTSynchronousLinkAttacher::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}

void CBTSynchronousLinkAttacher::DoCancel()
	{
	FLOG(_L("CBTSynchronousLinkAttacher cancel attach sync link"));
	if (iSCOType == ESCO)
		{
		iParent.SCOSocket().CancelConnect();
		}
	else
		{
		iParent.ESCOSocket().CancelConnect();
		}
	}


//--
CBTSynchronousLinkDetacher* CBTSynchronousLinkDetacher::NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	{
	CBTSynchronousLinkDetacher* self = new (ELeave) CBTSynchronousLinkDetacher(aParent, aSCOType);
	return self;
	}

CBTSynchronousLinkDetacher::CBTSynchronousLinkDetacher(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iSCOType(aSCOType)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkDetacher::~CBTSynchronousLinkDetacher()
	{
	Cancel();
	}

void CBTSynchronousLinkDetacher::DetachSCOLink(const TDesC8& aDesOut, TDes8& aDesIn)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	FTRACE(FPrint(_L("CBTSynchronousLinkDetacher detach sync link (0x%08x)"), &iParent));
	
	if(iSCOType == ESCO)
		{
		iParent.SCOSocket().Shutdown(RSocket::ENormal, aDesOut, aDesIn, iStatus);	
		}
	else
		{
		iParent.ESCOSocket().Shutdown(RSocket::ENormal, aDesOut, aDesIn, iStatus);	
		}
	
	SetActive();
	}

void CBTSynchronousLinkDetacher::RunL()
	{
	FTRACE(FPrint(_L("CBTSynchronousLinkDetacher detached sync link (0x%08x)"), &iParent));
	iParent.HandleDisconnectionCompleteL(iStatus.Int());
	}

TInt CBTSynchronousLinkDetacher::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}


void CBTSynchronousLinkDetacher::DoCancel()
//
//not possible.
//
	{
	FTRACE(FPrint(_L("CBTSynchronousLinkDetacher cancel detach sync link (0x%08x) -- no-op"), &iParent));
	}


//--
CBTSynchronousLinkAccepter* CBTSynchronousLinkAccepter::NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	{
	CBTSynchronousLinkAccepter* self = new (ELeave) CBTSynchronousLinkAccepter(aParent, aSCOType);
	return self;
	}


CBTSynchronousLinkAccepter::CBTSynchronousLinkAccepter(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iSCOType(aSCOType)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkAccepter::~CBTSynchronousLinkAccepter()
	{
	Cancel();
	}

void CBTSynchronousLinkAccepter::Accept()

	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	FLOG(_L("CBTSynchronousLinkAccepter accept sync link"));
	if (iSCOType == ESCO)
		{
		iParent.ListeningSCOSocket().Accept(iParent.SCOSocket(), iStatus);
		}
	else
		{
		iParent.ListeningESCOSocket().Accept(iParent.ESCOSocket(), iStatus);
		}
	SetActive();
	}


void CBTSynchronousLinkAccepter::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	FLOG(_L("CBTSynchronousLinkAccepter accepted sync link"));
	iParent.HandleAcceptConnectionCompleteL(iStatus.Int(), iSCOType);
	}

TInt CBTSynchronousLinkAccepter::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}


void CBTSynchronousLinkAccepter::DoCancel()
	{
	FLOG(_L("CBTSynchronousLinkAccepter cancel accept sync link"));
	if (iSCOType == ESCO)
		{
		iParent.ListeningSCOSocket().CancelAccept();
		}
	else
		{
		iParent.ListeningESCOSocket().CancelAccept();
		}
	}


//--
CBTSynchronousLinkSender* CBTSynchronousLinkSender::NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	{
	CBTSynchronousLinkSender* self = new (ELeave) CBTSynchronousLinkSender(aParent, aSCOType);
	return self;
	}


CBTSynchronousLinkSender::CBTSynchronousLinkSender(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iSCOType(aSCOType)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkSender::~CBTSynchronousLinkSender()
	{
	Cancel();
	}


void CBTSynchronousLinkSender::SendSCOData(const TDesC8& aSCOData)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

#ifdef STACK_SCO_DATA

	if (iSCOType == ESCO)
		{
		iParent.SCOSocket().Write(aSCOData, iStatus);
		}
	else
		{
		iParent.ESCOSocket().Write(aSCOData, iStatus);
		}
	
	SetActive();
#else
	(void)aSCOData;
	
	// As transferring data through the HCI is not supported in this configuration,
	// complete our own request with Not Supported.
	// Uses event completion to keep sync/async behaviour consistent between
	// configurations.
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(status, KErrNotSupported);
#endif
	}

void CBTSynchronousLinkSender::RunL()
	{
	iParent.HandleSendCompleteL(iStatus.Int());
	}

TInt CBTSynchronousLinkSender::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}


void CBTSynchronousLinkSender::DoCancel()
	{
#ifdef STACK_SCO_DATA

	if(iSCOType == ESCO)
		{
		iParent.SCOSocket().CancelWrite();	
		}
	else
		{
		iParent.ESCOSocket().CancelWrite();
		}
	
#endif
	// !STACK_SCO_DATA will have already completed the request, but Cancel() will
	// swallow callback.
	}


//--
CBTSynchronousLinkReceiver* CBTSynchronousLinkReceiver::NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	{
	CBTSynchronousLinkReceiver* self = new (ELeave) CBTSynchronousLinkReceiver(aParent, aSCOType);
	return self;
	}


CBTSynchronousLinkReceiver::CBTSynchronousLinkReceiver(CBluetoothSynchronousLink& aParent, TSCOType aSCOType)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iSCOType(aSCOType)
	{
	CActiveScheduler::Add(this);
	}

CBTSynchronousLinkReceiver::~CBTSynchronousLinkReceiver()
	{
	Cancel();
	}

void CBTSynchronousLinkReceiver::ReadSCOData(TDes8& aDesc)

	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

#ifdef STACK_SCO_DATA
	
	if(iSCOType == ESCO)
		{
		iParent.SCOSocket().Read(aDesc, iStatus);	
		}
	else
		{
		iParent.ESCOSocket().Read(aDesc, iStatus);
		}
	
	SetActive();
#else
	(void)aDesc;
	
	// As transferring data through the HCI is not supported in this configuration,
	// complete our own request with Not Supported.
	// Uses event completion to keep sync/async behaviour consistent between
	// configurations.
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(status, KErrNotSupported);
#endif
	}


void CBTSynchronousLinkReceiver::RunL()
	{
	iParent.HandleReceiveCompleteL(iStatus.Int());
	}

TInt CBTSynchronousLinkReceiver::RunError(TInt /*aError*/)
	{
	// Swallow the error
	return KErrNone;
	}


void CBTSynchronousLinkReceiver::DoCancel()
	{
#ifdef STACK_SCO_DATA
	
	if (iSCOType == ESCO)
		{
		iParent.SCOSocket().CancelRead();
		}
	else
		{
		iParent.ESCOSocket().CancelRead();
		}
	
#endif
	// !STACK_SCO_DATA will have already completed the request, but Cancel() will
	// swallow callback.
	}
