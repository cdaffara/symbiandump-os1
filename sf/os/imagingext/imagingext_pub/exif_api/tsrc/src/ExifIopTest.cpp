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

const TInt gpsTagsNo = 3;
const TUint gpsTags[3][4] =	{	
								{5,1,1,0}, 
								{27,7,1,0},
								{28,7,1,0}
							};
// Exif.Iop.001
// Parse the data of various Exif images 
// Writes the Exif data and the thumbnail image of each image into associated files.
void CExifIopTest::ExifIop001L()
	{
//Various Exif images having different versions and content.
	TFindFile file_finder(iFs); 
    CDir* file_list; 
    TInt error = file_finder.FindWildByDir(_L("Exif*.jpg"), KIopDir, file_list);
    while (error == KErrNone)
        {
		CleanupStack::PushL(file_list);
        TInt i;
        for (i=0; i<file_list->Count(); i++) 
            {
            TParse fullentry;
            fullentry.Set((*file_list)[i].iName,& file_finder.File(),NULL); 
  			ExifIop1L(fullentry.FullName());
            }
		CleanupStack::PopAndDestroy(file_list);;
        error = file_finder.FindWild(file_list); 
        }	
    if(error == KErrNoMemory)
        {
        User::Leave(KErrNoMemory);
        }
	}


// Exif.Iop.002
// Modify the data of various Exif images.
// Writes out the modified Exif images into new files.
void CExifIopTest::ExifIop002L()
	{
//Various Exif images having different versions and content.
	TFindFile file_finder(iFs); 
    CDir* file_list; 

    TInt error = file_finder.FindWildByDir(_L("Exif*.jpg"), KIopDir, file_list);
    while (error == KErrNone)
        {
		CleanupStack::PushL(file_list);
        TInt i;
        for (i=0; i<file_list->Count(); i++) 
            {
            TParse fullentry;
            fullentry.Set((*file_list)[i].iName,& file_finder.File(),NULL); 
  			ExifIop2L(fullentry.FullName());
            }
		CleanupStack::PopAndDestroy(file_list);;
        error = file_finder.FindWild(file_list); 
        }
    if(error == KErrNoMemory)
        {
        User::Leave(KErrNoMemory);
        }
	}

// Exif.Iop.003
// Create new Exif images
// Writes out the new Exif images into new files.
void CExifIopTest::ExifIop003L()
	{
//Various Jpeg images having different content.
	TFindFile file_finder(iFs); 
    CDir* file_list; 
	HBufC8* buffer2 = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(buffer2);
	CExifRead* exifRead = 0;
	exifRead = CExifRead::NewL(*buffer2);
	CleanupStack::PushL(exifRead);

    TInt error = file_finder.FindWildByDir(_L("*.jpg"), KIopDir, file_list);
    while (error == KErrNone)
        {
		CleanupStack::PushL(file_list);
        TInt i;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
        for (i=0; i<file_list->Count(); i++) 
#else
        for (i=0; i<76; i++) 
#endif
            {
            TParse fullentry;
            fullentry.Set((*file_list)[i].iName,& file_finder.File(),NULL); 
  			ExifIop3L(exifRead, fullentry.FullName());
            }
		CleanupStack::PopAndDestroy(file_list);;
//#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
//        error = file_finder.FindWild(file_list); 
//#else
        error = KErrNotFound;
//#endif
        }	
    if(error == KErrNoMemory)
        {
        User::Leave(KErrNoMemory);
        }
	CleanupStack::PopAndDestroy(exifRead);
	CleanupStack::PopAndDestroy(buffer2);
	}

// Exif.Iop.MEM.004
// Test the behavior of the previous test cases in OOM situations.
// Successfully operates or leaves with OOM error, without any memory leaks.
void CExifIopTest::ExifIop004L()
	{
	// OOM in EXIF.IOP.001- EXIF.IOP.003

	iWrite = EFalse;
	TInt error = KErrGeneral;
	TInt i = 0;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(i = 1; i < 1500; i+=30)
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
		TRAP(error, ExifIop001L());
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
	for(i = 1; i < 1500; i+=30)
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
		TRAP(error, ExifIop002L());
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
	for(i = 1; i < 1500; i+=30)
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
		TRAP(error, ExifIop003L());
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
	}

