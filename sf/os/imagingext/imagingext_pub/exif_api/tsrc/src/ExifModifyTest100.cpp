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

// Test for fast option

// Exif.Modify.101
// Instantiate an Exif modifier with valid Exif image
// Modified Exif modifier instance in EModify mode is returned.
void CExifModifyTest::ExifModify101L()
	{
    
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	if(!modify)
		User::Leave(KErrGeneral);
	delete modify;
	CleanupStack::PopAndDestroy(exif);
    
    /*
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify);
	if(!modify)
		User::Leave(KErrGeneral);
    modify->SetCustomRenderedL(0);
    HBufC8* b = modify->WriteDataL(*exif);
    RFile file;
	User::LeaveIfError(file.Open(iFs, _L("C:\\FullExif.jpg"), EFileWrite));
 
    file.Write(b->Des());
    file.Close();
    delete b;
*/
	}

// Exif.Modify.102
// Try to instantiate an Exif modifier with invalid/ corrupted Exif image
// Leaves with proper error code.
void CExifModifyTest::ExifModify102L()
	{
	// Various invalid/ corrupted Exif images
	HBufC8* exif = TUtils::ReadFileL(iFs, KInvalidExif1);
	CleanupStack::PushL(exif);
	CExifModify* modify = 0;
    TRAPD( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif2);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif3);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif4);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif5);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif6);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
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
	CleanupStack::PopAndDestroy(exif);

    /* Supported, but uncompressed thumbnail is skipped */
	exif = TUtils::ReadFileL(iFs, KNotSupportedExif1);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
    if ( ( error != KErrNone ) || ( !modify ) )
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
    delete modify;
    modify = NULL;
	CleanupStack::PopAndDestroy(exif);

    /* Supported, but unsupported (uncompressed) thumbnail is skipped */
	exif = TUtils::ReadFileL(iFs, KNotSupportedExif2);
	CleanupStack::PushL(exif);
    TRAP( error, modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing) );
    if ( ( error != KErrCorrupt ) )
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
    delete modify;
	CleanupStack::PopAndDestroy(exif);
    }

// Exif.Modify.103
// Retrieve reader instance for the related Exif data.
// Returns unmodifiable reader instance.
void CExifModifyTest::ExifModify103L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	const CExifRead* reader = modify->Reader();
	if(!reader)
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Modify.104
// Insert/ Update a valid tag into the specified IFD in Exif data.
// The given tag instance is inserted or updated.
void CExifModifyTest::ExifModify104L()
	{
// For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
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
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.105
// Try to insert/ update a valid tag into the specified IFD in Exif data.
// Leaves with proper error code.
void CExifModifyTest::ExifModify105L()
	{
//* Tag is not allowed to be inserted into the specified IFD,
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
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
	buf = TUtils::CreateDummyBufL(65000);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 65000; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
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
	buf = TUtils::CreateDummyBufL(19);
	CleanupStack::PushL(buf);
	tag.iId = KIdImageDescription;
	tag.iDataType = CExifTag::ETagByte;
	tag.iDataCount = 19; 
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
    // !! DROPPED
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

    // Tag cannot be modified
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdExifIfdPointer;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdGpsIfdPointer;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdIntOpIfdPointer;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdJpegInterchangeFormat;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdJpegInterchangeFormatLength;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdExifVersion;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdGpsVersion;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdFlashPixVersion;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);
	buf = TUtils::CreateDummyBufL(4);
	CleanupStack::PushL(buf);
	tag.iId = KIdCompression;
	tag.iDataType = CExifTag::ETagAscii;
	tag.iDataCount = 4; 
	TRAP(error, modify->SetTagL(EIfd0, tag, buf->Des()));
	if(error != KErrNotSupported)
	{
		if(error)
			User::Leave(error);
		User::Leave(KErrGeneral);
	}
	CleanupStack::PopAndDestroy(buf);


	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.106
// Remove a tag from the specified IFD in Exif data.
// Removes the tag from the specified IFD.
void CExifModifyTest::ExifModify106L()
	{
    // For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

    User::LeaveIfError(modify->DeleteTag(EIfd0, KIdXResolution));

    User::LeaveIfError(modify->DeleteTag(EIfdExif, KIdExposureProgram));

    User::LeaveIfError(modify->DeleteTag(EIfd1, KIdResolutionUnit));

    User::LeaveIfError(modify->DeleteTag(EIfdGps, 27));

    User::LeaveIfError(modify->DeleteTag(EIfdIntOp, 1));
    
	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.107
// Try to remove a tag from the specified IFD  in Exif data.
// Returns proper error code.
void CExifModifyTest::ExifModify107L()
	{
    //"For each IFD;
//* IFD exists but tag doesn't exist,
//* IFD doesn't exist"

	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);
    TInt error = modify->DeleteTag(EIfd0, 0x013f);
    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteTag(EIfdExif, KIdMakerNote);
    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteTag(EIfd1, KIdCompression);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteTag(EIfdIntOp, 4);
    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteTag(EIfdGps, 27);
    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    
//* Invalid tag ID
    error = modify->DeleteTag(EIfdExif, 0x0205);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

//* Tag cannot be removed!
    error = modify->DeleteTag(EIfdExif, KIdGpsIfdPointer);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdFlashPixVersion);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdExifIfdPointer);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdIntOpIfdPointer);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdJpegInterchangeFormat);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdGpsVersion);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
    error = modify->DeleteTag(EIfdExif, KIdJpegInterchangeFormatLength);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }    
    error = modify->DeleteTag(EIfdExif, KIdExifVersion);
    if(error != KErrNotSupported)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.108
