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


#ifndef AVCTPSERVICES_H
#define AVCTPSERVICES_H

#include <bluetoothav.h>
#include <bluetooth/avctptypes.h>

/** Protocol version number for use in SDP records.
*/
const TUint16 KAvctpProtocolVersion = 0x0103;

/**
Mixin for receiving AVCTP events from RAvctp

Note RAvctp doesn't expect the MAvctpEventNotify class to synchronously call Close()
on it within any of these callbacks with the exception of MaenErrorNotify. You may 
call CloseGracefully since its asynchronous by nature.

@internalTechnology
@see RAvctp
*/
class MAvctpEventNotify
	{
public:
/**
	AVCTP Connection Confirm
	This is a response to RAvctp::ConnectRequest() and passes on the result of the Connection attempt.
	NB we don't return the configuration status as in Appendix A - AVCTP Upper Interface of [R2]. 
	If the aConnectResult is KErrNone then RAvctp is now connected.
	@param aBTDevice the address of the device connected to
	@param aConnectResult KErrNone if the connection attempt was successful or one of the system-wide error codes
*/
	virtual void MaenAttachConfirm(const TBTDevAddr& aBTDevice, TInt aMaxMessageSize, TInt aConnectResult) = 0;

/**
	AVCTP Connection Indicate
	This is a is called when a remote device has connected to us.
	NB we don't return the configuration status as in Appendix A - AVCTP Upper Interface of [R2]. 
	@param aBTDevice the address of the device connected to
	@param aAccept this parameter is provided so that the client of RAvctp can indicate whether
				   they want to accept the connection. If aAccept is not changed the connection
				   will be refused however this may not result in the actual disconnection of the 
				   device if another RAvctp client did accept the connection. The meaning of a 
				   refusal is that you don't care whether or not the connection is there or not.				   
*/
	virtual void MaenAttachIndicate(const TBTDevAddr& aBTDevice, TInt aMaxMessageSize, TBool& aAccept) = 0;

/**
	AVCTP Disconnection Indication that a remote device has disconnected from us.
	It is only called if the device had been explicitly Connected to. 
	@param aBTDevice the address of the disconnecting device
*/
	virtual void MaenDetachIndicate(const TBTDevAddr& aBTDevice) = 0; 

/**
	AVCTP Disconnection Confirm - disconnection from a device complete
	This is the response to RAvctp::DisconnectRequest.
	@param aBTDevice the address of the disconnected device
	@param aDisconnectResult will be one of the system-wide error codes. If KErrTimedOut is returned then the RAvctp will be disconnected.
*/
	virtual void MaenDetachConfirm(const TBTDevAddr& aBTDevice, TInt aDisconnectResult) = 0; 

/**
	AVCTP Message received indication
	This method is called when a message has been received from the given device on the RAvctp's PID. 
	
	Note that because AVCTP is a connectionless protocol, it is perfectly possible to get a 
	MaenMessageReceivedIndicate event from a device that you have not either explicitly connected to.
	For instance even if you don't accept a MaenConnectIndicate you may still receive messages from that 
	remote device.	
	
	@param aBTDevice address of the device sending us an AVCTP message
	@param aTransactionLabel message transaction label
	@param aType type of message 
	@param aIpidBitSet this will be set to true only if a message has been received indicating 
					   that the profile corresponding to the originally sent message is not valid.
					   If RAvctp was used to send the message then this response will have come from
					   the remote device aBTDevice.
	@param aMessageInformation contains only the AVCTP Command / Response Message Information and not the whole packet.
	                           Ownership transferred to client.
*/
	virtual void MaenMessageReceivedIndicate(const TBTDevAddr& aBTDevice,
											SymbianAvctp::TTransactionLabel aTransactionLabel,
											SymbianAvctp::TMessageType aType,
											TBool aIpidBitSet,
							 				const TDesC8& aMessageInformation) = 0;

/**
	AVCTP Message send complete.
	This method is called when a RAvctp has attempted to send the message defined by aTransactionLabel and aBTDevice. 
	@param aTransactionLabel The transaction label of the message that has been sent
	@param aBTDevice the device to which the send has completed
	@param aSendResult KErrNone if the send was successful or one of the system-wide error codes

*/
	virtual void MaenMessageSendComplete(const TBTDevAddr& aBTDevice, 
							       		SymbianAvctp::TTransactionLabel  aTransactionLabel,   
										TInt aSendResult) = 0; 
/**
	AVCTP Close Complete
	This is the response to the CloseGracefully() that has been called on a RAvctp object. It is the
	last event that will be called until the RAvctp object is Open()'d again.
*/
	virtual void MaenCloseComplete() = 0;
	
/**
	AVCTP error notification
	Note an errored device does not indicate that the device has been disconnected. If it has then a 
	MaenDisconnectIndicate event will be used to indicate this.
	@param aBTDevice the remote device associated with the error or TBTDevAddr(0) for a general error
	@param aError system wide error
*/
	virtual void MaenErrorNotify(const TBTDevAddr& aBTDevice, TInt aError) = 0;

/**
	Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	@param aInterface UID of the interface to return
    @param aObject the container for another interface as specified by aInterface
*/
	virtual void MaenExtensionInterfaceL(TUid aInterface, void*& aObject) = 0; 
	};


