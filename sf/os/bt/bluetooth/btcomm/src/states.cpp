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
// BTComm state machine state implementation.
// 
//

#include <bluetooth/logger.h>
#include <cs_port.h>
#include "btcomm.h"
#include "btstate.h"
#include "btcommactive.h"
#include <bt_sock.h>
#include "sdpkey.h"
#include "sdpclientcsy.h"
#include <e32test.h>
#include <btmanclient.h>
#include "btcommutil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM);
#endif

#define BAD_BTCOMM_EVENT PanicInState(EBTCommBadEventForThisState);



TBTPortState::TBTPortState(CBTPortStateFactory* aParent)
	: iFactory(aParent)
/**
	TBTPortState c'tor.
	TBTPortState is the abstract base class that all CSY state
	objects inherit from.  However, it is not a 'C' class but 
	a 'T' class which is slightly bogus but the best way to 
	implement the base state for the pattern.
**/
	{
	LOG_FUNC
	}

TBTPortState::~TBTPortState()
	{
	LOG_FUNC
	}

/**
Calls the appropriate panic function to encode the panic
code with the current state identifier.
@param aPanic The panic code that the state is panicking with.
*/
void TBTPortState::PanicInState(TBTCommPanic aPanic) const
	{
	LOG_FUNC
	BTCommUtil::Panic(aPanic, iFactory->StateIndex(this));
	}

//		***** Default state *****

TBTPortDefaultState::TBTPortDefaultState(CBTPortStateFactory* aParent) 
	: TBTPortState(aParent)
	{
	LOG_FUNC
	}

TBTPortDefaultState::~TBTPortDefaultState()
	{
	LOG_FUNC
	}

void TBTPortDefaultState::Open(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::Close(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::Read(CBTPortProxy* /*aContext*/,TAny* /*aPtr*/,TInt /*aLength*/)
/**
	TBTPortDefaultState Read.
**/
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::Write(CBTPortProxy* /*aContext*/,TAny* /*aPtr*/,TInt /*aLength*/)
/**
	TBTPortDefaultState Write.
**/
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::ReadCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::WriteCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::DoRunL(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::DoCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::DoLockedAction(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::DoWriteCompleted(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortDefaultState::DoReadCompleted(CBTPortProxy* /*aContext*/,TInt aError) 
	{ 
	// because we managed to get reopened before destruction 
	// and this event sources from the reader's request completing 
	// after a read cancel was issued from the closing. 
	LOG_FUNC
	if(aError==KErrCancel) {return;}  

	BAD_BTCOMM_EVENT 
	} 


void TBTPortDefaultState::Error(CBTPortProxy* aContext,TInt aError)
/**
	This will call each state's error loging and move the CSY to the error state.
*/
	{
	LOG_FUNC
	LogStateError(aContext,aError);
	aContext->iLastError=aError; // record this before we lose context
	aContext->MoveToErrorState();
	}


// ****** Common Pre-Connection I/O Base State  *****

TBTPortCommonBaseState::TBTPortCommonBaseState(CBTPortStateFactory* aParent)
	:TBTPortDefaultState(aParent)
	{
	LOG_FUNC
	}

void TBTPortCommonBaseState::Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength)
	{
	LOG_FUNC

	if(aLength)
		{
		aContext->iClientReadPtr=aPtr;
		aContext->iClientReadLength=aLength;
		aContext->iClientRemainderToRead=aLength;
#ifdef _DEBUG
	aContext->iReadsPending++;
#endif
		}
	else
		{
		//zero length read, complete it immediately
		aContext->iPort->ReadCompleted(KErrNone);
		}
	}

void TBTPortCommonBaseState::Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength)
	{
	LOG_FUNC

	aContext->iClientWritePtr=aPtr;
	aContext->iClientWriteLength=aLength;
	aContext->iClientLengthWrittenSoFar=aLength;
	}

void TBTPortCommonBaseState::WriteCancel(CBTPortProxy* aContext)
/**
	Cancel initial Write if there was one or do nothing.
*/
	{
	LOG_FUNC

	if(aContext->iClientWritePtr)
		{
		aContext->iClientWritePtr=NULL;
		aContext->iClientWriteLength=0;
		aContext->iClientLengthWrittenSoFar=0;
		aContext->iPort->WriteCompleted(KErrCancel);
		}
	}

void TBTPortCommonBaseState::ReadCancel(CBTPortProxy* aContext)
/**
	Cancel initial Read if there was one or do nothing.
*/
	{
	LOG_FUNC

	if(aContext->iClientReadPtr)
		{
		aContext->iClientReadPtr=NULL;
		aContext->iClientReadLength=0;
		aContext->iClientRemainderToRead=0;
#ifdef _DEBUG
		aContext->iReadsPending--;
#endif
		aContext->iPort->ReadCompleted(KErrCancel);
		}
	}

void TBTPortCommonBaseState::Close(CBTPortProxy* aContext)
	{
	LOG_FUNC

	aContext->StopReader();

	//check to see if we had any client requests cached and cancel them
	if(aContext->iClientReadPtr)
		{
		ReadCancel(aContext);
		}	
	if(aContext->iClientWritePtr)
		{
		WriteCancel(aContext);
		}

	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EClosing));
	}

//		***** Idle state *****

TBTPortStateIdle::TBTPortStateIdle(CBTPortStateFactory* aParent)
	:TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateIdle::~TBTPortStateIdle()
	{
	LOG_FUNC
	}

void TBTPortStateIdle::Open(CBTPortProxy* aContext)
/**
	This function is the entry point for the BT CSY state machine.
	The first thing we will do is to pretend we opened a BTCOMM connection.
	The reason for this is to avoid any deadlock across the C32-ESock-ETel 
	trilogy.
  **/
	{
	LOG_FUNC
	aContext->CancelShutDownTimer(); // just in case we were going down before this call
	}

void TBTPortStateIdle::Close(CBTPortProxy* aContext)
/**
	A call to this method will cancel any pending requests.
	It will also cancel any transition to any other state.
  */	
	{
	LOG_FUNC
	
	aContext->Cancel();// any transition

	//check to see if we had any client requests cached and cancel them
	ReadCancel(aContext);	
	WriteCancel(aContext);
	}


void TBTPortStateIdle::Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)
/**
	Caches the Read() for completion after the connection is setup, starts the protocol and state machine.
*/
	{
	LOG_FUNC
	LOG(_L("**TBTPortStateIdle::Read -- Queueing one**"));
	TBTPortCommonBaseState::Read(aContext,aPtr,aLength);
	// Connect to ESock
	SockServConnect(aContext);
	}

void TBTPortStateIdle::Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)
/**
	Caches the Write() for completion after the connection is setup, starts the protocol and state machine.
*/
	{
	LOG_FUNC
	LOG(_L("**TBTPortStateIdle::Write -- Queueing one**"));
	TBTPortCommonBaseState::Write(aContext,aPtr,aLength);
	// Connect to ESock
	SockServConnect(aContext);
	}


