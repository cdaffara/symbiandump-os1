// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bt_sock.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/aclsockaddr.h>
#include <bluetooth/hci/aclpacketconsts.h>
#include "btsocketpanic.h"

//.................................
//
//Actual raw client
//

RBTBaseband::RBTBaseband()
	{
	}

void RBTBaseband::Close()
	{
	if (SubSessionHandle())
		{
		iSocket.Close();
		}
	}


/**

  API useful for Bluetooth as seen from a single physical link perspective

*/

// for 'getting' a RBTBaseband from a connected socket
TInt RBTBaseband::Open(RSocketServ& aSocketServ, RSocket& aConnectedSocket)
	{
	if (!aConnectedSocket.SubSessionHandle())
		{
		return KErrNotReady;
		}
	
	THCIConnHandle bbHandle;
	TPckg<THCIConnHandle> bbHandleBuf(bbHandle);

	TInt err = aConnectedSocket.GetOpt(KLMGetACLHandle, KSolBtACL, bbHandleBuf);
	if (err)
		{
		return err;
		}
	err = iSocket.Open(aSocketServ, KBTAddrFamily, bbHandle, KBTLinkManager);

	return err;
	}

TInt RBTBaseband::Open(RSocketServ& aSocketServ, const TBTDevAddr& aBDAddr)
	{
	//open unsubscribed socket (currently use KSockBluetoothTypeRawBroadcast)
	TInt err = iSocket.Open(aSocketServ, 
							KBTAddrFamily, 
							KSockBluetoothTypeRawBroadcast, 
							KBTLinkManager);
	if (err)
		{
		return err;
		}

	//attempt to subscribe socket to supplied address
	TPckg<TBTDevAddr> pckg(aBDAddr);
	err = iSocket.SetOpt(EBBSubscribePhysicalLink, KSolBtLMProxy, pckg);
	if(err)
		//only leave open if there is an EXISTING connection
		{
		iSocket.Close();
		}

	return err;
	}

TInt RBTBaseband::PhysicalLinkState(TUint32& aState)
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		TBTBasebandEvent pckgEvent;

		err = iSocket.GetOpt(EBBGetPhysicalLinkState, KSolBtLMProxy, pckgEvent);
		if (err == KErrNone)
			{
			aState = pckgEvent().EventType();
			}
		}

	return err;
	}

/**Role change methods*/
TInt RBTBaseband::PreventRoleSwitch()
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		err = iSocket.SetOpt(EBBRequestPreventRoleChange, KSolBtLMProxy, 0);
		}

	return err;
	}

TInt RBTBaseband::AllowRoleSwitch()
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		err = iSocket.SetOpt(EBBRequestAllowRoleChange, KSolBtLMProxy, 0);
		}
		
	return err;
	}

TInt RBTBaseband::RequestMasterRole()
	{
	return RequestRole(EBBRequestRoleMaster);
	}

TInt RBTBaseband::RequestSlaveRole()
	{
	return RequestRole(EBBRequestRoleSlave);
	}


/**Low power mode methods*/
TInt RBTBaseband::PreventLowPowerModes(TUint32 aLowPowerModes)
	{
	if (!SubSessionHandle())
		{
		return KErrNotReady;
		}
	//if caller has used silly bits
	if(aLowPowerModes & ~(EAnyLowPowerMode))
		{
		return KErrArgument;
		}

	TUint32 mask = 0;
	if(aLowPowerModes & EHoldMode)
		{
		mask |= EBBRequestPreventHold;
		}
	if(aLowPowerModes & ESniffMode)
		{
		mask |= EBBRequestPreventSniff;
		}
	if(aLowPowerModes & EParkMode)
		{
		mask |= EBBRequestPreventPark;
		}

	return iSocket.SetOpt(mask, KSolBtLMProxy);
	}

TInt RBTBaseband::AllowLowPowerModes(TUint32 aLowPowerModes)
	{
	
	if (!SubSessionHandle())
		{
		return KErrNotReady;
		}
	
	//if caller has used silly bits
	if(aLowPowerModes & ~(EAnyLowPowerMode))
		{
		return KErrArgument;
		}

	TUint32 mask = 0;
	if(aLowPowerModes & EHoldMode)
		{
		mask |= EBBRequestAllowHold;
		}
	if(aLowPowerModes & ESniffMode)
		{
		mask |= EBBRequestAllowSniff;
		}
	if(aLowPowerModes & EParkMode)
		{
		mask |= EBBRequestAllowPark;
		}

	return iSocket.SetOpt(mask, KSolBtLMProxy);
	}

