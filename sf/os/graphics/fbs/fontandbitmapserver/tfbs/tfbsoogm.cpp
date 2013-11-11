// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: Test the operation of the Font and Bitmap Server's GOoM plug-in
//


#include <gdi.h>
#include "tfbsoogm.h"



const TInt KTFbsOogmFrameworkPause = 500000; // How long does this need to be in order to be reliable?
const TInt KTFbsOogmImageSizeX = 2048;
const TInt KTFbsOogmImageSizeY = 2048;


void CTFbsOogm::RunTestCaseL( TInt aCurTestCase )
    {
    ( (CTFbsOogmStep*)iStep )->SetTestStepID( KUnknownSYMTestCaseIDName );

    switch( aCurTestCase )
        {
    case 1:
        ( (CTFbsOogmStep*)iStep )->SetTestStepID( _L("GRAPHICS-FBSERV-0675") );

        CacheClearanceAndLimitAdjustments();
        break;

    default:
        ( (CTFbsOogmStep*)iStep )->SetTestStepID( KNotATestSYMTestCaseIDName );
        ( (CTFbsOogmStep*)iStep )->CloseTMSGraphicsStep();
        TestComplete();

        break;
        }

    ( (CTFbsOogmStep*)iStep )->RecordTestResultL();

    }



/**
 	@SYMTestCaseID
	GRAPHICS-FBSERV-0675

	@SYMTestCaseDesc
	Tests the operation of Font and Bitmap server's  Out-Of-Graphics-Memory plugin.
	ie Ensure that the hardware glyph cache is cleared in response to a low graphics 
	memory notification, and that the cache's maximum size limit is reduced.


	@SYMTestActions
	Acquire the glyph cache's usage and other metrics.
	
	Populate the glyph-cache.

	Acquire the glyph-cache's usage and other metrics.

	Precipitate an Out-of-Graphics-Memory condition, triggering the GOOM monitor.

	Acquire the glyph cache's usage and other metrics.

	Ensure the glyph-cache has been cleared and its maximum limit reduced.

	Precipitate a MemoryGood() call from the GOoM framework.

	Establish that the cache-size limit has been reinstated.

	@SYMTestExpectedResults
    The glyph-cache should be cleared in response to OoGM condition and its upper limit reduced.

    The Glyph-cache's upper limit should be reinstated in response to a memory-good notification.
 */