// Exif.Iop.005
// Test case for CameraAPI project.
void CExifIopTest::ExifIop005L()
	{
	HBufC8* jpeg = TUtils::ReadFileL(iFs, KCameraAPIJpeg);
	CleanupStack::PushL(jpeg);

    // Create EXIF modifier object.
    CExifModify* exifModifier = CExifModify::NewL( jpeg->Des(), CExifModify::ECreate );
    
    CleanupStack::PushL( exifModifier );
    // Mandatory EXIF tags:

    // EXIF tag: Orientation
    exifModifier->SetOrientationL( 1 );

    // EXIF tag: YCbCrPositioning
    exifModifier->SetYCbCrPositioningL( 1 );

    // EXIF tag: XResolution
    exifModifier->SetXResolutionL( 300, 1 );
    
    // EXIF tag: YResolution
    exifModifier->SetYResolutionL( 300, 1 );

    // EXIF tag: ResolutionUnit
    exifModifier->SetResolutionUnitL( 2 );
    
    // EXIF tag: ColorSpace
    exifModifier->SetColorSpaceL( 1 );
    
    // EXIF tag: ComponentsConfiguration
    exifModifier->SetComponentsConfigurationL( 1, 2, 3, 0 );
    
    TSize size(1152, 864);

    // EXIF tag: PixelXDimension
    exifModifier->SetPixelXDimensionL( size.iWidth );
    
    // EXIF tag: PixelYDimension
    exifModifier->SetPixelYDimensionL( size.iHeight );

    // Optional EXIF tags:

    // Get and format home time
    TBuf8<20> timeStr;
    TTime time;
    TDateTime dateTime;
    time.HomeTime();
    dateTime=time.DateTime();
    _LIT8(KFormatTxt,"%4d:%02d:%02d %02d:%02d:%02d\0");
    timeStr.Format( KFormatTxt,
                    dateTime.Year(),
                    // Day and month ranges begin at zero (0-30 and 0-11), 
                    // so add one when formatting
                    TInt(dateTime.Month()+1), 
                    dateTime.Day()+1, 
                    dateTime.Hour(), 
                    dateTime.Minute(), 
                    dateTime.Second() );

    // EXIF tag: DateTimeOriginal
    exifModifier->SetDateTimeOriginalL( timeStr );

    // EXIF tag: DateTimeDigitized
    exifModifier->SetDateTimeDigitizedL( timeStr );

    // Get pointer to ancillary data
    TUint8 timeByte1, timeByte2, timeByte3, fpsByte;
    
    // Read fps value from ancillary data
    fpsByte = 100;

    // Read exposure time(1) bytes
    timeByte1 = 1;
    timeByte2 = 2;
    timeByte3 = 3;

    // Concatenate exposure time bytes together
    TUint32 timeHorizPeriods;
    timeHorizPeriods = ( (timeByte3 & 0x07) << 14 ) | 
                       ( (timeByte2 & 0x7F) << 7 )  | 
                         (timeByte1 & 0x7F);

    TInt horizPeriodsInSec;
    if ( fpsByte & 0x01 )
        {
        // 15 fps
        horizPeriodsInSec = 15;
        }
    else 
        {
        // 7.5 fps
        horizPeriodsInSec = 15/2;
        }

    // Exposure time = 1/2 periods (=never happens?)
    if ( !timeHorizPeriods )
        {
        timeHorizPeriods = 1;
        horizPeriodsInSec *= 2;
        }

    // EXIF tag: ExposureTime 
    exifModifier->SetExposureTimeL( timeHorizPeriods, horizPeriodsInSec );

    // Scene capture type can be normal or night
    exifModifier->SetSceneCaptureTypeL( 0 );

    // EXIF tag: Make 
    exifModifier->SetMakeL( _L8("NOKIA") );
    exifModifier->SetModelL( _L8("66xx") );

    // Setting FNumber does not have custom method in EXIF lib -> must use SetTagL()
	TExifTagInfo fNumberTag( 0x829d , CExifTag::ETagRational, 1 ); // Tag id = 0x829d, data type = Rational, Data count = 1
    TBuf8<8> tagValue;
    tagValue.SetLength(8);
    *reinterpret_cast<TUint32*>(const_cast<TUint8*>(tagValue.Ptr())) = 28;
    *reinterpret_cast<TUint32*>(const_cast<TUint8*>(tagValue.Ptr()+4)) = 10;

    // EXIF tag: FNumber 
	exifModifier->SetTagL( EIfdExif, fNumberTag, tagValue ); // The tag goes into the Exif IFD.
    
    // Setting FocalLength does not have custom method in EXIF lib -> must use SetTagL()
	TExifTagInfo focalLengthTag( 0x920a, CExifTag::ETagRational, 1 ); // Tag id = 0x920a, data type = Rational, Data count = 1
    *reinterpret_cast<TUint32*>(const_cast<TUint8*>(tagValue.Ptr())) = 37;
    *reinterpret_cast<TUint32*>(const_cast<TUint8*>(tagValue.Ptr()+4)) = 10;

    // EXIF tag: FocalLength 
	exifModifier->SetTagL( EIfdExif, focalLengthTag, tagValue ); // The tag goes into the Exif IFD.
    
    // EXIF tag: ExposureMode 
    exifModifier->SetExposureModeL( 0 ); // auto

    // EXIF tag: WhiteBalance 
    exifModifier->SetWhiteBalanceL( 0 ); // auto

    TExifTagInfo tag( 1, CExifTag::ETagAscii, 3 );
    exifModifier->SetTagL( EIfdIntOp, tag, _L8("R98") );
    tag.iDataCount = 4;
    tag.iDataType = CExifTag::ETagUndefined;
    tag.iId = 2;
    TBuf8<4> buf;
    buf.Copy( _L8("0100") );
    exifModifier->SetTagL( EIfdIntOp, tag, buf );

	HBufC8* thumbnail = TUtils::ReadFileL(iFs, KValidThumbnail);
	CleanupStack::PushL(thumbnail);
	exifModifier->SetThumbnailL(thumbnail->Des());
	CleanupStack::PopAndDestroy(thumbnail);
    exifModifier->SetThumbnailResolutionUnitL( 2 );
    exifModifier->SetThumbnailXResolutionL( 72, 1 );
    exifModifier->SetThumbnailYResolutionL( 72, 1 );


    TReal tmp = 1;

    TReal isoSpeedRating = 1600;	

    // Round isoSpeedRating to one of the values listed in
    // ISO 12232:1998
    const TUint16 isoSpeedRatingsLut[35] = { 4, 5, 6, 8, 10, 12, 16, 20, 25, 32, 40, 50, 64, 80,
                                             100, 125, 160, 200, 250, 320, 400, 500, 640, 800,
                                             1000, 1250, 1600, 2000, 2500, 3200, 4000, 5000, 6400, 8000, 
                                             10000 };  

    if ( isoSpeedRating > 10000 )
        {
        isoSpeedRating = 10000;
        }
    else
        {
        TInt i=1;
        while ( isoSpeedRatingsLut[i] < isoSpeedRating )
            {
            i++;
            }
        isoSpeedRating = isoSpeedRatingsLut[i-1];
        }

    // Put the iso speed rating value in a descriptor
    TBuf8<2> isoSpeedDes;
    isoSpeedDes.SetLength(2);
    *reinterpret_cast<TUint16*>(const_cast<TUint8*>(isoSpeedDes.Ptr())) = static_cast<TUint16>(isoSpeedRating);
    
    // EXIF tag: IsoSpeedRatings
    exifModifier->SetIsoSpeedRatingsL( isoSpeedDes );

    // Setting ShutterSpeedValue does not have custom method in EXIF lib -> must use SetTagL()
	TExifTagInfo shutterSpeedValueTag( 0x9201, CExifTag::ETagSrational, 1 ); // Tag id = 0x9201, data type = Rational, Data count = 1

    // Log( exposure time )
    *reinterpret_cast<TInt32*>(const_cast<TUint8*>(tagValue.Ptr())) =  static_cast<TInt32>(-1*tmp/0.00030103);// =-1*tmp/(log(2)/1000)
    *reinterpret_cast<TInt32*>(const_cast<TUint8*>(tagValue.Ptr()+4)) = static_cast<TInt32>(1000);

    // EXIF tag: ShutterSpeedValue
	exifModifier->SetTagL( EIfdExif, shutterSpeedValueTag, tagValue ); // The tag goes into the Exif IFD.

    // EXIF tag: ApertureValue
    exifModifier->SetApertureValueL( 30, 10 );
    
    // EXIF tag: DigitalZoomRatio
    exifModifier->SetDigitalZoomRatioL( 100, 100 );

    // Setting GainControl does not have custom method in EXIF lib -> must use SetTagL()
	TExifTagInfo gainControlTag( 0xa407, CExifTag::ETagShort, 1 ); // Tag id = 0xa407, data type = Short, Data count = 1

    // Put the gain control value in a descriptor
    TBuf8<2> gainControlDes;
    gainControlDes.SetLength(2);
    TReal analogGain = 3;
    if ( analogGain < -0.1 )
        {
        *reinterpret_cast<TUint16*>(const_cast<TUint8*>(gainControlDes.Ptr())) = static_cast<TUint16>( 3 ); // low gain down
        }
    else if ( analogGain >= -0.1 && analogGain < 0.1 )
        {
        *reinterpret_cast<TUint16*>(const_cast<TUint8*>(gainControlDes.Ptr())) = static_cast<TUint16>( 0 ); // none
        }
    else if ( analogGain >= 0.1 && analogGain < 6.0 )
        {
        *reinterpret_cast<TUint16*>(const_cast<TUint8*>(gainControlDes.Ptr())) = static_cast<TUint16>( 1 ); // low gain up
        }
    else // analogGain => 6.0 
        {
        *reinterpret_cast<TUint16*>(const_cast<TUint8*>(gainControlDes.Ptr())) = static_cast<TUint16>( 2 ); // high gain up
        }

    // EXIF tag: GainControl
	exifModifier->SetTagL( EIfdExif, gainControlTag, gainControlDes ); // The tag goes into the Exif IFD.

    // EXIF tag: LightSource
    exifModifier->SetLightSourceL( 0 ); // unknown

    HBufC8* exif = exifModifier->WriteDataL(jpeg->Des());
    CleanupStack::PushL(exif);

    RFile file;
	TBuf<255> fileName;

    fileName.Copy(KRootOut);
	fileName.Append(_L("out\\"));
    // Create out folder, ignore errors
   	iFs.MkDir(fileName);
	fileName.Append(_L("CameraApi.jpg"));
	if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
        {
		if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
            {
            fileName.Copy(KRootC);
	        fileName.Append(_L("out\\"));
	        // Create out folder, ignore errors
        	iFs.MkDir(fileName);
	        fileName.Append(_L("CameraApi.jpg"));
	        if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                {
		        if(file.Create(iFs, fileName, EFileWrite) != KErrNone)
                    {
                    fileName.Copy(KRootData);
	                fileName.Append(_L("out\\"));
        	        // Create out folder, ignore errors
                	iFs.MkDir(fileName);
	                fileName.Append(_L("CameraApi.jpg"));
	                if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
                        {
                        User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));    
                        }
                    }
                }
            }
        }
 

    file.Write(exif->Des());
    file.Close();
    CleanupStack::PopAndDestroy(exif);
    CleanupStack::PopAndDestroy(exifModifier);
    CleanupStack::PopAndDestroy(jpeg);
    }

