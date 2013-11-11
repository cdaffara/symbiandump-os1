/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Exif file format creator/modifier class
*
*/


#ifndef EXIFMODIFY_H
#define EXIFMODIFY_H

// INCLUDES
#include <e32base.h>
#include "ExifRead.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TExifTagInfo;

// CLASS DECLARATION

/**
*  CExifModify
*  Interface class for modifying existing Exif v2.2 (or prior) file format or 
*  creating Exif v2.2 file format using valid Jpeg image. 
*  An instance of this class can be instantiated in one of two ways:
*  by providing valid Exif data or by providing valid Jpeg image.
* 
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifModify ): public CBase
    {
	public:  // Enumerations

        /**
        *  The operation mode enumeration specifying the create or modify modes.
        */
    	enum TOperationMode
            {
        	EModify = 0,
        	ECreate
            };
            
        public:
            enum TExifModifyOption
            {
            ENoOptions        = 0x0000,
            ENoJpegParsing    = 0x0001, // No jpeg validity checking is done
            ENoTagChecking    = 0x0002  // Ignore unknown EXIF tags and mandatory tag checking
            };

	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
    	IMPORT_C static CExifModify* NewL( 
            const TDesC8& aInData, 
            CExifModify::TOperationMode aOperationMode = EModify );

    	IMPORT_C static CExifModify* NewL( 
            const TDesC8& aInData, 
            CExifModify::TOperationMode aOperationMode,
            TUint aExifModifyOption );


        /**
        * Two-phased constructor.
        */
    	IMPORT_C static CExifModify* NewL();
            
        /**
        * Destructor.
        */
    	virtual ~CExifModify();

	public: // New functions
        
        /**
        * Returns a constant pointer to a CExifRead instance that can be used 
        * to parse the associated Exif image.
        * @since 2.6 
        * @return Unmodifiable pointer to Exif reader instance.
        */
    	virtual const CExifRead* Reader() const = 0;

        /**
        * Inserts/Updates the given tag in the specified IFD structure of the 
        * Exif data.
        * @since 2.6 
        * @param aIfdType The hosting IFD type.
        * @param aExifTagInfo The informative fields of tag that is updated.
        * @param aExifData Data of the tag that is updated.
        * @return void
        */
    	virtual void SetTagL( 
            TExifIfdType aIfdType, 
            TExifTagInfo aExifTagInfo, 
            const TDesC8& aTagData ) = 0;

        /**
        * Removes the tag with the given tag ID from the specified IFD 
        * structure in the Exif data.
        * @since 2.6 
        * @param aIfdType The hosting IFD type.
        * @param aTagId The target tag ID.
        * @return Error code.
        */
    	virtual TInt DeleteTag( TExifIfdType aIfdType, TUint16 aTagId ) = 0;

        /**
        * Removes the specified IFD structure and all its tags from the Exif 
        * data.
        * @since 2.6 
        * @param aIfdType The target IFD type.
        * @return Error code.
        */
    	virtual TInt DeleteIfd( TExifIfdType aIfdType ) = 0;

        /**
        * Inserts/Updates the given thumbnail Jpeg image data into the 1st IFD 
        * structure in the Exif data.
        * @since 2.6 
        * @param aThumbnailData The Exif thumbnail image data that is updated.
        * @return void
        */
    	virtual void SetThumbnailL( const TDesC8& aThumbnailData ) = 0;
        
        /**
        * Removes the thumbnail Jpeg image data from the 1st IFD structure in 
        * the Exif data.
        * @since 2.6 
        * @return Error code.
        */
    	virtual TInt RemoveThumbnail() = 0;
        
        /**
        * Flushes the Exif data into the given data buffer, and releases the 
        * internal structures.
        * @since 2.6 
        * @param aInData The original Exif image data, which contains identical 
        * data provided while calling the NewL function.
        * @return Pointer to descriptor containing new Exif image data.
        */
    	virtual HBufC8* WriteDataL( const TDesC8& aInData ) = 0;

        /**
        * Inserts/Updates given Image Description in the Exif data.
        * @since 2.6
        * @param aImageDescription Updated Image Description data.
        * @return void
        */
    	virtual void SetImageDescriptionL( const TDesC8& aImageDescription ) = 0;

        /**
        * Inserts/Updates given Make in the Exif data.
        * @since 2.6
        * @param aMake Updated Make data.
        * @return void
        */
        virtual void SetMakeL( const TDesC8& aMake ) = 0; 
    	
        /**
        * Inserts/Updates given Model in the Exif data.
        * @since 2.6
        * @param aModel Updated Model data.
        * @return void
        */        
        virtual void SetModelL( const TDesC8& aModel ) = 0; 
    	
        /**
        * Inserts/Updates given Orientation in the Exif data.
        * @since 2.6
        * @param aOrientation Updated Orientation data.
        * @return void
        */
        virtual void SetOrientationL( TUint16 aOrientation ) = 0; 
    	
        /**
        * Inserts/Updates given X Resolution in the Exif data.
        * @since 2.6
        * @param aXResolution1 Updated X Resolution numerator.
        * @param aXResolution2 Updated X Resolution denominator.
        * @return void
        */        
        virtual void SetXResolutionL( 
            TUint32 aXResolution1, 
            TUint32 aXResolution2 ) = 0; 
    	
        /**
        * Inserts/Updates given Y Resolution in the Exif data.
        * @since 2.6
        * @param aYResolution1 Updated Y Resolution numerator.
        * @param aYResolution2 Updated Y Resolution denominator.
        * @return void
        */        
        virtual void SetYResolutionL( 
            TUint32 aYResolution1, 
            TUint32 aYResolution2 ) = 0; 
    	
        /**
        * Inserts/Updates given Resolution Unit in the Exif data.
        * @since 2.6
        * @param aResolutionUnit Updated Resolution Unit data.
        * @return void
        */        
        virtual void SetResolutionUnitL( TUint16 aResolutionUnit ) = 0; 
    	
        /**
        * Inserts/Updates given Transfer Function in the Exif data.
        * @since 2.6
        * @param aTransferFunction Updated Transfer Function data.
        * @return void
        */        
        virtual void SetTransferFunctionL( 
            const TDesC8& aTransferFunction ) = 0; 
    	
        /**
        * Inserts/Updates given Date Time in the Exif data.
        * @since 2.6
        * @param aDateTime Updated Date Time data.
        * @return void
        */        
        virtual void SetDateTimeL( const TDesC8& aDateTime ) = 0; 
    	
        /**
        * Inserts/Updates given YCbCr Positioning in the Exif data.
        * @since 2.6
        * @param aYCbCrPositioning YCbCr Positioning data.
        * @return void
        */        
        virtual void SetYCbCrPositioningL( TUint16 aYCbCrPositioning) = 0; 
    	
        /**
        * Inserts/Updates given Software in the Exif data.
        * @since 2.6
        * @param aSoftware Updated Software data.
        * @return void
        */        
        virtual void SetSoftwareL( const TDesC8& aSoftware ) = 0; 
    	
        /**
        * Inserts/Updates given Copyright in the Exif data.
        * @since 2.6
        * @param aCopyright Updated Copyright data.
        * @return void
        */        
        virtual void SetCopyrightL( const TDesC8& aCopyright ) = 0;

        /**
        * Inserts/Updates given Exposure Time in the Exif data.
        * @since 2.6
        * @param aExposureTime1 Updated Exposure Time numerator.
        * @param aExposureTime2 Updated Exposure Time denominator.
        * @return void
        */
    	virtual void SetExposureTimeL( 
            TUint32 aExposureTime1, 
            TUint32 aExposureTime2 ) = 0;

        /**
        * Inserts/Updates given Components Configuration in the Exif data.
        * @since 2.6
        * @param aComponentsConfiguration Updated Components Configuration data.
        * @return void
        */    	
        virtual void SetComponentsConfigurationL( 
            TUint8 aFirstComponent, TUint8 aSecondComponent, 
            TUint8 aThirdComponent, TUint8 aFourthComponent ) = 0; 
    	
        /**
        * Inserts/Updates given Flash in the Exif data.
        * @since 2.6
        * @param aFlash Updated Flash data.
        * @return void
        */        
        virtual void SetFlashL( TUint16 aFlash ) = 0; 
    	
        /**
        * Inserts/Updates given Color Space in the Exif data.
        * @since 2.6
        * @param aColorSpace Updated Color Space data.
        * @return void
        */        
        virtual void SetColorSpaceL( TUint16 aColorSpace ) = 0;
    	
        /**
        * Inserts/Updates given Pixel X Dimension in the Exif data.
        * @since 2.6
        * @param aPixelXDimension Updated Pixel X Dimension data.
        * @return void
        */        
        virtual void SetPixelXDimensionL( TUint32 aPixelXDimension ) = 0;
    	
        /**
        * Inserts/Updates given Pixel Y Dimension in the Exif data.
        * @since 2.6
        * @param aPixelYDimension Updated Pixel Y Dimension data.
        * @return void
        */        
        virtual void SetPixelYDimensionL( TUint32 aPixelYDimension ) = 0;
    	
        /**
        * Inserts/Updates given Exposure Mode in the Exif data.
        * @since 2.6
        * @param aExposureMode Updated Exposure Mode data.
        * @return void
        */        
        virtual void SetExposureModeL( TUint16 aExposureMode ) = 0;
    	
        /**
        * Inserts/Updates given White Balance in the Exif data.
        * @since 2.6
        * @param aWhiteBalance Updated White Balance data.
        * @return void
        */        
        virtual void SetWhiteBalanceL( TUint16 aWhiteBalance ) = 0; 
    	
        /**
        * Inserts/Updates given Scene Capture Type in the Exif data.
        * @since 2.6
        * @param aSceneCaptureType Updated Scene Capture Type data.
        * @return void
        */        
        virtual void SetSceneCaptureTypeL( TUint16 aSceneCaptureType ) = 0; 
    	
        
        /**
        * Inserts/Updates given Exposure Program in the Exif data.
        * @since 2.6
        * @param aExposureProgram Updated Exposure Program data.
        * @return void
        */virtual void SetExposureProgramL( TUint16 aExposureProgram ) = 0; 
    	
        /**
        * Inserts/Updates given Iso Speed Ratings in the Exif data.
        * @since 2.6
        * @param aIsoSpeedRatings Updated Iso Speed Ratings data.
        * @return void
        */        
        virtual void SetIsoSpeedRatingsL( const TDesC8& aIsoSpeedRatings ) = 0; 
    	
        /**
        * Inserts/Updates given Date Time Original in the Exif data.
        * @since 2.6
        * @param aDateTimeOriginal Updated Date Time Original data.
        * @return void
        */        
        virtual void SetDateTimeOriginalL( 
            const TDesC8& aDateTimeOriginal ) = 0;
    	
        /**
        * Inserts/Updates given Date Time Digitized in the Exif data.
        * @since 2.6
        * @param aDateTimeDigitized Updated Date Time Digitized data.
        * @return void
        */        
        virtual void SetDateTimeDigitizedL( 
            const TDesC8& aDateTimeDigitized ) = 0;
    	
        /**
        * Inserts/Updates given Aperture Value in the Exif data.
        * @since 2.6
        * @param aApertureValue1 Updated Aperture Value numerator.
        * @param aApertureValue2 Updated Aperture Value denominator.
        * @return void
        */        
        virtual void SetApertureValueL( 
            TUint32 aApertureValue1, 
            TUint32 aApertureValue2 ) = 0; 
    	
        /**
        * Inserts/Updates given  Exposure Bias Value in the Exif data.
        * @since 2.6
        * @param aExposureBiasValue1 Updated Exposure Bias Value numerator.
        * @param aExposureBiasValue2 Updated Exposure Bias Value denominator.
        * @return void
        */        
        virtual void SetExposureBiasValueL( 
            TInt32 aExposureBiasValue1, 
            TInt32 aExposureBiasValue2 ) = 0;
    	
        /**
        * Inserts/Updates given Metering Mode in the Exif data.
        * @since 2.6
        * @param aMeteringMode Updated Metering Mode data.
        * @return void
        */        
        virtual void SetMeteringModeL( TUint16 aMeteringMode ) = 0; 
    	
        /**
        * Inserts/Updates given Light Source in the Exif data.
        * @since 2.6
        * @param aLightSource Updated Light Source data.
        * @return void
        */        
        virtual void SetLightSourceL( TUint16 aLightSource ) = 0; 
    	
        /**
        * Inserts/Updates given Maker Note in the Exif data.
        * @since 2.6
        * @param aMakerNote Updated Maker Note data.
        * @return void
        */        
        virtual void SetMakerNoteL( const TDesC8& aMakerNote ) = 0; 
    	
        /**
        * Inserts/Updates given User Comment in the Exif data.
        * @since 2.6
        * @param aUserComment Updated User Comment data.
        * @return void
        */        
        virtual void SetUserCommentL( const TDesC8& aUserComment ) = 0;
    	
        /**
        * Inserts/Updates given Related Sound File in the Exif data.
        * @since 2.6
        * @param aRelatedSoundFile Updated Related Sound File data.
        * @return void
        */        
        virtual void SetRelatedSoundFileL( 
            const TDesC8& aRelatedSoundFile ) = 0; 
    	
        /**
        * Inserts/Updates given File Source in the Exif data.
        * @since 2.6
        * @param aFileSource Updated File Source data.
        * @return void
        */        
        virtual void SetFileSourceL( TInt8 aFileSource ) = 0; 
    	
        /**
        * Inserts/Updates given Digital Zoom Ratio in the Exif data.
        * @since 2.6
        * @param aDigitalZoomRatio1 Updated Digital Zoom Ratio numerator.
        * @param aDigitalZoomRatio2 Updated Digital Zoom Ratio denominator.
        * @return void
        */        
        virtual void SetDigitalZoomRatioL( 
            TUint32 aDigitalZoomRatio1, 
            TUint32 aDigitalZoomRatio2 ) = 0; 
    	
        /**
        * Inserts/Updates given Contrast in the Exif data.
        * @since 2.6
        * @param aContrast Updated Contrast data.
        * @return void
        */        
        virtual void SetContrastL( TUint16 aContrast ) = 0; 
    	
        /**
        * Inserts/Updates given Saturation in the Exif data.
        * @since 2.6
        * @param aSaturation Updated Saturation data.
        * @return void
        */        
        virtual void SetSaturationL( TUint16 aSaturation ) = 0; 
    	
        /**
        * Inserts/Updates given Sharpness in the Exif data.
        * @since 2.6
        * @param aSharpness Updated Sharpness data.
        * @return void
        */        
        virtual void SetSharpnessL( TUint16 aSharpness ) = 0; 

        /**
        * Inserts/Updates given thumbnail X Resolution in the Exif data.
        * @since 2.6
        * @param aXResolution1 Updated thumbnail X Resolution numerator.
        * @param aXResolution2 Updated thumbnail X Resolution denominator.
        * @return void
        */
    	virtual void SetThumbnailXResolutionL( 
            TUint32 aXResolution1, 
            TUint32 aXResolution2 ) = 0;
    	
        /**
        * Inserts/Updates given thumbnail Y Resolution in the Exif data.
        * @since 2.6
        * @param aYResolution1 Updated thumbnail Y Resolution numerator.
        * @param aYResolution2 Updated thumbnail Y Resolution denominator.
        * @return void
        */
        virtual void SetThumbnailYResolutionL( 
            TUint32 aYResolution1, 
            TUint32 aYResolution2 ) = 0;
    	
        /**
        * Inserts/Updates given thumbnail Resolution Unit in the Exif data.
        * @since 2.6
        * @param aResolutionUnit Updated thumbnail Resolution Unit data.
        * @return void
        */        
        virtual void SetThumbnailResolutionUnitL( TUint16 aResolutionUnit ) = 0;

        /**
        * Inserts/Updates given Shutter Speed Value in the Exif data.
        * @since 2.6
        * @param aShutterSpeedValue1 Updated Shutter Speed Value numerator.
        * @param aShutterSpeedValue2 Updated Shutter Speed Value denominator.
        * @return void
        */
    	virtual void SetShutterSpeedValueL( 
            TInt32 aShutterSpeedValue1, 
            TInt32 aShutterSpeedValue2 ) = 0;

        /**
        * Inserts/Updates given Brightness Value in the Exif data.
        * @since 2.6
        * @param aBrightnessValue1 Updated Brightness Value numerator.
        * @param aBrightnessValue2 Updated Brightness Value denominator.
        * @return void
        */
    	virtual void SetBrightnessValueL( 
            TInt32 aBrightnessValue1, 
            TInt32 aBrightnessValue2 ) = 0;

        /**
        * Inserts/Updates given Custom Rendered in the Exif data.
        * @since 2.6
        * @param aCustomRendered Updated Custom Rendered data.
        * @return void
        */        
        virtual void SetCustomRenderedL( TUint16 aCustomRendered ) = 0; 

        /**
        * Inserts/Updates given Gain Control in the Exif data.
        * @since 2.6
        * @param aGainControl Updated Gain Control data.
        * @return void
        */        
        virtual void SetGainControlL( TUint16 aGainControl ) = 0; 

    }; 

#endif // EXIFMODIFY_H

// End of File
