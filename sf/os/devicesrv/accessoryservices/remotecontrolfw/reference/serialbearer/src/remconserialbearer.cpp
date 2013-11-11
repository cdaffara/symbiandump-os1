// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <remcon/remconbearerobserver.h>
#include <remcon/remconconverterplugin.h>
#include <remconaddress.h>
#include "remconserialbearer.h"
#include "sender.h"
#include "receiver.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_REF_SER_BEARER);
#endif

PANICCATEGORY("SerialBear");

/**
Factory function.
@return Ownership of a new CRemConSerialBearer.
*/
CRemConSerialBearer* CRemConSerialBearer::NewL(TBearerParams& aParams)
	{
	LOG_STATIC_FUNC
	CRemConSerialBearer* self = new(ELeave) CRemConSerialBearer(aParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	return self;
	}

/**
Destructor.
*/
CRemConSerialBearer::~CRemConSerialBearer()
	{
	LOG_LINE
	LOG_FUNC

	delete iSender;
	delete iReceiver;

	iComm.Close();
	iCommServ.Close();

/*#ifndef __WINS__
	if ( iUsb.Handle() )
		{
		// Can't do anything with error here.
		(void)iUsb.Stop();
		}
	iUsb.Close();
#endif
*/
	}

/**
Constructor.
*/
CRemConSerialBearer::CRemConSerialBearer(TBearerParams& aParams)
:	CRemConBearerPlugin(aParams)
	{
	LOG_FUNC
	}

/**
2nd-phase construction.
*/
void CRemConSerialBearer::ConstructL()
	{
	// NB The sender and receiver have references to our RComm. We protect our 
	// down-calls in case the port subsession is not open.
	iSender = CSender::NewL(iComm, *this);
	iReceiver = CReceiver::NewL(iComm, *this);

/*#ifndef __WINS__
	// Connect and Start USB to begin with. 
	LEAVEIFERRORL(iUsb.Connect());
	TRequestStatus stat;
	iUsb.Start(stat);
	User::WaitForRequest(stat);
	LEAVEIFERRORL(stat.Int());
#endif
*/
	// We trap and discard errors from this 'connection' attempt. We don't 
	// want to fail RemCon server initialisation because the port is being 
	// used by someone else. When RemCon wants to send a command, it will 
	// expect the bearer to reconnect.
	// In any case a controller client is allowed to request a 
	// connection-oriented channel over this bearer- this will result in 
	// another 'connection' attempt.
	TRAPD(err, ConnectL());
	// If we 'connected' OK, throw an indication up to RemCon.
	if ( err == KErrNone )
		{
		TRemConAddress addr;
		addr.BearerUid() = Uid();
		err = Observer().ConnectIndicate(addr);
		if ( err != KErrNone )
			{
			// We couldn't successfully tell RemCon about our connection, so 
			// tear it down.
			ClosePort();
			}
		}
		
	if ( iComm.SubSessionHandle() )
		{
		Receive();
		}

//	TRemConAddress addr;
//	addr.BearerUid() = Uid();
//	addr.Addr() = _L8("different");
//	Observer().ConnectIndicate(addr);

	LOG(_L("<<CRemConSerialBearer::ConstructL"));
	}

TAny* CRemConSerialBearer::GetInterface(TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\taUid = 0x%08x"), aUid);

	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConBearerInterface1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConBearerInterface*>(this)
			);
		}

	LOG1(_L("\tret = [0x%08x]"), ret);
	return ret;
	}

void CRemConSerialBearer::ConnectRequest(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	// This bearer does not use client-specified connection data. The 
	// connection data gives the *remote* address to connect to. This is not a 
	// valid concept in the wired case. 

	// This ASSERTS if we already have the port open!!!!
	TRAPD(connectErr, ConnectL());
	
	TInt err = Observer().ConnectConfirm(aAddr, connectErr);
	// If there was an error in RemCon we should drop the connection otherwise 
	// RemCon might get surprised by later incoming messages on a connection 
	// it doesn't know exists. Otherwise, start listening for incoming data.
	// Of course, don't start listening unless the port opened OK 
	// (connectErr).
	if ( err != KErrNone || connectErr != KErrNone )
		{
		ClosePort();
		}
	else
		{
		Receive();
		}
	}

void CRemConSerialBearer::DisconnectRequest(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8);
	LOG_FUNC;

	// make sure we are connected before disconnect
	ASSERT_ALWAYS(iCommServ.Handle());
	
	// Undo everything done in Connect (without assuming that Connect 
	// completed successfully). Must cancel any outstanding requests for this 
	// to be safe.
	ClosePort();
	Observer().DisconnectConfirm(aAddr, KErrNone);
	}

void CRemConSerialBearer::ConnectL()
	{
	LOG_FUNC;

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

	// make sure we aren't connected
	ASSERT_ALWAYS(!iCommServ.Handle());
	
	TInt r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		LEAVEIFERRORL(r);
		}
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		LEAVEIFERRORL(r);
		}

	_LIT(KModule, "ECUART");
