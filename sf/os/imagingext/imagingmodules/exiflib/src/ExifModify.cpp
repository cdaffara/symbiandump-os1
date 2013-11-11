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
* Description:  Exif v2.2 file format modifier API implementation
*
*/


// INCLUDE FILES
#include "ExifModifyImpl.h"
#include "ExifReadImpl.h"
#include "ExifTagImpl.h"
#include "ExifCore.h"
#include "ExifCommon.h"

// ============================ CLASS CExifModify ==============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifModify::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CExifModify* CExifModify::NewL( 
    const TDesC8& aExifData, 
    CExifModify::TOperationMode aOperationMode )
    {
    return CExifModifyImpl::NewL( aExifData, STATIC_CAST( TBool, 
        aOperationMode ), CExifModify::ENoOptions );
    }

EXPORT_C CExifModify* CExifModify::NewL( 
    const TDesC8& aExifData, 
    CExifModify::TOperationMode aOperationMode,
    TUint aExifModifyOption )
    {
    return CExifModifyImpl::NewL( aExifData, STATIC_CAST( TBool, 
        aOperationMode ), aExifModifyOption );
    }

// -----------------------------------------------------------------------------
// CExifModify::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CExifModify* CExifModify::NewL()
    {
    return CExifModifyImpl::NewL();
    }

// Destructor
CExifModify::~CExifModify()
    {
    }


