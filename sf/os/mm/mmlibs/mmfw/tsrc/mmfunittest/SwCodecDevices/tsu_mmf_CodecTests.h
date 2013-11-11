// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_CODECTESTS_H__
#define __TSU_MMF_CODECTESTS_H__

#include <testframework.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>
#include "TSU_MMF_ACOD.h"

const TInt KMaxMethods = 2;

class CTestSuite ;
class CTestSuite_MMF_SwCodecDevices;

//[ comparator class used to compare results in a unit test ]
class TComparator
{
public:
	virtual TBool CompareL( TUint8* aData, TUint8* aData2, TInt aLength );
};

//[Comparator class used when the results differ]
template <TInt T= -300>
class TDbComparator : public  TComparator
{
public:
	virtual TBool CompareL( TUint8* aData, TUint8* aData2, TInt aLength );
};


class TCodecUnitTestParams;
template <class T, class Comparator, TInt a=0, TInt b=0, TInt c = 0>
class CTestStepCodecUnitTest : public CTestStep_MMF_SwCodecDevices
	{
public:
	CTestStepCodecUnitTest( TUint aTestIndex );
	virtual ~CTestStepCodecUnitTest();
	virtual enum TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
public:
	// set up typedef for pointer to dispatch function
    typedef void (CTestStepCodecUnitTest::*pCallBack)(CMMFDataBuffer* &aBuffer, const TDesC& aFile );
    typedef void (CTestStepCodecUnitTest::*pPrintFormatCallBack)(TUint8* aRefCodedData, TUint8* aCodedData, TInt aDataLength);

    virtual void ReadWavFileL( CMMFDataBuffer* &aBuffer, const TDesC& aFile );
	virtual void ReadAuFileL( CMMFDataBuffer* &aBuffer, const TDesC& aFile );
    TInt ReadIntB( RFile& aFile, TInt& aValue );
	TInt ComputeBuffersToProcess();
	TInt ReadInt( RFile& aFile, TInt& aValue );
protected:
	virtual void Print8BitResults( TUint8* aRefCodedData, TUint8* aCodedData, TInt aDataLength );
	virtual void Print16BitResults( TUint8* aRefCodedData, TUint8* aCodedData, TInt aDataLength );
protected:
	const TCodecUnitTestParams *iTestParameters;
	Comparator iComparator;
	T*  iCodecUnderTest;  // The Codec Under Test
    CMMFDataBuffer* iSourceData;   // reference source data
	CMMFDataBuffer* iCodedData;    // coded data
	CMMFDataBuffer* iRefCodedData; // reference coded data
	RFs iFs;                       // file server
	pCallBack iReaders[KMaxMethods]; //[Configurable file readers]
	pPrintFormatCallBack iPrintFormats[KMaxMethods]; //[ configurable print formats ]
	};

#endif