#if defined (__WINS__)
	_LIT(KPort, "COMM::1");
#else
//	_LIT(KPort, "COMM::2");  // On H2 and H4 this is the IR port
							 // use COMM::2 for Lubbock.
	_LIT(KPort, "COMM::3");  // need this for H2 and H4 board
#endif

	LEAVEIFERRORL(iCommServ.Connect());
	
 	// if appropriate close will unloadcommmodule internally
 	CleanupClosePushL(iCommServ);

	LEAVEIFERRORL(iCommServ.LoadCommModule(KModule));
	
	// as above if not connected this should be NULL
	ASSERT_ALWAYS(!iComm.SubSessionHandle());
	
	LEAVEIFERRORL(iComm.Open(iCommServ, KPort, ECommExclusive));

	// put these on clean up stack - will call close if we leave
	CleanupClosePushL(iComm);

	// Put the port's config into a known state.
	TCommConfig portSettings;
	LEAVEIFERRORL(iComm.Config(portSettings));
	portSettings().iRate = EBps115200;
	portSettings().iDataBits = EData8;
	portSettings().iStopBits = EStop1;
	LEAVEIFERRORL(iComm.SetConfig(portSettings));

	CleanupStack::Pop(2,&iCommServ);// pop iComm then iCommServ

	/* clear out the serial port so we know we start from fresh...
	 simple expedient (that hopefully will work), try and receive 20 bytes,
	 then cancel the receive. Given the current behaviour of the read cancel this
	 will return any characters that are currently in the serial port buffers 
	*/
	ASSERT_DEBUG(iReceiver);
	Receive();
	iReceiver->Cancel();
	}

void CRemConSerialBearer::ClosePort()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iSender);
	iSender->Cancel();
	ASSERT_DEBUG(iReceiver);
	iReceiver->Cancel();
	iComm.Close();
	iCommServ.Close();
	}

TInt CRemConSerialBearer::SendResponse(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint /*aTransactionId*/, // we don't care about this transaction ID
		RBuf8& aData, 
		const TRemConAddress& /*aAddr*/)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	
	TInt ret = DoSend(aInterfaceUid, aOperationId, aData, ERemConResponse);

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TInt CRemConSerialBearer::SendCommand(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId,  
		RBuf8& aData, 
		const TRemConAddress& /*aAddr*/)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	
	iTransactionId = aTransactionId;
	TInt ret = DoSend(aInterfaceUid, aOperationId, aData, ERemConCommand);

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TInt CRemConSerialBearer::DoSend(TUid aInterfaceUid, 
		TUint aOperationId, 
		RBuf8& aData, 
		TRemConMessageType aMsgType)
	{
	LOG_FUNC;
	
	TInt ret = KErrInUse;

	// make sure we are connected
	ASSERT_ALWAYS(iComm.SubSessionHandle());

	// Check we're not already busy sending.
	ASSERT_DEBUG(iSender);
	if ( iSender->IsActive() )
		{
		ret = KErrInUse;
		}
	else
		{
		ret = Observer().InterfaceToBearer(Uid(), aInterfaceUid, aOperationId, aData, aMsgType, iOutMsg);
		if ( ret == KErrNone )
			{
			LOG1(_L("\tsending text \"%S\""), &iOutMsg);
			iSender->Send(iOutMsg);
			}
		}

	// If no error, we took ownership of aData- we've finished with it now.
	if ( ret == KErrNone )
		{
		aData.Close();
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

void CRemConSerialBearer::MsoSendComplete(TInt aError)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	(void)aError;

	// We don't do anything. How does RemCon get to know of this? Does it 
	// care? No. A command which never gets a response is standard remote 
	// control behaviour. The user will retry, and doesn't need an InfoMsg to 
	// tell them anything about it.
	}

TInt CRemConSerialBearer::GetResponse(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	LOG_FUNC;

	ASSERT_DEBUG(iReceiver);
	// RemCon shouldn't call use unless we prompted them to with NewResponse.
	ASSERT_DEBUG(!iReceiver->IsActive()); 
	// The message should be a response, unless RemCon is being buggy and 
	// calling GetResponse at bad times.
	ASSERT_DEBUG(iMsgType == ERemConResponse);

	TRAPD(err, DoGetResponseL(aInterfaceUid, aOperationId, aData, aAddr));
	LOG3(_L("\taInterfaceUid = 0x%08x, aOperationId = 0x%02x, msgType = %d"), 
		aInterfaceUid, aOperationId, iMsgType);

	// The serial bearer only has one incoming message in it at once, so we 
	// don't need any queueing or anything fancy to get the transaction ID- 
	// it's just the last one RemCon told us.
	aTransactionId = iTransactionId;

	// Repost read request on port.
	Receive();

	LOG1(_L("\terr = %d"), err);
	return err;
	}

void CRemConSerialBearer::DoGetResponseL(TUid& aInterfaceUid, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	LOG_FUNC;

	aInterfaceUid = iInterfaceUid;
	aOperationId = iOperationId;
	ASSERT_DEBUG(aData.MaxLength() == 0);
	// Pass ownership of this to RemCon.
	aData.CreateL(iData);

	aAddr.BearerUid() = Uid();
	aAddr.Addr() = KNullDesC8();
	}

TInt CRemConSerialBearer::GetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	LOG_FUNC;

	ASSERT_DEBUG(iReceiver);
	// RemCon shouldn't call use unless we prompted them to with NewCommand.
	ASSERT_DEBUG(!iReceiver->IsActive()); 
	// The message should be a command, unless RemCon is being buggy and 
	// calling GetCommand at bad times.
	ASSERT_DEBUG(iMsgType == ERemConCommand);

	TRAPD(err, DoGetCommandL(aInterfaceUid, aOperationId, aData, aAddr));
	LOG3(_L("\taInterfaceUid = 0x%08x, aOperationId = 0x%02x, msgType = %d"), 
		aInterfaceUid, aOperationId, iMsgType);

	// The serial bearer only has one incoming message in it at once, so we 
	// don't need any queueing or anything fancy to get the transaction ID- 
	// it's just the last one RemCon told us.
	aTransactionId = iTransactionId;
	
	// Repost read request on port.
	Receive();

	LOG1(_L("\terr = %d"), err);
	return err;
	}

