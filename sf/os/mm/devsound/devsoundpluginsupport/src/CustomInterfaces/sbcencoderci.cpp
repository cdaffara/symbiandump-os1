// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/implementationproxy.h>
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>

#include "sbcencoderci.h"

class TBitpoolRange
	{
public:
	TUint iMin;
	TUint iMax;
	};


// MUX //
/*****************************************************************************/

TInt CMMFSbcEncoderMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceSbcEncoderDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFSbcEncoderMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle > 0)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iDestructorKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}

/*****************************************************************************/	
void CMMFSbcEncoderMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFSbcEncoderMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFSbcEncoderMux::NewL()
	{
	CMMFSbcEncoderMux* self = new (ELeave) CMMFSbcEncoderMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFSbcEncoderMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MSbcEncoderIntfc* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFSbcEncoderMux::CMMFSbcEncoderMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFSbcEncoderMux::~CMMFSbcEncoderMux()
	{	
	}

/*****************************************************************************/
// from MSbcEncoderIntfc
TInt CMMFSbcEncoderMux::GetSupportedBitpoolRange(TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize)
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TBitpoolRange range;
		range.iMax = 0;
		range.iMin = 0;
		TPckgBuf<TBitpoolRange> rangeBuffer(range);
		err = iUtility->SendSlaveSyncCommandResult( iRemoteHandle,
													EMMFDevSoundCISbcEncoderGetSupportedBitpoolRange,
													KNullDesC8,
													rangeBuffer);
		if (err == KErrNone)	
			{
			aMinSupportedBitpoolSize = rangeBuffer().iMin;
			aMaxSupportedBitpoolSize = rangeBuffer().iMax;
			}
		}
	return err;
	}

void CMMFSbcEncoderMux::SetSamplingFrequency(TUint aSamplingFrequency)
	{
	if (iRemoteHandle > 0)
		{
		// send the frequency in the sync command
		TPckgBuf<TUint> freqBuffer(aSamplingFrequency);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetSamplingFrequency,
										freqBuffer);
		}
	}

void CMMFSbcEncoderMux::SetChannelMode (TSbcChannelMode aChannelMode)
	{
	if (iRemoteHandle > 0)
		{
		// send the channel mode in the sync command
		TPckgBuf<MSbcEncoderIntfc::TSbcChannelMode> channelModeBuffer(aChannelMode);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetChannelMode,
										channelModeBuffer);
		}
	}

void CMMFSbcEncoderMux::SetNumOfSubbands ( TUint aNumOfSubbands )
	{
	if (iRemoteHandle > 0)
		{
		// send the number of subbands in the sync command
		TPckgBuf<TUint> numBuffer(aNumOfSubbands);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetSubbands,
										numBuffer);
		}
	}

void CMMFSbcEncoderMux::SetNumOfBlocks ( TUint aNumOfBlocks )
	{
	if (iRemoteHandle > 0)
		{
		// send the number of blocks in the sync command
		TPckgBuf<TUint> blocksBuffer(aNumOfBlocks);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetBlocks,
										blocksBuffer);
		}
	}
	
void CMMFSbcEncoderMux::SetAllocationMethod (TSbcAllocationMethod aAllocationMethod )
	{
	if (iRemoteHandle > 0)
		{
		// send the allocation method in the sync command
		TPckgBuf<TSbcAllocationMethod> allocMethodBuffer(aAllocationMethod);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetAllocationMethod,
										allocMethodBuffer);
		}
	}

void CMMFSbcEncoderMux::SetBitpoolSize (TUint aBitpoolSize )
	{
	if (iRemoteHandle > 0)
		{
		// send the bitpool size in the sync command
		TPckgBuf<TUint> sizeBuffer(aBitpoolSize);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCISbcEncoderSetBitpoolSize,
										sizeBuffer);
		}
	}

TInt CMMFSbcEncoderMux::ApplyConfig()
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		err = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderApplyConfig,
												KNullDesC8);
		}
	return err;
	}

TInt CMMFSbcEncoderMux::GetSamplingFrequency(TUint& aSamplingFrequency)
	{
	TInt err = KErrNotReady;

	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> freqBuffer;		
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetSamplingFrequency, 
													KNullDesC8,
													freqBuffer);
		if (err == KErrNone)
			{
			aSamplingFrequency = freqBuffer();
			}
		}
		
	return err;
	}