class MAvctpChannel
	{
public:
/**
	Starts an explicit connection attempt using the default AVCTP security settings
	This function should be used if you wish to ensure a connection is available before calling 	
	SendMessage to ensure that the message is sent with minimal delay.
	Following this function, MaenConnectConfirm will be called on aNotify with results of the connection 
	attempt provided this function returned KErrNone.
	@param aBTDevice the remote device address to connect to
	@return a system-wide error code
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual TInt MacAttachRequest(const TBTDevAddr& aBTDevice) = 0;
	
/**
	Can be used to determine with this client has explicitly connected to the given device.
	@param aBTDevice the remote device in question
	@return ETrue if the client has previously requested a connection to this device, otherwise EFalse	
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual TBool MacIsAttached(const TBTDevAddr& aBTDevice) = 0;
	
/**
	Cancels an outstanding connect operation so that MaenConnectConfirm is NOT called on aNotify
	Can be called at any time. 
	
	@param aBTDevice the remote device of the Connect to cancel
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual void MacCancelAttach(const TBTDevAddr& aBTDevice) = 0;
/**
	Begins the disconnection process if the RAvctp object is explicitly connected to aBTDevice. 
	When the remote device has been disconnected MaenDisconnectConfirm will be called on aNotify,
	provided this function returned KErrNone.
	
	Note that this function can not be used to prevent an ongoing connection attempt. If this is 
	what you wish to do, use CancelConnect(...) instead.
	
	@param aBTDevice the remote device to disconnect from
	@return a system-wide error code
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual TInt MacDetachRequest(const TBTDevAddr& aBTDevice) = 0;
	
/**
	Send an AVCTP message
	This is a helper function so that the user doesn't have to know how to format an AVCTP packet. 
	When the message has been sent, MaenMessageSendComplete will be called on aNotify, provided this function
	 returned KErrNone.
	
	It is not necessary to call Connect(aBTDevice) before calling this function since a connection will be 
	implicitly started to carry this message if one doesn't already exist. However, this has two potential
	drawbacks:
	i) If there isn't already a connection to the remote device then the packet may take some time to reach
	its destination
	ii) If there isn't already a connection to the remote device then and this connection attempt fails, you
	will not be informed of this fact. Effectively the packet will have been dropped.
	NB both the above problems will not occur if you explicitly connect the remote device and wait for the
	resulting ConnectCfm event.
	
	@param aBTDevice the remote device to connect to
	@param aTransactionLabel will be the transaction ID for the message as specified in the AVCTP 1.0 
				spec. It is the responsibility of the RAvctp user to provide a transaction ID that "uniquely
				identifies the transaction with a limited scope in the command / response flow".
	@param aType the packet type
	@param aMessageInformation should contain only the AVCTP Command / Response Message 
				Information
	@return KErrInUse if the RAvctp is already sending a message (if this happens you need to wait until 
			 you get an MaenMessageSendComplete event before sending again) or another system wide error code
	@panic if the most significant four bits of aTransactionLabel are not zero
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual TInt MacSendMessage(const TBTDevAddr& aBTDevice, 
						SymbianAvctp::TTransactionLabel aTransactionLabel,
						SymbianAvctp::TMessageType aType,
						const TDesC8& aMessageInformation) = 0;
	
/**
	Cancels an outstanding send operation so that MaenMessageSendComplete is NOT called on aNotify
	Can be called at any time.
	
	@panic if the RAvctp object isn't open or Close / CloseGracefully  has been called on the RAvctp object 
*/
	virtual void MacCancelSend() = 0;
	};