void TBTPortStateIdle::DoRunL(CBTPortProxy* /*aContext*/)
/**
	Will be called only after we  decided to close in the Discovering state.
	This is a NoOp method.
 */
	{
	LOG_FUNC
	}

void TBTPortStateIdle::SockServConnect(CBTPortProxy* aContext)
/**
	Makes an asynchronous attempt to connect to ESock and moves to the next state.
**/
	{
	LOG_FUNC
	aContext->iSockServConnector->SockServConnect(aContext->iStatus);
	
	// now move to the next state where we pre-load the protocol
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::ELoadingProtocol));
	aContext->SetActive();
	}

void TBTPortStateIdle::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}
	
//		***** Loading Protocol state *****

TBTPortStateLoadingProtocol::TBTPortStateLoadingProtocol(CBTPortStateFactory* aParent)
	:TBTPortCommonBaseState(aParent), iClosePending(EFalse)
	{
	LOG_FUNC
	}

TBTPortStateLoadingProtocol::~TBTPortStateLoadingProtocol()
	{
	LOG_FUNC
	}

void TBTPortStateLoadingProtocol::Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength)
	{
	LOG_FUNC

	// Cache the read ready for when our connection is up. 
	if(aLength)
		{
		aContext->iClientReadPtr=aPtr;
		aContext->iClientReadLength=aLength;
		aContext->iClientRemainderToRead=aLength;
#ifdef _DEBUG
	aContext->iReadsPending++;
#endif
		}
	else
		{
		//zero length read, complete it immediately
		aContext->iPort->ReadCompleted(KErrNone);
		}

	// If we were planning on closing this reopens us - we've cached the read as
	// normal, just unset the close pending flag and we can carry on setting up
	// the connection as normal.
	if(!iClosePending)
		{
		iClosePending = EFalse;
		}
	}

void TBTPortStateLoadingProtocol::Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength)
	{
	LOG_FUNC
	
	// Cache the write ready for when our connection is up.  
	aContext->iClientWritePtr=aPtr;
	aContext->iClientWriteLength=aLength;
	aContext->iClientLengthWrittenSoFar=aLength;
	
	// If we were planning on closing this reopens us - we've cached the write as
	// normal, just unset the close pending flag and we can carry on setting up
	// the connection as normal.
	if(iClosePending)
		{
		iClosePending = EFalse;
		}
	}

void TBTPortStateLoadingProtocol::Close(CBTPortProxy* aContext)
/**
	A call to this method will cancel any pending requests.
	It will also cancel any transition to any other state.
  */	
	{
	LOG_FUNC
	
	// If we're in this state our connection attempt on ESock hasn't
	// completed yet.  We can't transition to the closing state yet 
	// because we don't have the handle for our ESock session which 
	// we need to close.  When the connection request completes we'll 
	// do the transition, unless there's a Read or Write before then, 
	// when we'll allow the connection set up to continue.
	
	//check to see if we had any client requests cached and cancel them
	ReadCancel(aContext);	
	WriteCancel(aContext);
	
	// Set the flag so we know where to go when the Esock connect has
	// completed
	iClosePending = ETrue;
	}

void TBTPortStateLoadingProtocol::DoRunL(CBTPortProxy* aContext)
/**
	Will be called only after we  decided to close in the Discovering state.
	This is a NoOp method.
 */
	{
	LOG_FUNC
	
	if (aContext->iStatus != KErrNone)
		{
		Error(aContext, aContext->iStatus.Int());	// couldn't connect to esock
		return;
		}
		
	// If we're waiting for the ESock handle to allow us to close, transition
	// to the closing state now, otherwise continue connection set up.
	if(iClosePending)
		{
		aContext->SetState(iFactory->GetState(CBTPortStateFactory::EClosing));
		aContext->SetActive();
		TRequestStatus* pStatus = &(aContext->iStatus);
		User::RequestComplete(pStatus, KErrNone);
		
		// Reset this incase we come back through the state machine later
		iClosePending = EFalse;
		}
	else
		{
		StartProtocol(aContext);
		}
	}

void TBTPortStateLoadingProtocol::StartProtocol(CBTPortProxy* aContext)
/**
	Makes an asynchronous attempt to invoke RSocketServ::StartProtocol() and moves to the next state.
**/
	{
	LOG_FUNC
    // now async load Bluetooth RFComm protocol to stop comm port open thread lock.
	RSocketServ &sockServ = aContext->iSockServ;
	LOG1(_L("TBTPortStateLoadingProtocol::Start L2CAP Protocol OK, iSockServ=0x%x"),sockServ.Handle());
    sockServ.StartProtocol(KBTAddrFamily,KSockSeqPacket,KL2CAP,aContext->iStatus);
	// now move to the next state were the results will be handled
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EDiscovering));
	aContext->SetActive();
	}

void TBTPortStateLoadingProtocol::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}

//		***** Discovering state *****

TBTPortStateDiscovering::TBTPortStateDiscovering(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateDiscovering::~TBTPortStateDiscovering()
	{
	LOG_FUNC
	}


void TBTPortStateDiscovering::DoRunL(CBTPortProxy* aContext)
/**
	Find-out the port's corresponding device and settings.
	This is done by looking at the registry default settings for this 
	BTComm port. 
**/
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// could not start Bluetooth protocol 
		// must error here so that the user notices this
		Error(aContext,ret);
		LOG(_L(" Could not load Bluetooth protocol !"));		
		// do tidy up here
		return;
		}
	else
		{// we have successfully started Bluetooth, lets discover the settings for this port
		aContext->iDefaultService.SetPort(aContext->iPortNo);
		ret=aContext->iPortSettings.Get(aContext->iDefaultService); 
		
		if(ret!=KErrNone)
			{
			LOG(_L(" Could not find default device !"));		
			Error(aContext,ret);
			return;
			}
		aContext->iBdaddr=aContext->iDefaultService.BDAddr();
				
		// In the context of RFCOMM, at this stage, we would be looking
		// to do an SDP service search query on the remote RFCOMM service
		// which means opening an SDP sap first which is a synchronous 
		// action so has to be done as a locked action.
		aContext->StartLocker();
		}
	}