void CTFbsOogm::CacheClearanceAndLimitAdjustments()
    {
    __UHEAP_MARK;

    RArray <RSgImage> sgImageArray;
    RSgDriver sgDriver;
    TInt err = sgDriver.Open();

    if( KErrNone != err )
        {
		TEST( KErrNone == err );
		INFO_PRINTF2( _L("SgDriver Open() returned error %d"), err );

        return;
        }

    if( (NULL == RFbsSession::GetSession()) )
         {
        if( KErrNone != RFbsSession::Connect() )
            {
 			TEST( -1 );
			INFO_PRINTF1(_L("Failed to connect to FbServ"));

            goto CleanupAndGo;
            }
         }


	// Establish the initial condition of the glyph-cache.
    TGlyphCacheMetrics initialGlyphCacheMetrics;
    err = RFbsSession::GetSession()->GetGlyphCacheMetrics( initialGlyphCacheMetrics );
    if( KErrNone != err )
        {
		TEST( KErrNone == err );
		INFO_PRINTF2( _L("GetGlyphCacheMetrics() returned error %d"), err );

        goto CleanupAndGo;
        }

	// Check that initial conditions are as expected.
    // There was a test here for a cache-size of zero, but this was felt to be a hazardous assumption.
	TEST( initialGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );
    INFO_PRINTF4( _L("Initial iMaxCacheSizeInBytes %d iCacheSizeInBytes %d iGpuCacheSizeLimitIsMax %d "),
                      initialGlyphCacheMetrics.iMaxCacheSizeInBytes,
                      initialGlyphCacheMetrics.iCacheSizeInBytes,
                      initialGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );


    TRAP( err, UseGpuL() ); // Populate the glyph cache then acquire its usage metrics.
    if( KErrNone != err )
        {
        TEST( KErrNone == err );
        INFO_PRINTF2( _L("UseGpuL() left with %d"), err );

        goto CleanupAndGo;
        }


    TGlyphCacheMetrics usageGlyphCacheMetrics;
    err = RFbsSession::GetSession()->GetGlyphCacheMetrics( usageGlyphCacheMetrics );
    if( KErrNone != err )
        {
		TEST( KErrNone == err );
		INFO_PRINTF2( _L("GetGlyphCacheMetrics() returned %d"), err );

        goto CleanupAndGo;
        }

    // Check that the glyph cache has been populated
    TEST( usageGlyphCacheMetrics.iCacheSizeInBytes > initialGlyphCacheMetrics.iCacheSizeInBytes);
    TEST( usageGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );
    INFO_PRINTF4( _L("Usage iMaxCacheSizeInBytes %d iCacheSizeInBytes %d iGpuCacheSizeLimitIsMax %d "),
                       usageGlyphCacheMetrics.iMaxCacheSizeInBytes,
                       usageGlyphCacheMetrics.iCacheSizeInBytes,
                       usageGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );



    // Precipitate the GOoM framework's call into the Plug-in's FreeRam() method.
    err = FillGraphicsMemoryWithImages( TSize(KTFbsOogmImageSizeX, KTFbsOogmImageSizeY), sgImageArray );
    if( KErrNoGraphicsMemory != err )
        {
		TEST( KErrNoGraphicsMemory == err );
		INFO_PRINTF2( _L("FillGraphicsMemoryWithImages() returned %d"), err );

        goto CleanupAndGo;
        }

	// Await the GOOM framework's call into FbServ's OoGM plugin,
    // then establish the cache's usage and other metrics.
    User::After( KTFbsOogmFrameworkPause );
    TGlyphCacheMetrics postOogmGlyphCacheMetrics;
    err = RFbsSession::GetSession()->GetGlyphCacheMetrics( postOogmGlyphCacheMetrics );

    if( KErrNone != err )
        {
		TEST( KErrNone == err );
		INFO_PRINTF2( _L("GetGlyphCacheMetrics() returned %d"), err );

        goto CleanupAndGo;
        }

    // The cache should have been cleared and the maximum size limit reduced.
    TEST( 0 == postOogmGlyphCacheMetrics.iCacheSizeInBytes );
    TEST( !postOogmGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );
    INFO_PRINTF4( _L("Post-Oogm iMaxCacheSizeInBytes %d iCacheSizeInBytes %d iGpuCacheSizeLimitIsMax %d "),
                        postOogmGlyphCacheMetrics.iMaxCacheSizeInBytes,
                        postOogmGlyphCacheMetrics.iCacheSizeInBytes,
                        postOogmGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );



   // Remove the images. This should provoke a GOoM monitor call into the plug-in's MemoryGood().
    for ( TInt i = sgImageArray.Count()-1; i >= 0; --i )
        {
        sgImageArray[i].Close();
        }
    sgImageArray.Reset();

    // Await activity from the GOoM monitor
    User::After( KTFbsOogmFrameworkPause );
    TGlyphCacheMetrics reinstatedGlyphCacheMetrics;
    err = RFbsSession::GetSession()->GetGlyphCacheMetrics( reinstatedGlyphCacheMetrics );
    if( KErrNone != err )
        {
        TEST( KErrNone == err );
        INFO_PRINTF2( _L("GetGlyphCacheMetrics() returned error %d"), err );

        goto CleanupAndGo;
        }

    // Cache size limit should have been increased
    TEST( reinstatedGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );
    INFO_PRINTF4( _L("After Mem Clear iMaxCacheSizeInBytes %d iCacheSizeInBytes %d iGpuCacheSizeLimitIsMax %d "),
                      reinstatedGlyphCacheMetrics.iMaxCacheSizeInBytes,
                      reinstatedGlyphCacheMetrics.iCacheSizeInBytes,
                      reinstatedGlyphCacheMetrics.iGpuCacheSizeLimitIsMax );



  CleanupAndGo:
    // Release any images before closing the array.
    // If the test was successful this should already be empty.
    for (TInt i = sgImageArray.Count()-1; i >= 0; --i)
        {
        sgImageArray[i].Close();
        }

    // Allow GOoM to make any pending adjustments before proceeding with any further tests.
    User::After( KTFbsOogmFrameworkPause );
    sgImageArray.Close();
    sgDriver.Close();

    __UHEAP_MARKEND;
    }



