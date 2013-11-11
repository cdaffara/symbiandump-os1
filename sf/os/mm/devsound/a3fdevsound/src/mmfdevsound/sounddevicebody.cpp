// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDE FILES
//



#include "sounddevicebody.h"
#include <a3f/mmfdevsoundcustominterfaceextensions.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include <mm/mmpluginutils.h>

/*
 *  Default Constructor.
 */
CMMFDevSound::CBody::CBody()
	{
	}

/*
 *  Destructor.
 *
 *  Deletes all objects and releases all resource owned by this
 *  instance.
 */
CMMFDevSound::CBody::~CBody()
	{
	// Clear the array of custom interfaces
	TInt numIfs = iCustomInterfaceArray.Count();
	for (TInt i = 0; i < numIfs; i++)
		{
		if(iCustomInterfaceArray[i].iInterface)
			{
			iCustomInterfaceArray[i].iInterface->Release();
			}
		}
	iCustomInterfaceArray.Reset();
	iCustomInterfaceArray.Close();

	// Delete the MUX utility
	delete iMuxUtility;

	if (iDevSoundProxy)
		{
		iDevSoundProxy->Close();
		delete iDevSoundProxy;
		}
	
	// Delete the CI extension
	if (iCIExtension)
		{
		iCIExtension->Release();
		}
	}

/*
 *  Constructs, and returns a pointer to, a new CMMFDevSound::CBody object.
 *
 *  Leaves on failure.
 */
