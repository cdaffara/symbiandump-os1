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

#ifndef CAMERASNAPSHOT_H
#define CAMERASNAPSHOT_H

#include <ecam.h>
#include <gdi.h>
#include <ecamcapturecontrol.h>
#include <ecam/ecamcommonuids.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecam/ecamcommonuidsconst.hrh>
#endif
class MCameraSnapshot;
class MCameraSnapshot2;

class MImplementationFactory;

/** 
Constant used to specify the default display time. When this default display time is specified by the client, the implementation 
may use its own time interval in order to display the direct snapshot data.

@see CCamera::CCameraSnapshot::TSnapshotParameters::iDisplayTime

@publishedPartner
@prototype
*/
static const TInt KECamSnapshotDefaultDisplayTime = 0;

/** 
The current Version of the TSnapshotParameters class.

@publishedPartner
@prototype
*/
static const TUint KECamSnapshotParametersCurrentVersion = 1;

/** 
UID used to identify the event that, the request for snapshot has completed successfully.
A callback to MCameraObserver2::HandleEvent() will occur when the snapshot data is ready to be retrieved.
If a call to StartSnapshot() is made without a successful call to PrepareSnapshotL() then the callback returns KErrNotReady.
*/
static const TUid KUidECamEventCameraSnapshot = {KUidECamEventSnapshotUidValue};

/** 
UID used to identify the CCamera::CCameraSnapshot API.
This is the UID which is used to obtain the CCameraSnapshot interface, 
via a call to CCamera::CustomInterface().

@see KECamSnapshotUidValue
@see CCamera::CCameraSnapshot
*/
static const TUid KECamSnapshotUid = {KECamSnapshotUidValue};

/**
This class allows the client to request snapshot data in a specified image format for both still images and video.

Snapshot object may be created out of new classes: CCameraImageCapture and CCameraVideoCaptureControl. The client selects 
snapshot parameters calling PrepareSnapshotL(const TSnapshotParameters& aSnapshotParameters). It can then enable and 
disable the snapshot generation by calling EnableSnapshotL() and DisableSnapshotL(). The client application using this API 
shall provide MCaptureImageObserver and MCaptureVideoObserver interface. Callbacks available are MCaptureImageObserver::
ClientSnapshotForImageReady and MCaptureVideoObserver::ClientSnapshotReady.

Snapshots are supposed to be displayed on a particular viewfinder. So, while destruction, client shall destroy snapshot 
first. In general, child objects shall be destroyed before the parent objects.

Following is valid for older clients who created snapshots using NewL method:-
The client selects snapshot parameters calling PrepareSnapshotL(). It can then start and stop 
receiving notifications from the ECam implementation by calling StartSnapshot() and StopSnapshot().
The client application using this API should provide MCameraObserver2 interface to be signalled,
with event KUidECamEventCameraSnapshot, when snapshot data is available to be retrieved from 
the ECam implementation.
The client can then retrieve the snapshot data from the ECam implementation calling SnapshotDataL().

@note This class provides a standardised client interface for the camera snapshot. Classes cannot be derived from it.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedPartner
@prototype
*/

