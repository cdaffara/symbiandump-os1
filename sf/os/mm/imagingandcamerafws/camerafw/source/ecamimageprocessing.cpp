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

#include <ecam/ecamimageprocessingintf.h>
#include <ecam/implementationfactoryintf.h>
#include <ecamimageprocessing.h>
#include "ecamversion.h"
#include <ecam/ecamconstants.h>

const TInt KBaselinedEffects = 0x000001FF;
const TUint KBaselinedImageProcessing = KUidECamEventImageProcessingGlareRemovalUidValue;

/**
Factory function for creating the CCameraImageProcessing object.
The created object is supposed to be meant for image captures only.

@param aCamera 
	a reference to a CCamera object providing the settings.

@return a pointer to a fully constructed CCameraImageProcessing object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.

@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraImageProcessing::NewL(CCamera& aCamera)
	{
	CCamera::CCameraImageProcessing* self = new (ELeave)CCamera::CCameraImageProcessing(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self; 
	}

/**
@internalComponent

Factory function for creating the CCameraImageProcessing object specifically for any one of the following:-
VideoCapture and Viewfinder. This may be used in 
other possible cases as well.

The other factory method CCamera::CCameraImageProcessing::NewL is assumed for being operated on image captures only.

@param aCamera 
	   A reference to a CCamera object providing the settings.
	
@param aImplFactory 
	   A reference to the MImplementationFactory derived object.

@return a pointer to a fully constructed CCameraImageProcessing object.

@leave KErrNoMemory Out of memory.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.

@note This method is supposed to be used by internal ECAM components only.
*/	
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraImageProcessing::CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory)
	{
 	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraImageProcessing* self = new (ELeave)CCamera::CCameraImageProcessing(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aImplFactory);
	CleanupStack::Pop(self);
		
	return self; 
	}	

/**	
CCameraImageProcessing Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 
*/	
EXPORT_C CCamera::CCameraImageProcessing::CCameraImageProcessing(CCamera& aOwner):iOwner(aOwner), iImpl(NULL), iImpl2(NULL), iImpl3(NULL)
	{
	}

/**
CCameraImageProcessing second phase constructor 
Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
Camera image processing interface pointer.

@leave KErrNoMemory Out of memory.
*/ 
EXPORT_C void CCamera::CCameraImageProcessing::ConstructL() 
	{
	iImpl = static_cast<MCameraImageProcessing*>(iOwner.CustomInterface(KECamMCameraImageProcessingUid));
	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl2 = static_cast<MCameraImageProcessing2*>(iOwner.CustomInterface(KECamMCameraImageProcessing2Uid));
	iImpl3 = static_cast<MCameraImageProcessing3*>(iOwner.CustomInterface(KECamMCameraImageProcessing3Uid));
	}
	
/**
@internalComponent

CCameraImageProcessing second phase constructor 

Function used to initialise internal state of the object specifically for any one of the following:-
VideoCapture and Viewfinder. This may be used in other possible cases as well.

@param aImplFactory 
	   A constant reference to the MImplementationFactory derived object.

@leave KErrNoMemory Out of memory; or any other error code as well.

@note This method is supposed to be used by this class only.
*/ 
EXPORT_C void CCamera::CCameraImageProcessing::ConstructL(const MImplementationFactory& aImplFactory) 
	{
	TInt err = KErrNone;
	TAny* implPtr = NULL;
	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraImageProcessingUid);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iImpl = static_cast<MCameraImageProcessing*>(implPtr);
	
	implPtr = NULL;	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraImageProcessing2Uid);
	if (err != KErrNone && err != KErrNotSupported)
		{
		User::Leave(err);
		}
	iImpl2 = static_cast<MCameraImageProcessing2*>(implPtr);
	
	implPtr = NULL;	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraImageProcessing3Uid);
	if (err != KErrNone && err != KErrNotSupported)
		{
		User::Leave(err);
		}
	iImpl3 = static_cast<MCameraImageProcessing3*>(implPtr);
	}
	
/**
Destructor
*/	
EXPORT_C CCamera::CCameraImageProcessing::~CCameraImageProcessing()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	
	if (iImpl2 != NULL)
		{
		iImpl2->Release();	
		}
		
	if (iImpl3 != NULL)
		{
		iImpl3->Release();	
		}
	}

