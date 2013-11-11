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
 @released
*/

#ifndef  ECAMIMAGEPROCESSINGINTF_H
#define  ECAMIMAGEPROCESSINGINTF_H

#include <ecamimageprocessing.h>
#include <ecam/ecamadvsettingsintfuids.hrh>

/** This is the UID which is used to obtain the interface MCameraImageProcessing, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. */   
static const TUid KECamMCameraImageProcessingUid 	=  {KECamMCameraImageProcessingUidValue};

/** 
This is the UID which is used to obtain the interface MCameraImageProcessing2, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. 
*/   
static const TUid KECamMCameraImageProcessing2Uid 	=  {KECamMCameraImageProcessing2UidValue};

/** 
This is the UID which is used to obtain the interface MCameraImageProcessing3, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. 
@publishedPartner
@prototype
*/   
static const TUid KECamMCameraImageProcessing3Uid 	=  {KECamMCameraImageProcessing3UidValue};


/** 
Mixin class for implementation by providers of the Image Processing Camera Extension API.

This class is used to perform image processing operations on the camera.
These include brightness, contrast, gamma, hue, sharpness and saturation adjustments. The client is also 
able to perform simple image transformations like cropping, rotation, mirroring, scaling, noise reduction and glare reduction. .  
When an operation selection is complete, all clients are notified with the respective event UID.

As often cameras may support only a subset of discrete values of the allowed range, 
the API allows the client to retrieve those and use them explicitly.   

@note it is assumed that setting a new value for a transformations(transform, adjust, effect) effectively
	  activates the transformations. Whilst for effects and adjustments there is always a value, transforms 
	  may have a dependency on other parameters and 
	  crop - requires setting of source rectangle.
	  scale - will use setting of source rectangle, and the magnification factor is 
	  				determined by the source rectangle and the output size. This is always magnification.
	  				if a value is set, it is assumed to be a scaling factor multiplied by KECamFineResolutionFactor
	  			and set to integer.
	  mirror - values of TMirror type.
	  rotation - the angle in degrees.
	  noise reduction - TNoiseReduction.
	  glare removal - TGlareReduction.	

Example 
		Let's assume that an application would need to check whether gamma correction is 
        supported on a particular platform. After obtaining a valid pointer to the interface, 
        it would call GetSupportedTransformationsL() to obtain the list of the supported 
        tranformations and check whether KUidECamEventImageProcessingAdjustGamma
		is in the list. If it is then call SetTranformationValue(KUidECamEventImageProcessingAdjustGamma, 200);
		to set the new value. A notification will be generated to the client to indicate success.
		
@publishedPartner
@released 	
*/
class MCameraImageProcessing
	{
	
public:
	
	/** 
	Releases the interface. 
	*/
    virtual void Release()=0;

	/** Get all transformations supported on the camera. 
	    
	@param  aTransformations
			An empty RArray of TUids to store the UIDs of the supported transformations.

	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
	virtual void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const=0;

	/** Get currently active transformations on the camera. 
	    
	@param  aTransformations
			An empty RArray of TUids to store the UIDs of the supported transformations.

	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.  
	*/
	virtual void GetActiveTransformationsL(RArray<TUid>& aTransformations) const=0;

	/** Get all values supported by an active transformation.
	    
	@param  aTransformation
			The UID of active transform for which values are requested.
			
	@param  aValues 
	        An array of integers to represent the values for the requested transformation.
	        
	@param  aInfo
	        Additional information describing the returned array of values.
	        
	@note   depending on the value of aInfo parameter, same array of values may describe 
	        different set of values.
	        When camera device doesn't support this, empty array should be returned and TValueInfo should be ENotActive,
	        and the corresponding getter/setters for this feature should not be used.
	        
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors. 
	*/
	virtual void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const=0;

	/** 
	@deprecated Use TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue);
	
	Get the current value of a transformation

	@param aTransformation
		   The UID of the transformation

	@return The integer value of the tranformation. 	   
	*/
	virtual TInt TransformationValue(TUid aTransformation) const=0;

	/** Get the current value of a transformation

	@param aTransformation
		   The UID of the transformation
		   
	@param aTransformationValue
		   Reference to the integer value of the tranformation.

	@return system wide error code. 	   
	*/
	virtual TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const=0;

	/** Set new value for a transformation. 
	 A notification event with the transformation UID is sent to 
	 all clients. UIDs are in the form KUidECamEventImageProcessingXXXX.

	@param aTransformation
		   The UID of the transformation

	@param aValue
		  The integer value of the tranformation. 	   
	*/
	virtual void SetTransformationValue(TUid aTransformation, TInt aValue)=0;

	/** Get the sequence of all active transforms, ordered in order of execution.
	 
	@param aTransformSequence
		   an empty array to be populated with sequence of transform UIDs, 
		   where transform entries with smaller index are executed earlier. 
		
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.	   
	*/
	virtual void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const=0;

	/** 
	Set the order of all active transform in terms of execution. The transforms with 
	smaller index are executed earlier.

	@param aTransformSequence
		   The list of ordered transforms, where transforms with smaller 
		   index are executed earlier.
	  
	@leave KErrNoMemory Out of memory. May also leave as a result of other system errors.
	*/
	virtual void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence)=0;

	/**
	Set the source rectangle for KUidECamEventImageProcessingTransformScale or 
	KUidECamEventImageProcessingTransformCrop.
	The coordinates should fall within the current image rectangle. The result 
	is always a logical AND operation between the two rectangles.
			   
	@param aRect
		   a reference to TRect object which describes the coordinates of the 
		   area of interest. 
	*/
	virtual void SetSourceRect(const TRect& aRect)=0;

	/**
	Get the source rectangle for KUidECamEventImageProcessingTransformScale or 
	KUidECamEventImageProcessingTransformCrop.
	The coordinates should fall within the current image rectangle. The result 
	is always a logical AND operation between the two rectangles.
			   
	@param aRect
			a reference to a TRect object to hold the current source rectangle 
			coordinates. If it has not been set, the coordinates match these of 
			the whole image.      
	*/
	virtual void GetSourceRect(TRect& aRect) const=0;
	};