class CCamera::CCameraSnapshot: public CBase
	{
	
	friend class CCamera;

	friend CCamera::CCameraSnapshot* CCamera::CCameraImageCapture::GetSnapshotHandleL(TInt aClientViewFinderId) const;
	friend CCamera::CCameraSnapshot* CCamera::CCameraVideoCaptureControl::GetSnapshotHandleL(TInt aClientViewFinderId) const;

public:
	/** 
	Describes the state of snapshot. 
	The enumeration list may be extended in future.
	*/
	enum TSnapshotState
		{
		/** Snapshot has not been prepared. */
		ESnapshotNotCreated,
		/** Snapshot has been prepared but not yet enabled. */
		ESnapshotInactive,
		/** Snapshot has been prepared and enabled but not yet disabled. */
		ESnapshotActive,
		/** Snapshot has been disabled. */
		ESnapshotDisabled,
		};
	
	/** 
	Specifies the video frames to be used in order to create snapshots fo video. 
	The enumeration list may be extended in future.
	*/	
	enum TSnapshotVideoFrames
		{
		/** Snapshot for video created from first video frame. Used by default. */
		ESnapshotVideoFirstFrame,
		/** Snapshot for video created from last video frame. */
		ESnapshotVideoLastFrame,
		/** Snapshot for video created from first and last video frame. */
		ESnapshotVideoFirstAndLastFrame,
		/** Snapshot for video created from every video frame. */
		ESnapshotVideoEveryFrame
		};
		
	/**
	Specifes the parameters necessary for client snapshots.
	*/
	class TSnapshotParameters
		{
	public:
		IMPORT_C TSnapshotParameters();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
				
		IMPORT_C TBool IsAspectRatioMaintained() const;
		
		IMPORT_C void SetAspectRatioState(TBool aIsAspectRatioMaintained);
		
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
		
		TInt iReserved5;
		TInt iReserved6;
		TInt iReserved7;
		
		TUint iReserved4:31;
		
		/** Set to ETrue if the aspect ratio of the snapshot image must be maintained when scaling down */
		TUint iIsAspectRatioMaintained:1;
		
	public:
		/** The image format that the snapshot must have. */
		CCamera::TFormat iFormat;
		/** The top left corner position (in pixels) which determines the layout of the snapshot image  
		within the dimensions provided by the iSnapshotSize parameter when the snapshot has been scaled 
		maintaining its aspect ratio. ECam implementation is supposed to calculate the proportion between
		the top left corner and the snapshot size and hence use it, in case, the snapshot size used is different. */
		TPoint iPosition;
		/** The size of the snapshot in pixels. The snapshot size may be changed by the implementation to meet hardware 
		requirements, performance etc. */
		TSize iSnapshotSize;
		};
	
public:	
	IMPORT_C static CCameraSnapshot* NewL(CCamera& aCamera);
	
    IMPORT_C CCamera::CCameraV2Histogram* CreateHistogramHandleL() const;
    
  	IMPORT_C TUint32 SupportedFormats();
	
	IMPORT_C void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
	IMPORT_C void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio);
	
	IMPORT_C void SetBgColorL(const TRgb& aBgColor); 
	IMPORT_C void SetPositionL(const TPoint& aPosition);
	
	IMPORT_C TBool IsSnapshotActive() const;
	IMPORT_C void StartSnapshot();
	IMPORT_C void StopSnapshot();
	    	
	IMPORT_C MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder);
	
	IMPORT_C ~CCameraSnapshot();
	
	IMPORT_C void PrepareSnapshotL(const TSnapshotParameters& aSnapshotParameters);
	
	IMPORT_C void GetSnapshotParametersL(TSnapshotParameters& aSnapshotParameters);
	IMPORT_C void SetSnapshotParametersL(const TSnapshotParameters& aSnapshotParameters); 
	
	IMPORT_C void GetSnapshotStatusL(TSnapshotState& aSnapshotState) const;
	
	IMPORT_C void SelectSnapshotVideoFramesL(TSnapshotVideoFrames aSnapshotVideoFrames);
	
	IMPORT_C void EnableSnapshotL();
	IMPORT_C void DisableSnapshotL();
	
private:
	IMPORT_C static CCameraSnapshot* CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory, TInt aClientViewFinderId);
	
	CCameraSnapshot(CCamera& aOwner);
	void ConstructL();
	void ConstructL(const MImplementationFactory& aImplFactory, TInt aClientViewFinderId);

private:	
	CCamera&        iOwner;
	MCameraSnapshot*  iImpl;   // not owned
	MCameraSnapshot2* iImpl2;  // not owned
	};
	
#endif // CAMERASNAPSHOT_H