/** Get all transformations supported on the camera. 
    
@param  aTransformations
		An empty RArray of TUids to store the UIDs of the supported transformations.

@leave KErrNoMemory Out of memory. 
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetSupportedTransformationsL(RArray<TUid>& aTransformations) const
	{
	iImpl->GetSupportedTransformationsL(aTransformations);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aTransformations.Count(); index++)
			{
			/** KBaselinedImageProcessing is the baseline. Any image processing attribute with greater uid value means that
			it has been added in later versions */
			if(aTransformations[index].iUid > KBaselinedImageProcessing)
				{
				aTransformations.Remove(index);
				index--;
				}
			}
		}
	}

/** Get currently active transformations on the camera. 
    
@param  aTransformations
		An empty RArray of TUids to store the UIDs of the supported transformations.

@leave KErrNoMemory Out of memory.  
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetActiveTransformationsL(RArray<TUid>& aTransformations) const
	{
	iImpl->GetActiveTransformationsL(aTransformations);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aTransformations.Count(); index++)
			{
			/** KBaselinedImageProcessing is the baseline. Any image processing attribute with greater uid value means that
			it has been added in later versions */
			if(aTransformations[index].iUid > KBaselinedImageProcessing)
				{
				aTransformations.Remove(index);
				index--;
				}
			}
		}
	}
	
/** Get all values supported by an active transformation.
    
@param  aTransformation
		The UID of active transform for which values are requested.
		
@param  aValues 
        An array of integers to represent the values for the requested transformation.
        
@param  aInfo
        Additional information describing the returned array of values.
        
@note   Depending on the value of aInfo parameter, same array of values may describe 
        different set of values.
        When camera device doesn't support this, empty array may be returned and TValueInfo may be ENotActive; 
        corresponding getter/setters for this feature should not be used in such a case.
        
@note   If CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added enum values for Effects. So, any extra enum value(unrecognised) passed 
	    from the implementation will be filtered at this point.
	    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values
	   
@leave  KErrNoMemory Out of memory.  
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const
	{
	iImpl->GetTransformationSupportedValuesL(aTransformation, aValues, aInfo);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	   application is not prepared to receive extra added enum values for Effects. So, any extra enum value(unrecognised) passed 
	   from the implementation will be filtered at this point.
	   To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	   to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(aTransformation == KUidECamEventImageProcessingEffect)
		{
		if(iOwner.CameraVersion() == KCameraDefaultVersion)
			{
			/** it is assumed that implementation will use EBitField to pack all supported effects */
			if(aInfo == EBitField)
				{
				aValues[0] &= KBaselinedEffects;
				}
			}
		}
	}
	
/** 
@deprecated Use TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue); 

Get the current value of a transformation

@param aTransformation
	   The UID of the transformation

@return The integer value of the tranformation. 	 
		
@note 	If CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added enum values for Effects. So, any extra enum value (unrecognised) received 
	    from the implementation will be dropped and EEffectNone would be passed instead.
	    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values 
*/
EXPORT_C TInt CCamera::CCameraImageProcessing::TransformationValue(TUid aTransformation) const
	{
	TInt value = iImpl->TransformationValue(aTransformation);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added enum values for Effects. So, any extra enum value (unrecognised) received 
    from the implementation will be dropped and EEffectNone would be passed instead.
    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(aTransformation == KUidECamEventImageProcessingEffect)
		{
		if(iOwner.CameraVersion() == KCameraDefaultVersion)
			{
			if(value > KBaselinedEffects)
				{
				value = CCamera::CCameraImageProcessing::EEffectNone;
				}
			}
		}
	return value;
	}
	
/** Get the current value of a transformation

@param aTransformation
	   The UID of the transformation
	   
@param  aTransformationValue
		Reference to the integer value of the tranformation.

@return system wide error code. 	 
		
@note 	If CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
	    application is not prepared to receive extra added enum values for Effects. So, any extra enum value (unrecognised) received 
	    from the implementation will be dropped and EEffectNone would be passed instead.
	    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
	    to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values 

@note	Use this method instead of deprecated TInt TransformationValue(TUid aTransformation) 

*/
EXPORT_C TInt CCamera::CCameraImageProcessing::GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const
	{
	TInt error = iImpl->GetTransformationValue(aTransformation, aTransformationValue);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added enum values for Effects. So, any extra enum value (unrecognised) received 
    from the implementation will be dropped and EEffectNone would be passed instead.
    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised enum values */
	if(aTransformation == KUidECamEventImageProcessingEffect)
		{
		if(iOwner.CameraVersion() == KCameraDefaultVersion)
			{
			if(aTransformationValue > KBaselinedEffects)
				{
				aTransformationValue = CCamera::CCameraImageProcessing::EEffectNone;
				}
			}
		}
		
	return error;
	}
	
