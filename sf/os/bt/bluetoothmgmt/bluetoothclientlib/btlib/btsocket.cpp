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

#include <bt_sock.h>
#include "btsocketpanic.h"
#include "btsockethelpers.h"
#include "e32cmn.h" 

#include <bluetooth/logger.h>



void Panic(TBTSocketPanics aCode)
	{
	_LIT(KPanicName, "BTSocket");
	User::Panic(KPanicName, aCode);
	}





//*****************//

//----
EXPORT_C CBluetoothSocket* CBluetoothSocket::NewL(MBluetoothSocketNotifier& aNotifier, 
												  RSocketServ& aServer, 
												  TUint aSockType,
												  TUint aProtocol)
/** Standard Bluetooth socket NewL constructor.

Opens a Bluetooth socket. The constructor identifies the server, socket type, and bluetooth 
protocol to be used for the socket, as well as an asynchronous notifier.
@pre A session on the socket server has been started.
@leave No memory, or error on opening socket.
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock).
@param aSockType One of a set of values defined in es_sock.h, for example KSockStream.
@param aProtocol An entry point into the Bluetooth stack, for example KL2CAP.
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self = NewLC(aNotifier, aServer, aSockType,aProtocol);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CBluetoothSocket* CBluetoothSocket::NewLC(MBluetoothSocketNotifier& aNotifier, 
												   RSocketServ& aServer,
												   TUint aSockType,
												   TUint aProtocol)
/** Standard Bluetooth socket NewLC constructor.

Opens a Bluetooth socket as with the NewL() above, but leaves the returned CBluetoothSocket
object on the cleanup stack.

@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aSockType One of a set of values defined in es_sock.h, for example KSockStream
@param aProtocol An entry point into the Bluetooth stack, for example KL2CAP
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self=new (ELeave) CBluetoothSocket(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL(aSockType, aProtocol);
	return self;
	}


EXPORT_C CBluetoothSocket* CBluetoothSocket::NewL(MBluetoothSocketNotifier& aNotifier, 
												  RSocketServ& aServer, 
												  TUint aSockType,
												  TUint aProtocol,
												  RConnection& aConnection)
/** Opens a Bluetooth socket on a specified connection.
@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aSockType One of a set of values defined in es_sock.h, for example KSockStream
@param aProtocol An entry point into the Bluetooth stack, for example KL2CAP
@param aConnection A socket server management interface for a connection
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self = NewLC(aNotifier, aServer, aSockType, aProtocol, aConnection);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CBluetoothSocket* CBluetoothSocket::NewLC(MBluetoothSocketNotifier& aNotifier, 
												   RSocketServ& aServer,
												   TUint aSockType,
												   TUint aProtocol,
												   RConnection& aConnection)
/** This constructor opens a Bluetooth socket on a specified connection.

Leaves the socket on the cleanup stack.

@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aSockType One of a set of values defined in es_sock.h, for example KSockStream
@param aProtocol An entry point into the Bluetooth stack, for example KL2CAP
@param aConnection A socket server management interface for a connection
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self=new (ELeave) CBluetoothSocket(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL(aSockType, aProtocol, aConnection);
	return self;
	}


EXPORT_C CBluetoothSocket* CBluetoothSocket::NewL(MBluetoothSocketNotifier& aNotifier, 
												  RSocketServ& aServer,
												  const TDesC& aName)
/** Bluetooth socket named protocol NewL() constructor.

Opens a Bluetooth socket over a specified named protocol, such as "L2CAP".
@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aName A descriptor containing the name of the protocol wanted for
making a Bluetooth connection, for example _L("L2CAP")
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self = NewLC(aNotifier, aServer, aName);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CBluetoothSocket* CBluetoothSocket::NewLC(MBluetoothSocketNotifier& aNotifier, 
												   RSocketServ& aServer,
												   const TDesC& aName)
/** Bluetooth socket named protocol NewLC() constructor.

Opens a Bluetooth socket over a specified named protocol, such as "L2CAP".
Leaves returned CBluetoothSocket object on the cleanup stack.

@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aName A descriptor containing the name of the protocol wanted for
making a Bluetooth connection, for example _L("L2CAP")
@return The Bluetooth socket created with the constructor.
@capability LocalServices
*/
	{
	CBluetoothSocket* self=new (ELeave) CBluetoothSocket(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}


EXPORT_C CBluetoothSocket* CBluetoothSocket::NewL(MBluetoothSocketNotifier& aNotifier, RSocketServ& aServer)
/** Bluetooth blank socket NewL() constructor.

Opens a blank socket to be used when accepting an incoming connection.
This socket should be used as the parameter when a listening socket
calls CBluetoothSocket::Accept(CBluetoothSocket& aBlankSocket). When 
that connection completes this blank socket becomes the Bluetooth 
socket for that connection.

@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@return The Bluetooth socket created with the constructor.
*/
	{
	CBluetoothSocket* self = NewLC(aNotifier, aServer);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CBluetoothSocket* CBluetoothSocket::NewLC(MBluetoothSocketNotifier& aNotifier, RSocketServ& aServer)
/** Bluetooth blank socket NewLC() constructor

Opens a blank socket to be used when accepting an incoming connection.
This socket should be used as the parameter when a listening socket
calls CBluetoothSocket::Accept(CBluetoothSocket& aBlankSocket). When 
that connection completes this blank socket becomes the Bluetooth 
socket for that connection.

Leaves returned CBluetoothSocket object on the cleanup stack.

@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@return The Bluetooth socket created with the constructor.
*/
	{
	CBluetoothSocket* self=new (ELeave) CBluetoothSocket(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CBluetoothSocket* CBluetoothSocket::NewL(MBluetoothSocketNotifier& aNotifier,
												  RSocketServ& aServer,
												  RSocket& aSocket)
/** Bluetooth socket using an existing RSocket instance constructor.

Takes ownership of an existing socket to be used as the underlying
connection for this Socket wrapper.  This should be used where an
existing API returned a RSocket representing a Bluetooth connection.

@pre A session on the socket server has been started, and aSocket is open and is
set as "Transferable" through the KSOEnableTransfer SetOpt. 
@leave No memory
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aSocket A handle to an existing socket.
@return The Bluetooth socket created with the constructor.
*/
	{
	CBluetoothSocket* self = NewLC(aNotifier, aServer, aSocket);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CBluetoothSocket* CBluetoothSocket::NewLC(MBluetoothSocketNotifier& aNotifier, 
												   RSocketServ& aServer,
												   RSocket& aSocket)
/** Bluetooth socket using an existing RSocket instance constructor.

Takes ownership of an existing socket to be used as the underlying
connection for this Socket wrapper.  This should be used where an
existing API returned a RSocket representing a Bluetooth connection.

Leaves returned CBluetoothSocket object on the cleanup stack.

@pre A session on the socket server has been started, and aSocket is open and is
set as "Transferable" through the KSOEnableTransfer SetOpt.
@leave No memory
@param aNotifier The object which will receive asynchronous events.
@param aServer A handle to an existing session on the socket server (ESock)
@param aSocket A handle to an existing socket.
@return The Bluetooth socket created with the constructor.
*/
	{
	CBluetoothSocket* self=new (ELeave) CBluetoothSocket(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL(aSocket);
	return self;
	}
	
EXPORT_C CBluetoothSocket::~CBluetoothSocket()
/** Destructor

Cancels any outstanding Bluetooth socket functions and closes the socket.
Note sockets should be closed or shutdown before destructor is called.
*/
	{
	delete iAutoSniffDelayTimer;
	delete iBTConnecter;
	delete iBTAccepter;
	delete iBTShutdowner;
	delete iBTReceiver;
	delete iBTSender;
	delete iBTIoctler;
	delete iBTBasebandChangeEventNotifier;
	if(Socket().SubSessionHandle())
		{
		Socket().Close();
		}
	delete iBTBasebandManager; // Unregisters any basebands it has registered
	delete iBTBasebandChangeEventDelegate;
	delete iAsyncDestroyer;
	}


//RSocket functions
EXPORT_C TInt CBluetoothSocket::Send(const TDesC8& aDesc,TUint someFlags)
/** Sends data to a remote bluetooth device.

The Send() operation checks that another Send() operation isn't already in
progress before sending.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed.
This is done in the function MBluetoothSocketNotifier::HandleSendCompleteL(TInt aErr).

If Automatic Sniff Mode is active, using this function will not automatically put the link
into active mode. If it is required that the link be taken out of sniff mode automatically
when data is to be sent then the CBluetoothSocket::Write(const TDesC8& aDesc) function
should be used instead.

@param aDesc A descriptor for the data being sent to the remote device.
@param someFlags Bluetooth specific flags.
@return KErrNone meaning the operation was successful and data is being sent to the remote device,
        or KErrInUse meaning a previous Send() is not yet finished.
@see RSocket::Send(const TDesC8& aDesc,TUint someFlags,TRequestStatus& aStatus)
*/
	{
	TInt rerr = KErrNone;
	if(iSending)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTSender->Send(aDesc, someFlags);
		iSending = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::Send(const TDesC8& aDesc,TUint someFlags,TSockXfrLength& aLen)
/**  Sends data to a remote bluetooth device.

The Send() operation checks that another Send() operation isn't already in
progress before sending.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function
MBluetoothSocketNotifier::HandleSendCompleteL(TInt aErr)

If Automatic Sniff Mode is active, using this function will not automatically put the link
into active mode. If it is required that the link be taken out of sniff mode automatically
when data is to be sent then the CBluetoothSocket::Write(const TDesC8& aDesc) function
should be used instead.

@param aDesc A descriptor for the data being sent to the remote device.
@param someFlags Bluetooth specific flags.
@param aLen Amount of data being sent.
@return KErrNone meaning the operation was successful and data is being sent to the remote device,
        or KErrInUse meaning a previous Send() is not yet finished.
@see RSocket::Send(const TDesC8& aDesc,TUint someFlags,TSockXfrLength& aLen)
*/
	{
	TInt rerr = KErrNone;
	if(iSending)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTSender->Send(aDesc, someFlags, aLen);
		iSending = ETrue;
		}
	return rerr;
	}

EXPORT_C void CBluetoothSocket::CancelSend()
/** Cancels an outstanding Bluetooth Send() operation.

Calling the function will cause an outstanding Bluetooth Send() 
operation to complete prematurely.

@see RSocket::CancelSend()
*/
	{
	if(iSending)
		{
		iBTSender->Cancel(); //cancels 'Write' or 'Send' as appropriate
		iSending = EFalse;
		}
	}

EXPORT_C TInt CBluetoothSocket::Recv(TDes8& aDesc,TUint flags)
/** Receives data from a remote Bluetooth device.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
@param aDesc A descriptor for the information being sent to the remote Bluetooth device.
@param flags Bluetooth information flags.
@return KErrNone meaning the operation was successful and data is being received from the remote device,
        or KErrInUse meaning a previous Recv() is not yet finished.
@see RSocket::Recv(TDes8& aDesc,TUint flags)
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->Recv(aDesc, flags);
		iReceiving = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::Recv(TDes8& aDesc,TUint flags, TSockXfrLength& aLen)
/** Receives data from a remote Bluetooth device.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
@param aDesc A descriptor for the information being sent to the remote Bluetooth device.
@param flags Bluetooth information flags.
@param aLen A length indicating how much data was read. This is the same as length of the returned aDesc.
@return KErrNone meaning the operation was successful and data is being received from the remote device,
        or KErrInUse meaning a previous Recv() is not yet finished.
@see RSocket::Recv(TDes8& aDesc,TUint flags, TSockXfrLength& aLen)
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->Recv(aDesc, flags, aLen);
		iReceiving = ETrue;	
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::RecvOneOrMore(TDes8& aDesc,TUint flags,TSockXfrLength& aLen)
/** Receives data from a remote Bluetooth device, completing when data is available.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr).
@param aDesc A descriptor for the information being sent to the remote Bluetooth device.
@param flags Bluetooth information flags.
@param aLen A length indicating how much data was read. This is the same as length of the returned aDesc.
@return KErrNone meaning the operation was successful and data is being received from the remote device,
        or KErrInUse meaning a previous Recv() is not yet finished.
@see RSocket::RecvOneOrMore(TDes8& aDesc,TUint flags,TSockXfrLength& aLen)
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->RecvOneOrMore(aDesc, flags, aLen);
		iReceiving = ETrue;
		}
	return rerr;
	}

EXPORT_C void CBluetoothSocket::CancelRecv()
/** Cancels an outstanding Recv() operation. 

Calling this function will cause any outstanding receive operation to cancel.

@see RSocket::CancelRecv()
*/
	{
	if(iReceiving)
		{
		iBTReceiver->Cancel(); //cancels 'Read' or 'Recv' as appropriate
		iReceiving = EFalse;
		}
	}

EXPORT_C TInt CBluetoothSocket::Read(TDes8& aDesc)
/** Receives data from a remote Bluetooth host.

Read() is only used with a connected Bluetooth host.

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
@param aDesc A descriptor for the data being read.
@return KErrNone meaning the operation was successful and data is being received from the remote device,
        or KErrInUse meaning a previous Recv() is not yet finished.
@see RSocket::Read(TDes8& aDesc)
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->Read(aDesc);
		iReceiving = ETrue;
		}
	return rerr;
	}

EXPORT_C void CBluetoothSocket::CancelRead()
/** Cancels an outstanding Read() operation. 

Calling this function will cause any outstanding Read() operation to cancel.
@see RSocket::CancelRead()
*/
	{
	if(iReceiving)
		{
		iBTReceiver->Cancel(); //cancels 'Read' or 'Recv' as appropriate
		iReceiving = EFalse;
		}
	}

EXPORT_C TInt CBluetoothSocket::Write(const TDesC8& aDesc)
/** Writes to a remote Bluetooth device

@see RSocket::Write(const TDesC8& aDesc)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleSendCompleteL(TInt aErr)

If Automatic Sniff Mode is active on this socket instance, then upon calling this function
the link will be put into active mode.  If no CBluetoothSocket::Write() calls are made
for a period of the timeout specified during activation of the automatic sniffing then
the link will be put back into sniff mode.
@see CBluetoothSocket::SetAutomaticSniffMode(TBool aAutoSniffMode, TInt aIdleSecondsBeforeSniffRequest)
@see CBluetoothSocket::SetAutomaticSniffMode(TBool aAutoSniffMode)

@param aDesc A descriptor for the data being sent to the remote Bluetooth device.
@return KErrNone meaning the operation was successful and data is being sent to the remote device,
        or KErrInUse meaning a previous Write() is not yet finished.
*/
	{
	TInt rerr = KErrNone;
	if(iSending)
		{
		rerr = KErrInUse;
		}
	else
		{
		if(iAutoSniffDelayTimer)
			{
			iAutoSniffDelayTimer->StartActive();
			}
		iBTSender->Write(aDesc);
		iSending = ETrue;
		}
	return rerr;
	}

EXPORT_C void CBluetoothSocket::CancelWrite()
/** Cancels an outstanding Write() operation.

Calling this operation will cause any outstanding Write() operation to cancel.
@see RSocket::CancelWrite()
*/
	{
	if(iSending)
		{
		iBTSender->Cancel(); //cancels 'Write' or 'Send' as appropriate
		iSending = EFalse;
		}
	}

EXPORT_C TInt CBluetoothSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags)
/** Sends the aDesc data to the aAddr specified and applies the flags indicated to the operation.

@see RSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleSendCompleteL(TInt aErr)

If Automatic Sniff Mode is active, using this function will not automatically put the link
into active mode. If it is required that the link be taken out of sniff mode automatically
when data is to be sent then the CBluetoothSocket::Write(const TDesC8& aDesc) function
should be used instead.

@param aDesc A descriptor for the data being sent.
@param aAddr The address of the Bluetooth device.
@param flags The Bluetooth flags.
@return KErrNone meaning the operation was successful and data is being sent to the specified 
        and addressed remote device, or KErrInUse meaning a previous SendTo() is not yet finished.
*/
	{
	TInt rerr = KErrNone;
	if(iSending)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTSender->SendTo(aDesc, aAddr, flags);
		iSending = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen)
/**  Sends the aDesc data to the aAddr specified and applies the flags indicated to the operation.

@see RSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleSendCompleteL(TInt aErr)

If Automatic Sniff Mode is active, using this function will not automatically put the link
into active mode. If it is required that the link be taken out of sniff mode automatically
when data is to be sent then the CBluetoothSocket::Write(const TDesC8& aDesc) function
should be used instead.

@param aDesc A descriptor for the data being sent.
@param aAddr The address of the Bluetooth device.
@param flags The Bluetooth flags.
@param aLen An integer representing the length of the message.
@return KErrNone meaning the operation was successful and data is being sent to the specified 
        and addressed remote device, or KErrInUse meaning a previous SendTo() is not yet finished.
*/
	{
	TInt rerr = KErrNone;
	if(iSending)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTSender->SendTo(aDesc, aAddr, flags, aLen);
		iSending = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags)
/** Receives aDesc data from the named remote aAddr Bluetooth device using the flags indicated. 

@see RSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
@param aDesc A descriptor for the data being received.
@param aAddr The address of the Bluetooth device.
@param flags The Bluetooth flags.
@return KErrNone meaning the operation was successful and data is being received from the specified 
        and addressed remote device, or KErrInUse meaning a previous RecvFrom() is not yet finished.
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->RecvFrom(aDesc, aAddr, flags);
		iReceiving = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen)
/** Receives aDesc data from the named remote aAddr Bluetooth device using the flags indicated. 

@see RSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
@param aDesc A descriptor for the data being received.
@param aAddr The address of the Bluetooth device.
@param flags The Bluetooth flags.
@param aLen An integer representing the length of the message.
@return KErrNone meaning the operation was successful and data is being received from the specified 
        and addressed remote device, or KErrInUse meaning a previous RecvFrom() is not yet finished.
*/
	{
	TInt rerr = KErrNone;
	if(iReceiving)
		{
		rerr = KErrInUse;
		}
	else
		{
		iBTReceiver->RecvFrom(aDesc, aAddr, flags, aLen);
		iReceiving = ETrue;
		}
	return rerr;
	}

EXPORT_C TInt CBluetoothSocket::Connect(TBTSockAddr& aAddr)
/** Opens a connection to a specific Bluetooth device by its address.

The address represents the address of the remote Bluetooth device. A socket may
only have one connect operation outstanding at any one time. Once the connect 
is completed, the socket is ready to send or receive data. If a socket is 
unbound - i.e. Bind() has not been called yet - then it will automatically have
a local address allocated.

@see RSocket::Connect(TBTSockAddr& aAddr)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleConnectCompleteL(TInt aErr)
@param aAddr The address of the bluetooth device to which the connection is being made.
@return KErrInUse if the device is already being used, else KErrNone is the connection is successful.
*/
	{
	if(iBTConnecter)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTConnecter = CBTConnecter::NewL(*this));
	if(err == KErrNone)
		{
		iBTConnecter->Connect(aAddr);
		}
	return err;
	}

EXPORT_C TInt CBluetoothSocket::Connect(TBTSockAddr& aAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn)
/** Connects to a remote Bluetooth device by address with a defined data-out and data-in descriptor.

The address provided specifies the address of the remote Bluetooth host. Data can be sent in 
connect request packets, which may be provided in the data-out descriptor, or connect responses, which may be collected in the data-in descriptor.

A socket may only have one connect operation outstanding at any one time. Once the connect is completed, the socket is ready to send or receive data. If a socket is unbound - i.e. Bind() has not been called yet - then it will automatically have a local address allocated.

@see RSocket::Connect(TBTSockAddr& aAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed.
This is done in the function MBluetoothSocketNotifier::HandleConnectCompleteL(TInt aErr).
@param aAddr The address of the remote Bluetooth device. 
@param aConnectDataOut A descriptor containing data to be sent.
@param aConnectDataIn A descriptor to receive data.
@return KErrInUse if the device is already being used, else KErrNone is the connection is successful.
*/
	{
	if(iBTConnecter)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTConnecter = CBTConnecter::NewL(*this));
	if(err == KErrNone)
		{
		iBTConnecter->Connect(aAddr, aConnectDataOut, aConnectDataIn);
		}
	return err;
	}

EXPORT_C TInt CBluetoothSocket::Connect(TBTSockAddr& aAddr, TUint16 aServiceBits)
/** Sets the Service bits in the Class Of Device. If this is successful, the socket then opens
a connection to a specific Bluetooth device by its address.

The address represents the address of the remote Bluetooth device. A socket may
only have one connect operation outstanding at any one time. Once the connect 
is completed, the socket is ready to send or receive data. If a socket is 
unbound - i.e. Bind() has not been called yet - then it will automatically have
a local address allocated.

@see RSocket::Connect(TBTSockAddr& aAddr)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleConnectCompleteL(TInt aErr)
@param aAddr The address of the bluetooth device to which the connection is being made.
@return KErrInUse if the device is already being used, else KErrNone is the connection is successful.
*/
	{
	TInt err = KErrNone;
	err = iSocket.SetOpt(KBTRegisterCodService, KSolBtSAPBase, aServiceBits);
	if(err == KErrNone)
		{
		if(iBTConnecter)
			{
			return KErrInUse;
			}

		TRAPD(err, iBTConnecter = CBTConnecter::NewL(*this));
		if(err == KErrNone)
			{
			iBTConnecter->Connect(aAddr);
			}
		}
	return err;
	}

EXPORT_C void CBluetoothSocket::CancelConnect()
/** Cancel an attempted connection.

@see RSocket::CancelConnect()
*/
	{
	if(iBTConnecter)
		{
		delete iBTConnecter;
		iBTConnecter = NULL;
		}
	}

EXPORT_C TInt CBluetoothSocket::Bind(TSockAddr& aAddr)
/** Sets a Bluetooth socket address.

Sets the local address of a socket. When a socket is opened it has no name associated
with it, and binding is required so data can be routed to the socket. Bind() should be
called before Listen() or Connect().

@param aAddr The address of the socket.
@see RSocket::Bind(TSockAddr& aAddr)
@return aAddr The address of the socket.
*/
	{
	return iSocket.Bind(aAddr);
	}

EXPORT_C TInt CBluetoothSocket::SetLocalPort(TInt aPort)
/** Sets the local port of a Bluetooth socket.

Setting the local port is equivalent to calling Bind() with only the port set in the address.

@see RSocket::SetLocalPort(TInt aPort)
@param aPort The socket port.
@return aPort The socket port.
*/
	{
	return iSocket.SetLocalPort(aPort);
	}

EXPORT_C TInt CBluetoothSocket::Accept(CBluetoothSocket& aBlankSocket)
/** Accepts a connection from a remote Bluetooth socket.

The call extracts the first pending connection on a queue of sockets, the queue
size being previously specified by Listen(). On successful completion the blank
socket is given the handle of the new socket and it may then be used to transfer
data. After completion the accept socket may be used to make further connections
with new blank sockets (see Open() on how to open a blank socket).

@see RSocket::Accept(CBluetoothSocket& aBlankSocket)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function MBluetoothSocketNotifier::HandleAcceptCompleteL(TInt aErr)
@param aBlankSocket A socket opened as a blank socket.
@return KErrNone if the connection is established. KErrInUse if a connection already exists.
*/
	{
	if(iBTAccepter)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTAccepter = CBTAccepter::NewL(*this, aBlankSocket));
	if(err == KErrNone)
		{
		iBTAccepter->Accept();
		}
	return err;
	}

EXPORT_C TInt CBluetoothSocket::Accept(CBluetoothSocket& aBlankSocket,TDes8& aConnectData)
/** Reserved for future.

@param aBlankSocket a socket opened as a blank socket.
@param aConnectData Data that may be received in connection.
@return KErrNone if the connection is established. KErrInUse if a connection already exists.
*/
	{
	if(iBTAccepter)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTAccepter = CBTAccepter::NewL(*this, aBlankSocket));
	if(err == KErrNone)
		{
		iBTAccepter->Accept(aConnectData);
		}
	return err;
	}

EXPORT_C void CBluetoothSocket::CancelAccept()
/** Cancels the Accept() operation.
@see RSocket::CancelAccept()
*/
	{
	if(iBTAccepter)
		{
		delete iBTAccepter;
		iBTAccepter = NULL;
		}
	}

EXPORT_C TInt CBluetoothSocket::Listen(TUint qSize)
/** Sets up a socket to listen for incoming connections.

Before calling this procedure a socket should be opened using Open() and be bound 
to a local address using Bind().

Listen() creates a queue to hold incoming connections which can be married with
blank sockets using Accept().

Once a listen queue has been created it will continue to allow peers to connect until
it is full, at which point it will reject any incoming connections as specified by
protocol behaviour. When a socket is accepted by the client a space is made
available in the queue.

@see RSocket::Listen(TUint qSize)
@param qSize The number of connections allowed in the queue.
@return The 'listen' queue.
*/
	{
	return iSocket.Listen(qSize);
	}

EXPORT_C TInt CBluetoothSocket::Listen(TUint qSize,const TDesC8& aConnectData)
/** Reserved for future.

@see RSocket::Listen(TUint qSize,const TDesC8& aConnectData)
@param qSize The size of the 'listen' queue.
@param aConnectData The descriptor for data sent in connection respoonses.
@return The 'listen' queue.
*/
	{
	return iSocket.Listen(qSize,aConnectData);
	}

EXPORT_C TInt CBluetoothSocket::Listen(TUint qSize, TUint16 aServiceBits)
/** Sets the Service bits in the Class Of Device. If this is successful, the socket
will then be set to Listen for incoming connections.

Before calling this procedure a socket should be opened using Open() and be bound 
to a local address using Bind().

Listen() creates a queue to hold incoming connections which can be married with
blank sockets using Accept().

Once a listen queue has been created it will continue to allow peers to connect until
it is full, at which point it will reject any incoming connections as specified by
protocol behaviour. When a socket is accepted by the client a space is made
available in the queue.

@see RSocket::Listen(TUint qSize)
@param qSize The number of connections allowed in the queue.
@return The 'listen' queue.
*/
	{
	TInt err = KErrNone;
	err = iSocket.SetOpt(KBTRegisterCodService, KSolBtSAPBase, aServiceBits);
	if(err == KErrNone)
		{
		return iSocket.Listen(qSize);
		}
	return err;
	}

EXPORT_C TInt CBluetoothSocket::SetOption(TUint aOptionName, TUint aOptionLevel,const TDesC8& aOption)
/** Sets a socket option.

The socket server has options which are generic to all sockets and protocols may add specific options.

Options available for all protocols can be set with anOptionLevel set to KSOLSocket. See individual protocol notes for other socket options. 

@see RSocket::SetOpt(TUint aOptionName, TUint aOptionLevel,const TDesC8& aOption)
@param aOptionName An option identifier.
@param aOptionLevel An option level. Option levels 'group' related options.
@param aOption The option value as a descriptor.
@return The socket options set.
*/
	{
	return iSocket.SetOpt(aOptionName, aOptionLevel,aOption);
	}
	
EXPORT_C TInt CBluetoothSocket::SetOpt(TUint aOptionName, TUint aOptionLevel,const TDesC8& aOption)
/** Sets a socket option.

The socket server has options which are generic to all sockets and protocols may add specific options.

Options available for all protocols can be set with anOptionLevel set to KSOLSocket. See individual protocol notes for other socket options. 

@see RSocket::SetOpt(TUint aOptionName, TUint aOptionLevel,const TDesC8& aOption)
@param aOptionName An option identifier.
@param aOptionLevel An option level. Option levels 'group' related options.
@param aOption The option value as a descriptor.
@return The socket options set.
@deprecated
*/
	{
	return iSocket.SetOpt(aOptionName, aOptionLevel,aOption);
	}

EXPORT_C TInt CBluetoothSocket::SetOpt(TUint aOptionName,TUint aOptionLevel,TInt aOption)
/**  Sets a socket option.

The socket server has options which are generic to all sockets and protocols may add specific options.

Options available for all protocols can be set with anOptionLevel set to KSOLSocket. See individual protocol notes for other socket options. 

@see RSocket::SetOpt(TUint aOptionName, TUint aOptionLevel,const TDesC8& aOption)
@param aOptionName An option identifier.
@param aOptionLevel An option level. Option levels 'group' related options.
@param aOption The option value as an integer.
@return The socket options set.
*/
	{
	return iSocket.SetOpt(aOptionName, aOptionLevel,aOption);
	}

EXPORT_C TInt CBluetoothSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption)
/** Gets a socket option.

The socket server has options which are generic to all sockets and protocols may add specific options.

Options available for all protocols can be got with anOptionLevel set to KSOLSocket. See individual protocol notes for other socket options. 

@see RSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption)
@param aOptionName An option identifier.
@param aOptionLevel An option level. Option levels 'group' related options.
@param aOption The option value as a descriptor.
@return The socket options set.
*/
	{
	return iSocket.GetOpt(aOptionName, aOptionLevel, aOption);
	}

