
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_AFMT_TestSteps_H__
#define __TSU_MMF_AFMT_TestSteps_H__

#include "TSU_MMF_AFMT.h"
#include "Tsu_mmf_afmt_newl.h"		// for NewL tests
#include "Tsu_mmf_afmt_Interval.h"	// for Interval tests
#include "Tsu_mmf_afmt_oom.h"		// for OutOfMemory tests
#include "Tsu_mmf_afmt_Duration.h"	// for Duration tests
#include "Tsu_mmf_afmt_FillBuffer.h"// for FillBufferL tests
#include "Tsu_mmf_afmt_NumChannels.h"//for NumChannels/SetNumChannels tests
#include "Tsu_mmf_afmt_SampleRate.h"// for SampleRate/SetSampleRate tests


		  
/** @xxxx
 * @class CTest_MMF_AFMT_U_0001
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0001 : public CTestStep_MMF_AFMT_NewL
{
public:
	CTest_MMF_AFMT_U_0001() ;
	~CTest_MMF_AFMT_U_0001(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_AFMT_U_0002
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0002 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0002() ;
	~CTest_MMF_AFMT_U_0002(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 

/** @xxxx
 * @class CTest_MMF_AFMT_U_0003
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0003 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0003() ;
	~CTest_MMF_AFMT_U_0003(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0004
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0004 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0004() ;
	~CTest_MMF_AFMT_U_0004(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0016
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0016 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0016() ;
	~CTest_MMF_AFMT_U_0016(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0018
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0018 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0018() ;
	~CTest_MMF_AFMT_U_0018(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0020
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0020 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0020() ;
	~CTest_MMF_AFMT_U_0020(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0025
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0025 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0025() ;
	~CTest_MMF_AFMT_U_0025(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0027
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0027 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0027() ;
	~CTest_MMF_AFMT_U_0027(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0028
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0028 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0028() ;
	~CTest_MMF_AFMT_U_0028(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0029
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0029 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0029() ;
	~CTest_MMF_AFMT_U_0029(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0030
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0030 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0030() ;
	~CTest_MMF_AFMT_U_0030(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0031
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0031 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0031() ;
	~CTest_MMF_AFMT_U_0031(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0032
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0032 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0032() ;
	~CTest_MMF_AFMT_U_0032(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0033
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0033 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0033() ;
	~CTest_MMF_AFMT_U_0033(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0034
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0034 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0034() ;
	~CTest_MMF_AFMT_U_0034(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;




//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0036
 * Test FillBufferL with PCM16 data.
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0036 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0036() ;
	~CTest_MMF_AFMT_U_0036(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0037
 * Test FillBufferL with ADPCM data.
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0037 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0037() ;
	~CTest_MMF_AFMT_U_0037(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0038
 * Test FillBufferL with GSM data.
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0038 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0038() ;
	~CTest_MMF_AFMT_U_0038(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0042
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0042 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0042() ;
	~CTest_MMF_AFMT_U_0042(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0043
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0043 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0043() ;
	~CTest_MMF_AFMT_U_0043(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0044
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0044 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0044() ;
	~CTest_MMF_AFMT_U_0044(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0045
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0045 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0045() ;
	~CTest_MMF_AFMT_U_0045(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0056
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0056 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0056() ;
	~CTest_MMF_AFMT_U_0056(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0058
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0058 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0058() ;
	~CTest_MMF_AFMT_U_0058(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0065
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0065 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0065() ;
	~CTest_MMF_AFMT_U_0065(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0067
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0067 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0067() ;
	~CTest_MMF_AFMT_U_0067(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0069
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0069 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0069() ;
	~CTest_MMF_AFMT_U_0069(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0074
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0074 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0074() ;
	~CTest_MMF_AFMT_U_0074(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0075
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0075 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0075() ;
	~CTest_MMF_AFMT_U_0075(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0076
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0076 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0076() ;
	~CTest_MMF_AFMT_U_0076(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0077
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0077 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0077() ;
	~CTest_MMF_AFMT_U_0077(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0078
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0078 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0078() ;
	~CTest_MMF_AFMT_U_0078(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0079
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0079 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0079() ;
	~CTest_MMF_AFMT_U_0079(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//-----------------------------------------------------------------
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0082
 * Test NewL 
 * @test Req.REQ172.11.21
 */
