/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "cmmfdevsoundcibitrateimplementationuid.hrh"

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>

#include "cmmfdevsoundcibitrate.h"

// __________________________________________________________________________
// Implementation

////////////////////////////////////////// MUX /////////////////////

TInt CMMFDevSoundCIBitRateMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceBitRateDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

void CMMFDevSoundCIBitRateMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle != -1)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMFDevSoundCIBitRateMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}

void CMMFDevSoundCIBitRateMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}
	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDevSoundCIBitRateMux::NewL()
	{
	CMMFDevSoundCIBitRateMux* self = new (ELeave) CMMFDevSoundCIBitRateMux;
	return self;
	}
	
TAny* CMMFDevSoundCIBitRateMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDevSoundCustomInterfaceBitRate* interface = this;
	return interface;
	}
	
CMMFDevSoundCIBitRateMux::CMMFDevSoundCIBitRateMux() :
	iRemoteHandle(-1)
	{	
	}

CMMFDevSoundCIBitRateMux::~CMMFDevSoundCIBitRateMux()
	{	
	}

// from MMMFDevSoundCustomInterfaceBitRate
void CMMFDevSoundCIBitRateMux::GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates)
	{
	if (iRemoteHandle == -1)
		{
		User::Leave(KErrNotReady);
		}
	
	// first clear out the array
	aSupportedBitRates.Reset();
	
	// now fetch the count from the server
	TInt count = -1;
	count = iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIBitRateGetSupportedBitRates, KNullDesC8);
	
	// if count is negative then the server side left with an error
	if (count < 0)
		{
		User::Leave(count);
		}
	
	// no point getting the data if the count is zero
	if (count != 0)
		{
		// allocate a temporary buffer to hold the bitrates
		HBufC8* buf = HBufC8::NewLC(count * sizeof(TInt32));
		TPtr8 ptr = buf->Des();
	
		// fetch the bitrates - but send over the received count to be sure
		TPckgBuf<TInt> countBuf(count);
		User::LeaveIfError(iUtility->SendSlaveSyncCommandResult(
											 iRemoteHandle, 
											 EMMFDevSoundCIBitRateGetSupportedBitRatesArray,
											 countBuf, ptr));
	
		// stream data into the pointer
		RDesReadStream stream(ptr);
		CleanupClosePushL(stream);
				
		TInt err = KErrNone;
		for (TInt i = 0; i < count; i++)
			{
			// note we don't destroy array because we don't own it
			// but we do reset it as it is incomplete
			err = aSupportedBitRates.Append(stream.ReadInt32L());
			if (err != KErrNone)
				{
				aSupportedBitRates.Reset();
				User::Leave(KErrCorrupt);
				}
			}
		
		CleanupStack::PopAndDestroy(2, buf);// stream, buf
		}
	}
	
TInt CMMFDevSoundCIBitRateMux::BitRateL()
	{
	if (iRemoteHandle == -1)
		{
		User::Leave(KErrNotReady);
		}
	
	// send EMMFDevSoundCIBitRateBitRate command to slave
	TInt bitrate = 0;
	bitrate = iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIBitRateBitRate, KNullDesC8);
	
	// if bitrate is negative then remote side left with an error
	if (bitrate < 0)
		{
		User::Leave(bitrate);
		}
	
	return bitrate;
	}
	
void CMMFDevSoundCIBitRateMux::SetBitRateL(TInt aBitRate)
	{
	if (iRemoteHandle == -1)
		{
		User::Leave(KErrNotReady);
		}
	
	// send the bitrate in the sync command
	TPckgBuf<TInt> bitBuffer(aBitRate);
	
	// any return code other than zero is an error
	User::LeaveIfError(iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIBitRateSetBitRate, bitBuffer));
	}

/////////////////////////////////////// DEMUX //////////////////////	
	

TInt CMMFDevSoundCIBitRateDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}
	
void CMMFDevSoundCIBitRateDeMux::Release()
	{
	TUid key = iKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMFDevSoundCIBitRateDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}
	
void CMMFDevSoundCIBitRateDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}
	
void CMMFDevSoundCIBitRateDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