TInt CMMFSbcEncoderMux::GetNumOfSubbands (TUint& aNumOfSubbands )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> sizeBuffer;
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetSubbands, 
													KNullDesC8,
													sizeBuffer);
		if (err == KErrNone)
			{
			aNumOfSubbands = sizeBuffer();
			}
		}
	return err;
	}
	
TInt CMMFSbcEncoderMux::GetNumOfBlocks (TUint& aNumOfBlocks )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> blocksBuffer;
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetBlocks, 
													KNullDesC8,
													blocksBuffer);
		if (err == KErrNone)
			{
			aNumOfBlocks = blocksBuffer();
			}
		}
	return err;
	}
	
TInt CMMFSbcEncoderMux::GetAllocationMethod (TSbcAllocationMethod& aAllocationMethod )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TSbcAllocationMethod> allocMethodBuffer;
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetAllocationMethod, 
													KNullDesC8,
													allocMethodBuffer);
		if (err == KErrNone)
			{
			aAllocationMethod = allocMethodBuffer();
			}
		}
	return err;
	}	

TInt CMMFSbcEncoderMux::GetBitpoolSize (TUint& aBitpoolSize )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> sizeBuffer;
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetBitpoolSize, 
													KNullDesC8,
													sizeBuffer);
		if (err == KErrNone)
			{
			aBitpoolSize = sizeBuffer();
			}
		}
	
	return err;	
	}

TInt CMMFSbcEncoderMux::GetChannelMode(TSbcChannelMode& aChannelMode)
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TSbcChannelMode> channelModeBuffer;
		err = iUtility->SendSlaveSyncCommandResult(	iRemoteHandle, 
													EMMFDevSoundCISbcEncoderGetChannelMode, 
													KNullDesC8,
													channelModeBuffer);
		if (err == KErrNone)
			{
			aChannelMode = channelModeBuffer();
			}
		}

	return err;	
	}
	
/********************************************************************************/
TInt CMMFSbcEncoderMux::GetSupportedSamplingFrequencies (RArray<TUint>& aSamplingFrequencies )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// Clear the array
		aSamplingFrequencies.Reset();
		// Fetch the count
		TInt count = -1;
		count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesCount,
												KNullDesC8);
		if (count < 0)
			{
			err = count;
			}
		else if (count > 0)
			{
			TRAP(err, DoGetTUintArrayL( aSamplingFrequencies,
										count,
										EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesArray));
			}
		else
			{
			// count == 0, nothing to do and no error...
			err = KErrNone;
			}
		}
	return err;
	}

/********************************************************************************/
TInt CMMFSbcEncoderMux::GetSupportedChannelModes (RArray<TSbcChannelMode>& aChannelModes )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// Clear the array
		aChannelModes.Reset();
		// Fetch the count
		TInt count = -1;
		count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderGetSupportedChannelModesCount,
												KNullDesC8);
		if (count < 0)
			{
			err = count;
			}
		else if (count > 0)
			{
			TRAP(err, DoGetChannelModesArrayL(aChannelModes, count));
			}
		else
			{
			// count == 0, nothing to do and no error...
			err = KErrNone;
			}
		}	
	return err;
	}

void CMMFSbcEncoderMux::DoGetChannelModesArrayL(RArray<TSbcChannelMode>& aChannelModes, TInt aCount)
	{
	// allocate a temporary buffer to hold the channel modes
	HBufC8* buf = HBufC8::NewLC(aCount * sizeof(TSbcChannelMode));
	TPtr8 ptr = buf->Des();

	// fetch the channel modes - but send over the received count to be sure
	TPckgBuf<TInt> countBuf(aCount);
	User::LeaveIfError(iUtility->SendSlaveSyncCommandResult(iRemoteHandle, 
															EMMFDevSoundCISbcEncoderGetSupportedChannelModesArray,
															countBuf, ptr));

	// stream data into the pointer
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);
	TInt err = KErrNone;
	TSbcChannelMode mode;
	for (TInt i = 0; i < aCount; i++)
		{
		// note we don't destroy array because we don't own it
		// but we do reset it as it is incomplete
		mode = static_cast<TSbcChannelMode>(stream.ReadInt32L());
		err = aChannelModes.Append(mode);
		if (err != KErrNone)
			{
			aChannelModes.Reset();
			User::Leave(KErrCorrupt);
			}
		}
	
	CleanupStack::PopAndDestroy(2, buf);// stream, buf
	}