TInt RBTBaseband::ActivateSniffRequester()
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else 
		{
		err = iSocket.SetOpt(EBBRequestSniff, KSolBtLMProxy, 0);
		}
	return err;
	}

TInt RBTBaseband::ActivateParkRequester()
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		err = iSocket.SetOpt(EBBRequestPark, KSolBtLMProxy);
		}
	return err;
	}

TInt RBTBaseband::CancelLowPowerModeRequester()
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else 
		{
		err = iSocket.SetOpt(EBBCancelModeRequest, KSolBtLMProxy, 0);
		}
		
	return err;
	}

TInt RBTBaseband::RequestExplicitActiveMode(TBool aActive)
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		TPckgBuf<TBool> active = aActive;
		err = iSocket.SetOpt(EBBRequestExplicitActiveMode, KSolBtLMProxy, active);
		}
	return err;
	}


/**Packet method*/
TInt RBTBaseband::RequestChangeSupportedPacketTypes(TUint16 aPacketTypes)
	{
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		// Only allow ACL packet types to be changed
		// Although EAnyNonEdrACLPacket is a confusing name in this context
		// it is correct. As standard ACL packet types are to be used when the
		// appropriate bit is set, for EDR packets the bit is set if they
		// are not to be used.  Thus we are correctly checking that only
		// valid bits are set when using the EAnyNonEdrACLPacket bitmask.

		if ((aPacketTypes & ~EAnyNonEdrACLPacket) != 0)
			{
			return KErrArgument;
			}
			
		TPckgBuf<TUint16> packetTypes = aPacketTypes;
		err = iSocket.SetOpt(EBBRequestChangeSupportedPacketTypes, KSolBtLMProxy, packetTypes);
		}
	return err;
	}

void RBTBaseband::ReadNewPhysicalLinkMetricValue(TRequestStatus& aStatus, TDes8& aPLMData, TBTLMIoctls anIoctl)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		iSocket.Ioctl(anIoctl, aStatus, &aPLMData, KSolBtLMProxy);
		}
	}

void RBTBaseband::CancelPhysicalLinkMetricUpdate()
	{
	if(SubSessionHandle())
		{
		iSocket.CancelIoctl();
		}
	}

/**Notification methods*/
void RBTBaseband::ActivateNotifierForOneShot(TBTBasebandEvent& aEventNotification, 
		                                     TRequestStatus& aStatus, 
											 TUint32 aEventMask)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		aEventNotification().SetEventType(aEventMask);
		iSocket.Ioctl(KLMBasebandEventOneShotNotificationIoctl, aStatus, &aEventNotification, KSolBtLMProxy);		
		}
	}

void RBTBaseband::ActivateNotifierForRecall(TBTBasebandEvent& aEventNotification, 
		                                    TRequestStatus& aStatus, 
											TUint32 aEventMask)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		aEventNotification().SetEventType(aEventMask);
		iSocket.Ioctl(KLMBasebandEventNotificationIoctl, aStatus, &aEventNotification, KSolBtLMProxy);		
		}
	}

void RBTBaseband::CancelNextBasebandChangeEventNotifier()
	{
	if (SubSessionHandle())
		{
		//only allowed one Ioctl at a time - only one used is the Notifications Ioctl
		iSocket.CancelIoctl();		
		}
	}

TInt RBTBaseband::Authenticate()
	{
	// First check that the socket is open etc	
	TInt err = KErrNone;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else 
		{
		// Send the request - this will return KErrAlreadyExists if the link is already authenticated.
		err = iSocket.SetOpt(EBBRequestLinkAuthentication, KSolBtLMProxy, 0);
		}

	return err;
	}


/**

  API useful for Bluetooth as seen from a device perspective

*/

TInt RBTBaseband::Open(RSocketServ& aSocketServ)
	{
	// need to specify this is a raw socket
	TInt err = iSocket.Open(aSocketServ, 
							KBTAddrFamily, 
							KSockBluetoothTypeRawBroadcast, 
							KBTLinkManager);
	return err;
	}

void RBTBaseband::Connect(const TBTDevAddr& aAddr, TRequestStatus& aStatus)
	{
	iConnectToken().iDevice.SetAddress(aAddr);
	DoConnect(aStatus);
	}

void RBTBaseband::Connect(const TPhysicalLinkQuickConnectionToken& aToken, TRequestStatus& aStatus)