class CTest_MMF_AFMT_U_0082 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0082() ;
	~CTest_MMF_AFMT_U_0082(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//-----------------------------------------------------------------
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0083
 * Test NewL 
 * @test Req.REQ172.11.21 
 */
class CTest_MMF_AFMT_U_0083 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0083() ;
	~CTest_MMF_AFMT_U_0083(){} 
	virtual TVerdict SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid);
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
protected:
		TUint   iSampleRate;
		TUint   iChannels;
		TFourCC iFourCC;
	} ;

//-----------------------------------------------------------------
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0084
 * Test NewL 
 * @test Req.REQ172.11.21
 */
class CTest_MMF_AFMT_U_0084 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0084() ;
	~CTest_MMF_AFMT_U_0084(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	TVerdict RunEmptyBufferTestL();
	void Close();
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0090
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0090 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0090() ;
	~CTest_MMF_AFMT_U_0090(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0091
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0091 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0091() ;
	~CTest_MMF_AFMT_U_0091(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0092
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0092 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0092() ;
	~CTest_MMF_AFMT_U_0092(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0093
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0093 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0093() ;
	~CTest_MMF_AFMT_U_0093(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0105
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0105 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0105() ;
	~CTest_MMF_AFMT_U_0105(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0111
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0111 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0111() ;
	~CTest_MMF_AFMT_U_0111(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0113
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0113 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0113() ;
	~CTest_MMF_AFMT_U_0113(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0114
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0114 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0114() ;
	~CTest_MMF_AFMT_U_0114(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0115
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0115 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0115() ;
	~CTest_MMF_AFMT_U_0115(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0116
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0116 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0116() ;
	~CTest_MMF_AFMT_U_0116(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0119
 * Test FillBufferL with PCM16 data.
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0119 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0119() ;
	~CTest_MMF_AFMT_U_0119(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0125
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0125 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0125() ;
	~CTest_MMF_AFMT_U_0125(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0126
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0126 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0126() ;
	~CTest_MMF_AFMT_U_0126(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0127
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0127 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0127() ;
	~CTest_MMF_AFMT_U_0127(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0128
 * Test NewL of the CMmfXXXFormatEncode object
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0128 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0128() ;
	~CTest_MMF_AFMT_U_0128(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0139
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0139 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0139() ;
	~CTest_MMF_AFMT_U_0139(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0145
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0145 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0145() ;
	~CTest_MMF_AFMT_U_0145(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0149
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0149 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0149() ;
	~CTest_MMF_AFMT_U_0149(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0150
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0150 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0150() ;
	~CTest_MMF_AFMT_U_0150(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0151
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0151 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0151() ;
	~CTest_MMF_AFMT_U_0151(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0152
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0152 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0152() ;
	~CTest_MMF_AFMT_U_0152(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0153
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0153 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0153() ;
	~CTest_MMF_AFMT_U_0153(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0154
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0154 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0154() ;
	~CTest_MMF_AFMT_U_0154(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0165
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0165 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0165() ;
	~CTest_MMF_AFMT_U_0165(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


// Removed CTest_MMF_AFMT_U_0180 - no longer required


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0189
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0189 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0189() ;
	~CTest_MMF_AFMT_U_0189(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0194
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0194 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0194() ;
	~CTest_MMF_AFMT_U_0194(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0195
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0195 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0195() ;
	~CTest_MMF_AFMT_U_0195(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0196
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0196 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0196() ;
	~CTest_MMF_AFMT_U_0196(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0197
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0197 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0197() ;
	~CTest_MMF_AFMT_U_0197(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0198
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0198 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0198() ;
	~CTest_MMF_AFMT_U_0198(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0199
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0199 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0199() ;
	~CTest_MMF_AFMT_U_0199(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;



//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0203
 * Test FillBufferL with PCM16 data.
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0203 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0203() ;
	~CTest_MMF_AFMT_U_0203(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0204
 * Test FillBufferL with ADPCM data.
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0204 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0204() ;
	~CTest_MMF_AFMT_U_0204(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0205
 * Test FillBufferL with GSM610 data.
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0205 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0205() ;
	~CTest_MMF_AFMT_U_0205(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0209
 * Test NewL 
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0209 : public CTestStep_MMF_AFMT_NewL
	{
public:
	CTest_MMF_AFMT_U_0209() ;
	~CTest_MMF_AFMT_U_0209(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0223
 * Test NewL 
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0223 : public CTestStep_MMF_AFMT_IntervalL
	{
public:
	CTest_MMF_AFMT_U_0223() ;
	~CTest_MMF_AFMT_U_0223(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0232
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0232 : public CTestStep_MMF_AFMT_DurationL
	{
public:
	CTest_MMF_AFMT_U_0232() ;
	~CTest_MMF_AFMT_U_0232(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0241
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0241 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0241() ;
	~CTest_MMF_AFMT_U_0241(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0242
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0242 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0242() ;
	~CTest_MMF_AFMT_U_0242(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0243
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0243 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0243() ;
	~CTest_MMF_AFMT_U_0243(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0244
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0244 : public CTestStep_MMF_AFMT_NumChannels
	{
public:
	CTest_MMF_AFMT_U_0244() ;
	~CTest_MMF_AFMT_U_0244(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	} ;


//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0245
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0245 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0245() ;
	~CTest_MMF_AFMT_U_0245(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
/** @xxxx
 * @class CTest_MMF_AFMT_U_0246
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0246 : public CTestStep_MMF_AFMT_SampleRate
	{
public:
	CTest_MMF_AFMT_U_0246() ;
	~CTest_MMF_AFMT_U_0246(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();	// Preamble
	void Close();		// Postamble
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0257
 * Test NewL 
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0257 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0257() ;
	~CTest_MMF_AFMT_U_0257(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0266
 * Test NewL 
 * @test Req. under test REQ172.11.21
 */
class CTest_MMF_AFMT_U_0266 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0266() ;
	~CTest_MMF_AFMT_U_0266(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0276
 * Test NewL 
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0276 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0276() ;
	~CTest_MMF_AFMT_U_0276(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0284
 * Test NewL 
 * @test Req. under test REQ172.11.22
 */
class CTest_MMF_AFMT_U_0284 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0284() ;
	~CTest_MMF_AFMT_U_0284(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0294
 * Test NewL 
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0294 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0294() ;
	~CTest_MMF_AFMT_U_0294(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

//------------------------------------------------------------------ 
 /** @xxxx
 * @class CTest_MMF_AFMT_U_0302
 * Test NewL 
 * @test Req. under test REQ172.11.23
 */
class CTest_MMF_AFMT_U_0302 : public CTestStep_MMF_AFMT_OOM
	{
public:
	CTest_MMF_AFMT_U_0302() ;
	~CTest_MMF_AFMT_U_0302(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


/** @xxxx
 * @class CTest_MMF_AFMT_U_0315
 * This test checks appending data to an existing clip which
 * has the format wav. The recording is done from a wav clip
 * The test passes if the resultant clip is of the correct size
 */
class CTest_MMF_AFMT_U_0315 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0315() ;
	~CTest_MMF_AFMT_U_0315(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


/** @xxxx
 * @class CTest_MMF_AFMT_U_0316
 * This test checks recording data to 
 */
class CTest_MMF_AFMT_U_0316 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0316() ;
	~CTest_MMF_AFMT_U_0316(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;


/** @xxxx
 * @class CTest_MMF_AFMT_U_0317
 * Test NewL 
 */
class CTest_MMF_AFMT_U_0317 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0317() ;
	~CTest_MMF_AFMT_U_0317(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

/** @xxxx
 * @class CTest_MMF_AFMT_U_0318
 * Test NewL 
 */
class CTest_MMF_AFMT_U_0318 : public CTestStep_MMF_AFMT
	{
public:
	 CTest_MMF_AFMT_U_0318();
	~CTest_MMF_AFMT_U_0318(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

/** @xxxx
 * @class CTest_MMF_AFMT_U_0319
 */
class CTest_MMF_AFMT_U_0319 : public CTestStep_MMF_AFMT_FillBufferL
	{
public:
	CTest_MMF_AFMT_U_0319() ;
	~CTest_MMF_AFMT_U_0319(){} ;
	virtual TVerdict DoTestStepL();
    virtual TVerdict SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid);

	// extra call back which allows us to set up raw info explicity
	TVerdict OpenL();
	void Close();
protected:
		TUint   iSampleRate;
		TUint   iChannels;
		TFourCC iFourCC;
	} ;

/** @xxxx
 * @class CTest_MMF_AFMT_U_
 */
class CTest_MMF_AFMT_U_0320 : public CTestStep_MMF_AFMT
	{
public:
	CTest_MMF_AFMT_U_0320() ;
	~CTest_MMF_AFMT_U_0320(){} ;
	virtual TVerdict DoTestStepL();
	TVerdict OpenL();
	void Close();
	} ;

#endif// TSU_MMF_AFMT_H__
