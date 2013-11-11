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

#ifndef MCAMERASNAPSHOT_H
#define MCAMERASNAPSHOT_H

#include <ecam/camerasnapshot.h> 
#include <ecam/ecamcommonuidif.hrh>

class TRgb;

/** 
This is the UID which is used to obtain the MCameraSnapshot interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface. 

@see KECamMCameraSnapshotUidValue
*/   
static const TUid KECamMCameraSnapshotUid = {KECamMCameraSnapshotUidValue};

/** 
This is the UID which is used to obtain the MCameraSnapshot2 interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface. 

@see KECamMCameraSnapshot2UidValue
*/   
static const TUid KECamMCameraSnapshot2Uid = {KECamMCameraSnapshot2UidValue};

/**
This is a mixin class to be implemented by providers of the extension API for the camera snapshot.

@note This class is intended for sub classing by licensees only.

@see CCamera::CCameraSnapshot

@publishedPartner
@prototype
*/

class MCameraSnapshot
	{
	
public:	
	/**
	@publishedPartner
	@released
	
	Gets a list of camera formats for which the ECam implementation supports snapshots.

	@return	Bit field containing the supported camera formats as CCamera::TFormat values.
	*/
	virtual TUint32 SupportedFormats()=0;
			
	/**
	@deprecated Use void MCameraSnapshot2::PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

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
    virtual void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio)=0;
    
	/**
	@deprecated Use void MCameraSnapshot2::PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

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
	virtual void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio)=0;
	
	/**
	@deprecated Use void MCameraSnapshot2::SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

	Sets the background colour to be used if the snapshot has been scaled (maintaining its aspect ratio)
	and does not fully fill the snapshot size as specified in PrepareSnapshot().

	@param 	aBgColor
			The new background colour.

	@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
	*/		
	virtual void SetBgColorL(const TRgb& aBgColor)=0; 
	
	/**
	@deprecated Use void MCameraSnapshot2::SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);

	Sets the top left corner position (in pixels), where the snapshot should be laid out after scaling
	down (maintaining its aspect ratio). The position is within the dimensions provided by the snapshot 
	size specified in PrepareSnapshot().
	
	@param 	aPosition
			The top left corner position in pixels of the snapshot.  		
	
	@leave  KErrNotSupported if the specified image format is not supported; also any system wide error.
	*/
	virtual void SetPositionL(const TPoint& aPosition)=0;
	
	/**
	@deprecated Use void MCameraSnapshot2::GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const;

	Determines if the snapshot feature is active.

	@return	ETrue, if StartSnapshot() has been called, otherwise EFalse.
	*/
    virtual TBool IsSnapshotActive() const=0;

	/**
	@deprecated Use void MCameraSnapshot2::EnableSnapshotL();

	Activates the snapshot feature. Calls to this method when the snapshot feature is already active
	will be ignored. 

	The client will not receive snapshot notifications until the snapshot feature is activated. 
	*/
    virtual void StartSnapshot()=0;

	/**
	@deprecated Use void MCameraSnapshot2::DisableSnapshotL();
	
	Deactivates the snapshot feature if it is active. 

	Once the snapshot has been deactivated, the client will no longer receive notifications about snapshots.
	*/
	virtual void StopSnapshot()=0;
        	
	/**
	@deprecated Use callbacks: MCaptureImageObserver::ClientSnapshotForImageReady and MCaptureVideoObserver::
				ClientSnapshotReady
	
	Returns the snapshot data from ECam implementation to the client. 

	The data is returned in an MCameraBuffer object. In the case where the driving mode returns more 
	than one image (burst mode, bracket mode, etc.) the buffer contains several snapshots which may 
	be returned in any order. The aFrameIndexOrder array provides the image sequence numbers in the 
	order in which the snapshots for those images are returned within the MCameraBuffer.

	@param 	aFrameIndexOrder
			A reference to an array that will receive the image sequence numbers in the order to which  
			the snapshots within MCameraBuffer relate.

	@return	A reference to an MCameraBuffer which will contain the returned snapshot image data.
		
	@leave	KErrNoMemory if the ECam implementation has not been able to create the camera buffer;
			also any system wide error.
	*/
	virtual MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder)=0;

	/**
	Destroys the object of this class on the ECam implementation. 
	This function is called from the destructor of CCamera::CCameraSnapshot.
	*/
	virtual void Release()=0;
	    
	};
	
/**
This is a mixin class to be implemented by providers of the extension API for the extra features for camera snapshot.

@see CCamera::CCameraSnapshot

@publishedPartner
@prototype
*/

class MCameraSnapshot2
	{
	
public:	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Sets the client viewfinder on which the client snapshot will be displayed.
	
	@param aClientViewFinderId
		   The client viewfinder on which this client snapshot will be displayed.
	*/
	virtual void SetClientViewFinderId(TInt aClientViewFinderId)=0;
	
	/**
	Retrieves the concrete factory handle for the histogram implementation in order to 
	use it specifically for a given snapshot.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the histogram implementation specific to the given snapshot.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	Sets the properties of the snapshot. ECam implementation may use a different size than that specified by this method. 
	
	@param  aSnapshotParameters
			The snaspshot parameters.
	
	@leave  May leave with any error code.
	
	@note   This method is used to provide snapshot parameters.
	*/
	virtual void PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)=0;
	
	/**
	Retrieves the snapshot parameters. ECam implementation may use a different size than that provided by this method.
	
	@param  aSnapshotParameters
			Retrieves the currently used snapshot parameters.
	
	@leave  May leave with any error code.
	
	@note   This method is used to retrieve snapshot parameters.
	*/
	virtual void GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)=0;
	
	/**
	Sets/updates the snapshot parameters.
	
	@param  aSnapshotParameters
			The desired snapshot parameters.
			
	@leave  May leave with any error code.
	
	@note   This method is used to set/update snapshot parameters.
	*/
	virtual void SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)=0; 

	/**
	Retrieves the current status for the snapshot.
	
	@param  aSnapshotState
			Retrieves information about the current snapshot state.
		
	@leave  May leave with any error code.

	@note   This method is used to retrieve the snapshot status.
	*/
	virtual void GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const=0;
	
	/**
	The method specifies the frames to be used from video captured data in order to create snapshot for video.

	@param  aSnapshotVideoFrames
			A TSnapshotVideoFrames used to specify the desired frames to be used for creating snapshots for video. 
			
	@leave  May leave with any error code.
	*/
	virtual void SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames aSnapshotVideoFrames)=0;
	
	/**
	Activates the snapshot feature. Calls to this method when the snapshot feature is already active will be ignored. 

	The client will not receive snapshot notifications until the snapshot feature is activated. 

	Implementation shall use the observers: MCaptureImageObserver and MCaptureVideoObserver. Snapshot notifications will 
	be send	to the clients via these observers and not through events.
	*/
	virtual void EnableSnapshot()=0;
	
	/**
	Deactivates the snapshot feature if it is active. 

	Once the snapshot has been deactivated, the client will no longer receive notifications about snapshots.
	*/
	virtual void DisableSnapshot()=0;
	};
	
#endif // MCAMERASNAPSHOT_H