/********************************************************************************/
TInt CMMFSbcEncoderMux::GetSupportedNumOfSubbands (RArray<TUint>& aNumOfSubbands )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// Clear the array
		aNumOfSubbands.Reset();
		// Fetch the count
		TInt count = -1;
		count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderGetSupportedSubbandsCount,
												KNullDesC8);
		if (count < 0)
			{
			err = count;
			}
		else if (count > 0)
			{
			TRAP(err, DoGetTUintArrayL(aNumOfSubbands, count, EMMFDevSoundCISbcEncoderGetSupportedSubbandsArray));
			}
		else
			{
			// count == 0, nothing to do and no error...
			err = KErrNone;
			}
		}
	
	return err;	
	}
	
/********************************************************************************/	
TInt CMMFSbcEncoderMux::GetSupportedAllocationMethods (RArray<TSbcAllocationMethod>& aAllocationMethods )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// Clear the array
		aAllocationMethods.Reset();
		// Fetch the count
		TInt count = -1;
		count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsCount,
												KNullDesC8);
		if (count < 0)
			{
			err = count;
			}
		else if (count > 0)
			{
			TRAP(err, DoGetAllocMethodsArrayL(aAllocationMethods, count));
			}
		else
			{
			// count == 0, nothing to do and no error...
			err = KErrNone;
			}
		}

	return err;
	}

void CMMFSbcEncoderMux::DoGetAllocMethodsArrayL(RArray<TSbcAllocationMethod>& aAllocationMethods, TInt aCount)
	{
	// allocate a temporary buffer to hold the allocation methods
	HBufC8* buf = HBufC8::NewLC(aCount * sizeof(TSbcAllocationMethod));
	TPtr8 ptr = buf->Des();

	// fetch the allocation methods - but send over the received count to be sure
	TPckgBuf<TInt> countBuf(aCount);
	User::LeaveIfError(iUtility->SendSlaveSyncCommandResult(iRemoteHandle, 
															EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsArray,
															countBuf, ptr));
	// stream data into the pointer
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);
	TInt err = KErrNone;
	TSbcAllocationMethod mode;
	for (TInt i = 0; i < aCount; i++)
		{
		// note we don't destroy array because we don't own it
		// but we do reset it as it is incomplete
		mode = static_cast<TSbcAllocationMethod>(stream.ReadInt32L());
		err = aAllocationMethods.Append(mode);
		if (err != KErrNone)
			{
			aAllocationMethods.Reset();
			User::Leave(KErrCorrupt);
			}
		}
	
	CleanupStack::PopAndDestroy(2, buf);// stream, buf	
	}
	
/********************************************************************************/
TInt CMMFSbcEncoderMux::GetSupportedNumOfBlocks (RArray<TUint>& aNumOfBlocks )
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// Clear the array
		aNumOfBlocks.Reset();
		// Fetch the count
		TInt count = -1;
		count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISbcEncoderGetSupportedBlocksCount,
												KNullDesC8);
		if (count < 0)
			{
			err = count;
			}
		else if (count > 0)
			{
			TRAP(err, DoGetTUintArrayL(aNumOfBlocks, count, EMMFDevSoundCISbcEncoderGetSupportedBlocksArray));
			}
		else
			{
			// count == 0, nothing to do and no error...
			err = KErrNone;
			}
		}

	return err;
	}

void CMMFSbcEncoderMux::DoGetTUintArrayL(RArray<TUint>& aArray,
										 TInt aCount,
										 TMMFDevSoundCISbcEncoderCommands aCommand)
	{
	// allocate a temporary buffer to hold the number of blocks
	HBufC8* buf = HBufC8::NewLC(aCount * sizeof(TInt32));
	TPtr8 ptr = buf->Des();

	// fetch the array data for the given command - but send over the received count to be sure
	TPckgBuf<TInt> countBuf(aCount);
	User::LeaveIfError(iUtility->SendSlaveSyncCommandResult(iRemoteHandle, 
															aCommand,
															countBuf, ptr));

	// stream data into the pointer
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);
	TInt err = KErrNone;
	for (TInt i = 0; i < aCount; i++)
		{
		err = aArray.Append(stream.ReadUint32L());
		if (err != KErrNone)
			{
			// note we don't destroy array because we don't own it
			// but we do reset it as it is incomplete
			aArray.Reset();
			User::Leave(KErrCorrupt);
			}
		}
	
	CleanupStack::PopAndDestroy(2, buf);// stream, buf		
	}

	
