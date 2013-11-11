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

// Name for PDD, must match LDD name with a '.' and distinguishing name appended
_LIT(KZeroCopyLoopbackPddName,"ZeroCopyLoopbackDriver.pdd");


class TQueue
	{
public:
	DCommsBuf* HeadBuffer() {return &iQueue[iQueueHead];}
	DCommsBuf* TailBuffer() {return &iQueue[iQueueTail];}
	
	TBool IsEmpty() const {return iLength == 0;}
	TBool IsFull() const {return iLength == KLoopbackQueueLen;}
	TInt MaxLength() const {return KLoopbackQueueLen;}
	TInt Length() const {return iLength;}

	void Pop()
		{
		__ASSERT_ALWAYS(iLength > 0, Kern::Fault("comms loopback", KErrOverflow));
		iQueueHead++;
		if(iQueueHead == KLoopbackQueueLen)
			{
			iQueueHead = 0;
			}
		iLength--;
		}

	void Push()
		{
		__ASSERT_ALWAYS(iLength < KLoopbackQueueLen, Kern::Fault("comms loopback", KErrOverflow));
		iQueueTail++;
		if(iQueueTail == KLoopbackQueueLen)
			{
			iQueueTail = 0;
			}
		iLength++;
		}

	TQueue() : iQueueHead(0), iQueueTail(0), iLength(0) {}

private:
	DCommsBuf iQueue[KLoopbackQueueLen];
	TInt iQueueHead;
	TInt iQueueTail;
	TInt iLength;
	};


class DZeroCopyLoopbackDevice : public DZeroCopyLoopback
	{
public:
	DZeroCopyLoopbackDevice(DZeroCopyLoopbackPddFactory* aFactory);
	~DZeroCopyLoopbackDevice();
	TInt DoCreate();
	// Inherited from DZeroCopyLoopback. These called by the LDD.
	virtual TInt BufferSize() const;
	virtual TInt Speed() const;
	virtual TInt SetSpeed(TInt aSpeed);
	virtual TInt RequestDataSend();
	virtual void SendDataCancel();
	virtual TInt RequestDataReceipt();
	virtual void ReceiveDataCancel();
	virtual DCommsBuf* SendBuffer();
	virtual DCommsBuf* ReceiveBuffer();
	virtual TBool ReceivedQueueLen();
	virtual void AdvanceReceiveQueue();

private:
	void SendDataCallback();
	void ReceiveDataCallback();
	
	static void DataRecvCallback(TAny* aSelf);
	void DoDataRecvCallback();

private:
	DZeroCopyLoopbackPddFactory* iFactory;
	TInt iSpeed;

	TQueue iSendQueue;
	TQueue iReceiveQueue;

	TBool iPendingRead;
	TDfc iRecvDataDfc;
	};



const TInt KZeroCopyLoopbackThreadPriority = 27;
_LIT(KZeroCopyLoopbackThread,"ZeroCopyLoopbackThread");

/**
  Standard export function for PDDs. This creates a DPhysicalDevice derived object,
  in this case, our DZeroCopyLoopbackPddFactory
*/
DECLARE_STANDARD_PDD()
	{
	return new DZeroCopyLoopbackPddFactory;
	}

DZeroCopyLoopbackPddFactory::DZeroCopyLoopbackPddFactory()
	{
	// Set version number for this device
	iVersion = RZeroCopyLoopbackDriver::VersionRequired();
	}

/**
  Second stage constructor for DPhysicalDevice derived objects.
  This must at least set a name for the driver object.

  @return KErrNone or standard error code.
*/
TInt DZeroCopyLoopbackPddFactory::Install()
	{
	// Allocate a kernel thread to run the DFC 
	TInt r = Kern::DynamicDfcQCreate(iDfcQ, KZeroCopyLoopbackThreadPriority, KZeroCopyLoopbackThread);
	if (r == KErrNone)
		{ 
		r = SetName(&KZeroCopyLoopbackPddName);
		} 	
	return r;
	}

