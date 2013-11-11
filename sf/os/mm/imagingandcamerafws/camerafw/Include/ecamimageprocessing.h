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
 @publishedAll
 @released
*/
#ifndef  ECAMIMAGEPROCESSING_H
#define  ECAMIMAGEPROCESSING_H

#include <e32base.h>
#include <gdi.h>
#include <ecam.h>
#include <ecamadvsettingsuids.hrh>
#include <ecam/ecamconstants.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamimageprocessingconst.h>
#include <ecamimageprocessingdef.h>
#endif


class MCameraImageProcessing;
class MCameraImageProcessing2;
class MCameraImageProcessing3;

class MImplementationFactory;

/** 
The current Version of the TColorOperationCapabilities class.

*/
static const TUint KECamColorOperationCapabilitiesCurrentVersion = 1;

/** 
The current Version of the TBitsIgnore class.

*/
static const TUint KECamBitsIgnoreCurrentVersion = 1;

/** 
The current Version of the TColorOperationEntry class.

*/
static const TUint KECamColorOperationEntryCurrentVersion = 1;


/* General purpose constants */

/** 
	Uid used to identify the CCamera Post Processing API.
	Such image operations are performed on the acquired image by the camera itself. 
	@see CCamera::CCameraPresets
*/
static const TUid  KECamImageProcessingUid 	= {KECamImageProcessingUidValue};

/** Transformations */
/* Adjustments */
/** Brightness adjustment - recognized range [-100:100] */
static const TUid  KUidECamEventImageProcessingAdjustBrightness 	= {KUidECamEventImageProcessingAdjustBrightnessUidValue};
/** Contrast adjustment  - recognized range [-100:100] */
static const TUid  KUidECamEventImageProcessingAdjustContrast 		= {KUidECamEventImageProcessingAdjustContrastUidValue};
/** Gamma adjustment  - recognized range [0:1000] */
static const TUid  KUidECamEventImageProcessingAdjustGamma 			= {KUidECamEventImageProcessingAdjustGammaUidValue};
/** Sharpness adjustment - recognized range [-100:100] */
static const TUid  KUidECamEventImageProcessingAdjustSharpness 		= {KUidECamEventImageProcessingAdjustSharpnessUidValue};
/** Saturation adjustment  - recognized range [-100:100] */
static const TUid  KUidECamEventtImageProcessingAdjustSaturation 	= {KUidECamEventtImageProcessingAdjustSaturationUidValue};
static const TUid  KUidECamEventImageProcessingAdjustSaturation 	= {KUidECamEventtImageProcessingAdjustSaturationUidValue};
/** Hue adjustment  - recognized range [-100:100] */
static const TUid  KUidECamEventtImageProcessingAdjustHue 			= {KUidECamEventtImageProcessingAdjustHueUidValue};
static const TUid  KUidECamEventImageProcessingAdjustHue 			= {KUidECamEventtImageProcessingAdjustHueUidValue};


/** Effect value */
static const TUid  KUidECamEventImageProcessingEffect 				= {KUidECamEventImageProcessingEffectUidValue};

/* Transforms */
/** Crop */
static const TUid  KUidECamEventImageProcessingTransformCrop 		= {KUidECamEventImageProcessingTransformCropUidValue};
/** Rotation */
static const TUid  KUidECamEventImageProcessingTransformRotate 		= {KUidECamEventImageProcessingTransformRotateUidValue};
/** Mirroring */
static const TUid  KUidECamEventImageProcessingTransformMirror		= {KUidECamEventImageProcessingTransformMirrorUidValue};
/** Rescale */
static const TUid  KUidECamEventImageProcessingTransformScale		= {KUidECamEventImageProcessingTransformScaleUidValue};
/** origin subset */
static const TUid  KUidECamEventImageProcessingSourceRect 			= {KUidECamEventImageProcessingSourceRectUidValue};
/** Noise reduction */
static const TUid  KUidECamEventImageProcessingNoiseReduction		= {KUidECamEventImageProcessingNoiseReductionUidValue};
/** Glare removal */
static const TUid  KUidECamEventImageProcessingGlareRemoval			= {KUidECamEventImageProcessingGlareRemovalUidValue};