// DEMUX //	
/*****************************************************************************/
TInt CMMFSbcEncoderDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFSbcEncoderDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFSbcEncoderDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFSbcEncoderDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFSbcEncoderDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFSbcEncoderDeMux::RefreshL()
	{
	// refetch the SBC encoder custom interface if we already have a target
	if (iTarget)
		{
		MSbcEncoderIntfc* ptr = NULL;

		ptr = static_cast <MSbcEncoderIntfc*> (iTarget->CustomInterface(KUidSbcEncoderIntfc));
	
		if (!ptr)
			{
			iInterfaceSbcEncoder = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iInterfaceSbcEncoder = ptr;
			}	
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFSbcEncoderDeMux::NewL()
	{
	CMMFSbcEncoderDeMux* self = new (ELeave) CMMFSbcEncoderDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFSbcEncoderDeMux::CMMFSbcEncoderDeMux()
	{	
	}

/*****************************************************************************/
CMMFSbcEncoderDeMux::~CMMFSbcEncoderDeMux()
	{
	// Clear up all the arrays.
	iSamplingFrequencies.Reset();
	iSamplingFrequencies.Close();
	iChannelModes.Reset();
	iChannelModes.Close();
	iNumOfSubbands.Reset();
	iNumOfSubbands.Close(); 
	iAllocationMethods.Reset(); 
	iAllocationMethods.Close();
	iNumOfBlocks.Reset();
	iNumOfBlocks.Close();
	}

/*****************************************************************************/
TInt CMMFSbcEncoderDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the SBD encoder Hw Device custom interface
	MSbcEncoderIntfc* ptr = NULL;

	ptr = static_cast<MSbcEncoderIntfc*> (iTarget->CustomInterface(KUidSbcEncoderIntfc)); 
	
	if (!ptr)
		{
		iInterfaceSbcEncoder = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfaceSbcEncoder = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFSbcEncoderDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFSbcEncoderDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	TInt retVal = KErrNone;
	switch (data.iCommand)
		{
		case EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesCount:
			{
			iSamplingFrequencies.Reset();
			User::LeaveIfError(DoGetSupportedSamplingFrequencies(iSamplingFrequencies));
			retVal = iSamplingFrequencies.Count();
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedBlocksCount:
			{
			iNumOfBlocks.Reset();
			User::LeaveIfError(DoGetSupportedNumOfBlocks(iNumOfBlocks));
			retVal = iNumOfBlocks.Count();
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedSubbandsCount:
			{
			iNumOfSubbands.Reset();
			User::LeaveIfError(DoGetSupportedNumOfSubbands(iNumOfSubbands));
			retVal = iNumOfSubbands.Count();
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedChannelModesCount:
			{
			iChannelModes.Reset();
			User::LeaveIfError(DoGetSupportedChannelModes(iChannelModes));
			retVal = iChannelModes.Count();
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsCount:
			{
			iAllocationMethods.Reset();
			User::LeaveIfError(DoGetSupportedAllocationMethods(iAllocationMethods));
			retVal = iAllocationMethods.Count();
			break;
			}		
		case EMMFDevSoundCISbcEncoderSetSamplingFrequency:
			{
			TPckgBuf<TUint> freqBuffer;
			iUtility->ReadFromInputDesL(aMessage, &freqBuffer);
			DoSetSamplingFrequency(freqBuffer());
			break;
			}
		case EMMFDevSoundCISbcEncoderSetChannelMode:
			{
			TPckgBuf<MSbcEncoderIntfc::TSbcChannelMode> channelBuffer;
			iUtility->ReadFromInputDesL(aMessage, &channelBuffer);
			DoSetChannelMode(channelBuffer());
			break;
			}
		case EMMFDevSoundCISbcEncoderSetSubbands:
			{
			TPckgBuf<TUint> valueBuffer;
			iUtility->ReadFromInputDesL(aMessage, &valueBuffer);
			DoSetNumOfSubbands(valueBuffer());
			break;
			}
		case EMMFDevSoundCISbcEncoderSetBlocks:
			{
			TPckgBuf<TUint> valueBuffer;
			iUtility->ReadFromInputDesL(aMessage, &valueBuffer);
			DoSetNumOfBlocks(valueBuffer());
			break;
			}
		case EMMFDevSoundCISbcEncoderSetBitpoolSize:
			{
			TPckgBuf<TUint> valueBuffer;
			iUtility->ReadFromInputDesL(aMessage, &valueBuffer);
			DoSetBitpoolSize(valueBuffer());
			break;
			}
		case EMMFDevSoundCISbcEncoderSetAllocationMethod:
			{
			TPckgBuf<MSbcEncoderIntfc::TSbcAllocationMethod> allocationMethodBuffer;
			iUtility->ReadFromInputDesL(aMessage, &allocationMethodBuffer);
			DoSetAllocationMethod(allocationMethodBuffer());
			break;
			}			
		case EMMFDevSoundCISbcEncoderApplyConfig:
			{
			retVal = DoApplyConfig();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}		
		};
		
	return retVal;
	}
	
/*****************************************************************************/	
TInt CMMFSbcEncoderDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt retVal = KErrNone;
	switch (data.iCommand)
		{
		case EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesArray:
			{
			// The array will already have been populated by the time this is called
			// but we can check that the count passed in matches that of the array
			DoWriteArrayToClientL(aMessage, iSamplingFrequencies);
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedSubbandsArray:
			{
			// The array will already have been populated by the time this is called
			// but we can check that the count passed in matches that of the array
			DoWriteArrayToClientL(aMessage, iNumOfSubbands);
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedBlocksArray:
			{
			// The array will already have been populated by the time this is called
			// but we can check that the count passed in matches that of the array
			DoWriteArrayToClientL(aMessage, iNumOfBlocks);
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedChannelModesArray:
			{
			// The array will already have been populated by the time this is called
			// but we can check that the count passed in matches that of the array
			// Pass ETrue to write out the channel modes array
			DoWriteArrayToClientL(aMessage, ETrue);
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsArray:
			{
			// The array will already have been populated by the time this is called
			// but we can check that the count passed in matches that of the array		
			// Pass EFalse for the alloc method array.
			DoWriteArrayToClientL(aMessage, EFalse);
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSupportedBitpoolRange:
			{
			TPckgBuf<TBitpoolRange> rangeBuf;
			DoGetSupportedBitpoolRange(rangeBuf().iMin, rangeBuf().iMax);
			iUtility->WriteToOutputDesL(aMessage, rangeBuf);	
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSamplingFrequency:
			{
			TPckgBuf<TUint> valueBuf;
			DoGetSamplingFrequency(valueBuf());
			iUtility->WriteToOutputDesL(aMessage, valueBuf);		
			break;
			}
		case EMMFDevSoundCISbcEncoderGetChannelMode:
			{
			TPckgBuf<MSbcEncoderIntfc::TSbcChannelMode> channelBuf;
			DoGetChannelMode(channelBuf());
			iUtility->WriteToOutputDesL(aMessage, channelBuf);		
			break;
			}
		case EMMFDevSoundCISbcEncoderGetSubbands:
			{
			TPckgBuf<TUint> valueBuf;
			DoGetNumOfSubbands(valueBuf());
			iUtility->WriteToOutputDesL(aMessage, valueBuf);		
			break;
			}
		case EMMFDevSoundCISbcEncoderGetBlocks:
			{
			TPckgBuf<TUint> valueBuf;
			DoGetNumOfBlocks(valueBuf());
			iUtility->WriteToOutputDesL(aMessage, valueBuf);		
			break;
			}
		case EMMFDevSoundCISbcEncoderGetAllocationMethod:
			{
			TPckgBuf<MSbcEncoderIntfc::TSbcAllocationMethod> allocationMethodBuf;
			DoGetAllocationMethod(allocationMethodBuf());
			iUtility->WriteToOutputDesL(aMessage, allocationMethodBuf);		
			break;
			}
		case EMMFDevSoundCISbcEncoderGetBitpoolSize:
			{
			TPckgBuf<TUint> valueBuf;
			DoGetBitpoolSize(valueBuf());
			iUtility->WriteToOutputDesL(aMessage, valueBuf);		
			break;
			}			
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return retVal;
	}

/*****************************************************************************/	
void CMMFSbcEncoderDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFSbcEncoderDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// SBC Encoder custom interface implementation
TInt CMMFSbcEncoderDeMux::DoGetSupportedSamplingFrequencies(RArray<TUint>& aSamplingFrequencies)
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedSamplingFrequencies(aSamplingFrequencies);
		}
	return err;
	}

TInt CMMFSbcEncoderDeMux::DoGetSupportedNumOfSubbands (RArray<TUint>& aNumOfSubbands )
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedNumOfSubbands(aNumOfSubbands);
		}
	return err;
	}

TInt CMMFSbcEncoderDeMux::DoGetSupportedNumOfBlocks (RArray<TUint>& aNumOfBlocks )
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedNumOfBlocks(aNumOfBlocks);
		}
	return err;
	}

TInt CMMFSbcEncoderDeMux::DoGetSupportedChannelModes (RArray<MSbcEncoderIntfc::TSbcChannelMode>& aChannelModes )
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedChannelModes(aChannelModes);
		}
	return err;
	}
	
TInt CMMFSbcEncoderDeMux::DoGetSupportedAllocationMethods (RArray<MSbcEncoderIntfc::TSbcAllocationMethod>& aAllocationMethods )
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedAllocationMethods(aAllocationMethods);
		}
	return err;
	}

TInt CMMFSbcEncoderDeMux::DoGetSupportedBitpoolRange (TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize)
	{
	TInt err = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		err = iInterfaceSbcEncoder->GetSupportedBitpoolRange(aMinSupportedBitpoolSize, aMaxSupportedBitpoolSize);
		}
	return err;	
	}
	
void CMMFSbcEncoderDeMux::DoSetSamplingFrequency (TUint aSamplingFrequency )
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetSamplingFrequency(aSamplingFrequency);
		}		
	}
	
