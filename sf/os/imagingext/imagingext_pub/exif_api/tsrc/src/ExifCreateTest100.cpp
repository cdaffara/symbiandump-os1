/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: ExifLibTest
*
*/
#include "ExifLibTestInc.h"


// Exif.Create.101
// Instantiate an Exif modifier with valid Jpeg image
// Created Exif modifier instance in ECreate mode is returned.
void CExifCreateTest::ExifCreate101L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	if(!modify)
		User::Leave(KErrGeneral);
	delete modify;
	CleanupStack::PopAndDestroy(jpeg);
	}

// Exif.Create.102
// Try to instantiate an Exif modifier with invalid/ corrupted Jpeg image
// Leaves with proper error code.
void CExifCreateTest::ExifCreate102L()
	{
	// Various invalid/ corrupted Jpeg images

    // Not baseline Jpeg
	HBufC8* jpeg = TUtils::ReadFileL( iFs, KInvalidJpeg1 );
	CleanupStack::PushL( jpeg );
	CExifModify* modify = 0;
    TRAPD( error, modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing) );
    if ( error ) // error detected
        {
        if ( ( error != KErrNotSupported ) || ( modify ) )
            {
            if ( modify )
                {
                delete modify;
                }
            if ( error )
                {
    		    User::Leave( error );
                }
            User::Leave( KErrGeneral );
            }
        }
    delete modify;
    modify = 0;
	CleanupStack::PopAndDestroy( jpeg );

    // No SOI
	jpeg = TUtils::ReadFileL( iFs, KInvalidJpeg2 );
	CleanupStack::PushL( jpeg );
	TRAP( error, modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing) );
    if ( error ) // error detected
        {
        if ( ( error != KErrCorrupt ) || ( modify ) )
            {
            if ( modify )
                {
                delete modify;
                }
            if ( error )
                {
    		    User::Leave( error );
                }
            User::Leave( KErrGeneral );
            }
        }
    delete modify;
    modify = 0;
	CleanupStack::PopAndDestroy( jpeg );

    // No EOI
	jpeg = TUtils::ReadFileL( iFs, KInvalidJpeg3 );
	CleanupStack::PushL( jpeg );
	TRAP( error, modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing) );
    if ( error ) // error detected
        {
        if ( ( error != KErrCorrupt ) || ( modify ) )
            {
            if ( modify )
                {
                delete modify;
                }
            if ( error )
                {
    		    User::Leave( error );
                }
            User::Leave( KErrGeneral );
            }
        }
    delete modify;
    modify = 0;
	CleanupStack::PopAndDestroy( jpeg );

	jpeg = TUtils::CreateDummyBufL( 120000 );
    CleanupStack::PushL( jpeg );
	HBufC8* jpeg2 = TUtils::ReadFileL( iFs, KValidJpeg );
    jpeg->Des().Copy( jpeg2->Ptr(), jpeg->Length() / 1000 );
    jpeg->Des().SetLength(120000);
    delete jpeg2;
    jpeg2 = 0;
	TRAP( error, modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing) );
    if ( error ) // error detected
        {
        if ( ( error != KErrCorrupt ) || ( modify ) )
            {
            if ( modify )
                {
                delete modify;
                }
            if ( error )
                {
    		    User::Leave( error );
                }
            User::Leave( KErrGeneral );
            }
        }
    delete modify;
    modify = 0;
	CleanupStack::PopAndDestroy( jpeg );
    }

// Exif.Create.103
// Retrieve reader instance for the related Exif data.
// Returns unmodifiable reader instance.
void CExifCreateTest::ExifCreate103L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	const CExifRead* reader = modify->Reader();
	if(!reader)
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);
	}