void CMMFDevSoundCIBitRateDeMux::RefreshL()
	{
	// refetch the bitrate custom interface if we already have a target
	if (iTarget)
		{
		MMMFDevSoundCustomInterfaceBitRate* ptr = NULL;
		ptr = static_cast <MMMFDevSoundCustomInterfaceBitRate*> (iTarget->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
	
		if (!ptr)
			{
			iBitRateInterface = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iBitRateInterface = ptr;
			}	
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDevSoundCIBitRateDeMux::NewL()
	{
	CMMFDevSoundCIBitRateDeMux* self = new (ELeave) CMMFDevSoundCIBitRateDeMux;
	return self;
	}
	
CMMFDevSoundCIBitRateDeMux::CMMFDevSoundCIBitRateDeMux()
	{
	
	}

CMMFDevSoundCIBitRateDeMux::~CMMFDevSoundCIBitRateDeMux()
	{
	iBitRateArray.Reset();
	iBitRateArray.Close();
	}


TInt CMMFDevSoundCIBitRateDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the bitrate custom interface
	MMMFDevSoundCustomInterfaceBitRate* ptr = NULL;
	ptr = static_cast <MMMFDevSoundCustomInterfaceBitRate*> (iTarget->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
	
	if (!ptr)
		{
		iBitRateInterface = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iBitRateInterface = ptr;
		}
	return KErrNone;
	}
	
void CMMFDevSoundCIBitRateDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFDevSoundCIBitRateDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	TInt retVal = -1;
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIBitRateBitRate:
			{
			retVal = DoBitRateL();
			break;
			}
		case EMMFDevSoundCIBitRateSetBitRate:
			{
			// we know that offset 2 contains a TInt
			TPckgBuf<TInt> bitBuffer;
			iUtility->ReadFromInputDesL(aMessage, &bitBuffer);
			
			DoSetBitRateL(bitBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIBitRateGetSupportedBitRates:
			{
			// reset the current bitrate array
			iBitRateArray.Reset();
			DoGetSupportedBitRatesL(iBitRateArray);
			
			// send back the array count
			TInt count = iBitRateArray.Count();
			retVal = count;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
		
	return retVal;
	}
	
TInt CMMFDevSoundCIBitRateDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIBitRateGetSupportedBitRatesArray:
			{
			DoCopyBitRateBufferToClientL(aMessage);		
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return KErrNone;
	}
	
void CMMFDevSoundCIBitRateDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
void CMMFDevSoundCIBitRateDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

// bitrate custom interface implementation
void CMMFDevSoundCIBitRateDeMux::DoGetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates)
	{
	if (!iBitRateInterface)
		{
		User::Leave(KErrNotReady);
		}
		
	iBitRateInterface->GetSupportedBitRatesL(aSupportedBitRates);
	}


void CMMFDevSoundCIBitRateDeMux::DoCopyBitRateBufferToClientL(const RMmfIpcMessage& aMessage)
	{
	if (!iBitRateInterface)
		{
		User::Leave(KErrNotReady);
		}
		
	// check our count is the same as the client's
	TPckgBuf<TInt> countBuffer;
	iUtility->ReadFromInputDesL(aMessage, &countBuffer);

	TInt count = countBuffer();
	if (count != iBitRateArray.Count())
		{
		User::Leave(KErrCorrupt);
		}
			
	// send back the array - the client has the count already
	const TInt KBufExpandSize8 = 8; //two TInt's
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	for (TInt i = 0; i < count; i++)
		{
		stream.WriteInt32L(iBitRateArray[i]);
		}
	
	// write the data to the supplied descriptor buffer
	TPtr8 ptrBuf = dataCopyBuffer->Ptr(0);
	iUtility->WriteToOutputDesL(aMessage, ptrBuf);
	stream.Close();

	CleanupStack::PopAndDestroy(2); // iDataCopyBuffer, stream
	}


TInt CMMFDevSoundCIBitRateDeMux::DoBitRateL()
	{
	if (!iBitRateInterface)
		{
		User::Leave(KErrNotReady);
		}
	return iBitRateInterface->BitRateL();
	}

void CMMFDevSoundCIBitRateDeMux::DoSetBitRateL(TInt aBitRate)
	{
	if (!iBitRateInterface)
		{
		User::Leave(KErrNotReady);
		}
			
	// set the bitrate		
	iBitRateInterface->SetBitRateL(aBitRate);
	}



//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceBitRateMux,	CMMFDevSoundCIBitRateMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceBitRateDeMux,	CMMFDevSoundCIBitRateDeMux::NewL),
	};


//
// ImplementationGroupProxy
//
////////////////////////////////////////////////////////////////////////////////

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


