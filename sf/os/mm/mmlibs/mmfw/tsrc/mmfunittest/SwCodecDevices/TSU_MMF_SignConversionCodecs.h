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

#ifndef __TSU_MMF_SIGNCONVERSIONS_H__
#define __TSU_MMF_SIGNCONVERSIONS_H__

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


class CTestSuite ;
class CTestSuite_MMF_SwCodecDevices;

/**
*
* class CMMFDataGenerator
* Provides interface for test generator
*
*/
class CMMFDataGenerator : public CBase 
                                          
	{
public:
	CMMFDataGenerator(){};
	~CMMFDataGenerator(){};
	virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )=0; 
	virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )=0;
	virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	virtual void AssembleValue( TUint8* data, TInt16& aValue, TBool bigEndian = EFalse );
    virtual void AssembleValue( TUint8* data, TUint16& aValue, TBool bigEndian = EFalse );
private:
	CTestSuite_MMF_SwCodecDevices* iRefSuite; //added to allow logging to file
	};

/**
*
* class CMMFPcm16toPcmU8Generator
* Template class to generate pcm16 data and converted pcmu8 data
*
**/
class CMMFPcm16ToPcmU8Generator: public CMMFDataGenerator
	{
public:
	CMMFPcm16ToPcmU8Generator();
	~CMMFPcm16ToPcmU8Generator();
	virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize ); 
	virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize );
	virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	};

/**
*
* class CMMFPcmU16toPcmU16BGenerator
* Template class to generate pcm16 data and converted pcmu8 data
*    
**/
class CMMFPcmU16toPcmU16BGenerator: public CMMFDataGenerator
	{
public:
	CMMFPcmU16toPcmU16BGenerator();
	~CMMFPcmU16toPcmU16BGenerator();
	virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize ); 
	virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize );
	virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	};

/**
*
* class CMMFPcmU16BtoPcm16BGenerator
* Template class to generate pcm16 data and converted pcmu8 data
*  
**/
class CMMFPcmU16BtoPcmU16Generator: public CMMFDataGenerator
	{
public:
	 CMMFPcmU16BtoPcmU16Generator();
	~CMMFPcmU16BtoPcmU16Generator();
	virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize ); 
	virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize );
	virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	};

/**
*
* class CMMFPcm8ToPcm16Generator
* Template class to generate pcm8 data and convert to pcm16 data
*
**/
class CMMFPcm8ToPcm16Generator: public CMMFDataGenerator
	{
public:
	CMMFPcm8ToPcm16Generator();
	~CMMFPcm8ToPcm16Generator();
	virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize ); 
	virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize );
	virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	};

/**
*
* class CMMFPcmU8ToPcm16Generator
*
**/
class CMMFPcmU8ToPcm16Generator : public CMMFDataGenerator
	{
public:
	 CMMFPcmU8ToPcm16Generator();
	~CMMFPcmU8ToPcm16Generator();
	 virtual void GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize ); 
	 virtual void GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize );
	 virtual void PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData );
	};

/**
* Class CTestStepSignConversionTest
* This class tests type conversions using
*  a codec T, test data generator Generator
*  and having test case name name 
**/
template <class T, class Generator, TInt index>
class CTestStepSignConversionTest : public CTestStep_MMF_SwCodecDevices
	 {
public:
	CTestStepSignConversionTest();
	virtual ~CTestStepSignConversionTest();
	virtual enum TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
private:
	TBool CompareData( CMMFDataBuffer* aData1, CMMFDataBuffer* aData2 );
private:
	T*  iCodecUnderTest;  // The Codec Under Test
	Generator* iGenerator;
    CMMFDataBuffer* iSourceData;   // reference source data
	CMMFDataBuffer* iCodedData;    // coded data
	CMMFDataBuffer* iRefCodedData; // reference coded data	
	CTestSuite_MMF_SwCodecDevices* iRefSuite;
	};

#endif