void TBTPortStateDiscovering::DoLockedAction(CBTPortProxy* aContext)
/**
	Do an SDP query for the remote service port number as a synchronous (locked) action.
	At this point we have locked the session.  We now need to open
	our NetDB and then follow that with our async state change action 
	which is an SDP query.
**/
	{
	LOG_FUNC
	// need to open the netdb, fire off an async SDP Connect Query
	// and then do the state transition.
	RNetDatabase& netdb=aContext->iNetDatabase;
	RSocketServ ss=aContext->iSockServ;
	// note this needs to be a reference.
	TInt ret=netdb.Open(ss,KBTAddrFamily,KSDP);
	// sync action completed ok - now stop locker.
	aContext->StopLocker();
	if (ret!=KErrNone)	
		{
		Error(aContext,ret);
		return;
		}

	// if NetDB was opened ok make a note of it for cleanup on closing state
	aContext->SetNetDbInUse();
	
	// now queue async action and move into next state
	// which is an SDP connect query on remote RFCOMM service.
	TSDPConnectQuery connQ;
	connQ.iQueryType = KSDPConnectQuery;
	connQ.iAddr = aContext->iBdaddr;
	aContext->iSDPRequest.Copy(TSDPConnectBuf(connQ));
	aContext->iSDPResult.SetMax();
	netdb.Query(aContext->iSDPRequest,aContext->iSDPResult,aContext->iStatus);
	aContext->SetActive();
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::ESDPConnected));
	}

void TBTPortStateDiscovering::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
/**
	TBTPortStateDiscovering Error.
	Reaching this error means that we haven't found a device.
**/
	{
	LOG_FUNC
	}

//		***** SDP Connected state *****

TBTPortStateSDPConnected::TBTPortStateSDPConnected(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateSDPConnected::~TBTPortStateSDPConnected()
	{
	LOG_FUNC
	}

void TBTPortStateSDPConnected::DoRunL(CBTPortProxy* aContext)
/**
	Entry at this call signals that the SDP connection query has completed.  
	We now need to check the remote port number to connect to before 
	invoking the locker to handle the opening of the port proxy's socket.
**/
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// didn't manage to succeed with SDP Connect query.
		Error(aContext,ret);
		LOG(_L("CSY: Could not connect to remote SDP server !"));
		return;
		}
	// we have successfully found the remote SDP server.
	RNetDatabase& netdb=aContext->iNetDatabase;
		
	// here we create the SDP service search query. 
	TSDPServiceSearchKey key;
	key.iQueryType = KSDPServiceQuery;
	key.iMaxCount = 20;
	key.iUUID = aContext->iDefaultService.UUID();

	key.iStateLength = 0;

	aContext->iSDPRequest.Copy(TSDPServiceSearchKeyBuf(key));
	aContext->iSDPServRecordHandle.SetMax(); 
	netdb.Query(aContext->iSDPRequest,aContext->iSDPServRecordHandle,aContext->iStatus);
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::ESDPServiceQuery));
	aContext->SetActive();		
	}

void TBTPortStateSDPConnected::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
/**
	Reaching this error means that we haven't found a remote device
	with the right service level on it.  ie. the SDP/IAS query failed.
**/
	{
	LOG_FUNC
	}


//      ***** SDP Service Retrieved State *****

TBTPortStateSDPServiceQuery::TBTPortStateSDPServiceQuery(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateSDPServiceQuery::~TBTPortStateSDPServiceQuery()
	{
	LOG_FUNC
	}

void TBTPortStateSDPServiceQuery::DoRunL(CBTPortProxy* aContext)
	{
	LOG_FUNC
	// we have completed the SDP service search query
	// get number of record handles in this response
	TInt ret=aContext->iStatus.Int();

	if (ret!=KErrNone)
		{// could not start Bluetooth protocol 
		// must error here so that the user notices this
		Error(aContext,ret);
		LOG(_L("SDPService query - error."));		
		// do tidy up here
		return;
		}
	else
		{
		
		iFactory->iSDPServRecordHandleCount=BigEndian::Get16(&aContext->iSDPServRecordHandle[2]);
		if (!iFactory->iSDPServRecordHandleCount)
			{// this is an error - found no matching service records.
			Error(aContext,KErrNotFound);
			return;
			}
		if (iFactory->iSDPServRecordHandleCount*4 > aContext->iSDPServRecordHandle.Length() - 5)
			{// this is a check on the length of the response.			
			Error(aContext,KErrUnknown);
			LOG2(_L("Bad length field %d on results length %d"), 
				iFactory->iSDPServRecordHandleCount*4,aContext->iSDPServRecordHandle.Length());
			return;
			}
			
		LOG(_L("\nFound Service records for UUID 0x03\n")); //add the number of records found
			
		//now retrieve the ServiceClassIDList from the first ServiceRecordHandle
		RNetDatabase& netdb=aContext->iNetDatabase;

		TSDPAttributeKey key;
		Mem::FillZ(&key, sizeof(TSDPAttributeKey));
		key.iQueryType = KSDPAttributeQuery;
		// suck out the first service record handle.
		iFactory->iExtractedHandleCount = 1;
		key.iServiceRecordHandle = BigEndian::Get32(&aContext->iSDPServRecordHandle[4]);
		key.iMaxLength = 200;
		key.iRange = EFalse;
		key.iAttribute = KSdpAttrIdServiceClassIDList; 
		key.iStateLength = 0;
		aContext->iSDPRequest.Copy(TSDPAttributeKeyBuf(key));
		aContext->iSDPResult.SetMax();
		// Now go away and do the SDP attribute request query
		netdb.Query(aContext->iSDPRequest,aContext->iSDPResult,aContext->iStatus);
		aContext->SetState(iFactory->GetState(CBTPortStateFactory::ESDPServiceIDListRetrieved));		
		aContext->SetActive();
		}
	}

void TBTPortStateSDPServiceQuery::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
/**
	Reaching this error means that we haven't found a remote device
	with the right service level on it.  ie. the SDP query failed.
**/
	{
	LOG_FUNC
	}



//      ***** SDP Service ID List Retrieved State *****

TBTPortStateServiceIDListRetrieved::TBTPortStateServiceIDListRetrieved(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}


TBTPortStateServiceIDListRetrieved::~TBTPortStateServiceIDListRetrieved()
	{
	LOG_FUNC
	}

void TBTPortStateServiceIDListRetrieved::DoRunL(CBTPortProxy* aContext)
	{
	LOG_FUNC

	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// could not start Bluetooth protocol 
		// must error here so that the user notices this
		Error(aContext,ret);
		LOG(_L("Service ID List error."));		
		// do tidy up here
		return;
		}
	else
		{
		RNetDatabase& netdb=aContext->iNetDatabase;

		const TInt KRspAttributeCountSize = 2;
		const TInt KContStateHeader = 1;

		LOG(_L("Successful Attribute query!!\r\nHere's the result....\n"));
		FTRACE(FHex(aContext->iSDPResult));

		if (aContext->iSDPResult.Length() < KRspAttributeCountSize+KContStateHeader)
			{
			Error(aContext,KErrNotSupported);
			LOG1(_L("Result is far too short at %d bytes\r\n"),aContext->iSDPResult.Length());
			return;
			}
		// suck out the byte count
		TUint16 bytecount=BigEndian::Get16(&aContext->iSDPResult[0]);
		if (aContext->iSDPResult.Length() < KRspAttributeCountSize+bytecount+KContStateHeader)
			{
			Error(aContext,KErrNotSupported);
			return;
			}
		if (aContext->iSDPResult[KRspAttributeCountSize+bytecount]!=0)
			{// continuation state to deal with
			// fixme - do something here!
			Error(aContext,KErrNotSupported);
			LOG(_L("Continuation state to deal with in SDP attr query response!!\r\n"));
			return;
			}

		//check for the expected UUID in the ServiceIDList

		CRFCommClass* builder=CRFCommClass::NewL(aContext->iDefaultService.UUID());
		CleanupStack::PushL(builder);
		CElementParser* parser = CElementParser::NewL(builder);
		CleanupStack::PushL(parser);
		//Parsing simple attribute list
		TRAPD(err, parser->ParseElementsL(aContext->iSDPResult.Mid(KRspAttributeCountSize,bytecount)));
		if (err)
			{
			LOG1(_L("Parser left with error %d\n"),err);
			Error(aContext,err);
			return;
			}
			
		TBool getProtocolDesc = builder->InService();
		CleanupStack::PopAndDestroy(2);

		TSDPAttributeKey key;
		Mem::FillZ(&key, sizeof(TSDPAttributeKey));
		key.iMaxLength = 200;
		key.iRange = EFalse;
		key.iStateLength = 0;
		key.iQueryType = KSDPAttributeQuery;
		
		aContext->iSDPResult.Zero();
		aContext->iSDPResult.SetMax();

		if (getProtocolDesc)
			{
			LOG(_L("**UUID found in ServiceClassIDList, moving on to getting Protocol Desc **"));


			// suck out the next service record handle.
			//if there are still some Handles left
			key.iServiceRecordHandle = BigEndian::Get32(&aContext->
												iSDPServRecordHandle[4*iFactory->iExtractedHandleCount]); //this handle
			key.iAttribute = KSdpAttrIdProtocolDescriptorList; 
			aContext->iSDPRequest.Copy(TSDPAttributeKeyBuf(key));
			// Now go away and do the SDP attribute request query
			// i.e search for the first service record, to find what protocols are supported
			netdb.Query(aContext->iSDPRequest,aContext->iSDPResult,aContext->iStatus);
			aContext->SetState(iFactory->GetState(CBTPortStateFactory::ESDPAttribListRetrieved));		
			aContext->SetActive();
			}
		else if ( iFactory->iExtractedHandleCount < (iFactory->iSDPServRecordHandleCount) )
			{
			LOG(_L("**UUID not found in ServiceClassIDList, inquire of next ServiceRecordHandle  **"));

			// suck out the next service record handle.
			//if there are still some Handles left
			iFactory->iExtractedHandleCount++;
			key.iServiceRecordHandle = BigEndian::Get32(&aContext->
												iSDPServRecordHandle[4*iFactory->iExtractedHandleCount]); //next handle
			key.iAttribute = KSdpAttrIdServiceClassIDList; 
			aContext->iSDPRequest.Copy(TSDPAttributeKeyBuf(key));
			// Now go away and do the SDP attribute request query
			netdb.Query(aContext->iSDPRequest,aContext->iSDPResult,aContext->iStatus);
			//state is remaining unchanged so no need to call aContext->SetState()
			aContext->SetActive();
			
			}
		else //UUID has not been found & there are no more ServiceRecordHandles 
			{
			LOG(_L("**UUID not found and no ServiceRecordHandles left **"));
			Error(aContext,KErrNotFound);
			}
		}
	}