void CMMFSbcEncoderDeMux::DoSetChannelMode (MSbcEncoderIntfc::TSbcChannelMode aChannelMode )
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetChannelMode(aChannelMode);
		}		
	}

void CMMFSbcEncoderDeMux::DoSetNumOfSubbands (TUint aNumOfSubbands)
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetNumOfSubbands(aNumOfSubbands);
		}		
	}

void CMMFSbcEncoderDeMux::DoSetNumOfBlocks (TUint aNumOfBlocks)
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetNumOfBlocks(aNumOfBlocks);
		}		
	}

void CMMFSbcEncoderDeMux::DoSetAllocationMethod (MSbcEncoderIntfc::TSbcAllocationMethod aAllocationMethod)
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetAllocationMethod(aAllocationMethod);
		}		
	}

void CMMFSbcEncoderDeMux::DoSetBitpoolSize (TUint aBitpoolSize)
	{
	if (iInterfaceSbcEncoder)
		{
		iInterfaceSbcEncoder->SetBitpoolSize(aBitpoolSize);
		}		
	}

TInt CMMFSbcEncoderDeMux::DoApplyConfig()
	{
	TInt retVal = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		retVal = iInterfaceSbcEncoder->ApplyConfig();
		}
	return retVal;
	}

TInt CMMFSbcEncoderDeMux::DoGetSamplingFrequency(TUint& aSamplingFrequency)
	{
	TInt ret = KErrNotReady;
	if (iInterfaceSbcEncoder)
		{
		ret = iInterfaceSbcEncoder->GetSamplingFrequency(aSamplingFrequency);
		}
	return ret;
	}

