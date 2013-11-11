// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_SBCCODEC_H__
#define __TSU_MMF_SBCCODEC_H__

#include <testframework.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>

#include "SBCFrameParameters.h"

/**
 * @class CTestStep_MMF_SbcCodec TSU_MMF_SbcCodec.h "TSU_MMF_SbcCodec.h"
 * @brief Test step class for testing sbc codec.
 * It's a base class for all test steps.
 */
class CTestStep_MMF_SbcCodec : public CTestStep
	{
public:
	CTestStep_MMF_SbcCodec();
	virtual ~CTestStep_MMF_SbcCodec();
			
protected:
	TVerdict TestNewL(TUid aCodecUID);
	TVerdict TestNewL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC);
	TVerdict TestProcessL();
	TVerdict TestEncodeL(TInt aTestNumber, TInt aSrcBufSize, TInt aDstBufSize);
	TVerdict TestRepositionL();
	TVerdict TestChangeConfigL();

	TVerdict TestMemoryScribbleL();
	TVerdict TestMemoryLeakL();
	TVerdict TestNewLOutOfMemoryL();
	TVerdict TestProcessLOutOfMemoryL();

private:
	const TDesC& GetTestFileName(TInt aTestNumber);
	const TDesC& GetRefFileName(TInt aTestNumber);
	void ReadSourceL(const TDesC& fileName, TDes8& aSrcBuffer, TInt aPosition);
	void WriteDstToFileL(const TDesC& fileName, const TDes8& aSrcBuffer, TInt aPosition);
	TInt CompareFileL(const TDesC& aFileName);
	TVerdict ProcessOnceForNewL(CMMFCodec* aCodec);
	};


/**
 * @class CTest_MMF_ACOD_U_001
 * @test Req. under test REQ2716 
 */
class CTest_MMF_SbcCodec_U_001 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_001();
	~CTest_MMF_SbcCodec_U_001() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_002
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_002 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_002();
	~CTest_MMF_SbcCodec_U_002() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_010
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_010 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_010();
	~CTest_MMF_SbcCodec_U_010() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_011
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_011 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_011();
	~CTest_MMF_SbcCodec_U_011() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_012
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_012 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_012();
	~CTest_MMF_SbcCodec_U_012() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_013
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_013 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_013();
	~CTest_MMF_SbcCodec_U_013() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_014
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_014 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_014();
	~CTest_MMF_SbcCodec_U_014() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_015
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_015 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_015();
	~CTest_MMF_SbcCodec_U_015() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_016
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_016 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_016();
	~CTest_MMF_SbcCodec_U_016() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_021
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_021 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_021();
	~CTest_MMF_SbcCodec_U_021() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_022
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_022 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_022();
	~CTest_MMF_SbcCodec_U_022() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_023
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_023 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_023();
	~CTest_MMF_SbcCodec_U_023() {}
	virtual TVerdict DoTestStepL();
	};

/**
 * @class CTest_MMF_ACOD_U_024
 * @test Req. under test REQ2716
 */
class CTest_MMF_SbcCodec_U_024 : public CTestStep_MMF_SbcCodec
	{
public:
	CTest_MMF_SbcCodec_U_024();
	~CTest_MMF_SbcCodec_U_024() {}
	virtual TVerdict DoTestStepL();
	};


#endif //__TSU_MMF_SBCCODEC_H__

