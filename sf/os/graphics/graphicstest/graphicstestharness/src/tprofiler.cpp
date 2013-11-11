// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <test/TestExecuteStepBase.h>
#include <e32math.h>
#include <hal.h>
#include "tprofiler.h"


#define PROFILER_TEST(a)  (iTestStep.testBooleanTrue((a), (TText8*)__FILE__, __LINE__)) 


#define PROFILER_INFO_PRINTF1(p1)                            iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define PROFILER_INFO_PRINTF2(p1, p2)                        iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))
#define PROFILER_INFO_PRINTF3(p1, p2, p3)                    iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3))
#define PROFILER_INFO_PRINTF4(p1, p2, p3, p4)                iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4))
#define PROFILER_INFO_PRINTF5(p1, p2, p3, p4, p5)            iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5))
#define PROFILER_INFO_PRINTF6(p1, p2, p3, p4, p5, p6)        iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6))
#define PROFILER_INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define PROFILER_WARN_PRINTF1(p1)                            iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1))
#define PROFILER_WARN_PRINTF2(p1, p2)                        iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2))
#define PROFILER_WARN_PRINTF3(p1, p2, p3)                    iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3))
#define PROFILER_WARN_PRINTF4(p1, p2, p3, p4)                iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4))
#define PROFILER_WARN_PRINTF5(p1, p2, p3, p4, p5)            iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5))
#define PROFILER_WARN_PRINTF6(p1, p2, p3, p4, p5, p6)        iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6))
#define PROFILER_WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define PROFILER_ERR_PRINTF1(p1)                             iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1)) 
#define PROFILER_ERR_PRINTF2(p1, p2)                         iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2)) 
#define PROFILER_ERR_PRINTF3(p1, p2, p3)                     iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3)) ;
#define PROFILER_ERR_PRINTF4(p1, p2, p3, p4)                 iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4)) 
#define PROFILER_ERR_PRINTF5(p1, p2, p3, p4, p5)             iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5)) 
#define PROFILER_ERR_PRINTF6(p1, p2, p3, p4, p5, p6)         iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define PROFILER_ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)     iTestStep.Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7))


CTProfiler::CTProfiler(CTestStep& aTestStep) :
    iTestStep(aTestStep)
    {
    }

EXPORT_C CTProfiler::~CTProfiler()
    {   
    iResults.Reset();
    iResultsTimingOrder.Reset();
    }

EXPORT_C CTProfiler* CTProfiler::NewL(CTestStep& aTestStep)
    {
    CTProfiler* profiler = new (ELeave) CTProfiler(aTestStep);
    CleanupStack::PushL(profiler);
    profiler->ConstructL();
    CleanupStack::Pop();
    return profiler;
    }

void CTProfiler::ConstructL()
    {
    User::LeaveIfError((HAL::Get(HALData::EFastCounterFrequency, iFreq)));
    PROFILER_INFO_PRINTF2(_L("FastCounterFrequency: %i   Hz"), iFreq);
    
    if(iFreq == 0)
        {
        User::Leave(KErrDivideByZero);
        }
    }

EXPORT_C void CTProfiler::StartTimer()
    {
    iStart = User::FastCounter();
    }

EXPORT_C TInt64 CTProfiler::StopTimer()
    {
    iEnd = User::FastCounter();
    // wrap around automatically dealt with by two's complement arithmetic as long as all variables are unsigned int
    iDiff += iEnd - iStart;
    TInt64 difftime = (1000000*(TInt64)iDiff) / (TInt64)iFreq;
    return difftime;
    }

/**
Initalises the results. Must be called before MarkResultSet
*/
EXPORT_C void CTProfiler::InitResults()
    {
    PROFILER_TEST(!iResultsInitalised);
    iResultsInitalised = ETrue;
    iDiff = 0;
    iResults.Reset();
    iSessionId = Math::Random();
    if (iSessionId < 1000000)
        {
        /*
         * The reason for bumping up low session id values is that it allows
         * us to in the future to re-compact the session ids into a unique
         * number range from zero to one million.  Duplicate session id values
         * generated on the same day can cause confusion but will be very rare.
         */
        iSessionId += 1000000;
        }
    
    /*
     * The build system needs to know the session id because it maps it to
     * the job ID it is running, so it can map test results to a particular
     * build in question.
     */
    PROFILER_INFO_PRINTF2(_L("SQL_SESSION_ID=%u"), iSessionId);
    StartTimer();
    }

