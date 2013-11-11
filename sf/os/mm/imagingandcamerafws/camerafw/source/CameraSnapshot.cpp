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

/**
 @file
 @publishedPartner
 @prototype
*/

#include <ecam/mcamerasnapshot.h>
#include <ecamimageprocessing.h>
#include <ecam/camerahistogram.h>
#include <ecam/implementationfactoryintf.h>

/**
Factory function that creates a new camera snapshot object on the heap.

@param	aCamera
		A reference to the camera object for which a camera snapshot object is to be created.

@leave  KErrNoMemory if out of memory; also any system wide error.

@return A pointer to the newly created camera snapshot object.

@note  This will be deprecated shortly. Snapshot should be used via CCameraImageCapture::GetSnapshotHandleL() or
	   CCameraVideoCaptureControl::GetSnapshotHandleL().
	   
@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/
EXPORT_C CCamera::CCameraSnapshot* CCamera::CCameraSnapshot::NewL(CCamera& aCamera)
	{
	CCamera::CCameraSnapshot* self = new (ELeave) CCamera::CCameraSnapshot(aCamera);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

/**
@internalComponent

Factory function that creates a new camera snapshot object specifically for image capture or video capture.

@param	aCamera
		A reference to the camera object for which a camera snapshot object is to be created.
		
@param aImplFactory 
	   A reference to the MImplementationFactory derived object.
	   
@param aClientViewFinderId
	   The client viewfinder on which this client snapshot will be displayed.

@leave  KErrNoMemory if out of memory; also any system wide error.

@return A pointer to a fully constructed camera snapshot object.
*/	
EXPORT_C CCamera::CCameraSnapshot* CCamera::CCameraSnapshot::CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory, TInt aClientViewFinderId)
	{
	CCamera::CCameraSnapshot* self = new (ELeave) CCamera::CCameraSnapshot(aCamera);
	CleanupStack::PushL(self);
	self->ConstructL(aImplFactory, aClientViewFinderId);
	CleanupStack::Pop(self);
	return self; 
	}
	
/**
CCameraSnapshot second phase constructor. 

This function is used to initialise internal state of the object. 
It uses reference to the camera to retrieve snapshot interface pointer.

@leave KErrNotSupported if this functionality is not supported; also any system wide error.
*/ 
void CCamera::CCameraSnapshot::ConstructL()
	{
	iImpl = static_cast<MCameraSnapshot*>(iOwner.CustomInterface(KECamMCameraSnapshotUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
	
	iImpl2 = static_cast<MCameraSnapshot2*>(iOwner.CustomInterface(KECamMCameraSnapshot2Uid));
	}
	
/**
@internalComponent

CCameraSnapshot second phase constructor 

Function used to initialise internal state of the object specifically for image capture or video capture. 
This may be used in other possible cases as well.

@param aImplFactory 
	   A constant reference to the MImplementationFactory derived object.
	   
@param aClientViewFinderId
	   The client viewfinder on which this client snapshot will be displayed.

@leave KErrNoMemory Out of memory; or any other error code as well.

@note This method is supposed to be used by this class only.
*/ 
void CCamera::CCameraSnapshot::ConstructL(const MImplementationFactory& aImplFactory, TInt aClientViewFinderId) 
	{
	TInt err = KErrNone;
	TAny* implPtr = NULL;
	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraSnapshotUid);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iImpl = static_cast<MCameraSnapshot*>(implPtr);
	
	implPtr = NULL;	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraSnapshot2Uid);
	if (err != KErrNone && err != KErrNotSupported)
		{
		User::Leave(err);
		}
	iImpl2 = static_cast<MCameraSnapshot2*>(implPtr);
	
	iImpl2->SetClientViewFinderId(aClientViewFinderId);
	}

/**
Constructor for the CCamera::CCameraSnapshot class.

@param 	aOwner
		A reference to the camera object for which a camera snapshot object is to be created.
*/
CCamera::CCameraSnapshot::CCameraSnapshot(CCamera& aOwner):iOwner(aOwner), iImpl(NULL), iImpl2(NULL)
	{
	}
	
/**
@released

Destructor for the CCamera::CCameraSnapshot class.

@note  The child objects(for example,hisotgrams) created out of this snapshot class object shall be 
	   delete beforehand.

*/
EXPORT_C CCamera::CCameraSnapshot::~CCameraSnapshot()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();
		}
	if (iImpl2 != NULL)
		{
		iImpl2->Release();	
		}
	}
	
/**
@released

Gets a list of camera formats for which the ECam implementation supports snapshots.

@return	Bit field containing the supported camera formats as CCamera::TFormat values.
*/
EXPORT_C TUint32 CCamera::CCameraSnapshot::SupportedFormats()
	{
	return iImpl->SupportedFormats();
	}
		