// Remove an IFD from the Exif data.
// Removes the IFD from the Exif data.
void CExifModifyTest::ExifModify108L()
	{
    // For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

    // Not allowed to delete Ifd0 and ExifIfd
    // User::LeaveIfError(modify->DeleteIfd(EIfd0));
    // User::LeaveIfError(modify->DeleteIfd(EIfdExif));
    User::LeaveIfError(modify->DeleteIfd(EIfd1));
    User::LeaveIfError(modify->DeleteIfd(EIfdGps));
    User::LeaveIfError(modify->DeleteIfd(EIfdIntOp));

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.109
// Try to remove an IFD from the Exif data, which doesn't contain that IFD.
// Returns proper error code.
void CExifModifyTest::ExifModify109L()
	{
    // For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);
    TInt error = modify->DeleteIfd(EIfdGps);

    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteIfd(EIfd0);
    if(error != KErrArgument)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    // IFD cannot be removed
    error = modify->DeleteIfd(EIfd0);
    if( error != KErrArgument)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }

    error = modify->DeleteIfd(EIfdExif);
    if( error != KErrArgument)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.110
// Insert/ Update compressed Exif thumbnail image into the the Exif data.
// Inserts/ Updates the Exif thumbnail image
void CExifModifyTest::ExifModify110L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KNoThumbnail);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	HBufC8* thumbnail = TUtils::ReadFileL(iFs, KValidThumbnail);
	CleanupStack::PushL(thumbnail);
	modify->SetThumbnailL(thumbnail->Des());
	CleanupStack::PopAndDestroy(thumbnail);

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.111
// Try to Insert/ Update compressed Exif thumbnail image with in invalid cases.
// Leaves with proper error code.
void CExifModifyTest::ExifModify111L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

//"* Given thumbnail data is not valid compressed Exif thumbnail image, !! Not supported yet !!
    HBufC8* buf = TUtils::CreateDummyBufL(8000);
	CleanupStack::PushL(buf);
	TRAPD( error, modify->SetThumbnailL(buf->Des()) );
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
	TExifTagInfo tag(KIdImageDescription, CExifTag::ETagAscii, 52000);
    buf = TUtils::CreateDummyBufL(52000);
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
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Modify.112
// Remove compressed Exif thumbnail image from the Exif data.
// Removes the thumbnail image.
void CExifModifyTest::ExifModify112L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

    User::LeaveIfError(modify->RemoveThumbnail());

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Modify.113
// Try to remove compressed Exif thumbnail image from the Exif data.
// Returns proper error code.
void CExifModifyTest::ExifModify113L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KNoThumbnail);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);
    TInt error = modify->RemoveThumbnail();

    if(error != KErrNotFound)
        {
        if(error)
            {
            User::Leave(error);
            }
        User::Leave(KErrGeneral);
        }
        

	CleanupStack::PopAndDestroy(modify);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Modify.114