EXPORT_C TInt CBluetoothSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TInt &aOption)
/** Gets a socket option.

The socket server has options which are generic to all sockets and protocols may add specific options.

Options available for all protocols can be got with anOptionLevel set to KSOLSocket. See individual protocol notes for other socket options. 

@see RSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption)
@param aOptionName An option identifier.
@param aOptionLevel An option level. Option levels 'group' related options.
@param aOption The option value as an integer.
@return The socket options set.
*/
	{
	return iSocket.GetOpt(aOptionName, aOptionLevel, aOption);
	}

EXPORT_C TInt CBluetoothSocket::Ioctl(TUint aLevel, TUint aCommand,TDes8* aDesc)
/** Applies an asynchronous control operation on a socket.

Data may be passed and received if a descriptor address is provided as an argument.
Only one Ioctl() operation may be outstanding for each socket.

@see RSocket::Ioctl(TUint aCommand,TDes8* aDesc,TUint aLevel)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleIoctlCompleteL(TInt aErr)
@param aCommand ioctl command.
@param aDesc Pointer to a descriptor in which data may be sent and received on completion.
@param aLevel Control operation level.
@return KErrNone if the ioctl command is applied or 
        KErrInUse if another ioctl command is still being used.
*/
	{
	if(iBTIoctler)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTIoctler = CBTIoctler::NewL(*this));
	if(err == KErrNone)
		{
		iBTIoctler->Ioctl(aCommand, aDesc, aLevel);
		}
	return err;
	}
	
