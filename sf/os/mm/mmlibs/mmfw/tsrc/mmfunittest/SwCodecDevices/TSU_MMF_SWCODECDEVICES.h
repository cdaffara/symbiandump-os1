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

#ifndef __TSU_MMF_SWCODECDEVICES_H__
#define __TSU_MMF_SWCODECDEVICES_H__

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

const TInt PcmToMuLawCompressionTableSize = 256;
const TInt MuLawToPcmCompressionTableSize = 256;
const TInt PcmToALawCompressionTableSize = 128;
const TInt ALawToPcmCompressionTableSize = 256;

class CTestSuite ;
class CTestSuite_MMF_SwCodecDevices;

/** 
 * @class CTest_MMF_ACOD_U_0001
 * @test Req. under test REQ172.6.3 
 */
class CTest_MMF_Gsm610 : public CTestStep_MMF_SwCodecDevices
	{
public:
	CTest_MMF_Gsm610();
	~CTest_MMF_Gsm610();
protected:
	void ReadDataL(CMMFDataBuffer*& aBuffer, const TDesC& aFile1);
	void PrintGsm610Frame(TUint8* aGsm610Frame);
	void PrintPcmDataFrame(TUint8* aPcm16Frame);
	TBool CompareEncodeResults( CMMFDataBuffer* aEncoded, CMMFDataBuffer* aRefFrame );
	TBool CompareDecodeResults(CMMFDataBuffer* aEncoded, CMMFDataBuffer* aRefFrame );
	TBool CompareGsm610Frames( TUint8* aGsmFrame,TUint8* aRefGsmFrame );
	void UnpackFrame1(struct codes* aCodeBuf, TUint8* pbuf);
	void UnpackFrame0(struct codes* aCodeBuf, TUint8* pbuf);
	void PackFrame0(struct codes* aCodeBuf, TInt8* pbuf);
	void PackFrame1(struct codes* aCodeBuf, TInt8* pbuf);
   
	TBool Compare( codes& aFrame1, codes& aFrame2 );
	void LogGsmFrames( codes& aGsmFrame, codes& aRefGsmFrame );
	void LogGsmFrame ( codes& aFrame );
	void FillPcmBuffer( CMMFDataBuffer& aSrcBuffer );
	void OpenFileL( RFile& aFile, const TDesC& aFileName );
	void ReadDataBufferL( const RFile& aFile, CMMFDataBuffer& aBuffer );
	void CloseFileL( RFile& aFile );
	void ParseFrameL( struct codes& aFrame, TUint8* &aBuffer );
	void ParseCodedDataL( CMMFDataBuffer* aBuffer );
protected:
	//[ files for conformance testing of the codec 
	// which are defined in the ETSI standard ]
	TPtrC iSourceDataFile;  // source data file name
	TPtrC iCodedDataFile;   // coded data file name
	TPtrC iDecodedDataFile; // decoded data file name
	RFile iSrcFile;         // src file 
	RFile iCodedFile;       // coded file
	RFile iDecodeFile;      // decoded file
	CMMFDataBuffer* iRefSrcData;    // reference source data
	CMMFDataBuffer* iRefCodedData; // refernce decoded data
	CMMFDataBuffer* iRefDecodedData; // refernce decoded data
	CMMFDataBuffer* iCodedData;   // buffer of coded data
	CMMFDataBuffer* iDecodedData; // buffer of actual decoded data
	CMMFDataBuffer* iScratchData; // buffer of actual decoded data
	RFs iFs;                      // file server
	RFile iFile;
	TInt iNumCodedFrames;         // number of coded frames in src buffer
	} ;

/*
*
* CTest_MMF_SwCodecDevices_U_0001
*
**/
class CTest_MMF_SwCodecDevices_U_0001: public CTest_MMF_Gsm610
{

public:
	CTest_MMF_SwCodecDevices_U_0001();
	~CTest_MMF_SwCodecDevices_U_0001();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
};


/**
*
* CTest_MMF_SwCodecDevices_U_0002
*
**/
class CTest_MMF_SwCodecDevices_U_0002: public CTest_MMF_Gsm610
{
public:
	CTest_MMF_SwCodecDevices_U_0002();
	~CTest_MMF_SwCodecDevices_U_0002();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
};

/**
*
* CTest_MMF_SwCodecDevices_U_0003
*
**/
class CTest_MMF_SwCodecDevices_U_0003: public CTest_MMF_Gsm610
{
public:
	CTest_MMF_SwCodecDevices_U_0003();
	~CTest_MMF_SwCodecDevices_U_0003();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);

};

//[ some utility functions to aid mu and alaw codec tests]
class TLawUtility
	{
	public:
      void FillSrcBufferL( TUint8* aSrcData, TInt aNoSamples, TInt16 aOffset );
	  TBool CompareCodedDataL(TUint8* aCodedData,TUint8* aRefCodedData, TInt aNoSamples );
	  TReal SNRatioL(TUint8* aDecodedData, TUint8* aSrcData, TInt aNoSamples );
	  TBool CompareSNRatiosL( TReal aCodecSN, TReal aCodecSN2, TReal aTolerance );
	  TInt16 AssembleValL(TUint8* aDecodedData);
	  TReal ComputeSNL( TReal aSumSigSquared, TReal aSumErrorSquared );
	  TReal SumSquaredL( TUint8* aData, TInt aNoSamples );
	  TReal SumErrorSquaredL( TUint8* aData, TUint8* aData2, TInt aNoSamples );
	};

//[ This Test compares the Symbian implmentation
// against another reference implementation
// of the mu law conversion codec due to problems encountered with
// gold wave coding which was different ]
class CTestMuLawCodec_U_0006: public CTestStep_MMF_SwCodecDevices
{
public:
	CTestMuLawCodec_U_0006();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
private:
	void ConvertPcmMuLawL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples );
    void ConvertMuLawPcmL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples );
	TUint8 LinearToMuLawSample(TInt16 aSample );
private:
	const static TInt8 MuLawCompressTable[PcmToMuLawCompressionTableSize];
	const static TInt16 MuLawDecompressTable[MuLawToPcmCompressionTableSize];
};

//[ This Test compares the Symbian implmentation
// against another reference implementation
// of the mu law conversion codec due to problems encountered with
// gold wave coding which was different ]
class CTestALawCodec_U_0004: public CTestStep_MMF_SwCodecDevices
{
public:
	CTestALawCodec_U_0004();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
private:
    void ConvertPcmALawL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples );
    void ConvertALawPcmL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples );
	TUint8 LinearToALawSample(TInt16 aSample);
private:
	const static TInt8  ALawCompressTable[PcmToALawCompressionTableSize];
	const static TInt16 ALawDecompressTable[ALawToPcmCompressionTableSize];
};

// This test tests the pcm16 to imaad codec
class CTestIMaadCodec: public CTestStep_MMF_SwCodecDevices
{
public:
	CTestIMaadCodec();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
private:
	// fill src buffer with linear ramp
	// for coding purposes
		void  FillSrcBufferL( CMMFDescriptorBuffer& aBuffer );
		TBool CompareResults( TReal aExpectedSNRatioDb, 
			                  CMMFDescriptorBuffer* aSrcBuffer,     
		                      CMMFDescriptorBuffer* aSinkBuffer);
};


//------------------------------------------------------------------ 

#endif// TSU_MMF_SWCODECDEVICES_H__