//  (Not for documentation!)
//	param	aDevice	
//	A valid nameless device - that may have been obtained from Registry

	{
	if (!iConnectToken().iDevice.IsValidAddress())
		{
		LocalComplete(aStatus, KErrArgument);
		}
	else
		{
		iConnectToken() = aToken;
		DoConnect(aStatus);
		}
	}


TInt RBTBaseband::Broadcast(const TDesC8& aData)
	{
#ifdef PROXY_COMMUNICATES
	// have to notify Proxy that we intend to write data via it
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		err = iSocket.SetOpt(EBBBeginRaw, KSolBtLMProxy, 0);
		}
	if (err == KErrNone || err == KErrAlreadyExists)
		{
		TRequestStatus s;
		TUint8 flags = KPiconetBroadcast << 2;
		iSocket.Send(aData, flags, s);	// 'send' to allow flags
		User::WaitForRequest(s);
		err = s.Int();
		}
	return err;
#else
	return KErrNotSupported;
#endif
	}

TInt RBTBaseband::ReadRaw(TDes8& aData)
/**
	NOT PROPERLY IMPLEMENTED - JUST A PLACE HOLDER REALLY
*/
	{
#ifdef PROXY_COMMUNICATES
	// have to notify Proxy that we intend to write data via it
	TInt err;
	if (!SubSessionHandle())
		{
		err = KErrNotReady;
		}
	else
		{
		err = iSocket.SetOpt(EBBBeginRaw, KSolBtLMProxy, 0);
		}
	if (err == KErrNone || err == KErrAlreadyExists)
		{
		TRequestStatus s;
		TUint8 flags = KPiconetBroadcast << 2;
		iSocket.Recv(aData, flags, s);	// 'send' to allow flags
		User::WaitForRequest(s);
		err = s.Int();
		}
	return err;
#else
	return KErrNotSupported;
#endif
	}


void RBTBaseband::TerminatePhysicalLink(TInt /*aReason*/)
	{
	// synchronise
	TRequestStatus stat;
	TerminatePhysicalLink(0, stat);
	User::WaitForRequest(stat);
	}

void RBTBaseband::TerminatePhysicalLink(TInt /*aReason*/, TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		// synchronise
		TBuf8<1> dummy;
		iSocket.Shutdown(RSocket::EImmediate, KDisconnectOnePhysicalLink, dummy, aStatus); // this *means* detach now
		}
	}

void RBTBaseband::TerminatePhysicalLink(TInt /*aReason*/, const TBTDevAddr& aAddr, TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		// ensure that BT address has persistence
		iSocketAddress.Zero();
		iSocketAddress.Copy(aAddr.Des());
		// the following line needs to be EImmediate when ESock has been fixed
		iSocket.Shutdown(RSocket::ENormal, iSocketAddress, iConnectInData, aStatus); // this *means* detach now
		}
	}

void RBTBaseband::ShutdownPhysicalLink(TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		// synchronise
		TBuf8<1> dummy;
  		iSocket.Shutdown(RSocket::ENormal, KDisconnectOnePhysicalLink, dummy, aStatus); // this *means* detach gently
		}	
	}

void RBTBaseband::TerminateAllPhysicalLinks(TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		TBuf8<1> dummy;
		iSocket.Shutdown(RSocket::ENormal, KDisconnectAllPhysicalLinks, dummy, aStatus); // this *means* detach now
		}
	}

void RBTBaseband::TerminateAllPhysicalLinksForPowerOff(TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	else
		{
		TBuf8<1> dummy;
		iSocket.Shutdown(RSocket::ENormal, KDisconnectAllPhysicalLinksForPowerOff, dummy, aStatus); // this *means* detach now
		}
	}


TInt RBTBaseband::Enumerate(RBTDevAddrArray& aBTDevAddrArray, TUint aMaxNumber)
	{
	if (!SubSessionHandle())
		{
		return KErrNotReady;
		}

	__ASSERT_DEBUG(aMaxNumber>=1, Panic(EBadArgument));
	if(!aMaxNumber)
		{
		return KErrArgument;
		}
	HBufC8* buffer=0;
	const TInt KAddrLen = sizeof(TBTDevAddr);

	TRAPD(err, buffer = HBufC8::NewL(aMaxNumber*KAddrLen));
	if(err)
		{
		return KErrNoMemory;
		}

	TPtr8 ptr = buffer->Des();
	err = iSocket.GetOpt(EBBEnumeratePhysicalLinks, KSolBtLMProxy, ptr);
	if (err)
		{
		delete buffer;
		return err;
		}

	/**
	Parse the supplied descriptor
	*/
	
	aBTDevAddrArray.Reset();
	while(ptr.Length()>=KBTDevAddrSize)
		{
		TBTDevAddr parsedAddr(ptr.Mid(ptr.Length()-KAddrLen, KBTDevAddrSize));
		ptr.SetLength(Max(ptr.Length()-KAddrLen, 0));
		aBTDevAddrArray.Append(parsedAddr);
		}

	delete buffer;
	return KErrNone;
	}