EXPORT_C TInt CBluetoothSocket::Ioctl(TUint aCommand,TDes8* aDesc,TUint aLevel)
/** Applies an asynchronous control operation on a socket.

Data may be passed and received if a descriptor address is provided as an argument.
Only one Ioctl() operation may be outstanding for each socket.

@see RSocket::Ioctl(TUint aCommand,TDes8* aDesc,TUint aLevel)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleIoctlCompleteL(TInt aErr)
@param aCommand ioctl command.
@param aDesc Pointer to a descriptor in which data may be sent and received on completion.
@param aLevel Control operation level.
@return KErrNone if the ioctl command is applied or 
        KErrInUse if another ioctl command is still being used.
@deprecated
*/
	{
	if(iBTIoctler)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTIoctler = CBTIoctler::NewL(*this));
	if(err == KErrNone)
		{
		iBTIoctler->Ioctl(aCommand, aDesc, aLevel);
		}
	return err;
	}

EXPORT_C void CBluetoothSocket::CancelIoctl()
/** Cancels the Ioctl() asynchronous control operation.
@see RSocket::CancelIoctl()
*/
	{
	if(iBTIoctler)
		{
		delete iBTIoctler;
		iBTIoctler = NULL;
		}
	}

EXPORT_C TInt CBluetoothSocket::GetDisconnectData(TDes8& aDesc)
/** 
@see RSocket::GetDisconnectData(TDes8& aDesc)
@param aDesc A descriptor for the disconnect data.
@return KErrNone if successful.
*/
	{
	return iSocket.GetDisconnectData(aDesc);
	}

