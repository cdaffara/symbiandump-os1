// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <kernel/kern_priv.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <comms-infras/zerocopy_loopback_driver.h>
#else
//this header is not exported, needs to be a user include
#include "zerocopy_loopback_driver.h"
#endif

#include "device.h"
#include <comms-infras/commsbufpond.h>


_LIT(KZeroCopyLoopbackPanicCategory,"ZeroCopyLoopback");


/**
  Standard export function for LDDs. This creates a DLogicalDevice derived object,
  in this case, our DZeroCopyLoopbackFactory
*/
DECLARE_STANDARD_LDD()
	{
	return new DZeroCopyLoopbackFactory;
	}

/**
  Constructor
*/
DZeroCopyLoopbackFactory::DZeroCopyLoopbackFactory()
	{
	// Set version number for this device
	iVersion=RZeroCopyLoopbackDriver::VersionRequired();
	// Indicate that we work with a PDD
	iParseMask=KDeviceAllowPhysicalDevice;
	}


/**
  Second stage constructor for DZeroCopyLoopbackFactory.
  This must at least set a name for the driver object.

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DZeroCopyLoopbackFactory::Install()
	{
	return SetName(&RZeroCopyLoopbackDriver::Name());
	}

DZeroCopyLoopbackFactory::~DZeroCopyLoopbackFactory()
   	{
   	}

/**
  Return the drivers capabilities.
  Called in the response to an RDevice::GetCaps() request.

  @param aDes User-side descriptor to write capabilities information into
*/
void DZeroCopyLoopbackFactory::GetCaps(TDes8& aDes) const
	{
	// Create a capabilities object
	RZeroCopyLoopbackDriver::TCaps caps;
	caps.iVersion = iVersion;
	// Write it back to user memory
	Kern::InfoCopy(aDes,(TUint8*)&caps,sizeof(caps));
	}

/**
  Called by the kernel's device driver framework to create a Logical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aChannel Set to point to the created Logical Channel

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DZeroCopyLoopbackFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel=new DZeroCopyLoopbackChannel;
	if(!aChannel)
		return KErrNoMemory;

	return KErrNone;
	}

DZeroCopyLoopbackChannel::DZeroCopyLoopbackChannel()
	:	iSendDataDfc(SendDataDfc, this, 1),        // DFC is priority '1'
		iReceiveDataDfc(ReceiveDataDfc, this, 1)   // DFC is priority '1'
	{
	// Get pointer to client threads DThread object
	iClient=&Kern::CurrentThread();

	// Open a reference on client thread so it's control block can't dissapear until
	// this driver has finished with it.
	// Note, this call to Open can't fail since its the thread we are currently running in
	iClient->Open();
	memclr(&iDebugProbes, sizeof(iDebugProbes));
	Kern::Printf("clearing probe points\n");
	}

/**
  Second stage constructor called by the kernel's device driver framework.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version argument supplied by the client to RBusLogicalChannel::DoCreate

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DZeroCopyLoopbackChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer)
	{
	// Check Platform Security capabilities of client thread (if required).
	//
	// Here we handle the simple case where:
	// 1. The device driver can only have one client thread
	// 2. The security policy is the binary all-or-nothing policy.
	//    E.g. "If you have the right capability you can do anything with the driver
	//    and if you don't have the capability you can't do anything"
	// 
	// If only some functionality of the driver is restricted, then the security check should
	// go elsewhere. E.g. in DoRequest/DoControl. In that case Kern::CurrentThreadHasCapability
	// shouldn't be used because the 'current thread' isn't the client.
	//
	// In this example we do a check here for ECapability_None (which always passes)...
	if(!Kern::CurrentThreadHasCapability(ECapability_None,__PLATSEC_DIAGNOSTIC_STRING("Checked by zerocopy loopback")))
		return KErrPermissionDenied;

	// Check version
	if (!Kern::QueryVersionSupported(RZeroCopyLoopbackDriver::VersionRequired(),aVer))
		return KErrNotSupported;

	// Setup LDD for receiving client messages
	SetDfcQ(((DZeroCopyLoopbackPddFactory*)iPhysicalDevice)->iDfcQ);
	iMsgQ.Receive();

	// Associate DFCs with the same queue we set above to receive client messages on
	iSendDataDfc.SetDfcQ(iDfcQ);
	iReceiveDataDfc.SetDfcQ(iDfcQ);

	iPond = DCommsPond::New();
	if(!iPond)
		{
		return KErrNoMemory;
		}
	
	// Give PDD a pointer to this channel
	Pdd()->iLdd = this;

	// Done
	return KErrNone;
	}

#define PRINT_PROBE_POINTS(name) \
	{\
	Kern::Printf("probe list:%s\n\t", name); \
	for(TInt i = 0; i < (sizeof(iDebugProbes) / sizeof(TInt)); i++)\
		{ \
		Kern::Printf("%d:%d, ", i, iDebugProbes[i]); \
		} \
	Kern::Printf("\n\n"); \
	}

#define PROBE_LDD(index) (iDebugProbes[index]++)
#define PROBE_LDD_STATIC(index, obj) (((obj)->iDebugProbes[index])++)

DZeroCopyLoopbackChannel::~DZeroCopyLoopbackChannel()
	{
	// Cancel all processing that we may be doing
	DoCancel(RZeroCopyLoopbackDriver::EAllRequests);
	
	delete iPond;
	
	// Close our reference on the client thread
	Kern::SafeClose((DObject*&)iClient,NULL);
	}

/**
  Called when a user thread requests a handle to this channel.
*/
TInt DZeroCopyLoopbackChannel::RequestUserHandle(DThread* aThread, TOwnerType aType)
	{
	// Make sure that only our client can get a handle
	if (aType!=EOwnerThread || aThread!=iClient)
		return KErrAccessDenied;
	return KErrNone;
	}