TInt RBTBaseband::SubSessionHandle() const
	{
	return iSocket.SubSessionHandle();
	}


/**Private Methods*/
TInt RBTBaseband::RequestRole(TBTLMOptions aRole)
	{
	if (!SubSessionHandle())
		{
		return KErrNotReady;
		}
	__ASSERT_DEBUG(aRole==EBBRequestRoleMaster||aRole==EBBRequestRoleSlave, 
				   Panic(EBadArgument));

	return iSocket.SetOpt(aRole, KSolBtLMProxy, 0);
	}


void RBTBaseband::LocalComplete(TRequestStatus& aStatus, TInt aErr)
	{
	aStatus = KRequestPending;
	TRequestStatus* pStat = &aStatus;
	User::RequestComplete(pStat, aErr);
	}

void RBTBaseband::DoConnect(TRequestStatus& aStatus)
	{
	if (!SubSessionHandle())
		{
		LocalComplete(aStatus, KErrNotReady);
		}
	
	__ASSERT_DEBUG(iConnectToken().iDevice.IsValidAddress(), Panic(EBBInvalidAddress));
	
	iSocketAddress.SetBTAddr(iConnectToken().iDevice.Address()); // not that useful
	iSocket.Connect(iSocketAddress, iConnectToken, iConnectInData, aStatus);
	}



//.................................
//
//Class used for choosing baseband events to report, and for reporting 
//those events to the Bluetooth client
//

EXPORT_C TInt TBTBasebandEventNotification::SymbianErrorCode() const
/** Returns an error in 'Symbian' format
	@return Symbian error code	
	@publishedAll
	@released
*/	
	{ 
	return iErrorCode==KErrNone?iErrorCode:KHCIErrorBase-iErrorCode; 
	};



//.................................
//
//Facade class for developers wishing who wish to use a pre-existing connected 
//physical link
//