/** Default value for image processing */
static const TInt  KECamImageProcessingDefaultValue  = 0;

/**
Uid used to indicate completion of StartColorSwap

*/
static const TUid  KUidECamEventCIPStartColorSwap					= {KUidECamEventCIPStartColorSwapUidValue};

/**
Uid used to indicate completion of StartColorAccent

*/
static const TUid  KUidECamEventCIPStartColorAccent					= {KUidECamEventCIPStartColorAccentUidValue};


/** 
This class is used to perform image processing operations on the camera.
These include brightness, contrast, gamma, hue, sharpness and saturation adjustments. The client is also 
able to perform simple image transformations like cropping, rotation, mirroring, scaling, noise reduction and glare reduction. .  
When an operation selection is complete, all clients are notified with the respective event UID.

As often cameras may support only a subset of discrete values of the allowed range, 
the API allows the client to retrieve those and use them explicitly.   

@note This class is not intended for sub-classing and used to standardise existing
	  varieties of implementations.

@note it is assumed that setting a new value for a transformations(transform, adjust, effect) effectively
	  activates the transformations. Whilst for effects and adjustments there is always a value, transforms 
	  may have a dependency on other parameters and 
	  crop 	- requires setting of source rectangle.
	  scale - will use setting of source rectangle, and the magnification factor is 
	  		  determined by the source rectangle and the output size. This is always magnification.
	  		  if a value is set, it is assumed to be a scaling factor multiplied by KECamFineResolutionFactor
	  		  and set to integer.
	  mirror - values of TMirror type.
	  rotation - the angle in degrees.
	  noise reduction - TNoiseReduction.
	  glare removal - TGlareReduction.

	  	Example 
		
		@code
		// Lets assume that an application would need to check whether gamma correction is 
		// supported on a particular platform. After obtaining a valid pointer to the interface, 
		// it would call GetSupportedTransformationsL() to obtain the list of the supported 
		// transformations and check whether KUidECamEventImageProcessingAdjustGamma
		// is in the list. If it is then call SetTranformationValue(KUidECamEventImageProcessingAdjustGamma, 200);
		// to set the new value. A notification will be generated to the client to indicate success.

		RArray<TUid> suppTransforms; // array of supported transformations
		CleanupClosePushL(suppTransforms);

		imageProcess->GetSupportedTransformationsL(suppTransfUids);
		 ...
		// if the gamma is supported 
		TInt gammaCorrection = 200; // gamma correction of 2.0
		imageProcess->SetTranformationValue(KUidECamEventImageProcessingAdjustGamma, gammaCorrection);
		...
		// pop stack to close the RArray
		@endcode
		
@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedAll
@released	
*/
class CCamera::CCameraImageProcessing : public CBase
	{
	
	// so can use internal factory functions etc.
	friend class CCamera;
	
public: 
	
	/** Gamma settings */		
   	enum TGamma
	   	{
   		/** Sets the gamma adjustment automatically. */
	   	EGammaAuto						= KMinTInt
	   	};
	/** Sharpness Settings */	   	
	enum TSharpness
	   	{
   		/** Sets the sharpness adjustment automatically. */
	   	ESharpnessAuto					= KMinTInt
	   	};   	
	/** Saturation Settings */	 
	enum TSaturation
	   	{
   		/** Sets the saturation adjustment automatically. */
	   	ESaturationAuto					= KMinTInt
	   	};
	/** Hue Settings */	 
	enum THue
	   	{
   		/** Sets the hue adjustment automatically. */
	   	EHueAuto						= KMinTInt
	   	};	   	
	   	     	
	/** Settings for the supported effects */
	enum TEffect
	  	{
		/** Effects off is default, always supported */
		EEffectNone			     	= 0x00000000,
		/** Negative visual effect. */
		EEffectNegative		   		= 0x00000001,
		/** Black and white visual effect. */
		EEffectMonochrome 			= 0x00000002,
		/** Solarization visual effect. */
		EEffectSolarize		 		= 0x00000004,
		/** Sepia visual effect. */
		EEffectSepia		     	= 0x00000008,
		/** Emboss visual effect. */
		EEffectEmboss 				= 0x00000010,
		/** Red eye glare reduction visual effect. */
		EEffectRedEyeReduction 		= 0x00000020,
		/** Vivid colour visual effect. */ 
		EEffectVivid 				= 0x00000040,
		/** Neutral colour visual effect. */ 
		EEffectNeutral 				= 0x00000080,
		/** Selective sharpening visual effect. */ 
		EEffectLowSharpening		= 0x00000100,
		/** 
		Natural visual effect. 
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/
		EEffectNatural 				= 0x00000200, 
		/** 
		Cine visual effect. 
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/
		EEffectCine 				= 0x00000400, 
		/** 
		Old film visual effect. 
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/
		EEffectOldFilm 				= 0x00000800,  
		/** 
		Color swap visual effect. 
		
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
		
		*/
		EEffectColorSwap 			= 0x00001000, 
		/** 
		Where a list of color can be kept as they are and the rest converted to gray. 
	
		This enum value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
		CCamera::NewDuplicate2L().
	
		*/
		EEffectColorAccent			= 0x00002000
 		};
		
	/** Mirror settings */		
	enum TMirror
		{
		/** Default value. Mirroring is disabled. */
		EMirrorNone			= 0x00000000, 
		/** Mirror along horizontal axis. */
		EMirrorHorizontal	= 0x00000001,
		/** Mirror along vertical axis. */
		EMirrorVertical		= 0x00000002,
		/** Mirror along horizontal and vertical axis. */
		EMirrorBoth			= 0x00000003
		};
	
	/** Noise filter settings */		
	enum TNoiseReduction
		{
		/** Default value. Noise reduction disabled. */
		ENoiseReductionNone			= 0x0000, 
		/** most basic noise reduction. */
		ENoiseReductionBasic		= 0x0001
		};
	
	/** Glare reduction settings */		
	enum TGlareReduction
		{
		/** Default value. Glare reduction disabled. */
		EGlareReductionNone			= 0x0000, 
		/** most basic glare reduction. */
		EGlareReductionBasic		= 0x0001
		};
	
	/** 
	Used for color swap and color accent feature in TEffect. It describes whether the given source 
	or target color represents itself or a group of color.
	
	*/
	enum TColorOperationRepresentation
		{
		/** Feature is not supported */
		ERepresentationNone         = 0x00,
		/** The given color is uniquely represented */
		ERepresentationSingle	    = 0x01,
		/** The given color represents a group of colors */
		ERepresentationMultiple     = 0x02
		};

	/**
	Used for color swap and color accent feature in TEffect. It describes the type of grouping for 
	source and target colors when they represent a group of color.
	
	*/
	enum TColorOperationRgbGroupingMode
		{
		/** Feature is not supported */
		ERgbGroupingNone			= 0x00,
		/** This means no neighbourhood colors are considered */
		ERgbGroupingFixed			= 0x01,
		/** This means neighbourhood colors are to be automatically decided by the implementation or hardware */
		ERgbGroupingAutomatic		= 0x02,
		/** This means neighbourhood colors for source have to be decided by ignoring few starting bits. Used for source only */
		ERgbGroupingSignificantBits = 0x04,
		/** This means colors in target are obtained as some uniform proportion using source. Used for target only */
		ERgbGroupingUniform			= 0x08
		};
	
	/**
	Used to provide different types of references for orientation of images retrieved. These references are the various 
	means by the orientation of an image can be defined. Client needs to choose a particular TOrientationReference and 
	specify any relative rotation, mirroring and flipping on top of it.	
	
	This enumeration has nothing to do with absolute rotation. Uid KUidECamEventImageProcessingTransformRotate is used 
	for absolute rotation purposes.
	
	The enumeration list may be extended in future.	
	
	@publishedPartner
	@prototype
	*/
	enum TOrientationReference
		{
		/** The retrieved images will follow the camera sensor orientation. This orientation would be relative 
		to the current screen mode as given by the reference screen. */
		EOrientationReferenceCameraSensorOrientation,
	
		/** The retrieved images will get oriented as per the tilt sensor information. 
		So,the orientation would be relative to the current screen mode as given by the reference screen. */
		EOrientationReferenceTiltOrientation
		};
	
	/**
	Used to describe the various relative rotation. The term 'relative' means that the rotation is applied after the 
	image undergoes orientation as given by TOrientationReference.
	
	The enumeration list may be extended in future.	
	
	@publishedPartner
	@prototype
	*/
	enum TRelativeRotation
		{
		/** Zero degree Rotation applied on top of orientation reference and is relative to current reference screen. */
		ERelativeRotation0Degrees				= 0x00,
		/** 90 degree clockwise Rotation applied on top of orientation reference and is relative to current reference screen. */
		ERelativeRotation90DegreesClockwise 	= 0x01,
		/** 180 degree clockwise Rotation applied on top of orientation reference and is relative to current reference screen. */
		ERelativeRotation180DegreesClockwise	= 0x02,
		/** 270 degree clockwise Rotation applied on top of orientation reference and is relative to current reference screen. */
		ERelativeRotation270DegreesClockwise 	= 0x04
		};
		
	/**
	Used to describe the various relative mirroring. The term 'relative' means that the mirroring is applied after the 
	image undergoes orientation as given by TOrientationReference.
		
	The enumeration list may be extended in future.	
	
	@publishedPartner
	@prototype
	*/
	enum TRelativeMirror
		{
		/** No mirroring. */
		ERelativeMirrorNone				= 0x00,
		/** Used to mirror an image about the horizontal axis. This is applied on top of orientation reference and is 
		relative to current reference screen. */
		ERelativeMirrorHorizontalAxis	= 0x10,
		/** Used to mirror an image about the vertical axis. This is applied on top of orientation reference and is 
		relative to current reference screen. */
		ERelativeMirrorVerticalAxis 	= 0x20 
		};
		
	/**
	Used to describe the various relative flipping. The term 'relative' means that the flipping is applied after the 
	image undergoes orientation as given by TOrientationReference.
	
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/
	enum TRelativeFlipping
		{
		/** No flipping. */
		ERelativeFlippingNone			= 0x00,
		/** Used to flip an image horizontally. This is applied on top of orientation reference and is relative to current
		reference screen. */
		ERelativeFlippingHorizontal		= 0x10,
		/** Used to flip an image vertically. This is applied on top of orientation reference and is relative to current
		reference screen. */
		ERelativeFlippingVertical 		= 0x20 
		};
		
	/**
	Different types of contrast enhancement options.
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/	
	enum TContrastEnhancement
		{
		/** Not Supported. */
		EContrastEnhancementNone				=0x00,
		/** Contrast Enhancement switched off. */
		EContrastEnhancementSwitchOff			=0x01,
		/** Contrast Enhacement switched On manually. */
		EContrastEnhancementManualSwitchOn		=0x02,
		/** Contrast Enhancement in auto mode. */
		EContrastEnhancementSwitchAuto			=0x04
		};
		
	/**
	Different types of software-based red eye reduction options.
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/
	enum TRedEyeReduction
		{
		/** Not Supported. */
		ERedEyeReductionNone				=0x00,
		/** Software based Red Eye Reduction can be switched off(disabled). */
		ERedEyeReductionSwitchOff			=0x01,
		/** Software based Red Eye Reduction can be switched on (enabled). */
		ERedEyeReductionManualSwitchOn		=0x02,
		/** Software based Red Eye Reduction can be put to auto mode. */
		ERedEyeReductionSwitchAuto			=0x04
		};

	/** 
	Color operation capabilities characterizing both color swap and color accent feature 
	
	*/
	class TColorOperationCapabilities
		{
	public:
		/**
		Constructor.
		Sets the size of this class in iSize.
		*/
		IMPORT_C TColorOperationCapabilities();
		
		IMPORT_C TInt Size() const;
		IMPORT_C TUint Version() const;
	
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt    iReserved;
		
	public:
		/** The source color representation that the server supports.
	    The supported representations are held as a bitwise logical OR of the relevant individual modes
	    defined in CCamera::CCameraImageProcessing:: TColorOperationRepresentation. */
		TUint	iSupportedSourceRepresentation;
		
		/** The target color representation that the server supports.
	    The supported representations are held as a bitwise logical OR of the relevant individual modes
	    defined in CCamera::CCameraImageProcessing:: TColorOperationRepresentation.
	    Not applicable for color accent */
		TUint	iSupportedTargetRepresentation;

		/** The source color Rgb grouping modes that the server supports.
	    The modes are held as a bitwise logical OR of the relevant individual modes
	    defined in CCamera::CCameraImageProcessing:: TColorOperationRgbGroupingMode. */
		TUint   iSupportedSourceRgbGroupingMode;

		/** The target color Rgb grouping modes that the server supports.
	    The modes are held as a bitwise logical OR of the relevant individual modes
	    defined in CCamera::CCameraImageProcessing:: TColorOperationRgbGroupingMode. 
	    Not applicable for color accent */
		TUint   iSupportedTargetRgbGroupingMode;
		
		/** This indicates whether the mapping mode and Rgb grouping mode uniform for every color entry */
		TBool   iIsCapabilityUniform;
		};

	/** 
	Class used to control the number of bits to ignore for Red, Green, Blue or Alpha.
	This may be used when deciding for neighbourhood for source color in case of color swap and color accent feature

	*/
	class TBitsIgnore
		{
	public:
		/**
		Constructor.
		Sets the size of this class in iSize.
		*/
		IMPORT_C TBitsIgnore();
		
		IMPORT_C TInt Size() const;
		IMPORT_C TUint Version() const;
	
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// for future use
		TUint iReservedBits:16;
			
	public:
		/** This gives the no. of bits to ignore for Red color. Varies from 0 to 8. 
		Any other value should be an error */
		TUint iRedBitsIgnore:4;
		
		/** This gives the no. of bits to ignore for Green color. Varies from 0 to 8. 
		Any other value should be an error */
		TUint iGreenBitsIgnore:4;
		
		/** This gives the no. of bits to ignore for Blue color. Varies from 0 to 8. 
		Any other value should be an error */
		TUint iBlueBitsIgnore:4;

		/** This gives the no. of bits to ignore for Alpha. Varies from 0 to 8. 
		Any other value should be an error */
		TUint iAlphaBitsIgnore:4;
		};

	/** 
	Color operation parameters characterizing both color swap and color accent feature; and for a particular entry.
	 
	*/
	class TColorOperationEntry
		{
	public:
		/**
		Constructor.
		Sets the size of this class in iSize.
		*/
		IMPORT_C TColorOperationEntry();
				
		IMPORT_C TInt Size() const;
		IMPORT_C TUint Version() const;

	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt iReserved;
		
	public:
		/** The source color (rgb triplet) which is changed to target color in case of Color Swap operation 
		and remains unaffected in case of Color Accent operation */
		TRgb iSourceColor;

		/** The target color (rgb triplet) which replaces the source color in case of Color Swap operation. 
		Redundant for Color Accent operation */
		TRgb iTargetColor;

		/** This defines whether the given source color represents itself or a group of colors */
		TColorOperationRepresentation iSourceColorRepresentation;

		/** This defines whether the given target color represents itself or a group of colors in case of 
		Color Swap operation. Redundant for Color Accent operation */
		TColorOperationRepresentation iTargetColorRepresentation;

		/** This defines the type of grouping for multiple representation of source color */
		TColorOperationRgbGroupingMode iColorOperationSourceRgbGroupingMode;

		/** This defines the type of grouping for multiple representation of target color in case of 
		Color Swap operation. Redundant for Color Accent operation */
		TColorOperationRgbGroupingMode iColorOperationTargetRgbGroupingMode;  

		/** This gives the number of bits to ignore in case ERgbGroupingSignificantBits is used for 
		multiple representation of source color. For any other case, no. of bits ignored for red, green, 
		blue or alpha should be 0 */
		TBitsIgnore iNumBitsIgnored;

		/** This gives the entry status of every color operation entry. If entry is not set or removed, 
		this is ENotActive. And if entry is set, then this is EDiscreteSteps */
		TValueInfo iEntryStatus;
		};

