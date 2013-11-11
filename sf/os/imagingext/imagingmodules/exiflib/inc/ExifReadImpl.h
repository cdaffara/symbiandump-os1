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
* Description:  Exif file format parser ( reader ) class implementation
*
*/


#ifndef EXIFREADIMPL_H
#define EXIFREADIMPL_H

//  INCLUDES
#include "ExifRead.h"
#include "ExifValueTable.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CExifEndianBase;
class CExifCore;

// CLASS DECLARATION

/**
* CExifReadImpl
* Implementation class for parsing Exif v2.2 file format. An instance of this 
* class can be instantiated providing valid Exif data. 
*  
* @lib ExifLib
* @since 2.6 
*/
NONSHARABLE_CLASS( CExifReadImpl ): public CExifRead
    {
	public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
    	static CExifReadImpl* NewL( const TDesC8& aExifData, TBool aCreate, TUint aExifReadOption );

        /**
        * Two-phased constructor.
        */
    	static CExifReadImpl* NewL();

        /**
        * Destructor.
        */
        virtual ~CExifReadImpl();
    
	public:  // Functions from base classes

        /**
        * From CExifRead. Returns the Tag instance, which has the specified ID 
        * from the requested IFD.
        * @since 2.6
        * @param aIfdType The hosting IFD type
        * @param aTagId The queried tag ID.
        * @return Unmodifiable tag instance returned.
        */
    	const CExifTag* GetTagL( TExifIfdType aIfdType, TUint16 aTagId ) const;

        /**
        * From CExifRead. Returns the IDs of all the tags that are stored in 
        * the Exif data.
        * @since 2.6
        * @param aIfdType The hosting IFD type.
        * @param aNoTags Number of tag IDs returned.
        * @return Pointer to the tag IDs.
        */
    	TUint16* GetTagIdsL( TExifIfdType aIfdType, TInt& aNoTags ) const;

        /**
        * From CExifRead. Returns the types of the IFDs stored in the Exif data.
        * @since 2.6
        * @param aNoIfd Number of IFD types returned.
        * @return Pointer to the IFD types.
        */
    	TExifIfdType* GetIfdTypesL( TInt& aNoIfd ) const;

        /**
        * From CExifRead. Returns pointer to a copy of the thumbnail image data.
        * @since 2.6
        * @return Pointer to the descriptor containing a copy of the Exif 
        * thumbnail image.
        */
    	HBufC8* GetThumbnailL() const;

        /**
        * From CExifRead. Returns a boolean stating if the queried IFD 
        * structure exists in the Exif data.
        * @since 2.6
        * @param aIfdType The queried IFD type.
        * @return Boolean stating if the specified IFD exists or not.
        */
    	TBool IfdExists( TExifIfdType aIfdType ) const;

        /**
        * From CExifRead. Returns a boolean stating if the queried tag exists 
        * in the specified IFD structure.
        * @since 2.6
        * @param aTagId Queried tag ID.
        * @param aIfdType The hosting IFD type.
        * @return Boolean stating if the specified tag exists or not.
        */
    	TBool TagExists( TUint16 aTagId, TExifIfdType aIfdType ) const;

        /**
        * From CExifRead. Gets the Image Description tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Image 
        * Description data.
        */
    	HBufC8* GetImageDescriptionL() const;

        /**
        * From CExifRead. Gets the Make tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Make data.
        */
    	HBufC8* GetMakeL() const;
    	
        /**
        * From CExifRead. Gets the Model tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Model data.
        */
        HBufC8* GetModelL() const;
    	
        /**
        * From CExifRead. Gets the Transfer Function tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Transfer 
        * Function data.
        */
        HBufC8* GetTransferFunctionL() const; 
    	
        /**
        * From CExifRead. Gets the Date Time tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * data.
        */
        HBufC8* GetDateTimeL() const; 
    	
        /**
        * From CExifRead. Gets the Software tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Software 
        * data.
        */
        HBufC8* GetSoftwareL() const; 
    	
        /**
        * From CExifRead. Gets the Copyright tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Copyright 
        * data.
        */
        HBufC8* GetCopyrightL() const; 

        /**
        * From CExifRead. Gets the Orientation tag data.
        * @since 2.6
        * @param aOrientation Returned Orientation data.
        * @return Error code.
        */
    	TInt GetOrientation( TUint16& aOrientation ) const;
    	
        /**
        * From CExifRead. Gets the X Resolution tag data.
        * @since 2.6
        * @param aXResolution1 Returned X Resolution numerator.
        * @param aXResolution2 Returned X Resolution denominator.
        * @return Error code. 
        */
        TInt GetXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const;
    	
        /**
        * From CExifRead. Gets the Y Resolution tag data.
        * @since 2.6
        * @param aYResolution1 Returned Y Resolution numerator.
        * @param aYResolution2 Returned Y Resolution denominator.
        * @return Error code.
        */
        TInt GetYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const;
    	
        /**
        * From CExifRead. Gets the Resolution Unit tag data.
        * @since 2.6
        * @param aResolutionUnit Returned Resolution Unit data.
        * @return Error code. 
        */
        TInt GetResolutionUnit( TUint16& aResolutionUnit ) const; 
    	
        /**
        * From CExifRead. Gets the YCbCr Positioning tag data.
        * @since 2.6
        * @param aYCbCrPositioning Returned YCbCr Positioning data.
        * @return Error code.
        */
        TInt GetYCbCrPositioning( TUint16& aYCbCrPositioning ) const; 
    	
        /**
        * From CExifRead. Gets the Exif Ifd Pointer tag data.
        * @since 2.6
        * @param aExifIfdPointer Returned Exif Ifd Pointer data.
        * @return Error code.
        */
        TInt GetExifIfdPointer( TUint32& aExifIfdPointer ) const; 
    	
        /**
        * From CExifRead. Gets the Gps Info Ifd Pointer tag data.
        * @since 2.6
        * @param aGpsInfoIfdPointer Returned Gps Info Ifd Pointer data.
        * @return Error code.
        */
        TInt GetGpsInfoIfdPointer( TUint32& aGpsInfoIfdPointer ) const; 

        /**
        * From CExifRead. Gets the Iso Speed Ratings tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Iso Speed 
        * Ratings data.
        */
    	HBufC8* GetIsoSpeedRatingsL() const; 
    	
        /**
        * From CExifRead. Gets the Date Time Original tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * Original data.
        */
        HBufC8* GetDateTimeOriginalL() const;
    	
        /**
        * From CExifRead. Gets the Date Time Digitized tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Date Time 
        * Digitized data.
        */
        HBufC8* GetDateTimeDigitizedL() const; 

    	/**
        * From CExifRead. Gets the Maker Note tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Maker Note 
        * data.
        */
        HBufC8* GetMakerNoteL() const; 
    	
        /**
        * From CExifRead. Gets the User Comment tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the User Comment 
        * data.
        */
        HBufC8* GetUserCommentL() const; 
    	
        /**
        * From CExifRead. Gets the Related Sound File tag data.
        * @since 2.6
        * @return Pointer to the descriptor containing copy of the Related 
        * Sound File data.
        */
        HBufC8* GetRelatedSoundFileL() const; 

        /**
        * From CExifRead. Gets the Exposure Time tag data.
        * @since 2.6
        * @param ExposureTime Returned Exposure Time data.
        * @return Error code.
        */
    	TInt GetExposureTime( 
            TUint32& aExposureTime1, 
            TUint32& aExposureTime2 ) const; 
    	
        /**
        * From CExifRead. Gets the Components Configuration tag data.
        * @since 2.6
        * @param aComponentsConfiguration Returned Components Configuration 
        * data.
        * @return Error code.
        */
        TInt GetComponentsConfiguration( 
            TUint8& aFirstComponent, TUint8& aSecondComponent,
            TUint8& aThirdComponent, TUint8& aFourthComponent) const; 

    	
        /**
        * From CExifRead. Gets the Flash tag data.
        * @since 2.6
        * @param aFlash Returned Flash data.
        * @return Error code.
        */
        TInt GetFlash( TUint16& aFlash ) const; 
    	
        /**
        * From CExifRead. Gets the ColorSpace tag data.
        * @since 2.6
        * @param aColorSpace Returned ColorSpace data.
        * @return Error code.
        */
        TInt GetColorSpace( TUint16& aColorSpace ) const; 
    	
        /**
        * From CExifRead. Gets the Pixel X Dimension tag data.
        * @since 2.6
        * @param aPixelXDimension Returned Pixel X Dimension data.
        * @return Error code.
        */
        TInt GetPixelXDimension( TUint32& aPixelXDimension ) const; 
    	
        /**
        * From CExifRead. Gets the Pixel Y Dimension tag data.
        * @since 2.6
        * @param aPixelYDimension Returned Pixel Y Dimension data.
        * @return Error code.
        */
        TInt GetPixelYDimension( TUint32& aPixelYDimension ) const; 
    	
        /**
        * From CExifRead. Gets the Exposure Mode tag data.
        * @since 2.6
        * @param aExposureMode Returned Exposure Mode data.
        * @return Error code.
        */
        TInt GetExposureMode( TUint16& aExposureMode ) const; 
    	
        /**
        * From CExifRead. Gets the White Balance tag data.
        * @since 2.6
        * @param aWhiteBalance Returned White Balance data.
        * @return Error code.
        */
        TInt GetWhiteBalance( TUint16& aWhiteBalance ) const; 
    	
        /**
        * From CExifRead. Gets the Scene Capture Type tag data.
        * @since 2.6
        * @param aSceneCaptureType Returned Scene Capture Type data.
        * @return Error code.
        */
        TInt GetSceneCaptureType( TUint16& aSceneCaptureType ) const; 
    	
        /**
        * From CExifRead. Gets the Exposure Program tag data.
        * @since 2.6
        * @param aExposureProgram Returned Exposure Program data.
        * @return Error code.
        */
        TInt GetExposureProgram( TUint16& aExposureProgram ) const;
    	
        /**
        * From CExifRead. Gets the Aperture Value tag data.
        * @since 2.6
        * @param aApertureValue1 Returned Aperture Value numerator.
        * @param aApertureValue2 Returned Aperture Value denominator.
        * @return Error code.
        */
        TInt GetApertureValue( 
            TUint32& aApertureValue1, 
            TUint32& aApertureValue2 ) const; 
    	
        /**
        * From CExifRead. Gets the Exposure Bias Value tag data.
        * @since 2.6
        * @param aExposureBiasValue1 Returned Exposure Bias Value numerator.
        * @param aExposureBiasValue1 Returned Exposure Bias Value denominator.
        * @return Error code.
        */
        TInt GetExposureBiasValue( 
            TInt32& aExposureBiasValue1, 
            TInt32& aExposureBiasValue2 ) const; 
    	
        /**
        * From CExifRead. Gets the Metering Mode tag data.
        * @since 2.6
        * @param aMeteringMode Returned Metering Mode data.
        * @return Error code.
        */
        TInt GetMeteringMode( TUint16& aMeteringMode ) const; 
    	
        /**
        * From CExifRead. Gets the Light Source tag data.
        * @since 2.6
        * @param aLightSource Returned Light Source data.
        * @return Error code.
        */
        TInt GetLightSource( TUint16& aLightSource ) const; 
    	
        /**
        * From CExifRead. Gets the File Source tag data.
        * @since 2.6
        * @param aFileSource Returned File Source data.
        * @return Error code.
        */
        TInt GetFileSource( TInt8& aFileSource ) const; 
    	
        /**
        * From CExifRead. Gets the Digital Zoom Ratio tag data.
        * @since 2.6
        * @param aDigitalZoomRatio1 Returned Digital Zoom Ratio numerator.
        * @param aDigitalZoomRatio2 Returned Digital Zoom Ratio denominator.
        * @return Error code.
        */
        TInt GetDigitalZoomRatio( 
            TUint32& aDigitalZoomRatio1, 
            TUint32& aDigitalZoomRatio2 ) const; 
    	
        /**
        * From CExifRead. Gets the Contrast tag data.
        * @since 2.6
        * @param aContrast Returned Contrast data.
        * @return Error code.
        */
        TInt GetContrast( TUint16& aContrast ) const; 
    	
        /**
        * From CExifRead. Gets the Saturation tag data.
        * @since 2.6
        * @param aSaturation Returned Saturation data.
        * @return Error code.
        */
        TInt GetSaturation( TUint16& aSaturation ) const; 
    	
        /**
        * From CExifRead. Gets the Sharpness tag data.
        * @since 2.6
        * @param aSharpness Returned Sharpness data.
        * @return Error code.
        */
        TInt GetSharpness( TUint16& aSharpness ) const; 
    	
        /**
        * From CExifRead. Gets the Exif Version tag data.
        * @since 2.6
        * @param aExifVersion Returned Exif Version data.
        * @return Error code.
        */
        TInt GetExifVersion( TUint32& aExifVersion ) const; 
    	
        /**
        * From CExifRead. Gets the Flash Pix Version tag data.
        * @since 2.6
        * @param aFlashPixVersion Returned Flash Pix Version data.
        * @return Error code.
        */
        TInt GetFlashPixVersion( TUint32& aFlashPixVersion ) const; 
    	
        /**
        * From CExifRead. Gets the Interoperability Ifd Pointer tag data.
        * @since 2.6
        * @param aInteroperabilityIfdPointer Returned Interoperability Ifd 
        * Pointer data.
        * @return Error code.
        */
        TInt GetInteroperabilityIfdPointer( 
            TUint32& aInteroperabilityIfdPointer ) const; 

        /**
        * From CExifRead. Gets the thumbnail X Resolution tag data.
        * @since 2.6
        * @param aXResolution1 Returned thumbnail X Resolution numerator.
        * @param aXResolution1 Returned thumbnail X Resolution denominator.
        * @return Error code.
        */
    	TInt GetThumbnailXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const;
    	
        /**
        * From CExifRead. Gets the thumbnail Y Resolution tag data.
        * @since 2.6
        * @param aYResolution1 Returned thumbnail Y Resolution numerator.
        * @param aYResolution1 Returned thumbnail Y Resolution denominator.
        * @return Error code.
        */        
        TInt GetThumbnailYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const;
    	
        /**
        * From CExifRead. Gets the thumbnail Resolution Unit tag data.
        * @since 2.6
        * @param aResolutionUnit Returned thumbnail Resolution Unit data.
        * @return Error code.
        */            
        TInt GetThumbnailResolutionUnit( TUint16& aResolutionUnit ) const;
    	
        /**
        * From CExifRead. Gets the thumbnail Compression tag data.
        * @since 2.6
        * @param aCompression Returned thumbnail Compression data.
        * @return Error code.
        */            
        TInt GetThumbnailCompression( TUint16& aCompression ) const; 
    	
        /**
        * From CExifRead. Gets the thumbnail Jpeg Interchange Format tag data.
        * @since 2.6
        * @param aJpegInterchangeFormat Returned thumbnail Jpeg Interchange 
        * Format data.
        * @return Error code.
        */            
        TInt GetJpegInterchangeFormat( TUint32& aJpegInterchangeFormat ) const; 
    	
        /**
        * From CExifRead. Gets the thumbnail Jpeg Interchange Format Length tag 
        * data.
        * @since 2.6
        * @param aJpegInterchangeFormatLength Returned thumbnail Jpeg 
        * Interchange Format Length data.
        * @return Error code.
        */    
        TInt GetJpegInterchangeFormatLength( 
            TUint32& aJpegInterchangeFormatLength ) const;

        /**
        * From CExifRead. Returns a copy of whole Exif APP1 segment in a 
        * descriptor.
        * @since 2.6
        * @return Descriptor containing the Exif APP1 segment data.
        */    
        HBufC8* GetExifAppSegmentL() const;

        /**
        * From CExifRead. Gets the Shutter Speed Value tag data.
        * @since 2.6
        * @param aShutterSpeedValue1 Shutter Speed Value numerator.
        * @param aShutterSpeedValue2 Shutter Speed Value denominator.
        * @return Error code.
        */
        TInt GetShutterSpeedValue( TInt32& aShutterSpeedValue1, 
            TInt32& aShutterSpeedValue2 ) const;

        /**
        * From CExifRead. Gets the Brightness Value tag data.
        * @since 2.6
        * @param aBrightnessValue1 Brightness Value numerator.
        * @param aBrightnessValue2 Brightness Value denominator.
        * @return Error code.
        */
        TInt GetBrightnessValue( TInt32& aBrightnessValue1, 
            TInt32& aBrightnessValue2 ) const;

        /**
        * From CExifRead. Gets the Custom Rendered tag data.
        * @since 2.6
        * @param aCustomRendered Returned Custom Rendered data.
        * @return Error code.
        */
        TInt GetCustomRendered( TUint16& aCustomRendered ) const;

        /**
        * From CExifRead. Gets the Gain Control tag data.
        * @since 2.6
        * @param aGainControl Returned Gain Control data.
        * @return Error code.
        */
        TInt GetGainControl( TUint16& aGainControl ) const;

        /**
        * From CExifRead. Gets the Gps Version tag data.
        * @since 2.6
        * @param aGpsVersion Returned Gps Version data.
        * @return Error code.
        */
        TInt GetGpsVersion( TUint32& aGpsVersion ) const; 

        /**
        * Parses the Jpeg primary image data and initializes the Jpeg primary 
        * image structures.
        * @since 2.6
        * @return void.
        */        
    	void ParseJpegDataL( const TDesC8& aJpegData );

	private: 
        
        /**
        * C++ default constructor.
        */
    	CExifReadImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    	void ConstructL( const TDesC8& aExifData, TBool aCreate, TUint aReadOption );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    	void ConstructL();

    private: // New functions

        /**
        * Parses the Exif data format and creates/initializes the internal 
        * Exif data structures.
        * @since 2.6
        * @return void.
        */
    	void ParseExifFormatL();

        /**
        * Parses the IFD data according to the specified IFD type. 
        * Creates/initializes the internal IFD data structures. 
        * Returns the offset of the next IFD in the parameter.
        * @since 2.6
        * @param aIfdType The parsed IFD type
        * @param aNextIfdOffset The next IFD offset returned.
        * @return void.
        */
    	void ParseIfdL( TExifIfdType aIfdType, TUint32& aNextIfdOffset );
        
        /**
        * Parses the Jpeg primary image data and initializes the Jpeg primary 
        * image structures.
        * @since 2.6
        * @return void.
        */        
    	void ParseJpegDataL();

        /**
        * Parses the Jpeg thumbnail image data and initializes the Jpeg 
        * thumbnail structures.
        * @since 2.6
        * @return void.
        */        
    	void ParseThumbnailL();

        /**
        * Reads the tag header, and returns the header information in the 
        * parameters.
        * @since 2.6
        * @param aTagId The tag ID read.
        * @param aTagType The tag data type read
        * @param aTagCount The number of tag data elements read.
        * @return Error code.
        */
    	TInt ReadTagHeader( 
            TUint16& aTagId, 
            TUint16& aTagType, 
            TUint32& aTagCount ) const;


	private:

        // Exif data reader/parser
    	CExifEndianBase* iReader;

        // Core Exif data structure storing the IFDs and tags.
    	CExifCore* iExifCore;

	private:

        // CExifModifyImpl (only) needs to access iExifCore member of this 
        // class. Thus, it is defined as friend.
    	friend class CExifModifyImpl;

	private:

        // Exif read options
    	TUint iExifReadOption;

    }; 

#endif      // EXIFREADIMPL_H   
            
// End of File