void CRemConSerialBearer::DoGetCommandL(TUid& aInterfaceUid,
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	LOG_FUNC;

	aInterfaceUid = iInterfaceUid;
	aOperationId = iOperationId;
	// Pass ownership of this to RemCon.
	aData.CreateL(iData);
	
	aAddr.BearerUid() = Uid();
	aAddr.Addr() = KNullDesC8();
	}

// Utility to repost a read on the port.
void CRemConSerialBearer::Receive()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iComm.SubSessionHandle());
	ASSERT_DEBUG(iReceiver);
	/* we should check we aren't currently trying to receive. 
	   In this design we only kick off a receive when we aren't already trying to read something already.
	   If we get this wrong the SetActive in iReceiver->Receive will blow up, so we'll assert here so we know.
	   This configuration is for test code not real life so there is no 'real' problem anyway.
	*/
	ASSERT_DEBUG(!iReceiver->IsActive());
	/* you might expect to see an iReceiver->Cancel() here BUT there are problems with doing this
	   a) The underlying RComm::Read may just decide to put the 5 bytes of message it currently has in 
	       the buffer (if we happen to call this half way through an incoming message). In this case this
	       bearer will NEVER recover as it assumes it will only receive the proper complete 30 byte message
	       and makes no attempt to get back in step (it will have 5 bytes from one message + 25 from another for ever).
	   b) We are pretty confident we don't need to (see above).
	*/
	iReceiver->Receive(iInMsg);
	}

void CRemConSerialBearer::MroReceiveComplete(TInt aError)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG2(_L("\taError = %d, iInMsg = \"%S\""), aError, &iInMsg);

	// If any error occurred either getting or decoding the message, we need 
	// to re-post now. Otherwise, we wait until after RemCon has picked up the 
	// message until we re-post a read. 
	if ( aError == KErrNone )
		{
		TRemConAddress addr;
		addr.BearerUid() = Uid();
		addr.Addr() = KNullDesC8();

		aError = Observer().BearerToInterface(Uid(), 
			iInMsg,
			iInMsg, 
			iInterfaceUid, 
			iOperationId,
			iMsgType, 
			iData);

		if ( aError == KErrNone )
			{
			if ( iMsgType == ERemConCommand )
				{
				iTransactionId = Observer().NewTransactionId();
				aError = Observer().NewCommand(addr);
				}
			else if ( iMsgType == ERemConResponse )
				{
				aError = Observer().NewResponse(addr);
				}
			else
				{
				// drop the message (malformed- not apparently a command or response)
				aError = KErrNotSupported;
				}
			}
		// else drop the message (either no converter for that interface UID or it couldn't convert the message)
		}
	// else just drop the message (error receiving it)
	
	// If we successfully told RemCon there was a new message to pick up, 
	// don't repost our port read until RemCon has come back and picked up the 
	// message. If there was any error, then RemCon isn't going to come back 
	// for this message- it's effectively dropped- and we should re-post our 
	// request now.
	if ( aError != KErrNone )
		{
		Receive();
		}
	}

TSecurityPolicy CRemConSerialBearer::SecurityPolicy() const
	{
	return TSecurityPolicy(ECapabilityLocalServices);
	}

void CRemConSerialBearer::ClientStatus(TBool aControllerPresent, TBool aTargetPresent)
	{
	LOG2(_L("CRemConSerialBearer::ClientStatus aControllerPresent = %d, aTargetPresent = %d"), 
		aControllerPresent, aTargetPresent);
	(void)aControllerPresent;
	(void)aTargetPresent;

	// I don't care.
	}
