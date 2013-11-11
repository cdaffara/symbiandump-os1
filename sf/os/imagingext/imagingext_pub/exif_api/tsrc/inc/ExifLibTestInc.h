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
#include "ExifRead.h"
#include "ExifModify.h"
#include "ExifTag.h"
#include "ExifValueTable.h"
#include <fbs.h>
#include "ExifCommon.h"


#ifdef _USE_CPPUNIT_
#include <CppUnit/Test.h>
#include <CppUnit/TestCase.h>
#include <CppUnit/TestCaller.h>
#include <CppUnit/TestSuite.h>
#else
#include "TestFrameWork/test.h"
#include "TestFrameWork/testCase.h"
#include "TestFrameWork/testCaller.h"
#include "TestFrameWork/testSuite.h"
#endif
_LIT( KRootC, "C:\\ExifLibTest\\" );
_LIT( KRootE, "F:\\testing\\data\\ExifLibTest\\" );
_LIT( KRootData, "C:\\data\\ExifLibTest\\" );

#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
_LIT(KIopDir, "C:\\ExifLibTest\\IOP\\");
_LIT( KRootOut, "C:\\ExifLibTest\\" );
#else
_LIT(KIopDir, "F:\\testing\\data\\ExifLibTest\\IOP\\");
_LIT( KRootOut, "F:\\testing\\data\\ExifLibTest\\" );
#endif

_LIT(KValidExif, "ValidExif.jpg");
_LIT(KInvalidExif1, "InvalidExif1.jpg");
_LIT(KInvalidExif2, "InvalidExif2.jpg");
_LIT(KInvalidExif3, "InvalidExif3.jpg");
_LIT(KInvalidExif4, "InvalidExif4.jpg");
_LIT(KInvalidExif5, "InvalidExif5.jpg");
_LIT(KInvalidExif6, "InvalidExif6.jpg");
_LIT(KInvalidExif7, "InvalidExif7.jpg");
_LIT(KInvalidExif8, "InvalidExif8.jpg");
_LIT(KInvalidExif9, "InvalidExif9.jpg");
_LIT(KInvalidExif10, "InvalidExif10.jpg");
_LIT(KInvalidExif11, "InvalidExif11.jpg");
_LIT(KInvalidExif12, "InvalidExif12.jpg");
_LIT(KInvalidExif13, "InvalidExif13.jpg");
_LIT(KInvalidExif14, "InvalidExif14.jpg");
_LIT(KNotSupportedExif1, "NotSupportedExif1.jpg");
_LIT(KNotSupportedExif2, "NotSupportedExif2.jpg");
_LIT(KFullExif, "FullExif.jpg");
_LIT(KFullExif3mpix, "FullExif3mpix.jpg");
_LIT(KValidJpeg, "ValidJpeg.jpg");
_LIT(KInvalidJpeg1, "InvalidJpeg1.jpg");
_LIT(KInvalidJpeg2, "InvalidJpeg2.jpg");
_LIT(KInvalidJpeg3, "InvalidJpeg3.jpg");
_LIT(KValidThumbnail, "Thumbnail.jpg");
_LIT(KNoThumbnail, "NoThumbnail.jpg");
_LIT(KCameraAPIJpeg, "CameraApi.jpg");
_LIT(KNoTagChk_IMG_AN19, "NoTagChk_IMG_AN19.jpeg");
_LIT(KNoTagChk_IMG_AN19_out, "NoTagChk_IMG_AN19_out.jpeg");
_LIT(KNoTagChk_Original, "NoTagChk_Original.jpg");
_LIT(KtagInWrongIfd, "tagInWrongIfd.jpg");
_LIT(KtagInWrongIfd_out, "tagInWrongIfd_out.jpg");
_LIT(KUnknown_tags, "DSCN1011_unknown_tags.jpg");
_LIT(KUnknown_tags_out, "DSCN1011_unknown_tags_out.jpg");


class CExifLibTestInc;