EXPORT_C void CBluetoothSocket::LocalName(TSockAddr& aAddr)
/** Gets the local address of a socket.

The local address is set either by calling Bind() or it is automatically set when Connect()
is called. If a socket is created through Accept() then a socket will inherit the port of
its parent unless otherwise specified by a protocol's behaviour.
@see RSocket::LocalName(TSockAddr& aAddr)
@param aAddr The socket address.
*/
	{
	iSocket.LocalName(aAddr);
	}

EXPORT_C TUint CBluetoothSocket::LocalPort()
/** Gets the local port of a socket.

Similar to LocalName().

@see RSocket::LocalPort()
@return The local port of the socket.
*/
	{
	return iSocket.LocalPort();
	}

EXPORT_C void CBluetoothSocket::RemoteName(TSockAddr& aAddr)
/** Gets the remote address of a socket.

The remote name (address) of a socket is associated with the remote host the
socket is connected to. The remote name is only valid for a connected socket.
A socket is either connected through calling Connect() or Accept().

@see RSocket::RemoteName(TSockAddr& aAddr)
@param aAddr The remote socket address.
*/
	{
	iSocket.RemoteName(aAddr);
	}

EXPORT_C TInt CBluetoothSocket::Shutdown(RSocket::TShutdown aHow)
/** Shuts down a connected socket. (The socket cannot be null. @see 
CBluetoothSocket::Info.)

The shutdown method allows input and output to be individually stopped for
a protocol endpoint.

@see RSocket::Shutdown(RSocket::TShutdown aHow)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleShutdownCompleteL(TInt aErr)
@param aHow Shutdown option. All variants complete when a socket is disconnected.
@return KErrNone if successful or 
        KErrInUse if the system is already trying to shutdown a connected socket.
*/
	{
	if(iBTShutdowner)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTShutdowner = CBTShutdowner::NewL(*this));
	if(err == KErrNone)
		{
		// AutomaticSniff mode should be unset, to avoid situation 
		// when sniff request is sent while we are in the middle of disconnection.
		if ( AutomaticSniffMode() )
			{
			(void) SetAutomaticSniffMode(EFalse);
			}
		// If we are in one of LMP, in order to disconnect, link needs 
		// to be put into acitve mode. It can happen that before link 
		// disconnects LMP requester acknowledges that link is in active mode, 
		// and will send LMP request. Therefore all LMP requesters should 
		// be cancelled before disconnecting socket.		
		else
			{
			(void) CancelLowPowerModeRequester();
			}
		iBTShutdowner->Shutdown(aHow);
		}
	return err;
	}

EXPORT_C TInt CBluetoothSocket::Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn)
/** Reserved for future.
The socket must not be null. (@see CBluetoothSocket::Info.)
@see RSocket::Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn)

When this request completes, notification is sent to the MBluetoothSocketNotifier
object	supplied when this CBluetoothSocket was constructed. 
This is done in the function 
MBluetoothSocketNotifier::HandleShutdownCompleteL(TInt aErr)
@param aHow Shutdown option. All variants complete when a socket is disconnected.
@param aDisconnectDataOut A descriptor to send data.
@param aDisconnectDataIn A descriptor to recive data.
@return KErrNone if successful or 
        KErrInUse if the system is already trying to shutdown a connected socket.
*/
	{
	if(iBTShutdowner)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTShutdowner = CBTShutdowner::NewL(*this));
	if(err == KErrNone)
		{
		// AutomaticSniff mode should be unset, to avoid situation 
		// when sniff request is sent while we are in the middle of disconnection.
		if ( AutomaticSniffMode() )
			{
			(void) SetAutomaticSniffMode(EFalse);
			}
		// If we are in one of LMP, in order to disconnect, link needs 
		// to be put into acitve mode. It can happen that before link 
		// disconnects LMP requester acknowledges that link is in active mode, 
		// and will send LMP request. Therefore all LMP requesters should 
		// be cancelled before disconnecting socket.		
		else
			{
			(void) CancelLowPowerModeRequester();
			}
		iBTShutdowner->Shutdown(aHow, aDisconnectDataOut, aDisconnectDataIn);
		}
	return err;
	}

EXPORT_C void CBluetoothSocket::CancelAll()
/** Cancels all outstanding operations. 

Calling it will cause all outstanding operations to cancel. Outstanding operations
for a socket include: read, write, Ioctl, connect, accept, shutdown and the Baseband 
event notifier. All of these operations will be completed by this call.
*/
	{
	//Sort any active objects
	delete iBTConnecter;
	iBTConnecter = NULL;
	delete iBTAccepter;
	iBTAccepter = NULL;
	delete iBTIoctler;
	iBTIoctler = NULL;
	delete iBTBasebandChangeEventNotifier;
	iBTBasebandChangeEventNotifier = NULL;
	CancelSend(); //ao not deleted
	CancelRecv(); //ao not deleted
	// The shutdowner may need to be canceled.
//	if(iBTShutdowner)
//		{
//		iBTShutdowner->Cancel();
//		}

	iSocket.CancelAll(); //belt and braces
	}

EXPORT_C TInt CBluetoothSocket::Info(TProtocolDesc& aProtocol)
/** Gets information about the protocol the socket is opened on.
@see RSocket::Info(TProtocolDesc& aProtocol)
@param aProtocol The protocol on which the socket is opened. Note that if the 
socket is a null socket, and aProtocol is a null TProtocolDesc when it is 
passed in, if will be null on return as well.
@return KErrNone if successful.
*/
	{
	return iSocket.Info(aProtocol);
	}

EXPORT_C TInt CBluetoothSocket::Name(TName& aName)
/** Used to get a unique name of a socket.

The name is needed when Transfer() is called.

@see RSocket::Name(TName& aName)
@param aName The system name of a socket.
@return KErrNone if successful.
*/
	{
	return iSocket.Name(aName);
	}

EXPORT_C TInt CBluetoothSocket::Transfer(RSocketServ& aServer, const TDesC& aName)
/** Transfers a socket from one socket server session to another.

It creates the socket in the target session, and removes the socket from the source
session. The call is made on an uninitialised RSocket object. The socket system name
'Name()' is used to identify the socket to transfer. 

If the call fails, the socket that is being transferred remains with the original session. 
Success or failure can be checked on the originating socket by calling Info(), which returns
KErrNone if the transfer failed, and KErrBadHandle if it succeeded.

@see RSocket::Transfer(RSocketServ& aServer, const TDesC& aName)
@param aServer The session to which the socket is being transfered
@param aName The system name of the socket being transfered.
@return KErrNone if successful.
*/
	{
	return iSocket.Transfer(aServer,  aName);
	}


	
	

	
//RBTBaseband functions
//Getter
EXPORT_C TInt CBluetoothSocket::PhysicalLinkState(TUint32& aState)
/** Get the state of the physical link.
@see RBTPhysicalLinkAdapter::PhysicalLinkState(TUint32& aState)
@param aState The state of the physical link.
@return The state of the physical link.
*/
	{
	return iBTBaseband.PhysicalLinkState(aState);
	}



//Role change functions
EXPORT_C TInt CBluetoothSocket::PreventRoleSwitch()
/** Role switching is not allowed.
@see RBTPhysicalLinkAdapter::PreventRoleSwitch()
@return KErrNone if successful.
*/
	{
	return iBTBaseband.PreventRoleSwitch();
	}

EXPORT_C TInt CBluetoothSocket::AllowRoleSwitch()
/** Allow the socket to switch roles.
@see RBTPhysicalLinkAdapter::AllowRoleSwitch()
@return KErrNone if successful.
*/
	{
	return iBTBaseband.AllowRoleSwitch();
	}

EXPORT_C TInt CBluetoothSocket::RequestMasterRole()
/** Request the socket take on the master role.
@see RBTPhysicalLinkAdapter::RequestMasterRole()
@return KErrNone if successful.
*/
	{
	return iBTBaseband.RequestMasterRole();
	}

EXPORT_C TInt CBluetoothSocket::RequestSlaveRole()
/** Request the socket take on a slave role.
@see RBTPhysicalLinkAdapter::RequestSlaveRole()
@return KErrNone if successful.
*/
	{
	return iBTBaseband.RequestSlaveRole();
	}


//Low power mode functions
EXPORT_C TInt CBluetoothSocket::PreventLowPowerModes(TUint32 aLowPowerModes)
/** Prevents low power modes from being activated.
@see RBTPhysicalLinkAdapter::PreventLowPowerModes(TUint32 aLowPowerModes)
@param aLowPowerModes The low power mode being prevented.
@return KErrNone if successful.
		KErrInUse if the socket is using automatic sniff mode.
@panic	EInvalidOpWhenAutoSniffOn (10) This API is not allowed when AutoSniff is On
*/
	{
	TInt err(KErrNone);
	__ASSERT_ALWAYS(!AutomaticSniffMode(), Panic(EInvalidOpWhenAutoSniffOn));
	err = iBTBaseband.PreventLowPowerModes(aLowPowerModes);
	return err;
	}