CMMFDevSound::CBody* CMMFDevSound::CBody::NewL()
	{
	CBody* self = new (ELeave) CBody;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
 *  Second phase constructor.
 */
void CMMFDevSound::CBody::ConstructL()
	{
	// all these data properties should be NULL, but add ASSERTs to verify.
	ASSERT(!iDevSoundProxy);
	iDevSoundProxy = new (ELeave) RMMFDevSoundProxy();
	TInt err = iDevSoundProxy->Open();
	User::LeaveIfError(err);
	
	User::LeaveIfError(iDevSoundProxy->PostOpen());
	
	// create Custom Interface MUX utility
	iMuxUtility = CMMFDevSoundCIMuxUtility::NewL(this);
	
	// Create Custom Interface extension
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	TUid interfaceUid = {KUidDevSoundCIClientExtension};
	REComSession::ListImplementationsL(interfaceUid, pluginArray);
	TUid destructorKey;
	if (pluginArray.Count() > 0)
		{
		// One or more exists - use the 1st one found
		iCIExtension = static_cast<MDevSoundCIClientExtension*>
						(REComSession::CreateImplementationL(pluginArray[0]->ImplementationUid(), destructorKey));
		}
	CleanupStack::PopAndDestroy(&pluginArray);
	if (iCIExtension)
		{
		// Extension exists. Complete the setup
		iCIExtension->PassDestructorKey(destructorKey);
		User::LeaveIfError(iCIExtension->Setup(*iDevSoundProxy));
		}
	}

/*
 *  CMMFDevSound::InitializeL
 *
 *  Initializes CMMFDevSound object. On completion of Initialization will
 *  call InitializeComplete() on aDevSoundObserver.
 *
 *  Leaves on failure.
 *
 *  @param  MDevSoundObserver&
 *          A reference to the DevSound Observer instance.
 *
 *  @param  TMMFState
 *          A mode for which this object will be used.
 */
void CMMFDevSound::CBody::InitializeL(MDevSoundObserver& aDevSoundObserver,
                                      TMMFState aMode)

	{

	ASSERT(iDevSoundProxy);
	iDevSoundProxy->InitializeL(aDevSoundObserver, aMode, *this);
	}

/*
 *  CMMFDevSound::InitializeL
 *
 *  Initializes CMMFDevSound object with hardware device with hardware
 *  device's FourCC code. On completion of Initialization will call
 *  InitializeComplete() on aDevSoundObserver.
 *
 *  Leaves on failure.
 *
 *  @param  MDevSoundObserver&
 *          A reference to the DevSound Observer instance.
 *
 *  @param  TFourCC
 *          CMMFHwDevice implementation FourCC.
 *
 *  @param  TMMFState
 *          A mode for which this object will be used.
 *
 */
void CMMFDevSound::CBody::InitializeL(MDevSoundObserver& aDevSoundObserver,
                                      TFourCC aDesiredFourCC,
                                      TMMFState aMode)
	{
	ASSERT(iDevSoundProxy);
	iDevSoundProxy->InitializeL(aDevSoundObserver, aDesiredFourCC, aMode, *this);
	}

/**
 *  CMMFDevSound::CBody::CustomInterface
 *
 *  Returns custom interface proxy object created by Proxy Custom Interface
 *  Utility.
 *
 *  @param  TUid
 *          UID of the custom interface object to be started.
 *
 *  @return TAny*
 *          Pointer to the Interface Returned by the DevSoundProxy member.
 */
TAny* CMMFDevSound::CBody::CustomInterface(TUid aInterfaceId)
	{
	// check if this UID refers to CancelInitialize()
	if (aInterfaceId == KMmfUidDevSoundCancelInitializeCustomInterface)
		{
		MMMFDevSoundCancelInitialize* result = this; 
		return result;
		}
	
	// check if this UID refers to EmptyBuffers()
	if (aInterfaceId == KMmfUidDevSoundEmptyBuffersCustomInterface)
		{
		MMMFDevSoundEmptyBuffers* result = this; 
		return result;
		}
		
	if (aInterfaceId == KMmfUidDevSoundAudioResourceCustomInterface)
		{
		MAutoPauseResumeSupport* result = this; 
		return result;
		}		

	if (aInterfaceId == KMmfUidDevSoundTimePlayedCustomInterface)
		{
		MMMFDevSoundTimePlayed* result = this; 
		return result;
		}
	
	if (aInterfaceId == KMmfUidDevSoundQueryIgnoresUnderflowCustomInterface)
		{
		MMMFDevSoundQueryIgnoresUnderflow* result = this;
		return result;
		}
		
	if (aInterfaceId == KMmfUidDevSoundAudioClientThreadInfoCustomInterface)
		{
		MAudioClientThreadInfo* result = this; 
		return result;
		}

	if (aInterfaceId == KMmfUidDevSoundTruePauseCustomInterface)
		{
		MMMFDevSoundTruePause* result = this;
		return result;
		}

	// first check if we already have resolved a custom interface of this type
	TInt index = FindCustomInterface(aInterfaceId);
	
	MMMFDevSoundCustomInterfaceMuxPlugin* ptr = NULL;
		
	// if we found the interface, take a copy of this instead
	if (index != KNullHandle)
		{
		// check our index is valid
		ptr = iCustomInterfaceArray[index-1].iInterface;
		if (ptr)		
			{
			return ptr->CustomInterface(aInterfaceId);
			}
		}
	else
		{
		// else try and instantiate a plugin tunnelling
		// pair to support this Custom Interface
		TRAPD(err, ptr = iMuxUtility->CreateCustomInterfaceMuxL(aInterfaceId));
	
		if (ptr && (err == KErrNone))
			{
			TMMFDevSoundCustomInterfaceData data;
			data.iInterface = ptr;
			data.iId = aInterfaceId;
			
			// attempt to open remote demux
			// this will store a handle in the mux plugin if successful
			// and also return it here - invalid handle = -1
			data.iHandle = ptr->OpenInterface(aInterfaceId);
			
			// if the handle is greater than zero then we know we have
			// successfully opened the interface
			if (data.iHandle > KNullHandle)
				{	
				// append this to the current interface list
				TInt err = KErrNone;
				err = iCustomInterfaceArray.Append(data);
				if (err == KErrNone)
					{
					// return the custom interface on the ptr
					return ptr->CustomInterface(aInterfaceId);
					}
				}
					
				// no memory or other problem so shut down interface
				ptr->Release();
				ptr = NULL;
			}
		}
		
	if (iCIExtension)
		{
		// CI Extension exists, see if it supports the interface
		TAny* interface;
		TInt err = iCIExtension->CustomInterfaceExtension(aInterfaceId, interface);
		if (err == KErrNone)
			{
			return interface;
			}
		}
	// No Mux/DeMux support & no support from CI Extension, if here
	// So, its not supported	
	return NULL;
	}

TInt CMMFDevSound::CBody::FindCustomInterface(TUid aInterfaceId)
	{
	TInt index = KNullHandle;
	TInt count = iCustomInterfaceArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCustomInterfaceArray[i].iId == aInterfaceId)
			{
			index = i+1; // use index+1 as the handle, so 0 is undefined/not-found
			break;
			}
		}
	
	return index;
	}

void CMMFDevSound::CBody::CloseCustomInterface(TInt aIndex)
	{
	TInt count = iCustomInterfaceArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		if(iCustomInterfaceArray[i].iHandle == aIndex)
			{
			iCustomInterfaceArray[i].iInterface->Release();
			iCustomInterfaceArray.Remove(i);
			break;
			}
		}
	}

TInt CMMFDevSound::CBody::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	return iDevSoundProxy->GetTimePlayed(aTime);
	}

TBool CMMFDevSound::CBody::QueryIgnoresUnderflow()
	{
	return ETrue;
	}

TInt CMMFDevSound::CBody::SetClientThreadInfo(TThreadId aTid)
	{
	return iDevSoundProxy->SetClientThreadInfo(aTid);
	}
// End of File