TInt CMMFSbcEncoderDeMux::DoGetChannelMode (MSbcEncoderIntfc::TSbcChannelMode& aChannelMode )
	{
	if (!iInterfaceSbcEncoder)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceSbcEncoder->GetChannelMode(aChannelMode);
		}
	}

TInt CMMFSbcEncoderDeMux::DoGetNumOfSubbands (TUint& aNumOfSubbands )
	{
	if (!iInterfaceSbcEncoder)
		{
		return KErrNotReady;  
		}
	else
		{
		return iInterfaceSbcEncoder->GetNumOfSubbands(aNumOfSubbands);
		}
	}

TInt CMMFSbcEncoderDeMux::DoGetNumOfBlocks (TUint& aNumOfBlocks )
	{
	if (!iInterfaceSbcEncoder)
		{
		return KErrNotReady;  
		}
	else
		{
		return iInterfaceSbcEncoder->GetNumOfBlocks(aNumOfBlocks);
		}		
	}

TInt CMMFSbcEncoderDeMux::DoGetAllocationMethod (MSbcEncoderIntfc::TSbcAllocationMethod& aAllocationMethod )
	{
	if (!iInterfaceSbcEncoder)
		{
		return KErrNotReady;  
		}
	else
		{
		return iInterfaceSbcEncoder->GetAllocationMethod(aAllocationMethod);
		}		
	}