/** Set new value for a transformation. 
 A notification event with the transformation UID is sent to 
 all clients. UIDs are in the form KUidECamEventImageProcessingXXXX.

@param aTransformation
	   The UID of the transformation

@param aValue
	   The integer value of the tranformation. 	   
*/
EXPORT_C void CCamera::CCameraImageProcessing::SetTransformationValue(TUid aTransformation, TInt aValue)
	{
	iImpl->SetTransformationValue(aTransformation, aValue);
	}
	
/** Get the sequence of all active transforms, ordered in order of execution.
 
@param aTransformSequence
	   an empty array to be populated with sequence of transform UIDs, 
	   where transform entries with smaller index are executed earlier. 
	
@leave KErrNoMemory Out of memory.  
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const
	{
	iImpl->GetActiveTransformSequenceL(aTransformSequence);
	
	/* if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added uid values (new settings added). So, any extra uid value passed 
    from the implementation will be filtered at this point.
    To receive extra added uid values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. In this case, application is assumed to be prepared to receive unrecognised uid values */
	if(iOwner.CameraVersion() == KCameraDefaultVersion)
		{
		for(TInt index =0; index < aTransformSequence.Count(); index++)
			{
			/** KBaselinedImageProcessing is the baseline. Any image processing attribute with greater uid value means that
			it has been added in later versions */
			if(aTransformSequence[index].iUid > KBaselinedImageProcessing)
				{
				aTransformSequence.Remove(index);
				index--;
				}
			}
		}
	}
	
/** 
Set the order of all active transform in terms of execution. The transforms with 
smaller index are executed earlier.

@param aTransformSequence
	   The list of ordered transforms, where transforms with smaller 
	   index are executed earlier.
  
@leave KErrNoMemory Out of memory.
*/
EXPORT_C void CCamera::CCameraImageProcessing::SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence)
	{
	iImpl->SetActiveTransformSequenceL(aTransformSequence);
	}
	
/**
Set the source rectangle for KUidECamEventImageProcessingTransformScale or 
KUidECamEventImageProcessingTransformCrop.
The coordinates should fall within the current image rectangle. The result 
is always a logical AND operation between the two rectangles.
		   
@param aRect
	   a reference to TRect object which describes the coordinates of the 
	   area of interest. 
*/
EXPORT_C void CCamera::CCameraImageProcessing::SetSourceRect(const TRect& aRect)
	{
	iImpl->SetSourceRect(aRect);
	}
	
/**
Get the source rectangle for KUidECamEventImageProcessingTransformScale or 
KUidECamEventImageProcessingTransformCrop.
The coordinates should fall within the current image rectangle. The result 
is always a logical AND operation between the two rectangles.
		   
@param aRect
		a reference to TRect object to hold the current source rectangle 
		coordinates. If it has not been set, the coordinates match these of 
		the whole image.      
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetSourceRect(TRect& aRect) const
	{
	iImpl->GetSourceRect(aRect);
	}
	

/**
Retrieves the maximum number of simultaneous color swapping possible.
	
@param aConcurrentColorSwappingSupported
	   Retrieves the number of simultaneous color swapping supported. 
	   Retrieves 0 when swapping feature is not supported.
	
@leave May leave as a result of some error.

*/       
EXPORT_C void CCamera::CCameraImageProcessing::GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetConcurrentColorSwappingsSupportedL(aConcurrentColorSwappingSupported);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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
EXPORT_C void CCamera::CCameraImageProcessing::GetColorSwapCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorSwapCapabilities) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetColorSwapCapabilitiesL(aIndex, aColorSwapCapabilities);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}
	
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
	   
@leave KErrNotSupported if implementation is not present.
	   
@note  Triggers KUidECamEventCIPSetColorSwapEntry to all MCameraObserver2 clients of the camera. 
	   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should 
	   be used in order to provide the entry no. of the color being set. 