/**
  Returns the drivers capabilities. This is not used by the Symbian OS device driver framework
  but may be useful for the LDD to use.

  @param aDes Descriptor to write capabilities information into
*/
void DZeroCopyLoopbackPddFactory::GetCaps(TDes8& aDes) const
	{
	// Create a capabilities object
	DZeroCopyLoopback::TCaps caps;
	caps.iVersion = iVersion;
	// Zero the buffer
	TInt maxLen = aDes.MaxLength();
	aDes.FillZ(maxLen);
	// Copy cpabilities
	TInt size=sizeof(caps);
	if(size>maxLen)
		size=maxLen;
	aDes.Copy((TUint8*)&caps,size);
	}

/**
  Called by the kernel's device driver framework to create a Physical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aChannel Set to point to the created Physical Channel
  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version number of the Logical Channel which will use this Physical Channel 

  @return KErrNone or standard error code.
*/
TInt DZeroCopyLoopbackPddFactory::Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
	{
	// Ignore the parameters we aren't interested in...
	(void)aUnit;
	(void)aInfo;
	(void)aVer;

	// Create a new physical channel
	DZeroCopyLoopbackDevice* device = new DZeroCopyLoopbackDevice(this);
	aChannel=device;
	if (!device)
		return KErrNoMemory;
	return device->DoCreate();
	}

/**
  Called by the kernel's device driver framework to check if this PDD is suitable for use with a Logical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version number of the Logical Channel which will use this Physical Channel 

  @return KErrNone or standard error code.
*/
TInt DZeroCopyLoopbackPddFactory::Validate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
	{
	// Check version numbers
	if ((!Kern::QueryVersionSupported(iVersion,aVer)) || (!Kern::QueryVersionSupported(aVer,TVersion(EMinimumLddMajorVersion,EMinimumLddMinorVersion,EMinimumLddBuild))))
		return KErrNotSupported;

	// We don't support units
    if (aUnit != -1)
        return KErrNotSupported;

	// Ignore extra info, (this could be used for validation purposes)
	// Note, aInof is a pointer to a descriptor in user memory, therefore safe methods should
	// be used for reading its contents. E.g. using Kern::KUDesGet()
	(void)aInfo;

	// OK
	return KErrNone;
	}

DZeroCopyLoopbackPddFactory::~DZeroCopyLoopbackPddFactory()
	{
	if (iDfcQ)
		iDfcQ->Destroy();
	}

DZeroCopyLoopbackDevice::DZeroCopyLoopbackDevice(DZeroCopyLoopbackPddFactory* aFactory)
	:	iFactory(aFactory),
		iSpeed(100000),  // 100000us (100ms) per byte
		iSendQueue(),
		iReceiveQueue(),
		iRecvDataDfc(DataRecvCallback, this, aFactory->iDfcQ, 1)

	{
	}

DZeroCopyLoopbackDevice::~DZeroCopyLoopbackDevice()
	{
	// Empty the queues
	// This means freeing up any DCommsBufs left inside
	while(!iSendQueue.IsEmpty())
		{
		iSendQueue.HeadBuffer()->Free();
		iSendQueue.Pop();
		}
	while(!iReceiveQueue.IsEmpty())
		{
		iSendQueue.HeadBuffer()->Free();
		iReceiveQueue.Pop();
		}
	
	// Driver no longer using hardware resources
	NKern::LockedDec(iFactory->iHardwareInUse);
	}

TInt DZeroCopyLoopbackDevice::DoCreate()
	{
	// Claim the hardware resources by incrementing iHardwareInUse.
	// Must do this before any other failure can happen in this function so that
	// the destructor can safely decrement iHardwareInUse.
	//
	// This method of ensuring hardware is only in use by one driver at a time
	// wouldn't be needed if the driver claimed real hardware resources which
	// could only be used once. E.g. binding to an interrupt.
	if(NKern::LockedInc(iFactory->iHardwareInUse))
		return KErrInUse;

	// Other setup goes here

	return KErrNone;
	}

