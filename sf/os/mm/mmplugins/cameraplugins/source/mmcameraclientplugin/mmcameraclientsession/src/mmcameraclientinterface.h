// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef MMCAMERACLIENTINTERFACE_H
#define MMCAMERACLIENTINTERFACE_H

#include <e32base.h>
#include <e32std.h>
#include <ecamuids.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamuidsdef.hrh>
#endif
#include <ecamviewfinder.h>
#include <graphics/surface.h>

static const TInt KECamMaxMessage = 100;
static const TUid KECamServerUid3 = {KUidMMCameraServer};
static const TInt KECamNoPriority = -101;
static const TInt KECamHandleNotKnown = -1;
static const TInt KECamNumAsynchMsg = 8;


_LIT(KMMCameraServerName, "!MMCameraServer");
_LIT(KMMCameraServerExe, "MMCameraServer");  // Exe name

enum TECamMsgId // Enums for use in ServiceL()
	{    
	ECamQueryCamerasAvailable = 0,	// Query MM server to retrieve number of physical cameras available
	ECamCameraAccessControl,    	// Enums to reserve/release the camera for exclusive use. TBool ETrue/EFalse decides Reserve/Release . 
	ECamPowerCamera,      			// To Power On the camera if TBool passed =ETrue, else power down 
  	ECamOpenCamera,        			// Called from ConnectSession to open a particular camera specified by the index 
	ECamHandleCamera,     			// Get the handle of the camera 
	ECamGetCameraInfo,    			// Retrieve the info about camera device
	ECamPrepareDirectViewFinder,	// Sets up the direct view finder
	ECamStartDirectViewFinder,		// Starts view finder operation
	ECamStopDirectViewFinder,  		// Stops view finder playback
	ECamPauseDirectViewFinder,		// Pauses view finder playback
	ECamResumeDirectViewFinder,		// If paused, resumes view finder playback
	ECamDirectViewFinderState,		// Retrieves state of direct view finder
	ECamSetDirectViewFinderMirror,	// Enables/Disables mirror mode on direct viewfinder
	ECamGetDirectViewFinderMirror,	// Retrieves mirror mode status of direct viewfinder
	ECamDirectViewFinderProperties, // Retrieves properties related to the direct viewfinder
	ECamSetParameter,				// Maps Set camera parameters
	ECamGetParameter,				// Retrives camera parameters
	ECamCameraHandle  		   		// Retrieves unique camera handle
	};
	
enum TCameraParameterId   // Camera parameters to set/get 
    {
    EZoomFactor,
    EDigitalZoomFactor,
    EContrast,
    EBrightness,
    EFlash,
    EExposure,
    EWhiteBalance,
    EJpegQuality
    };
/*    
enum TCameraMode   // Camera modes in which the parameter setting is valid 
    {
    ECameraVF = 0x0001,
    ECameraIC = 0x0002,
    ECameraVC = 0x0004
    };
	
*/

enum TCameraAccessControl
	{
	ECameraRelease,
	ECameraReservedNotification,
	ECameraCancelReservedNotification,
	ECameraOverthrowNotification,
	ECameraCancelOverthrowNotification		
	};
	
enum TCameraPower
	{
	ECameraPowerOnNotification,
	ECameraPowerOff,
	ECameraCancelPowerOnNotification		
	};
	
enum TServerPanic
	{
	EPanicBadDescriptor,
	EPanicNotSupported
	};
	
typedef TPckg <TCameraInfo> TCameraInfoBuf;

struct TCamerasAvailable
	{
	TInt iCameraCount;
	};
typedef TPckg <TCamerasAvailable> TCamerasAvailablePckg;

struct TOpenCamera    // Used for passing the infos to the server side session while creating camera channel 
    {
    TInt iCameraIndex; // between 0-2 assuming 3 cameras 
    TInt iPriority; //  If NewDuplicateL called, set KRefCamNoPriority 
    TInt iHandle;   //  If NewL called, set -1 
    TBool iMMCapability; // set ETrue 
    TBool iCollaborativeClient; //  If NewDuplicateL used 
    };
typedef TPckg <TOpenCamera> TOpenCameraPckg;

struct TCameraHandle    
    {
    TInt iHandle;   
    };
typedef TPckg <TCameraHandle> TCameraHandlePckg;

struct TDirectViewFinderInfo    
    {
	TInt iScreenNum;
	TRect iScreenRect;
	TRect iClipRect;
	TSurfaceId iSurfaceId;
    };
typedef TPckg <TDirectViewFinderInfo> TDirectViewFinderInfoPckg;

struct TDirectViewFinderState
	{
	CCamera::CCameraV2DirectViewFinder::TViewFinderState iState;
	};
typedef TPckg <TDirectViewFinderState> TDirectViewFinderStatePckg;

struct TCameraZoom
	{
	TInt iZoom;
	};
typedef TPckg <TCameraZoom> TCameraZoomPckg;

struct TCameraDigitalZoom
	{
	TInt iDigitalZoom;
	};
typedef TPckg <TCameraDigitalZoom> TCameraDigitalZoomPckg;

struct TCameraContrast
	{
	TInt iContrast;
	};
typedef TPckg <TCameraContrast> TCameraContrastPckg;

struct TCameraBrightness
	{
	TInt iBrightness;
	};
typedef TPckg <TCameraBrightness> TCameraBrightnessPckg;

struct TViewFinderMirror  
    {
	TBool iMirror;
    };
typedef TPckg <TViewFinderMirror> TViewFinderMirrorPckg;

struct TCameraFlash
	{
	CCamera::TFlash iFlash;
	};
typedef TPckg <TCameraFlash> TCameraFlashPckg;

struct TCameraExposure
	{
	CCamera::TExposure iExposure;
	};
typedef TPckg <TCameraExposure> TCameraExposurePckg;

struct TCameraWhiteBalance
	{
	CCamera::TWhiteBalance iWhiteBalance;
	};
typedef TPckg <TCameraWhiteBalance> TCameraWhiteBalancePckg;


/**
 * Interface class to allow communication between clients and server.
 * 
 * Provides API's for both synchronous and asynchronous methods of communication. 
 */
class RMMCameraSession : public RSessionBase
	{
public:
	TInt Connect();

	TInt SendMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TDes8& aMessage);
	TInt SendMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TInt aMessage);
	TInt SendMessage(TECamMsgId aMsgId, TDes8& aMessage);
	TInt SendMessage(TECamMsgId aMsgId, TInt aMessage);
	TInt SendMessage(TECamMsgId aMsgId);

	TInt SendRxMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TDes8& aMessage) const;
	TInt SendRxMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TInt aMessage) const;
	TInt SendRxMessage(TECamMsgId aMsgId, TDes8& aMessage) const;

	TInt ReceiveMessage(TECamMsgId aMsgId, TDes8& aMessage, TRequestStatus& aStatus);
	void ReceiveMessage(TECamMsgId aMsgId, TInt aMessage, TRequestStatus& aStatus);
	}; 
	
#endif // MMCAMERACLIENTINTERFACE_H
