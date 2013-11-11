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


// Exif.Read.001
// Instantiate an Exif reader with valid Exif image
// Created Exif reader instance is returned.
void CExifReadTest::ExifRead001L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	if(!read)
		User::Leave(KErrGeneral);
	delete read;
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Read.002
// Try to instantiate an Exif reader with invalid/corrupted Exif image
// Leaves with proper error code.
void CExifReadTest::ExifRead002L()
	{
	// Various invalid/ corrupted Exif images
	HBufC8* exif = TUtils::ReadFileL(iFs, KInvalidExif1);
	CleanupStack::PushL(exif);
	CExifRead* read = 0;
    TRAPD( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif2);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::CreateDummyBufL(120000);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif3);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif4);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif5);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif7);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif8);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif9);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif10);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif11);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif12);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KInvalidExif13);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

    /* Supported, but uncompressed thumbnail is skipped */
	exif = TUtils::ReadFileL(iFs, KNotSupportedExif1);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
    delete read;
    read = 0;
	CleanupStack::PopAndDestroy(exif);

    /* Supported, but unsupported (uncompressed) thumbnail is skipped */
	exif = TUtils::ReadFileL(iFs, KNotSupportedExif2);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( !error ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
    delete read;
    read = 0;
	CleanupStack::PopAndDestroy(exif);

    // Corrupted image with too big (>64k) data
	exif = TUtils::ReadFileL(iFs, KInvalidExif14);
	CleanupStack::PushL(exif);
    TRAP( error, read = CExifRead::NewL(*exif) );
	if ( ( error != KErrCorrupt ) || ( read ) )
        {
        if ( read )
            {
            delete read;
            }
        if ( error )
            {
            User::Leave( error );
            }
		User::Leave(KErrGeneral);
        }
	CleanupStack::PopAndDestroy(exif);

	}