/**
@deprecated Use void PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

Sets the properties of the snapshot data including the background colour and the position of the snapshot.

@param 	aFormat
		The image format that the snapshot must have.
@param 	aPosition
		The top left corner position (in pixels) which determines the layout of the snapshot image  
		within the dimensions provided by the aSize parameter when the snapshot has been scaled 
		maintaining its aspect ratio. See also SetPositionL().
@param 	aSize
		The size of the snapshot in pixels.
@param 	aBgColor
		The background colour to be used if the snapshot has been scaled (maintaining its aspect ratio)
		and does not fully fill the dimension provided by the aSize parameter. See also SetBgColorL().
@param 	aMaintainAspectRatio
		Set to ETrue if the aspect ratio of the snapshot image must be maintained when scaling down.

@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
*/
EXPORT_C void CCamera::CCameraSnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio)
	{
	iImpl->PrepareSnapshotL(aFormat, aPosition, aSize, aBgColor, aMaintainAspectRatio);
	}

/**
@deprecated Use void PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

Sets the properties of the snapshot data, excluding the background colour and the position of the snapshot.
This method can be used when the client wishes to determine the layout and background colour after the
snapshot image has been generated. See also SetPositionL() and SetBgColorL().

@param 	aFormat
		The image format that the snapshot must have.
@param 	aSize
		The size of the snapshot in pixels.
@param 	aMaintainAspectRatio
		Set to ETrue if the aspect ratio of the snapshot image must be maintained when scaling down.

@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
*/
EXPORT_C void CCamera::CCameraSnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio)
	{
	iImpl->PrepareSnapshotL(aFormat, aSize, aMaintainAspectRatio);
	}

/**
@deprecated Use void SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

Sets the background colour to be used if the snapshot has been scaled (maintaining its aspect ratio)
and does not fully fill the snapshot size as specified in PrepareSnapshot().

@param 	aBgColor
		The new background colour.

@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
*/		
EXPORT_C void CCamera::CCameraSnapshot::SetBgColorL(const TRgb& aBgColor)
	{
	iImpl->SetBgColorL(aBgColor);
	}

/**
@deprecated Use void SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

Sets the top left corner position (in pixels), where the snapshot should be laid out after scaling
down (maintaining its aspect ratio). The position is within the dimensions provided by the snapshot 
size specified in PrepareSnapshot().

@param 	aPosition
		The top left corner position in pixels of the snapshot.  		

@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
*/
EXPORT_C void CCamera::CCameraSnapshot::SetPositionL(const TPoint& aPosition)
	{
	iImpl->SetPositionL(aPosition);
	}
	
/**
@deprecated Use void GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const;

Determines if the snapshot feature is active.

@return	ETrue, if StartSnapshot() has been called, otherwise EFalse.
*/
EXPORT_C TBool CCamera::CCameraSnapshot::IsSnapshotActive() const
	{
	return iImpl->IsSnapshotActive();
	}

/**
@deprecated Use void EnableSnapshotL();

Activates the snapshot feature. Calls to this method when the snapshot feature is already active
will be ignored. 

The client will not receive snapshot notifications until the snapshot feature is activated. 

@see KUidECamEventCameraSnapshot
*/
EXPORT_C void CCamera::CCameraSnapshot::StartSnapshot()
	{
	iImpl->StartSnapshot();
	}

/**
@deprecated Use void DisableSnapshotL();

Deactivates the snapshot feature if it is active. 

Once the snapshot has been deactivated, the client will no longer receive notifications about snapshots.
*/
EXPORT_C void CCamera::CCameraSnapshot::StopSnapshot()
	{
	iImpl->StopSnapshot();
	}
    	
/**
@deprecated Use callbacks: MCaptureImageObserver::ClientSnapshotForImageReady and MCaptureVideoObserver::ClientSnapshotReady

Returns the snapshot data from ECam implementation to the client. 

The data is returned in an MCameraBuffer object. In the case where the driving mode returns more 
than one image (burst mode, bracket mode, etc.) the buffer contains several snapshots which may 
be returned in any order. The aFrameIndexOrder array provides the image sequence numbers in the 
order in which the snapshots for those images are returned within the MCameraBuffer.

@note 	The client application using this API should provide MCameraObserver2 interface to be 
		signalled when snapshot data is available to be retrieved from the ECAM implementation.

@see 	KUidECamEventCameraSnapshot

@param 	aFrameIndexOrder
		A reference to an array that will receive the image sequence numbers in the order to which  
		the snapshots within MCameraBuffer relate.

@return	A reference to an MCameraBuffer which will contain the returned snapshot image data.
		
@leave	KErrNoMemory if the ECam implementation has not been able to create the camera buffer;
		also any system wide error.
*/
EXPORT_C MCameraBuffer& CCamera::CCameraSnapshot::SnapshotDataL(RArray<TInt>& aFrameIndexOrder)
	{
	return iImpl->SnapshotDataL(aFrameIndexOrder);
	}