// ============================ CLASS CExifModifyImpl ==========================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifModifyImpl::CExifModifyImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifModifyImpl::CExifModifyImpl()
    {
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::ConstructL( const TDesC8& aInData, TBool aCreate, TUint aExifModifyOption )
    {
    iExifModifyOption = aExifModifyOption;
    TUint readOption = CExifRead::ENoOptions;
    if ( aExifModifyOption & CExifModify::ENoJpegParsing )
        {
        readOption |= CExifRead::EFastJpegParsing;
        }
    if ( aExifModifyOption & CExifModify::ENoTagChecking )
        {
        readOption |= CExifRead::ENoTagChecking;
        }
    iExifRead = CExifReadImpl::NewL( aInData, aCreate, readOption );
    iExifCore = iExifRead->iExifCore; 

    if ( !aCreate )
        {
        if ( iExifCore->IfdExists( EIfdGps ) )
            {
            iExifCore->SetTagDataL( KIdGpsVersion, KGpsVersion );
            }
        }
    iExifCore->SetTagDataL( KIdExifVersion, KExifVersion );
    iExifCore->SetTagDataL( KIdFlashPixVersion, KFlashPixVersion );

    // Keep some data from the beginning, middle and end for cross-checking.
    iCrossCheckLength = aInData.Length();
    iCrossCheckBegin = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, aInData.Ptr() ) );
    iCrossCheckMiddle = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, aInData.Ptr() ) + aInData.Length() / 2 );
    iCrossCheckEnd = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, aInData.Ptr() ) + aInData.Length() - 5 );

    User::LeaveIfError( iExifCore->Finalize() );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::ConstructL()
    {
    iExifModifyOption = CExifModify::ENoOptions;
    iExifRead = CExifReadImpl::NewL();
    iExifCore = iExifRead->iExifCore; 

    iExifCore->SetTagDataL( KIdExifVersion, KExifVersion );
    iExifCore->SetTagDataL( KIdFlashPixVersion, KFlashPixVersion );

    // No cross-checking since there is no input data.
    iCrossCheckLength = 0;

    User::LeaveIfError( iExifCore->Finalize() );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifModifyImpl* CExifModifyImpl::NewL( const TDesC8& aExifData, TBool aCreate, TUint aExifModifyOption )
    {
    CExifModifyImpl* self = new( ELeave ) CExifModifyImpl();
    CleanupStack::PushL( self ); 
    self->ConstructL( aExifData, aCreate, aExifModifyOption );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifModifyImpl* CExifModifyImpl::NewL()
    {
    CExifModifyImpl* self = new( ELeave ) CExifModifyImpl();
    CleanupStack::PushL( self ); 
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CExifModifyImpl::~CExifModifyImpl()
    {
    if ( iExifRead )
        {
        delete iExifRead;
        }
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::Reader
// Returns a constant pointer to a CExifRead instance that can be used to 
// parse the associated Exif image.
// -----------------------------------------------------------------------------
//
const CExifRead* CExifModifyImpl::Reader() const
    {
    return iExifRead;
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetTagL
// Inserts/Updates the given tag in the specified IFD structure of the Exif data
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetTagL( 
    TExifIfdType aIfdType, 
    TExifTagInfo aExifTagInfo, 
    const TDesC8& aTagData )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    // Check if ASCII string terminates with NULL character.
    // If not add NULL.
    HBufC8* buffer = NULL;
    if ( ( aExifTagInfo.iDataType == CExifTag::ETagAscii ) && 
        ( aTagData.Ptr()[aTagData.Length() - 1] ) )
        {
        ++aExifTagInfo.iDataCount;
        buffer = HBufC8::NewL( aTagData.Length() + 1 );
        buffer->Des().Copy( aTagData );
        *( CONST_CAST( TUint8*, buffer->Des().Ptr() ) + 
            aTagData.Length() ) = NULL;
        buffer->Des().SetLength( aTagData.Length() + 1  );
        }
    else
        {
        buffer = aTagData.AllocL();
        }
    CleanupStack::PushL( buffer );
    CExifTagImpl* tag = NULL;
    if ( iExifModifyOption & CExifModify::ENoTagChecking )
        {
        tag = CExifTagImpl::NewL( aExifTagInfo, buffer, EFalse );
        }
    else
        {
        tag = CExifTagImpl::NewL( aExifTagInfo, buffer, ETrue );
        }
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );

    switch ( tag->TagInfo().iId )
        {
        case KIdExifIfdPointer:
        case KIdGpsIfdPointer:
        case KIdIntOpIfdPointer:
        case KIdJpegInterchangeFormat:
        case KIdJpegInterchangeFormatLength:
        case KIdExifVersion:
        case KIdGpsVersion:
        case KIdFlashPixVersion:
        case KIdCompression:
            User::Leave( KErrNotSupported );
        default:
            break;
        }

    if ( iExifModifyOption & CExifModify::ENoTagChecking )
        {
        iExifCore->InsertTagL( aIfdType, tag, EFalse );
        }
    else
        {
        iExifCore->InsertTagL( aIfdType, tag, ETrue );
        }
    CleanupStack::Pop( tag );
    if ( aIfdType == EIfdGps )
        {
        iExifCore->SetTagDataL( KIdGpsVersion, KGpsVersion );
        }
    TInt error = iExifCore->Finalize();
    if ( error )
        {
        iExifCore->DeleteTag( aIfdType, aExifTagInfo.iId );
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::DeleteTag
// Removes the tag with the given tag ID from the specified IFD structure in 
// the Exif data.
// -----------------------------------------------------------------------------
//
TInt CExifModifyImpl::DeleteTag( TExifIfdType aIfdType, TUint16 aTagId )
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    switch ( aTagId )
        {
        case KIdExifIfdPointer:
        case KIdGpsIfdPointer:
        case KIdIntOpIfdPointer:
        case KIdJpegInterchangeFormat:
        case KIdJpegInterchangeFormatLength:
        case KIdExifVersion:
        case KIdGpsVersion:
        case KIdFlashPixVersion:
        case KIdCompression:
            return KErrNotSupported;
        default:
            if ( !TExifCommon::IsValidTagId( aTagId ) )
                {
                return KErrNotSupported;
                }
        }

    TInt error = iExifCore->DeleteTag( aIfdType, aTagId );
    if ( error )
        {
        return error;
        }
    return iExifCore->Finalize();
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::DeleteIfd
// Removes the specified IFD structure and all its tags from the Exif data.
// -----------------------------------------------------------------------------
//
TInt CExifModifyImpl::DeleteIfd( TExifIfdType aIfdType )
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    if ( ( aIfdType == EIfd0 ) || ( aIfdType == EIfdExif ) )
        {
        return KErrArgument;
        }

    TInt error = iExifCore->DeleteIfd( aIfdType );
    if ( error )
        {
        return error;
        }
    return iExifCore->Finalize();
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetThumbnailL
// Inserts/Updates the given thumbnail Jpeg image data into the 1st IFD 
// structure in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetThumbnailL( const TDesC8& aThumbnailData )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }
    TDesC8* data = aThumbnailData.AllocL();
    CleanupStack::PushL( data );
    iExifCore->InsertThumbnailL( data );
    CleanupStack::Pop( data );
    TInt error = iExifCore->Finalize();
    if ( error )
        {
        iExifCore->RemoveThumbnail();
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::RemoveThumbnail
// Removes the thumbnail Jpeg image data from the 1st IFD structure in the Exif 
// data.
// -----------------------------------------------------------------------------
//
TInt CExifModifyImpl::RemoveThumbnail()
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }
    
    TInt error = iExifCore->RemoveThumbnail();
    if ( error )
        {
        return error;
        }
    return iExifCore->Finalize();
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::WriteDataL
// Flushes the Exif data into the given data buffer, and releases the internal 
// structures.
// -----------------------------------------------------------------------------
//
HBufC8* CExifModifyImpl::WriteDataL( const TDesC8& aInData )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    // Input Jpeg or Exif data is provided at instantiation (or not)
    TBool inputProvidedEarly = iCrossCheckLength; 

    // Input Jpeg or Exif data is provided for this function (or not)
    TBool inputProvidedLate = aInData.Length();

    if ( inputProvidedEarly )
        {
        if ( inputProvidedLate )
            {
            // Cross-check. Check if content of aInData is identical with 
            // the descriptor provided during instantiation.
            if ( aInData.Length() != iCrossCheckLength )
                {
                User::Leave(KErrArgument);
                }
            TUint32 crossCheck = TExifCommon::Uint32L( 
                CONST_CAST( TUint8*, aInData.Ptr() ) );
            if ( crossCheck != iCrossCheckBegin )
                {
                User::Leave(KErrArgument);
                }
            crossCheck = TExifCommon::Uint32L( 
                CONST_CAST( TUint8*, aInData.Ptr() + aInData.Length() / 2 ) );
            if ( crossCheck != iCrossCheckMiddle )
                {
                User::Leave(KErrArgument);
                }
            crossCheck = TExifCommon::Uint32L( 
                CONST_CAST( TUint8*, aInData.Ptr() + aInData.Length() - 5 ) );
            if ( crossCheck != iCrossCheckEnd )
                {
                User::Leave(KErrArgument);
                }
            }
        }
    else
        {
        if ( inputProvidedLate )
            {
            iExifRead->ParseJpegDataL( aInData );
            }
        }

    User::LeaveIfError( iExifCore->Finalize() );    

    if ( !iExifCore->IsValid() )
        {
        User::Leave( KErrNotReady );
        }

    TUint totalSize = 0;
    if ( inputProvidedLate )
        {
        iExifCore->SetDataStartPtr( CONST_CAST( TUint8*, aInData.Ptr() ) );
        totalSize = iExifCore->TotalSize();
        }
    else
        {
        totalSize = iExifCore->App1Size() + 2;
        }
    // Allocation amount is even for 2-byte alignment
    HBufC8* exifData = HBufC8::NewLC( totalSize + ( totalSize % 2 ) );
    exifData->Des().SetLength( totalSize + ( totalSize % 2 ) );
    TUint pos = 0;
    if ( inputProvidedLate )
        {
        iExifCore->WriteJpegHeaderL( exifData, pos );
        iExifCore->WriteExifDataL( exifData, pos );
        TPtr8 ptr( CONST_CAST( TUint8*, exifData->Ptr() ) + pos, 
            iExifCore->JpegSize() );
        User::LeaveIfError( iExifCore->GetJpegData( ptr ) );
        }
    else
        {
        iExifCore->WriteExifDataL( exifData, pos );
        }
    exifData->Des().SetLength( totalSize );
    CleanupStack::Pop();
    return exifData;
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetImageDescriptionL
// Inserts/Updates given ImageDescription in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetImageDescriptionL( const TDesC8& aImageDescription )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdImageDescription, aImageDescription );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetMakeL
// Inserts/Updates given Make in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetMakeL( const TDesC8& aMake )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdMake, aMake );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetModelL
// Inserts/Updates given Model in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetModelL( const TDesC8& aModel )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdModel, aModel );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetOrientationL
// Inserts/Updates given Orientation in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetOrientationL( TUint16 aOrientation )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdOrientation, aOrientation );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetXResolutionL
// Inserts/Updates given X Resolution in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetXResolutionL( 
    TUint32 aXResolution1, 
    TUint32 aXResolution2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdXResolution, aXResolution1, aXResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetYResolutionL
// Inserts/Updates given Y Resolution in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetYResolutionL( 
    TUint32 aYResolution1, 
    TUint32 aYResolution2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdYResolution, aYResolution1, aYResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetResolutionUnitL
// Inserts/Updates given Resolution Unit in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetResolutionUnitL( TUint16 aResolutionUnit )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdResolutionUnit, aResolutionUnit );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetTransferFunctionL
// Inserts/Updates given Transfer Function in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetTransferFunctionL( const TDesC8& aTransferFunction )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdTransferFunction, aTransferFunction );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetDateTimeL
// Inserts/Updates given Date Time in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetDateTimeL( const TDesC8& aDateTime )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdDateTime, aDateTime );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetYCbCrPositioningL
// Inserts/Updates given YCbCr Positioning in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetYCbCrPositioningL( TUint16 aYCbCrPositioning )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdYCbCrPositioning, aYCbCrPositioning );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetSoftwareL
// Inserts/Updates given Software in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetSoftwareL( const TDesC8& aSoftware )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdSoftware, aSoftware );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetCopyrightL
// Inserts/Updates given Copyright in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetCopyrightL( const TDesC8& aCopyright )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdCopyright, aCopyright );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetExposureTimeL
// Inserts/Updates given Exposure Time in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetExposureTimeL( 
    TUint32 aExposureTime1, 
    TUint32 aExposureTime2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdExposureTime, aExposureTime1, aExposureTime2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetComponentsConfigurationL
// Inserts/Updates given Components Configuration in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetComponentsConfigurationL( 
    TUint8 aFirstComponent, TUint8 aSecondComponent, 
    TUint8 aThirdComponent, TUint8 aFourthComponent ) 
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    TUint32 componentsConfiguration = ( aFourthComponent << 24 ) + 
        ( aThirdComponent << 16 ) + ( aSecondComponent << 8 ) + aFirstComponent;
    iExifCore->SetTagDataL( KIdComponentsConfiguration, 
        componentsConfiguration );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetFlashL
// Inserts/Updates given Flash in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetFlashL( TUint16 aFlash )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdFlash, aFlash );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetColorSpaceL
// Inserts/Updates given Color Space in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetColorSpaceL( TUint16 aColorSpace )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdColorSpace, aColorSpace );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetPixelXDimensionL
// Inserts/Updates given Pixel X Dimension in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetPixelXDimensionL( TUint32 aPixelXDimension )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdPixelXDimension, aPixelXDimension );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetPixelYDimensionL
// Inserts/Updates given Pixel Y Dimension in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetPixelYDimensionL( TUint32 aPixelYDimension )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdPixelYDimension, aPixelYDimension );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetExposureModeL
// Inserts/Updates given Exposure Mode in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetExposureModeL( TUint16 aExposureMode )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdExposureMode, aExposureMode );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetWhiteBalanceL
// Inserts/Updates given White Balance in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetWhiteBalanceL( TUint16 aWhiteBalance )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdWhiteBalance, aWhiteBalance );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetSceneCaptureTypeL
// Inserts/Updates given Scene Capture Type in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetSceneCaptureTypeL( TUint16 aSceneCaptureType )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdSceneCaptureType, aSceneCaptureType );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetExposureProgramL
// Inserts/Updates given Exposure Program in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetExposureProgramL( TUint16 aExposureProgram )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdExposureProgram, aExposureProgram );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetIsoSpeedRatingsL
// Inserts/Updates given Iso Speed Ratings in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetIsoSpeedRatingsL( const TDesC8& aIsoSpeedRatings )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdIsoSpeedRatings, aIsoSpeedRatings );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetDateTimeOriginalL
// Inserts/Updates given Date Time Original in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetDateTimeOriginalL( const TDesC8& aDateTimeOriginal )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdDateTimeOriginal, aDateTimeOriginal );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetDateTimeDigitizedL
// Inserts/Updates given Date Time Digitized in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetDateTimeDigitizedL( const TDesC8& aDateTimeDigitized )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdDateTimeDigitized, aDateTimeDigitized );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetApertureValueL
// Inserts/Updates given Aperture Value in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetApertureValueL( 
    TUint32 aApertureValue1, 
    TUint32 aApertureValue2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( 
        KIdApertureValue, aApertureValue1, aApertureValue2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetExposureBiasValueL
// Inserts/Updates given Exposure Bias Value in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetExposureBiasValueL(
    TInt32 aExposureBiasValue1, 
    TInt32 aExposureBiasValue2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdExposureBiasValue, 
        aExposureBiasValue1, aExposureBiasValue2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetMeteringModeL
// Inserts/Updates given Metering Mode in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetMeteringModeL( TUint16 aMeteringMode )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdMeteringMode, aMeteringMode );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetLightSourceL
// Inserts/Updates given Light Source in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetLightSourceL( TUint16 aLightSource )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdLightSource, aLightSource );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetMakerNoteL
// Inserts/Updates given Maker Note in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetMakerNoteL( const TDesC8& aMakerNote )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdMakerNote, aMakerNote );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetUserCommentL
// Inserts/Updates given User Comment in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetUserCommentL( const TDesC8& aUserComment )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }
    iExifCore->SetTagDataL( KIdUserComment, aUserComment );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetRelatedSoundFileL
// Inserts/Updates given Related Sound File in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetRelatedSoundFileL( const TDesC8& aRelatedSoundFile )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdRelatedSoundFile, aRelatedSoundFile );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetFileSourceL
// Inserts/Updates given FileSource in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetFileSourceL(TInt8 aFileSource )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdFileSource, aFileSource );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetDigitalZoomRatioL
// Inserts/Updates given Digital Zoom Ratio in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetDigitalZoomRatioL( 
    TUint32 aDigitalZoomRatio1, 
    TUint32 aDigitalZoomRatio2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdDigitalZoomRatio, 
        aDigitalZoomRatio1, aDigitalZoomRatio2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetContrastL
// Inserts/Updates given Contrast in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetContrastL( TUint16 aContrast )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdContrast, aContrast );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetSaturationL
// Inserts/Updates given Saturation in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetSaturationL( TUint16 aSaturation )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdSaturation, aSaturation );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetSharpnessL
// Inserts/Updates given Sharpness in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetSharpnessL( TUint16 aSharpness )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdSharpness, aSharpness );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetThumbnailXResolutionL
// Inserts/Updates given Thumbnail X Resolution in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetThumbnailXResolutionL( 
    TUint32 aXResolution1, 
    TUint32 aXResolution2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetThumbnailTagDataL(KIdXResolution, 
        aXResolution1, aXResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetThumbnailYResolutionL
// Inserts/Updates given Thumbnail Y Resolution in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetThumbnailYResolutionL( 
    TUint32 aYResolution1, 
    TUint32 aYResolution2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetThumbnailTagDataL(KIdYResolution, 
        aYResolution1, aYResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetThumbnailResolutionUnitL
// Inserts/Updates given Thumbnail Resolution Unit in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetThumbnailResolutionUnitL( TUint16 aResolutionUnit )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetThumbnailTagDataL(KIdResolutionUnit, aResolutionUnit );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetShutterSpeedValueL
// Inserts/Updates given Shutter Speed Value in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetShutterSpeedValueL( 
    TInt32 aShutterSpeedValue1, 
    TInt32 aShutterSpeedValue2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL(KIdShutterSpeedValue, 
        aShutterSpeedValue1, aShutterSpeedValue2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetBrightnessValueL
// Inserts/Updates given Brightness Value in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetBrightnessValueL( 
    TInt32 aBrightnessValue1, 
    TInt32 aBrightnessValue2 )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL(KIdBrightnessValue, 
        aBrightnessValue1, aBrightnessValue2 );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetCustomRenderedL
// Inserts/Updates given Custom Rendered in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetCustomRenderedL( TUint16 aCustomRendered )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdCustomRendered, aCustomRendered );
    }

// -----------------------------------------------------------------------------
// CExifModifyImpl::SetGainControlL
// Inserts/Updates given Gain Control in the Exif data.
// -----------------------------------------------------------------------------
//
void CExifModifyImpl::SetGainControlL( TUint16 aGainControl )
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    iExifCore->SetTagDataL( KIdGainControl, aGainControl );
    }