TInt CMMFSbcEncoderDeMux::DoGetBitpoolSize(TUint& aBitpoolSize)
	{
	if (!iInterfaceSbcEncoder)
		{
		return KErrNotReady;  
		}
	else
		{
		return iInterfaceSbcEncoder->GetBitpoolSize(aBitpoolSize);
		}		
	}

// This is a utility method used by each of the TUint parametered methods to write their arrays
// back to the client (using aMessage)
void CMMFSbcEncoderDeMux::DoWriteArrayToClientL(const RMmfIpcMessage& aMessage, RArray<TUint>& aArray)
	{
	// The message already contains the array count so retrieve it
	// and verify that nothing's awry.
	TPckgBuf<TInt> countBuf;
	iUtility->ReadFromInputDesL(aMessage, &countBuf);
	TInt count = countBuf();
	if (count != aArray.Count())
		{
		User::Leave(KErrCorrupt);
		}
	// Create a suitably sized buffer
	HBufC8* buf = HBufC8::NewLC(count * sizeof(TUint));
	TPtr8 ptr = buf->Des();
	RDesWriteStream stream(ptr);
	CleanupClosePushL(stream);
	// Stream the array data
	for (TInt i = 0; i < count; i++)
		{
		stream.WriteUint32L(aArray[i]);
		}
	// Commit the data to the stream
	stream.CommitL();
	// Write the buffer back to the mux
	iUtility->WriteToOutputDesL(aMessage, *buf);
	CleanupStack::PopAndDestroy(2, buf); // stream, buf
	}

void CMMFSbcEncoderDeMux::DoWriteArrayToClientL(const RMmfIpcMessage& aMessage, TBool aWriteChannelModeArray)
	{
	// The message already contains the array count so retrieve it
	// and verify that nothing's awry.
	TPckgBuf<TInt> countBuf;
	iUtility->ReadFromInputDesL(aMessage, &countBuf);
	TInt count = countBuf();
	TInt arrayCount = 0;
		
	if (aWriteChannelModeArray)
		{
		arrayCount = iChannelModes.Count();
		}
	else
		{
		arrayCount = iAllocationMethods.Count();
		}
		
	if (count != arrayCount)
		{
		User::Leave(KErrCorrupt);
		}
		
	// Create a suitably sized buffer
	HBufC8* buf = HBufC8::NewLC(count * sizeof(TUint));
	TPtr8 ptr = buf->Des();
	RDesWriteStream stream(ptr);
	CleanupClosePushL(stream);
	// Stream the array data
	if (aWriteChannelModeArray)
		{
		for (TInt i = 0; i < count; i++)
			{
			stream.WriteUint32L(iChannelModes[i]);
			}
		
		}
	else
		{
		for (TInt i = 0; i < count; i++)
			{
			stream.WriteUint32L(iAllocationMethods[i]);
			}
		}
	
	// Commit the data to the stream
	stream.CommitL();
	// Write the buffer back to the mux
	iUtility->WriteToOutputDesL(aMessage, *buf);
	CleanupStack::PopAndDestroy(2, buf); // stream, buf
	}

/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSbcEncoderMux, CMMFSbcEncoderMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSbcEncoderDeMux, CMMFSbcEncoderDeMux::NewL),
	};

/*****************************************************************************/
//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