/**
Retrieve pointer to histogram API in order to use it specifically for snapshots.

@return Pointer to use histogram API specifically for the snapshots.
		
@leave  May leave with any error code.

@note   Different types of histogram may be used for a snapshot. Every time this method will be called on the 
		CCameraSnapshot class object, a new type of histogram will be created.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraSnapshot::CreateHistogramHandleL() const
	{
	if(iImpl2 != NULL)
		{
		MImplementationFactory* implFactory = NULL;
		
		iImpl2->CreateHistogramImplFactoryL(implFactory);
		
		CleanupReleasePushL(*implFactory);
		CCamera::CCameraV2Histogram* histogram = CCamera::CCameraV2Histogram::CreateL(iOwner, *implFactory);
		CleanupStack::Pop(implFactory);
		
		implFactory->Release();
		return histogram;	
		}
	else
		{
		return NULL;	
		}
	}

/**
Sets the properties of the snapshot. ECam implementation may use a different size than that specified by this method. 
GetSnapshotParametersL may be used by the client know the actual parameters being used for snapshot creation.

@param  aSnapshotParameters
		The snaspshot parameters.

@leave  May leave with any error code.

@note   This method is used to provide snapshot parameters.
*/
EXPORT_C void CCamera::CCameraSnapshot::PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->PrepareSnapshotL(aSnapshotParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
/**
Retrieves the snapshot parameters used by the ECam implementation for snapshot creation.

@param  aSnapshotParameters
		Retrieves the currently used snapshot parameters.

@leave  May leave with any error code.

@note   This method is used to retrieve snapshot parameters.
*/
EXPORT_C void CCamera::CCameraSnapshot::GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetSnapshotParametersL(aSnapshotParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
/**
Sets/updates the snapshot parameters.

@param  aSnapshotParameters
		The desired snapshot parameters.
		
@leave  May leave with any error code.

@note   This method is used to set/update snapshot parameters.
*/
EXPORT_C void CCamera::CCameraSnapshot::SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters) 
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SetSnapshotParametersL(aSnapshotParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Retrieves the current status for the snapshot.

@param  aSnapshotState
		Retrieves information about the snapshot current state.
	
@leave  May leave with any error code.

@note   This method is used to retrieve the snapshot status.
*/
EXPORT_C void CCamera::CCameraSnapshot::GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetSnapshotStatusL(aSnapshotState);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
The method specifies the frames to be used from video captured data in order to create snapshot for video.

@param  aSnapshotVideoFrames
		A TSnapshotVideoFrames used to specify the desired frames to be used for creating snapshots for video. 
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraSnapshot::SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames aSnapshotVideoFrames)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SelectSnapshotVideoFramesL(aSnapshotVideoFrames);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
/**
Activates the snapshot feature. Calls to this method when the snapshot feature is already active will be ignored. 

The client will not receive snapshot notifications until the snapshot feature is activated. 

Client shall implement the observers: MCaptureImageObserver and MCaptureVideoObserver. Snapshot notifications will be send
to the clients via these observers and not through events.

@leave  KErrNotSupported if the implementation of this method is not present.
*/
EXPORT_C void CCamera::CCameraSnapshot::EnableSnapshotL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->EnableSnapshot();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Deactivates the snapshot feature if it is active. 

Once the snapshot has been deactivated, the client will no longer receive notifications about snapshots.

@leave  KErrNotSupported if the implementation of this method is not present.
*/
EXPORT_C void CCamera::CCameraSnapshot::DisableSnapshotL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->DisableSnapshot();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Constructor for the TSnapshotParameters class.
Sets the size and version of this class.
*/	
EXPORT_C CCamera::CCameraSnapshot::TSnapshotParameters::TSnapshotParameters()
	{
	iSize = sizeof(CCamera::CCameraSnapshot::TSnapshotParameters);
	iVersion = KECamSnapshotParametersCurrentVersion;
	}

/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
corrrectly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraSnapshot::TSnapshotParameters::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraSnapshot::TSnapshotParameters::Version() const
	{
	return iVersion;
	}

/** 
Indicates whether the aspect ratio of the snapshot image has to maintained (if ETrue) or not (if EFalse) 
while scaling down.

@return TBool: ETrue implies aspect ratio has to be maintained, EFalse otherwise.

@see CCamera::CCameraSnapshot::TSnapshotParameters::iIsAspectRatioMaintained
*/
EXPORT_C TBool CCamera::CCameraSnapshot::TSnapshotParameters::IsAspectRatioMaintained() const
	{
	if (iIsAspectRatioMaintained)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** 
Sets the state to inform whether the aspect ratio of the snapshot image has to be maintained or not while scaling
down.

@param  aIsAspectRatioMaintained
		ETrue implies must be maintained, EFalse otherwise.
		
@see CCamera::CCameraSnapshot::TSnapshotParameters::iIsAspectRatioMaintained
*/
EXPORT_C void CCamera::CCameraSnapshot::TSnapshotParameters::SetAspectRatioState(TBool aIsAspectRatioMaintained)
	{
	iIsAspectRatioMaintained = static_cast<TUint>(aIsAspectRatioMaintained);
	}
