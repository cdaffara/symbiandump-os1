// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/
#ifndef ECAM__CONST_H
#define ECAM__CONST_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamuidsconst.hrh>
#endif
/** 
Event indicating continuous zoom progess. This event is used for StartContinuousZoomL feature. This is a part of class 
CCamera::CCameraAdvancedSettings. This event should be packed in TECAMEvent2 class. 

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::
NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

Note: TECAMEvent2::iParam represents percentage continuous zoom completion.

@note  If zoom direction is EZoomDirectionWide, percentage completion will target minimum possible value as 100%.
@note  If zoom direction is EZoomDirectionTele, percentage completion will target maximum possible value as 100%.

@publishedPartner
@prototype
*/
static const TUid  KUidECamEvent2CameraSettingContinuousZoomPercentageCompletion  = {KUidECamEvent2CameraSettingContinuousZoomPercentageCompletionUidValue};

/**
Notifies that unrequested feature changes have occurred. The method GetIndirectFeatureChangesL() is called to
retrieve the list of unrequested feature changes. The unrequested feature changes are ECAM component wide.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

Note: TECAMEvent2::iParam represents the TInt used to obtain a uid which represents the requested feature change.

@publishedPartner
@prototype
*/
static const TUid KUidECamEvent2IndirectFeatureChange  = {KUidECamEvent2IndirectFeatureChangeUidValue};

/**
Event indicating auto aperture is being used.
This event should be packed in TECAMEvent2 class.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::
NewDuplicate2L():
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

Note: TECAMEvent2::iParam represents actual value of aperture being used if camera is capable of. Otherwise, KErrNotFound will be retrieved.
Note: This event is also used if exposure is set to CCamera::EExposureAuto or CCamera::EExposureShutterPriority.


@publishedPartner
@prototype
*/
static const TUid  KUidECamEvent2CameraSettingAutoAperture  = {KUidECamEvent2CameraSettingAutoApertureUidValue};

/** Specifies whether the camera is reserved or not.
	The enumeration list may be extended in future.
	
@publishedPartner
@prototype
*/
enum TECamReserveStatus
	{
	/** Camera Status unknown */
	ECameraStatusUnknown,
	/** Camera is reserved */
	ECameraReserved,
	/** Camera is unreserved */
	ECameraUnReserved
	};

/** Mixin base class for camera clients to handle the notification of Reserve status.

Client must implement MReserveObserver in order to handle the notifications and take appropriate steps accordingly. 

@note  By the time client gets unreserved status via callback, the camera may be on its way getting reserved by another client who 
	   might be continuously polling for it.
	   So, there is no guarantee that the client will be able to reserve it. But it is guaranteed that the client will
	   receive the notification about change in reserve status. 

@publishedPartner
@prototype
*/
class MReserveObserver
	{
public:
	/**
	This notification is send to provide the reserve status for the camera. 
	
	@param aCameraIndex
		   The camera index for which the Reserve status has to be provided.
		   
	@param aReserveStatus
		   The reserve status for the camera.
		   
	@param aErrorCode
		   The error value. 
		   
	@note  If error is not KErrNone, then the client is expected to destroy the CCameraStatusWatch retrieved through 
		   TReservedInfo::SubscribeReserveInfoL and re-subscribe if desired.
	*/
	virtual void ReserveStatus(TInt iCameraIndex, TECamReserveStatus aReserveStatus, TInt aErrorCode) =0;
	};

class CCameraStatusWatch;

/**
@publishedPartner
@prototype

Client uses it to asynchronously receive the reserve status of a camera index through MReserveObserver
*/
class TReservedInfo
	{
public:
	IMPORT_C static void SubscribeReserveInfoL(MReserveObserver& aReserveObserver, TInt aCameraIndex, CCameraStatusWatch*& aCameraStatusWatch); 
	};

/**
This class is used to provide extra buffer informations through a custom interface.

@see MCameraImageBuffer

@publishedPartner
@prototype
*/
class MCameraBuffer2 : public MCameraBuffer
	{
public:
	/**
	Retrieves an array of uids which represents the class identifier used for buffer extension.
	
	@param aInterfaceUids
		   An array of uids which represents the class identifier.
		   
	@return The error code.
	*/
	virtual TInt GetInterfaceUids(RArray<TUid>& aInterfaceUids) = 0;
	
	/**
	Gets a custom interface for extra buffer information. 

	@param aInterface
		   The Uid of the particular interface function required for buffer information.
		   
	@param aPtrInterface
		   The client has to cast the custom interface pointer to the appropriate type.	

	@return The error code.
	*/
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface) = 0;
	};
	
#endif // ECAM__CONST_H