/**
Records set current time. Can be called multiple times so an average can be taken.
*/
EXPORT_C void CTProfiler::MarkResultSetL()
    {   
    MarkResultSetAndSuspendL();
    StartTimer();
    }

/**
Records set current time. Unlike MarkResultSetL() the function doesn't 
restart the timer at the end. The following operations will not be 
included into benchmark mesuarment. To resume the profiling the user must 
start the timer.
@see StartTimer()
@see MarkResultSetL() 
Can be called multiple times so an average can be taken.
*/
EXPORT_C void CTProfiler::MarkResultSetAndSuspendL()
    {   
    TUint32 res = StopTimer();
    iResults.InsertInUnsignedKeyOrderAllowRepeatsL(res);
    if(iStoreResultInTimingOrder)
        {
        iResultsTimingOrder.AppendL(res);
        }
    iDiff = 0;
    PROFILER_TEST(iResultsInitalised);
    }

/**
Frees all memory allocated by results capturing methods like MarkResultSetL().
*/
EXPORT_C void CTProfiler::FreeResultsMemory()
    {
    iResults.Reset();
    iResultsTimingOrder.Reset();
    }

/**
Returns the trimmed mean of a set of results. i.e. Trims off the 20% smallest and the 20% largest of the data results.
*/
EXPORT_C TUint32 CTProfiler::GetTrimedMean()
    {
    TInt64 total = 0;   
    if (iResults.Count() <= 50)
        {
        PROFILER_WARN_PRINTF2(_L("Not enough results for trimming - need more than 50, but got %d"), iResults.Count());
        }
    if (iResults.Count() == 0)      // If iResults is zero then do nothing
        {
        return 0;
        }

    TInt twentyPercentCount = iResults.Count()/5;
    for (TInt count = twentyPercentCount; count < iResults.Count()-twentyPercentCount; count++)
        {
        total += iResults[count];
        }
    return ((TUint32)(total/(iResults.Count()-(twentyPercentCount*2))));
    }

EXPORT_C TInt CTProfiler::PercentageChange(TInt aFirstTime, TInt aSecondTime)
    {
    return ((aFirstTime - aSecondTime)*100) / aFirstTime;
    }

/**
Finds the maximum time taken
*/
EXPORT_C TUint32 CTProfiler::TimeMax()
    {
    if (iResults.Count() == 0)
        {
        return 0;
        }
    TUint32 result = iResults[0];
    for(TInt i = 0; i < iResults.Count(); i++)
        {
        if(iResults[i] > result)
            result = iResults[i];
        }
    return result;
    }

/**
Finds the minimum time taken
*/  
EXPORT_C TUint32 CTProfiler::TimeMin()
    {
    if (iResults.Count() == 0)
        {
        return 0;
        }
    TUint32 result = iResults[0];
    for(TInt i = 0; i < iResults.Count(); i++)
        {
        if(iResults[i] < result)
            result = iResults[i];
        }
    return result;
    }