// Forward declaration
class CAvctpBody;



/**
AVCTP service provider

Note that because AVCTP is a connectionless protocol, this class doesn't provide an API
to directly control all aspects of whether or not a connection exists to a remote device. 
It provides an API to force a connection to exist (RAvctp::ConnectRequest()) until either a 
MaenDisconnectIndicate or MaenDisconnectConfirm event is notified via the MAvctpEventNotify
class that clients should derive from.

When using RAvctp clients need to make the distinction between an explicit connection and 
an actual connection. All this class provides is an API to bring up and in keeping a connection
up by allowing explicit connections to be made. However if no explicit connection to a remote
device has been made, that does NOT mandate that no such connection to this remote will exist.
Instead it means the client doesn't care if there is a connection or not to this remote.
Hence a client can receive messages from or send messages to a device they've not explicitly 
connected to.

@internalTechnology
*/
NONSHARABLE_CLASS(RAvctp)
	{
public:
/**
	@see RAvctp::Close()
*/
	enum TCloseType	
		{
		ENormal,
		EImmediate,
		};
		
public:

/**
	Default constructor
*/
	IMPORT_C RAvctp();

/**
	Opens a channel to the AVCTP protocol.
	Only one RAvctp can be registered to receive events on each PID. 
	@param aNotify the MAvctpEventNotify object to notify with all this RAvctp's AVCTP events.
	@param aPid the PID on which RAvctp will be used
	@return KErrInUse if aPid is already being used somewhere in the system
			 KErrPermissionDenied if aNotify has the incorrect SID for aPid
			 a system-wide error code
	@panic if Open() is not called before any other RAvctp method
	@capability the SID of the process holding the RAvctp is checked if it is allowed to access aPid. 
	@panic if the RAvctp object is already open
*/
	IMPORT_C TInt Open(MAvctpEventNotify& aNotify, SymbianAvctp::TPid aPid, MAvctpChannel*& aPrimaryChannel);

/**
	Closes the RAvctp session by immediately disconnecting all remote devices it is explicitly connected to, 
	cancelling all outstanding requests and freeing all resources associated with it. This method deregisters
	aNotify from receiving any more AVCTP event notifications.  The client will have to Open() this object 
	before it can be used again.
	
	This function is safe to call at almost any time.  The only case when this is not appropriate is
	if a secondary channel is installed in a different thread AND that thread has it's own heap. For
	this case the UninstallSecondaryChannel function must be called in the thread running the secondary
	channel BEFORE this function is called.
	
	@see RAvctp::UninstallSecondaryChannel
	
	@param aImmediacy if the default, ENormal, is used then this function will shutdown gracefully.
					  if EImmediate is given then RAvctp will close as quickly as possible which may result
					  in some data not actually being sent that the client had received a successful SendComplete.
*/
	IMPORT_C void Close(TCloseType aImmediacy = ENormal);

/**
	This function is not currently supported.  Use of it will result in MaenErrorNotify being called
	with an error of KErrNotSupported.
*/
	IMPORT_C void CloseGracefully();
	
/**
	Installs the second AVCTP channel.
*/
	IMPORT_C TInt InstallSecondaryChannel(MAvctpEventNotify& aSecondChannelObserver, MAvctpChannel*& aSecondaryChannel);
	
/**
	Uninstalls the second AVCTP channel.
	This can be used for when the AVCTP second channel is run in a different thread to the primary channel.
	
	@param aImmediacy if the default, ENormal, is used then this function will shutdown gracefully.
					  if EImmediate is given then the second channel will close as quickly as possible which may result
					  in some data not actually being sent that the client had received a successful SendComplete.
**/
	IMPORT_C void UninstallSecondaryChannel(TCloseType aImmediacy = ENormal);
private:
/**
	The copy constructor is made private to help ensure there is only ever one RAvctp object 
	per PID
*/
	RAvctp(const RAvctp& aObjectToCopy);

private:
	CAvctpBody* iBody;
	};

#endif // AVCTPSERVICES_H