class TUtils
	{
	public:
		
		static HBufC8* ReadFileL(RFs aFs, const TDesC& aFileName) 
			{
            HBufC* fileName = HBufC::NewL( 128 );
            CleanupStack::PushL( fileName );
            fileName->Des().Append( KRootC );
            fileName->Des().Append( aFileName );
			RFile file;
      LOGTEXT2( _L( "ExifLib: TUtils::ReadFileL %s" ), fileName->Ptr());
			TInt error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
            if ( error != KErrNone )
                {
                fileName->Des().Copy( KRootE );
                fileName->Des().Append( aFileName );
                error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
                if ( error != KErrNone )
                    {
                    fileName->Des().Copy( KRootData );
                    fileName->Des().Append( aFileName );
                    error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
                    if ( error != KErrNone )
                        {
                        User::LeaveIfError( file.Open(aFs,aFileName,EFileShareExclusive|EFileRead) );
                        }
                    }
                }
			CleanupClosePushL(file);
			TInt size = 0;
			file.Size(size);
			
			HBufC8* buffer = HBufC8::NewL(size);
			CleanupStack::PushL(buffer);
			TPtr8 bufferDes(buffer->Des());
			User::LeaveIfError(file.Read(bufferDes));
			CleanupStack::Pop(buffer);
			CleanupStack::PopAndDestroy();
            CleanupStack::PopAndDestroy();
			return buffer;
			};

		static HBufC8* ReadFileL(RFs aFs, const TDesC& aFileName, TUint32 aMaxRead) 
			{
            HBufC* fileName = HBufC::NewL( 128 );
            CleanupStack::PushL( fileName );
            fileName->Des().Append( KRootC );
            fileName->Des().Append( aFileName );
			RFile file;
      LOGTEXT2( _L( "ExifLib: TUtils::ReadFileL %s" ), fileName->Ptr());
			TInt error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
            if ( error != KErrNone )
                {
                fileName->Des().Copy( KRootE );
                fileName->Des().Append( aFileName );
                error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
                if ( error != KErrNone )
                    {
                    fileName->Des().Copy( KRootData );
                    fileName->Des().Append( aFileName );
                    error = file.Open(aFs,fileName->Des(),EFileShareExclusive|EFileRead);
                    if ( error != KErrNone )
                        {
                        User::LeaveIfError( file.Open(aFs,aFileName,EFileShareExclusive|EFileRead) );
                        }
                    }
                }
			CleanupClosePushL(file);
			TInt size = 0;
			file.Size(size);
			// read up to max size parameter
			if ( size > aMaxRead )
				size = aMaxRead;
			
			HBufC8* buffer = HBufC8::NewL(size);
			CleanupStack::PushL(buffer);
			TPtr8 bufferDes(buffer->Des());
			User::LeaveIfError(file.Read(bufferDes, size));
			CleanupStack::Pop(buffer);
			CleanupStack::PopAndDestroy();
            CleanupStack::PopAndDestroy();
			return buffer;
			};

		static HBufC8* CreateDummyBufL(TInt aSize)
			{
			HBufC8* buf = HBufC8::NewL(aSize);
			CleanupStack::PushL(buf);
			TUint8* tmp = new(ELeave) TUint8[aSize];
			CleanupStack::PushL(tmp);
			TInt i = 0;
			for(i = 0; i < aSize - (aSize % 5); i+=5)
				{
				tmp[i] = 'D';
				tmp[i+1] = 'U';
				tmp[i+2] = 'M';
				tmp[i+3] = 'M';
				tmp[i+4] = 'Y';
				}
			for(TInt j = 0; j < aSize % 5; j++)
				tmp[i + j] = 'D';
			buf->Des().Copy(tmp, aSize);
			buf->Des().SetLength(aSize);
			CleanupStack::PopAndDestroy(tmp);
			CleanupStack::Pop(buf);
			return buf;
			};

		static void GetAllTagsTestL(CExifRead* read, const TDesC& aFileName)
			{
			TInt error = 0;
	        RDebug::Print(_L("GetAllTagsTestL starts for %s"), aFileName.Ptr());
            if ( !read )
                {
                return;
                }
            HBufC8* thumbnail = NULL;
            TRAP( error, thumbnail = read->GetThumbnailL());
	        CleanupStack::PushL(thumbnail);
	        if(!error && thumbnail)
		        RDebug::Print(_L("thumbnail ok, size %d ..."), thumbnail->Des().Size());
	        else
		        RDebug::Print(_L("no thumbnail %d..."), error);
	        CleanupStack::PopAndDestroy(thumbnail);

            HBufC8* datetime  = NULL;
            TRAP( error, datetime  = read->GetDateTimeL());
	        CleanupStack::PushL(datetime);
	        if( !error && datetime)
		        RDebug::Print(_L("datetime ok, size %d ..."), datetime->Des().Size());
	        else
		        RDebug::Print(_L("no datetime %d..."), error);
	        CleanupStack::PopAndDestroy(datetime);

            HBufC8* imagedescription  = NULL;
            TRAP( error, imagedescription  = read->GetImageDescriptionL() );
	        CleanupStack::PushL(imagedescription);
	        if(!error && imagedescription)
		        RDebug::Print(_L("imagedescription ok, size %d ..."), imagedescription->Des().Size());
    	    else
	    	    RDebug::Print(_L("no imagedescription %d..."), error);
	        CleanupStack::PopAndDestroy(imagedescription);

            HBufC8* make  = NULL;
            TRAP( error, make  = read->GetMakeL() );
	        CleanupStack::PushL(make);
    	    if(!error && make)
	    	    RDebug::Print(_L("make ok, size %d ..."), make->Des().Size());
	        else
		        RDebug::Print(_L("no make %d..."), error);
    	    CleanupStack::PopAndDestroy(make);

            HBufC8* model  = NULL;
            TRAP( error, model  = read->GetModelL() );
    	    CleanupStack::PushL(model);
	        if(!error && model)
		        RDebug::Print(_L("model ok, size %d ..."), model->Des().Size());
    	    else
	    	    RDebug::Print(_L("no model %d..."), error);
	        CleanupStack::PopAndDestroy(model);

            HBufC8* software  = NULL;
            TRAP( error, software  = read->GetSoftwareL() );
	        CleanupStack::PushL(software);
    	    if(!error && software)
	    	    RDebug::Print(_L("software ok, size %d ..."), software->Des().Size());
	        else
		        RDebug::Print(_L("no software %d..."), error);
    	    CleanupStack::PopAndDestroy(software);

            HBufC8* copyright  = NULL;
            TRAP( error, copyright  = read->GetCopyrightL() );
    	    CleanupStack::PushL(copyright);
	        if(!error && copyright)
		        RDebug::Print(_L("copyright ok, size %d ..."), copyright->Des().Size());
    	    else
	    	    RDebug::Print(_L("no copyright %d..."), error);
	        CleanupStack::PopAndDestroy(copyright);

            TUint16 orientation = 0;
            TInt err = read->GetOrientation( orientation);
    	    if(!err)
	            RDebug::Print(_L("orientation ok %d ..."), orientation);
	        else
		        RDebug::Print(_L("no orientation %d..."), err);
	        
	        TUint32 gpsversion = 0;
            err = read->GetGpsVersion( gpsversion );
    	    if(!err)
	            RDebug::Print(_L("gpsversion ok 0x%x ..."), gpsversion);
	        else
		        RDebug::Print(_L("no gpsversion %d..."), err);
	        
	        TUint32 exifversion = 0;
            err = read->GetExifVersion( exifversion );
    	    if(!err)
	            RDebug::Print(_L("exifversion ok 0x%x ..."), exifversion);
	        else
		        RDebug::Print(_L("no exifversion %d..."), err);
	        
	        TUint32 flashpixversion = 0;
            err = read->GetFlashPixVersion( flashpixversion );
    	    if(!err)
	            RDebug::Print(_L("flashpixversion ok 0x%x ..."), flashpixversion);
	        else
		        RDebug::Print(_L("no flashpixversion %d..."), err);

            HBufC8* datetimeoriginal  = NULL;
            TRAP( error, datetimeoriginal  = read->GetDateTimeOriginalL() );
    	    CleanupStack::PushL(datetimeoriginal);
	        if(!error && datetimeoriginal)
		        RDebug::Print(_L("datetimeoriginal ok, size %d ..."), datetimeoriginal->Des().Size());
    	    else
	    	    RDebug::Print(_L("no datetimeoriginal %d..."), error);
	        CleanupStack::PopAndDestroy(datetimeoriginal);

            HBufC8* datetimedigitized  = NULL;
            TRAP( error, datetimedigitized  = read->GetDateTimeDigitizedL() );
    	    CleanupStack::PushL(datetimedigitized);
	        if(!error && datetimedigitized)
		        RDebug::Print(_L("datetimedigitized ok, size %d ..."), datetimedigitized->Des().Size());
    	    else
	    	    RDebug::Print(_L("no datetimedigitized %d..."), error);
	        CleanupStack::PopAndDestroy(datetimedigitized);
	        
            HBufC8* usercomment  = NULL;
            TRAP( error, usercomment  = read->GetUserCommentL() );
    	    CleanupStack::PushL(usercomment);
	        if(!error && usercomment)
		        RDebug::Print(_L("usercomment ok, size %d ..."), usercomment->Des().Size());
    	    else
	    	    RDebug::Print(_L("no usercomment %d..."), error);
	        CleanupStack::PopAndDestroy(usercomment);
	        
            HBufC8* relatedsoundfile  = NULL;
            TRAP( error, relatedsoundfile  = read->GetRelatedSoundFileL() );
    	    CleanupStack::PushL(relatedsoundfile);
	        if(!error && relatedsoundfile)
		        RDebug::Print(_L("relatedsoundfile ok, size %d ..."), relatedsoundfile->Des().Size());
    	    else
	    	    RDebug::Print(_L("no relatedsoundfile %d..."), error);
	        CleanupStack::PopAndDestroy(relatedsoundfile);
	        

/*-------------- missing test cases---------

        TInt GetXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const;
    	
        TInt GetYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const;
    	
        TInt GetResolutionUnit( TUint16& aResolutionUnit ) const; 
    	
        TInt GetYCbCrPositioning( TUint16& aYCbCrPositioning ) const; 
    	
        TInt GetExifIfdPointer( TUint32& aExifIfdPointer ) const; 
    	
        TInt GetGpsInfoIfdPointer( TUint32& aGpsInfoIfdPointer ) const; 

    	HBufC8* GetIsoSpeedRatingsL() const; 
    	
        HBufC8* GetMakerNoteL() const; 
    	

    	TInt GetExposureTime( 
            TUint32& aExposureTime1, 
            TUint32& aExposureTime2 ) const; 
    	
        TInt GetComponentsConfiguration( 
            TUint8& aFirstComponent, TUint8& aSecondComponent,
            TUint8& aThirdComponent, TUint8& aFourthComponent) const; 

        TInt GetFlash( TUint16& aFlash ) const; 
    	
        TInt GetColorSpace( TUint16& aColorSpace ) const; 
    	
        TInt GetPixelXDimension( TUint32& aPixelXDimension ) const; 
    	
        TInt GetPixelYDimension( TUint32& aPixelYDimension ) const; 
    	
        TInt GetExposureMode( TUint16& aExposureMode ) const; 
    	
        TInt GetWhiteBalance( TUint16& aWhiteBalance ) const; 
    	
        TInt GetSceneCaptureType( TUint16& aSceneCaptureType ) const; 
    	
        TInt GetExposureProgram( TUint16& aExposureProgram ) const;
    	
        TInt GetApertureValue( 
            TUint32& aApertureValue1, 
            TUint32& aApertureValue2 ) const; 
    	
        TInt GetExposureBiasValue( 
            TInt32& aExposureBiasValue1, 
            TInt32& aExposureBiasValue2 ) const; 
    	
        TInt GetMeteringMode( TUint16& aMeteringMode ) const; 
    	
        TInt GetLightSource( TUint16& aLightSource ) const; 
    	
        TInt GetFileSource( TInt8& aFileSource ) const; 
    	
        TInt GetDigitalZoomRatio( 
            TUint32& aDigitalZoomRatio1, 
            TUint32& aDigitalZoomRatio2 ) const; 
    	
        TInt GetContrast( TUint16& aContrast ) const; 
    	
        TInt GetSaturation( TUint16& aSaturation ) const; 
    	
        TInt GetSharpness( TUint16& aSharpness ) const; 
    	
    	
    	
        TInt GetInteroperabilityIfdPointer( 
            TUint32& aInteroperabilityIfdPointer ) const; 

    	TInt GetThumbnailXResolution( 
            TUint32& aXResolution1, 
            TUint32& aXResolution2 ) const;
    	
        TInt GetThumbnailYResolution( 
            TUint32& aYResolution1, 
            TUint32& aYResolution2 ) const;
    	
        TInt GetThumbnailResolutionUnit( TUint16& aResolutionUnit ) const;
    	
        TInt GetThumbnailCompression( TUint16& aCompression ) const; 
    	
        TInt GetJpegInterchangeFormat( TUint32& aJpegInterchangeFormat ) const; 
    	
        TInt GetJpegInterchangeFormatLength( 
            TUint32& aJpegInterchangeFormatLength ) const;

        HBufC8* GetExifAppSegmentL() const;

        TInt GetShutterSpeedValue( TInt32& aShutterSpeedValue1, 
            TInt32& aShutterSpeedValue2 ) const;

        TInt GetBrightnessValue( TInt32& aBrightnessValue1, 
            TInt32& aBrightnessValue2 ) const;

        TInt GetCustomRendered( TUint16& aCustomRendered ) const;

        TInt GetGainControl( TUint16& aGainControl ) const;



--------------*/
			return;
			};
	};