void TBTPortStateServiceIDListRetrieved::LogStateError(CBTPortProxy* /*aContext*/, TInt /*aError*/)
	{
	LOG_FUNC
	}


//      ***** SDP Attribute List Retrieved State *****

TBTPortStateSDPAttributeListRetrieved::TBTPortStateSDPAttributeListRetrieved(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateSDPAttributeListRetrieved::~TBTPortStateSDPAttributeListRetrieved()
	{
	LOG_FUNC
	}


void TBTPortStateSDPAttributeListRetrieved::DoRunL(CBTPortProxy* aContext)
/**
	The SDP attribute request query completed and here we parse the results.
*/
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// could not start Bluetooth protocol 
		// must error here so that the user notices this
		Error(aContext,ret);
		LOG(_L("AttributeListRetrieved - error."));		
		// do tidy up here
		return;
		}
	else
		{	
		const TInt KContStateHeader = 1;
		const TInt KRspAttributeCountSize = 2;

		if (aContext->iSDPResult.Length() < KRspAttributeCountSize+KContStateHeader)
			{
			Error(aContext,KErrNotSupported);
			LOG1(_L("Result is far too short at %d bytes\r\n"),aContext->iSDPResult.Length());
			return;
			}
		// suck out the byte count
		TUint16 bytecount=BigEndian::Get16(&aContext->iSDPResult[0]);
		if (aContext->iSDPResult.Length() < KRspAttributeCountSize+bytecount+KContStateHeader)
			{
			Error(aContext,KErrNotSupported);
			return;
			}
		if (aContext->iSDPResult[KRspAttributeCountSize+bytecount]!=0)
			{// continuation state to deal with
			// fixme - do something here!
			Error(aContext,KErrNotSupported);
			LOG(_L("Continuation state to deal with in SDP attr query response!!\r\n"));
			return;
			}
		LOG(_L("Successful Attribute query!!\r\nHere's the result....\n"));
		FTRACE(FHex(aContext->iSDPResult));
				
		// Parse the attribute list
				
		CRFCommAttribs* builder=CRFCommAttribs::NewL(NULL);
		CleanupStack::PushL(builder);
		CElementParser* parser = CElementParser::NewL(builder);
		CleanupStack::PushL(parser);
		//Parsing simple attribute list
		TUint rem=0;

		TRAPD(err,rem=parser->ParseElementsL(aContext->iSDPResult.Mid(KRspAttributeCountSize,bytecount)));
		if (err)
			{
			LOG1(_L("Parser left with error %d\n"),err);
			Error(aContext,err);
			// cleanup since the leave/error does not get propagated and we return.
			CleanupStack::PopAndDestroy(2);
			return;
			}
		else
			{
			LOG1(_L("Parser returned %d\n"),rem);
			}
			
		(void)(rem != KMaxTUint); // keep the compiler happy by taking rem as an r-value in urel
				
		// get the remote rfcomm port number to connect to.

		err = builder->GetRFCommPort(aContext->iRemoteRfcommPortNumber); // channel number
		if (err != KErrNone)
			{
			LOG1(_L("Error %d determining RFCOMM server channel\n"), err);
			Error(aContext,err);
			}

		CleanupStack::PopAndDestroy(2);  // getting rid of parser and builder.

		// do an RFCOMM connect on the remote RFCOMM port
		// which means opening the socket first which is a synchronous 
		// action so has to be done as a locked action.
		aContext->StartLocker();
		}
	}