void CExifIopTest::ExifIop1L(const TDesC& aFileName)
	{
	RDebug::Print(aFileName);
	//RDebug::Print(_L("Read File..."));
	HBufC8* buffer = TUtils::ReadFileL(iFs,aFileName);
	CleanupStack::PushL(buffer);

	//RDebug::Print(_L("Create reader.."));
	CExifRead* exifRead = 0;
	exifRead = CExifRead::NewL(*buffer);
	CleanupStack::PushL(exifRead);

	//RDebug::Print(_L("Get ifds.."));
	TInt noIfds = 0;
	TExifIfdType* ifds = exifRead->GetIfdTypesL(noIfds);
	CleanupStack::PushL(ifds);
	if((!ifds) || (!noIfds))
		return;

	//RDebug::Print(_L("Create file.."));
	RFile file;
	TBuf<255> fileName;
	TInt flen = KIopDir.iTypeLength; // folder length
	if(iWrite)
		{
		fileName.Copy(aFileName);
		fileName.Insert(flen,_L("out\\"));
        // Create out folder, ignore errors
       	iFs.MkDir(fileName);
		fileName.Append(_L(".txt"));
		if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
			User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));
		}

	for(TInt i=0; i<noIfds; ++i)
		{
		if(iWrite)
			{
			User::LeaveIfError(file.Write(_L8("______________________________________________\n")));
			switch(ifds[i])
				{
				case EIfd0:
					User::LeaveIfError(file.Write(_L8("Ifd 0:\n\n")));
					break;
				case EIfdExif:
					User::LeaveIfError(file.Write(_L8("Ifd Exif:\n\n")));
					break;
				case EIfd1:
					User::LeaveIfError(file.Write(_L8("Ifd 1:\n\n")));
					break;
				case EIfdGps:
					User::LeaveIfError(file.Write(_L8("Ifd GPS:\n\n")));
					break;
				case EIfdIntOp:
					User::LeaveIfError(file.Write(_L8("Ifd IOP:\n\n")));
				}
			}
		
		TInt noTags = 0;

		//RDebug::Print(_L("Retrieve tag ids..."));
		TUint16* tags = exifRead->GetTagIdsL(ifds[i], noTags);
		//RDebug::Print(_L("Retrieved tag ids."));
		CleanupStack::PushL(tags);
		if((!tags) || (!noTags))
			continue;

		//RDebug::Print(_L("Write tags..."));
		for(TInt j=0; j<noTags; ++j)
			{
			TBuf8<48> tagHeader;
			tagHeader.Insert(0, _L8("Inf: "));
			TBuf8<8> num;
			TInt id = (exifRead->GetTagL(ifds[i], tags[j]))->TagInfo().iId;
			num.Num(id, EHex);
			tagHeader.Append(num);
			tagHeader.Append(_L("\t"));
			num.Num((exifRead->GetTagL(ifds[i], tags[j]))->TagInfo().iDataType);
			tagHeader.Append(num);
			tagHeader.Append(_L("\t"));
			num.Num(static_cast<TUint16>( (exifRead->GetTagL(ifds[i], tags[j]))->TagInfo().iDataCount));
			tagHeader.Append(num);
			tagHeader.Append(_L("\t"));
			if(iWrite)
				{
				User::LeaveIfError(file.Write(tagHeader));
				User::LeaveIfError(file.Write((exifRead->GetTagL(ifds[i], tags[j]))->Data()));
				User::LeaveIfError(file.Write(_L8("\n")));
				}
			}
		CleanupStack::PopAndDestroy(tags);
		}
	if(iWrite)
		file.Close();

	//Thumbnail
	TInt error;
	HBufC8* thumbnail = 0;
	//RDebug::Print(_L("Retrieve thumnail..."));
	TRAP(error, thumbnail = exifRead->GetThumbnailL());
	CleanupStack::PushL(thumbnail);
	if(!error)
		{
		//RDebug::Print(_L("Thumbnail retrieved"));
		fileName.Copy(aFileName);
		fileName.Insert(flen,_L("out\\"));
        // Create out folder, ignore errors
       	iFs.MkDir(fileName);
		fileName.Append(_L(".thm.jpg"));
		//RDebug::Print(_L("Open file..."));
		if(iWrite)
			{
			if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
				User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));
			}
		//RDebug::Print(_L("File opened"));
		if(iWrite)
			file.Write(thumbnail->Des());
		//RDebug::Print(_L("File written."));
		if(iWrite)
			file.Close();
		}
	CleanupStack::PopAndDestroy(thumbnail);
	CleanupStack::PopAndDestroy(ifds);
	CleanupStack::PopAndDestroy(exifRead);
	CleanupStack::PopAndDestroy(buffer);
	}