class CExifTagTest : public CTestCase
	{
	friend class CExifLibTestInc;

	RFs iFs;
	public:
		CExifTagTest(){};
		~CExifTagTest(){};
		void ExifTag001L();
		void ExifTag002L();
        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			};
        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	};

class CExifReadTest : public CTestCase
	{
	friend class CExifLibTestInc;
	RFs iFs;

	public:
		CExifReadTest(){};
		~CExifReadTest(){};
		void ExifRead001L();
		void ExifRead002L();
		void ExifRead003L();
		void ExifRead004L();
		void ExifRead005L();
		void ExifRead006L();
		void ExifRead007L();
		void ExifRead008L();
		void ExifRead009L();
		void ExifRead010L();
		void ExifRead011L();
		void ExifRead012L();
		void ExifRead013L();
		void ExifRead014L();
		void ExifRead015L();
        void ExifRead016L();
		void ExifRead017L();
		void ExifRead018L();
		// For fast reader
		void ExifRead101L();
		void ExifRead102L();
		void ExifRead103L();
		void ExifRead104L();
		void ExifRead105L();
		void ExifRead106L();
		void ExifRead107L();
		void ExifRead108L();
		void ExifRead109L();
		void ExifRead110L();
		void ExifRead111L();
		void ExifRead112L();
		void ExifRead113L();
		void ExifRead114L();
		void ExifRead115L();
        void ExifRead116L();
		// For no tag validity checking
		void ExifRead201L();
		void ExifRead202L();
		void ExifRead203L();
		void ExifRead204L();
		void ExifRead205L();
		void ExifRead206L();
		void ExifRead207L();
		void ExifRead208L();
		void ExifRead209L();
		void ExifRead210L();
		void ExifRead211L();
		void ExifRead212L();
		void ExifRead213L();
		void ExifRead214L();
		void ExifRead215L();
        void ExifRead216L();
		
        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			};

        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	};