/** 
Mixin class for implementation of extended features like color swap and color accent processing 
by providers of the Image Processing Camera Extension API.

@publishedPartner
@released
*/
class MCameraImageProcessing2
	{
	
public:
	
	/** 
	Releases the interface. 
	*/
    virtual void Release()=0;	
    
	/**
	Retrieves the maximum number of simultaneous color swapping possible.
	
	@param aConcurrentColorSwappingSupported
		   Retrieves the number of simultaneous color swapping supported. 
		   Retrieves 0 when swapping feature is not supported.
	
	@leave May leave as a result of some error. 
	*/		
	virtual void GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const=0;
	
	/**
	Retrieves the color swapping capabilites per entry, if different entries have different capabilities 
	otherwise the same capabilities retrieved for a particular entry can be assumed to be valid for every entry
	
	@param aIndex 
		   This is a value from 0 to numOfSimultaneousColorSwappings -1. Color swapping capabilities specific to 
		   a particular entry are retrieved. If uniform capability exists for every entry, then this method need not 
		   be called per entry.	
		   
	@param aColorSwapCapabilities
		   This retrieves the color swap capabilities.
		   
	@leave May leave as a result of some error.
	*/
	virtual void GetColorSwapCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorSwapCapabilities) const=0;

	/**
	Set the color swap entries
	
	@param aIndex
		   This is a value from 0 to numOfSimultaneousColorSwappings -1. This helps in managing the limited no. of 
		   simultaneous color swaps. If parameters are already set for the given entry, then it's up to the implementation
		   to replace the existing one or discard it. 
    
    @param aColorSwapParameters
    	   The parameters necessary to define clearly the color swapping operation for the given entry. 
    	   iEntryStatus has to be updated by the implementation as per the result of the setting operation. 
    	   So, iEntryStatus value is redundant at this point.
    	   
	@note  Triggers KUidECamEventCIPSetColorSwapEntry to all MCameraObserver2 clients of the camera. 
		   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should 
		   be used in order to provide the entry no. of the color being set. 
		   Implementation Hint: On success, entry status (iEntryStatus) for that entry should
		   be set to TValueInfo::EDiscreteSteps and saved on the implementation side.
	*/
	virtual void SetColorSwapEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters)=0; 
	
	/**
	Removes the color swap entry corresponding to the given index
	
	@param aIndex
		   This gives the color swapping entry to be removed. 
		   
	@note  Triggers KUidECamEventCIPRemoveColorSwapEntry to all MCameraObserver2 clients of the camera.
		   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be
		   used in order to provide the entry no. of the color being removed. 
		   Implementation Hint: On success, entry status (iEntryStatus) for that entry should
		   be set to TValueInfo::ENotActive and saved on the implementation side.
	*/
	virtual void RemoveColorSwapEntry(TInt aIndex)=0;
	
	/**
	Get the details of the color swap entry corresponding to the given index
	
	@param aIndex
		   This gives the color swapping entry whose information has to be retrieved.
	
	@param aColorSwapParameters
		   This contains the parameters currently being used by the color swapping operation for the given entry.
	
	@leave May leave as a result of some error. 
	*/
	virtual void GetColorSwapEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const=0; 
	
	/**
	Starts the color swapping process after taking into account the color swap entries updated up to this point.
	
	@note Triggers KUidECamEventCIPStartColorSwap to all MCameraObserver2 
		  clients of the camera. HandleEvent is used to report the result or any possible error. 
		  One possible error case is when more than one entry describe the same color source. 
		  New ecam error KErrECamColorOperationConflict used in such a case.
	*/
	virtual void StartColorSwapping()=0;
	
	/**
	Cancel the color swapping process.
	
	@leave May leave as a result of some error.
	
	@note  Used to cancel the color swapping process which might have been just started. 
		   If the issued StartColorSwappingL() gets cancelled, its event should report KErrCancel.
	*/
	virtual void CancelColorSwappingL()=0;
	
	/**
	Retrieves the maximum number of color entries on which simultaneous color accent process is possible.
	
	@param aConcurrentColorAccentSupported
		   Retrieves the number of color entries on which simultaneous color accent process is possible. 
		   Retrieves 0 when color accent process is not supported.
	
	@leave May leave as a result of some error. 
	*/
	virtual void GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const=0;
	
	/**
	Retrieves the color accent capabilites per entry, if different entries have different capabilities 
	otherwise the same capabilities retrieved for a particular entry can be assumed to be valid for every entry
	
	@param aIndex 
		   This is a value from 0 to numOfSimultaneousColorAccent -1. Color accent capabilities specific to 
		   a particular entry are retrieved. If uniform capability exists for every entry, then this method need not 
		   be called per entry.	
		   
	@param aColorAccentCapabilities
		   This retrieves the color accent capabilities.
		   
	@leave May leave as a result of some error.
	*/
	virtual void GetColorAccentCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorAccentCapabilities) const=0;
	
	/**
	Set the color accent entries
	
	@param aIndex
		   This is a value from 0 to numOfSimultaneousColorAccent -1. This helps in managing the limited no. of 
		   simultaneous color accent. If parameters are already set for the given entry, then it's up to the implementation
		   to replace the existing one or discard it. 
    
    @param aColorAccentParameters
    	   The parameters necessary to define clearly the color accent operation for the given entry. 
    	   iEntryStatus has to be updated by the implementation as per the result of the setting operation. 
    	   So, iEntryStatus value is redundant at this point. The parameters defined for target colors in 
    	   TColorOperationEntry are redundant for color accent.
    	   
	@note  Triggers KUidECamEventCIPSetColorAccentEntry to all MCameraObserver2 clients of the camera. 
		   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be 
		   used in order to provide the entry no. of the color being set. 
		   Implementation Hint: On success, entry status (iEntryStatus) for that entry should
		   be set to TValueInfo::EDiscreteSteps and saved on the implementation side.
	*/
	virtual void SetColorAccentEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters)=0; 

	/**
	Removes the color accent entry corresponding to the given index
	
	@param aIndex
		   This gives the color accent entry to be removed. 
		   
	@note  Triggers KUidECamEventCIPRemoveColorAccentEntry to all MCameraObserver2 clients of the camera.
		   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be 
		   used in order to provide the entry no. of the color being removed. 
		   Implementation Hint: On success, entry status (iEntryStatus) for that entry should
		   be set to TValueInfo::ENotActive and saved on the implementation side.
	*/
	virtual void RemoveColorAccentEntry(TInt aIndex)=0;
	 
	/**
	Get the details of the color accent entry corresponding to the given index
	
	@param aIndex
		   This gives the color accent entry whose information has to be retrieved.
	
	@param aColorAccentParameters
		   This contains the parameters currently being used by the color accent operation for the given entry.
		   The parameters defined for target colors in TColorOperationEntry are redundant for color accent.
	
	@leave May leave as a result of some error. 
	*/
	virtual void GetColorAccentEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const=0; 
	
	/**
	Starts the color accent process after taking into account the color accent entries updated up to this point.
	
	@note  Triggers KUidECamEventCIPStartColorAccent to all MCameraObserver2 
		   clients of the camera. HandleEvent is used to report the result or any possible error. 
	*/
	virtual void StartColorAccent()=0;
	
	/**
	Cancel the color accent process.
	
	@leave May leave as a result of some error.
	
	@note  Used to cancel the color accent process which might have been just started. 
		   If the issued StartColorAccentL() gets cancelled, its event should report KErrCancel.
	*/
	virtual void CancelColorAccentL()=0;
	};
	