/**
  Process a message for this logical channel.
  This function is called in the context of a DFC thread.

  @param aMessage The message to process.
	              The iValue member of this distinguishes the message type:
	              iValue==ECloseMsg, channel close message
	              iValue==KMaxTInt, a 'DoCancel' message
	              iValue>=0, a 'DoControl' message with function number equal to iValue
	              iValue<0, a 'DoRequest' message with function number equal to ~iValue
*/
void DZeroCopyLoopbackChannel::HandleMsg(TMessageBase* aMsg)
	{
	TThreadMessage& m=*(TThreadMessage*)aMsg;

	// Get message type
	TInt id=m.iValue;

	// Decode the message type and dispatch it to the relevent handler function...

	if (id==(TInt)ECloseMsg)
		{
		// Channel Close
		PRINT_PROBE_POINTS("ldd");
		DoCancel(RZeroCopyLoopbackDriver::EAllRequests);
		m.Complete(KErrNone, EFalse);
		return;
		}

	if (id==KMaxTInt)
		{
		// DoCancel
		DoCancel(m.Int0());
		m.Complete(KErrNone,ETrue);
		return;
		}

	if (id<0)
		{
		// DoRequest
		TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
		TInt r=DoRequest(~id,pS,m.Ptr1(),m.Ptr2());
		if (r!=KErrNone)
			{
			Kern::RequestComplete(iClient,pS,r);
			}
		m.Complete(KErrNone,ETrue);
		}
	else
		{
		// DoControl
		TInt r=DoControl(id,m.Ptr0(),m.Ptr1());
		m.Complete(r,ETrue);
		}
	}

/**
  Process synchronous 'control' requests
*/
TInt DZeroCopyLoopbackChannel::DoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	(void)a2;   // a2 not used in this example

	TInt r;

	switch (aFunction)
		{
		case RZeroCopyLoopbackDriver::EGetConfig:
			r = GetConfig((TDes8*)a1);
			break;

		case RZeroCopyLoopbackDriver::ESetConfig:
			r = SetConfig((const TDesC8*)a1);
			break;

		case RZeroCopyLoopbackDriver::ELoadPond:
			r = LoadPond((TPondTransferBuf*)a1);
			break;
		
		case RZeroCopyLoopbackDriver::EUnloadPond:
			r = UnloadPond();
			break;
		
		default:
			r = KErrNotSupported;
			break;
		}

	return r;
	}