void CExifIopTest::ExifIop2L(const TDesC& aFileName)
	{
	RDebug::Print(aFileName);
	HBufC8* buffer = TUtils::ReadFileL(iFs, aFileName);
	CleanupStack::PushL(buffer);

	CExifModify* exifModify = 0;
	exifModify = CExifModify::NewL(*buffer, CExifModify::EModify);
	CleanupStack::PushL(exifModify);

	HBufC8* buf = 0;
	TUint16 tmp1 = 0;
	TUint32 tmp2 = 0;
	TUint32 tmp3 = 0;
	TInt error = 0;

	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	exifModify->SetImageDescriptionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	error = exifModify->Reader()->GetXResolution(tmp2,tmp3);
	exifModify->SetXResolutionL(tmp2,tmp3);
	error = exifModify->Reader()->GetYResolution(tmp2,tmp3);
	exifModify->SetYResolutionL(tmp2,tmp3);
	error = exifModify->Reader()->GetResolutionUnit(tmp1);
	exifModify->SetResolutionUnitL(tmp1);

	
	buf = TUtils::CreateDummyBufL(768*2);
	CleanupStack::PushL(buf);
	exifModify->SetTransferFunctionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	
	HBufC8* buffer2 = exifModify->WriteDataL(*buffer);
	CleanupStack::PushL(buffer2);

	if(iWrite)
		{
		RFile file;
		TBuf<255> fileName;
    	TInt flen = KIopDir.iTypeLength; // folder length
		fileName.Copy(aFileName);
		fileName.Insert(flen,_L("out\\"));
        // Create out folder, ignore errors
       	iFs.MkDir(fileName);
		fileName.Append(_L(".cpy.jpg"));
		if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
			User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));

		User::LeaveIfError(file.Write(*buffer2, buffer2->Length()));
		file.Close();
		}
	CleanupStack::PopAndDestroy(buffer2);
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(buffer);
	}