TInt DZeroCopyLoopbackDevice::BufferSize() const
	{
	return 1500;
	}

TInt DZeroCopyLoopbackDevice::Speed() const
	{
	return iSpeed;
	}

TInt DZeroCopyLoopbackDevice::SetSpeed(TInt aSpeed)
	{
	if(aSpeed<=0)
		return KErrArgument;
	iSpeed = aSpeed;
	return KErrNone;
	}

TInt DZeroCopyLoopbackDevice::RequestDataSend()
	{
	// Push our send buffer in to the queue
	iSendQueue.Push();
	
	// Trigger reception
	NKern::Lock(); 
	SendDataCallback();
	NKern::Unlock(); 
	
	return KErrNone;
	}

void DZeroCopyLoopbackDevice::SendDataCancel()
	{
	}

void DZeroCopyLoopbackDevice::SendDataCallback()
	{
	// Tell self new data is coming
	iRecvDataDfc.Add();

	// Tell LDD we've done sending
	iLdd->SendDataComplete(KErrNone);
	}

TInt DZeroCopyLoopbackDevice::RequestDataReceipt()
	{
	iPendingRead = ETrue;
	if(!(iReceiveQueue.IsEmpty() && iSendQueue.IsEmpty()))
		{
		NKern::Lock(); 
		ReceiveDataCallback();
		NKern::Unlock(); 
		}
	return KErrNone;
	}

void DZeroCopyLoopbackDevice::ReceiveDataCancel()
	{
	}

void DZeroCopyLoopbackDevice::ReceiveDataCallback()
	{
	// Copy buffer from send queue (it's like our receive hardware) to receive queue
	DCommsBuf* srcBuf = iSendQueue.HeadBuffer();
	if(!iReceiveQueue.IsFull() && !iSendQueue.IsEmpty())
		{
		// Alloc a new buffer in to which we will copy the received (sent) buffer (mimicking DMA in the non zerocopy case)
		DCommsBuf newBuf;
		TInt result = iLdd->Pond().Alloc(newBuf);
		if(result == KErrNone)
			{
			// Copy our buffer and set the new buffer's properties
			TInt srcPayloadLength = srcBuf->Length();
			newBuf.SetOffset(0);
			newBuf.SetLength(srcPayloadLength);

			// Do the copy
			TUint8* srcPayloadPtr = srcBuf->Ptr();
			TUint8* destPayloadPtr = newBuf.Ptr();
			memcpy(destPayloadPtr, srcPayloadPtr, srcPayloadLength);

			// Put the new buffer in the receive queue
			DCommsBuf* destBuf = iReceiveQueue.TailBuffer();
			*destBuf = newBuf;
			iReceiveQueue.Push();

			// Drop the incoming buffer
			srcBuf->Free();

			// Step the queue
			iSendQueue.Pop();
			}

		// We can now complete any outstanding request for received data - if indeed we managed to alloc a receive buffer
		if(iPendingRead && !iReceiveQueue.IsEmpty())
			{
			iPendingRead = FALSE;
			
			// Tell LDD we're done
			iLdd->ReceiveDataComplete(KErrNone);
			}
		}
	}

DCommsBuf* DZeroCopyLoopbackDevice::SendBuffer()
	{
	return iSendQueue.TailBuffer();
	}

DCommsBuf* DZeroCopyLoopbackDevice::ReceiveBuffer()
	{
	return iReceiveQueue.HeadBuffer();
	}

void DZeroCopyLoopbackDevice::AdvanceReceiveQueue()
	{
	iReceiveQueue.Pop();
	}

TInt DZeroCopyLoopbackDevice::ReceivedQueueLen()
	{
	return iReceiveQueue.Length();
	}


void DZeroCopyLoopbackDevice::DataRecvCallback(TAny* aSelf)
	{
	((DZeroCopyLoopbackDevice*)aSelf)->ReceiveDataCallback();
	}