/** 
Mixin class for implementation of extended features like custom orientation and event filtering 
by providers of the Image Processing Camera Extension API.

@publishedPartner
@prototype
*/
class MCameraImageProcessing3
	{

public:
	
	/** 
	Releases the interface. 
	*/
    virtual void Release()=0;
	
	/**
	Retrieves the supported options for a particular orientation reference.

	@param  aOrientationReference
			A TOrientationReference for which supported relative custom orientation have to retrieved.
			
	@param  aSupportedRelativeRotation
			A bitfield which retrieves the supported TRelativeRotation for 'aOrientationReference'
			
	@param  aSupportedRelativeMirroring
			A bitfield which retrieves the supported TRelativeMirror for 'aOrientationReference'
			
	@param  aSupportedRelativeFlipping
			A bitfield which retrieves the supported TRelativeFlipping for 'aOrientationReference'

	@leave  May leave with any error code.
	*/
	virtual void GetSupportedRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
			TUint& aSupportedRelativeRotation, TUint& aSupportedRelativeMirroring, TUint& aSupportedRelativeFlipping) const=0;
	
	/**
	Retrieves the options which is being used for the current orientation reference.

	@param  aOrientationReference
			A TOrientationReference which is the current orientation reference being used.
			
	@param  aRelativeRotation
			A TRelativeRotation which is the current relative rotation being used with aOrientationReference.
			
	@param  aRelativeMirror
			A TRelativeMirror which is the current relative mirroring being used with aOrientationReference.
			
	@param  aRelativeFlipping
			A TRelativeFlipping which is the current relative flipping being used with aOrientationReference.
			
	@leave  May leave with any error code.
	*/													
	virtual void GetCurrentRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference& aOrientationReference, 
											CCamera::CCameraImageProcessing::TRelativeRotation& aRelativeRotation, 
											CCamera::CCameraImageProcessing::TRelativeMirror& aRelativeMirror, 
											CCamera::CCameraImageProcessing::TRelativeFlipping& aRelativeFlipping) const=0;
	
	/**
	Sets the options which would be used with the desired orientation reference.

	@param  aOrientationReference
			The desired TOrientationReference.
			
	@param  aRelativeRotation
			The desired TRelativeRotation which would be used with 'aOrientationReference'.
			
	@param  TRelativeMirror
			The desired TRelativeMirror which would be used with 'aOrientationReference'

	@param  TRelativeFlipping
			The desired TRelativeFlipping which would be used with 'aOrientationReference'
			
	@note   Event KUidECamEventImageProcessingTransformRelativeOrientation is used to notify clients about relative 
			custom orientation setting operation.
			
	@note   If the current picture orientation (Refer CCamera::CCameraAdvancedSettings::TPictureOrientation) is not possible
			to be achieved with the relative custom orientation, event KUidECamEventPictureOrientationUnachievable will be
			notified to the client.
			
	@note   If the dimension of the image gets changed by the desired relative orientation options, notification 
			KUidECamEventCameraSettingImageSize will be notified to the client.
	*/												
	virtual void SetRelativeOrientationOptions(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
											CCamera::CCameraImageProcessing::TRelativeRotation aRelativeRotation, 
											CCamera::CCameraImageProcessing::TRelativeMirror aRelativeMirror, 
											CCamera::CCameraImageProcessing::TRelativeFlipping aRelativeFlipping) const=0;
	};
	
#endif // ECAMIMAGEPROCESSINGINTF_H
