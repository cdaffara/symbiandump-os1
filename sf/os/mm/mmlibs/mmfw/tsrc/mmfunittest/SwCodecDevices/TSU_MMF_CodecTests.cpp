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

// EPOC includes
#include <e32base.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>

// Test system includes
#include "tsu_mmf_CodecTests.h"
#include "TSU_MMF_DeviceSuite.h"

//[codec includes ]
#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include "MmfPcm16toAlawhwDevice.h"
#include "MmfALawToPcm16HwDevice.h"
#include "mmfpcm16toMulawhwdevice.h"
#include "MmfMuLawToPcm16hwDevice.h"
#include "mmfpcmS8ToPcmS16HwDevice.h"
#include "mmfpcmS16PcmS8HwDevice.h"
#include "mmfpcm16topcmU16BEHwDevice.h"
#include "mmfpcm16SwapEndianhwdevice.h"
#include "mmfpcm16ToImaAdpcm.h"
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include "MMFpcm16ToPcm16HwDevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "MMFpcmU8ToPcm16HwDevice.h"
#include "mmfpcmS16PcmS8HwDevice.h"

const TInt KFmtChunkSize = 16;
const TInt KAuMagic = 0x2e736e64;

//[ Codec Unit tests  structure 
// The unit tests shall use text files
// for small portions of test data 
// which should be stored are stored in a simple format
// containing the relevant parameters for the test
//]
class TCodecUnitTestParams
	{	
public:
	const TText* iTestName;       // name of the test
	const TText* iInputFilename;  // input wav file
	const TText* iComparisonFileName;  // output wav file
	TInt		 iExpectedResult; // expected result
	};

