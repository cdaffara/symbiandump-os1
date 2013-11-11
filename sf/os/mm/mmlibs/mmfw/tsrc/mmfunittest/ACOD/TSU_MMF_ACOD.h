
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_ACOD_H__
#define __TSU_MMF_ACOD_H__

//#include <e32base.h>
//#include <e32test.h>
//#include <e32keys.h>
//#include <c32comm.h>
//#include <f32file.h>

#include <testframework.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>

#include "TSU_MMF_ACOD_BufferSizes.h"

class CTestSuite ;
class CTestSuite_MMF_ACOD;

/** @xxxx
 * @class CTestStep_MMF_ACOD TSU_MMF_ACOD.h "TSU_MMF_ACOD.h"
 * @brief Test class that enables MMF Audio Codec tests.
 * It's a base class for all test steps.
 */
class CTestStep_MMF_ACOD : public CTestStep
	{
public:
	CTestStep_MMF_ACOD();
	virtual ~CTestStep_MMF_ACOD();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite_MMF_ACOD* aTestSuite )
		{ iACODTestSuite = aTestSuite;}  
protected:
	void GetComparisonFilenameFromCodecUidL(TInt aCodecUID, TDes& aFilename);
	void GetComparisonFilenameFromFourCCL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC, TDes& aFilename);
	void GetSourceFilenameFromCodecUidL(TInt aCodecUID, TDes& aFilename);
	void GetSourceFilenameFromFourCCL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC, TDes& aFilename);

	TBool CheckOutputAgainstComparisonFileL(const TDesC& aFilename, TUint aPosition);	
	TBool CheckSrcBytesProcessedL(TUint aCount);
	TBool CheckDstBytesAddedL(TUint aCount);
	TBool CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL();
	TBool CheckReturnValueL(TUint aCount);

	TBool TestProcessL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize, TUint aSrcBufferLength = 0);
	TBool TestProcessLLoopFiveTimesL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize, TUint aSrcBufferLength = 0, TBool aCheckExpectedBytesProcessed = ETrue);
	TBool TestRepositionL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize);
	TBool TestNewL(TUid aCodecUID);
	TBool TestNewL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC);
	TBool TestNewLPreferredSupplierL();

	TBool TestMemoryScribbleL(TUid aCodecUID);
	TBool TestMemoryLeakL(TUid aCodecUID);
	TBool TestProcessLOutOfMemoryL(TUid aCodecUID);
	TBool TestNewLOutOfMemoryL(TUid aCodecUID);
	
	void FillSourceBufferFromSourceFileL(CMMFBuffer* aSrcBuffer, const TDesC& aFilename, TUint sPosition);

protected:
	// pointer to suite which owns this test 
	const CTestSuite_MMF_ACOD* iACODTestSuite;

	TUint iExpectedDstBytesAdded[5];
	TUint iExpectedSrcBytesProcessed[5];
	TCodecProcessResult iExpectedReturnValue[5];
	TBool iExpectedLeaveErrorCode;

	TCodecProcessResult iCodecProcessResult;
	HBufC8* iOutputDataBuffer;
	TUint iTotalDstBytesAdded;
	TUint iTotalSrcBytesProcessed;
	TInt iFileDataLeft;
	TBool iSingleIteration;
	};


//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0001
 * @test Req. under test REQ172.6.3 
 */