// Exif.Read.003
// Retrieve a tag existing in the specified IFD from Exif data.
// The requested tag instance (unmodifiable / const) is returned.
void CExifReadTest::ExifRead003L()
	{
	// For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	const CExifTag* tag = read->GetTagL(EIfd0, KIdXResolution);
	if(!tag)
		User::Leave(KErrGeneral);

	tag = read->GetTagL(EIfdExif, KIdColorSpace);
	if(!tag)
		User::Leave(KErrGeneral);

	tag = read->GetTagL(EIfd1, KIdCompression);
	if(!tag)
		User::Leave(KErrGeneral);

	tag = read->GetTagL(EIfdIntOp, 1);
	if(!tag)
		User::Leave(KErrGeneral);

	tag = read->GetTagL(EIfdGps, KIdGpsVersion);
	if(!tag)
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.004
// Try to retrieve a tag which doesn't exist in the Exif data.
// Leaves with proper error code.
void CExifReadTest::ExifRead004L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	const CExifTag* tag = NULL;
	// IFD exists but tag doesn't exist, 
	TRAPD(error, tag = read->GetTagL(EIfd0, KIdArtist));
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	// IFD doesn't exist
	TRAP(error, tag = read->GetTagL(EIfdGps, KIdGpsVersion));
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.005
// Try to retrieve a tag with invalid parameters.
// Leaves with proper error code.
void CExifReadTest::ExifRead005L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	const CExifTag* tag;
	// Tag ID is invalid 
	TRAPD(error, tag = read->GetTagL(EIfdExif, 200));
	if(error != KErrNotSupported)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	// Tag cannot exist in the specified IFD.
	TRAP(error, tag = read->GetTagL(EIfdGps, KIdImageDescription));
	if(error != KErrNotSupported)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.006
// Retrieve the IDs of tags from an IFD in Exif data.
// Returns the tag IDs and the number of IDs
void CExifReadTest::ExifRead006L()
	{
	// For each IFD.
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	TInt no = 0;
	TUint16* tags = read->GetTagIdsL(EIfd0, no);
	if((!tags) || (no != 14))
	{
		if(tags)
			delete tags;
		User::Leave(KErrGeneral);
	}
	delete tags;
	tags = 0;

	tags = read->GetTagIdsL(EIfdExif, no);
	if((!tags) || (no != 41))
	{
		if(tags)
			delete tags;
		User::Leave(KErrGeneral);
	}
	delete tags;
	tags = 0;

	tags = read->GetTagIdsL(EIfd1, no);
	if((!tags) || (no != 6))
	{
		if(tags)
			delete tags;
		User::Leave(KErrGeneral);
	}
	delete tags;
	tags = 0;

	tags = read->GetTagIdsL(EIfdIntOp, no);
	if((!tags) || (no != 4))
	{
		if(tags)
			delete tags;
		User::Leave(KErrGeneral);
	}
	delete tags;
	tags = 0;

	tags = read->GetTagIdsL(EIfdGps, no);
	if((!tags) || (no != 4))
	{
		if(tags)
			delete tags;
		User::Leave(KErrGeneral);
	}
	delete tags;
	tags = 0;

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.007
// Try to retrieve the IDs of tags from an IFD, which doesn't exist in Exif data.
// Leaves with proper error code.
void CExifReadTest::ExifRead007L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	TInt no = 0;
	TUint16* tags = NULL;
	TRAPD(error, tags = read->GetTagIdsL(EIfdGps, no));
	if(tags)
		delete tags;
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.008
// Retrieve the types of IFDs included in Exif data.
// Returns the IFD types and the number of IFDs.
void CExifReadTest::ExifRead008L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	TInt no = 0;
	TExifIfdType* ifds = read->GetIfdTypesL(no);
	if((!ifds) && (no != 4))
		{
		if(ifds)
			delete ifds;
		User::Leave(KErrGeneral);
		}
	delete ifds;
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.009
// Retrieve a copy of existing Exif thumbnail image from Exif data.
// Returns the Exif thumbnail image.
void CExifReadTest::ExifRead009L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	HBufC8* thumbnail = read->GetThumbnailL();
	if(!thumbnail)
		User::Leave(KErrGeneral);

	delete thumbnail;
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.010
// Try to retrieve a copy of Exif thumbnail image from Exif data that doesn't contain compressed Exif thumbnail image.
// Leaves with proper error code.
void CExifReadTest::ExifRead010L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KNoThumbnail);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	HBufC8* thumbnail = NULL;
	TRAPD(error, thumbnail = read->GetThumbnailL());
	if(thumbnail)
		delete thumbnail;
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.011
// Check if an IFD exists in Exif data.
// Returns ETrue if specified IFD exists, EFalse otherwise.
void CExifReadTest::ExifRead011L()
	{
	// For each IFD
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	// IFD exists,
	if(!read->IfdExists(EIfd0))
		User::Leave(KErrGeneral);
	if(!read->IfdExists(EIfdExif))
		User::Leave(KErrGeneral);
	if(!read->IfdExists(EIfd1))
		User::Leave(KErrGeneral);
	if(!read->IfdExists(EIfdIntOp))
		User::Leave(KErrGeneral);
	// IFD doesn't exist.
	if(read->IfdExists(EIfdGps))
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.012
// Check if a tag exists in the specified IFD of Exif data.
// Returns ETrue if tag exists in the specified IFD, EFalse otherwise.
void CExifReadTest::ExifRead012L()
	{
	// For each IFD;
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	// Tag  exists in IFD, 
	if(!read->TagExists(KIdXResolution, EIfd0)) 
		User::Leave(KErrGeneral);
	if(!read->TagExists(KIdComponentsConfiguration, EIfdExif)) 
		User::Leave(KErrGeneral);
	if(!read->TagExists(1, EIfdIntOp)) 
		User::Leave(KErrGeneral);
	// IFD exists but tag doesn't exist,
	if(read->TagExists(KIdCopyright, EIfd1)) 
		User::Leave(KErrGeneral);
	// IFD doesn't exist
	if(read->TagExists(KIdGpsVersion, EIfdGps)) 
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.013
// Retrieve specific data using Get functions.
// Returns the requested data.
void CExifReadTest::ExifRead013L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	// For some Get functions.
	HBufC8* data = read->GetImageDescriptionL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

	data = read->GetMakeL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

	data = read->GetDateTimeOriginalL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetTransferFunctionL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetRelatedSoundFileL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetModelL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetMakerNoteL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetIsoSpeedRatingsL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetDateTimeDigitizedL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;

    data = read->GetCopyrightL();
	if(!data)
		User::Leave(KErrGeneral);
	delete data;
	data = 0;


	TUint16 uInt16 = 0;
	TUint32 uInt32 = 0;
	TUint32 uInt32i = 0;
	TInt32 int32 = 0;
	TInt32 int32i = 0;
	TInt8 int8 = 0;
    TUint8 a1 = 0;
    TUint8 a2 = 0;
    TUint8 a3 = 0;
    TUint8 a4 = 0;

    User::LeaveIfError(read->GetExposureTime(uInt32, uInt32i));
    if((uInt32 != 0) || (uInt32i != 0))
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetShutterSpeedValue(int32, int32i));
    if((int32 != 0x00087383) || (int32i != 0x00010000))
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetBrightnessValue(int32, int32i));
    if((int32 != 0) || (int32i != 1))
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetCustomRendered(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetGainControl(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetGpsVersion(uInt32));
    if(uInt32 != 0x00000202)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetComponentsConfiguration(a1, a2, a3, a4));
    if((a1 != 1) || (a2 != 2) || (a3 != 3 ) || (a4 != 0))
        User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetOrientation(uInt16));
	if(uInt16 != 1)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetXResolution(uInt32, uInt32i));
	if((uInt32 != 180) || (uInt32i != 1))
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetExifIfdPointer(uInt32)); 
	if(uInt32 != 1798)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetFileSource(int8)); 
	if(int8 != 3)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetSharpness(uInt16)); 
	if(uInt16 != 0)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetExifVersion(uInt32)); 
	if(uInt32 != 0x30323230)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetThumbnailXResolution(uInt32, uInt32i));
	if((uInt32 != 180) || (uInt32i != 1))
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetThumbnailCompression(uInt16)); 
	if(uInt16 != 6)
		User::Leave(KErrGeneral);
	User::LeaveIfError(read->GetJpegInterchangeFormat(uInt32)); 
	if(uInt32 != 2814)
		User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetWhiteBalance(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetSceneCaptureType(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetSaturation(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetMeteringMode(uInt16));
    if(uInt16 != 2)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetJpegInterchangeFormatLength(uInt32));
    if(uInt32 != 5342)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetInteroperabilityIfdPointer(uInt32));
    if(uInt32 != 2666)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetGpsInfoIfdPointer(uInt32));
    if(uInt32 != 2612)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetFlashPixVersion(uInt32));
    if(uInt32 != 0x30303130)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetExposureProgram(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetExposureMode(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetExposureBiasValue(int32, int32i));
    if((int32 != 0) || (int32i != 3))
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetContrast(uInt16));
    if(uInt16 != 0)
        User::Leave(KErrGeneral);
    User::LeaveIfError(read->GetApertureValue(uInt32, uInt32i));
    if((uInt32 != 262144) || (uInt32i != 65536))
        User::Leave(KErrGeneral);


	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Read.014