void TBTPortStateSDPAttributeListRetrieved::DoLockedAction(CBTPortProxy* aContext)
/**
	Get a locked connection to the Socket server/RFComm and attempt a connection to the remote RFComm port.
**/
	{
	LOG_FUNC
	
	TPckgBuf<TUint8> aSig;
	RSocket& sock=aContext->iSocket;
	RSocketServ ss=aContext->iSockServ;
	// note this needs to be a reference.
	TInt ret=sock.Open(ss,KBTAddrFamily,KSockStream,KRFCOMM);
	if (ret!=KErrNone)
		{
		Error(aContext,ret);
		LOG(_L("**TBTPortStateSDPAttributeListRetrieved could NOT open RFComm socket connection **"));
		return;
		}
	aSig = KModemSignalDV; // KModemSignalRTC and KModemSignalRTR have been turned off
	sock.SetOpt(KRFCOMMErrOnMSC, KSolBtRFCOMM, aSig);
	// sync action completed ok - now stop locker.
	aContext->StopLocker();
	// now queue async action and move into next state
	// which is an RFComm connect on remote port.
	aContext->iAddr.SetBTAddr(aContext->iBdaddr);
	aContext->iAddr.SetPort(aContext->iRemoteRfcommPortNumber);

	TBTServiceSecurity security;
	security.SetAuthentication(aContext->iDefaultService.IsSecuritySet()); // note: does this need to be updated to be SSP aware?

	TBool doEncrypt = aContext->iDefaultService.IsEncryptionSet();
	security.SetEncryption(doEncrypt);

	aContext->iAddr.SetSecurity(security);

	sock.Connect(aContext->iAddr, aContext->iStatus);
	aContext->SetActive();	
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EConnecting));
	}


void TBTPortStateSDPAttributeListRetrieved::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
/**
	Signals a problem with the retrieval of the remote SDP attribute List.
**/
	{
	LOG_FUNC
	}


//		***** Connecting state *****

TBTPortStateConnecting::TBTPortStateConnecting(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateConnecting::~TBTPortStateConnecting()
	{
	LOG_FUNC
	}

void TBTPortStateConnecting::DoRunL(CBTPortProxy* aContext)
/**
	At this point we have finally got our connection response.
	If successful then we are in the open state and can begin
	to freely handle reads and writes.  The only state transition 
	that can take us away from the open state is a close.
**/
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// didn't manage to succeed with connect.
		Error(aContext,ret);
		LOG(_L("Didn't manage to succeed with connect"));
		return;
		}
	else
		{// we have successfully connected!!!!
		// so we can transition to the open state.
		// first we check if we have any queued writes
		// to do and we also queue a read.
		if (aContext->iClientWritePtr)
			{
			if (aContext->iClientWriteLength>KBTCOMMSendBufferLength)
				{
				aContext->iClientLengthWrittenSoFar=KBTCOMMSendBufferLength;
				aContext->iMoreSendsToCome=ETrue;
				}
			//else see TBTPortStateConnecting::Write(..)

			TPtr8& sendptr=aContext->iSendBufPtr;
			sendptr.SetLength(0);
			TPtr8 ptr((TUint8*)sendptr.Ptr(),0,aContext->iClientLengthWrittenSoFar);
			if(aContext->iClientLengthWrittenSoFar>0)
				{ // to avoid panicing on zero length write to from client
				aContext->iPort->IPCRead(aContext->iClientWritePtr,ptr,0);
				}
			sendptr.Append(ptr);
			aContext->iSendBufPtr.SetLength(aContext->iClientLengthWrittenSoFar);
			aContext->StartWriter();
			}

		if((aContext->iClientReadLength==0)&&(aContext->iClientReadPtr))
			{// someone asked for a zero length read
			aContext->iPort->ReadCompleted(KErrNone);
			}
		aContext->StartReader();
		aContext->SetState(iFactory->GetState(CBTPortStateFactory::EOpen));
		}
	}

void TBTPortStateConnecting::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
/**
	TBTPortStateConnecting Error.
	At this point our RFComm connect has failed for some reason.
**/
	{
	LOG_FUNC
	}

//		***** Open state *****

TBTPortStateOpen::TBTPortStateOpen(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent) 
	{
	LOG_FUNC
	}

TBTPortStateOpen::~TBTPortStateOpen()
	{
	LOG_FUNC
	}

void TBTPortStateOpen::Close(CBTPortProxy* aContext)
/**
	TBTPortStateOpen Close.
	This function is called when the user has invoked a Close on
	the CSY session.
	It will stop the reader and begin the shutdown and close of the socket
	session.
**/
	{
	LOG_FUNC
	
	aContext->StopReader();

	// the close() call is a direct call to C32 from the client (in ESock probably)
	// thus can pre-empt both DoLockedAction() and DoRunL(). Consequently
	// if someone issued a WriteCancel(), just before the call
	// this needs to be serviced within a DoLockedAction. But since Shutdown
	// starts here, this will not be needed. Therefore we will NOT stop the locker
	// here and will check wether a cancel is pending in the Closing state's
	// DoLockedAction().

	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EClosing));
	aContext->SetActive();
	TRequestStatus* pStatus = &(aContext->iStatus);
	User::RequestComplete(pStatus, KErrNone); 
	}

void TBTPortStateOpen::Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)
/**
	The client side has requested that a read call be completed.  
	Moreover if the reader (for the socket reads) was stoped it 
	will be restarted from here if the low watermark for the CBTPortProxy
	read-in circular buffer was reached.
**/
	{
	LOG_FUNC
#ifdef _DEBUG
	aContext->iReadsPending++;
#endif

	aContext->iClientReadPtr=aPtr;
	aContext->iClientReadLength=aLength;
	aContext->iClientRemainderToRead=aLength;
	aContext->iClientWriteOffset=0; // to indicate the offset in the buf 
									// for the first write back to client
	if(aLength)
		{
		// need to check if there's anything in the circular buffer.
		HandleIPCWriteToClient(aContext);
		}
	else
		{
		aContext->iPort->ReadCompleted(KErrNone);
		}
	// unfortunately the state has to decide whether the reader should be 
	// kicked into action again or not, because there are states like the 
	// Idle one where this is irrelevant.
	if(aContext->ReadInBufferLowWatermarkReached())
		{
		// and the reader was previously stoped
		if(!aContext->iPortReader->IsReading())
			{
			aContext->StartReader();
			}
		}
	}

void TBTPortStateOpen::DoReadCompleted(CBTPortProxy* aContext,TInt aError)
/**
	New data arrived from the socket to C32	
	This method is called by the CBTportProxy when new data arrive 
	from the socket.

	This function first needs to see if there's anything that needs to 
	be written client side.  HandleIPCWriteToClient() does the actual 
	work of invoking the c32 IPCWrite.  

	Then it checks to see if more reads to the socket should be queued or
	whether the CBTPortProxy read-in buffer is filling up.
**/
	{
	LOG_FUNC
#ifdef _DEBUG
	aContext->iReadsPending--;
#endif

	if ((aError!=KErrNone)&&(aError!=KErrCancel))
		{// we need to break out of here without requeueing the reader.
		aContext->iPort->ReadCompleted(aError);
		return;
		}
	if (aContext->iClientReadPtr)
		{// we have a read queued - 
		// need to see if we can do IPCWrite client side
		HandleIPCWriteToClient(aContext);
		}

	// check to see if we've reached or surpased our watermark
	if(aContext->ReadInBufferHighWatermarkReached())
		{
		aContext->StopReader();
		return;
		} 
	// else
	aContext->StartReader();

	}