EXPORT_C RBTPhysicalLinkAdapter::RBTPhysicalLinkAdapter()
/** Constructor
	@publishedAll
	@released
*/
	{
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::Open(RSocketServ& aSocketServ, RSocket& aSocket)
/**	Open a physical link adapter on an existing physical link defined by 'aSocket'.
	@pre There exists a Bluetooth connection
	@param aSocketServ	 
	An existing ESock session
	@param aSocket
	An open connected socket (ESock subsession) on that existing ESock session
	@return Error code	
	@publishedAll
	@released
	@capability LocalServices
*/
	{
	return iBTBaseband.Open(aSocketServ, aSocket);
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::Open(RSocketServ& aSocketServ, const TBTDevAddr& aBDAddr)
/**	Open a physical link adapter on an existing physical link defined by 'aDevAddr'.
	@pre There exists a Bluetooth connection
	@param aSocketServ	 
	An existing ESock session
	@param aBDAddr 
	The Bluetooth address of a remote device with which there is an existing connection 
	@return Error code	
	@publishedAll
	@released
	@capability LocalServices
*/
	{
	return iBTBaseband.Open(aSocketServ, aBDAddr);
	}

EXPORT_C void RBTPhysicalLinkAdapter::Close()
/**	Close the physical link adapter.
	@publishedAll
	@released
*/
	{
	iBTBaseband.Close();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::PhysicalLinkState(TUint32& aState)
/**	Get the state of the physical link.
	@pre One of the Open functions has been called
	@param aState
	Used to return the physical link state - as a combination of bit values
	defined in TBTPhysicalLinkStateNotifier.
	@see TBTPhysicalLinkStateNotifier
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.PhysicalLinkState(aState);
	}

//Role change methods
EXPORT_C TInt RBTPhysicalLinkAdapter::PreventRoleSwitch()
/**	Blocks a role switch

	Stops a Master/Slave switch occurring until such time as 'AllowRoleSwitch' is called. 
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.PreventRoleSwitch();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::AllowRoleSwitch()
/**	Ensures this object does not block a role switch.

	Switches off 'PreventRoleSwitch'. If another RBTPhysicalLinkAdapter object requests, 
	or has requested a Master/Slave switch, that request will now not be blocked by 
	this RBTPhysicalLinkAdapter object
	The default is to allow a Master/Slave switch.
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.AllowRoleSwitch();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::RequestMasterRole()
/**	Attempt to be the Bluetooth Master of a Piconet.

	If the local device is currently the slave, a role switch maybe performed 
	if no other user of a RBTPhysicalLinkAdapter object has called PreventRoleSwitch 
	and the remote device allows the role switch.
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.RequestMasterRole();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::RequestSlaveRole()
/**	Attempt to be a Bluetooth Slave of a Piconet. 

	If the local device is currently the master, a role switch maybe performed if 
	no other user of a RBTPhysicalLinkAdapter object has called PreventRoleSwitch 
	and the remote device allows the role switch.
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.RequestSlaveRole();
	}

//Low power mode methods
EXPORT_C TInt RBTPhysicalLinkAdapter::PreventLowPowerModes(TUint32 aLowPowerModes)
/**	Blocks the use of a specified set of low power modes

	Stops the physical link using any one of the set of low power modes specified by 
	the bit mask 'aLowPowerModes'. To undo this blocking mechanism for a given set
	of low power modes, 'AllowLowPowerModes' needs to be called with appropriate values 
	in its 'aLowPowerModes' parameter.
	
	NB  THIS METHOD CAN BE USED TO FORCE THE PHYSICAL LINK INTO ACTIVE MODE.
		To do this set the parameter to EAnyLowPowerMode. The requests for low power 
		modes by any RBTPhysicalLinkAdapter objects will now be blocked by this object.
	
	NB  Some remote devices will automatically disconnect from a device whose Link Policy 
		settings prevent low power modes.  
		
	@pre One of the Open functions has been called
	@param	aLowPowerModes
	A mask to specify which power modes are to be prevented. 
	(Combine EHoldMode, ESniffMode, EParkMode or use EAnyLowPowerMode)
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.PreventLowPowerModes(aLowPowerModes);
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::AllowLowPowerModes(TUint32 aLowPowerModes)
/**	Ensures this object does not block the use of a specified set of low power modes

	Switches off 'PreventLowPowerModes' for the low power modes specified by the parameter 
	'aLowPowerModes'. If another RBTPhysicalLinkAdapter object requests, or has 
	requested one of those low power modes, that request will now NOT be blocked by 
	this RBTPhysicalLinkAdapter object. 
	The default is to allow all low power modes.
	NB. Warning this may reactivate a low power mode requester.
		For example: 
		ActivateSniffRequester(); //sniff requester active
		PreventLowPowersModes(ESniffMode); //sniff requester dormant
		....
		AllowLowPowersModes(ESniffMode); //sniff requseter active

	@pre One of the Open functions has been called
	@param	aLowPowerModes
	A mask to specify which power modes are to be prevented. 
	(Combine EHoldMode, ESniffMode, EParkMode or use EAnyLowPowerMode)
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.AllowLowPowerModes(aLowPowerModes);
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::ActivateSniffRequester()
/**	Start a facility that will continually attempt to put the physical link into Sniff Mode.

	Attempt to put the physical link into Sniff mode. If for any reason this is
	not possible (e.g another user of a RBTPhysicalLinkAdapter object has called 
	PreventLowPowerModes on Sniff) or the physical link comes out of Sniff mode, 
	this attempt will be repeated whenever a relevant event occurs or command is made.
	These attempts will cease if a call to either ActivateParkRequester, ActivateActiveRequester or
	CancelLowPowerModeRequester is made.
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.ActivateSniffRequester();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::ActivateParkRequester()
/**	Start a facility that will continually attempt to put the physical link into Park Mode.

	Attempt to put the physical link into Park mode. If for any reason this is
	not possible (e.g another user of a RBTPhysicalLinkAdapter object has called 
	PreventLowPowerModes on Park) or the physical link comes out of Park mode, 
	this attempt will be repeated whenever a relevant event occurs or command is made.
	These attempts will cease, if a call to either ActivateSniffRequester, ActivateActiveRequester or
	CancelLowPowerModeRequester is made.
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.ActivateParkRequester();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::ActivateActiveRequester()
/**	Start a facility that will continually attempt to put the physical link into Active Mode.

	Puts the physical link into Active mode, even if a Low Power Mode (Sniff or Park) has 
	been explicitly requested by another client of the physical link.
	Calling CancelLowPowerModeRequests() will cancel the explicit request for Active mode.
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	TInt err;
	err = iBTBaseband.RequestExplicitActiveMode(ETrue);
	if(err == KErrNone)
		{
		err = iBTBaseband.CancelLowPowerModeRequester();
		}
	return err;
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::CancelLowPowerModeRequester()
/**	Cancel a facility that is continually requesting a low power mode

	If ActivateSniffRequester, ActivateParkRequester or ActivateActiveRequester has been called by the user
	of this RBTPhysicalLinkAdapter object, repeated attempts will be made to put/return 
	the physical link to that mode whenever a relevant event occurs or command is made. 
	CancelLowPowerModeRequester stops these requests. However if another user of a 
	RBTPhysicalLinkAdapter object has called ActivateSniffRequester, ActivateParkRequester or ActivateActiveRequester,
	those requests will still be active, and so the physical link will remain controlled by these requests.
	
	To try to force the physical link into active mode, a call to 
	either PreventLowPowerModes(EAnyLowPowerMode) or ActivateActiveRequester() should be made.
	
	@pre One of the Open functions has been called
	@return Error code	
	@publishedAll
	@released
*/
	{
	TInt err;
	err = iBTBaseband.RequestExplicitActiveMode(EFalse);
	if(err == KErrNone)
		{
		err = iBTBaseband.CancelLowPowerModeRequester();
		}
	return err; 
	}


//Packet method
EXPORT_C TInt RBTPhysicalLinkAdapter::RequestChangeSupportedPacketTypes(TUint16 aPacketTypes)
/**	Update the set of baseband packet types that are allowed locally

	Attempts to control which Bluetooth baseband ACL packet types (i.e. DM1, DH1, DM3 etc)
	are allowed by our host controller on the physical link.
	@pre One of the Open functions has been called
	@param aPacketTypes
	Bitmask for packet types to be supported 
	(Combine elements of TBTPacketType (or use TBTPacketTypeCombinations))
	@see TBTPacketType
	@see TBTPacketTypeCombinations

	@return Error code	
	@publishedAll
	@released
*/
	{
	return iBTBaseband.RequestChangeSupportedPacketTypes(aPacketTypes);
	}

//Notification methods
EXPORT_C void RBTPhysicalLinkAdapter::NotifyNextBasebandChangeEvent(TBTBasebandEvent& aEventNotification, 
		                                                            TRequestStatus& aStatus, 
											                        TUint32 aEventMask)
/**	Request a notification 

	Request notification the next time one of a user specified selection (see parameter 3) 
	of baseband events occurs.
 	@pre One of the Open functions has been called
	@param aEventNotification
	Return parameter
	@param aStatus
	Status parameter for asynchronous request
	@param aEventMask
	Bitmask for those events for which notification is being requested 
	Use TBTPhysicalLinkStateNotifier (and TBTPhysicalLinkStateNotifierCombinations)
	@see TBTPhysicalLinkStateNotifier
	@see TBTPhysicalLinkStateNotifierCombinations
	@publishedAll
	@released
*/
	{
	iBTBaseband.ActivateNotifierForOneShot(aEventNotification, aStatus, aEventMask);
	}

EXPORT_C void RBTPhysicalLinkAdapter::CancelNextBasebandChangeEventNotifier()
/**	Cancel a currently requested notification

	Switch off the currently active baseband change event notifier.	
	@pre One of the Open functions has been called
	@publishedAll
	@released
*/
	{
	iBTBaseband.CancelNextBasebandChangeEventNotifier();
	}

EXPORT_C TInt RBTPhysicalLinkAdapter::Authenticate()
/**	Attempts to authenticate the existing physical link

	If the the physical link has already been authenticated it will return an error,
	otherwise an Authentication Request will be made to the remote device.
	
	This is a synchronous call and will return immediately the request has been issued.
	If required, NotifyNextBasebandChangeEvent() should be issued before this to wait for
	the completion of this authenticaton (for both authentication success and failure)	

	@pre One of the Open functions has been called
	@return Error code. KErrAlreadyExists if the link is already authenticated
*/
	{
	return iBTBaseband.Authenticate();
	}

EXPORT_C TBool RBTPhysicalLinkAdapter::IsOpen() const
/**	Check whether the physical link adapter is open

	This method is not required to be called before the other methods.
	KErrNotReady will be returned by other methods, if RBTPhysicalLinkAdapter is not open yet.

	@publishedAll
	@released
*/
	{
	return (iBTBaseband.SubSessionHandle() ? ETrue : EFalse);
	}
	