/**
  Process asynchronous requests.
*/
TInt DZeroCopyLoopbackChannel::DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2)
	{
	(void)a2;   // a2 not used in this example

	TInt r;

	switch(aReqNo)
		{
		case RZeroCopyLoopbackDriver::ESendData_ByZeroCopy:
			r = SendData(aStatus, (TInt)a1);
			break;

		case RZeroCopyLoopbackDriver::EReceiveData_ByZeroCopy:
			// Example Platform Security capability check which tests the
			// client for ECapability_None (which always passes)...
			if(iClient->HasCapability(ECapability_None,__PLATSEC_DIAGNOSTIC_STRING("Checked by zerocopy loopback")))
				r = ReceiveData(aStatus, (TInt*)a1);
			else
				r = KErrPermissionDenied;
			break;

		default:
			r = KErrNotSupported;
			break;
		}

	return r;
	}

/**
  Process cancelling of asynchronous requests.
*/
void DZeroCopyLoopbackChannel::DoCancel(TUint aMask)
	{
	if(aMask&(1<<RZeroCopyLoopbackDriver::ESendData))
		SendDataCancel();
	if(aMask&(1<<RZeroCopyLoopbackDriver::EReceiveData))
		ReceiveDataCancel();
	}

/**
  Process a GetConfig control message. This writes the current driver configuration to a
  RZeroCopyLoopbackDriver::TConfigBuf supplied by the client.
*/
TInt DZeroCopyLoopbackChannel::GetConfig(TDes8* aConfigBuf)
	{
	// Create a structure giving the current configuration
	RZeroCopyLoopbackDriver::TConfig config;
	CurrentConfig(config);

	// Write the config to the client
	TPtrC8 ptr((const TUint8*)&config,sizeof(config));
	return Kern::ThreadDesWrite(iClient,aConfigBuf,ptr,0,KTruncateToMaxLength,NULL);
	}

/**
  Process a SetConfig control message. This sets the driver configuration using a
  RZeroCopyLoopbackDriver::TConfigBuf supplied by the client.
*/
TInt DZeroCopyLoopbackChannel::SetConfig(const TDesC8* aConfigBuf)
	{
	// Don't allow configuration changes whilst we're busy
	if(iSendDataStatus || iReceiveDataStatus)
		return KErrInUse;

	// Create a config structure.
	RZeroCopyLoopbackDriver::TConfig config;
	CurrentConfig(config);

	// Note: We have filled config with the current settings, this is to allow
	// backwards compatibility when a client gives us an old (and shorter) version
	// of the config structure.

	// Read the config structure from client
	TPtr8 ptr((TUint8*)&config,sizeof(config));
	TInt r=Kern::ThreadDesRead(iClient,aConfigBuf,ptr,0);
	if(r!=KErrNone)
		return r;

	// Use config data to setup the driver. Checking that parameters which aren't settable
	// either contain the correct values or are zero (meaning 'default')
	if(config.iPddBufferSize && config.iPddBufferSize!=Pdd()->BufferSize())
		return KErrArgument;

	if(config.iMaxSendDataSize && config.iMaxSendDataSize!=KLoopbackMTU)
		return KErrArgument;

	if(config.iMaxReceiveDataSize && config.iMaxReceiveDataSize!=KLoopbackMTU)
		return KErrArgument;

	r=Pdd()->SetSpeed(config.iSpeed);
	if(r!=KErrNone)
		return r;

	return r;
	}