void TBTPortStateOpen::HandleIPCWriteToClient(CBTPortProxy* aContext)
/**
	This function works out how much of the circular buffer to write back to the client. 
	The c32 ReadCompleted() function is only invoked when the complete read has been done.
**/
	{
	LOG_FUNC
	TInt dataLenInBuf=aContext->iCircularReadBuf->Count();
	if (dataLenInBuf==0)
		{
		return; //nothing to do
		}

	TInt bytesToCopy=0;
	TPtr8& readptr=aContext->iReadBufPtr; // incoming data buffer
	readptr.SetLength(0);
	TUint8* ptr=CONST_CAST(TUint8*,(readptr.Ptr()));
	TInt clientMaxLen=aContext->iClientReadLength;

	// This should cover terminated reads, plain reads and some read one 
	// or more cases		
	if(dataLenInBuf>=clientMaxLen)		
		{
		// complete if possible any terminated reads for both larger or smaller 
		// client buffers than ours
		if(aContext->iTerminatedReads)
			{ // try to find the terminated string
			// bytesToCopy here can be greater than the clientMaxLen
			// or can be -1 so we have to check
			bytesToCopy=aContext->iCircularReadBuf->ScanForTerminator(aContext->iTerm);
			if((bytesToCopy>clientMaxLen) || (bytesToCopy == KErrNotFound))
				{
				bytesToCopy=clientMaxLen;
				}
			// else the terminated reads complete with no terminator because 
			// the client buffer can be filled completely.
			}
		else
			{
			// The plain reads where the client has a smaller buffer should 
			// complete here for the same reason.
			// Read one or more would complete anyway for this case.
			bytesToCopy=clientMaxLen;
			}
		}
	// This should cover Receive one or more for the rest of the cases

	//will allow the full coverage of the high watermark case even if we do not do bounded reads
//#define BTCOMM_TEST_HIGH_WATERMARK
#ifndef BTCOMM_TEST_HIGH_WATERMARK
	else if(aContext->iClientReadOneOrMore)
		{
		// if we reached this point it means that datLenInBuf<clientMaxLen
		bytesToCopy=dataLenInBuf;			
		}
#endif
	// Our client can handle lots of data, more than we can
	else if(KBTCOMMCircularBufferLength<clientMaxLen)
		{		
		if(aContext->iClientRemainderToRead<=dataLenInBuf)
			{ // then this is the last chunk of a multi write to client
			if(aContext->iTerminatedReads)
				{ // try to find the terminated string
				// bytesToCopy here can be -1 
				// hence we have to check
				bytesToCopy=aContext->iCircularReadBuf->ScanForTerminator(aContext->iTerm);
				if(bytesToCopy==KErrNotFound)
					{// ternminator was requested but not found... what the heck
					bytesToCopy=aContext->iClientRemainderToRead;
					}
				}
			else
				{
				// this is the last chunk to be read for this client read
				// write as much as needed to the client
				bytesToCopy=aContext->iClientRemainderToRead;
				}
			}
		else if(aContext->ReadInBufferHighWatermarkReached()) // to minimise IPC
			{ 
			if(aContext->iTerminatedReads)
				{ // try to find the terminated string
				// bytesToCopy here can be -1 
				// hence we have to check
				bytesToCopy=aContext->iCircularReadBuf->ScanForTerminator(aContext->iTerm);
				if(bytesToCopy==KErrNotFound)
					{// terminator was not found... but
					// we have almost filled our buffer; write what we've got to the client
					bytesToCopy=aContext->iCircularReadBuf->Remove(ptr,dataLenInBuf);
					__ASSERT_DEBUG(bytesToCopy==dataLenInBuf,PanicInState(EBTCommOpenStateWriteToClientPossibleLossOfData));
					
					// fill their buf as much as we can and continue
					readptr.SetLength(bytesToCopy);
					TInt offset=aContext->iClientWriteOffset;
					aContext->iPort->IPCWrite(aContext->iClientReadPtr,readptr,offset);
					aContext->iClientRemainderToRead-=bytesToCopy;
					aContext->iClientWriteOffset+=bytesToCopy; // offset for next write to client
					return;	//our work is done wait for the next 
					}
				// else the terminator was found hence copy data up to the 
				// terminator and complete the read 
				}
			else // if no terminated reads were requested but the high watermark was reached
				{
				// copy what we have across and keep track of the offset
				bytesToCopy=aContext->iCircularReadBuf->Remove(ptr,dataLenInBuf);
				__ASSERT_DEBUG(bytesToCopy==dataLenInBuf,PanicInState(EBTCommOpenStateWriteToClientPossibleLossOfData));
					
				// fill their buf as much as we can and continue
				readptr.SetLength(bytesToCopy);
				TInt offset=aContext->iClientWriteOffset;
				aContext->iPort->IPCWrite(aContext->iClientReadPtr,readptr,offset);
				aContext->iClientRemainderToRead-=bytesToCopy;
				aContext->iClientWriteOffset+=bytesToCopy; // offset for next write to client
				return;	//our work is done wait for the next
				}
			} //matches else if(aContext->ReadInBufferHighWatermarkReached())
		// if the high watermark was not reached but we do terminated reads  
#ifndef BTCOMM_TEST_HIGH_WATERMARK 
		else if (aContext->iTerminatedReads) 
			{
			bytesToCopy=aContext->iCircularReadBuf->ScanForTerminator(aContext->iTerm);
			}
#endif
		}
	// complete if possible any terminated reads for smaller 
	// client buffers than ours
	else if(aContext->iTerminatedReads)
		{ // try to find the terminated string
		// bytesToCopy here cannot be greater than the clientMaxLen
		bytesToCopy=aContext->iCircularReadBuf->ScanForTerminator(aContext->iTerm);
		}	

	if(bytesToCopy>KErrNone) 
		{
		// do the copy to the client buffer
		TInt offset=aContext->iClientWriteOffset;
		readptr.SetLength(bytesToCopy);
		bytesToCopy=aContext->iCircularReadBuf->Remove(ptr,bytesToCopy);
		aContext->iPort->IPCWrite(aContext->iClientReadPtr,readptr,offset);
	
		// reset the offsets and counters and complete the read
		aContext->iClientReadLength=0;
		aContext->iClientReadPtr=0;
		aContext->iClientRemainderToRead=0;
		aContext->iClientWriteOffset=0;  
		
		aContext->iPort->ReadCompleted(KErrNone);
		}
	}