class CTest_MMF_ACOD_U_0001 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0001() ;
	~CTest_MMF_ACOD_U_0001(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0002
 * @test Req. under test REQ172.6.2 REQ172.11.1
 */
class CTest_MMF_ACOD_U_0002 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0002() ;
	~CTest_MMF_ACOD_U_0002(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 
 
/** @xxxx
 * @class CTest_MMF_ACOD_U_0003
 * @test Req. under test REQ172.6.1 REQ172.11.1
 */
class CTest_MMF_ACOD_U_0003 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0003() ;
	~CTest_MMF_ACOD_U_0003(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0004
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0004 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0004() ;
	~CTest_MMF_ACOD_U_0004(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0005
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0005 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0005() ;
	~CTest_MMF_ACOD_U_0005(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0006
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0006 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0006() ;
	~CTest_MMF_ACOD_U_0006(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0007
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0007 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0007() ;
	~CTest_MMF_ACOD_U_0007(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0008
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0008 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0008() ;
	~CTest_MMF_ACOD_U_0008(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0009
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0009 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0009() ;
	~CTest_MMF_ACOD_U_0009(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0010
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0010 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0010() ;
	~CTest_MMF_ACOD_U_0010(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0011
 * @test Req. under test REQ172.6.2 REQ172.11.3
 */
class CTest_MMF_ACOD_U_0011 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0011() ;
	~CTest_MMF_ACOD_U_0011(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0012
 * @test Req. under test REQ172.6.1 REQ172.11.3
 */
class CTest_MMF_ACOD_U_0012 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0012() ;
	~CTest_MMF_ACOD_U_0012(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0013
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0013 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0013() ;
	~CTest_MMF_ACOD_U_0013(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0014
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0014 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0014() ;
	~CTest_MMF_ACOD_U_0014(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0015
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0015 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0015() ;
	~CTest_MMF_ACOD_U_0015(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0016
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0016 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0016() ;
	~CTest_MMF_ACOD_U_0016(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0017
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0017 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0017() ;
	~CTest_MMF_ACOD_U_0017(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0018
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0018 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0018() ;
	~CTest_MMF_ACOD_U_0018(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0019
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0019 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0019() ;
	~CTest_MMF_ACOD_U_0019(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0020
 * @test Req. under test REQ172.6.2 REQ172.11.4
 */
class CTest_MMF_ACOD_U_0020 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0020() ;
	~CTest_MMF_ACOD_U_0020(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0021
 * @test Req. under test REQ172.6.1 REQ172.11.4
 */
class CTest_MMF_ACOD_U_0021 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0021() ;
	~CTest_MMF_ACOD_U_0021(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0022
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0022 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0022() ;
	~CTest_MMF_ACOD_U_0022(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0023
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0023 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0023() ;
	~CTest_MMF_ACOD_U_0023(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0024
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0024 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0024() ;
	~CTest_MMF_ACOD_U_0024(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0025
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0025 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0025() ;
	~CTest_MMF_ACOD_U_0025(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0026
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0026 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0026() ;
	~CTest_MMF_ACOD_U_0026(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0027
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0027 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0027() ;
	~CTest_MMF_ACOD_U_0027(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0028
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0028 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0028() ;
	~CTest_MMF_ACOD_U_0028(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0029
 * @test Req. under test REQ172.6.2 REQ172.11.5
 */
class CTest_MMF_ACOD_U_0029 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0029() ;
	~CTest_MMF_ACOD_U_0029(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0030
 * @test Req. under test REQ172.6.1 REQ172.11.5
 */
class CTest_MMF_ACOD_U_0030 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0030() ;
	~CTest_MMF_ACOD_U_0030(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0031
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0031 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0031() ;
	~CTest_MMF_ACOD_U_0031(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0032
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0032 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0032() ;
	~CTest_MMF_ACOD_U_0032(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0033
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0033 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0033() ;
	~CTest_MMF_ACOD_U_0033(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0034
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0034 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0034() ;
	~CTest_MMF_ACOD_U_0034(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0035
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0035 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0035() ;
	~CTest_MMF_ACOD_U_0035(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0036
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0036 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0036() ;
	~CTest_MMF_ACOD_U_0036(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0037
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0037 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0037() ;
	~CTest_MMF_ACOD_U_0037(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0038
 * @test Req. under test REQ172.6.2 REQ172.11.6
 */
class CTest_MMF_ACOD_U_0038 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0038() ;
	~CTest_MMF_ACOD_U_0038(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0039
 * @test Req. under test REQ172.6.2 REQ172.11.6
 */
class CTest_MMF_ACOD_U_0039 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0039() ;
	~CTest_MMF_ACOD_U_0039(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0040
 * @test Req. under test REQ172.6.2 REQ172.11.6
 */
class CTest_MMF_ACOD_U_0040 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0040() ;
	~CTest_MMF_ACOD_U_0040(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0041
 * @test Req. under test REQ172.6.2 REQ172.11.6
 */
class CTest_MMF_ACOD_U_0041 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0041() ;
	~CTest_MMF_ACOD_U_0041(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0042
 * @test Req. under test REQ172.6.1 REQ172.11.6
 */
class CTest_MMF_ACOD_U_0042 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0042() ;
	~CTest_MMF_ACOD_U_0042(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0043
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0043 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0043() ;
	~CTest_MMF_ACOD_U_0043(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0044
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0044 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0044() ;
	~CTest_MMF_ACOD_U_0044(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0045
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0045 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0045() ;
	~CTest_MMF_ACOD_U_0045(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0046
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0046 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0046() ;
	~CTest_MMF_ACOD_U_0046(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0047
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0047 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0047() ;
	~CTest_MMF_ACOD_U_0047(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0048
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0048 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0048() ;
	~CTest_MMF_ACOD_U_0048(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0049
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0049 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0049() ;
	~CTest_MMF_ACOD_U_0049(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0050
 * @test Req. under test REQ172.6.2
 */
class CTest_MMF_ACOD_U_0050 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0050() ;
	~CTest_MMF_ACOD_U_0050(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0051
 * @test Req. under test REQ172.6.1
 */
class CTest_MMF_ACOD_U_0051 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0051() ;
	~CTest_MMF_ACOD_U_0051(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0052
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0052 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0052() ;
	~CTest_MMF_ACOD_U_0052(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0053
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0053 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0053() ;
	~CTest_MMF_ACOD_U_0053(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0054
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0054 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0054() ;
	~CTest_MMF_ACOD_U_0054(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0055
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0055 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0055() ;
	~CTest_MMF_ACOD_U_0055(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0056
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0056 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0056() ;
	~CTest_MMF_ACOD_U_0056(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0057
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0057 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0057() ;
	~CTest_MMF_ACOD_U_0057(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0058
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0058 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0058() ;
	~CTest_MMF_ACOD_U_0058(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0059
 * @test Req. under test REQ172.6.2, REQ172.11.7
 */
class CTest_MMF_ACOD_U_0059 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0059() ;
	~CTest_MMF_ACOD_U_0059(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0060
 * @test Req. under test REQ172.6.1, REQ172.11.7
 */
class CTest_MMF_ACOD_U_0060 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0060() ;
	~CTest_MMF_ACOD_U_0060(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0061
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0061 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0061() ;
	~CTest_MMF_ACOD_U_0061(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0062
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0062 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0062() ;
	~CTest_MMF_ACOD_U_0062(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0063
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0063 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0063() ;
	~CTest_MMF_ACOD_U_0063(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0064
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0064 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0064() ;
	~CTest_MMF_ACOD_U_0064(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0065
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0065 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0065() ;
	~CTest_MMF_ACOD_U_0065(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0066
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0066 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0066() ;
	~CTest_MMF_ACOD_U_0066(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0067
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0067 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0067() ;
	~CTest_MMF_ACOD_U_0067(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0068
 * @test Req. under test REQ172.6.2, REQ172.11.8
 */
class CTest_MMF_ACOD_U_0068 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0068() ;
	~CTest_MMF_ACOD_U_0068(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0069
 * @test Req. under test REQ172.6.1, REQ172.11.8
 */
class CTest_MMF_ACOD_U_0069 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0069() ;
	~CTest_MMF_ACOD_U_0069(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0070
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0070 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0070() ;
	~CTest_MMF_ACOD_U_0070(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0071
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0071 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0071() ;
	~CTest_MMF_ACOD_U_0071(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0072
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0072 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0072() ;
	~CTest_MMF_ACOD_U_0072(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0073
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0073 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0073() ;
	~CTest_MMF_ACOD_U_0073(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0074
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0074 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0074() ;
	~CTest_MMF_ACOD_U_0074(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0075
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0075 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0075() ;
	~CTest_MMF_ACOD_U_0075(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0076
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0076 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0076() ;
	~CTest_MMF_ACOD_U_0076(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0077
 * @test Req. under test REQ172.6.2, REQ172.11.9
 */
class CTest_MMF_ACOD_U_0077 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0077() ;
	~CTest_MMF_ACOD_U_0077(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0078
 * @test Req. under test REQ172.6.1, REQ172.11.9
 */
class CTest_MMF_ACOD_U_0078 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0078() ;
	~CTest_MMF_ACOD_U_0078(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0079
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0079 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0079() ;
	~CTest_MMF_ACOD_U_0079(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0080
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0080 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0080() ;
	~CTest_MMF_ACOD_U_0080(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0081
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0081 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0081() ;
	~CTest_MMF_ACOD_U_0081(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0082
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0082 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0082() ;
	~CTest_MMF_ACOD_U_0082(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0083
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0083 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0083() ;
	~CTest_MMF_ACOD_U_0083(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0084
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0084 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0084() ;
	~CTest_MMF_ACOD_U_0084(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0085
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0085 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0085() ;
	~CTest_MMF_ACOD_U_0085(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0086
 * @test Req. under test REQ172.6.2, REQ172.11.25
 */
class CTest_MMF_ACOD_U_0086 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0086() ;
	~CTest_MMF_ACOD_U_0086(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0087
 * @test Req. under test REQ172.6.1, REQ172.11.25
 */
class CTest_MMF_ACOD_U_0087 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0087() ;
	~CTest_MMF_ACOD_U_0087(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0088
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0088 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0088() ;
	~CTest_MMF_ACOD_U_0088(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0089
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0089 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0089() ;
	~CTest_MMF_ACOD_U_0089(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0090
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0090 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0090() ;
	~CTest_MMF_ACOD_U_0090(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0091
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0091 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0091() ;
	~CTest_MMF_ACOD_U_0091(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0092
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0092 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0092() ;
	~CTest_MMF_ACOD_U_0092(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0093
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0093 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0093() ;
	~CTest_MMF_ACOD_U_0093(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0094
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0094 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0094() ;
	~CTest_MMF_ACOD_U_0094(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0095
 * @test Req. under test REQ172.6.2, REQ172.11.11
 */
class CTest_MMF_ACOD_U_0095 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0095() ;
	~CTest_MMF_ACOD_U_0095(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0096
 * @test Req. under test REQ172.6.1, REQ172.11.11
 */
class CTest_MMF_ACOD_U_0096 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0096() ;
	~CTest_MMF_ACOD_U_0096(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0097
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0097 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0097() ;
	~CTest_MMF_ACOD_U_0097(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0098
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0098 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0098() ;
	~CTest_MMF_ACOD_U_0098(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0099
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0099 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0099() ;
	~CTest_MMF_ACOD_U_0099(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0100
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0100 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0100() ;
	~CTest_MMF_ACOD_U_0100(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0101
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0101 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0101() ;
	~CTest_MMF_ACOD_U_0101(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0102
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0102 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0102() ;
	~CTest_MMF_ACOD_U_0102(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0103
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0103 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0103() ;
	~CTest_MMF_ACOD_U_0103(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0104
 * @test Req. under test REQ172.6.2, REQ172.11.13
 */
class CTest_MMF_ACOD_U_0104 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0104() ;
	~CTest_MMF_ACOD_U_0104(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0105
 * @test Req. under test REQ172.6.1, REQ172.11.13
 */
class CTest_MMF_ACOD_U_0105 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0105() ;
	~CTest_MMF_ACOD_U_0105(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0106
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0106 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0106() ;
	~CTest_MMF_ACOD_U_0106(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0107
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0107 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0107() ;
	~CTest_MMF_ACOD_U_0107(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0108
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0108 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0108() ;
	~CTest_MMF_ACOD_U_0108(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0109
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0109 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0109() ;
	~CTest_MMF_ACOD_U_0109(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0110
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0110 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0110() ;
	~CTest_MMF_ACOD_U_0110(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0111
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0111 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0111() ;
	~CTest_MMF_ACOD_U_0111(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0112
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0112 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0112() ;
	~CTest_MMF_ACOD_U_0112(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0113
 * @test Req. under test REQ172.6.2, REQ172.11.15
 */
class CTest_MMF_ACOD_U_0113 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0113() ;
	~CTest_MMF_ACOD_U_0113(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0114
 * @test Req. under test REQ172.6.1, REQ172.11.15
 */
class CTest_MMF_ACOD_U_0114 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0114() ;
	~CTest_MMF_ACOD_U_0114(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0115
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0115 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0115() ;
	~CTest_MMF_ACOD_U_0115(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0116
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0116 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0116() ;
	~CTest_MMF_ACOD_U_0116(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0117
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0117 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0117() ;
	~CTest_MMF_ACOD_U_0117(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0118
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0118 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0118() ;
	~CTest_MMF_ACOD_U_0118(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0119
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0119 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0119() ;
	~CTest_MMF_ACOD_U_0119(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0120
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0120 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0120() ;
	~CTest_MMF_ACOD_U_0120(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0121
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0121 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0121() ;
	~CTest_MMF_ACOD_U_0121(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0122
 * @test Req. under test REQ172.6.2, REQ172.11.12
 */
class CTest_MMF_ACOD_U_0122 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0122() ;
	~CTest_MMF_ACOD_U_0122(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0123
 * @test Req. under test REQ172.6.1, REQ172.11.12
 */
class CTest_MMF_ACOD_U_0123 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0123() ;
	~CTest_MMF_ACOD_U_0123(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0124
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0124 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0124() ;
	~CTest_MMF_ACOD_U_0124(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0125
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0125 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0125() ;
	~CTest_MMF_ACOD_U_0125(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0126
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0126 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0126() ;
	~CTest_MMF_ACOD_U_0126(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0127
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0127 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0127() ;
	~CTest_MMF_ACOD_U_0127(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0128
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0128 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0128() ;
	~CTest_MMF_ACOD_U_0128(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0129
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0129 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0129() ;
	~CTest_MMF_ACOD_U_0129(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0130
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0130 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0130() ;
	~CTest_MMF_ACOD_U_0130(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0131
 * @test Req. under test REQ172.6.2, REQ172.11.14
 */
class CTest_MMF_ACOD_U_0131 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0131() ;
	~CTest_MMF_ACOD_U_0131(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0132
 * @test Req. under test REQ172.6.1, REQ172.11.14
 */
class CTest_MMF_ACOD_U_0132 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0132() ;
	~CTest_MMF_ACOD_U_0132(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0133
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0133 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0133() ;
	~CTest_MMF_ACOD_U_0133(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0134
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0134 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0134() ;
	~CTest_MMF_ACOD_U_0134(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0135
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0135 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0135() ;
	~CTest_MMF_ACOD_U_0135(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0136
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0136 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0136() ;
	~CTest_MMF_ACOD_U_0136(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0137
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0137 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0137() ;
	~CTest_MMF_ACOD_U_0137(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0138
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0138 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0138() ;
	~CTest_MMF_ACOD_U_0138(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0139
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0139 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0139() ;
	~CTest_MMF_ACOD_U_0139(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0140
 * @test Req. under test REQ172.6.2, REQ172.11.17
 */
class CTest_MMF_ACOD_U_0140 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0140() ;
	~CTest_MMF_ACOD_U_0140(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0141
 * @test Req. under test REQ172.6.1, REQ172.11.17
 */
class CTest_MMF_ACOD_U_0141 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0141() ;
	~CTest_MMF_ACOD_U_0141(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0142
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0142 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0142() ;
	~CTest_MMF_ACOD_U_0142(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0143
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0143 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0143() ;
	~CTest_MMF_ACOD_U_0143(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0144
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0144 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0144() ;
	~CTest_MMF_ACOD_U_0144(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0145
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0145 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0145() ;
	~CTest_MMF_ACOD_U_0145(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0146
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0146 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0146() ;
	~CTest_MMF_ACOD_U_0146(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0147
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0147 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0147() ;
	~CTest_MMF_ACOD_U_0147(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0148
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0148 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0148() ;
	~CTest_MMF_ACOD_U_0148(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0149
 * @test Req. under test REQ172.6.2, REQ172.11.18
 */
class CTest_MMF_ACOD_U_0149 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0149() ;
	~CTest_MMF_ACOD_U_0149(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0150
 * @test Req. under test REQ172.6.1, REQ172.11.18
 */
class CTest_MMF_ACOD_U_0150 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0150() ;
	~CTest_MMF_ACOD_U_0150(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0151
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0151 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0151() ;
	~CTest_MMF_ACOD_U_0151(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0152
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0152 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0152() ;
	~CTest_MMF_ACOD_U_0152(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0153
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0153 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0153() ;
	~CTest_MMF_ACOD_U_0153(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0154
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0154 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0154() ;
	~CTest_MMF_ACOD_U_0154(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0155
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0155 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0155() ;
	~CTest_MMF_ACOD_U_0155(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0156
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0156 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0156() ;
	~CTest_MMF_ACOD_U_0156(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0157
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0157 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0157() ;
	~CTest_MMF_ACOD_U_0157(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0158
 * @test Req. under test REQ172.6.2, REQ172.11.19
 */
class CTest_MMF_ACOD_U_0158 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0158() ;
	~CTest_MMF_ACOD_U_0158(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0159
 * @test Req. under test REQ172.6.1, REQ172.11.19
 */
class CTest_MMF_ACOD_U_0159 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0159() ;
	~CTest_MMF_ACOD_U_0159(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0160
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0160 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0160() ;
	~CTest_MMF_ACOD_U_0160(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0161
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0161 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0161() ;
	~CTest_MMF_ACOD_U_0161(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0162
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0162 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0162() ;
	~CTest_MMF_ACOD_U_0162(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0163
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0163 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0163() ;
	~CTest_MMF_ACOD_U_0163(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0164
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0164 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0164() ;
	~CTest_MMF_ACOD_U_0164(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0165
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0165 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0165() ;
	~CTest_MMF_ACOD_U_0165(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0166
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0166 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0166() ;
	~CTest_MMF_ACOD_U_0166(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0167
 * @test Req. under test REQ172.6.2, REQ172.11.20
 */
class CTest_MMF_ACOD_U_0167 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0167() ;
	~CTest_MMF_ACOD_U_0167(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0168
 * @test Req. under test REQ172.6.1, REQ172.11.20
 */
class CTest_MMF_ACOD_U_0168 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0168() ;
	~CTest_MMF_ACOD_U_0168(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0169
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0169 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0169() ;
	~CTest_MMF_ACOD_U_0169(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0170
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0170 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0170() ;
	~CTest_MMF_ACOD_U_0170(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0171
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0171 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0171() ;
	~CTest_MMF_ACOD_U_0171(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0172
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0172 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0172() ;
	~CTest_MMF_ACOD_U_0172(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0173
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0173 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0173() ;
	~CTest_MMF_ACOD_U_0173(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0174
 * @test Req. under test REQ172.6.4 REQ172.6.5
 */
class CTest_MMF_ACOD_U_0174 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0174() ;
	~CTest_MMF_ACOD_U_0174(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0175
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0175 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0175() ;
	~CTest_MMF_ACOD_U_0175(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0176
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0176 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0176() ;
	~CTest_MMF_ACOD_U_0176(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0177
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0177 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0177() ;
	~CTest_MMF_ACOD_U_0177(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0178
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0178 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0178() ;
	~CTest_MMF_ACOD_U_0178(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0179
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0179 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0179() ;
	~CTest_MMF_ACOD_U_0179(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0180
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0180 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0180() ;
	~CTest_MMF_ACOD_U_0180(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0181
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0181 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0181() ;
	~CTest_MMF_ACOD_U_0181(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0182
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0182 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0182() ;
	~CTest_MMF_ACOD_U_0182(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0183
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0183 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0183() ;
	~CTest_MMF_ACOD_U_0183(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0184
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0184 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0184() ;
	~CTest_MMF_ACOD_U_0184(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0185
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0185 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0185() ;
	~CTest_MMF_ACOD_U_0185(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0186
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0186 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0186() ;
	~CTest_MMF_ACOD_U_0186(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0187
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0187 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0187() ;
	~CTest_MMF_ACOD_U_0187(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0188
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0188 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0188() ;
	~CTest_MMF_ACOD_U_0188(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0189
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0189 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0189() ;
	~CTest_MMF_ACOD_U_0189(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0190
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0190 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0190() ;
	~CTest_MMF_ACOD_U_0190(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0191
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0191 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0191() ;
	~CTest_MMF_ACOD_U_0191(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0192
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0192 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0192() ;
	~CTest_MMF_ACOD_U_0192(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0193
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0193 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0193() ;
	~CTest_MMF_ACOD_U_0193(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0194
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0194 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0194() ;
	~CTest_MMF_ACOD_U_0194(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0195
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0195 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0195() ;
	~CTest_MMF_ACOD_U_0195(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0196
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0196 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0196() ;
	~CTest_MMF_ACOD_U_0196(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0197
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0197 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0197() ;
	~CTest_MMF_ACOD_U_0197(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0198
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0198 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0198() ;
	~CTest_MMF_ACOD_U_0198(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0199
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0199 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0199() ;
	~CTest_MMF_ACOD_U_0199(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0200
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0200 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0200() ;
	~CTest_MMF_ACOD_U_0200(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0201
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0201 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0201() ;
	~CTest_MMF_ACOD_U_0201(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0202
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0202 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0202() ;
	~CTest_MMF_ACOD_U_0202(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0203
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0203 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0203() ;
	~CTest_MMF_ACOD_U_0203(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0204
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0204 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0204() ;
	~CTest_MMF_ACOD_U_0204(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0205
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0205 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0205() ;
	~CTest_MMF_ACOD_U_0205(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0206
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0206 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0206() ;
	~CTest_MMF_ACOD_U_0206(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0207
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0207 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0207() ;
	~CTest_MMF_ACOD_U_0207(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0208
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0208 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0208() ;
	~CTest_MMF_ACOD_U_0208(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0209
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0209 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0209() ;
	~CTest_MMF_ACOD_U_0209(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0210
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0210 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0210() ;
	~CTest_MMF_ACOD_U_0210(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0211
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0211 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0211() ;
	~CTest_MMF_ACOD_U_0211(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0212
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0212 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0212() ;
	~CTest_MMF_ACOD_U_0212(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0213
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0213 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0213() ;
	~CTest_MMF_ACOD_U_0213(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0214
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0214 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0214() ;
	~CTest_MMF_ACOD_U_0214(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0215
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0215 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0215() ;
	~CTest_MMF_ACOD_U_0215(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0216
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0216 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0216() ;
	~CTest_MMF_ACOD_U_0216(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0217
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0217 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0217() ;
	~CTest_MMF_ACOD_U_0217(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0218
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0218 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0218() ;
	~CTest_MMF_ACOD_U_0218(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0219
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0219 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0219() ;
	~CTest_MMF_ACOD_U_0219(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0220
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0220 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0220() ;
	~CTest_MMF_ACOD_U_0220(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0221
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0221 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0221() ;
	~CTest_MMF_ACOD_U_0221(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0222
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0222 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0222() ;
	~CTest_MMF_ACOD_U_0222(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0223
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0223 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0223() ;
	~CTest_MMF_ACOD_U_0223(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0224
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0224 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0224() ;
	~CTest_MMF_ACOD_U_0224(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0225
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0225 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0225() ;
	~CTest_MMF_ACOD_U_0225(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0226
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0226 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0226() ;
	~CTest_MMF_ACOD_U_0226(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0227
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0227 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0227() ;
	~CTest_MMF_ACOD_U_0227(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0228
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0228 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0228() ;
	~CTest_MMF_ACOD_U_0228(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0229
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0229 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0229() ;
	~CTest_MMF_ACOD_U_0229(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0230
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0230 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0230() ;
	~CTest_MMF_ACOD_U_0230(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0231
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0231 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0231() ;
	~CTest_MMF_ACOD_U_0231(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0232
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0232 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0232() ;
	~CTest_MMF_ACOD_U_0232(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0233
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0233 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0233() ;
	~CTest_MMF_ACOD_U_0233(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0234
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0234 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0234() ;
	~CTest_MMF_ACOD_U_0234(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0235
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0235 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0235() ;
	~CTest_MMF_ACOD_U_0235(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0236
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0236 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0236() ;
	~CTest_MMF_ACOD_U_0236(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0237
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0237 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0237() ;
	~CTest_MMF_ACOD_U_0237(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0238
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0238 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0238() ;
	~CTest_MMF_ACOD_U_0238(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0239
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0239 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0239() ;
	~CTest_MMF_ACOD_U_0239(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0240
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0240 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0240() ;
	~CTest_MMF_ACOD_U_0240(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0241
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0241 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0241() ;
	~CTest_MMF_ACOD_U_0241(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0242
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0242 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0242() ;
	~CTest_MMF_ACOD_U_0242(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0243
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0243 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0243() ;
	~CTest_MMF_ACOD_U_0243(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0244
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0244 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0244() ;
	~CTest_MMF_ACOD_U_0244(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0245
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0245 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0245() ;
	~CTest_MMF_ACOD_U_0245(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0246
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0246 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0246() ;
	~CTest_MMF_ACOD_U_0246(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0247
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0247 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0247() ;
	~CTest_MMF_ACOD_U_0247(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0248
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0248 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0248() ;
	~CTest_MMF_ACOD_U_0248(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0249
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0249 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0249() ;
	~CTest_MMF_ACOD_U_0249(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0250
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0250 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0250() ;
	~CTest_MMF_ACOD_U_0250(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0251
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0251 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0251() ;
	~CTest_MMF_ACOD_U_0251(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0252
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0252 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0252() ;
	~CTest_MMF_ACOD_U_0252(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0253
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0253 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0253() ;
	~CTest_MMF_ACOD_U_0253(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0254
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0254 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0254() ;
	~CTest_MMF_ACOD_U_0254(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0255
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0255 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0255() ;
	~CTest_MMF_ACOD_U_0255(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0256
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0256 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0256() ;
	~CTest_MMF_ACOD_U_0256(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0257
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0257 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0257() ;
	~CTest_MMF_ACOD_U_0257(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0258
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0258 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0258() ;
	~CTest_MMF_ACOD_U_0258(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0259
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0259 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0259() ;
	~CTest_MMF_ACOD_U_0259(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0260
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0260 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0260() ;
	~CTest_MMF_ACOD_U_0260(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0261
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0261 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0261() ;
	~CTest_MMF_ACOD_U_0261(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0262
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0262 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0262() ;
	~CTest_MMF_ACOD_U_0262(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0263
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0263 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0263() ;
	~CTest_MMF_ACOD_U_0263(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0264
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0264 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0264() ;
	~CTest_MMF_ACOD_U_0264(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0265
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0265 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0265() ;
	~CTest_MMF_ACOD_U_0265(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0266
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0266 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0266() ;
	~CTest_MMF_ACOD_U_0266(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0267
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0267 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0267() ;
	~CTest_MMF_ACOD_U_0267(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0268
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0268 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0268() ;
	~CTest_MMF_ACOD_U_0268(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 


/** @xxxx
 * @class CTest_MMF_ACOD_U_0269
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0269 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0269() ;
	~CTest_MMF_ACOD_U_0269(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0270
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
class CTest_MMF_ACOD_U_0270 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0270() ;
	~CTest_MMF_ACOD_U_0270(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0271
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0271 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0271() ;
	~CTest_MMF_ACOD_U_0271(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0272
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0272 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0272() ;
	~CTest_MMF_ACOD_U_0272(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_ACOD_U_0273
 * @test Req. under test REQ172.11
 */
class CTest_MMF_ACOD_U_0273 : public CTestStep_MMF_ACOD
	{
public:
	CTest_MMF_ACOD_U_0273() ;
	~CTest_MMF_ACOD_U_0273(){} ;
	virtual TVerdict DoTestStepL();
	
	} ;

//------------------------------------------------------------------ 

#endif// TSU_MMF_ACOD_H__