class CExifCreateTest : public CTestCase
	{
	friend class CExifLibTestInc;
	RFs iFs;

	public:
		CExifCreateTest(){};
		~CExifCreateTest(){};
		void ExifCreate001L();
		void ExifCreate002L();
		void ExifCreate003L();
		void ExifCreate004L();
		void ExifCreate005L();
		void ExifCreate006L();
		void ExifCreate007L();
		void ExifCreate008L();
		void ExifCreate009L();
		void ExifCreate010L();
		void ExifCreate011L();
		void ExifCreate012L();
		void ExifCreate013L();
		//For fast test
		void ExifCreate101L();
		void ExifCreate102L();
		void ExifCreate103L();
		void ExifCreate104L();
		void ExifCreate105L();
		void ExifCreate106L();
		void ExifCreate107L();
		void ExifCreate108L();
		void ExifCreate109L();
		void ExifCreate110L();
		void ExifCreate111L();
		void ExifCreate112L();
		void ExifCreate113L();
		
        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			};
        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	};

class CExifModifyTest : public CTestCase
	{
	friend class CExifLibTestInc;
	RFs iFs;

	public:
		CExifModifyTest(){};
		~CExifModifyTest(){};
		void ExifModify001L();
		void ExifModify002L();
		void ExifModify003L();
		void ExifModify004L();
		void ExifModify005L();
		void ExifModify006L();
		void ExifModify007L();
		void ExifModify008L();
		void ExifModify009L();
		void ExifModify010L();
		void ExifModify011L();
		void ExifModify012L();
		void ExifModify013L();
		void ExifModify014L();
		void ExifModify015L();
		void ExifModify016L();
		void ExifModify017L();
        void ExifModify018L();
        void ExifModify019L();
        void ExifModify020L();
        void ExifModify021L();
        void ExifModify022L();
        void ExifModify023L();
        // For fast modifier
		void ExifModify101L();
		void ExifModify102L();
		void ExifModify103L();
		void ExifModify104L();
		void ExifModify105L();
		void ExifModify106L();
		void ExifModify107L();
		void ExifModify108L();
		void ExifModify109L();
		void ExifModify110L();
		void ExifModify111L();
		void ExifModify112L();
		void ExifModify113L();
		void ExifModify114L();
		void ExifModify115L();
		void ExifModify116L();
		void ExifModify117L();
        void ExifModify118L();
        void ExifModify119L();
        

        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			};
        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	};