EXPORT_C TInt CBluetoothSocket::AllowLowPowerModes(TUint32 aLowPowerModes)
/** Allow low power modes.

The power modes are indicated by aLowPowerModes.

@see RBTPhysicalLinkAdapter::AllowLowPowerModes(TUint32 aLowPowerModes)
@param aLowPowerModes The low power mode.
@return KErrNone if successful.
		KErrInUse if the socket is using automatic sniff mode.
@panic	EInvalidOpWhenAutoSniffOn (10) This API is not allowed when AutoSniff is On
*/
	{
	TInt err(KErrNone);
	__ASSERT_ALWAYS(!AutomaticSniffMode(), Panic(EInvalidOpWhenAutoSniffOn));
	err = iBTBaseband.AllowLowPowerModes(aLowPowerModes);
	return err;
	}


//Low power mode functions
EXPORT_C TInt CBluetoothSocket::ActivateSniffRequester()
/**
@see RBTPhysicalLinkAdapter::ActivateSniffRequester()
@return KErrNone if successful.
		KErrInUse if the socket is using automatic sniff mode.
@panic	EInvalidOpWhenAutoSniffOn (10) This API is not allowed when AutoSniff is On
*/
	{
	TInt err(KErrNone);
	__ASSERT_ALWAYS(!AutomaticSniffMode(), Panic(EInvalidOpWhenAutoSniffOn));
	err = iBTBaseband.ActivateSniffRequester();
	return err;
	}

EXPORT_C TInt CBluetoothSocket::ActivateParkRequester()
/** Activate park requestor.
@see RBTPhysicalLinkAdapter::ActivateParkRequester()
@return KErrNone if successful.
		KErrInUse if the socket is using automatic sniff mode.
@panic	EInvalidOpWhenAutoSniffOn (10) This API is not allowed when AutoSniff is On
*/
	{
	TInt err(KErrNone);
	__ASSERT_ALWAYS(!AutomaticSniffMode(), Panic(EInvalidOpWhenAutoSniffOn));
	err = iBTBaseband.ActivateParkRequester();
	return err;
	}

EXPORT_C TInt CBluetoothSocket::CancelLowPowerModeRequester()
/** Cancels the low power mode request.
@see RBTPhysicalLinkAdapter::CancelLowPowerModeRequester()
@return KErrNone if successful.
		KErrInUse if the socket is using automatic sniff mode.
@panic	EInvalidOpWhenAutoSniffOn (10) This API is not allowed when AutoSniff is On
*/
	{
	TInt err(KErrNone);
	__ASSERT_ALWAYS(!AutomaticSniffMode(), Panic(EInvalidOpWhenAutoSniffOn));
	err = iBTBaseband.CancelLowPowerModeRequester();
	return err;
	}


//Packet function
EXPORT_C TInt CBluetoothSocket::RequestChangeSupportedPacketTypes(TUint16 aPacketTypes)
/** Request to change the supported packet types.
@see RBTPhysicalLinkAdapter::RequestChangeSupportedPacketTypes(TUint16 aPacketTypes)
@param Integer representing the packet types.
@return KErrNone if successful.
*/
	{
	return iBTBaseband.RequestChangeSupportedPacketTypes(aPacketTypes);
	}


//Notification functions
EXPORT_C TInt CBluetoothSocket::ActivateBasebandEventNotifier(TUint32 aEventTypes)
/** Set up notification of baseband events

Elect to be notified whenever one of a user specified selection
of baseband events occurs.

The notification takes place in the pure virtual function 
MBluetoothSocketNotifier::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification)

The FIRST notification will take place ALMOST INSTANTANEOUSLY and will be 
A FULL DESCRIPTION (OR SNAPSHOT) OF THE CURRENT STATE OF THE PHYSICAL LINK 
irrespective of the specified selection of events wanted. This first notification or
snapshot has NOTHING to do with a baseband event occurring. Once this snapshot has 
taken place, all occurrences of the specified events from the instant of the snapshot 
onwards will be notified until CBluetoothSocket::CancelBasebandEventNotifier() is called.

@see PhysicalLinkState(TUint32& aState) if only a single full 
description of the physical link state is required.

@param aEventTypes bitmask for those events for which notification is being requested 
(Combine elements of TBTPhysicalLinkStateNotifier and TBTPhysicalLinkStateNotifierCombinations)
@see TBTPhysicalLinkStateNotifier
@see TBTPhysicalLinkStateNotifierCombinations
@return KErrNone if activated or KErrInUse if the baseband event notifier is already being used.
*/
	{
	if(iBTBasebandChangeEventNotifier)
		{
		return KErrInUse;
		}

	TRAPD(err, iBTBasebandChangeEventNotifier = CBTBasebandChangeEventNotifier::NewL(*iBTBasebandChangeEventDelegate));
	if(err == KErrNone)
		{
		iNotifierEventMask = aEventTypes;
		iBTBasebandChangeEventNotifier->SetNotify(iNotifierEventMask);
		}
	return err;
	}

EXPORT_C void CBluetoothSocket::CancelBasebandEventNotifier()
/** Cancels an active baseband notifier.

No further baseband events will be notified.

If this function is called when there is no active baseband notifier, 
it will do nothing.
*/
	{
	if(iBTBasebandChangeEventNotifier)
		{
		delete iBTBasebandChangeEventNotifier;//calls Cancel()
		iBTBasebandChangeEventNotifier = NULL;
		}
	}
	
EXPORT_C void CBluetoothSocket::SetNotifier(MBluetoothSocketNotifier& aNewNotifier)
/** Changes the owner (receiver of notifications) of the socket.

The previous notifier will not be deleted after the change has been made.

@param aNewNotifier The object which will receive asynchronous events.
*/
	{
	// We do not delete the current notifier as although it is stored as a
	// pointer we do not own it. (it is always passed to us by reference).
	iNotifier = &aNewNotifier;
	}

EXPORT_C TInt CBluetoothSocket::SetAutomaticSniffMode(TBool aAutoSniffMode)
/** Sets up automatic sniff/unsniff requesting for the socket. 

If Automatic Sniff requesting is being selected, a timer with default delay in seconds before the idle socket will
request the link to go into sniff mode. The CBluetoothSocket::Write() method will clear an outstanding sniff
requester and start the CAutoSniffDelayTimer active object which will issue another request for sniff mode once it
times out.

If Automatic Sniff requesting is being deselected, a previous sniff requester will be cancelled.

If Automatic Sniff requesting is selected when it is already active, no change will be performed and KErrInUse will
be returned

@param aAutoSniffMode ETrue if Automatic Sniff/Unsniff requests are required.
@return KErrNone if the automatic sniff mode has been successfully changed.
		KErrInUse if automatic sniff mode is requested, when it is already active.
		KErrArgument if an invalid idle timer value for the sniff request is provided.
		A Symbian wide error code for another other error that might have occurred.
*/
	{
	const TInt KDefaultSecondsBeforeSniffRequest = 5;
	return SetAutomaticSniffMode(aAutoSniffMode, KDefaultSecondsBeforeSniffRequest);
	}

EXPORT_C TInt CBluetoothSocket::SetAutomaticSniffMode(TBool aAutoSniffMode, TInt aIdleSecondsBeforeSniffRequest)
/** Sets up automatic sniff/unsniff requesting for the socket. 

If Automatic Sniff requesting is being selected, supply the delay in seconds required before the idle socket will 
request the link to go into sniff mode. The CBluetoothSocket::Write() method will clear an outstanding sniff 
requester and start the CAutoSniffDelayTimer active object which will issue another request for sniff mode once 
it times out.

If Automatic Sniff requesting is being deselected, a previous sniff requester will be cancelled.

If Automatic Sniff requesting is selected when it is already active, no change will be performed and KErrInUse will
be returned

@param aAutoSniffMode ETrue if Automatic Sniff/Unsniff requests are required.
@param aIdleSecondsBeforeSniffRequest A delay after the socket becomes idle before it will request sniff mode.
@return KErrNone if the automatic sniff mode has been successfully changed.
		KErrInUse if automatic sniff mode is requested, when it is already active.
		KErrArgument if an invalid idle timer value for the sniff request is provided.
		A Symbian wide error code for another other error that might have occurred.
*/
	{
	TInt err(KErrNone);
	TBool usingAutoSniffMode = AutomaticSniffMode();
	
	if(aAutoSniffMode && usingAutoSniffMode)
		{
		// if we request auto sniff requesting when it is already active we error.
		err = KErrInUse;
		}
	else if(aAutoSniffMode)
		{
		if(aIdleSecondsBeforeSniffRequest > 0)
			{
			// if here we know iAutoSniffDelayTimer is NULL, so there is no need to delete
			__ASSERT_DEBUG(!iAutoSniffDelayTimer, Panic(EUnfinishedBusiness));
			TRAP(err, iAutoSniffDelayTimer = CAutoSniffDelayTimer::NewL(*this, aIdleSecondsBeforeSniffRequest));
			if(err == KErrNone)
				{
				err = iBTBaseband.CancelLowPowerModeRequester();
				}
			}
		else
			{
			err = KErrArgument;
			}
		}
	else if(usingAutoSniffMode)
		{
		delete iAutoSniffDelayTimer;
        iAutoSniffDelayTimer = NULL;
		}

	return err;
	}


EXPORT_C TBool CBluetoothSocket::AutomaticSniffMode() const
/** Reports the automatic sniff/unsniff requesting status of the socket.

@return ETrue if automatic sniff/unsniff has been selected for this socket.
*/
	{
	return iAutoSniffDelayTimer ? ETrue : EFalse;
	}

EXPORT_C void CBluetoothSocket::AsyncDelete()
/** Asynchronous deletion of the CBluetoothSocket object.

Function will cause asynchronous deletion of CBluetoothSocket object. 
It should be called if associated MBluetoothSocketNotifier needs to delete 
CBluetoothSocket from any of its callback functions.
*/
	{
	iAsyncDestroyer->CallBack();
	}


MBluetoothSocketNotifier& CBluetoothSocket::Notifier()
	{
	// iNotifier should never be null, so a deref. is always valid.
	return static_cast<MBluetoothSocketNotifier&> (*iNotifier);
	}