/**
Reports analysis results

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
*/
EXPORT_C void CTProfiler::ResultsAnalysis(const TDesC& aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters)
    {
    PROFILER_TEST(iResultsInitalised);
    TBuf<128> variation;
    variation.Format(KVariation, aRotation, aSrcScreenMode, aDstScreenMode,aIters);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;
    variationMin.Format(KMinTime, &variation);
    TBuf<128> variationTrimmedMean;
    variationTrimmedMean.Format(KTrimmedMean, &variation);
    
    SqlInsert(&aTestName, &variationTrimmedMean,    &KMicroSeconds,  GetTrimedMean());
    SqlInsert(&aTestName, &variationMax,            &KMicroSeconds,  TimeMax());
    SqlInsert(&aTestName, &variationMin,            &KMicroSeconds,  TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
Reports analysis results for pixel rates

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumPixels is the number of pixels rendered
*/
EXPORT_C void CTProfiler::ResultsAnalysisPixelRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixelsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);

    TReal time = (TReal)iResults[0] / 1000000;
    TInt32 pixelRate = aNumPixelsPerIteration * aIters / time;
    
    TBuf<128> variation;
    variation.Format(KVariationPPI, aRotation, aSrcScreenMode, aDstScreenMode, aIters, aNumPixelsPerIteration);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
    
    SqlInsert(&aTestName, &variation,       &KPixelsPerSecond,  pixelRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,     TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,     TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
Reports analysis results for character rates

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumChars is the number of characters rendered
*/
EXPORT_C void CTProfiler::ResultsAnalysisCharacterRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumCharsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);
    TReal time = (TReal)iResults[0] / 1000000;
    TInt32 characterRate = aNumCharsPerIteration*aIters/time;
    
    TBuf<128> variation;
    variation.Format(KVariationCPI, aRotation, aSrcScreenMode, aDstScreenMode, aIters, aNumCharsPerIteration);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
    
    SqlInsert(&aTestName, &variation,       &KCharacterRate,  characterRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,   TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,   TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
Reports analysis results when there is one total result given the number of itertions
This function should only be used if you cannot obtain the times for each iteration of 
the test and ResultsAnalysis cannot be used.

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
*/
EXPORT_C void CTProfiler::ResultsAnalysisAverageByIterations(const TDesC& aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters)
    {
    PROFILER_TEST(iResultsInitalised);
    
    TUint32 result = iResults[0]/aIters;
    
    TBuf<128> variation;
    variation.Format(KVariation, aRotation, aSrcScreenMode, aDstScreenMode,aIters);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;
    variationMin.Format(KMinTime, &variation);  

    SqlInsert(&aTestName, &variation,       &KMicroSeconds,  result);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,  TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,  TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
 * Generate an SQL insert statement into the logs, intended for subsequent
 * extraction by a script to load into the database.
 * Inserts data fields for 
 * (sessionid, testname, testattribute, testunits, testresultvalue).
 * An example insertion:
 * <code>
 * SQL_UPLOAD_VERSION_0:insert into performance.testresultstable (sessionid, testname, testattribute, testunits, testresultvalue) values
 * SQL_UPLOAD_VERSION_0:('1136416860', 'GRAPHICS-UI-BENCH-S60-0009', 'Rot_0_SrcMode_0_DestMode_0_Iters_25', 'pixels/second', '394159');
 * </code>
 * 
 * @param aTestName Column testname
 * @param aTestAttribute Column testattribute
 * @param aTestUnit Column testunits
 * @param aTestResultValue Column testresultvalue
 */
EXPORT_C void CTProfiler::SqlInsert(const TDesC* aTestName, const TDesC* aTestAttribute, const TDesC* aTestUnit, TInt32 aTestResultValue)
    {
    TBuf<200>scratchPad;
    /*
     * There is a 256 character limit on logging output, and a 7 vararg limit
     * on the macro we can use to issue printfs to the test framework.  Each
     * output line which has SQL in it needs to have a marker at the front so
     * that a script can reliably extract the SQL statements.  Hence we use
     * the following incremental strategy of getting our SQL statements output
     * into the logs.
     */
    scratchPad.Format(KSqlInsert);
    PROFILER_INFO_PRINTF2(_L("%S"), &scratchPad);
    scratchPad.Format(KSqlData, iSessionId, aTestName, aTestAttribute, aTestUnit, aTestResultValue); 
    PROFILER_INFO_PRINTF2(_L("%S"), &scratchPad);
    }

/**
Reports analysis results for frame rates

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumPixels is the number of pixels rendered
*/
EXPORT_C void CTProfiler::ResultsAnalysisFrameRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixelsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);
    TReal time = (TReal)iResults[0] / 1000000;
    TInt32 pixelRate = aNumPixelsPerIteration * aIters / time;
    TInt32 frameRate = aIters / time;
    
    TBuf<128> variation;
    variation.Format(KVariation, aRotation, aSrcScreenMode, aDstScreenMode, aIters);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
    
    SqlInsert(&aTestName, &variation,       &KPixelsPerSecond,  pixelRate);
    SqlInsert(&aTestName, &variation,       &KFrameRate,        frameRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,     TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,     TimeMin());

    iResultsInitalised = EFalse;
    }

/**
Reports analysis results for screen rotation rates

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumPixels is the number of pixels rendered
*/
EXPORT_C void CTProfiler::ResultsAnalysisScreenRotationRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixelsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);
    TReal time = (TReal)iResults[0] / 1000000;
    TInt32 pixelRate = aNumPixelsPerIteration * aIters / time;
    TInt32 frameRate = aIters / time;
    
    TBuf<128> variation;
    variation.Format(KVariationPPI, aRotation, aSrcScreenMode, aDstScreenMode, aIters, aNumPixelsPerIteration);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
    
    SqlInsert(&aTestName, &variation,       &KPixelsPerSecond,  pixelRate);
    SqlInsert(&aTestName, &variation,       &KFrameRate,        frameRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,     TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,     TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
Reports analysis results for Z Order Switching rates

@param aTestName is the name of the test case
@param aZorderSwitching is the z Order Switching being used in the test
@param aSrcScreenMode is the source screen mode being used, 
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumPixels is the number of pixels rendered
*/
EXPORT_C void CTProfiler::ResultsAnalysisZorderSwitchingRate(const TDesC & aTestName, TInt aZorderSwitching, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixelsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);
    TReal time = (TReal)iResults[0] / 1000000;
    TInt32 pixelRate = aNumPixelsPerIteration * aIters / time;
    TInt32 frameRate = aIters / time;
    
    TBuf<128> variation;
    variation.Format(KVariationZOrder, aZorderSwitching, aSrcScreenMode, aDstScreenMode, aIters, aNumPixelsPerIteration);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
    
    SqlInsert(&aTestName, &variation,       &KPixelsPerSecond,  pixelRate);
    SqlInsert(&aTestName, &variation,       &KFrameRate,        frameRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,     TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,     TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
This function calclulates the mean without deleting any of the values from iResults
*/  
EXPORT_C TUint32 CTProfiler::Mean()
    {
    TInt64 total = 0;   
    TInt count1 = iResults.Count();
    for (TInt count = 0; count < iResults.Count(); count++)
        {
        total += iResults[count];
        }
    return ((TUint32)(total/(iResults.Count())));   
    }


/**
Reports analysis of results when there are more then one values by calculating the average based on value of iResult.count().

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
*/
EXPORT_C void CTProfiler::ResultsAnalysisAverageByNumberOfIterations(const TDesC& aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode,TInt aIters)
    {
    PROFILER_TEST(iResultsInitalised);
    TBuf<128> variation;
    variation.Format(KVariation, aRotation, aSrcScreenMode, aDstScreenMode, aIters);
    TBuf<128> variationMean;
    variationMean.Format(KMean, &variation);   
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;   
    variationMin.Format(KMinTime, &variation);
 
    SqlInsert(&aTestName, &variationMean, &KMicroSeconds,  Mean());
    SqlInsert(&aTestName, &variationMax,  &KMicroSeconds,  TimeMax());
    SqlInsert(&aTestName, &variationMin,  &KMicroSeconds,  TimeMin());
    
    iResultsInitalised = EFalse;
    }

/**
Output all results.
*/
EXPORT_C void CTProfiler::ShowResultArrayInTimingOrder()
    {
    for(TInt i=0; i < iResultsTimingOrder.Count(); i++)
        {
        PROFILER_INFO_PRINTF3(_L("iResultsTimingOrder[%4d]: %8d"),i, iResultsTimingOrder[i]);
        }
    }

/**
 @param aStoreResultInTimingOrder Signify whether the results should be stored as they coming   
 */
EXPORT_C void CTProfiler::SetStoreResultInTimingOrder(TBool aStoreResultInTimingOrder)
    {
    iStoreResultInTimingOrder = aStoreResultInTimingOrder;
    }

/**
Reports analysis results for glyph rates

@param aTestName is the name of the test case
@param aRotation is the screen rotation being used in the test
@param aSrcScreenMode is the source screen mode being used, 
i.e. for bitmap display conversion the source and destinations bitmaps maybe different
@param aDstScreenMode is the destination screen mode (usually the display screen mode)
@param aIters is the number of iterations used in the test
@param aNumGlyphs is the number of glyphs used per iteration
*/

EXPORT_C void CTProfiler::ResultsAnalysisGlyphRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumGlyphsPerIteration)
    {
    PROFILER_TEST(iResultsInitalised);
    TReal time = (iResults.Count() > 0) ? (TReal)iResults[0] / 1000000 : 0;
    TInt32 glyphRate = aNumGlyphsPerIteration*aIters/time;

    TBuf<128> variation;
    variation.Format(KVariationCPI, aRotation, aSrcScreenMode, aDstScreenMode, aIters, aNumGlyphsPerIteration);
    TBuf<128> variationMax;
    variationMax.Format(KMaxTime, &variation);
    TBuf<128> variationMin;
    variationMin.Format(KMinTime, &variation);

    SqlInsert(&aTestName, &variation,       &KGlyphRate,  glyphRate);
    SqlInsert(&aTestName, &variationMax,    &KMicroSeconds,  TimeMax());
    SqlInsert(&aTestName, &variationMin,    &KMicroSeconds,  TimeMin());

    iResultsInitalised = EFalse;
    }