void TBTPortStateOpen::Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)
/**
	TBTPortState Open.
	Store the client side ptr and length and then signal
	start write immediately.
**/
	{
	LOG_FUNC
	// this is again due to the fact that we have to be asynchronous to avoid 
	// deadlocks with (mostly ESock) other comms components.
	if(aContext->IsWriteCancelPending())
		{
		aContext->iPort->WriteCompleted(KErrNotReady);
		//aContext->DoWriteCancel(); a deadlock example !
		return;
		}

	LOG(_L("**TBTPortStateOpen::Write**"));
	aContext->iClientWritePtr=aPtr;
	aContext->iClientWriteLength=aLength;
	aContext->iClientLengthWrittenSoFar=aLength;

	if (aLength>KBTCOMMSendBufferLength)
		{
		aContext->iClientLengthWrittenSoFar=KBTCOMMSendBufferLength;
		aContext->iMoreSendsToCome=ETrue;
		}
	TPtr8& sendptr=aContext->iSendBufPtr;
	sendptr.SetLength(0);
	// now read the client side data
	TPtr8 ptr((TUint8*)sendptr.Ptr(),0,aContext->iClientLengthWrittenSoFar);
	aContext->iPort->IPCRead(aPtr,ptr,0);

	sendptr.Append(ptr);
	aContext->iSendBufPtr.SetLength(aContext->iClientLengthWrittenSoFar);
	aContext->StartWriter();
	}

void TBTPortStateOpen::DoWriteCompleted(CBTPortProxy* aContext,TInt aError)
/**
	TBTPortStateOpen DoWriteCompleted.	
	If there is no error here, at this point the write 
	has successfully completed and
	can be signalled complete back to the client code.
**/
	{
	LOG_FUNC
	if (aContext->iMoreSendsToCome)
		{// we need to keep going 
		LOG(_L("**TBTPortStateOpen::DoWriteCompleted - more to come**"));

		TInt lentowrite=aContext->iClientWriteLength-aContext->iClientLengthWrittenSoFar;
		TInt lensofar=aContext->iClientLengthWrittenSoFar;
		if (lentowrite>KBTCOMMSendBufferLength)
			{// still more to come
			lentowrite=KBTCOMMSendBufferLength;
			aContext->iClientLengthWrittenSoFar+=KBTCOMMSendBufferLength;
			aContext->iMoreSendsToCome=ETrue;
			}
		else
			{
			aContext->iClientLengthWrittenSoFar+=lentowrite;
			aContext->iMoreSendsToCome=EFalse;
			}
		TPtr8 sendptr=aContext->iSendBufPtr;
		sendptr.SetLength(0);
		// read the client side data
		TPtr8 ptr((TUint8*)sendptr.Ptr(),0,lentowrite);
		aContext->iPort->IPCRead(aContext->iClientWritePtr,ptr,lensofar);
		
		sendptr.Append(ptr);
		aContext->iSendBufPtr.SetLength(lentowrite);
		aContext->StartWriter();
		}
	else
		{// we have finished - can signal to client and 0 the client stuff.
		LOG(_L("**TBTPortStateOpen::DoWriteCompleted - finished**"));
		aContext->iMoreSendsToCome=EFalse;
		aContext->iClientLengthWrittenSoFar=0;
		aContext->iClientWritePtr=0;                 
		aContext->iClientWriteLength=0;
		aContext->iPort->WriteCompleted(aError);
		}
	}

void TBTPortStateOpen::WriteCancel(CBTPortProxy* aContext)
/**
	It will complete the cancelation to the client and then request it from the server.
*/
	{
	LOG_FUNC
	// do say yes you cancelled and then cancel if necessary
	aContext->iPort->WriteCompleted(KErrCancel);
	aContext->SetWriteCancelPending();
	aContext->StartLocker();
	}

void TBTPortStateOpen::ReadCancel(CBTPortProxy* aContext)
/**
	It will complete the cancelation to the client.
*/
	{
	LOG_FUNC
#ifdef _DEBUG
	aContext->iReadsPending--;
#endif
	// do say yes you cancelled and then cancel if necessary
	aContext->iPort->ReadCompleted(KErrCancel);
	// Read Cancel does not need to be propagated to the underlying socket
	// since CSY has a ring buffer which is filled with inbound data regardless
	// of what the upper app is up to with its RComm
	}

void TBTPortStateOpen::DoLockedAction(CBTPortProxy* aContext)
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)	
		{
		aContext->StopLocker();
		Error(aContext,ret);
		return;
		}
	if(aContext->IsWriteCancelPending())
		{
		aContext->DoWriteCancel(); // the locked action for which we came in here
		}
	else //default handle - stray event must be caught
		{
		((TBTPortDefaultState*)this)->DoLockedAction(aContext);
		}
	aContext->StopLocker();
	//NOTE: we remain in this state
	}

void TBTPortStateOpen::LogStateError(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}

//		***** Closing state *****

TBTPortStateClosing::TBTPortStateClosing(CBTPortStateFactory* aParent)
	: TBTPortCommonBaseState(aParent)
	{
	LOG_FUNC
	}

TBTPortStateClosing::~TBTPortStateClosing()
	{
	LOG_FUNC
	}

void TBTPortStateClosing::Close(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortStateClosing::Read(CBTPortProxy* aContext, TAny* aPtr,TInt aLength)
/**
	Read in this state indirectly calls the DoLockedAction to get out and in the state machine again.
	This is done because of the huge window of opportunity for race conditions
	while the async timer is ticking before shutdown. Since we need the 
	shutdown timer for the closing to be async (to avoid deadlocks with Etel etc), 
	hence we should close the socket etc and transfer the Read() to the
	Idle state.
*/
	{
	LOG_FUNC
	
	LOG(_L("**TBTPortStateClosing Canceling the shutdown timer **"));
	aContext->CancelShutDownTimer();
	
	// cache the read
	aContext->iClientReadPtr=aPtr;
	aContext->iClientReadLength=aLength;
	aContext->iClientRemainderToRead=aLength;
	// close sockets resolver etc	
	aContext->StartLocker(); // next call is to our DoLockedAction(..);	
	// we now know that someone re-opened us while going down so...
	// transfer the call
	}

void TBTPortStateClosing::Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)
/**
	Write in this state indirectly calls the DoLockedAction to get out and in the state machine again.
	This is done because of the huge window of opportunity for race conditions
	while the async timer is ticking before shutdown. Since we need the 
	shutdown timer for the closing to be async (to avoid deadlocks with Etel etc), 
	hence we should close the socket etc and transfer the write() to the
	Idle state.
*/
	{
	LOG_FUNC

	LOG(_L("**TBTPortStateClosing Canceling the shutdown timer **"));
	aContext->CancelShutDownTimer();

	// cache the write
	aContext->iClientWritePtr=aPtr;
	aContext->iClientWriteLength=aLength;
	aContext->iClientLengthWrittenSoFar=aLength;
	
	// close sockets resolver etc
	//next call is in our DoLockedAction(..);
	aContext->StartLocker();
	// we now know that someone re-opened us while going down so...
	// transfer the call
	}