public:
	
    IMPORT_C static CCameraImageProcessing* NewL(CCamera& aCamera);
    
    IMPORT_C static CCameraImageProcessing* CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory);
    
	IMPORT_C void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const;

	IMPORT_C void GetActiveTransformationsL(RArray<TUid>& aTransformations) const;

	IMPORT_C void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const;

	IMPORT_C TInt TransformationValue(TUid aTransformation) const;
	
	IMPORT_C TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const;

	IMPORT_C void SetTransformationValue(TUid aTransformation, TInt aValue);

	IMPORT_C void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const;

	IMPORT_C void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence);

	IMPORT_C void SetSourceRect(const TRect& aRect);

	IMPORT_C void GetSourceRect(TRect& aRect) const;

	IMPORT_C ~CCameraImageProcessing();
	
	IMPORT_C void GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const; 
	
	IMPORT_C void GetColorSwapCapabilitiesL(TInt aIndex, TColorOperationCapabilities& aColorSwapCapabilities) const;
	
	IMPORT_C void SetColorSwapEntryL(TInt aIndex, const TColorOperationEntry& aColorSwapParameters); 
	
	IMPORT_C void RemoveColorSwapEntryL(TInt aIndex);
	
	IMPORT_C void GetColorSwapEntryL(TInt aIndex, TColorOperationEntry& aColorSwapParameters) const; 
	
	IMPORT_C void StartColorSwappingL();
	
	IMPORT_C void CancelColorSwappingL();
	
	IMPORT_C void GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const; 
	
	IMPORT_C void GetColorAccentCapabilitiesL(TInt aIndex, TColorOperationCapabilities& aColorAccentCapabilities) const;
	
	IMPORT_C void SetColorAccentEntryL(TInt aIndex, const TColorOperationEntry& aColorAccentParameters); 
	
	IMPORT_C void RemoveColorAccentEntryL(TInt aIndex);
	
	IMPORT_C void GetColorAccentEntryL(TInt aIndex, TColorOperationEntry& aColorAccentParameters) const; 
	
	IMPORT_C void StartColorAccentL();
	
	IMPORT_C void CancelColorAccentL();
	
	IMPORT_C void GetSupportedRelativeOrientationOptionsL(TOrientationReference aOrientationReference, 
		TUint& aSupportedRelativeRotation, TUint& aSupportedRelativeMirroring, TUint& aSupportedRelativeFlipping) const;
																
	IMPORT_C void GetCurrentRelativeOrientationOptionsL(TOrientationReference& aOrientationReference, 
		TRelativeRotation& aRelativeRotation, TRelativeMirror& aRelativeMirror, TRelativeFlipping& aRelativeFlipping) const;
												
	IMPORT_C void SetRelativeOrientationOptionsL(TOrientationReference aOrientationReference, 
		TRelativeRotation aRelativeRotation, TRelativeMirror aRelativeMirror, TRelativeFlipping aRelativeFlipping);
	
private:
	IMPORT_C CCameraImageProcessing(CCamera& aOwner);
	IMPORT_C void ConstructL();
	IMPORT_C void ConstructL(const MImplementationFactory& aImplFactory);
	
private:
	CCamera& iOwner;
	MCameraImageProcessing* iImpl;  // not owned
	MCameraImageProcessing2* iImpl2;   //not owned
	MCameraImageProcessing3* iImpl3;   //not owned
	};
#endif//ECAMIMAGEPROCESSING_H