void CExifIopTest::ExifIop3L(CExifRead* aExifRead, const TDesC& aFileName)
	{
	RDebug::Print(aFileName);
	CExifRead* exifRead = aExifRead;
	HBufC8* buffer = TUtils::ReadFileL(iFs, aFileName);
	CleanupStack::PushL(buffer);

	CExifModify* exifModify = 0;
	exifModify = CExifModify::NewL(*buffer, CExifModify::ECreate);
	CleanupStack::PushL(exifModify);

	// Insert Tags
	HBufC8* tmp = 0;
	HBufC8* buf = 0;
	TUint16 tmp1 = 0;
	TUint32 tmp3 = 1;
	TUint32 tmp4 = 2;

	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	exifModify->SetImageDescriptionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;


	buf = TUtils::CreateDummyBufL(768*2);
	CleanupStack::PushL(buf);
	exifModify->SetTransferFunctionL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	tmp = exifRead->GetDateTimeL();
	CleanupStack::PushL(tmp);
	exifModify->SetDateTimeL(tmp->Des());
	CleanupStack::PopAndDestroy(tmp);

	User::LeaveIfError(exifRead->GetYCbCrPositioning(tmp1));
	exifModify->SetYCbCrPositioningL(tmp1);

	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	exifModify->SetSoftwareL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(5);
	CleanupStack::PushL(buf);
	exifModify->SetCopyrightL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	exifModify->SetExposureTimeL(tmp3, tmp4);

	User::LeaveIfError(exifRead->GetXResolution(tmp3, tmp4));
	exifModify->SetXResolutionL(tmp3, tmp4);
	User::LeaveIfError(exifRead->GetYResolution(tmp3, tmp4));
	exifModify->SetYResolutionL(tmp3, tmp4);
	User::LeaveIfError(exifRead->GetResolutionUnit(tmp1));
	exifModify->SetResolutionUnitL(tmp1);

	User::LeaveIfError(exifRead->GetPixelXDimension(tmp3));
	exifModify->SetPixelXDimensionL(tmp3);
	User::LeaveIfError(exifRead->GetPixelYDimension(tmp3));
	exifModify->SetPixelYDimensionL(tmp3);

	exifModify->SetComponentsConfigurationL(1, 2, 3, 0);
	User::LeaveIfError(exifRead->GetFlash(tmp1));
	exifModify->SetFlashL(tmp1);
	User::LeaveIfError(exifRead->GetColorSpace(tmp1));
	exifModify->SetColorSpaceL(tmp1);

	exifModify->SetExposureModeL(0);
	exifModify->SetWhiteBalanceL(0);
	exifModify->SetSceneCaptureTypeL(0);
	exifModify->SetExposureProgramL(0);
	
	buf = TUtils::CreateDummyBufL(6);
	CleanupStack::PushL(buf);
	exifModify->SetIsoSpeedRatingsL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	exifModify->SetLightSourceL(0);

	buf = TUtils::CreateDummyBufL(16);
	CleanupStack::PushL(buf);
	exifModify->SetMakerNoteL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;

	buf = TUtils::CreateDummyBufL(12);
	CleanupStack::PushL(buf);
	exifModify->SetRelatedSoundFileL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	buf = 0;
	
	InsertGpsTagsL(exifModify);

	exifModify->SetDigitalZoomRatioL(0,1);
	exifModify->SetContrastL(0);
	exifModify->SetSaturationL(0);
	exifModify->SetSharpnessL(0);

	buf = exifRead->GetThumbnailL();
	CleanupStack::PushL(buf);
	exifModify->SetThumbnailL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	User::LeaveIfError(exifRead->GetThumbnailResolutionUnit(tmp1));
	exifModify->SetThumbnailResolutionUnitL(tmp1);
	User::LeaveIfError(exifRead->GetThumbnailXResolution(tmp3, tmp4));
	exifModify->SetThumbnailXResolutionL(tmp3, tmp4);
	User::LeaveIfError(exifRead->GetThumbnailYResolution(tmp3, tmp4));
	exifModify->SetThumbnailYResolutionL(tmp3, tmp4);

		
	HBufC8* buffer3 = exifModify->WriteDataL(*buffer);
	CleanupStack::PushL(buffer3);

	if(iWrite)
		{
		RFile file;
		TBuf<255> fileName;
    	TInt flen = KIopDir.iTypeLength; // folder length
		fileName.Copy(aFileName);
		fileName.Insert(flen,_L("out\\"));
        // Create out folder, ignore errors
       	iFs.MkDir(fileName);
		fileName.Append(_L(".new.jpg"));
		if(file.Open(iFs, fileName, EFileWrite) != KErrNone)
			User::LeaveIfError(file.Create(iFs, fileName, EFileWrite));

		User::LeaveIfError(file.Write(*buffer3, buffer3->Length()));
		file.Close();
		}

	CleanupStack::PopAndDestroy(buffer3);
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(buffer);
	}

void CExifIopTest::InsertGpsTagsL(CExifModify* aModify)
	{
	for(TInt i = 0; i< gpsTagsNo; i++)
		{
		HBufC8* buf = HBufC8::NewL(1);
		CleanupStack::PushL(buf);
		buf->Des().SetLength(1);
		*((TUint8*)buf + 4) = gpsTags[i][3];
		TExifTagInfo tag(gpsTags[i][0], (CExifTag::TExifTagDataType)gpsTags[i][1], gpsTags[i][2]);
		aModify->SetTagL(EIfdGps, tag, buf->Des());
		CleanupStack::PopAndDestroy(buf);
		}
	}
