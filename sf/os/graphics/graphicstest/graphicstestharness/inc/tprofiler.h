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

#ifndef __TPROFILER_H__
#define __TPROFILER_H__

#include <e32base.h>
#include <test/tefexportconst.h>

class CTestStep;

_LIT(KSqlInsert,        "SQL_UPLOAD_VERSION_0:insert into performance.testresultstable (sessionid, testname, testattribute, testunits, testresultvalue) values ");
_LIT(KSqlData,          "SQL_UPLOAD_VERSION_0:('%u', '%S', '%S', '%S', '%d');");
_LIT(KMicroSeconds,     "us");
_LIT(KPixelsPerSecond,  "pixels/second");
_LIT(KFrameRate,        "frames/second");
_LIT(KCharacterRate,    "characters/second");
_LIT(KGlyphRate,        "glyphs/second");
_LIT(KVariation,        "Rot_%i_SrcMode_%i_DestMode_%i_Iters_%i");
_LIT(KVariationPPI,     "Rot_%i_SrcMode_%i_DestMode_%i_Iters_%i_PPI_%i");
_LIT(KVariationZOrder,  "ZOrd_%i_SrcMode_%i_DestMode_%i_Iters_%i_PPI_%i");
_LIT(KVariationCPI,     "Rot_%i_SrcMode_%i_DestMode_%i_Iters_%i_CPI_%i");
_LIT(KMaxTime,          "%S_Max");
_LIT(KMinTime,          "%S_Min");
_LIT(KMean,             "%S_Mean");
_LIT(KTrimmedMean,      "%S_TrimmedMean");

class CTProfiler : public CBase
    {
public:
    IMPORT_C static CTProfiler* NewL(CTestStep& aTestStep);
    IMPORT_C ~CTProfiler();
    IMPORT_C void StartTimer();
    IMPORT_C TInt64 StopTimer();
    IMPORT_C void InitResults();
    IMPORT_C void MarkResultSetL();
    IMPORT_C void MarkResultSetAndSuspendL();
    IMPORT_C TUint32 GetTrimedMean();
    IMPORT_C TUint32 Mean();
    IMPORT_C TInt PercentageChange(TInt aFirstTime, TInt aSecondTime);
    IMPORT_C void ResultsAnalysis(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters);
    IMPORT_C void ResultsAnalysisPixelRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);
    IMPORT_C void ResultsAnalysisCharacterRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);
    IMPORT_C void ResultsAnalysisAverageByIterations(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters);
    IMPORT_C void ResultsAnalysisAverageByNumberOfIterations(const TDesC& aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters);
    IMPORT_C void ResultsAnalysisFrameRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);
    IMPORT_C void ResultsAnalysisScreenRotationRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);
    IMPORT_C void ResultsAnalysisZorderSwitchingRate(const TDesC & aTestName, TInt aZorderSwitching, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);
	IMPORT_C void FreeResultsMemory();
	IMPORT_C TUint32 TimeMax();
	IMPORT_C TUint32 TimeMin();
	IMPORT_C void ShowResultArrayInTimingOrder();
	IMPORT_C void SetStoreResultInTimingOrder(TBool aStoreResultInTimingOrder);
	IMPORT_C void SqlInsert(const TDesC* aTestName, const TDesC* aTestAttribute, const TDesC* aTestUnit, TInt32 aTestResultValue);
	IMPORT_C void ResultsAnalysisGlyphRate(const TDesC & aTestName, TInt aRotation, TInt aSrcScreenMode, TInt aDstScreenMode, TInt aIters, TInt aNumPixels);

private:
    CTProfiler(CTestStep& aTestStep);
    void ConstructL();
    
private:
    TUint32             iStart;
    TUint32             iEnd;
    TUint32             iDiff;
    TInt                iFreq;  
    RArray<TUint32>     iResults;
    RArray<TUint32>     iResultsTimingOrder;
    TBool               iResultsInitalised;
    CTestStep&          iTestStep;
    TBool               iStoreResultInTimingOrder;
    /*
     * The session identifier is a number which allows a number of
     * related performance metrics for a given test to be grouped
     * under the same "session".
     */
	 TUint32			iSessionId;
    };

#endif