void CBluetoothSocket::ConfigureSocket()
	{
	// This is done as a best effort in order to make more efficient usage
	// of the baseband packets being created. As a result we don't bother 
	// checking the results as if they fail it won't cause the socket to 
	// behave incorrectly	
	TPckgBuf<TUint> optBuf(65535); //If all else fails we should get a max 64K buffer
	TProtocolDesc info;
	
	TInt err = iSocket.Info(info);
	
	if (err == KErrNone && info.iProtocol == KRFCOMM)
		{
		// For RFCOMM we set the buffers based on the optimal MTU for RFCOMM	
		iSocket.GetOpt(KRFCOMMMaximumMTU, KSolBtRFCOMM, optBuf);
		iSocket.SetOpt(KSOSendBuf, KSOLSocket, optBuf);
		iSocket.SetOpt(KSORecvBuf, KSOLSocket, optBuf);		
		}
	else
		{
		// For L2CAP, or others, we just ensure that we can accomodate the negotiated MTU
		iSocket.GetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, optBuf);
		iSocket.SetOpt(KSOSendBuf, KSOLSocket, optBuf);
			
		iSocket.GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, optBuf);
		iSocket.SetOpt(KSORecvBuf, KSOLSocket, optBuf);
		}
	}

void CBluetoothSocket::HandleConnectCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleConnectCompleteL(aErr %d)"), aErr));
	delete iBTConnecter;
	iBTConnecter = NULL;

	if (aErr == KErrNone)
		{
		ConfigureSocket();
		}
	
	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
#ifdef __FLOGGING__
	TRAPD(err, Notifier().HandleConnectCompleteL(aErr));
	FTRACE(FPrint(_L("Connect upcall to link owner returned %d"), err));
	User::LeaveIfError(err);
#else
	Notifier().HandleConnectCompleteL(aErr);
#endif
		}
		
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleConnectCompleteL(aErr);
	}

void CBluetoothSocket::HandleAcceptCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleAcceptCompleteL(aErr %d)"), aErr));
	delete iBTAccepter;
	iBTAccepter = NULL;

	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
#ifdef __FLOGGING__
	TRAPD(err, Notifier().HandleAcceptCompleteL(aErr));
	FTRACE(FPrint(_L("Accept upcall to link owner returned %d"), err));
	User::LeaveIfError(err);
#else
	Notifier().HandleAcceptCompleteL(aErr);
#endif
		}
		
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleAcceptCompleteL(aErr);	
	}

void CBluetoothSocket::HandleShutdownCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleShutdownCompleteL(aErr %d)"), aErr));

	CancelAll();
	BTBasebandManager().HandleConnectionDown();

	delete iBTShutdowner;
	iBTShutdowner = NULL;

	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
#ifdef __FLOGGING__
	TRAPD(err, Notifier().HandleShutdownCompleteL(aErr));
	FTRACE(FPrint(_L("Shutdown upcall to link owner returned %d"), err));
	User::LeaveIfError(err);
#else
	Notifier().HandleShutdownCompleteL(aErr);
#endif
		}
		
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleShutdownCompleteL(aErr);	
	}

void CBluetoothSocket::HandleSendCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleSendCompleteL(aErr %d)"), aErr));
	iSending = EFalse;

	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
		Notifier().HandleSendCompleteL(aErr);
		}
		
	FLOG(_L("Send complete upcall complete"));
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleSendCompleteL(aErr);	
	}

void CBluetoothSocket::HandleReceiveCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleReceiveCompleteL(aErr %d)"), aErr));
	iReceiving = EFalse;
	if(iAutoSniffDelayTimer)
		{
		iAutoSniffDelayTimer->Start();
		}

	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
		Notifier().HandleReceiveCompleteL(aErr);
		}
	
	FLOG(_L("Receive complete upcall complete"));
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleReceiveCompleteL(aErr);	
	}

void CBluetoothSocket::HandleIoctlCompleteL(TInt aErr)
	{
	FTRACE(FPrint(_L("CBluetoothSocket::HandleIoctlCompleteL(aErr %d)"), aErr));
	delete iBTIoctler;
	iBTIoctler = NULL;

	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{	
#ifdef __FLOGGING__
		TRAPD(err, Notifier().HandleIoctlCompleteL(aErr));
		FTRACE(FPrint(_L("Ioctl upcall to link owner returned %d"), err));
		User::LeaveIfError(err);
#else
		Notifier().HandleIoctlCompleteL(aErr);
#endif
		}
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleIoctlCompleteL(aErr);	
	}

void CBluetoothSocket::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification aEventNotification)
	{
	iBTBasebandChangeEventNotifier->SetNotify(iNotifierEventMask);
	
	// If AsyncDestroyer was called, client could delete associated MBluetoothSocketNotifier
	// therefore no further calls to iNotifier should be made.
	if (!iAsyncDestroyer->IsActive())
		{
		Notifier().HandleActivateBasebandEventNotifierCompleteL(aErr, aEventNotification);	
		}
		
	// Third parties intend to delete CBluetoothSocket object from upcall to
	// associated MBluetoothSocketNotifier, therefore we should make sure that 
	// there is no code to execute after upcall Notifier().HandleActivateBasebandEventNotifierCompleteL(aErr, aEventNotification);	
	}

//
//Getters
//

RSocket& CBluetoothSocket::Socket()
	{
	return iSocket;
	}

RSocketServ& CBluetoothSocket::SocketServer() 
	{
	return iSockServer;
	}

RBTBaseband& CBluetoothSocket::BTBaseband() 
	{
	return iBTBaseband;
	}

CBTBasebandManager& CBluetoothSocket::BTBasebandManager()
	{
	return *iBTBasebandManager;
	}




//
//Private Functions
//

CBluetoothSocket::CBluetoothSocket(MBluetoothSocketNotifier& aNotifier, RSocketServ& aServer)
: iSockServer(aServer), iNotifier(&aNotifier), iNotifierEventMask(0), iSending(EFalse), iReceiving(EFalse)
//
//Constructor
//
	{
	}

//RSocket 'Open' functions now called in NewL overloads
void CBluetoothSocket::ConstructL(TUint sockType, TUint protocol)
	{
	User::LeaveIfError(Socket().Open(iSockServer, KBTAddrFamily, sockType, protocol));
	InitialiseL();
	}

void CBluetoothSocket::ConstructL(TUint sockType,TUint protocol, RConnection& aConnection)
	{
	User::LeaveIfError(Socket().Open(iSockServer, KBTAddrFamily, sockType, protocol, aConnection));
	InitialiseL();
	}

void CBluetoothSocket::ConstructL(const TDesC& aName)
	{
	User::LeaveIfError(Socket().Open(iSockServer, aName));
	InitialiseL();
	}

void CBluetoothSocket::ConstructL()
	{
	User::LeaveIfError(Socket().Open(iSockServer));
	InitialiseL();
	}

void CBluetoothSocket::ConstructL(RSocket& aSocket)
	{
	// We create the Sender and Receiver first, so that if either
	// leave during construction the original RSocket handle will
	// still be valid (as it has not yet been transferred.)
	InitialiseL();
	
	// If we leave from now on, the original RSocket should still
	// be valid (as it won't have successfully transferred.)	
	TName name;
	aSocket.Name(name);
	User::LeaveIfError(Socket().Transfer(SocketServer(), name));
	
	TProtocolDesc protocolDesc;
	// We expect a KErrBadHandle from the original socket.
	if (KErrBadHandle != aSocket.Info(protocolDesc))
		{
		// Otherwise we leave with the status of the internal RSocket.
		User::Leave(Socket().Info(protocolDesc));
		}
	}

void CBluetoothSocket::InitialiseL()
	{
	iBTSender = CBTSender::NewL(*this);
	iBTReceiver = CBTReceiver::NewL(*this);
	iBTBasebandManager = CBTBasebandManager::NewL();
	iBTBasebandChangeEventDelegate = CBTBasebandChangeEventDelegate::NewL(*this);
	User::LeaveIfError(BTBasebandManager().RegisterBTBaseband(*iBTBasebandChangeEventDelegate));
	
	TCallBack asyncDelete(AsyncDeleteCallBack, this);
	iAsyncDestroyer = new (ELeave) CAsyncCallBack(asyncDelete, CActive::EPriorityStandard);
	}
	
TInt CBluetoothSocket::AsyncDeleteCallBack(TAny *aThisPtr)
	{
	__ASSERT_DEBUG(aThisPtr, Panic(ENullPointerInAsyncDelete));
	CBluetoothSocket* self = static_cast<CBluetoothSocket*>(aThisPtr);
	delete self;
	return KErrNone;
	}
	

//==========================================================
//Active Object Helpers
//

//
//for CBluetoothSocket
//

//--
CBTConnecter* CBTConnecter::NewL(CBluetoothSocket& aParent)
	{
	CBTConnecter* self = new (ELeave) CBTConnecter(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}

CBTConnecter::CBTConnecter(CBluetoothSocket& aParent)
: CActive(CActive::EPriorityStandard),iParent(aParent)
//
//Constructor - the CBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	}

void CBTConnecter::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTConnecter::~CBTConnecter()
//
//Destructor
//
	{
	Cancel();
	iActiveModeRequester.Close();
	}

void CBTConnecter::Connect(TBTSockAddr& aSockAddr)
//
//Attempts to connect to the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	iParent.Socket().Connect(aSockAddr, iStatus);
	
	
	if (iParent.AutomaticSniffMode())
		{
		TBTDevAddr devAddr = aSockAddr.BTAddr();
		TInt err = iActiveModeRequester.Open(iParent.SocketServer(), devAddr);
		if(err == KErrNone)
			{
			// If there is an error, there is nothing we can do about it. This is safe to
			// do as the only effect is that we will fail to ensure connection is done in active
			// mode.
			(void)iActiveModeRequester.RequestExplicitActiveMode(ETrue);
			}
		}
	
	SetActive();
	}

void CBTConnecter::Connect(TBTSockAddr& aSockAddr, const TDesC8& aConnectDataOut, TDes8& aConnectDataIn)
//
//Attempts to connect to the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	iParent.Socket().Connect(aSockAddr, aConnectDataOut, aConnectDataIn, iStatus);
	
	if (iParent.AutomaticSniffMode())
		{
		TBTDevAddr devAddr = aSockAddr.BTAddr();
		TInt err = iActiveModeRequester.Open(iParent.SocketServer(), devAddr);
		if(err == KErrNone)
			{
			// If there is an error, there is nothing we can do about it. This is safe to
			// do as the only effect is that we will fail to ensure connection is done in active
			// mode.
			(void)iActiveModeRequester.RequestExplicitActiveMode(ETrue);
			}
		}
		
	SetActive();
	}

