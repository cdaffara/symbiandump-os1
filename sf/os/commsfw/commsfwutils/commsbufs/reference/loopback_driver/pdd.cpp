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
#include <comms-infras/legacy_loopback_driver.h>
#else
//this header is not exported, needs to be a user include
#include "legacy_loopback_driver.h"
#endif

#include "device.h"

// Name for PDD, must match LDD name with a '.' and distinguishing name appended
_LIT(KLegacyLoopbackPddName,"legacy_loopback.pdd");


typedef TBuf8<KLoopbackMTU> TMTUBuffer;

class TQueue
	{
public:
	TMTUBuffer* HeadBuffer() {return &iQueue[iQueueHead];}
	TMTUBuffer* TailBuffer() {return &iQueue[iQueueTail];}
	
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
	TMTUBuffer iQueue[KLoopbackQueueLen];
	TInt iQueueHead;
	TInt iQueueTail;
	TInt iLength;
	};


class DESockLoopbackDevice : public DESockLoopback
	{
public:
	DESockLoopbackDevice(DESockLoopbackPddFactory* aFactory);
	~DESockLoopbackDevice();
	TInt DoCreate();
	// Inherited from DESockLoopback. These called by the LDD.
	virtual TInt BufferSize() const;
	virtual TInt Speed() const;
	virtual TInt SetSpeed(TInt aSpeed);
	virtual TInt RequestDataSend();
	virtual void SendDataCancel();
	virtual TInt RequestDataReceipt();
	virtual void ReceiveDataCancel();
	virtual TDes8& SendBuffer();
	virtual TDesC8& ReceiveBuffer();
	virtual TBool ReceivedQueueLen();
	virtual void AdvanceReceiveQueue();

private:
	void SendDataCallback();
	void ReceiveDataCallback();
	
	static void DataRecvCallback(TAny* aSelf);
	void DoDataRecvCallback();
private:
	DESockLoopbackPddFactory* iFactory;
	TInt iSpeed;

	TQueue iSendQueue;
	TQueue iReceiveQueue;

	TBool iPendingRead;
	TDfc iRecvDataDfc;
	};



const TInt KESockLoopbackThreadPriority = 27;
_LIT(KESockLoopbackThread,"ESockLoopbackThread");

/**
  Standard export function for PDDs. This creates a DPhysicalDevice derived object,
  in this case, our DESockLoopbackPddFactory
*/
DECLARE_STANDARD_PDD()
	{
	return new DESockLoopbackPddFactory;
	}

DESockLoopbackPddFactory::DESockLoopbackPddFactory()
	{
	// Set version number for this device
	iVersion=RLegacyLoopbackDriver::VersionRequired();
	}

/**
  Second stage constructor for DPhysicalDevice derived objects.
  This must at least set a name for the driver object.

  @return KErrNone or standard error code.
*/
TInt DESockLoopbackPddFactory::Install()
	{
	// Allocate a kernel thread to run the DFC 
	TInt r = Kern::DynamicDfcQCreate(iDfcQ, KESockLoopbackThreadPriority, KESockLoopbackThread);
	if (r == KErrNone)
		{ 
		r = SetName(&KLegacyLoopbackPddName);
		} 	
	return r;
	}

/**
  Returns the drivers capabilities. This is not used by the Symbian OS device driver framework
  but may be useful for the LDD to use.

  @param aDes Descriptor to write capabilities information into
*/
void DESockLoopbackPddFactory::GetCaps(TDes8& aDes) const
	{
	// Create a capabilities object
	DESockLoopback::TCaps caps;
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
TInt DESockLoopbackPddFactory::Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
	{
	// Ignore the parameters we aren't interested in...
	(void)aUnit;
	(void)aInfo;
	(void)aVer;

	// Create a new physical channel
	DESockLoopbackDevice* device=new DESockLoopbackDevice(this);
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
TInt DESockLoopbackPddFactory::Validate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
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

DESockLoopbackPddFactory::~DESockLoopbackPddFactory()
	{
	if (iDfcQ)
		iDfcQ->Destroy();
	}

DESockLoopbackDevice::DESockLoopbackDevice(DESockLoopbackPddFactory* aFactory)
	:	iFactory(aFactory),
		iSpeed(100000),  // 100000us (100ms) per byte
		iSendQueue(),
		iReceiveQueue(),
		iRecvDataDfc(DataRecvCallback, this, aFactory->iDfcQ, 1)

	{
	}

DESockLoopbackDevice::~DESockLoopbackDevice()
	{
	// Driver no longer using hardware resources
	NKern::LockedDec(iFactory->iHardwareInUse);
	}

TInt DESockLoopbackDevice::DoCreate()
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

TInt DESockLoopbackDevice::BufferSize() const
	{
	return iSendQueue.MaxLength();
	}

TInt DESockLoopbackDevice::Speed() const
	{
	return iSpeed;
	}

TInt DESockLoopbackDevice::SetSpeed(TInt aSpeed)
	{
	if(aSpeed<=0)
		return KErrArgument;
	iSpeed = aSpeed;
	return KErrNone;
	}

TInt DESockLoopbackDevice::RequestDataSend()
	{
	// Push our send buffer in to the queue
	iSendQueue.Push();
	
	// Trigger reception
	NKern::Lock(); 
	SendDataCallback();
	NKern::Unlock(); 
	
	return KErrNone;
	}

void DESockLoopbackDevice::SendDataCancel()
	{
	}

void DESockLoopbackDevice::SendDataCallback()
	{
	// Tell self new data is coming
	iRecvDataDfc.Add();

	// Tell LDD we've done sending
	iLdd->SendDataComplete(KErrNone);
	}

TInt DESockLoopbackDevice::RequestDataReceipt()
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

void DESockLoopbackDevice::ReceiveDataCancel()
	{
	}

void DESockLoopbackDevice::ReceiveDataCallback()
	{
	// Copy buffer from send queue (it's like our receive hardware) to receive queue
	if(!iReceiveQueue.IsFull() && !iSendQueue.IsEmpty())
		{
		// Copy the buffer from one queue to the other and patch up descriptor
		TDes8* srcDesc = iSendQueue.HeadBuffer();
		TDes8* destDesc = iReceiveQueue.TailBuffer();

		const TUint8* srcPtr = srcDesc->Ptr();
		const TUint8* destPtr = destDesc->Ptr();

		TInt length = srcDesc->Length();

		memcpy((void*)destPtr, (void*)srcPtr, length);

		destDesc->SetLength(length);
		
		// Bump the receive queue now that we have loaded it
		iReceiveQueue.Push();
	
		// Drop our incoming buffer
		iSendQueue.Pop();

		// If there is a pending read request then we can now satisfy it
		if(iPendingRead)
			{
			iPendingRead = FALSE;

			// Tell LDD we're done
			iLdd->ReceiveDataComplete(KErrNone);
			}
		}
	}

TDes8& DESockLoopbackDevice::SendBuffer()
	{
	return *iSendQueue.TailBuffer();
	}

TDesC8& DESockLoopbackDevice::ReceiveBuffer()
	{
	return *iReceiveQueue.HeadBuffer();
	}

void DESockLoopbackDevice::AdvanceReceiveQueue()
	{
	iReceiveQueue.Pop();
	}

TInt DESockLoopbackDevice::ReceivedQueueLen()
	{
	return iReceiveQueue.Length();
	}


void DESockLoopbackDevice::DataRecvCallback(TAny* aSelf)
	{
	((DESockLoopbackDevice*)aSelf)->ReceiveDataCallback();
	}