*/		 
EXPORT_C void CCamera::CCameraImageProcessing::SetColorSwapEntryL(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SetColorSwapEntry(aIndex, aColorSwapParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}
	
/**
Removes the color swap entry corresponding to the given index

@param aIndex
	   This gives the color swapping entry to be removed. 
	   
@leave KErrNotSupported if implementation is not present.
	   
@note  Triggers KUidECamEventCIPRemoveColorSwapEntry to all MCameraObserver2 clients of the camera.
	   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be
	   used in order to provide the entry no. of the color being removed. 

*/
EXPORT_C void CCamera::CCameraImageProcessing::RemoveColorSwapEntryL(TInt aIndex)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->RemoveColorSwapEntry(aIndex);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Get the details of the color swap entry corresponding to the given index

@param aIndex
	   This gives the color swapping entry whose information has to be retrieved.

@param aColorSwapParameters
	   This contains the parameters currently being used by the color swapping operation for the given entry.

@leave May leave as a result of some error. 

*/			   
EXPORT_C void CCamera::CCameraImageProcessing::GetColorSwapEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const
	 {
	if(iImpl2 != NULL)
		{
		iImpl2->GetColorSwapEntryL(aIndex, aColorSwapParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Starts the color swapping process after taking into account the color swap entries updated up to this point.

@leave KErrNotSupported if implementation is not present.

@note Triggers KUidECamEventCIPStartColorSwap to all MCameraObserver2 clients of the camera. 
	  HandleEvent is used to report the result or any possible error. 
	  One possible error case is when more than one entry describe the same color source. 
	  New ecam error KErrECamColorOperationConflict used in such a case.

*/		 
EXPORT_C void CCamera::CCameraImageProcessing::StartColorSwappingL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->StartColorSwapping();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Cancel the color swapping process.

@leave May leave as a result of some error.

@note  Used to cancel the color swapping process which might have been just started. 
	   If the issued StartColorSwappingL() gets cancelled, its event should report KErrCancel.

*/
EXPORT_C void CCamera::CCameraImageProcessing::CancelColorSwappingL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->CancelColorSwappingL();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves the maximum number of color entries on which simultaneous color accent process is possible.

@param aConcurrentColorAccentSupported
	   Retrieves the number of color entries on which simultaneous color accent process is possible. 
	   Retrieves 0 when color accent process is not supported.

@leave May leave as a result of some error. 

*/    
EXPORT_C void CCamera::CCameraImageProcessing::GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetConcurrentColorAccentSupportedL(aConcurrentColorAccentSupported);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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
EXPORT_C void CCamera::CCameraImageProcessing::GetColorAccentCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorAccentCapabilities) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetColorAccentCapabilitiesL(aIndex, aColorAccentCapabilities);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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
	   
@leave KErrNotSupported if implementation is not present.
	   
@note  Triggers KUidECamEventCIPSetColorAccentEntry to all MCameraObserver2 clients of the camera. 
	   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be 
	   used in order to provide the entry no. of the color being set. 

*/		 
EXPORT_C void CCamera::CCameraImageProcessing::SetColorAccentEntryL(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SetColorAccentEntry(aIndex, aColorAccentParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Removes the color accent entry corresponding to the given index

@param aIndex
	   This gives the color accent entry to be removed. 
	   
@leave KErrNotSupported if implementation is not present.
	   
@note  Triggers KUidECamEventCIPRemoveColorAccentEntry to all MCameraObserver2 clients of the camera.
	   HandleEvent is used to report the result or any possible error. TECAMEvent2 class should be 
	   used in order to provide the entry no. of the color being removed. 

*/		 
EXPORT_C void CCamera::CCameraImageProcessing::RemoveColorAccentEntryL(TInt aIndex)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->RemoveColorAccentEntry(aIndex);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Get the details of the color accent entry corresponding to the given index

@param aIndex
	   This gives the color accent entry whose information has to be retrieved.

@param aColorAccentParameters
	   This contains the parameters currently being used by the color accent operation for the given entry.
	   The parameters defined for target colors in TColorOperationEntry are redundant for color accent.

@leave May leave as a result of some error. 

*/		  
EXPORT_C void CCamera::CCameraImageProcessing::GetColorAccentEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetColorAccentEntryL(aIndex, aColorAccentParameters);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	} 

/**
Starts the color accent process after taking into account the color accent entries updated up to this point.

@leave KErrNotSupported if implementation is not present.

@note  Triggers KUidECamEventCIPStartColorAccent to all MCameraObserver2 clients of the camera. 
	   HandleEvent is used to report the result or any possible error. 

*/
EXPORT_C void CCamera::CCameraImageProcessing::StartColorAccentL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->StartColorAccent();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Cancel the color accent process.

@leave May leave as a result of some error.

@note  Used to cancel the color accent process which might have been just started. 
	   If the issued StartColorAccentL() gets cancelled, its event should report KErrCancel.

*/
EXPORT_C void CCamera::CCameraImageProcessing::CancelColorAccentL()
	{
	if(iImpl2 != NULL)
		{
		iImpl2->CancelColorAccentL();	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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

@publishedPartner
@prototype
*/
EXPORT_C void CCamera::CCameraImageProcessing::GetSupportedRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
		TUint& aSupportedRelativeRotation, TUint& aSupportedRelativeMirroring, TUint& aSupportedRelativeFlipping) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetSupportedRelativeOrientationOptionsL(aOrientationReference, aSupportedRelativeRotation, 
									aSupportedRelativeMirroring, aSupportedRelativeFlipping);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}		
	}										

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

@publishedPartner
@prototype
*/															
EXPORT_C void CCamera::CCameraImageProcessing::GetCurrentRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference& aOrientationReference, 
											CCamera::CCameraImageProcessing::TRelativeRotation& aRelativeRotation, 
											CCamera::CCameraImageProcessing::TRelativeMirror& aRelativeMirror, 
											CCamera::CCameraImageProcessing::TRelativeFlipping& aRelativeFlipping) const
	{
	if(iImpl3 != NULL)
		{
		iImpl3->GetCurrentRelativeOrientationOptionsL(aOrientationReference, aRelativeRotation, aRelativeMirror, aRelativeFlipping);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

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
		
@leave  KErrNotSupported if the implementation of this method is not present.
		
@note   Event KUidECamEventImageProcessingTransformRelativeOrientation is used to notify clients about relative 
		custom orientation setting operation.
		
@note   If the current picture orientation (Refer CCamera::CCameraAdvancedSettings::TPictureOrientation) is not possible
		to be achieved with the relative custom orientation, event KUidECamEventPictureOrientationUnachievable will be
		notified to the client.
		
@note   If the dimension of the image gets changed by the desired relative orientation options, notification 
		KUidECamEventCameraSettingImageSize will be notified to the client.
		
@publishedPartner
@prototype
*/												
EXPORT_C void CCamera::CCameraImageProcessing::SetRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
											CCamera::CCameraImageProcessing::TRelativeRotation aRelativeRotation, 
											CCamera::CCameraImageProcessing::TRelativeMirror aRelativeMirror, 
											CCamera::CCameraImageProcessing::TRelativeFlipping aRelativeFlipping)
	{
	if(iImpl3 != NULL)
		{
		iImpl3->SetRelativeOrientationOptions(aOrientationReference, aRelativeRotation, aRelativeMirror, aRelativeFlipping);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Constructor for the TColorOperationCapabilities class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraImageProcessing::TColorOperationCapabilities::TColorOperationCapabilities()
	{
	iSize = sizeof(CCamera::CCameraImageProcessing::TColorOperationCapabilities);
	iVersion = KECamColorOperationCapabilitiesCurrentVersion;
	}
	
/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TInt CCamera::CCameraImageProcessing::TColorOperationCapabilities::Size() const
	{
	return static_cast<TInt>(iSize);
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraImageProcessing::TColorOperationCapabilities::Version() const
	{
	return iVersion;	
	}
	
/**
Constructor for the TBitsIgnore class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraImageProcessing::TBitsIgnore::TBitsIgnore()
	{
	iSize = sizeof(CCamera::CCameraImageProcessing::TBitsIgnore);
	iVersion = KECamBitsIgnoreCurrentVersion;
	iRedBitsIgnore = 0;
	iGreenBitsIgnore = 0;
	iBlueBitsIgnore = 0;
	iAlphaBitsIgnore = 0;
	}
	
/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/	
EXPORT_C TInt CCamera::CCameraImageProcessing::TBitsIgnore::Size() const
	{
	return static_cast<TInt>(iSize);
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraImageProcessing::TBitsIgnore::Version() const
	{
	return iVersion;	
	}
	
/**
Constructor for the TColorOperationEntry class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraImageProcessing::TColorOperationEntry::TColorOperationEntry()
	{
	iSize = sizeof(CCamera::CCameraImageProcessing::TColorOperationEntry);
	iVersion = KECamColorOperationEntryCurrentVersion;
	}
	
/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/	
EXPORT_C TInt CCamera::CCameraImageProcessing::TColorOperationEntry::Size() const
	{
	return static_cast<TInt>(iSize);
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraImageProcessing::TColorOperationEntry::Version() const
	{
	return iVersion;	
	}