void CBTConnecter::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	TInt err = iStatus.Int();
	if(iStatus == KErrNone)
		{
		err = BasebandAccessor();
		}

	// We have now finished with this temporary SAP.
	iActiveModeRequester.Close();

	//must come LAST .. deletes this active object
	iParent.HandleConnectCompleteL(err); 
	}

TInt CBTConnecter::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleConnectCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTConnecter::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	iParent.Socket().CancelConnect();
	iActiveModeRequester.Close();
	}
	
//Call HandleConnectionUp() to open registered basebands
TInt CBTConnecter::BasebandAccessor()
	{
	TInt err = iParent.BTBasebandManager().HandleConnectionUp(iParent.SocketServer(), iParent.Socket());
	return err;
	}







//--
CBTAccepter* CBTAccepter::NewL(CBluetoothSocket& aParent, CBluetoothSocket& aBlankSocket)
	{
	CBTAccepter* self = new (ELeave) CBTAccepter(aParent, aBlankSocket);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}

CBTAccepter::CBTAccepter(CBluetoothSocket& aParent, CBluetoothSocket& aBlankSocket)
: CActive(CActive::EPriorityStandard),iParent(aParent),iBlankSocket(aBlankSocket)
//
//Constructorthe CBluetoothSocket parent is provided.
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	}

void CBTAccepter::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTAccepter::~CBTAccepter()
//
//Destructor
//
	{
	Cancel();
	}

void CBTAccepter::Accept()
//
//Attempts to accept from the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	iParent.Socket().Accept(iBlankSocket.Socket(), iStatus);
	SetActive();
	}

void CBTAccepter::Accept(TDes8& aConnectData)
//
//Attempts to accept from the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	iParent.Socket().Accept(iBlankSocket.Socket(), aConnectData, iStatus);
	SetActive();
	}

void CBTAccepter::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	if(iStatus == KErrNone)
		{
		BasebandAccessor();
		iBlankSocket.ConfigureSocket();
		}

	//must come LAST .. deletes this active object
	iParent.HandleAcceptCompleteL(iStatus.Int()); 
	}

TInt CBTAccepter::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleAcceptCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTAccepter::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	iParent.Socket().CancelAccept();
	}

void CBTAccepter::BasebandAccessor()
	{
	// iBlankSocket
	iBlankSocket.BTBasebandManager().HandleConnectionUp(iBlankSocket.SocketServer(), iBlankSocket.Socket());
	}







//--
CBTShutdowner* CBTShutdowner::NewL(CBluetoothSocket& aParent)
//
//NewL: the CBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	CBTShutdowner* self = new (ELeave) CBTShutdowner(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}


CBTShutdowner::CBTShutdowner(CBluetoothSocket& aParent)
: CActive(CActive::EPriorityStandard),
 iParent(aParent)
//
//Constructor
//
	{
	}

void CBTShutdowner::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTShutdowner::~CBTShutdowner()
//
//Destructor
//
	{
	Cancel();
	}

void CBTShutdowner::Shutdown(RSocket::TShutdown aHow)
//
//Attempts to connect to the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	iParent.Socket().Shutdown(aHow, iStatus);
	SetActive();
	}

void CBTShutdowner::Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	iParent.BTBasebandManager().HandleConnectionDown();
	iParent.Socket().Shutdown(aHow, aDisconnectDataOut, aDisconnectDataIn, iStatus);

	SetActive();
	}

void CBTShutdowner::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	iParent.HandleShutdownCompleteL(iStatus.Int());
	}

TInt CBTShutdowner::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleShutdownCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTShutdowner::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	//iParent.Socket().CancelAll(); 
	//what do we do here if anything: no CancelShutdown exists in RSocket?
	}







//--
CBTSender* CBTSender::NewL(CBluetoothSocket& aParent)
//
//NewL: the CBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	CBTSender* self = new (ELeave) CBTSender(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}


CBTSender::CBTSender(CBluetoothSocket& aParent)
: CActive(CActive::EPriorityStandard),
 iParent(aParent)
//
//Constructor
//
	{
	}

void CBTSender::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTSender::~CBTSender()
//
//Destructor
//
	{
	Cancel();
	}

void CBTSender::Write(const TDesC8& aDesc)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Write(aDesc, iStatus);
	iCurrentRequest = EWrite;
	SetActive();
	}

void CBTSender::Send(const TDesC8& aDesc, TUint someFlags)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Send(aDesc, someFlags, iStatus);
	iCurrentRequest = ESend;
	SetActive();
	}

void CBTSender::Send(const TDesC8& aDesc, TUint someFlags, TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Send(aDesc, someFlags, iStatus, aLen);
	iCurrentRequest = ESend;
	SetActive();
	}

void CBTSender::SendTo(const TDesC8& aDesc, TSockAddr& aAddr, TUint flags)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().SendTo(aDesc, aAddr, flags, iStatus);
	iCurrentRequest = ESend;
	SetActive();
	}

void CBTSender::SendTo(const TDesC8& aDesc, TSockAddr& aAddr, TUint flags, TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().SendTo(aDesc, aAddr, flags, iStatus, aLen);
	iCurrentRequest = ESend;
	SetActive();
	}

void CBTSender::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	iParent.HandleSendCompleteL(iStatus.Int());
	}

TInt CBTSender::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleSendCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTSender::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	switch (iCurrentRequest)
		{
		//zam todo do these cover all Write/Send functions?
		case EWrite:
			iCurrentRequest = ENone;
			iParent.Socket().CancelWrite(); 
			break;
		case ESend:
			iCurrentRequest = ENone;
			iParent.Socket().CancelSend(); 
			break;
		default:
			iCurrentRequest = ENone;
			break;
		};
	}







//--
CBTReceiver* CBTReceiver::NewL(CBluetoothSocket& aParent)
//
//NewL: the CBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	CBTReceiver* self = new (ELeave) CBTReceiver(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}


CBTReceiver::CBTReceiver(CBluetoothSocket& aParent)
: CActive(CActive::EPriorityStandard),
 iParent(aParent)
//
//Constructor
//
	{
	}

void CBTReceiver::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTReceiver::~CBTReceiver()
//
//Destructor
//
	{
	Cancel();
	}

void CBTReceiver::Read(TDes8& aDesc)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Read(aDesc, iStatus);
	iCurrentRequest = ERead;
	SetActive();
	}

void CBTReceiver::Recv(TDes8& aDesc,TUint flags)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Recv(aDesc, flags, iStatus);
	iCurrentRequest = ERecv;
	SetActive();
	}

void CBTReceiver::Recv(TDes8& aDesc,TUint flags,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Recv(aDesc, flags, iStatus, aLen);
	iCurrentRequest = ERecv;
	SetActive();
	}

void CBTReceiver::RecvOneOrMore(TDes8& aDesc,TUint flags,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().RecvOneOrMore(aDesc,flags, iStatus, aLen);
	iCurrentRequest = ERecv;
	SetActive();
	}

void CBTReceiver::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().RecvFrom(aDesc, aAddr, flags, iStatus);
	iCurrentRequest = ERecv;
	SetActive();
	}

void CBTReceiver::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().RecvFrom(aDesc, aAddr, flags, iStatus, aLen);
	iCurrentRequest = ERecv;
	SetActive();
	}



void CBTReceiver::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	iParent.HandleReceiveCompleteL(iStatus.Int());
	}

TInt CBTReceiver::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleReceiveCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTReceiver::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	switch (iCurrentRequest)
		{
		//zam todo do these cover all Read/Recv functions?
		case ERead:
			iCurrentRequest = ENone;
			iParent.Socket().CancelRead(); 
			break;
		case ERecv:
			iCurrentRequest = ENone;
			iParent.Socket().CancelRecv(); 
			break;
		default:
			iCurrentRequest = ENone;
			break;
		};

	iParent.Socket().CancelRead(); 
	}







//--
CBTIoctler* CBTIoctler::NewL(CBluetoothSocket& aParent)
//
//NewL: the CBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	CBTIoctler* self = new (ELeave) CBTIoctler(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}


CBTIoctler::CBTIoctler(CBluetoothSocket& aParent)
: CActive(CActive::EPriorityStandard),
 iParent(aParent)
//
//Constructor
//
	{
	}

void CBTIoctler::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTIoctler::~CBTIoctler()
//
//Destructor
//
	{
	Cancel();
	}

void CBTIoctler::Ioctl(TUint aCommand, TDes8* aDesc, TUint aLevel)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.Socket().Ioctl(aCommand, iStatus, aDesc, aLevel);
	SetActive();
	}

void CBTIoctler::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	iParent.HandleIoctlCompleteL(iStatus.Int());
	}


TInt CBTIoctler::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleIoctlCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTIoctler::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	iParent.Socket().CancelIoctl();
	}

//--
CBTBasebandChangeEventNotifier* CBTBasebandChangeEventNotifier::NewL(MBTBasebandHandler& aUser)
//
//NewL: the RBluetoothPhysicalLinksManager parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	CBTBasebandChangeEventNotifier* self = new (ELeave) CBTBasebandChangeEventNotifier(aUser);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}


CBTBasebandChangeEventNotifier::CBTBasebandChangeEventNotifier(MBTBasebandHandler& aUser)
: CActive(CActive::EPriorityStandard),
 iUser(aUser)
//
//Constructor
//
	{
	}

void CBTBasebandChangeEventNotifier::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTBasebandChangeEventNotifier::~CBTBasebandChangeEventNotifier()
//
//Destructor
//
	{
	Cancel();
	}

void CBTBasebandChangeEventNotifier::SetNotifyNextModeChange()
	{
	iCurrentRequest = ENotifyAnyPowerMode;
	SetNotify(iCurrentRequest);
	}

void CBTBasebandChangeEventNotifier::SetNotify(TUint32 aNotifications)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	iEvent = TBTBasebandEventNotification();
	iUser.MbbhBTBaseband().ActivateNotifierForRecall(iEvent, iStatus, aNotifications);
	SetActive();
	}

void CBTBasebandChangeEventNotifier::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	iCurrentRequest = 0;
	TBTBasebandEventNotification notification = iEvent();
	iUser.MbbhHandleActivateBasebandEventNotifierCompleteL(iStatus.Int(), notification);
	}