// Write the final Exif image to a buffer.
// Writes the whole Exif image in a buffer, and returns the buffer.
void CExifModifyTest::ExifModify114L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

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

    tmp5 = 0;
    tmp6 = 1;
    modify->SetShutterSpeedValueL(tmp5, tmp6);

    modify->SetBrightnessValueL(tmp5, tmp6);

    modify->SetCustomRenderedL(0);

    modify->SetGainControlL(0);

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

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
	modify->SetYResolutionL(tmp3, tmp4);
	tmp1 = 2;
	modify->SetResolutionUnitL(tmp1);
	tmp1 = 1;
	modify->SetYCbCrPositioningL(tmp1);

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

	HBufC8* buffer = modify->WriteDataL(exif->Des());
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
	CleanupStack::PopAndDestroy(exif);	
	}


// Exif.Modify.115
// Try to write the final Exif image to a buffer for invalid cases or incorrect original data buffer
// Leaves with proper error code.
void CExifModifyTest::ExifModify115L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

//"* Given buffer content is not the same with the original buffer content.
    TInt error = 0;
	HBufC8* buf = 0;
    HBufC8* buffer = 0;
    
    buf = TUtils::CreateDummyBufL( exif->Length() );
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

    buf = TUtils::CreateDummyBufL( exif->Length() - 5 );
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

    buf = TUtils::CreateDummyBufL( exif->Length() );
    CleanupStack::PushL( buf );
    buf->Des().Copy( exif->Ptr(), exif->Length() / 2 - 10 );
    buf->Des().SetLength( exif->Length() );
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

    buf = TUtils::CreateDummyBufL( exif->Length() );
    CleanupStack::PushL( buf );
    buf->Des().Copy( exif->Ptr(), exif->Length() / 2 + 10 );
    buf->Des().SetLength( exif->Length() );
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

	modify->SetComponentsConfigurationL(1, 2, 3, 0);
	tmp1 = 1;
	modify->SetColorSpaceL(tmp1);
	tmp3 = 768;
	modify->SetPixelYDimensionL(tmp3);
    modify->DeleteTag(EIfdExif, KIdPixelXDimension);


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
	TRAP(error, buffer = modify->WriteDataL(exif->Des()));
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

  	buffer = 0;
	TPtrC8 tmpDes;
    TRAP(error, buffer = modify->WriteDataL( tmpDes ));
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
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.116
// Test the behavior of the previous test cases after the Exif image is written into a buffer (WriteDataL) called.
// Leaves with or returns proper error code.
// CHANGED:
// This feature is changed. Multiple usage is allowed any more. Test using the modifier after WriteDataL function.
void CExifModifyTest::ExifModify116L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifModify* modify = CExifModify::NewL(*exif, CExifModify::EModify, CExifModify::ENoJpegParsing);
	CleanupStack::PushL(modify);

	// Insert Tags
	TUint16 tmp1 = 0;
	TUint32 tmp3 = 0;
	TUint32 tmp4 = 0;

	tmp3 = 72;
	tmp4 = 1;
	modify->SetXResolutionL(tmp3, tmp4);
	tmp1 = 2;
	modify->SetResolutionUnitL(tmp1);

	modify->SetComponentsConfigurationL(1, 2, 3, 0);
	tmp1 = 1;
	modify->SetColorSpaceL(tmp1);
	tmp3 = 768;
	modify->SetPixelYDimensionL(tmp3);
    modify->DeleteTag(EIfdExif, KIdPixelXDimension);

    HBufC8* buffer = NULL;
    TRAPD( error, buffer = modify->WriteDataL( exif->Des() ) );
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

    modify->SetPixelXDimensionL(1024);
    
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

    TRAP( error, buffer = modify->WriteDataL( exif->Des() ) );
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
	fileName.Append(_L("Modify116_01.jpg"));
	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
        {
		if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
            {
            fileName.Copy(KRootC);
	        fileName.Append(_L("out\\"));
	        fileName.Append(_L("Modify116_01.jpg"));
	        if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                {
		        if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
                    {
                    fileName.Copy(KRootData);
	                fileName.Append(_L("out\\"));
	                fileName.Append(_L("Modify116_01.jpg"));
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

    TRAP( error, buffer = modify->WriteDataL( exif->Des() ) );
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
	fileName.Append(_L("Modify116_02.jpg"));
	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
        {
		if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
            {
            fileName.Copy(KRootC);
	        fileName.Append(_L("out\\"));
	        fileName.Append(_L("Modify116_02.jpg"));
	        if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                {
		        if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
                    {
                    fileName.Copy(KRootData);
	                fileName.Append(_L("out\\"));
	                fileName.Append(_L("Modify116_02.jpg"));
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

    TRAP( error, buffer = modify->WriteDataL( exif->Des() ) );
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
	CleanupStack::PopAndDestroy(exif);	
	}

// Exif.Modify.118
// Try to retrieve the Exif APP1 segment when the Exif is not complete yet.
// Leaves with KErrNotReady.
void CExifModifyTest::ExifModify118L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
    CExifModify* modify = CExifModify::NewL( *exif, CExifModify::EModify );
	CleanupStack::PushL( modify );

    modify->DeleteTag( EIfd0, KIdXResolution );
    HBufC8* appSegment = 0;
    TRAPD( error, appSegment = modify->Reader()->GetExifAppSegmentL() );
    if ( (error != KErrNotReady) || ( appSegment ) )
        {
        if ( appSegment )
            delete appSegment;
        User::Leave( KErrGeneral );
        }

	CleanupStack::PopAndDestroy( modify );
	CleanupStack::PopAndDestroy( exif );
	}

// Exif.Modify.119
// Check NULL character at the end of ASCII type tags
// Returns error for invalid cases and behaves properly for other cases.
void CExifModifyTest::ExifModify119L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
    CExifModify* modify = CExifModify::NewL( *exif, CExifModify::EModify );
	CleanupStack::PushL( modify );

	HBufC8* buf = TUtils::CreateDummyBufL(19);
	CleanupStack::PushL(buf);
    buf->Des().Copy(_L("2004:01:19 11:00:00"));
    modify->SetDateTimeOriginalL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
    buf = modify->Reader()->GetDateTimeOriginalL();
    CleanupStack::PushL(buf);
    if ( ( buf->Des().Ptr()[buf->Des().Length() - 1] != NULL ) &&
        ( buf->Des().Length() != 20 ) )
        {
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(20);
	CleanupStack::PushL(buf);
    buf->Des().Copy(_L("2004:01:19 11:00:00\0"));
    modify->SetDateTimeOriginalL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
    buf = modify->Reader()->GetDateTimeOriginalL();
    CleanupStack::PushL(buf);
    if ( ( buf->Des().Ptr()[buf->Des().Length() - 1] != NULL ) &&
        ( buf->Des().Length() != 20 ) )
        {
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(18);
	CleanupStack::PushL(buf);
    buf->Des().Copy(_L("2004:01:19 11:00:0"));
    TRAPD( error, modify->SetDateTimeOriginalL(buf->Des()) );
    if ( error != KErrNotSupported )
        {
        User::LeaveIfError( error );
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(20);
	CleanupStack::PushL(buf);
    buf->Des().Copy(_L("2004:01:19 11:00:001"));
    TRAP( error, modify->SetDateTimeOriginalL(buf->Des()) );
    if ( error != KErrNotSupported )
        {
        User::LeaveIfError( error );
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(12);
	CleanupStack::PushL(buf);
	TExifTagInfo tag(KIdRelatedSoundFile, CExifTag::ETagAscii, 12);
	modify->SetTagL(EIfdExif, tag, buf->Des());
	CleanupStack::PopAndDestroy(buf);
    buf = modify->Reader()->GetRelatedSoundFileL();
    CleanupStack::PushL(buf);
    if ( ( buf->Des().Ptr()[buf->Des().Length() - 1] != NULL ) &&
        ( buf->Des().Length() != 13 ) )
        {
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(13);
	CleanupStack::PushL(buf);
	TExifTagInfo tag2(KIdRelatedSoundFile, CExifTag::ETagAscii, 13);
    *( CONST_CAST( TUint8*, buf->Des().Ptr()) + 12 ) = NULL;
	modify->SetTagL(EIfdExif, tag2, buf->Des());
	CleanupStack::PopAndDestroy(buf);
    buf = modify->Reader()->GetRelatedSoundFileL();
    CleanupStack::PushL(buf);
    if ( ( buf->Des().Ptr()[buf->Des().Length() - 1] != NULL ) &&
        ( buf->Des().Length() != 13 ) )
        {
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(14);
	CleanupStack::PushL(buf);
    *( CONST_CAST( TUint8*, buf->Des().Ptr()) + 13 ) = NULL;
	TExifTagInfo tag3(KIdRelatedSoundFile, CExifTag::ETagAscii, 14);
	TRAP( error, modify->SetTagL(EIfdExif, tag3, buf->Des()) );
    if ( error != KErrNotSupported )
        {
        User::LeaveIfError( error );
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
    buf = 0;

	buf = TUtils::CreateDummyBufL(11);
	CleanupStack::PushL(buf);
	TExifTagInfo tag4(KIdRelatedSoundFile, CExifTag::ETagAscii, 11);
	TRAP( error, modify->SetTagL(EIfdExif, tag4, buf->Des()) );
    if ( error != KErrNotSupported )
        {
        User::LeaveIfError( error );
        User::Leave( KErrGeneral );
        }
	CleanupStack::PopAndDestroy(buf);
    buf = 0;

	CleanupStack::PopAndDestroy( modify );
	CleanupStack::PopAndDestroy( exif );
	}

// Exif.Modify.117
// Test the behavior of the previous test cases in OOM situations.
// Successfully operates or leaves with OOM error, without any memory leaks.
void CExifModifyTest::ExifModify117L()
	{
    // OOM in EXIF.Modify.101- EXIF.Modify.116
	TInt error = KErrGeneral;
	TInt i = 0;

	__UHEAP_SETFAIL( RHeap::ENone, 0 );
	RDebug::Print(_L("ExifModify101L() memory test"));
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
		TRAP(error, ExifModify101L());
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
	RDebug::Print(_L("ExifModify102L() memory test"));
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
		TRAP(error, ExifModify102L());
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
	RDebug::Print(_L("ExifModify103L() memory test"));
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
		TRAP(error, ExifModify103L());
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
	RDebug::Print(_L("ExifModify104L() memory test"));
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
		TRAP(error, ExifModify104L());
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
	RDebug::Print(_L("ExifModify105L() memory test"));
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
		TRAP(error, ExifModify105L());
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
	RDebug::Print(_L("ExifModify106L() memory test"));
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
		TRAP(error, ExifModify106L());
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
	RDebug::Print(_L("ExifModify107L() memory test"));
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
		TRAP(error, ExifModify107L());
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
	RDebug::Print(_L("ExifModify108L() memory test"));
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
		TRAP(error, ExifModify108L());
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
	RDebug::Print(_L("ExifModify109L() memory test"));
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
		TRAP(error, ExifModify109L());
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
	RDebug::Print(_L("ExifModify110L() memory test"));
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
		TRAP(error, ExifModify110L());
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
	RDebug::Print(_L("ExifModify111L() memory test"));
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
		TRAP(error, ExifModify111L());
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
	RDebug::Print(_L("ExifModify112L() memory test"));
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
		TRAP(error, ExifModify112L());
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
	RDebug::Print(_L("ExifModify113L() memory test"));
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
		TRAP(error, ExifModify113L());
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
	RDebug::Print(_L("ExifModify114L() memory test"));
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
		TRAP(error, ExifModify114L());
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
	RDebug::Print(_L("ExifModify115L() memory test"));
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
		TRAP(error, ExifModify115L());
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
	RDebug::Print(_L("ExifModify116L() memory test"));
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
		TRAP(error, ExifModify116L());
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
	RDebug::Print(_L("ExifModify118L() memory test"));
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
		TRAP(error, ExifModify118L());
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
	RDebug::Print(_L("ExifModify119L() memory test"));
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
		TRAP(error, ExifModify119L());
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