class CExifModifyTest2 : public CTestCase
	{
	friend class CExifLibTestInc;
	RFs iFs;

	public:
		CExifModifyTest2(){};
		~CExifModifyTest2(){};
		void ExifModify001L();
		void ExifModify002L();
		void ExifModify003L();
		void ExifModify004L();
		void ExifModify005L();
		void ExifModify006L();
		void ExifModify007L();
		void ExifModify008L();
		void ExifModify009L();
		void ExifModify010L();
		void ExifModify011L();
		void ExifModify012L();
		void ExifModify013L();
		void ExifModify014L();
		void ExifModify015L();
		void ExifModify016L();
		void ExifModify017L();
        void ExifModify018L();
        void ExifModify019L();

        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			};
        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	};

class CExifIopTest : public CTestCase
	{
	friend class CExifLibTestInc;
	RFs iFs;

	public:
		CExifIopTest(){};
		~CExifIopTest(){};
		void ExifIop001L();
		void ExifIop002L();
		void ExifIop003L();
		void ExifIop004L();
        void ExifIop005L();
        // Allocate the resources for one test function
        void setUpL ()
			{
			User::LeaveIfError(iFs.Connect());
			iWrite = ETrue;
			};
        
        // Free the resources reserved in setUpL()
        void tearDown ()
			{
			iFs.Close();
			};
	private:
		void ExifIop1L(const TDesC& aFileName);
		void ExifIop2L(const TDesC& aFileName);
		void ExifIop3L(CExifRead* aExifRead, const TDesC& aFileName);
		HBufC8* CreateDummyBufL(TInt aSize);
		void InsertGpsTagsL(CExifModify* aModify);	

		TBool iWrite;
	};

class CExifLibTestInc : public CTestCase
    {
    friend class CExifTagTest;
	friend class CExifReadTest;
	friend class CExifCreateTest;
    public:
        
        CExifLibTestInc();
        ~CExifLibTestInc();
        
        // Allocate the resources for one test function
        void setUpL ();
        
        // Free the resources reserved in setUpL()
        void tearDown ();
        
        // A function to collect and return a suite of tests
        static MTest* suiteL ();
	};
