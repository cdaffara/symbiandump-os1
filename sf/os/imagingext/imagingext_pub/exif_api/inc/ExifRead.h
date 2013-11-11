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
* Description:  Exif file format parser ( reader ) class
*
*/


#ifndef EXIFREAD_H
#define EXIFREAD_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
enum TExifIfdType
    {
	EIfd0 = 0,
	EIfdExif,
	EIfd1,
	EIfdGps,
	EIfdIntOp
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CExifTag;

// CLASS DECLARATION

/**
* CExifRead
* Interface class for parsing Exif v2.2 file format. An instance of this class 
* can be instantiated providing valid Exif data. 
*
* @lib ExifLib
* @since 2.6 
*/
NONSHARABLE_CLASS( CExifRead ): public CBase
    {

    public:  // Enumerations
        enum TExifReadOption
        {
	    ENoOptions          = 0x0000, // Original, safe full parsing
	    ENoJpeg             = 0x0001, // No main JPEG included, no parsing for main image
	    EFastJpegParsing    = 0x0002, // Fast JPEG marker parsing
	    ENoTagChecking      = 0x0004  // Ignore unknown EXIF tags and mandatory tag checking
        };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
    	IMPORT_C static CExifRead* NewL( const TDesC8& aExifData );

    	IMPORT_C static CExifRead* NewL( const TDesC8& aExifData, TUint aExifReadOption );
        
        /**
        * Destructor.
        */
    	virtual ~CExifRead();

    public: // New functions

        /**
        * Returns the Tag instance, which has the specified ID from the 
        * requested IFD.
        * @since 2.6
        * @param aIfdType The hosting IFD type
        * @param aTagId The queried tag ID.
        * @return Unmodifiable tag instance returned.
        */
    	virtual const CExifTag* GetTagL( 
            TExifIfdType aIfdType, 
            TUint16 aTagId ) const = 0;

        /**
        * Returns the IDs of all the tags that are stored in the Exif data.
        * @since 2.6
        * @param aIfdType The hosting IFD type.
        * @param aNoTags Number of tag IDs returned.
        * @return Pointer to the tag IDs.
        */
    	virtual TUint16* GetTagIdsL( 
            TExifIfdType aIfdType, 
            TInt& aNoTags ) const = 0;

        /**
        * Returns the types of the IFDs stored in the Exif data.
        * @since 2.6
        * @param aNoIfd Number of IFD types returned.
        * @return Pointer to the IFD types.
        */
    	virtual TExifIfdType* GetIfdTypesL( TInt& aNoIfd ) const = 0;

        /**
        * Returns pointer to a copy of the thumbnail image data.
        * @since 2.6
        * @return Pointer to the descriptor containing a copy of the 
        * Exif thumbnail image.
        */
    	virtual HBufC8* GetThumbnailL() const = 0;

        /**
        * Returns a boolean stating if the queried IFD structure exists in the 
        * Exif data.
        * @since 2.6
        * @param aIfdType The queried IFD type.
        * @return Boolean stating if the specified IFD exists or not.
        */
    	virtual TBool IfdExists( TExifIfdType aIfdType ) const = 0;

        /**
        * Returns a boolean stating if the queried tag exists in the specified 
        * IFD structure.
        * @since 2.6
        * @param aTagId Queried tag ID.
        * @param aIfdType The hosting IFD type.
        * @return Boolean stating if the specified tag exists or not.
        */
    	virtual TBool TagExists( 
            TUint16 aTagId, 
            TExifIfdType aIfdType ) const = 0;

        /**
        * Gets the Image Description tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Image 
        * Description data.
        */
    	virtual HBufC8* GetImageDescriptionL() const = 0;

        /**
        * Gets the Make tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Make data.
        */
    	virtual HBufC8* GetMakeL() const = 0;
    	
        /**
        * Gets the Model tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Model data.
        */
        virtual HBufC8* GetModelL() const = 0;
    	
        /**
        * Gets the Transfer Function tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Transfer 
        * Function data.
        */
        virtual HBufC8* GetTransferFunctionL() const = 0; 
    	
        /**
        * Gets the Date Time tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * data.
        */
        virtual HBufC8* GetDateTimeL() const = 0; 
    	
        /**
        * Gets the Software tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Software 
        * data.
        */
        virtual HBufC8* GetSoftwareL() const = 0; 
    	
        /**
        * Gets the Copyright tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Copyright 
        * data.
        */
        virtual HBufC8* GetCopyrightL() const = 0; 

        /**
        * Gets the Orientation tag data.
        * @since 2.6
        * @param aOrientation Returned Orientation data.
        * @return Error code.
        */
    	virtual TInt GetOrientation( TUint16& aOrientation ) const = 0;
    	
        /**
        * Gets the X Resolution tag data.
        * @since 2.6
        * @param aXResolution1 Returned X Resolution numerator.
        * @param aXResolution2 Returned X Resolution denominator.
        * @return Error code. 
        */
        virtual TInt GetXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const = 0;
    	
        /**
        * Gets the Y Resolution tag data.
        * @since 2.6
        * @param aYResolution1 Returned Y Resolution numerator.
        * @param aYResolution2 Returned Y Resolution denominator.
        * @return Error code.
        */
        virtual TInt GetYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const = 0;
    	
        /**
        * Gets the Resolution Unit tag data.
        * @since 2.6
        * @param aResolutionUnit Returned Resolution Unit data.
        * @return Error code. 
        */
        virtual TInt GetResolutionUnit( TUint16& aResolutionUnit ) const = 0; 
    	
        /**
        * Gets the YCbCr Positioning tag data.
        * @since 2.6
        * @param aYCbCrPositioning Returned YCbCr Positioning data.
        * @return Error code.
        */
        virtual TInt GetYCbCrPositioning( 
            TUint16& aYCbCrPositioning ) const = 0; 
    	
        /**
        * Gets the Exif Ifd Pointer tag data.
        * @since 2.6
        * @param aExifIfdPointer Returned Exif Ifd Pointer data.
        * @return Error code.
        */
        virtual TInt GetExifIfdPointer( TUint32& aExifIfdPointer ) const = 0; 
    	
        /**
        * Gets the Gps Info Ifd Pointer tag data.
        * @since 2.6
        * @param aGpsInfoIfdPointer Returned Gps Info Ifd Pointer data.
        * @return Error code.
        */
        virtual TInt GetGpsInfoIfdPointer( 
            TUint32& aGpsInfoIfdPointer ) const = 0; 

        /**
        * Gets the Iso Speed Ratings tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Iso Speed 
        * Ratings data.
        */
    	virtual HBufC8* GetIsoSpeedRatingsL() const = 0; 
    	
        /**
        * Gets the Date Time Original tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * Original data.
        */
        virtual HBufC8* GetDateTimeOriginalL() const = 0;
    	
        /**
        * Gets the Date Time Digitized tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * Digitized data.
        */
        virtual HBufC8* GetDateTimeDigitizedL() const = 0; 

    	/**
        * Gets the Maker Note tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Maker Note 
        * data.
        */
        virtual HBufC8* GetMakerNoteL() const = 0; 
    	
        /**
        * Gets the User Comment tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the User 
        * Comment data.
        */
        virtual HBufC8* GetUserCommentL() const = 0; 
    	
        /**
        * Gets the Related Sound File tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Related 
        * Sound File data.
        */
        virtual HBufC8* GetRelatedSoundFileL() const = 0; 

        /**
        * Gets the Exposure Time tag data.
        * @since 2.6
        * @param ExposureTime Returned Exposure Time data.
        * @return Error code.
        */
    	virtual TInt GetExposureTime( 
            TUint32& aExposureTime1, 
            TUint32& aExposureTime2 ) const = 0; 
    	
        /**
        * Gets the Components Configuration tag data.
        * @since 2.6
        * @param aComponentsConfiguration Returned Components Configuration 
        * data.
        * @return Error code.
        */
        virtual TInt GetComponentsConfiguration( 
            TUint8& aFirstComponent, TUint8& aSecondComponent,
            TUint8& aThirdComponent, TUint8& aFourthComponent) const = 0; 
    	
        /**
        * Gets the Flash tag data.
        * @since 2.6
        * @param aFlash Returned Flash data.
        * @return Error code.
        */
        virtual TInt GetFlash( TUint16& aFlash ) const = 0; 
    	
        /**
        * Gets the ColorSpace tag data.
        * @since 2.6
        * @param aColorSpace Returned ColorSpace data.
        * @return Error code.
        */
        virtual TInt GetColorSpace( TUint16& aColorSpace ) const = 0; 
    	
        /**
        * Gets the Pixel X Dimension tag data.
        * @since 2.6
        * @param aPixelXDimension Returned Pixel X Dimension data.
        * @return Error code.
        */
        virtual TInt GetPixelXDimension( TUint32& aPixelXDimension ) const = 0; 
    	
        /**
        * Gets the Pixel Y Dimension tag data.
        * @since 2.6
        * @param aPixelYDimension Returned Pixel Y Dimension data.
        * @return Error code.
        */
        virtual TInt GetPixelYDimension( TUint32& aPixelYDimension ) const = 0; 
    	
        /**
        * Gets the Exposure Mode tag data.
        * @since 2.6
        * @param aExposureMode Returned Exposure Mode data.
        * @return Error code.
        */
        virtual TInt GetExposureMode( TUint16& aExposureMode ) const = 0; 
    	
        /**
        * Gets the White Balance tag data.
        * @since 2.6
        * @param aWhiteBalance Returned White Balance data.
        * @return Error code.
        */
        virtual TInt GetWhiteBalance( TUint16& aWhiteBalance ) const = 0; 
    	
        /**
        * Gets the Scene Capture Type tag data.
        * @since 2.6
        * @param aSceneCaptureType Returned Scene Capture Type data.
        * @return Error code.
        */
        virtual TInt GetSceneCaptureType( 
            TUint16& aSceneCaptureType ) const = 0; 
    	
        /**
        * Gets the Exposure Program tag data.
        * @since 2.6
        * @param aExposureProgram Returned Exposure Program data.
        * @return Error code.
        */
        virtual TInt GetExposureProgram( TUint16& aExposureProgram ) const = 0;
    	
        /**
        * Gets the Aperture Value tag data.
        * @since 2.6
        * @param aApertureValue1 Returned Aperture Value numerator.
        * @param aApertureValue2 Returned Aperture Value denominator.
        * @return Error code.
        */
        virtual TInt GetApertureValue( 
            TUint32& aApertureValue1, 
            TUint32& aApertureValue2 ) const = 0; 
    	
        /**
        * Gets the Exposure Bias Value tag data.
        * @since 2.6
        * @param aExposureBiasValue1 Returned Exposure Bias Value numerator.
        * @param aExposureBiasValue1 Returned Exposure Bias Value denominator.
        * @return Error code.
        */
        virtual TInt GetExposureBiasValue( 
            TInt32& aExposureBiasValue1, 
            TInt32& aExposureBiasValue2 ) const = 0; 
    	
        /**
        * Gets the Metering Mode tag data.
        * @since 2.6
        * @param aMeteringMode Returned Metering Mode data.
        * @return Error code.
        */
        virtual TInt GetMeteringMode( TUint16& aMeteringMode ) const = 0; 
    	
        /**
        * Gets the Light Source tag data.
        * @since 2.6
        * @param aLightSource Returned Light Source data.
        * @return Error code.
        */
        virtual TInt GetLightSource( TUint16& aLightSource ) const = 0; 
    	
        /**
        * Gets the File Source tag data.
        * @since 2.6
        * @param aFileSource Returned File Source data.
        * @return Error code.
        */
        virtual TInt GetFileSource( TInt8& aFileSource ) const = 0; 
    	
        /**
        * Gets the Digital Zoom Ratio tag data.
        * @since 2.6
        * @param aDigitalZoomRatio1 Returned Digital Zoom Ratio numerator.
        * @param aDigitalZoomRatio2 Returned Digital Zoom Ratio denominator.
        * @return Error code.
        */
        virtual TInt GetDigitalZoomRatio( 
            TUint32& aDigitalZoomRatio1, 
            TUint32& aDigitalZoomRatio2 ) const = 0; 
    	
        /**
        * Gets the Contrast tag data.
        * @since 2.6
        * @param aContrast Returned Contrast data.
        * @return Error code.
        */
        virtual TInt GetContrast( TUint16& aContrast ) const = 0; 
    	
        /**
        * Gets the Saturation tag data.
        * @since 2.6
        * @param aSaturation Returned Saturation data.
        * @return Error code.
        */
        virtual TInt GetSaturation( TUint16& aSaturation ) const = 0; 
    	
        /**
        * Gets the Sharpness tag data.
        * @since 2.6
        * @param aSharpness Returned Sharpness data.
        * @return Error code.
        */
        virtual TInt GetSharpness( TUint16& aSharpness ) const = 0; 
    	
        /**
        * Gets the Exif Version tag data.
        * @since 2.6
        * @param aExifVersion Returned Exif Version data.
        * @return Error code.
        */
        virtual TInt GetExifVersion( TUint32& aExifVersion ) const = 0; 
    	
        /**
        * Gets the Flash Pix Version tag data.
        * @since 2.6
        * @param aFlashPixVersion Returned Flash Pix Version data.
        * @return Error code.
        */
        virtual TInt GetFlashPixVersion( TUint32& aFlashPixVersion ) const = 0; 
    	
        /**
        * Gets the Interoperability Ifd Pointer tag data.
        * @since 2.6
        * @param aInteroperabilityIfdPointer Returned Interoperability Ifd 
        * Pointer data.
        * @return Error code.
        */
        virtual TInt GetInteroperabilityIfdPointer( 
            TUint32& aInteroperabilityIfdPointer ) const = 0; 

        /**
        * Gets the thumbnail X Resolution tag data.
        * @since 2.6
        * @param aXResolution1 Returned thumbnail X Resolution numerator.
        * @param aXResolution1 Returned thumbnail X Resolution denominator.
        * @return Error code.
        */
    	virtual TInt GetThumbnailXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const = 0;
    	
        /**
        * Gets the thumbnail Y Resolution tag data.
        * @since 2.6
        * @param aYResolution1 Returned thumbnail Y Resolution numerator.
        * @param aYResolution1 Returned thumbnail Y Resolution denominator.
        * @return Error code.
        */        
        virtual TInt GetThumbnailYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const = 0;
    	
        /**
        * Gets the thumbnail Resolution Unit tag data.
        * @since 2.6
        * @param aResolutionUnit Returned thumbnail Resolution Unit data.
        * @return Error code.
        */            
        virtual TInt GetThumbnailResolutionUnit( 
            TUint16& aResolutionUnit ) const = 0;
    	
        /**
        * Gets the thumbnail Compression tag data.
        * @since 2.6
        * @param aCompression Returned thumbnail Compression data.
        * @return Error code.
        */            
        virtual TInt GetThumbnailCompression( TUint16& aCompression ) const = 0; 
    	
        /**
        * Gets the thumbnail Jpeg Interchange Format tag data.
        * @since 2.6
        * @param aJpegInterchangeFormat Returned thumbnail Jpeg Interchange 
        * Format data.
        * @return Error code.
        */            
        virtual TInt GetJpegInterchangeFormat( 
            TUint32& aJpegInterchangeFormat ) const = 0; 
    	
        /**
        * Gets the thumbnail Jpeg Interchange Format Length tag data.
        * @since 2.6
        * @param aJpegInterchangeFormatLength Returned thumbnail Jpeg 
        * Interchange Format Length data.
        * @return Error code.
        */    
        virtual TInt GetJpegInterchangeFormatLength( 
            TUint32& aJpegInterchangeFormatLength ) const = 0;

        /**
        * Returns a copy of whole Exif APP1 segment in a descriptor.
        * @since 2.6
        * @return Descriptor containing the Exif APP1 segment data.
        */    
        virtual HBufC8* GetExifAppSegmentL() const = 0;

        /**
        * Gets the Shutter Speed Value tag data.
        * @since 2.6
        * @param aShutterSpeedValue1 Shutter Speed Value numerator.
        * @param aShutterSpeedValue2 Shutter Speed Value denominator.
        * @return Error code.
        */
        virtual TInt GetShutterSpeedValue( TInt32& aShutterSpeedValue1, 
            TInt32& aShutterSpeedValue2 ) const = 0;

        /**
        * Gets the Brightness Value tag data.
        * @since 2.6
        * @param aBrightnessValue1 Brightness Value numerator.
        * @param aBrightnessValue2 Brightness Value denominator.
        * @return Error code.
        */
        virtual TInt GetBrightnessValue( TInt32& aBrightnessValue1, 
            TInt32& aBrightnessValue2 ) const = 0;

        /**
        * Gets the Custom Rendered tag data.
        * @since 2.6
        * @param aCustomRendered Returned Custom Rendered data.
        * @return Error code.
        */
        virtual TInt GetCustomRendered( TUint16& aCustomRendered ) const = 0;

        /**
        * Gets the Gain Control tag data.
        * @since 2.6
        * @param aGainControl Returned Gain Control data.
        * @return Error code.
        */
        virtual TInt GetGainControl( TUint16& aGainControl ) const = 0;

        /**
        * Gets the Gps Version tag data.
        * @since 2.6
        * @param aGpsVersion Returned Gps Version data.
        * @return Error code.
        */
        virtual TInt GetGpsVersion( TUint32& aGpsVersion ) const = 0; 
    };

#endif      // EXIFREAD_H   
            
// End of File