// Exif.Create.104
// Insert/ Update a valid tag into the specified IFD in Exif data.
// The given tag instance is inserted or updated.
void CExifCreateTest::ExifCreate104L()
	{
//For each IFD.
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);


	HBufC8* buf = TUtils::CreateDummyBufL(16);
	CleanupStack::PushL(buf);
	TExifTagInfo tag(KIdImageDescription, CExifTag::ETagAscii, 16);
	modify->SetTagL(EIfd0, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(20);
	CleanupStack::PushL(buf);
	tag.iId = KIdUserComment;
	tag.iDataType = CExifTag::ETagUndefined;
	tag.iDataCount = 20; 
	modify->SetTagL(EIfdExif, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 0;
	tag.iId = KIdExposureProgram;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 1;
	modify->SetTagL(EIfdExif, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(1);
	CleanupStack::PushL(buf);
	tag.iId = 3;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 1;
	modify->SetTagL(EIfdGps, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(3);
	CleanupStack::PushL(buf);
	tag.iId = 1;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 3;
	modify->SetTagL(EIfdIntOp, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 2;
	tag.iId = KIdResolutionUnit;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 1; 
	modify->SetTagL(EIfd1, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.105
// Try to insert/ update an invalid tag into the specified IFD in Exif data.
// Leaves with proper error code.
void CExifCreateTest::ExifCreate105L()
	{
//* Tag is not allowed to be inserted into the specified IFD,
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);


	HBufC8* buf = TUtils::CreateDummyBufL(16);
	CleanupStack::PushL(buf);
	TExifTagInfo tag(KIdImageDescription, CExifTag::ETagAscii, 16);
	TRAPD(error, modify->SetTagL(EIfdExif, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(20);
	CleanupStack::PushL(buf);
	tag.iId = KIdUserComment;
	tag.iDataType = CExifTag::ETagUndefined;
	tag.iDataCount = 20; 
	TRAP(error, modify->SetTagL(EIfd1, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 0;
	tag.iId = KIdExposureProgram;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 1; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	tag.iId = 3;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 2; 
	TRAP(error, modify->SetTagL(EIfdIntOp, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}

	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(3);
	CleanupStack::PushL(buf);
	tag.iId = 1;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 3; 
	TRAP(error, modify->SetTagL(EIfdExif, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 2;
	tag.iId = KIdResolutionUnit;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 1; 
	TRAP(error, modify->SetTagL(EIfdGps, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

    //* The Exif data size exceeds 64K" 

	buf = TUtils::CreateDummyBufL(66000);
	CleanupStack::PushL(buf);
	tag.iId = 8;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 66000; 
	TRAP(error, modify->SetTagL(EIfdGps, tag, buf->Des()));
	if(error != KErrOverflow)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	// Tag ID is invalid,	
	buf = TUtils::CreateDummyBufL(19);
	CleanupStack::PushL(buf);
	tag.iId = 200;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 19; 
	TRAP(error, modify->SetTagL(EIfdGps, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	//Tag ID doesn't match with the data type, (Full validity)
	//* Not supported yet!!!
	buf = TUtils::CreateDummyBufL(20);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagByte;
	tag.iDataCount = 20; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
        {
        if(error)
            User::Leave(error);
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(buf);
	//*/

	//Data count doesn't match with the given tag data size
	buf = TUtils::CreateDummyBufL(19);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 24; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrArgument)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	//Tag data value is not valid (Full validity) 
    // !!! DROPPED !!!
	/* Not supported yet!!!
	buf = TUtils::CreateDummyBufL(2);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 2; 
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 200;
	TRAP(error, modify->SetTagL(EIfdExif, tag, buf->Des()));
	if(error != KErrArgument)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
    */

    // Data length = 0
    buf = TUtils::CreateDummyBufL(2);
    buf->Des().SetLength(0);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagShort;
	tag.iDataCount = 2; 
	*(reinterpret_cast<TUint16*>(const_cast<TUint8*>(buf->Ptr()))) = 200;
	TRAP(error, modify->SetTagL(EIfdExif, tag, buf->Des()));
	if(error != KErrArgument)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.106
// Insert/ Update compressed Exif thumbnail image into the the Exif data.
// Inserts/ Updates the Exif thumbnail image
void CExifCreateTest::ExifCreate106L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	HBufC8* thumbnail = TUtils::ReadFileL(iFs, KValidThumbnail);
	CleanupStack::PushL(thumbnail);
	modify->SetThumbnailL(thumbnail->Des());
	CleanupStack::PopAndDestroy(thumbnail);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.107
// Try to Insert/ Update compressed Exif thumbnail image with in invalid cases.
// Leaves with proper error code.
void CExifCreateTest::ExifCreate107L()
	{
	TInt error = 0;
    HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
    CExifModify* modify = 0;
    //"* Given thumbnail data is not valid compressed Exif thumbnail image, !! Not supported yet !!
    modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

    HBufC8* buf = TUtils::CreateDummyBufL(8000);
	CleanupStack::PushL(buf);
	TRAP( error, modify->SetThumbnailL(buf->Des()) );
    if ( error != KErrCorrupt )
        {
        if ( error )
            {
            User::Leave( error );
            }
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( buf );
    

//* The Exif data size exceeds 64K"
	TExifTagInfo tag(KIdImageDescription, CExifTag::ETagAscii, 60000);
    buf = TUtils::CreateDummyBufL(60000);
	CleanupStack::PushL(buf);
	modify->SetTagL(EIfd0, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);

	HBufC8* thumbnail = TUtils::ReadFileL(iFs, KValidThumbnail);
	CleanupStack::PushL(thumbnail);
	TRAP( error, modify->SetThumbnailL(thumbnail->Des()) );
    if ( error != KErrOverflow )
        {
        if ( error )
            {
            User::Leave( error );
            }
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(thumbnail);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.108
// Write the final Exif image to a buffer.
// Writes the whole Exif image in a buffer, and returns the buffer.
void CExifCreateTest::ExifCreate108L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	// Insert Tags
	HBufC8* buf = 0;
	TUint16 tmp1 = 0;
	TUint32 tmp3 = 0;
	TUint32 tmp4 = 0;

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
	modify->SetYResolutionL(tmp3, tmp4);
	tmp1 = 2;
	modify->SetResolutionUnitL(tmp1);
	tmp1 = 1;
	modify->SetYCbCrPositioningL(tmp1);

	tmp3 = 0x1230;
	modify->SetComponentsConfigurationL(1, 2, 3, 0);
	tmp1 = 1;
	modify->SetColorSpaceL(tmp1);
	tmp3 = 1024;
	modify->SetPixelXDimensionL(tmp3);
	tmp3 = 768;
	modify->SetPixelYDimensionL(tmp3);


	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	modify->SetImageDescriptionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	
	buf = TUtils::CreateDummyBufL(768*2);
	CleanupStack::PushL(buf);
	modify->SetTransferFunctionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	modify->SetExposureModeL(0);
	modify->SetWhiteBalanceL(0);
	modify->SetSceneCaptureTypeL(0);
	modify->SetExposureProgramL(0);
	
	modify->SetLightSourceL(0);

	buf = TUtils::CreateDummyBufL(16);
	CleanupStack::PushL(buf);
	modify->SetMakerNoteL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	HBufC8* buffer = modify->WriteDataL(jpeg->Des());
	if(!buffer)
        User::Leave(KErrGeneral);
	delete buffer;
	buffer = 0;

    TPtrC8 tmpDes;
    buffer = modify->WriteDataL( tmpDes );
	if(!buffer)
        User::Leave(KErrGeneral);
	delete buffer;

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}


// Exif.Create.109
// Try to write the final Exif image to a buffer for invalid cases or incorrect original data buffer
// Leaves with proper error code.
void CExifCreateTest::ExifCreate109L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

//"* Given buffer content is not the same with the original buffer content
    TInt error = 0;
	HBufC8* buf = 0;
    HBufC8* buffer = 0;
    
    buf = TUtils::CreateDummyBufL( jpeg->Length() );
    CleanupStack::PushL( buf );
    TRAP( error, buffer = modify->WriteDataL( *buf ) );
    
    if ( ( error != KErrArgument ) || ( buffer ) )
        {
        if ( buffer )
            delete buffer;
        if ( error )
            {
            User::Leave( error );
            }
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( buf );

//* The Exif data doesn't contain all mandatory tags"

	// Insert Tags
	TUint16 tmp1 = 0;
	TUint32 tmp3 = 0;
	TUint32 tmp4 = 0;

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
	tmp1 = 2;
	modify->SetResolutionUnitL(tmp1);

	tmp3 = 0x1230;
	modify->SetComponentsConfigurationL(1, 2, 3, 0);
	tmp1 = 1;
	modify->SetColorSpaceL(tmp1);
	tmp3 = 768;
	modify->SetPixelYDimensionL(tmp3);


	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	modify->SetImageDescriptionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	
	buf = TUtils::CreateDummyBufL(768*2);
	CleanupStack::PushL(buf);
	modify->SetTransferFunctionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	modify->SetExposureModeL(0);
	modify->SetWhiteBalanceL(0);
	modify->SetSceneCaptureTypeL(0);
	modify->SetExposureProgramL(0);
	
	modify->SetLightSourceL(0);

	buf = TUtils::CreateDummyBufL(16);
	CleanupStack::PushL(buf);
	modify->SetMakerNoteL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buffer = 0;
	TRAP(error, buffer = modify->WriteDataL(jpeg->Des()));
	if(buffer)
		{
		delete buffer;
		User::Leave(KErrGeneral);
		}
	if(error != KErrNotReady)
	    {
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	    }

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.110
// Insert/Update specific data using Set functions.
// Inserts/ Updates the tag data
void CExifCreateTest::ExifCreate110L()
	{
//For each Set function.
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	// For some Set functions.
	// Insert Tags
	HBufC8* buf = 0;
    TUint16 tmp1 = 0;
	TUint32 tmp3 = 0;
	TUint32 tmp4 = 0;
    TInt32 tmp5 = 0;
    TInt32 tmp6 = 0;

	buf = TUtils::CreateDummyBufL(10);
	CleanupStack::PushL(buf);
    modify->SetUserCommentL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
    modify->SetModelL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

    tmp1 = 0;
    modify->SetMeteringModeL(tmp1);

    tmp5 = 1;
    tmp6 = 1;
    modify->SetExposureBiasValueL(tmp5, tmp6);

	buf = TUtils::CreateDummyBufL(19);
	CleanupStack::PushL(buf);
    buf->Des().Copy(_L("2004:01:19 11:00:00"));
    modify->SetDateTimeOriginalL(buf->Des());
    modify->SetDateTimeDigitizedL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

    tmp5 = 1;
    tmp6 = 1;
    modify->SetApertureValueL(tmp5, tmp6);
    
	buf = TUtils::CreateDummyBufL(12);
	CleanupStack::PushL(buf);
	modify->SetImageDescriptionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);

	buf = TUtils::CreateDummyBufL(12);
	CleanupStack::PushL(buf);
	modify->SetMakeL(buf->Des());
	CleanupStack::PopAndDestroy(buf);

	modify->SetOrientationL(1);
	modify->SetExposureModeL(0);
	modify->SetWhiteBalanceL(0);
	modify->SetSceneCaptureTypeL(0);
	modify->SetExposureProgramL(0);

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
	modify->SetThumbnailYResolutionL(tmp3, tmp4);
	modify->SetFileSourceL(1); 
	modify->SetSharpnessL(0); 

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.111
// Try to Insert/Update specific data using Set functions in invalid cases.
// Leaves with or returns proper error code.
void CExifCreateTest::ExifCreate111L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);
//"* Data size is invalid 
	HBufC8* buf = 0;
	buf = TUtils::CreateDummyBufL(700);
	CleanupStack::PushL(buf);
	TRAPD(error, modify->SetTransferFunctionL(buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);

//* Data value is invalid" (Full validity) 
    // !!! DROPPED !!!

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.112
// Test the behavior of the previous test cases after the Exif image is written into a buffer (WriteDataL) called.
// Leaves with or returns proper error code.
// CHANGED:
// This feature is changed. Multiple usage is allowed any more. Test using the modifier after WriteDataL function.
void CExifCreateTest::ExifCreate112L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KValidJpeg);
	CleanupStack::PushL(jpeg);
	CExifModify* modify = CExifModify::NewL(*jpeg, CExifModify::ECreate, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	// Insert Tags
	TUint16 tmp1 = 0;
	TUint32 tmp3 = 0;
	TUint32 tmp4 = 0;

    modify->SetYCbCrPositioningL(2);
	modify->SetComponentsConfigurationL(1, 2, 3, 0);
	tmp1 = 1;
	modify->SetColorSpaceL(tmp1);
	tmp3 = 480;
	modify->SetPixelYDimensionL(tmp3);

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
    modify->SetYResolutionL(tmp3, tmp4);
	tmp1 = 2;
	modify->SetResolutionUnitL(tmp1);

    HBufC8* buffer = NULL;
    TRAPD( error, buffer = modify->WriteDataL( jpeg->Des() ) );
    if( (error != KErrNotReady) || (buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }

    TPtrC8 tmpDes;
    TRAP( error, buffer = modify->WriteDataL( tmpDes ) );
    if( (error != KErrNotReady) || (buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }

    modify->SetPixelXDimensionL(640);
    
    TRAP( error, buffer = modify->WriteDataL( tmpDes ) );
    if( (error != KErrNone) || (!buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }
    delete buffer;
    buffer = 0;

    TRAP( error, buffer = modify->WriteDataL( jpeg->Des() ) );
    if( (error != KErrNone) || (!buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }

    CleanupStack::PushL(buffer);
    RFile file;
	TBuf<255> fileName;
    fileName.Copy(KRootOut);
	fileName.Append(_L("out\\"));
	fileName.Append(_L("Create112_01.jpg"));
	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
        {
		if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
            {
            fileName.Copy(KRootC);
	        fileName.Append(_L("out\\"));
	        fileName.Append(_L("Create112_01.jpg"));
	        if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                {
		        if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
                    {
                    fileName.Copy(KRootData);
	                fileName.Append(_L("out\\"));
	                fileName.Append(_L("Create112_01.jpg"));
	                if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                        {
                        User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));    
                        }
                    }
                }
            }
        }
 
    file.Write(buffer->Des());
    file.Close();
    CleanupStack::PopAndDestroy(buffer);
    buffer = 0;

    TRAP( error, buffer = modify->WriteDataL( tmpDes ) );
    if( (error != KErrNone) || (!buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }
    delete buffer;
    buffer = 0;


    TRAP( error, buffer = modify->WriteDataL( jpeg->Des() ) );
    if( (error != KErrNone) || (!buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }

    CleanupStack::PushL(buffer);
    fileName.Copy(KRootOut);
	fileName.Append(_L("out\\"));
	fileName.Append(_L("Create112_02.jpg"));
	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
        {
		if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
            {
            fileName.Copy(KRootC);
	        fileName.Append(_L("out\\"));
	        fileName.Append(_L("Create112_02.jpg"));
	        if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                {
		        if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
                    {
                    fileName.Copy(KRootData);
	                fileName.Append(_L("out\\"));
	                fileName.Append(_L("Create112_02.jpg"));
	                if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                        {
                        User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));    
                        }
                    }
                }
            }
        }
 
    file.Write(buffer->Des());
    file.Close();
    CleanupStack::PopAndDestroy(buffer);
    buffer = 0;

    modify->DeleteTag(EIfdExif, KIdPixelXDimension);

    TRAP( error, buffer = modify->WriteDataL( jpeg->Des() ) );
    if( (error != KErrNotReady) || (buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }    

    TRAP( error, buffer = modify->WriteDataL( tmpDes ) );
    if( (error != KErrNotReady) || (buffer) )
        {
        if( buffer )
            delete buffer;
        buffer = 0;
        if( error )
            {
            User::Leave( error);
            }
        User::Leave( KErrGeneral );
        }    

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(jpeg);	
	}

// Exif.Create.MEM.013
// Test the behavior of the previous test cases in OOM situations.
// Successfully operates or leaves with OOM error, without any memory leaks.
void CExifCreateTest::ExifCreate113L()
	{
//OOM in EXIF.Create.101- EXIF.Create.112
	TInt error = KErrGeneral;
	TInt i = 0;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate101L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate102L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate103L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate104L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate105L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate106L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate107L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate108L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate109L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate110L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate111L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 500; i++)
#else
    for(i = 1; i < 10; i++)
#endif
		{
		__UHEAP_MARK;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifCreate112L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}
	__UHEAP_SETFAIL( RHeap::ENone, 0 );
	}