/**
@purpose Create a local pond with a flattened description passed from the client
@param aPond The pond in a descriptor form
*/
TInt DZeroCopyLoopbackChannel::LoadPond(TPondTransferBuf* aPond)
	{
	// Load the local pond with the flattened representation of it
	TInt r = iPond->Load(*aPond, iClient);
	if(r != KErrNone)
		{
		return r;
		}
	
	// Now seems like the right time to set the default allocation pool
	r = iPond->SetDefaultAllocPool(KLoopbackMTU);
	if (r != KErrNone)
		{
		iPond->Unload();
		return r;
		}
	
	return KErrNone;
	}



/**
@purpose Unloads the local commsbuf pond
*/
TInt DZeroCopyLoopbackChannel::UnloadPond()
	{
	__ASSERT_DEBUG(iPond, Kern::ThreadKill(iClient, EExitPanic, 0, KZeroCopyLoopbackPanicCategory));
	if(iPond)
		{
		iPond->Unload();
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

/**
  Fill a TConfig with the drivers current configuration.
*/
void DZeroCopyLoopbackChannel::CurrentConfig(RZeroCopyLoopbackDriver::TConfig& aConfig)
	{
	aConfig.iSpeed = Pdd()->Speed();
	aConfig.iPddBufferSize = Pdd()->BufferSize();
	aConfig.iMaxSendDataSize = KLoopbackMTU;
	aConfig.iMaxReceiveDataSize = KLoopbackMTU;
	}

/**
@purpose Processes a request to send a commsbuf
@param aStatus
@param aUserHandle Opaque handle to commsbuf to be sent.
*/
TInt DZeroCopyLoopbackChannel::SendData(TRequestStatus* aStatus, TInt aUserHandle)
	{
	// Check that a 'SendData' isn't already in progress
	PROBE_LDD(0);
	if(iSendDataStatus)
		{
		Kern::ThreadKill(iClient, EExitPanic, ERequestAlreadyPending, KZeroCopyLoopbackPanicCategory);
		return KErrInUse;
		}

	// Open the commsbuf received from the user for ourselves - the user side hold on it will be dropped
	// We need to provide a kernel private location for the incoming buffer so the current send buffer will do nicely
	DCommsBuf* sendBuf = Pdd()->SendBuffer();
	if(sendBuf)
		{
		TInt r = DCommsBuf::AcceptFromClient(iClient, aUserHandle, sendBuf);

		// Outbound packet loaded in to the output buffer so can now ask the pdd to send it
		if(r == KErrNone)
			{
			r = Pdd()->RequestDataSend();
			if(r != KErrNone)
				return r;

			// Save the client request status and return
			iSendDataStatus = aStatus;
			return KErrNone;
			}
		else
			{
			return r;
			}
		}
	else
		{
		// No where to put outbound buffer so can only drop it
		DCommsBuf sendBuf;
		TInt r = DCommsBuf::AcceptFromClient(iClient, aUserHandle, &sendBuf);
		sendBuf.Free();
		return KErrNone;
		}
	}

/**
  Cancel a SendData request.
*/
void DZeroCopyLoopbackChannel::SendDataCancel()
	{
	if(iSendDataStatus)
		{
		// Tell PDD to stop processing the request
		Pdd()->SendDataCancel();

		// Cancel DFC
		iSendDataDfc.Cancel();

		// Complete clients request
		Kern::RequestComplete(iClient,iSendDataStatus,KErrCancel);
		}
	}

/**
  Called by PDD from ISR to indicate that a SendData operation has completed.
*/
void DZeroCopyLoopbackChannel::SendDataComplete(TInt aResult)
	{
	PROBE_LDD(2);
	// Save result code
	iSendDataResult = aResult;

	// Queue DFC
	iSendDataDfc.Add();
	}

/**
  DFC callback which gets triggered after the PDD has signalled that SendData completed.
  This just casts aPtr and calls DoSendDataComplete().
*/
void DZeroCopyLoopbackChannel::SendDataDfc(TAny* aPtr)
	{
	PROBE_LDD_STATIC(3, ((DZeroCopyLoopbackChannel*)aPtr));
	((DZeroCopyLoopbackChannel*)aPtr)->DoSendDataComplete();
	}

/**
  Called from a DFC after the PDD has signalled that SendData completed.
*/
void DZeroCopyLoopbackChannel::DoSendDataComplete()
	{
	TInt result = iSendDataResult;

	// Complete clients request
	Kern::RequestComplete(iClient,iSendDataStatus,result);
	}

/**
  Start processing a ReceiveData request.
*/
TInt DZeroCopyLoopbackChannel::ReceiveData(TRequestStatus* aStatus, TInt* aRxBufferPtr)
	{
	PROBE_LDD(4);
	// Check that a 'ReceiveData' isn't already in progress
	if(iReceiveDataStatus)
		{
		Kern::ThreadKill(iClient,EExitPanic,ERequestAlreadyPending,KZeroCopyLoopbackPanicCategory);
		return KErrInUse;
		}

	// Save the client request status and descriptor for later completion
	iReceiveDataStatus = aStatus;
	iReceiveDataDescriptor = aRxBufferPtr;
	
	if(Pdd()->ReceivedQueueLen() > 0)
		{
		// complete with waiting data
		DoReceiveDataComplete();
		}

	// Ask PDD for data
	NKern::Lock(); 
	TInt r = Pdd()->RequestDataReceipt();
	NKern::Unlock(); 

	if(r != KErrNone)
		{
		iReceiveDataDescriptor = NULL;
		return r;
		}

	PROBE_LDD(5);
	return KErrNone;
	}

/**
  Cancel a ReceiveData request.
*/
void DZeroCopyLoopbackChannel::ReceiveDataCancel()
	{
	if(iReceiveDataStatus)
		{
		// Tell PDD to stop processing the request
		Pdd()->ReceiveDataCancel();

		// Cancel DFC
		iReceiveDataDfc.Cancel();

		// Finished with client descriptor, so NULL it to help detect coding errors
		iReceiveDataDescriptor = NULL;

		// Complete clients request
		Kern::RequestComplete(iClient,iReceiveDataStatus,KErrCancel);
		}
	}

/**
  Called by PDD from ISR to indicate that a ReceiveData operation has completed.
*/
void DZeroCopyLoopbackChannel::ReceiveDataComplete(TInt aResult)
	{
	PROBE_LDD(6);
	// Save result code
	iReceiveDataResult = aResult;

	// Queue DFC
	NKern::Lock(); 
	iReceiveDataDfc.Add();
	NKern::Unlock(); 
	}

/**
  DFC Callback which gets triggered after the PDD has signalled that ReceiveData completed.
  This just casts aPtr and calls DoReceiveDataComplete().
*/
void DZeroCopyLoopbackChannel::ReceiveDataDfc(TAny* aPtr)
	{
	PROBE_LDD_STATIC(7, ((DZeroCopyLoopbackChannel*)aPtr));
	((DZeroCopyLoopbackChannel*)aPtr)->DoReceiveDataComplete();
	}

/**
  Called from a DFC after the PDD has signalled that ReceiveData completed.
*/
void DZeroCopyLoopbackChannel::DoReceiveDataComplete()
	{
	if(iReceiveDataStatus)
		{
		PROBE_LDD(8);

		// Fetch the receive buffer. We shouldn't be told to complete rx unless there is data waiting
		DCommsBuf* rxBuf = Pdd()->ReceiveBuffer();
		__ASSERT_ALWAYS(rxBuf, Kern::Fault("comms loopback", KErrUnderflow));

		// Prepare commsbuf for handing to the client and close our reference to it
		TInt result;
		result = rxBuf->TransferToClient(iClient);
		
		// If we managed to create a handle for the client
		if(result > 0)
			{
			// Complete client's request (it will carry the handle to the received buffer)
			Kern::RequestComplete(iClient, iReceiveDataStatus, result);
			}

		// Step the receive queue
		Pdd()->AdvanceReceiveQueue();
		}
	}
