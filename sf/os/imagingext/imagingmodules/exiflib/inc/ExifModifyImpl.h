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
* Description:  Exif file format creator/modifier class implementation
*
*/


#ifndef EXIFMODIFYIMPL_H
#define EXIFMODIFYIMPL_H

// INCLUDES
#include "ExifModify.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CExifReadImpl;
class CExifCore;

/**
* CExifModify
* Implementation class for modifying existing Exif v2.2 (or prior) file 
* format or creating Exif v2.2 file format using valid Jpeg image. 
* An instance of this class can be instantiated in one of two ways:
* by providing valid Exif data or by providing valid Jpeg image.
* 
* @lib ExifLib.lib
* @since 2.6 
*/
NONSHARABLE_CLASS( CExifModifyImpl ): public CExifModify
    {
    public:  // Constructors and destructor
 
        /**
        * Two-phased constructor.
        */
    	static CExifModifyImpl* NewL( const TDesC8& aExifData, TBool aCreate, TUint aExifModifyOption );

        /**
        * Two-phased constructor.
        */
    	static CExifModifyImpl* NewL();

        /**
        * Destructor.
        */
        virtual ~CExifModifyImpl();

    protected:  // Functions from base classes
        
        /**
        * From CExifModify. Returns a constant pointer to a CExifRead instance 
        * that can be used to parse the associated Exif image.
        * @since 2.6 
        * @return Unmodifiable pointer to Exif reader instance.
        */
    	const CExifRead* Reader() const;

        /**
        * From CExifModify. Inserts/Updates the given tag in the specified IFD 
        * structure of the Exif data.
        * @since 2.6 
        * @param aIfdType The hosting IFD type.
        * @param aExifTagInfo The informative fields of tag that is updated.
        * @param aExifData Data of the tag that is updated.
        * @return void
        */
    	void SetTagL( 
            TExifIfdType aIfdType, 
            TExifTagInfo aExifTagInfo, 
            const TDesC8& aTagData );

        /**
        * From CExifModify. Removes the tag with the given tag ID from the 
        * specified IFD structure in the Exif data.
        * @since 2.6 
        * @param aIfdType The hosting IFD type.
        * @param aTagId The target tag ID.
        * @return Error code.
        */
    	TInt DeleteTag( TExifIfdType aIfdType, TUint16 aTagId );

        /**
        * From CExifModify. Removes the specified IFD structure and all its tags 
        * from the Exif data.
        * @since 2.6 
        * @param aIfdType The target IFD type.
        * @return Error code.
        */
    	TInt DeleteIfd( TExifIfdType aIfdType );

        /**
        * From CExifModify. Inserts/Updates the given thumbnail Jpeg image data 
        * into the 1st IFD structure in the Exif data.
        * @since 2.6 
        * @param aThumbnailData The Exif thumbnail image data that is updated.
        * @return void
        */
    	void SetThumbnailL( const TDesC8& aThumbnailData );
        
        /**
        * From CExifModify. Removes the thumbnail Jpeg image data from the 1st 
        * IFD structure in the Exif data.
        * @since 2.6 
        * @return Error code.
        */
    	TInt RemoveThumbnail();
        
        /**
        * From CExifModify. Flushes the Exif data into the given data buffer, 
        * and releases the internal structures.
        * @since 2.6 
        * @param aInData The original Exif image data, which contains identical 
        *                data provided while calling the NewL function.
        * @return Pointer to descriptor containing new Exif image data.
        */
    	HBufC8* WriteDataL( const TDesC8& aInData );

        /**
        * From CExifModify. Inserts/Updates given Image Description in the Exif 
        * data.
        * @since 2.6
        * @param aImageDescription Updated Image Description data.
        * @return void
        */
    	void SetImageDescriptionL( const TDesC8& aImageDescription );

        /**
        * From CExifModify. Inserts/Updates given Make in the Exif data.
        * @since 2.6
        * @param aMake Updated Make data.
        * @return void
        */
        void SetMakeL( const TDesC8& aMake ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Model in the Exif data.
        * @since 2.6
        * @param aModel Updated Model data.
        * @return void
        */        
        void SetModelL( const TDesC8& aModel ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Orientation in the Exif data.
        * @since 2.6
        * @param aOrientation Updated Orientation data.
        * @return void
        */
        void SetOrientationL( TUint16 aOrientation ); 
    	
        /**
        * From CExifModify. Inserts/Updates given X Resolution in the Exif data.
        * @since 2.6
        * @param aXResolution1 Updated X Resolution numerator.
        * @param aXResolution2 Updated X Resolution denominator.
        * @return void
        */        
        void SetXResolutionL( TUint32 aXResolution1, TUint32 aXResolution2 ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Y Resolution in the Exif data.
        * @since 2.6
        * @param aYResolution1 Updated Y Resolution numerator.
        * @param aYResolution2 Updated Y Resolution denominator.
        * @return void
        */        
        void SetYResolutionL( TUint32 aYResolution1, TUint32 aYResolution2 ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Resolution Unit in the Exif 
        * data.
        * @since 2.6
        * @param aResolutionUnit Updated Resolution Unit data.
        * @return void
        */        
        void SetResolutionUnitL( TUint16 aResolutionUnit ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Transfer Function in the Exif 
        * data.
        * @since 2.6
        * @param aTransferFunction Updated Transfer Function data.
        * @return void
        */        
        void SetTransferFunctionL( const TDesC8& aTransferFunction ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Date Time in the Exif data.
        * @since 2.6
        * @param aDateTime Updated Date Time data.
        * @return void
        */        
        void SetDateTimeL( const TDesC8& aDateTime ); 
    	
        /**
        * From CExifModify. Inserts/Updates given YCbCr Positioning in the Exif 
        * data.
        * @since 2.6
        * @param aYCbCrPositioning YCbCr Positioning data.
        * @return void
        */        
        void SetYCbCrPositioningL( TUint16 aYCbCrPositioning); 
    	
        /**
        * From CExifModify. Inserts/Updates given Software in the Exif data.
        * @since 2.6
        * @param aSoftware Updated Software data.
        * @return void
        */        
        void SetSoftwareL( const TDesC8& aSoftware ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Copyright in the Exif data.
        * @since 2.6
        * @param aCopyright Updated Copyright data.
        * @return void
        */        
        void SetCopyrightL( const TDesC8& aCopyright );

        /**
        * From CExifModify. Inserts/Updates given Exposure Time in the Exif 
        * data.
        * @since 2.6
        * @param aExposureTime1 Updated Exposure Time numerator.
        * @param aExposureTime2 Updated Exposure Time denominator.
        * @return void
        */
    	void SetExposureTimeL( TUint32 aExposureTime1, TUint32 aExposureTime2 );

        /**
        * From CExifModify. Inserts/Updates given Components Configuration in 
        * the Exif data.
        * @since 2.6
        * @param aComponentsConfiguration Updated Components Configuration data.
        * @return void
        */    	
        void SetComponentsConfigurationL( 
            TUint8 aFirstComponent, TUint8 aSecondComponent, 
            TUint8 aThirdComponent, TUint8 aFourthComponent ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Flash in the Exif data.
        * @since 2.6
        * @param aFlash Updated Flash data.
        * @return void
        */        
        void SetFlashL( TUint16 aFlash ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Color Space in the Exif data.
        * @since 2.6
        * @param aColorSpace Updated Color Space data.
        * @return void
        */        
        void SetColorSpaceL( TUint16 aColorSpace );
    	
        /**
        * From CExifModify. Inserts/Updates given Pixel X Dimension in the Exif 
        * data.
        * @since 2.6
        * @param aPixelXDimension Updated Pixel X Dimension data.
        * @return void
        */        
        void SetPixelXDimensionL( TUint32 aPixelXDimension );
    	
        /**
        * From CExifModify. Inserts/Updates given Pixel Y Dimension in the Exif 
        * data.
        * @since 2.6
        * @param aPixelYDimension Updated Pixel Y Dimension data.
        * @return void
        */        
        void SetPixelYDimensionL( TUint32 aPixelYDimension );
    	
        /**
        * From CExifModify. Inserts/Updates given Exposure Mode in the Exif 
        * data.
        * @since 2.6
        * @param aExposureMode Updated Exposure Mode data.
        * @return void
        */        
        void SetExposureModeL( TUint16 aExposureMode );
    	
        /**
        * From CExifModify. Inserts/Updates given White Balance in the Exif 
        * data.
        * @since 2.6
        * @param aWhiteBalance Updated White Balance data.
        * @return void
        */        
        void SetWhiteBalanceL( TUint16 aWhiteBalance ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Scene Capture Type in the Exif 
        * data.
        * @since 2.6
        * @param aSceneCaptureType Updated Scene Capture Type data.
        * @return void
        */        
        void SetSceneCaptureTypeL( TUint16 aSceneCaptureType ); 
    	
        
        /**
        * From CExifModify. Inserts/Updates given Exposure Program in the Exif 
        * data.
        * @since 2.6
        * @param aExposureProgram Updated Exposure Program data.
        * @return void
        */void SetExposureProgramL( TUint16 aExposureProgram ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Iso Speed Ratings in the Exif 
        * data.
        * @since 2.6
        * @param aIsoSpeedRatings Updated Iso Speed Ratings data.
        * @return void
        */        
        void SetIsoSpeedRatingsL( const TDesC8& aIsoSpeedRatings ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Date Time Original in the Exif 
        * data.
        * @since 2.6
        * @param aDateTimeOriginal Updated Date Time Original data.
        * @return void
        */        
        void SetDateTimeOriginalL( const TDesC8& aDateTimeOriginal );
    	
        /**
        * From CExifModify. Inserts/Updates given Date Time Digitized in the 
        * Exif data.
        * @since 2.6
        * @param aDateTimeDigitized Updated Date Time Digitized data.
        * @return void
        */        
        void SetDateTimeDigitizedL( const TDesC8& aDateTimeDigitized );
    	
        /**
        * From CExifModify. Inserts/Updates given Aperture Value in the Exif 
        * data.
        * @since 2.6
        * @param aApertureValue1 Updated Aperture Value numerator.
        * @param aApertureValue2 Updated Aperture Value denominator.
        * @return void
        */        
        void SetApertureValueL( 
            TUint32 aApertureValue1, 
            TUint32 aApertureValue2 ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Exposure Bias Value in the 
        * Exif data.
        * @since 2.6
        * @param aExposureBiasValue1 Updated Exposure Bias Value numerator.
        * @param aExposureBiasValue2 Updated Exposure Bias Value denominator.
        * @return void
        */        
        void SetExposureBiasValueL( 
            TInt32 aExposureBiasValue1, 
            TInt32 aExposureBiasValue2 );
    	
        /**
        * From CExifModify. Inserts/Updates given Metering Mode in the Exif data.
        * @since 2.6
        * @param aMeteringMode Updated Metering Mode data.
        * @return void
        */        
        void SetMeteringModeL( TUint16 aMeteringMode ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Light Source in the Exif data.
        * @since 2.6
        * @param aLightSource Updated Light Source data.
        * @return void
        */        
        void SetLightSourceL( TUint16 aLightSource ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Maker Note in the Exif data.
        * @since 2.6
        * @param aMakerNote Updated Maker Note data.
        * @return void
        */        
        void SetMakerNoteL( const TDesC8& aMakerNote ); 
    	
        /**
        * From CExifModify. Inserts/Updates given User Comment in the Exif data.
        * @since 2.6
        * @param aUserComment Updated User Comment data.
        * @return void
        */        
        void SetUserCommentL( const TDesC8& aUserComment );
    	
        /**
        * From CExifModify. Inserts/Updates given Related Sound File in the Exif 
        * data.
        * @since 2.6
        * @param aRelatedSoundFile Updated Related Sound File data.
        * @return void
        */        
        void SetRelatedSoundFileL( const TDesC8& aRelatedSoundFile ); 
    	
        /**
        * From CExifModify. Inserts/Updates given File Source in the Exif data.
        * @since 2.6
        * @param aFileSource Updated File Source data.
        * @return void
        */        
        void SetFileSourceL( TInt8 aFileSource ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Digital Zoom Ratio in the Exif 
        * data.
        * @since 2.6
        * @param aDigitalZoomRatio1 Updated Digital Zoom Ratio numerator.
        * @param aDigitalZoomRatio2 Updated Digital Zoom Ratio denominator.
        * @return void
        */        
        void SetDigitalZoomRatioL( 
            TUint32 aDigitalZoomRatio1, 
            TUint32 aDigitalZoomRatio2 ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Contrast in the Exif data.
        * @since 2.6
        * @param aContrast Updated Contrast data.
        * @return void
        */        
        void SetContrastL( TUint16 aContrast ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Saturation in the Exif data.
        * @since 2.6
        * @param aSaturation Updated Saturation data.
        * @return void
        */        
        void SetSaturationL( TUint16 aSaturation ); 
    	
        /**
        * From CExifModify. Inserts/Updates given Sharpness in the Exif data.
        * @since 2.6
        * @param aSharpness Updated Sharpness data.
        * @return void
        */        
        void SetSharpnessL( TUint16 aSharpness ); 

        /**
        * From CExifModify. Inserts/Updates given thumbnail X Resolution in the 
        * Exif data.
        * @since 2.6
        * @param aXResolution1 Updated thumbnail X Resolution numerator.
        * @param aXResolution2 Updated thumbnail X Resolution numerator.
        * @return void
        */
    	void SetThumbnailXResolutionL( 
            TUint32 aXResolution1, 
            TUint32 aXResolution2 );
    	
        /**
        * From CExifModify. Inserts/Updates given thumbnail Y Resolution in the 
        * Exif data.
        * @since 2.6
        * @param aYResolution1 Updated thumbnail Y Resolution numerator.
        * @param aYResolution2 Updated thumbnail Y Resolution numerator.
        * @return void
        */
        void SetThumbnailYResolutionL( 
            TUint32 aYResolution1, 
            TUint32 aYResolution2 );
    	
        /**
        * From CExifModify. Inserts/Updates given thumbnail Resolution Unit in 
        * the Exif data.
        * @since 2.6
        * @param aResolutionUnit Updated thumbnail Resolution Unit data.
        * @return void
        */        
        void SetThumbnailResolutionUnitL( TUint16 aResolutionUnit );

        /**
        * From CExifModify. Inserts/Updates given Shutter Speed Value in the 
        * Exif data.
        * @since 2.6
        * @param aShutterSpeedValue1 Updated Shutter Speed Value numerator.
        * @param aShutterSpeedValue2 Updated Shutter Speed Value denominator.
        * @return void
        */
    	void SetShutterSpeedValueL( 
            TInt32 aShutterSpeedValue1, 
            TInt32 aShutterSpeedValue2 );

        /**
        * From CExifModify. Inserts/Updates given Brightness Value in the Exif 
        * data.
        * @since 2.6
        * @param aBrightnessValue1 Updated Brightness Value numerator.
        * @param aBrightnessValue2 Updated Brightness Value denominator.
        * @return void
        */
    	void SetBrightnessValueL( 
            TInt32 aBrightnessValue1, 
            TInt32 aBrightnessValue2 );

        /**
        * From CExifModify. Inserts/Updates given CustomRendered in the Exif 
        * data.
        * @since 2.6
        * @param aCustomRendered Updated Custom Rendered data.
        * @return void
        */        
        void SetCustomRenderedL( TUint16 aCustomRendered ); 

        /**
        * From CExifModify. Inserts/Updates given Gain Control in the Exif 
        * data.
        * @since 2.6
        * @param aGainControl Updated Gain Control data.
        * @return void
        */        
        void SetGainControlL( TUint16 aGainControl ); 

	private:

        /**
        * C++ default constructor.
        */
    	CExifModifyImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    	void ConstructL( const TDesC8& aExifData, TBool aCreate, TUint aExifModifyOption );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    	void ConstructL();

	private: 

        // Exif reader class
    	CExifReadImpl* iExifRead;

        // Core Exif data structure storing the IFDs and tags.
    	CExifCore* iExifCore;

        // The first 4 bytes of the data provided during instantiation. 
        // Used for cross-checking while returning the final Exif data.
        TUint32 iCrossCheckBegin;

        // The 4 bytes in the middle of the data provided during instantiation.
        // Used for cross-checking while returning the final Exif data.
        TUint32 iCrossCheckMiddle;

        // The 4 bytes at the end of the data provided during instantiation.
        // Used for cross-checking while returning the final Exif data.
        TUint32 iCrossCheckEnd;

        // The length of the data provided during instantiation.
        // Used for cross-checking while returning the final Exif data.
        TInt iCrossCheckLength;

	private:

        // Exif modify options
    	TUint iExifModifyOption;

    }; // CExifModifyImpl

#endif // EXIFMODIFYIMPL_H

// End of File