// Try to retrieve specific data that is not present in the Exif data using Get functions.
// Leaves with or returns proper error code.
void CExifReadTest::ExifRead014L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KNoThumbnail);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

	// For some Get functions.
	HBufC8* data = NULL;
	TRAPD(error, data = read->GetSoftwareL());
	if(data)
		delete data;
	data = 0;
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	TRAP(error, data = read->GetUserCommentL());
	if(data)
		delete data;
	data = 0;
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	TUint16 uInt16 = 0;
	TUint32 uInt32 = 0;
	TUint32 uInt32i = 0;
	TInt8 int8 = 0;
	error = read->GetLightSource(uInt16); 
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	error = read->GetFileSource(int8);
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	error = read->GetDigitalZoomRatio(uInt32, uInt32i); 
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	error = read->GetThumbnailCompression(uInt16);
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}
	error = read->GetJpegInterchangeFormat(uInt32);
	if(error != KErrNotFound)
		{
		if(error)
			User::Leave(error);
		else
			User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Read.016
// Retrieve the Exif APP1 segment.
// Returns the whole Exif APP1 segment.
void CExifReadTest::ExifRead016L()
	{
    
	HBufC8* exif = TUtils::ReadFileL(iFs, KValidExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

    HBufC8* appSegment = read->GetExifAppSegmentL();
    CleanupStack::PushL( appSegment );
    if ( appSegment->Length() != 12670 )
        {
        User::Leave( KErrGeneral );
        }
    if ( *appSegment->Ptr() != 0xff )
        {
        User::Leave( KErrGeneral );
        }
    if ( *( appSegment->Ptr() + appSegment->Length() - 1 ) != 0xd9 )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( appSegment );

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

	exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	read = CExifRead::NewL(*exif);
	CleanupStack::PushL(read);

    appSegment = read->GetExifAppSegmentL();
    CleanupStack::PushL( appSegment );
    if ( appSegment->Length() != 8166 )
        {
        User::Leave( KErrGeneral );
        }
    if ( *appSegment->Ptr() != 0xff )
        {
        User::Leave( KErrGeneral );
        }
    if ( *( appSegment->Ptr() + appSegment->Length() - 1 ) != 0xd9 )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( appSegment );

	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Read.MEM.015
// Test the behavior of the previous test cases in OOM situations.
// Successfully operates or leaves with OOM error, without any memory leaks.
void CExifReadTest::ExifRead015L()
	{
	// OOM in EXIF.READ.001- EXIF.READ.014
	TInt error = KErrGeneral;
	TInt i = 0;

	RDebug::Print(_L("ExifRead001L() memory test"));
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
		TRAP(error, ExifRead001L());
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
	RDebug::Print(_L("ExifRead002L() memory test"));
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
		TRAP(error, ExifRead002L());
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
	RDebug::Print(_L("ExifRead003L() memory test"));
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
		TRAP(error, ExifRead003L());
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
	RDebug::Print(_L("ExifRead004L() memory test"));
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
		TRAP(error, ExifRead004L());
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
	RDebug::Print(_L("ExifRead005L() memory test"));
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
		TRAP(error, ExifRead005L());
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
	RDebug::Print(_L("ExifRead006L() memory test"));
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
		TRAP(error, ExifRead006L());
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
	RDebug::Print(_L("ExifRead007L() memory test"));
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
		TRAP(error, ExifRead007L());
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
	RDebug::Print(_L("ExifRead008L() memory test"));
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
		TRAP(error, ExifRead008L());
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
	RDebug::Print(_L("ExifRead009L() memory test"));
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
		TRAP(error, ExifRead009L());
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
	RDebug::Print(_L("ExifRead010L() memory test"));
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
		TRAP(error, ExifRead010L());
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
	RDebug::Print(_L("ExifRead011L() memory test"));
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
		TRAP(error, ExifRead011L());
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
	RDebug::Print(_L("ExifRead012L() memory test"));
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
		TRAP(error, ExifRead012L());
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
	RDebug::Print(_L("ExifRead013L() memory test"));
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
		TRAP(error, ExifRead013L());
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
	RDebug::Print(_L("ExifRead014L() memory test"));
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
		TRAP(error, ExifRead014L());
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
	RDebug::Print(_L("ExifRead016L() memory test"));
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
		TRAP(error, ExifRead016L());
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

// Exif.Read.Fast.017
// Instantiate an Exif reader with valid Exif image
// Created Exif reader instance is returned.
void CExifReadTest::ExifRead017L()
	{
	RDebug::Print(_L("1.Open file KFullExif..."));
	 HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead..."));
	CExifRead* read = CExifRead::NewL(*exif);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	HBufC8* thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
		
	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

	RDebug::Print(_L("2.Open file KFullExif 64k..."));
	exif = TUtils::ReadFileL(iFs, KFullExif, 65536);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead no jpeg..."));
	read = CExifRead::NewL(*exif, CExifRead::ENoJpeg);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

	RDebug::Print(_L("3.Open file KFullExif3mpix..."));
	exif = TUtils::ReadFileL(iFs, KFullExif3mpix);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead..."));
	read = CExifRead::NewL(*exif);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

	RDebug::Print(_L("4.Open file KFullExif3mpix 64k..."));
	exif = TUtils::ReadFileL(iFs, KFullExif3mpix, 65536);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead no jpeg..."));
	read = CExifRead::NewL(*exif, CExifRead::ENoJpeg);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	
	RDebug::Print(_L("delete buffers and exit..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}

// Exif.Read.NOTAGCHECKING.018
// Instantiate an Exif reader with Exif image that has unknown tags or missing mandatory tags
// Created Exif reader instance is returned.
void CExifReadTest::ExifRead018L()
	{
	// file 0
	RDebug::Print(_L("0.Open file KNoTagChk_IMG_AN19..."));
	HBufC8* exif = TUtils::ReadFileL(iFs, KNoTagChk_IMG_AN19);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead and ignore unknown/missing tags..."));
	CExifRead* read = CExifRead::NewL(*exif, CExifRead::ENoTagChecking);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	HBufC8* thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	
	// Save Thumbnail
    /*
	if(thumbnail)
		{
	    RFile file;
    	TBuf<255> fileName;

		fileName.Copy(_L("c:\\NoTagChecking_thumbnail.jpg"));
    	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
				User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));
		RDebug::Print(_L("File opened"));
		file.Write(thumbnail->Des());
		RDebug::Print(_L("File written."));
		file.Close();
		}
	*/	
    TUtils::GetAllTagsTestL( read, KNoTagChk_IMG_AN19 );
	
	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

    // File 1
	RDebug::Print(_L("1.Open file KNoTagChk_Original..."));
	exif = TUtils::ReadFileL(iFs, KNoTagChk_Original);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead and ignore unknown/missing tags..."));
	read = CExifRead::NewL(*exif, CExifRead::ENoTagChecking);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	
	// Save Thumbnail
    /*
	if(thumbnail)
		{
	    RFile file;
    	TBuf<255> fileName;

		fileName.Copy(_L("c:\\NoTagChecking_thumbnail.jpg"));
    	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
				User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));
		RDebug::Print(_L("File opened"));
		file.Write(thumbnail->Des());
		RDebug::Print(_L("File written."));
		file.Close();
		}
	*/	
    TUtils::GetAllTagsTestL( read, KNoTagChk_Original );

	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);

    // File 2
	RDebug::Print(_L("1.Open file KtagInWrongIfd..."));
	exif = TUtils::ReadFileL(iFs, KtagInWrongIfd);
	CleanupStack::PushL(exif);

	RDebug::Print(_L("Instantiate CExifRead and ignore unknown/missing tags..."));
	read = CExifRead::NewL(*exif, CExifRead::ENoTagChecking);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);
	
	RDebug::Print(_L("Read thumbnail..."));
	thumbnail = read->GetThumbnailL();
	CleanupStack::PushL(thumbnail);

	if(thumbnail)
		RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	else
		RDebug::Print(_L("no thumbnail..."));
	
	// Save Thumbnail
    /*
	if(thumbnail)
		{
	    RFile file;
    	TBuf<255> fileName;

		fileName.Copy(_L("c:\\KtagInWrongIfd_thumbnail.jpg"));
    	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
				User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));
		RDebug::Print(_L("File opened"));
		file.Write(thumbnail->Des());
		RDebug::Print(_L("File written."));
		file.Close();
		}
	*/	

	RDebug::Print(_L("delete buffers..."));
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);


	}