void TBTPortStateClosing::DoRunL(CBTPortProxy* aContext)
/**
	At this point our socket shutdown has returned.
	We can now close down the host resolver, netdb and finally
	the session to the socket server itself.  These are all 
	synchronous actions so must be handled by the locker.

	But we need to defer the actual completion of the final closing for later
	in order to give some time for other components like Etel to close first.
	Note the values used here are arbitrary and should be considered a 
	hacked fix :-(.

  	NOTE: the next transition is in TBTPortStateClosing::DoLockedAction 
	      unless the shutdown timer is cancelled by a new read or write 
**/
	{
	LOG_FUNC
	TInt ret=aContext->iStatus.Int();
	if (ret!=KErrNone)
		{// didn't manage to succeed with socket shutdown somehow.
		LOG(_L("**TBTPortStateClosing Closing with an error !**"));
		aContext->StartShutdownTimerL();// this will eventually call aContext->StartLocker()
		}
	else
		{// we have successfully shutdown the socket.
		aContext->StartShutdownTimerL();// this will eventually call aContext->StartLocker()		
		}
	//NOTE: the next transition is in TBTPortStateClosing::DoLockedAction 
	//      unless the shutdown timer is cancelled by a new read or write     
	}

void TBTPortStateClosing::DoLockedAction(CBTPortProxy* aContext)
/**
	TBTPortStateClosing DoLockedAction.
	At this point we have obtained the lock hence we are able to undertake
	the locked actions.
	This method should be called indirectly by the shutdown timer or due to
	a previous WriteCancel call at the open state, followed by a 
	Close() call ( which brought as here in this state).
**/
	{
	LOG_FUNC
	if(aContext->IsWriteCancelPending())
		{
		LOG(_L("**TBTPortStateClosing::DoLockedAction -- Servicing Write cancelations first**"));
		aContext->DoWriteCancel(); // the locked action for which we came in here
		aContext->StopLocker();
		return;
		}
		
	if(aContext->IsNetDbInUse())
		{
		// we need this in the case of cancelation before the netdb.Open()
		// because the kernel will panic us if it doesn't find the session
		RNetDatabase& netdb=aContext->iNetDatabase;
		netdb.Cancel();
		netdb.Close();
		aContext->SetNetDbNotInUse();
		LOG(_L("**TBTPortStateClosing, NetDB session clean-up done **"));
		}

	// the following immediate shutdown will not stiff C32 and is needed for 
	// the next close to be possible in ESock without it being blocked waiting
	// for the protocol SAP to be cleaned
	//
	// This shutdown will cause any outstanding socket ops to be cancelled so
	// we can safely call aContext->Cancel() afterwards.
	if(aContext->iSocket.SubSessionHandle())
		{// Make sure we had managed to open the socket connection before we arriveed here		
		aContext->iSocket.Shutdown(RSocket::EImmediate,aContext->iStatus);
		User::WaitForRequest(aContext->iStatus); 
		}
		
	// Do BT CSY specific locked action.
  	aContext->iSocket.Close();

	// We can only make synch calls to ESock from within the locker, otherwise 
	// we risk deadlock.  This means we can't put any Cancel()s that may end
	// up in ESock in the port proxy's DoCancel() unless we guarantee that Cancel()
	// can only be called from within the locker.  Instead we ensure we've done
	// all the necessary cancelling before calling Cancel().
	aContext->Cancel();
   

	// We should never transition to the closing state whilst waiting for
	// an ESock connection, unless I've bugged the state machine.
	__ASSERT_DEBUG(!aContext->iSockServConnector->IsActive(), PanicInState(EBTCommCloseWhileWaitingForSockServHandle));

   	if(aContext->iSockServ.Handle())
   		{
  		aContext->iSockServ.Close();
   		}
   	aContext->StopLocker();	// sync actions completed ok - now stop locker.

	// we do this just before destruction, just in case someone has reopened us
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EIdle));
	// UGLY
	aContext->DestructContext(); // will not do anything if we got reopened
	}

void TBTPortStateClosing::DoWriteCompleted(CBTPortProxy * /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}

void TBTPortStateClosing::DoReadCompleted(CBTPortProxy * /*aContext*/, TInt /*aError*/)
	{
	LOG_FUNC
	}

void TBTPortStateClosing::LogStateError(CBTPortProxy * /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}


// Error State

TBTPortErrorState::TBTPortErrorState(CBTPortStateFactory* aParent) 
	: TBTPortState(aParent)
	{
	LOG_FUNC
	}

TBTPortErrorState::~TBTPortErrorState()
	{
	LOG_FUNC
	}

void TBTPortErrorState::Open(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::Read(CBTPortProxy* aContext,TAny* /*aPtr*/,TInt /*aLength*/)
/**
	Notify the client about this recent error.
*/	
	{
	LOG_FUNC
	aContext->iPort->ReadCompleted(aContext->iLastError);
	}

void TBTPortErrorState::Write(CBTPortProxy* aContext,TAny* /*aPtr*/,TInt /*aLength*/)
/**
	Notify the client about this recent error.
*/	
	{
	LOG_FUNC
	aContext->iPort->WriteCompleted(aContext->iLastError);
	}

void TBTPortErrorState::Close(CBTPortProxy* aContext)
	{
	LOG_FUNC
	aContext->SetState(iFactory->GetState(CBTPortStateFactory::EClosing));
		
	// check to see if the error came before we managed to do the locked action 
	// ..and cancel the pending locked action then
	if(aContext->IsLockerOn()) 
		{            
		aContext->StopLocker();
		}
	aContext->SetActive();
	TRequestStatus* pStatus = &(aContext->iStatus);
	User::RequestComplete(pStatus, KErrNone); 
	}

void TBTPortErrorState::DoRunL(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::DoCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::WriteCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::ReadCancel(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::DoLockedAction(CBTPortProxy* /*aContext*/)
	{
	LOG_FUNC
	BAD_BTCOMM_EVENT
	}

void TBTPortErrorState::DoWriteCompleted(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::DoReadCompleted(CBTPortProxy* /*aContext*/,TInt /*aError*/)
	{
	LOG_FUNC
	}

void TBTPortErrorState::Error(CBTPortProxy* aContext,TInt aError)
/**
	Error will be the first method to be called in entering this state.
	So here Error will check to see if there are any Queued reads, which 
	it will immediately complete.
	
	This should only be called on moving from any other state to the error 
	state and not otherwise.
 */
	{
	LOG_FUNC
	if (aContext->iClientReadPtr)
		{ // this means that a read was queued already so we need to signal 
		  // the error immediately
		aContext->iPort->ReadCompleted(aError);		
		}
	if (aContext->iClientWritePtr)
		{ // same for write
		aContext->iPort->WriteCompleted(aError);
		}
	}