// constant table of parameters for tests
const TCodecUnitTestParams KTestParameters[] =
{	
	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0010-HP"),   
	_S("Pcm16Mono8khz400hzTone.wav"),
	_S("Pcm16BMono8Khz400hzTone.Au"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0011-HP"),   
	_S("Pcm16Mono8khz400hzTone.wav"),
	_S("Imaad4BitMono8Khz400hzTone.wav"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0012-HP"),   
	_S("Imaad4BitMono8Khz400hzTone.wav"),
	_S("Pcm16Mono8khz400hzTone.wav"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0014-HP"),
	_S("Pcm16Mono8khz400hzTone.wav"),
	_S("PcmU8Mono8khz400hzTone.wav"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0015-HP"),
	_S("PcmU8Mono8khz400hzTone.wav"),
	_S("Pcm16Mono8khz400hzTone.wav"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-SWCODECDEVICES-U-0016-HP"),
	_S("Pcm16Stereo8khz400hzTone.wav"),
	_S("PcmU8Stereo8khz400hzTone.wav"),
	KErrNone 
	},
};

/**
*
* Print8BitResults
* @param aRefCodedData
* @param aCodedData
* @param aDataLength
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
void CTestStepCodecUnitTest<T,Comparator, A, B, C>::Print8BitResults( TUint8* aRefCodedData, TUint8* aCodedData, TInt aDataLength )
	{
     __ASSERT_DEBUG(aRefCodedData,Panic(EBadArgument));
	 __ASSERT_DEBUG(aCodedData,Panic(EBadArgument));

	for( TInt i = 0; i < aDataLength; i++ )
		{
        INFO_PRINTF6( _L("delta %u p1 %u p2 %u p1 %x p2 %x"), (*aRefCodedData-*aCodedData),*aRefCodedData, *aCodedData, *aRefCodedData, *aCodedData );
		aRefCodedData++;
		aCodedData++;
		}
	}

/**
*
* Print16BitResults
* @param aRefCodedData
* @param aCodedData
* @param aDataLength
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
void CTestStepCodecUnitTest<T,Comparator, A, B, C>::Print16BitResults( TUint8* aRefCodedData, TUint8* aCodedData, TInt aDataLength )
	{
	//[precondition pointers are aok]
     __ASSERT_DEBUG(aRefCodedData,Panic(EBadArgument));
	 __ASSERT_DEBUG(aCodedData,Panic(EBadArgument));
	 //[precondition data is of even length ]
	 __ASSERT_DEBUG( (aDataLength % 2 == 0),Panic(EBadArgument));
	
	TInt length = aDataLength /2;
	TInt16 refCodedValue = 0;
	TInt16 codedValue = 0;
	for( TInt i = 0; i < length; i++ )
		{
		refCodedValue  = static_cast<TInt16>( aRefCodedData[0] );  
		refCodedValue |=  static_cast<TInt16>((aRefCodedData[1] << 8 ));
		codedValue  = static_cast<TInt16>( aCodedData[0] &KAndMask8bit);  
		codedValue |=  static_cast<TInt16>((aCodedData[1] << 8 ));
        INFO_PRINTF6( _L("delta %d p1 %d p2 %d p1 %x p2 %x"), (refCodedValue-codedValue),refCodedValue, codedValue, refCodedValue, codedValue );
		aRefCodedData+=2;
		aCodedData+=2;
		}
	}
/**
*
* CTestStepCodecUnitTest
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
CTestStepCodecUnitTest<T,Comparator, A, B, C>::CTestStepCodecUnitTest( TUint aTestIndex )
	{
	//[precondition valid index ]
//	__ASSERT_DEBUG( (aTestIndex >= 0),Panic(EBadArgument));	// EABI warning removal
	__ASSERT_DEBUG( (aTestIndex < (sizeof(KTestParameters)/sizeof(TCodecUnitTestParams))),Panic(EBadArgument));
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

/**
*
* ~CTestStepCodecUnitTest
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
CTestStepCodecUnitTest<T,Comparator, A, B, C>::~CTestStepCodecUnitTest()
	{
	}
/**
*
* DoTestStepL
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TVerdict CTestStepCodecUnitTest<T,Comparator, A, B, C>::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;

	// code and decode the input file
    TInt numBuffersToProcess = ComputeBuffersToProcess();
	TInt codedBufferSize = numBuffersToProcess*iCodecUnderTest->SinkBufferSize();
	iCodedData = CMMFDescriptorBuffer::NewL( codedBufferSize);
	// Compare the results and return test status
	TUint8* ptrSrc  = CONST_CAST(TUint8*,iSourceData->Data().Ptr());
	TUint8* ptrDest = CONST_CAST(TUint8*,iCodedData->Data().Ptr());
    CMMFDataBuffer* srcBuffer = CMMFDescriptorBuffer::NewL(iCodecUnderTest->SourceBufferSize());
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* destBuffer = CMMFDescriptorBuffer::NewL(iCodecUnderTest->SinkBufferSize());
	TUint8* pInBuffer  = CONST_CAST(TUint8*,srcBuffer->Data().Ptr());
    TUint8* pOutBuffer = CONST_CAST(TUint8*,destBuffer->Data().Ptr());
	CleanupStack::PushL(destBuffer);

	for( TInt i = 0; i < numBuffersToProcess;  i++)
		{
         //[ copy data & increment input pointer]
		Mem::Copy( pInBuffer, ptrSrc, iCodecUnderTest->SourceBufferSize() );
		srcBuffer->Data().SetLength( iCodecUnderTest->SourceBufferSize() );
		ptrSrc+= iCodecUnderTest->SourceBufferSize();

		 //[ code the data ]
		iCodecUnderTest->ProcessL( *srcBuffer, *destBuffer);

		 //[ copy out the data & increment pointer ]
		Mem::Copy( ptrDest, pOutBuffer, iCodecUnderTest->SinkBufferSize() );
        iCodedData->Data().SetLength( (i+1)*iCodecUnderTest->SinkBufferSize() );
		destBuffer->Data().SetLength(0); // reset buffer length
		ptrDest+=iCodecUnderTest->SinkBufferSize();
		}

	//[ compare the coded data against the reference data]
	//[ compare the processed number of bytes in the 
	// coded buffer to the reference data]
	__ASSERT_DEBUG( iRefCodedData->Data().Length() >= iCodedData->Data().Length(),Panic(EBadInvariant));
   
	TUint8* ptr1 = CONST_CAST(TUint8*,iRefCodedData->Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*,iCodedData->Data().Ptr());

	if(!iComparator.CompareL( ptr1,ptr2, iCodedData->Data().Length())!=0)
		{
		 INFO_PRINTF1( _L("Comparison has failed"));
		//(this->*iPrintFormats[ C ])( ptr1, ptr2, iCodedData->Data().Length() );
		result = EFail ;
		}

	//[ pop buffers ]
	CleanupStack::PopAndDestroy(2,srcBuffer); //srcBuffer, destBuffer
	delete iCodedData;
	iCodedData = NULL;

	__MM_HEAP_MARKEND;
	return result;
	}

/**
*
* DoTestStepPreambleL
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TVerdict CTestStepCodecUnitTest<T,Comparator,A,B, C>::DoTestStepPreambleL()
	{
	//[ assert preconditions on a and b ]
	__ASSERT_DEBUG( A >= 0, Panic(EBadInvariant)); //sanity check on data
	__ASSERT_DEBUG( B >= 0, Panic(EBadInvariant)); //sanity check on data
	__ASSERT_DEBUG( C >= 0, Panic(EBadInvariant)); //sanity check on data
	__ASSERT_DEBUG( A < 2, Panic(EBadInvariant)); //sanity check on data
	__ASSERT_DEBUG( B < 2, Panic(EBadInvariant)); //sanity check on data
	__ASSERT_DEBUG( C < 2, Panic(EBadInvariant)); //sanity check on data

	//[ initialise file reader callbacks ]
	iReaders[ 0 ] = &CTestStepCodecUnitTest<T,Comparator,A,B,C>::ReadWavFileL;
    iReaders[ 1 ] = &CTestStepCodecUnitTest<T,Comparator,A,B,C>::ReadAuFileL;
   
	//[ initialise print format callbacks ]
	iPrintFormats[ 0 ] = &CTestStepCodecUnitTest<T,Comparator,A,B,C>::Print8BitResults;
    iPrintFormats[ 1 ] = &CTestStepCodecUnitTest<T,Comparator,A,B,C>::Print16BitResults;
   
    // make codec
	iCodecUnderTest = new(ELeave) T; // a cmmfcodec 
	
	// [ Parse files into buffers, input & comparison
	//   using the configured file readers ]
	TBuf<40> testFileName = iTestParameters->iInputFilename;
	(this->*iReaders[ A ])(iSourceData, testFileName );
	testFileName = iTestParameters->iComparisonFileName;
    (this->*iReaders[ B ])(iRefCodedData,testFileName );
	return EPass;
	}
/**
*
* DoTestStepPostambleL
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TVerdict CTestStepCodecUnitTest<T,Comparator, A, B, C>::DoTestStepPostambleL(void)
	{
    //Destroy Codec
    delete iCodecUnderTest;
	delete iSourceData;  
	delete iCodedData;   
	delete iRefCodedData;
	return EPass;
	}

/**
* Reads wav file data into the supplied buffer
* ReadFileL
* @param
* @param
* This function reads the data portion of a wav file
* into a data buffer
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
void CTestStepCodecUnitTest<T,Comparator, A, B, C>::ReadWavFileL( CMMFDataBuffer* &aBuffer, const TDesC& aFile )
	{
	// connect to the file server
	User::LeaveIfError(iFs.Connect());
	
	// [open the file and read its data contents into the buffer ]
	// [the assumption will be the data is stored in wav format only]
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(aFile);

	RFile file1;
	User::LeaveIfError(file1.Open(iFs, fileName, EFileShareAny | EFileStream | EFileRead));
	CleanupClosePushL(file1);

    //[ get the size and position of the data from the wav file ]
	TInt pos  = KFmtChunkSize;
	User::LeaveIfError(file1.Seek( ESeekStart, pos ));
	TInt fmtChunkSize = 0;
	User::LeaveIfError(ReadInt(file1, fmtChunkSize));
	//[ seek to data chunk size ]
    pos = KFmtChunkSize+fmtChunkSize+8;
	User::LeaveIfError(file1.Seek(ESeekStart, pos ));    
	//read data chunk size
	TInt dataChunkSize = 0;
	User::LeaveIfError(ReadInt(file1, dataChunkSize));
    //create buffer large eneough to deal with data size
	TInt fileSize = 0;
	User::LeaveIfError(file1.Size(fileSize));
	__ASSERT_DEBUG( fileSize > dataChunkSize, Panic(EBadInvariant)); //sanity check on data
	aBuffer = CMMFDescriptorBuffer::NewL(dataChunkSize);
	User::LeaveIfError(file1.Read( aBuffer->Data(),dataChunkSize));
	aBuffer->Data().SetLength(dataChunkSize);
	file1.Close();
	CleanupStack::PopAndDestroy(1); //file1
	}

/**
* Reads Au file data into the supplied buffer
* ReadFileL
* @param
* @param
* This function reads the data portion of a wav file
* into a data buffer
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
void CTestStepCodecUnitTest<T,Comparator, A, B, C>::ReadAuFileL( CMMFDataBuffer* &aBuffer, const TDesC& aFile )
	{
	// connect to the file server
	User::LeaveIfError(iFs.Connect());
	
	// [open the file and read its data contents into the buffer ]
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(aFile);

	RFile file1;
	User::LeaveIfError(file1.Open(iFs, fileName, EFileShareAny | EFileStream | EFileRead));
	CleanupClosePushL(file1);
     
  	TInt magicNumber = 0;
	User::LeaveIfError(ReadIntB(file1, magicNumber));
    if( magicNumber != KAuMagic )
		{
         // have not detected the appropriate header
		 INFO_PRINTF1(_L("Have not detected Au Header ie magic number 0x2e736e64"));
		 User::Leave( KErrCorrupt );
		}

	//[ header size ]
    TInt headerSize = 0;
	User::LeaveIfError(ReadIntB(file1, headerSize));

	TInt fileSize = 0;
	User::LeaveIfError(file1.Size(fileSize));

    //[ filesize - headersize = datasize ]
    TInt dataSize = fileSize - headerSize;

    // [ assert that the datasize >= 0 ]
	__ASSERT_DEBUG( dataSize >= 0, Panic(EBadInvariant)); //sanity check on data

	//[Seek to the correct position and read data into Buffer]
	User::LeaveIfError(file1.Seek(ESeekStart, headerSize )); 
	__ASSERT_DEBUG( fileSize > dataSize, Panic(EBadInvariant)); //sanity check on data

	//[ read the data into a Buffer ]
	aBuffer = CMMFDescriptorBuffer::NewL(dataSize);
	User::LeaveIfError(file1.Read( aBuffer->Data(),dataSize));
	aBuffer->Data().SetLength(dataSize);
	file1.Close();
	CleanupStack::PopAndDestroy(1); //file1
	}

/**
* Reads file into buffer
* ReadFileL
* @param aFile the file from which to read the data
* @param aValue the returned value
* This function reads a 32bit value in a rather inefficient way
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TInt CTestStepCodecUnitTest<T,Comparator, A, B, C>::ReadInt( RFile& aFile, TInt& aValue )
	{
	 TUint8 data[4];
     TPtr8 theDes( data, 4 ); 
 	 User::LeaveIfError(aFile.Read( theDes, 4));
     // now assemble the data from the buffer
     aValue  = data[0];
	 aValue |= data[1] << 8;
	 aValue |= data[2] << 16;
	 aValue |= data[3] << 24;
	 
	 return KErrNone;
	}


/**
* Reads file into buffer
* ReadFileL
* @param aFile the file from which to read the data
* @param aValue the returned value
* This function reads a 32bit value in BigEndian forma
and coverts it to Little Endian in a rather inefficient way
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TInt CTestStepCodecUnitTest<T,Comparator, A, B, C>::ReadIntB( RFile& aFile, TInt& aValue )
	{
	 TUint8 data[4];
     TPtr8 theDes( data, 4 ); 
 	 User::LeaveIfError(aFile.Read( theDes, 4));
     // now assemble the data from the buffer
     aValue  = data[3];
	 aValue |= data[2] << 8;
	 aValue |= data[1] << 16;
	 aValue |= data[0] << 24;
	 
	 return KErrNone;
	}

/**
*
* ComputeBuffersToProcess
* @precondition InputBuffer has been setup
* @precondition Codec has been instantiated
*
**/
template <class T, class Comparator, TInt A, TInt B, TInt C>
TInt CTestStepCodecUnitTest<T,Comparator, A, B, C>::ComputeBuffersToProcess()
	{
   	TInt numWholeBuffers = (iSourceData->Data().MaxLength()/iCodecUnderTest->SourceBufferSize() );
	return numWholeBuffers;
    }


/**
*
* This is used for template instantiation.
*
**/

template class CTestStepCodecUnitTest<CMMFPcm16SwapEndianCodec,TComparator,0,1,1>;
template class CTestStepCodecUnitTest<CMMFPcm16ToImaAdpcmCodec,TComparator>;
template class CTestStepCodecUnitTest<CMMFImaAdpcmToPcm16Codec,TDbComparator<>,0,0,1>;
template class CTestStepCodecUnitTest<CMMFPcm16ToPcmU8Codec,TComparator>;
template class CTestStepCodecUnitTest<CMMFPcmU8ToPcm16Codec,TDbComparator<>,0,0,1>;


/**
*
* Compare
* @param aData
* @param aData2
* @param aNoSamples
* @return TBool
*
**/
TBool TComparator::CompareL( TUint8* aData, TUint8* aData2, TInt aLength )
	{
	TBool result = ETrue;
    if( !aData )
		User::Leave(KErrArgument);
	if( !aData2 )
		User::Leave(KErrArgument);

	if( Mem::Compare( aData,aLength, aData2, aLength )!=0)
		{
		result = EFalse;
		}

	return result;
	}

/**
*
* Compare
* @param aData
* @param aData2
* @param aNoSamples
* @result TBool
*
**/
template <TInt T>
TBool TDbComparator<T>::CompareL( TUint8* aData, TUint8* aData2, TInt aLength )
	{
	TBool result = ETrue;
    if( !aData )
		User::Leave(KErrArgument);
	if( !aData2 )
		User::Leave(KErrArgument);

    //[ lets compute the signal to noise ratio ]
    TReal sumSigSquared   = 0.0;
	TReal sumErrorSquared = 0.0;
	TUint8* pData = aData;
	TUint8* pData2 = aData2;
	TInt numSamples = aLength/2;
	// compute the sum of sig and error squared
	for( TInt count = 0; count < numSamples; count++ )
		{
		TInt16 sample1  = static_cast<TInt16>( pData[0] &KAndMask8bit);  
		       sample1 |=  static_cast<TInt16>((pData[1] << 8 ));
		TInt16 sample2  = static_cast<TInt16>( pData2[0] &KAndMask8bit);  
		       sample2 |=  static_cast<TInt16>((pData2[1] << 8 ));
			   pData +=1;
			   pData2+=2;
		sumSigSquared   += sample1*sample1;
		sumErrorSquared += (sample1-sample2)*(sample1-sample2);
		}

	TReal sn = 0.0;
	const TReal tolerance = 0.001;
	//[precondition error is >= tolerance ]
	if( sumErrorSquared < tolerance )
		User::Leave( KErrArgument );
	//[claculate ratio safely ]
	Math::Log( sn, (sumSigSquared/sumErrorSquared));
	sn*= 10;
	TReal threshold = T; // integer used as real with 100ths db accuracy
	      threshold /= 100.0;
	if( sn < threshold )
		result = EFalse;
	
	return result;
	}