TInt CBTBasebandChangeEventNotifier::RunError(TInt aError)
	{
	#ifdef __FLOGGING__
	FTRACE(FPrint(_L("CBluetoothSocket::HandleActivateBasebandEventNotifierCompleteL left with error %d"), aError));
	#else
	(void) aError;
	#endif
	return KErrNone;
	}

void CBTBasebandChangeEventNotifier::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	iUser.MbbhBTBaseband().CancelNextBasebandChangeEventNotifier();
	iCurrentRequest = 0;
	}


// CAutoSniffDelayTimer

CAutoSniffDelayTimer::CAutoSniffDelayTimer(CBluetoothSocket& aParent, TInt aIdleSecondsBeforeSniffRequest)
	:CTimer(CActive::EPriorityStandard),
	iParent(aParent),
	iSniffModeRequested(EFalse)
	{
	const TInt KMicroSecondsInASecond = 1000000;
	iAutoSniffDelay = aIdleSecondsBeforeSniffRequest * KMicroSecondsInASecond;
	}

CAutoSniffDelayTimer::~CAutoSniffDelayTimer()
	{
	Cancel();
	delete iBTAutoSniffBasebandChangeEventNotifier;
	// Disable explicit active mode and cancel any low power request if we
	// have done an auto sniff, there is nothing we do if we error in this process.
	(void)MbbhBTBaseband().RequestExplicitActiveMode(EFalse);
	if(iSniffModeRequested)
		{
		(void)MbbhBTBaseband().CancelLowPowerModeRequester();
		}
	(void)iParent.BTBasebandManager().UnRegisterBTBaseband(*this);
	}

void CAutoSniffDelayTimer::ConstructL()
	{
	CTimer::ConstructL();
	User::LeaveIfError(iParent.BTBasebandManager().RegisterBTBaseband(*this));
	iBTAutoSniffBasebandChangeEventNotifier = CBTBasebandChangeEventNotifier::NewL(*this);
	// Issue a sniff request as soon as we enable auto sniffing.
	User::LeaveIfError(MakeSniffRequest());
	iBTAutoSniffBasebandChangeEventNotifier->SetNotify(ENotifyActiveMode);
	CActiveScheduler::Add(this);
	}
	
CAutoSniffDelayTimer* CAutoSniffDelayTimer::NewL(CBluetoothSocket& aParent, TInt aIdleSecondsBeforeSniffRequest)
	{
	CAutoSniffDelayTimer* self = new (ELeave) CAutoSniffDelayTimer(aParent, aIdleSecondsBeforeSniffRequest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CAutoSniffDelayTimer::Start()
	{
	Cancel();
	if(iSniffModeRequested)
		{
		// Try to cancel the low power modes, if we fail to do that
		// then we just continue anyway as there isn't much else
		// we can do.
		(void)MbbhBTBaseband().CancelLowPowerModeRequester();
		}

	iSniffModeRequested = EFalse;
	After(iAutoSniffDelay);
	}

//Start a 5-minute window for active mode
void CAutoSniffDelayTimer::StartActive()
	{
	Start();
	// When enabling auto sniffing, we make use of Explicit active mode.
	(void)MbbhBTBaseband().RequestExplicitActiveMode(ETrue);
	}

RBTBaseband& CAutoSniffDelayTimer::MbbhBTBaseband() 
	{
	return iBTAutoSniffBaseband;
	}

void CAutoSniffDelayTimer::MbbhHandleActivateBasebandEventNotifierCompleteL(TInt /*aErr*/, TBTBasebandEventNotification& aEventNotification)
	{
	if(aEventNotification.EventType() == ENotifyActiveMode)
		{
		// Link has gone into active mode - if we are requesting
		// sniff mode, then it is up to us to cancel the sniff
		// requester to prevent the ping-pong effect.
		if(iSniffModeRequested)
			{
			MbbhBTBaseband().CancelLowPowerModeRequester();
			iSniffModeRequested = EFalse;
			}
		}
	__ASSERT_ALWAYS(iBTAutoSniffBasebandChangeEventNotifier != NULL, Panic(EBBInvalidAddress));
	iBTAutoSniffBasebandChangeEventNotifier->SetNotify(ENotifyActiveMode);
	}
	
TInt CAutoSniffDelayTimer::MbbhOpen(RSocketServ& aSocketServ, RSocket& aSocket)
	{
	TInt err;
	err = iBTAutoSniffBaseband.Open(aSocketServ, aSocket);
	if(err == KErrNone)
		{
		//On sucessful opening, preventing Park Mode
		err = iParent.PreventLowPowerModes(EParkMode);
		}
	return err;
	}
	
void CAutoSniffDelayTimer::MbbhClose()
	{
	iParent.BTBaseband().AllowLowPowerModes(EParkMode);
	iBTAutoSniffBaseband.Close();
	}

void CAutoSniffDelayTimer::RunL()
/**
Request Sniff Mode when the timer expires
**/
	{
	(void)MbbhBTBaseband().RequestExplicitActiveMode(EFalse);
	(void)MakeSniffRequest();
	}

TInt CAutoSniffDelayTimer::MakeSniffRequest()
	{
	TInt err = MbbhBTBaseband().ActivateSniffRequester();
	if(err == KErrNone)
		{
		iSniffModeRequested = ETrue;
		}
	return err;
	}




// Class CBTBasebandManager 

CBTBasebandManager* CBTBasebandManager::NewL()
	{
	CBTBasebandManager* self = new (ELeave) CBTBasebandManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CBTBasebandManager::~CBTBasebandManager()
	{
	CloseAllBasebands();
	iBasebandHandlers.Close();
	}
	
CBTBasebandManager::CBTBasebandManager()
//	: iConnected(EFalse) // By default we are not connected.
	{
	}
	
void CBTBasebandManager::ConstructL()
	{
	}

//Register a RBTBaseband as specified by aBasebandRegisterMask.
//After registering, Open the RBTBaseband if the connection is up.
TInt CBTBasebandManager::RegisterBTBaseband(MBTBasebandHandler& aBasebandHandler)
	{
	TInt err = KErrNone;
	//Trying to register too many basebands
	if(KErrNotFound != iBasebandHandlers.Find(&aBasebandHandler))
		{
		// Trying to register an existing handler again
		err = KErrArgument;
		}
	else
		{
		// We know we have a baseband "slot" available from the check above
		// so just find somewhere to put it.
		err = iBasebandHandlers.Append(&aBasebandHandler);
		 
		if(err != KErrNone)
			{
			//Most likely KErrNoMemory
			return err;
			}

		// Might already be open, so to be sure we close it now.
	 	aBasebandHandler.MbbhClose();
		// Now bring the baseband up to the current state
		if(iConnected)
			{
		 	err = aBasebandHandler.MbbhOpen(*iSocketServ, *iSocket);
			}
		if(err != KErrNone)
			{
			// On error - roll back the managers state
			TInt handlerIndex = iBasebandHandlers.Find(&aBasebandHandler);
			__ASSERT_DEBUG((handlerIndex != KErrNotFound),Panic(EBadArgument));
			iBasebandHandlers.Remove(handlerIndex);
			}
		}
	return err;
	}
	
//Un-register a RBTBaseband
TInt CBTBasebandManager::UnRegisterBTBaseband(MBTBasebandHandler& aBasebandHandler)
	{
	// Check that the baseband is actually registered in the manager
	// We check to see if it is the same instance of MBTBasebandHandler through
	// their pointer addresses.
	TInt handlerIndex = iBasebandHandlers.Find(&aBasebandHandler);
	if (handlerIndex == KErrNotFound)
		{
		return KErrNotFound;		// send error condition back to API
		}
	iBasebandHandlers.Remove(handlerIndex);
		
	// Close the baseband if we successfully unregistered it.
	aBasebandHandler.MbbhClose();

	return KErrNone;
	}
	
//Open all registered basebands when connection is up
TInt CBTBasebandManager::HandleConnectionUp(RSocketServ& aSocketServ, RSocket& aSocket)
	{
	TInt err = KErrNone;
	TInt handlerCount = iBasebandHandlers.Count();
	
	if(iConnected)
		{
		err = KErrInUse;
		}
	else
		{
		// Cache the information for basebands that register after the connection has come up
		iSocketServ = &aSocketServ;
		iSocket = &aSocket;
		
		for(TInt i = 0; i < handlerCount; i++)
			{
			err = iBasebandHandlers[i]->MbbhOpen(aSocketServ, aSocket);
			if(err != KErrNone)
				{
				// There was an error opening one of the sockets, we aren't connected
				// so make sure we roll all RBTBasebands back to being closed.
				CloseAllBasebands();
				break;			
				}
			}
			
		if(err == KErrNone)
			{
			iConnected = ETrue;
			}
		}

	return err;
	}
	
//Close all opened basebands when connection is down
void CBTBasebandManager::HandleConnectionDown()
	{
	CloseAllBasebands();
	iConnected = EFalse;
	}

TBool CBTBasebandManager::Connected() const
	{
	return iConnected;
	}

void CBTBasebandManager::CloseAllBasebands()
	{
	TInt handlerCount = iBasebandHandlers.Count();
	for(TInt i = 0; i < handlerCount; i++)
		{
		iBasebandHandlers[i]->MbbhClose();
		}
	}


//class CBTBasebandChangeEventDelegate
CBTBasebandChangeEventDelegate* CBTBasebandChangeEventDelegate::NewL(CBluetoothSocket& aParent)
	{
	CBTBasebandChangeEventDelegate* self = new (ELeave) CBTBasebandChangeEventDelegate(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CBTBasebandChangeEventDelegate::CBTBasebandChangeEventDelegate(CBluetoothSocket& aParent)
:iParent(aParent)
	{	
	}

void CBTBasebandChangeEventDelegate::ConstructL()
	{
	}
	
RBTBaseband& CBTBasebandChangeEventDelegate::MbbhBTBaseband()
	{
	return iParent.BTBaseband();
	}
	
void CBTBasebandChangeEventDelegate::MbbhHandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification)
	{
	iParent.HandleActivateBasebandEventNotifierCompleteL(aErr, aEventNotification);
	}

TInt CBTBasebandChangeEventDelegate::MbbhOpen(RSocketServ& aSocketServ, RSocket& aSocket)
	{
	TInt err = iParent.BTBaseband().Open(aSocketServ, aSocket);
	return err;
	}
	
void CBTBasebandChangeEventDelegate::MbbhClose()
	{
	iParent.BTBaseband().Close();
	}