CTFbsOogm::CTFbsOogm( CTestStep* aStep )
: CTGraphicsBase(aStep)
    {
    }



const TInt KNumGlyphCodesLatin = 96;
/*
 Lookup table to convert from ascii code to
glyph code for the Deja Vu family of fonts.
 */
const TUint DejaVuASCIIToGlyphCode[] =
    {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    91, 92, 93, 94, 95, 96, 97, 98,
    };



void CTFbsOogm::ConstructL()
    {
    }


CTFbsOogm::~CTFbsOogm()
    {
    RFbsSession::Disconnect();
    }



/**
 Utility function to fill the GPU memory.
 */
TInt CTFbsOogm::FillGraphicsMemoryWithImages( const TSize& aSize, RArray<RSgImage>& aImages )
    {
    TInt err = KErrNone;

    // Loop should terminate with KErrNoGraphicsMemory
    while( KErrNone == err )
        {
        RSgImage sgImage;
        err = sgImage.Create( TSgImageInfo(aSize, ESgPixelFormatA_8, ESgUsageBitOpenVgImage) );
        if( KErrNone == err )
            {
            err = aImages.Append( sgImage );
            }
        }

    INFO_PRINTF2( _L("Images created %d"), aImages.Count() );
    return err;
    }



/**
 Utility function to populate the GPU with typeface glyphs.
 */
void CTFbsOogm::UseGpuL()
    {
    _LIT( KTKASTypefaceName, "DejaVu Sans Condensed" );

    // Need to open one of these "in the context" of this process in order
    // to manipulate RSgImages. Even though we are only calling 'Open', 'Next' and 'Close' on
    // RFbsGlyphDataIterator
    RSgDriver sgDriver;
    User::LeaveIfError( sgDriver.Open() );

    // CFbsTypefaceStore seems to need an fbserv session open.
    if( (NULL == RFbsSession::GetSession()) )
     {
     User::LeaveIfError( RFbsSession::Connect() );
     }

    TUint* glyphCodesLatin = new(ELeave) TUint[ KNumGlyphCodesLatin ];

    for ( TInt ii = 0; ii < KNumGlyphCodesLatin; ++ii )
        {
        TUint asciiCode = ii+0x20; // ASCII characters from 0020 to 007F
        glyphCodesLatin[ii] = DejaVuASCIIToGlyphCode[asciiCode];
        }

    iTs = ( CFbsTypefaceStore* )CFbsTypefaceStore::NewL( NULL );
    User::LeaveIfError( iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont, TFontSpec(KTKASTypefaceName, 15)) );

    TInt iterErr = KErrNone;
    TInt iterNextErr = KErrNone;

    for( TInt arraySize = 1; (arraySize < KNumGlyphCodesLatin) && (iterErr == KErrNone); ++arraySize )
        {
        RFbsGlyphDataIterator iter;
        iterErr = iter.Open( *iFont, glyphCodesLatin, arraySize );

        if( KErrNone != iterErr )
            {
            continue;
            }

          for ( TInt index = 0; KErrNone == iterNextErr; iterNextErr = iter.Next(), ++index )
              {
              // Iterating through the glyphs should introduce them into the cache
             if(iter.GlyphCode() != glyphCodesLatin[index])
                {
                 INFO_PRINTF4( _L("Wanted glyphcode %d, got %d"), arraySize, glyphCodesLatin[index], iter.GlyphCode() );
                }
              }

        iterNextErr = KErrNone;
        iter.Close();
        }

    sgDriver.Close();
    }


__CONSTRUCT_STEP__( FbsOogm )
