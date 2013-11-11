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
#include "types.h"
#include "gsm610fr.h"
#include "MMFAudioCodecBase.h"

// Test system includes
#include "TSU_MMF_SWCODECDEVICES.h"
#include "TSU_MMF_SwCodecDevices_BufferSizes.h"
#include "TSU_MMF_DeviceSuite.h"
#include "MMFAudioSPcm16ToALawCodec.h"
#include "MMFAudioALawToS16PcmCodec.h"
#include "MMFAudioMuLawToS16PcmCodec.h"
#include "MMFAudioSPcm16ToMuLawCodec.h"
#include "mmfpcm16ToImaAdpcm.h"
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include "GSM610.H"

/**
*
* CTestStep_MMF_SwCodecDevices
*
**/
CTestStep_MMF_SwCodecDevices::CTestStep_MMF_SwCodecDevices()
	{
	iHeapSize = 300000;
	}

/**
*
* ~CTestStep_MMF_SwCodecDevices
*
*/
CTestStep_MMF_SwCodecDevices::~CTestStep_MMF_SwCodecDevices()
	{
	}

/**
*
* SetSuite
* @param aPtr ref to the test suite
*
**/
void CTestStep_MMF_SwCodecDevices::SetSuite( CTestSuite_MMF_SwCodecDevices* aPtr )
	{
	iRefSuite = aPtr;
	}
/**
*
* GetSuite 
* @return CTestSuite_MMF_SwCodecDevices*
*
**/
CTestSuite_MMF_SwCodecDevices* CTestStep_MMF_SwCodecDevices::GetSuite()
	{
	return iRefSuite;
	}
/**
*
* CTest_MMF_Gsm610
*
*/
CTest_MMF_Gsm610::CTest_MMF_Gsm610()
	{

	}

/**
*
* CTest_MMF_SwCodecDevices_U_0001
*
*/
CTest_MMF_SwCodecDevices_U_0001::CTest_MMF_SwCodecDevices_U_0001()
		{
		// store the name of this test case
		// this is the name that is used by the script file
		iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0001-HP");
		}

/**
*
* CTest_MMF_SwCodecDevices_U_0001
*
**/
CTest_MMF_SwCodecDevices_U_0001::~CTest_MMF_SwCodecDevices_U_0001()
		{

		}
/**
*
*
* ~CTest_MMF_Gsm610
*
*/
CTest_MMF_Gsm610::~CTest_MMF_Gsm610()
	{
     // nothing to do ..
	}

/**
*
*
* DoTestStepL
*
*/
TVerdict CTest_MMF_SwCodecDevices_U_0001::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;
	
	CMmfGsm610ToPcm16HwDevice* pDecoder = CMmfGsm610ToPcm16HwDevice::NewL();
	CleanupStack::PushL(pDecoder);
	   
	// do the same for the inverse operation
	CMmfPcm16ToGsm610HwDevice* pEncoder = CMmfPcm16ToGsm610HwDevice::NewL();
	CleanupStack::PushL(pEncoder);
	CMMFSwCodec& theEncode = pEncoder->Codec();
	
	//[ create buffers of the appropriate size ]
	const TInt srcBufferSize   = theEncode.SourceBufferSize();
	const TInt sinkBufferSize  = theEncode.SinkBufferSize();
	const TInt codedBufferSize = 76*4* KNumFramesInABuffer; // number of coded buffers * size of coded buffer 63*2 
	iNumCodedFrames =  KNumFramesInABuffer;                                //XXX claculate these constants soon <GK>
	
	iScratchData    = CMMFDescriptorBuffer::NewL( codedBufferSize);

	//[ pcm data buffers ]
	iRefSrcData     = CMMFDescriptorBuffer::NewL( srcBufferSize );      
	iDecodedData    = CMMFDescriptorBuffer::NewL( srcBufferSize ); 

	//[ coded data buffers ]
	iRefCodedData   = CMMFDescriptorBuffer::NewL( sinkBufferSize );  
	iRefDecodedData = CMMFDescriptorBuffer::NewL( sinkBufferSize );
	iCodedData      = CMMFDescriptorBuffer::NewL( sinkBufferSize );
	
	//[ for now process only the first few buffers 
	// and encode the data]
	INFO_PRINTF1(_L("Encoding Frame..."));
	TInt srcFileSize = 0;
	iSrcFile.Size( srcFileSize);
	TInt buffLen =iRefSrcData->Data().MaxLength();
	TInt numBuffers = srcFileSize/buffLen;
	if( numBuffers > 4 )  // [ not all buffers are processed without error
		// and this is the intention probably of the test sequences]
		numBuffers = 4;  
	TInt badBufferCount = 0;
	TInt goodBufferCount = 0;
	
	for( TInt bufferCount = 0; bufferCount < numBuffers; bufferCount++ )
		{
		//[precondition pointers are ok]
		ReadDataBufferL(iSrcFile, *iRefSrcData );
		
		ParseCodedDataL(iRefCodedData);
		
		CMMFSwCodec::TCodecProcessResult encodeRes = theEncode.ProcessL( *iRefSrcData, *iCodedData );
		if( encodeRes !=  CMMFSwCodec::TCodecProcessResult::EProcessComplete  )
			{
			INFO_PRINTF1( _L("Error Failed to complete coding") );
			return EFail;               
			}
		
		//[ compare results ]
		if( !CompareEncodeResults( iCodedData, iRefCodedData ) )
			{
			result = EFail;
			badBufferCount++;
			}
		else
			{
			goodBufferCount++;
			}
			
		}

	 	//[ log number of good buffers & number of bad buffers ]
	INFO_PRINTF4(_L("Good Buffers %d, Bad Buffers %d, Total Buffers %d"), 
		goodBufferCount, badBufferCount, badBufferCount+goodBufferCount );

	 //[pop data from the cleanup stack ]
	 CleanupStack::PopAndDestroy(2, pDecoder); //pDecoder, theCodec

	 delete iRefSrcData;     // reference source data
	 iRefSrcData = NULL;
	 delete iRefCodedData;   //reference coded data
	 iRefCodedData = NULL;
	 delete iRefDecodedData; // reference decoded data
	 iRefDecodedData = NULL;
	 delete iCodedData;      // buffer of coded data
	 iCodedData = NULL;
	 delete iDecodedData;    // buffer of actual decoded data
	 iDecodedData = NULL;
	 delete iScratchData;    // scratch data buffer
	 iScratchData = NULL;

	__MM_HEAP_MARKEND;
	  
    return result;
	}

/**
*
* ParseCodedDataL
*
*/
void  CTest_MMF_Gsm610::ParseCodedDataL( CMMFDataBuffer* aBuffer )
	{
   	codes frame0;
	codes frame1;

	//[ read all the coded data into the scratch buffer from reference file ]
	ReadDataBufferL( iCodedFile, *iScratchData ); 
	TUint8* dest = const_cast<TUint8*>(aBuffer->Data().Ptr());
	TUint8* src  = const_cast<TUint8*>(iScratchData->Data().Ptr());

	//[ for all the coded frames parse these frames to proper coded form ]
	for( TInt count = 0; count < iNumCodedFrames; count++ )
		{
		// parse data to frame
		ParseFrameL( frame0, src );
		ParseFrameL( frame1, src );		
		//[ finally pack the two frames into the coded data buffer ]
		PackFrame0( &frame0, reinterpret_cast<TInt8*>(dest) );
		PackFrame1( &frame1, reinterpret_cast<TInt8*>(dest) );
		dest+= KGsmFrameSize;
		}
	
	  aBuffer->Data().SetLength(KGsmFrameSize*iNumCodedFrames);
	}
/**
*
* ParseFrame
* @param aFrame this stores the gsm frame in an unpacked structure
* @param aBuffer this contains the reference file data as 76 words
*
*/
void CTest_MMF_Gsm610::ParseFrameL( struct codes& aFrame, TUint8* &aSrc )
	{
	for(TInt i = 0; i < 8; i++ )
		{
		aFrame.LARc[i]  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
		aFrame.LARc[i] |= static_cast<TInt16>(*aSrc++ << 8 );
		}
	
	for( TInt j = 0; j < 4; j++ )
		{ 
		aFrame.sfc[j].Nc  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
		aFrame.sfc[j].Nc |= static_cast<TInt16>(*aSrc++ << 8 );
		aFrame.sfc[j].bc  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
		aFrame.sfc[j].bc |= static_cast<TInt16>(*aSrc++ << 8 );
		aFrame.sfc[j].Mc  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
		aFrame.sfc[j].Mc |= static_cast<TInt16>(*aSrc++ << 8 );
		aFrame.sfc[j].xmaxc  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
		aFrame.sfc[j].xmaxc |= static_cast<TInt16>(*aSrc++ << 8 );
		
		for( TInt k = 0; k < 13; k++ )
			{
			aFrame.sfc[j].xMc[k]  = static_cast<TInt16> (*aSrc++ &KAndMask8bit );
			aFrame.sfc[j].xMc[k] |= static_cast<TInt16>(*aSrc++ << 8 );
			}
		
		}
	
	}
 
/**
*
* DoTestStepPreambleL
*
*/
enum TVerdict CTest_MMF_SwCodecDevices_U_0001::DoTestStepPreambleL(void)
	{
	TVerdict result = EPass;
    //[ connect to the file server ]
	User::LeaveIfError( iFs.Connect());

	//[ read the ref source data ]
	if(!GetStringFromConfig(_L("SectionOne"), _L("SourceData01"), iSourceDataFile) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("CodedData01"), iCodedDataFile) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("DecodedData01"), iDecodedDataFile) )
		{
		return EInconclusive;
		}

	//[ open the data files]
	OpenFileL(iSrcFile,iSourceDataFile );
	OpenFileL(iCodedFile,iCodedDataFile );
	OpenFileL(iDecodeFile,iDecodedDataFile);
	return result;
	}

/**
*
* CompareCodedResults
* @param aEncodedFrame
* @param aRefFrame
* @result TBool
*
*/
TBool CTest_MMF_Gsm610::CompareEncodeResults( CMMFDataBuffer* aEncoded, CMMFDataBuffer* aRefFrame )
	{
    TBool result = ETrue;
	
	//[precondition no encoded frames == refframes ]
	__ASSERT_DEBUG(aEncoded,Panic(EBadArgument));
	__ASSERT_DEBUG(aRefFrame,Panic(EBadArgument));

	TInt upperLimit = aEncoded->Data().Length()/KGsmFrameSize;

	const CMMFDataBuffer* encoded = STATIC_CAST(const CMMFDataBuffer*, aEncoded);
	const CMMFDataBuffer* reference = STATIC_CAST(CMMFDataBuffer*, aRefFrame);

	TUint8* pFrames    = CONST_CAST(TUint8*,encoded->Data().Ptr());
	TUint8* pRefFrames = CONST_CAST(TUint8*,reference->Data().Ptr());

	TInt badFrameCount = 0;
	TInt goodFrameCount = 0;

    //[ for all frames ]
    for( TInt index = 0; index < upperLimit; index++ )
		{
	 //[ compare src and ref frame]
	   if( !CompareGsm610Frames( pFrames, pRefFrames ))
		   {
		   result = EFalse;
		   badFrameCount++;
		   INFO_PRINTF2(_L("Bad Frame Number: %d"), index );
		   }
	   else{
		   goodFrameCount++;
		   }

	    //[ increment frame pointers by size of gsmencoded frame  
	    pFrames    += KGsmFrameSize;
		pRefFrames += KGsmFrameSize;
		}

		 	//[ log number of good frames & number of bad frames ]
	INFO_PRINTF4(_L("Good Frames %d, Bad Frames %d, Total Frames %d"), 
		goodFrameCount, badFrameCount, badFrameCount+goodFrameCount );

	 return result ;
	}

/**
*
* CompareGsm610Frames
* This function compares two encoded gsm610 frames
* @result TBool Frames are the same or different
*
**/
TBool CTest_MMF_Gsm610::CompareGsm610Frames( TUint8* aGsmFrame,TUint8* aRefGsmFrame )
	{
	 TBool result = ETrue;
	 ASSERT( aGsmFrame );
	 __ASSERT_DEBUG(aGsmFrame,Panic(EBadArgument));
	 __ASSERT_DEBUG(aRefGsmFrame,Panic(EBadArgument));
	 
	 codes codeBuf0;
	 codes codeBuf1;
	 codes refCodeBuf0;
	 codes refCodeBuf1; 

	 UnpackFrame0(&codeBuf0, aGsmFrame );
	 UnpackFrame1(&codeBuf1, aGsmFrame );
	 UnpackFrame0(&refCodeBuf0, aRefGsmFrame );
	 UnpackFrame1(&refCodeBuf1, aRefGsmFrame );
    
	if( !Compare( codeBuf0, refCodeBuf0 ) ||
		!Compare( codeBuf1, refCodeBuf1) )

		{
		 //LogGsmFrames( codeBuf0, refCodeBuf0 ); // add for debug reasons
		 //LogGsmFrames( codeBuf1, refCodeBuf1 ); // add for debugging reasons
		result = EFalse;
		}

	 return result ;
	}

/**
*
* LogGsmFrames
* @param aGsmFrame
* @param aRefGsmFrame 
*
**/
void  CTest_MMF_Gsm610::LogGsmFrames( codes& aGsmFrame, codes& aRefGsmFrame )
	{
    //[ print the quantized lar coefficients ]
   	INFO_PRINTF1(_L("Coded Frame:")); 
	LogGsmFrame( aGsmFrame );
	INFO_PRINTF1(_L("Reference Frame:")); 
	LogGsmFrame( aRefGsmFrame );
	}

/**
*
* LogGsmFrame
* prints a GsmFrame to the test log
* @param aFrame 
*
**/
void  CTest_MMF_Gsm610::LogGsmFrame ( codes& aFrame )
	{
	INFO_PRINTF2(_L("LARc[0] = %d"), aFrame.LARc[0]); 
	INFO_PRINTF2(_L("LARc[1] = %d"), aFrame.LARc[1]);
	INFO_PRINTF2(_L("LARc[2] = %d"), aFrame.LARc[2]);
	INFO_PRINTF2(_L("LARc[3] = %d"), aFrame.LARc[3]);
	INFO_PRINTF2(_L("LARc[4] = %d"), aFrame.LARc[4]); 
	INFO_PRINTF2(_L("LARc[5] = %d"), aFrame.LARc[5]);
	INFO_PRINTF2(_L("LARc[6] = %d"), aFrame.LARc[6]);
	INFO_PRINTF2(_L("LARc[7] = %d"), aFrame.LARc[7]);

	//[ for each sub frame print its data ]
	for( TInt i = 0; i < 4; i++ )
		{
         INFO_PRINTF2(_L("Nc = %d"), aFrame.sfc[i].Nc); 
		 INFO_PRINTF2(_L("bc = %d"), aFrame.sfc[i].bc);
		 INFO_PRINTF2(_L("Mc= %d"), aFrame.sfc[i].Mc);
		 INFO_PRINTF2(_L("xmaxc = %d"), aFrame.sfc[i].xmaxc);
		 INFO_PRINTF2(_L("xMc[0] = %d"), aFrame.sfc[i].xMc[0]);
		 INFO_PRINTF2(_L("xMc[1] = %d"), aFrame.sfc[i].xMc[1]);
		 INFO_PRINTF2(_L("xMc[2] = %d"), aFrame.sfc[i].xMc[2]);
		 INFO_PRINTF2(_L("xMc[3] = %d"), aFrame.sfc[i].xMc[3]);
		 INFO_PRINTF2(_L("xMc[4] = %d"), aFrame.sfc[i].xMc[4]);
		 INFO_PRINTF2(_L("xMc[5] = %d"), aFrame.sfc[i].xMc[5]);
		 INFO_PRINTF2(_L("xMc[6] = %d"), aFrame.sfc[i].xMc[6]);
		 INFO_PRINTF2(_L("xMc[7] = %d"), aFrame.sfc[i].xMc[7]);
		 INFO_PRINTF2(_L("xMc[8] = %d"), aFrame.sfc[i].xMc[8]);
		 INFO_PRINTF2(_L("xMc[9] = %d"), aFrame.sfc[i].xMc[9]);
		 INFO_PRINTF2(_L("xMc[10] = %d"), aFrame.sfc[i].xMc[10]);
		 INFO_PRINTF2(_L("xMc[11] = %d"), aFrame.sfc[i].xMc[11]);
		 INFO_PRINTF2(_L("xMc[12] = %d"), aFrame.sfc[i].xMc[12]);
		}
	}

/**
*
* Compare
*
**/
TBool CTest_MMF_Gsm610::Compare( codes& aFrame1, codes& aFrame2 )
	{
     TBool result = ETrue;
     
	if( (aFrame1.LARc[0] != aFrame2.LARc[0] ) ||
	    (aFrame1.LARc[1] != aFrame2.LARc[1] ) ||
	    (aFrame1.LARc[2] != aFrame2.LARc[2] ) ||
	    (aFrame1.LARc[3] != aFrame2.LARc[3] ) ||
	    (aFrame1.LARc[4] != aFrame2.LARc[4] ) ||
	    (aFrame1.LARc[5] != aFrame2.LARc[5] ) ||
	    (aFrame1.LARc[6] != aFrame2.LARc[6] ) ||
	    (aFrame1.LARc[7] != aFrame2.LARc[7] ) )
		{
		result = EFalse;
		}

	for( TInt i = 0; i < 4; i++ )
		{
		if( 
         (aFrame1.sfc[i].Nc !=  aFrame2.sfc[i].Nc) ||
		 (aFrame1.sfc[i].bc !=  aFrame2.sfc[i].bc) ||
		 (aFrame1.sfc[i].Mc !=  aFrame2.sfc[i].Mc) ||
		 (aFrame1.sfc[i].xmaxc !=	 aFrame2.sfc[i].xmaxc) ||
		 (aFrame1.sfc[i].xMc[0] != aFrame2.sfc[i].xMc[0]) ||
		 (aFrame1.sfc[i].xMc[1] !=	aFrame2.sfc[i].xMc[1]) ||
		 (aFrame1.sfc[i].xMc[2] !=	aFrame2.sfc[i].xMc[2]) ||
		 (aFrame1.sfc[i].xMc[3] !=	aFrame2.sfc[i].xMc[3]) ||
		 (aFrame1.sfc[i].xMc[4] !=	aFrame2.sfc[i].xMc[4]) ||
		 (aFrame1.sfc[i].xMc[5] !=	aFrame2.sfc[i].xMc[5]) ||
		 (aFrame1.sfc[i].xMc[6] !=	aFrame2.sfc[i].xMc[6]) ||
		 (aFrame1.sfc[i].xMc[7] !=	aFrame2.sfc[i].xMc[7]) ||
		 (aFrame1.sfc[i].xMc[8] !=	aFrame2.sfc[i].xMc[8]) ||
		 (aFrame1.sfc[i].xMc[9] !=	aFrame2.sfc[i].xMc[9]) ||
		 (aFrame1.sfc[i].xMc[10] !=	aFrame2.sfc[i].xMc[10]) ||
		 (aFrame1.sfc[i].xMc[11] !=	aFrame2.sfc[i].xMc[11]) ||
		 (aFrame1.sfc[i].xMc[12] !=	aFrame2.sfc[i].xMc[12]))
			{
			result = EFail;
			}
		}

	 return result;
	}

/**
*
* CompareCodedResults
*
*/
TBool CTest_MMF_Gsm610::CompareDecodeResults(CMMFDataBuffer* aEncoded, CMMFDataBuffer* aRefFrame )
	{
    TBool result = ETrue;
	
    //[ precondition the buffers are of the same length ]
	__ASSERT_DEBUG(aEncoded,Panic(EBadArgument));
	__ASSERT_DEBUG(aRefFrame,Panic(EBadArgument));
	__ASSERT_DEBUG(aEncoded->Data().MaxLength() == aRefFrame->Data().MaxLength(),Panic(EBadArgument));
	
	TUint8 *pResults = CONST_CAST(TUint8*,aEncoded->Data().Ptr());  
	TUint8 *pRefData = CONST_CAST(TUint8*,aRefFrame->Data().Ptr());
	TInt numResults  = aEncoded->Data().MaxLength(); 

    if (Mem::Compare( pResults,numResults,pRefData,numResults)!=0)
        {
		result = EFalse;
		}
  
    return result ;
	}
/**
*
* ReadDataL
* Reads entire data file into buffer 
*
**/
void CTest_MMF_Gsm610::ReadDataL( CMMFDataBuffer*& aBuffer, const TDesC& aFile1 )
	{
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(aFile1);

	RFile file1;
	User::LeaveIfError(file1.Open(iFs, fileName, EFileShareAny | EFileStream | EFileRead));
	CleanupClosePushL(file1);
	TInt fileSize = 0;
	User::LeaveIfError(file1.Size(fileSize));
	aBuffer = CMMFDescriptorBuffer::NewL(fileSize);
	User::LeaveIfError(file1.Read( aBuffer->Data(),fileSize));
	CleanupStack::PopAndDestroy(1); //file1
	}

/**
*
* OpenFileL
* @param aFile
* @param aFileName
*
**/
void CTest_MMF_Gsm610::OpenFileL( RFile& aFile, const TDesC& aFileName )
	{
	User::LeaveIfError(aFile.Open(iFs, aFileName, EFileShareAny | EFileStream | EFileRead));
	}

/**
*
* CloseFileL
* @param aFile
*
**/
void CTest_MMF_Gsm610::CloseFileL( RFile& aFile )
	{
	aFile.Close();
	}

/**
*
* ReadDataBuffer
* @param aFile
* assumes file reads sufficient data
*
**/
void CTest_MMF_Gsm610::ReadDataBufferL( const RFile& aFile, CMMFDataBuffer& aBuffer )
	{
	//[ The read will set the length of the descriptor to the number of bytes read]
	User::LeaveIfError(aFile.Read( aBuffer.Data(),aBuffer.Data().MaxLength() ));
	INFO_PRINTF2(_L("Bytes read = %d"), aBuffer.Data().Length() );
   	}

/**
*
* FillPcmBuffer
* @param aSrcBuffer
* Fill a buffer with a sine wave
*
**/
void CTest_MMF_Gsm610::FillPcmBuffer( CMMFDataBuffer& aSrcBuffer )
	{
	//fill the Src Buffer
	 TUint8* pDst = CONST_CAST(TUint8*,aSrcBuffer.Data().Ptr());
	 TInt length =  aSrcBuffer.Data().MaxLength();

	 //[encode the data]
	 TInt16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 for(TInt i=0; i<length/2; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = KPi*i/5; // fundamental of 800hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TInt16>( 1000 * val );
	      *pDst++ = static_cast<TUint8>( srcValue & KAndMask8bit);
		  *pDst++ = static_cast<TUint8>((srcValue >> 8) & KAndMask8bit );
		  //INFO_PRINTF2(_L("Sine = %d"), srcValue );  //uncomment for debugging purposes     
		 }
    
	 aSrcBuffer.Data().SetLength(length);

	}

/**
*
* DoTestStepPostambleL
* @result TVerdict
* 
*/
TVerdict CTest_MMF_SwCodecDevices_U_0001::DoTestStepPostambleL(void)
	{
	// close files
	CloseFileL( iSrcFile );
	CloseFileL( iCodedFile );
	CloseFileL( iDecodeFile );
    //[ clean up the buffers etc ]
	delete iRefSrcData;     // reference source data
	delete iRefCodedData;   //reference coded data
	delete iRefDecodedData; // reference decoded data
	delete iCodedData;      // buffer of coded data
	delete iDecodedData;    // buffer of actual decoded data
	delete iScratchData;    // scratch data buffer
	return EPass;
	}

/**
*
* UnpackFrame0
* @param aCodeBuf 
* @param pbuf
*
**/
void CTest_MMF_Gsm610::UnpackFrame0(codes* aCodeBuf,  TUint8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;

    // unpack the LAR[0..7] from the first 4.5 bytes
    LAR[0] = (TInt16)((pbuf[0] & 0x3F));
    LAR[1] = (TInt16)(((pbuf[0] & 0xC0) >> 6) | ((pbuf[1] & 0x0F) << 2));
    LAR[2] = (TInt16)(((pbuf[1] & 0xF0) >> 4) | ((pbuf[2] & 0x01) << 4));
    LAR[3] = (TInt16)(((pbuf[2] & 0x3E) >> 1));
    LAR[4] = (TInt16)(((pbuf[2] & 0xC0) >> 6) | ((pbuf[3] & 0x03) << 2));
    LAR[5] = (TInt16)(((pbuf[3] & 0x3C) >> 2));
    LAR[6] = (TInt16)(((pbuf[3] & 0xC0) >> 6) | ((pbuf[4] & 0x01) << 2));
    LAR[7] = (TInt16)(((pbuf[4] & 0x0E) >> 1));

    // unpack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
#define sfb(x) (pbuf[4+i*7+x])
        c.Nc = (TInt16)(((sfb(0) & 0xF0) >> 4) | ((sfb(1) & 0x07) << 4));
        c.bc = (TInt16)(((sfb(1) & 0x18) >> 3));
        c.Mc = (TInt16)(((sfb(1) & 0x60) >> 5));
        c.xmaxc = (TInt16)(((sfb(1) & 0x80) >> 7) | ((sfb(2) & 0x1F) << 1));
        c.xMc[0] = (TInt16)(((sfb(2) & 0xE0) >> 5));
        c.xMc[1] = (TInt16)((sfb(3) & 0x07));
        c.xMc[2] = (TInt16)(((sfb(3) & 0x3C) >> 3));
        c.xMc[3] = (TInt16)(((sfb(3) & 0xC0) >> 6) | ((sfb(4) & 0x01) << 2));
        c.xMc[4] = (TInt16)(((sfb(4) & 0x0E) >> 1));
        c.xMc[5] = (TInt16)(((sfb(4) & 0x70) >> 4));
        c.xMc[6] = (TInt16)(((sfb(4) & 0x80) >> 7) | ((sfb(5) & 0x03) << 1));
        c.xMc[7] = (TInt16)(((sfb(5) & 0x1C) >> 2));
        c.xMc[8] = (TInt16)(((sfb(5) & 0xE0) >> 5));
        c.xMc[9] = (TInt16)((sfb(6) & 0x07));
        c.xMc[10] = (TInt16)(((sfb(6) & 0x38) >> 3));
        c.xMc[11] = (TInt16)(((sfb(6) & 0xC0) >> 6) | ((sfb(7) & 0x01) << 2));
        c.xMc[12] = (TInt16)(((sfb(7) & 0x0E) >> 1));
#undef sfb
        }
    }

/**
*
* UnpackFrame1
* @param aCodeBuf 
* @param pbuf
*
**/
void CTest_MMF_Gsm610::UnpackFrame1(struct codes* aCodeBuf, TUint8* pbuf)
{
    TInt16* LAR = aCodeBuf->LARc;

    // unpack the LAR[0..7] from the first 4.5 bytes
    LAR[0] = (TInt16)(((pbuf[32] & 0xF0) >> 4) | ((pbuf[33] & 0x03) << 4));
    LAR[1] = (TInt16)(((pbuf[33] & 0xFC) >> 2));
    LAR[2] = (TInt16)(((pbuf[34] & 0x1F)));
    LAR[3] = (TInt16)(((pbuf[34] & 0xE0) >> 5) | ((pbuf[35] & 0x03) << 3));
    LAR[4] = (TInt16)(((pbuf[35] & 0x3C) >> 2));
    LAR[5] = (TInt16)(((pbuf[35] & 0xC0) >> 6) | ((pbuf[36] & 0x03) << 2));
    LAR[6] = (TInt16)(((pbuf[36] & 0x1C) >> 2));
    LAR[7] = (TInt16)(((pbuf[36] & 0xE0) >> 5));

    // unpack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
#define sfb(x) (pbuf[37+i*7+x])
        c.Nc = (TInt16)(sfb(0) & 0x7F);
        c.bc = (TInt16)(((sfb(0) & 0x80) >> 7) | ((sfb(1) & 0x01) << 1));
        c.Mc = (TInt16)(((sfb(1) & 0x06) >> 1));
        c.xmaxc = (TInt16)(((sfb(1) & 0xF8) >> 3) | ((sfb(2) & 0x01) << 5));
        c.xMc[0] = (TInt16)(((sfb(2) & 0x0E) >> 1));
        c.xMc[1] = (TInt16)(((sfb(2) & 0x70) >> 4));
        c.xMc[2] = (TInt16)(((sfb(2) & 0x80) >> 7) | ((sfb(3) & 0x03) << 1));
        c.xMc[3] = (TInt16)(((sfb(3) & 0x1C) >> 2));
        c.xMc[4] = (TInt16)(((sfb(3) & 0xE0) >> 5));
        c.xMc[5] = (TInt16)(((sfb(4) & 0x07)));
        c.xMc[6] = (TInt16)(((sfb(4) & 0x38) >> 3));
        c.xMc[7] = (TInt16)(((sfb(4) & 0xC0) >> 6) | ((sfb(5) & 0x01) << 2));
        c.xMc[8] = (TInt16)(((sfb(5) & 0x0E) >> 1));
        c.xMc[9] = (TInt16)(((sfb(5) & 0x70) >> 4));
        c.xMc[10] = (TInt16)(((sfb(5) & 0x80) >> 7) | ((sfb(6) & 0x03) << 1));
        c.xMc[11] = (TInt16)(((sfb(6) & 0x1C) >> 2));
        c.xMc[12] = (TInt16)(((sfb(6) & 0xE0) >> 5));

#undef sfb
        }
    }

/**
*
* PackFrame0 
* Pack the codewords of the even frame into pack buffer.
* Packing as in MS gsm610 encoder.
* @param aCodeBuf  Code words for one speech frame.
* @param pbuf the output buffer
*
**/
void CTest_MMF_Gsm610::PackFrame0(struct codes* aCodeBuf, TInt8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;
    
    // pack the LARc[0..7] into the first 4.5 bytes
    *pbuf++ = (TUint8)(((LAR[0]     ) & 0x3F) | ((LAR[1] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[1] >> 2) & 0x0F) | ((LAR[2] << 4) & 0xF0));
    *pbuf++ = (TUint8)(((LAR[2] >> 4) & 0x01) | ((LAR[3] << 1) & 0x3E) | ((LAR[4] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[4] >> 2) & 0x03) | ((LAR[5] << 2) & 0x3C) | ((LAR[6] << 6) & 0xC0));
    *pbuf   = (TUint8)(((LAR[6] >> 2) & 0x01) | ((LAR[7] << 1) & 0x0E));
    
    // pack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
        *pbuf++ |= ((c.Nc << 4) & 0xF0);
        *pbuf++ = (TUint8)(((c.Nc >> 4) & 0x07) | ((c.bc << 3) & 0x18) | ((c.Mc << 5) & 0x60) | ((c.xmaxc << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xmaxc >> 1) & 0x1F) | ((c.xMc[0] << 5) & 0xE0));
        *pbuf++ = (TUint8)((c.xMc[1] & 0x07) | ((c.xMc[2] << 3) & 0x38) | ((c.xMc[3] << 6) & 0xC0));
        *pbuf++ = (TUint8)(((c.xMc[3] >> 2) & 0x01) | ((c.xMc[4] << 1) & 0x0E) | ((c.xMc[5] << 4) & 0x70) | ((c.xMc[6] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[6] >> 1) & 0x03) | ((c.xMc[7] << 2) & 0x1C) | ((c.xMc[8] << 5) & 0xE0));
        *pbuf++ = (TUint8)((c.xMc[9] & 0x07) | ((c.xMc[10] << 3) & 0x38) | ((c.xMc[11] << 6) & 0xC0));
        *pbuf   = (TUint8)(((c.xMc[11] >> 2) & 0x01) | ((c.xMc[12] << 1) & 0x0E));
        }
    }

/**
*
* PackFrame1 
* Pack the codewords of the even frame into pack buffer.
* Packing as in MS gsm610 encoder.
* @param aCodeBuf  Code words for one speech frame.
* @param pbuf the output buffer
*
**/
void CTest_MMF_Gsm610::PackFrame1(struct codes* aCodeBuf, TInt8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;
    
	pbuf += (PACKSIZE / 2);

    // pack the LARc[0..7] into the first 4.5 bytes, starting with the msb of the first byte
    *pbuf++ = (TUint8) (pbuf[0] | ((LAR[0] << 4) & 0xF0));
    *pbuf++ = (TUint8)(((LAR[0] >> 4) & 0x03) | ((LAR[1] << 2) & 0xFC));
    *pbuf++ = (TUint8)(((LAR[2]     ) & 0x1F) | ((LAR[3] << 5) & 0xE0));
    *pbuf++ = (TUint8)(((LAR[3] >> 3) & 0x03) | ((LAR[4] << 2) & 0x3C) | ((LAR[5] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[5] >> 2) & 0x03) | ((LAR[6] << 2) & 0x1C) | ((LAR[7] << 5) & 0xE0));
    
    // pack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
        *pbuf++ = (TUint8)((c.Nc & 0x7F) | ((c.bc << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.bc >> 1) & 0x01) | ((c.Mc << 1) & 0x06) | ((c.xmaxc << 3) & 0xF8));
        *pbuf++ = (TUint8)(((c.xmaxc >> 5) & 0x01) | ((c.xMc[0] << 1) & 0x0E) | ((c.xMc[1] << 4) & 0x70) | ((c.xMc[2] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[2] >> 1) & 0x03) | ((c.xMc[3] << 2) & 0x1C) | ((c.xMc[4] << 5) & 0xE0));
        *pbuf++ = (TUint8)(((c.xMc[5]) & 0x07) | ((c.xMc[6] << 3) & 0x38) | ((c.xMc[7] << 6) & 0xC0));
        *pbuf++ = (TUint8)(((c.xMc[7] >> 2) & 0x01) | ((c.xMc[8] << 1) & 0x0E) | ((c.xMc[9] << 4) & 0x70) | ((c.xMc[10] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[10] >> 1) & 0x03) | ((c.xMc[11] << 2) & 0x1C) | ((c.xMc[12] << 5) & 0xE0));
        }
    }

/**
*
* CTest_MMF_SwCodecDevices_U_0001
*
*/
CTest_MMF_SwCodecDevices_U_0002::CTest_MMF_SwCodecDevices_U_0002()
		{
		// store the name of this test case
		// this is the name that is used by the script file
		iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0002-HP");
		}
/**
*
* CTest_MMF_SwCodecDevices_U_0001
*
**/
CTest_MMF_SwCodecDevices_U_0002::~CTest_MMF_SwCodecDevices_U_0002()
		{

		}

/**
*
*
* DoTestStepL
*
**/
TVerdict CTest_MMF_SwCodecDevices_U_0002::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;
	
	 CMmfGsm610ToPcm16HwDevice* pDecoder = CMmfGsm610ToPcm16HwDevice::NewL();
     CleanupStack::PushL(pDecoder);
	 //[ note this reference should be a ptr ]
     CMMFSwCodec& theCodec = pDecoder->Codec();
 
	 //[ create buffers of the appropriate size ]
	 const TInt srcBufferSize  = theCodec.SourceBufferSize();
	 const TInt sinkBufferSize = theCodec.SinkBufferSize();
	 const TInt codedBufferSize = 76*4* KNumFramesInABuffer; // number of coded buffers * size of coded buffer 63*2 
	 iNumCodedFrames =  KNumFramesInABuffer;                                //XXX claculate these constants soon <GK>
	
	 iScratchData    = CMMFDescriptorBuffer::NewL( codedBufferSize);

	 iRefCodedData   = CMMFDescriptorBuffer::NewL( srcBufferSize );  
	 iCodedData      = CMMFDescriptorBuffer::NewL( srcBufferSize );
	 iRefDecodedData = CMMFDescriptorBuffer::NewL( sinkBufferSize );
	 iDecodedData    = CMMFDescriptorBuffer::NewL( sinkBufferSize); 
	 
	 //[ for now process only the first buffer ]
	 //[encode the data]
	 INFO_PRINTF1(_L("Decoding Frames..."));
	 TInt codedFileSize = 0;
	 iCodedFile.Size( codedFileSize);
	
     TInt numBuffers = codedFileSize/srcBufferSize;
	 if(numBuffers > 4 )
		 numBuffers = 4;
	 TInt badBufferCount = 0;
	 TInt goodBufferCount = 0;
	 for( TInt bufferCount = 0; bufferCount < numBuffers; bufferCount++ )
		 {
		 ReadDataBufferL(iDecodeFile, *iRefDecodedData );
		 ParseCodedDataL(iCodedData);
 
		 CMMFSwCodec::TCodecProcessResult decodeRes = theCodec.ProcessL( *iCodedData, *iDecodedData );
		 if( decodeRes !=  CMMFSwCodec::TCodecProcessResult::EProcessComplete  )
			 {
			 INFO_PRINTF1( _L("Error Failed to complete decoding") );
			 return EFail;               
			 }
	
		 //[ compare results ]
		 if(!CompareDecodeResults( iDecodedData, iRefDecodedData ))
			 {
			 result = EFail;
		     badBufferCount++;
			 }
		 else
			 {
			 goodBufferCount++;
			 }
		
		 }

	 	//[ log number of good frames & number of bad frames ]
	INFO_PRINTF4(_L("Good Frames %d, Bad Frames %d, Total Frames %d"), 
		goodBufferCount, badBufferCount, badBufferCount+goodBufferCount );

	 //[pop data from the cleanup stack ]
	 CleanupStack::PopAndDestroy(pDecoder); 

	 delete iRefCodedData;
	 iRefCodedData = NULL;
	 delete iRefDecodedData;
	 iRefDecodedData = NULL;
	 delete iDecodedData; 
	 iDecodedData = NULL;
	 delete iCodedData ;
	 iCodedData = NULL;
	 delete iScratchData;
	 iScratchData = NULL;

	__MM_HEAP_MARKEND;                         
    return result;
	}

/**
*
* DoTestStepPreambleL
*
*/
enum TVerdict CTest_MMF_SwCodecDevices_U_0002::DoTestStepPreambleL(void)
	{
	TVerdict result = EPass;
    //[ connect to the file server ]
	User::LeaveIfError( iFs.Connect());

	//[ read the ref source data ]
	if(!GetStringFromConfig(_L("SectionOne"), _L("SourceData01"), iSourceDataFile) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("CodedData01"), iCodedDataFile) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("DecodedData01"), iDecodedDataFile) )
		{
		return EInconclusive;
		}

	//[ open the data files]
	OpenFileL(iSrcFile,iSourceDataFile );
	OpenFileL(iCodedFile,iCodedDataFile );
	OpenFileL(iDecodeFile,iDecodedDataFile);

	iScratchData = CMMFDescriptorBuffer::NewL(KCodedBufferSize);

	return result;
	}

/**
*
* DoTestStepPostambleL
* @result TVerdict
* 
*/
TVerdict CTest_MMF_SwCodecDevices_U_0002::DoTestStepPostambleL(void)
	{
	// close files
	CloseFileL( iCodedFile );
	CloseFileL( iDecodeFile );
    //[ clean up the buffers etc ]
	delete iRefSrcData;     // reference source data
	delete iRefCodedData;   //reference coded data
	delete iRefDecodedData; // refernce decoded data
	delete iCodedData;      // buffer of coded data
	delete iDecodedData;    // buffer of actual decoded data
	delete iScratchData;    // scratch data buffer
	return EPass;
	}

/**
*
* CTest_MMF_SwCodecDevices_U_0003
*
**/
CTest_MMF_SwCodecDevices_U_0003::CTest_MMF_SwCodecDevices_U_0003()
		{
		// store the name of this test case
		// this is the name that is used by the script file
		iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0003-HP");
		}
/**
*
* ~CTest_MMF_SwCodecDevices_U_0003
*
**/
CTest_MMF_SwCodecDevices_U_0003::~CTest_MMF_SwCodecDevices_U_0003()
	{

	}

/**
*
* DoTestStepL
*
**/
TVerdict CTest_MMF_SwCodecDevices_U_0003::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;
	
	 CMmfGsm610ToPcm16HwDevice* pDecoder = CMmfGsm610ToPcm16HwDevice::NewL();
     CleanupStack::PushL(pDecoder);
	 //[ note this reference should be a ptr ]
     CMMFSwCodec& theCodec = pDecoder->Codec();
 
	 // do the same for the inverse operation
	 CMmfPcm16ToGsm610HwDevice* pEncoder = CMmfPcm16ToGsm610HwDevice::NewL();
     CleanupStack::PushL(pEncoder);
     CMMFSwCodec& theEncode = pEncoder->Codec();

	 //[ create buffers of the appropriate size ]
	 const TInt srcBufferSize = theEncode.SourceBufferSize();
	 const TInt sinkBufferSize = theEncode.SinkBufferSize();

	 iRefSrcData     = CMMFDescriptorBuffer::NewL( srcBufferSize );   
	 iRefCodedData   = CMMFDescriptorBuffer::NewL( sinkBufferSize );  
	 iRefDecodedData = CMMFDescriptorBuffer::NewL( srcBufferSize );
	 iCodedData      = CMMFDescriptorBuffer::NewL( sinkBufferSize );   
	 iDecodedData    = CMMFDescriptorBuffer::NewL( srcBufferSize ); 
	 
     //[ now get a sine wave of 800hz, code and decode and
	 // compare the results ]
	 FillPcmBuffer( *iRefSrcData );

	 // encode
	CMMFSwCodec::TCodecProcessResult encodeRes = theEncode.ProcessL( *iRefSrcData, *iCodedData );
	 if( encodeRes !=  CMMFSwCodec::TCodecProcessResult::EProcessComplete  )
		 {
		 INFO_PRINTF1( _L("Error Failed to complete coding") );
		 return EFail;               
		 }

	 //decode
	 CMMFSwCodec::TCodecProcessResult decodeRes = theCodec.ProcessL( *iCodedData, *iDecodedData );
	 if( decodeRes !=  CMMFSwCodec::TCodecProcessResult::EProcessComplete  )
		 {
		 INFO_PRINTF1( _L("Error Failed to complete decoding") );
		 return EFail;               
		 }

	 //[ because the codec overwrites its input regenerate it ]
	 FillPcmBuffer( *iRefSrcData );

     //[ NOW COMPARE THE RESULTS DISTORTION < 18 DB ]
	 
	 TUint8 *pResults = (TUint8*)(iDecodedData->Data().Ptr());  
	 TUint8 *pRefData = (TUint8*)(iRefSrcData->Data().Ptr());
	 TInt numResults  = iDecodedData->Data().MaxLength(); 
	 TReal sum1 = 0.0;
	 TReal sum2 = 0.0;
	 TReal ratio = 0.0;
	 TInt16 temp1 = 0;
	 TInt16 temp2 = 0;
	 numResults /= 2; // compensate for bytes to short conversion
	 //[print the results to allow analysis]
	 for( TInt index = 0; index < numResults; index++ )
		 {
		 temp1  = static_cast<TInt16>((*pResults++) &KAndMask8bit);
		 temp1 |= static_cast<TInt16>((*pResults++ << 8));
		 sum1 += temp1*temp1;
		 temp2  = static_cast<TInt16>((*pRefData++) &KAndMask8bit);
		 temp2 |= static_cast<TInt16>((*pRefData++ << 8));
		 sum2 += (temp2-temp1)*(temp2-temp1);
		 //INFO_PRINTF3( _L("S %d D %d"),temp2, temp1 ); // add for debugging purposes
		 }

	 //[calculate the ratio ]
	 ratio = sum1;
	 ratio /=sum2;
	 TReal sn = 0.0;
	 // calculate as 18db
	 Math::Log( sn, ratio );
	 sn *= 10;

	 INFO_PRINTF2( _L("Signal to Noise Ratio @800Hz %f db"), sn );
#ifdef EABI
	 if( sn < 18 ) //[ @800hz a sn of less than 18db is deemed a failure 
		           // not that sn is a great measure of a voice coder's quality]
		 result = EFail ;
#else
	 if( sn < 14 ) //DEF086144 - Codec source buffer size is reduced
		 result = EFail ;
#endif
	 //[pop data from the cleanup stack ]
	 CleanupStack::PopAndDestroy(2, pDecoder); //pDecoder, theCodec,
     delete iRefSrcData;   
	 iRefSrcData = NULL;
	 delete iRefCodedData;
	 iRefCodedData = NULL;
	 delete iRefDecodedData;
	 iRefDecodedData = NULL;
	 delete iCodedData;   
	 iCodedData = NULL;
	 delete iDecodedData;
	 iDecodedData = NULL;
	 
	__MM_HEAP_MARKEND;

    return result;
	}

/**
*
* DoTestStepPreambleL
*
**/
TVerdict CTest_MMF_SwCodecDevices_U_0003::DoTestStepPreambleL(void)
	{
	return EPass;
	}

/**
*
* DoTestStepPostambleL
*
**/
TVerdict CTest_MMF_SwCodecDevices_U_0003::DoTestStepPostambleL(void)
	{
	return EPass;
	}

/**
*
* FillSrcBufferL
* @param aSrcData pointer to the src data which stores 16bit samples
* @param aNoSamples number of 16bit samples to store
* @param aOffset offset used to generate the samples (linear range)
**/
void TLawUtility::FillSrcBufferL( TUint8* aSrcData, TInt aNoSamples, TInt16 aOffset )
	{
	//[precondition aSrcData != NULL ]
	if( !aSrcData )
		User::Leave( KErrArgument );
	const TInt16 KUpperLimit = static_cast<TInt16>(aOffset + aNoSamples);
	TUint8* pDest = aSrcData ;
	for( TInt16 i = aOffset; i< KUpperLimit; i++ )
		{
	     *pDest++ = static_cast<TUint8>( i & 0xff);
		 *pDest++ = static_cast<TUint8>( (i >>8) &0xff );
		}
	}

/**
*
* CompareCodedData
* @param aCodedData the data coded using symbian codec
* @param aRefCodedData the data coded using independent implementation
* @param aNoSamples the number of coded samples
*
**/
TInt TLawUtility::CompareCodedDataL(TUint8* aCodedData,TUint8* aRefCodedData, TInt aNoSamples )
	{
	TInt result = KErrNone;
	//[precondition aCodedData != NULL ]
     if( !aCodedData )
		 User::Leave( KErrArgument);
	 //[preciondition aRefCodedData != NULL ]
	 if( !aRefCodedData )
		 User::Leave( KErrArgument );

	 //[ use mem compare to compare the data Buffers ]
	if( Mem::Compare(aCodedData, aNoSamples, aRefCodedData, aNoSamples )!=0)
		{
		//[ data is not the same ]
		for( TInt count = 0; count < aNoSamples; count++ )
			{
		 //RDebug::Print(_L("c1 %u c2 %u"), *aCodedData++, *aRefCodedData++); Statement commented under DEF105143
		      *aCodedData++; 
		      *aRefCodedData++; 
			}
		result = KErrCorrupt;
		}

	 return result;
	}

/**
*
* AssembleValL
* @param aDecodedData
* @return decoded value
*
**/
TInt16 TLawUtility::AssembleValL(TUint8* aDecodedData)
	{
	TInt16 val;
    if(!aDecodedData )
		User::Leave( KErrArgument);
 
	//assemble the value 
	val  = static_cast<TInt16>( aDecodedData[0] &KAndMask8bit);  
	val |=  static_cast<TInt16>((aDecodedData[1] << 8 ));

	return val;
	}

/**
*
* SNRatio
*
**/
TReal TLawUtility::SNRatioL(TUint8* aDecodedData, TUint8* aSrcData, TInt aNoSamples )
	{
	const TReal KThreshold = 0.0001;
    TReal ratio = 0.0;
    //[precondition aDecodedData != NULL ]
    if( !aDecodedData )
		User::Leave( KErrArgument );
	//[ precondition aSrcData != NULL ]
	if( !aSrcData )
		User::Leave( KErrArgument );
    
	TReal   sumSig      = 0.0;    // numerator
	TReal   sumNoise    = 0.0; // denominator
	TInt    difference  = 0;
	TInt16  dataValue   = 0;
	TInt    decodeVal   = 0;
	for( TInt count = 0; count < aNoSamples; count++ )
		{
        decodeVal = AssembleValL(aDecodedData);
        dataValue = AssembleValL(aSrcData);
		difference = decodeVal - dataValue;
		sumSig += (decodeVal*decodeVal);     // sum of the squares of the signal
		sumNoise += (difference * difference );  // sum of the square of the difference
		aDecodedData+=2;
		aSrcData+=2;
		}

	//[ guard against division by zero ]
	if( !( sumNoise >= KThreshold ))
		User::Leave( KErrUnderflow );

	//[ calculate the sn ratio ]
	//[ 10log10( sumSig/SumNoise ]
	Math::Log( ratio, (sumSig/sumNoise) );
    ratio *= 10;  // ratio = 10*log( x**2/(error**2)) in db
    return ratio;
	}

/**
*
* CompareSNRatiosL
* @param aCodecSN   codec under test SN ratio in db
* @param aCodecSN2  refernce codec SN ratio in db
* @param aThreshold difference allowed in db
* @result within tolerance
*
**/
TBool TLawUtility::CompareSNRatiosL( TReal aCodecSN, TReal aCodecSN2, TReal aTolerance )
	{
    TBool result = ETrue; 
	TReal difference = (aCodecSN - aCodecSN2);
	//[ it would be nice to replace this with a abs function ?]
	if( difference < 0.0 )
		{
         if( aTolerance > difference )
			 {
			 result = EFalse;
			 }
		}
	else
		{
         if( aTolerance < difference )
			 {
			 result = EFalse;
			 }
		}

	return result;
	}

/**
*
* ComputeSNL compute the Signal to Noise Ratio
*
**/
TReal TLawUtility::ComputeSNL( TReal aSumSigSquared, TReal aSumErrorSquared )
	{
	 TReal sn = 0.0;
     const TReal tolerance = 0.001;
	 //[precondition error is >= tolerance ]
	 if( aSumErrorSquared < tolerance )
		 User::Leave( KErrArgument );
     //[claculate ratio safely ]
	 Math::Log( sn, (aSumSigSquared/aSumErrorSquared));
	 sn*= 10;
	 return sn;
	}

/**
*
* SumSquaredL
*
**/
TReal TLawUtility::SumSquaredL( TUint8* aData, TInt aNoSamples )
	{
	//[precondition arg is ok ]
     if( !aData )
		 {
		 User::Leave(KErrArgument);
		 }

	 TUint8* pData = aData;
	 TInt16 sample ;
	 TReal sumSigSquared = 0.0;
	 for( TInt count = 0; count < aNoSamples; count++ )
		 {
		 sample  = static_cast<TInt16>( pData[0] &KAndMask8bit);  
		 sample |=  static_cast<TInt16>((pData[1] << 8 ));
		 sumSigSquared += (sample*sample);
		 pData+=2;
		 }

	 return sumSigSquared;
	}

/**
*
* SumErrorSquaredL
* @param aData
* @param aData2
* @param aNoSamples
* @result TReal
*
**/
TReal TLawUtility::SumErrorSquaredL( TUint8* aData, TUint8* aData2, TInt aNoSamples )
	{
	//[precondition aData is not NULL]
	 if( !aData )
		 {
		 User::Leave(KErrArgument);
		 }

	 //[precondition aData2 is not NULL ]
	if( !aData2 )
		 {
		 User::Leave(KErrArgument);
		 }

	 TUint8* pData = aData;
	 TUint8* pData2 = aData2;
	 TInt16 sample ;
	 TInt16 sample2;
	 TReal sumErrorSquared = 0.0;
	 TInt error;
	 for( TInt count = 0; count < aNoSamples; count++ )
		 {
		 error = 0;
		 sample  = static_cast<TInt16>( pData[0] &KAndMask8bit);  
		 sample |=  static_cast<TInt16>((pData[1] << 8 ));
		 sample2  = static_cast<TInt16>( pData2[0] &KAndMask8bit);  
		 sample2 |=  static_cast<TInt16>((pData2[1] << 8 ));
		 error = sample -sample2; // compute the error
		 sumErrorSquared += (error*error); // add error squared to the sum
		 pData  +=2;
		 pData2 +=2;
		 }

	 return sumErrorSquared;
	}

/**
*
* CTestMuLawCodec_U_0006
*
**/
CTestMuLawCodec_U_0006::CTestMuLawCodec_U_0006()
	{
     //[ set test name]
	iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0006-HP");
	}

/**
*
* LinearToMuLawSample
* @param aSample a 16 bit pcm sample
* @result Mu Law encoded sample
*
**/
TUint8 CTestMuLawCodec_U_0006::LinearToMuLawSample( TInt16 aSample)
	{
	const int KBias = 0x84;
	const int KClip = 32635;
	TInt sign = (aSample >> 8) & 0x80;
	if(sign)
		aSample = static_cast<TInt16>(-aSample);
	if(aSample > KClip)
		aSample = KClip;
	aSample = static_cast<TInt16>(aSample + KBias);
	TInt exponent = static_cast<TInt>( MuLawCompressTable[(aSample>>7) & 0xFF]);
	TInt mantissa = (aSample >> (exponent+3)) & 0x0F;
	TInt compressedByte = ~(sign | (exponent << 4) | mantissa);
	return static_cast<TUint8>( compressedByte );
	}

/**
*
* ConvertPcmMuLaw
*
**/
void CTestMuLawCodec_U_0006::ConvertPcmMuLawL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples ) 
	{
	//[ precondition aSrcData ]
     if( !aSrcData )
		 User::Leave( KErrArgument );
	 //[precondition aCodedData ]
	 if( !aCodedData )
		 User::Leave( KErrArgument );

	 TUint8* pCoded = aCodedData;
	 TUint8* pData  = aSrcData ;
	 TInt16 pcmSample;
	 for( TInt count = 0; count < aNumSamples; count++ )
		 {
          //[ code the data ]
		  pcmSample  = static_cast<TInt16>(pData[0]);
		  pcmSample |= static_cast<TInt16>((pData[1] << 8 )); 
          *pCoded++ = LinearToMuLawSample(pcmSample);
		  pData+=2;
		 }
	}

/**
*
* ConvertMuLawPcm
* 
**/
void CTestMuLawCodec_U_0006::ConvertMuLawPcmL(TUint8* aCoded, TUint8* aDecoded, TInt aNumSamples )
	{
	//[ precondition aCoded ]
	if( !aCoded )
		User::Leave( KErrArgument );
	//[precondition aDecoded ]
	if( !aDecoded )
		User::Leave( KErrArgument );
	
	TInt16  pcmSample;
	TUint8* pCoded   = aCoded;
	TUint8* pDecoded = aDecoded;
	//[ lets convert the data ]
	for(TInt count = 0; count < aNumSamples; count++ )
		{
		pcmSample = MuLawDecompressTable[*pCoded++];
		*pDecoded++ = static_cast<TUint8>( pcmSample & 0xFF);
		*pDecoded++ = static_cast<TUint8>((pcmSample >> 8 ) & 0xFF);
		}
	}

/**
*
* DoTestStepL
*
**/
TVerdict CTestMuLawCodec_U_0006::DoTestStepL()
	{
	TVerdict result = EPass;
	const TInt KSrcBufferSize     = 400;    // small buffer size
	const TInt KHalfSrcBufferSize = 200;    // small buffer size
	const TInt KCodedBufferSize   = 200;    // small buffer size
	const TInt KLowerLimit        = -800; //lower limit of test range
	const TInt KUpperLimit        = 800;  // upper limit of test range +1
	
	//[ allocate memory buffers]
	TUint8* pSymbianSrcData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pSymbianSrcData);
	TUint8* pIndependentSrcData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pIndependentSrcData);
	TUint8* pSymbianCodedData = new(ELeave)TUint8[KCodedBufferSize];
	CleanupStack::PushL(pSymbianCodedData);
	TUint8* pIndependentCodedData = new(ELeave)TUint8[KCodedBufferSize];
	CleanupStack::PushL(pIndependentCodedData);
	TUint8* pSymbianDecodedData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pSymbianDecodedData);
	TUint8* pIndependentDecodedData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pIndependentDecodedData);
	
    TMMFAudioMuLawToS16PcmCodec decoder;
	TMMFAudioSPcm16ToMuLawCodec encoder;

	TLawUtility helper;
	TReal symbianCodecSN = 0.0;
	TReal independentCodecSN = 0.0;

	TReal sumRefSig       = 0.0; // sum of sig squared
	TReal sumRefError     = 0.0; // sum of error sig squared
	TReal sumSymbianSig   = 0.0; // sum of sig squared
	TReal sumSymbianError = 0.0; // sum of error sig squared

	//[ interate over a suitable range and process each buffer]
	for( TInt index = KLowerLimit; index < KUpperLimit; index+= KHalfSrcBufferSize )
		{
		TInt16 offset = static_cast<TInt16>( index);
		//[ fill the src buffers ]
		helper.FillSrcBufferL( pSymbianSrcData, KHalfSrcBufferSize, offset );
		helper.FillSrcBufferL( pIndependentSrcData, KHalfSrcBufferSize, offset );

		//[encode the src data ]
		encoder.Convert( pSymbianSrcData, pSymbianCodedData, KHalfSrcBufferSize );
		ConvertPcmMuLawL(pIndependentSrcData,pIndependentCodedData,KHalfSrcBufferSize );
	
		//[ decode the data ]
        decoder.Convert( pSymbianCodedData, pSymbianDecodedData, KHalfSrcBufferSize );		
	    ConvertMuLawPcmL( pIndependentCodedData,pIndependentDecodedData,KHalfSrcBufferSize);		
	
		//[ check both codecs code the data similarly]
		TInt errorCode =helper.CompareCodedDataL(pIndependentCodedData, pSymbianCodedData, KHalfSrcBufferSize );		
		if( errorCode != KErrNone )
			{
			INFO_PRINTF1(_L("Forward Transformation for Mu-Law codec is not conformant to ref codec"));
			User::LeaveIfError(errorCode);
			}
			//[ upate running total sums to be used for signal to noise
		// ratio calculations ]
		sumRefSig        += helper.SumSquaredL(pIndependentSrcData, KHalfSrcBufferSize);     
		sumRefError      += helper.SumErrorSquaredL(pIndependentSrcData,pIndependentDecodedData,KHalfSrcBufferSize); 
		sumSymbianSig    += helper.SumSquaredL(pSymbianSrcData,KHalfSrcBufferSize);
		sumSymbianError  += helper.SumErrorSquaredL(pSymbianSrcData,pSymbianDecodedData,KHalfSrcBufferSize);
		}
 		
	const TReal KTolerance = 1; // allow for a 1 db tolerance
	symbianCodecSN     = helper.ComputeSNL(sumSymbianSig,sumSymbianError);
	independentCodecSN = helper.ComputeSNL(sumRefSig, sumRefError);
    // Gamma = (dynamic range of codec /signal std deviation )
	INFO_PRINTF1(_L("We would expect S/N ration to be greater than 35db for an MuLaw codec with Gamma = 10"));
	INFO_PRINTF2(_L("Signal/Noise Ratio Symbian Codec %f"), symbianCodecSN ); 
	INFO_PRINTF2(_L("Signal/Noise Ratio Reference Codec %f"), independentCodecSN ); 

	//[ compare the s/n ratio's of the two codec implementations]
	if( !helper.CompareSNRatiosL( symbianCodecSN, independentCodecSN, KTolerance ))
		{
		//[ fail the test because the s/n ratios were divergent ]
		result = EFail;
		}
	CleanupStack::PopAndDestroy(6,pSymbianSrcData); //pSymbianSrcData,pIndependentSrcData,
	                                                //pSymbianCodedData,pIndependentCodedData
	                                                //pSymbianDecodedData,pIndependentDecodedData 
	
	return result;
	}

/**
*
*  DoTestStepPreambleL
*
**/
TVerdict CTestMuLawCodec_U_0006::DoTestStepPreambleL(void)
	{
	TVerdict result = EPass;
	return result; //nothing doing
	}

/**
*
* DoTestStepPostambleL
*
**/
TVerdict CTestMuLawCodec_U_0006::DoTestStepPostambleL(void)
	{
	TVerdict result = EPass;
	return result; //nothing doing
	}

/**
*
* Mu-Law Compression Table
*
**/
const TInt8 CTestMuLawCodec_U_0006::MuLawCompressTable[PcmToMuLawCompressionTableSize] =
{
0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

/**
*
* Mu-Law Decompression Table
*
**/
const TInt16 CTestMuLawCodec_U_0006::MuLawDecompressTable[MuLawToPcmCompressionTableSize]=
{
-32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
-23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
-15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
-11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
-7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
-5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
-3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
-2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
-1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
-1372, -1308, -1244, -1180, -1116, -1052, -988, -924,
-876, -844, -812, -780, -748, -716, -684, -652,
-620, -588, -556, -524, -492, -460, -428, -396,
-372, -356, -340, -324, -308, -292, -276, -260,
-244, -228, -212, -196, -180, -164, -148, -132,
-120, -112, -104, -96, -88, -80, -72, -64,
-56, -48, -40, -32, -24, -16, -8, 0,
32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
11900, 11388, 10876, 10364, 9852, 9340, 8828, 8316,
7932, 7676, 7420, 7164, 6908, 6652, 6396, 6140,
5884, 5628, 5372, 5116, 4860, 4604, 4348, 4092,
3900, 3772, 3644, 3516, 3388, 3260, 3132, 3004,
2876, 2748, 2620, 2492, 2364, 2236, 2108, 1980,
1884, 1820, 1756, 1692, 1628, 1564, 1500, 1436,
1372, 1308, 1244, 1180, 1116, 1052, 988, 924,
876, 844, 812, 780, 748, 716, 684, 652,
620, 588, 556, 524, 492, 460, 428, 396,
372, 356, 340, 324, 308, 292, 276, 260,
244, 228, 212, 196, 180, 164, 148, 132,
120, 112, 104, 96, 88, 80, 72, 64,
56, 48, 40, 32, 24, 16, 8, 0
};

/**
*
* CTestALawCodec_U_0004
*
**/
CTestALawCodec_U_0004::CTestALawCodec_U_0004()
	{
    //[ set test name ]
	iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0004-HP");
	}

/**
*
* ConvertPcmALaw converts Pcm 16 to 8bit ALaw
* @param aSrcData The src data
* @param aCoded   The coded result
* @param aNumSamples The number of samples to be processed
* @precondition aSrcData is not NULL
* @precondition aCodedData is not NULL
* @precondition there is sufficient room in the destination
* to contain the coded samples
*
**/
void CTestALawCodec_U_0004::ConvertPcmALawL(TUint8* aSrcData, TUint8* aCodedData, TInt aNumSamples )
	{
	//[ precondition aSrcData ]
     if( !aSrcData )
		 User::Leave( KErrArgument );
	 //[precondition aCodedData ]
	 if( !aCodedData )
		 User::Leave( KErrArgument );

	 TUint8* pCoded = aCodedData;
	 TUint8* pData  = aSrcData ;
	 TInt16 pcmSample;
	 for( TInt count = 0; count < aNumSamples; count++ )
		 {
          //[ code the data ]
		  pcmSample  = static_cast<TInt16>(pData[0]);
		  pcmSample |= static_cast<TInt16>((pData[1] << 8 )); 
          *pCoded++ = LinearToALawSample(pcmSample);
		  pData+=2;
		 }
	}

/**
*
* ConvertALawPcm converts from 8bit ALaw to Pcm16
* @param aCoded The coded data
* @param aDecoded The decoded result
* @param aNumSamples The number of samples to be processed
* @precondition aCoded is not NULL
* @precondition aDecoded is not NULL
* @precondition there is sufficient room in the destination
* to contain the coded samples
*
**/
void CTestALawCodec_U_0004::ConvertALawPcmL(TUint8* aCoded, TUint8* aDecoded, TInt aNumSamples )
	{
	 //[ precondition aSrcData ]
     if( !aCoded )
		 User::Leave( KErrArgument );
	 //[precondition aCodedData ]
	 if( !aDecoded )
		 User::Leave( KErrArgument );
   
	 TInt16  pcmSample;
     TUint8* pCoded   = aCoded;
	 TUint8* pDecoded = aDecoded;
	 //[ lets convert the data ]
     for(TInt count = 0; count < aNumSamples; count++ )
		 {
          pcmSample = ALawDecompressTable[*pCoded++];
          *pDecoded++ = static_cast<TUint8>(pcmSample & 0xFF);
		  *pDecoded++ = static_cast<TUint8>((pcmSample >> 8 ) & 0xFF);
		 }
	}

/**
*
* LinearToALawSample converts a Pcm16 sample to ALaw
* @param aSample the PCM 16 sample to be converted
* @result coded result
* 
**/
TUint8 CTestALawCodec_U_0004::LinearToALawSample(TInt16 aSample)
	{
	const TInt KClip = 32635;
	TInt sign;
	TInt exponent;
	TInt mantissa;
	TUint8 compressedByte;
	sign = ((~aSample) >> 8) & 0x80;
	if(!sign)
		aSample = static_cast<TInt16>(-aSample);
	if(aSample > KClip)
		aSample = KClip;
	if(aSample >= 256)
		{
		exponent = static_cast<TInt>( ALawCompressTable[(aSample >> 8) & 0x7F]);
		mantissa = (aSample >> (exponent + 3) ) & 0x0F;
		compressedByte = static_cast<TUint8> ((exponent << 4) | mantissa);
		}
	else
		{
		compressedByte = static_cast<TUint8> (aSample >> 4);
		}
	compressedByte ^= (sign ^ 0x55);
	return compressedByte;
	}

/**
*
* DoTestStepL
* 
**/
TVerdict CTestALawCodec_U_0004::DoTestStepL()
	{
	TVerdict result = EPass;
	const TInt KSrcBufferSize     = 400;    // small buffer size
	const TInt KHalfSrcBufferSize = 200;    // small buffer size
	const TInt KCodedBufferSize   = 200;    // small buffer size
	const TInt KLowerLimit        = -400; //lower limit of test range
	const TInt KUpperLimit        = 400;  // upper limit of test range +1
	
	//[ allocate memory buffers]
	TUint8* pSymbianSrcData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pSymbianSrcData);
	TUint8* pIndependentSrcData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pIndependentSrcData);
	TUint8* pSymbianCodedData = new(ELeave)TUint8[KCodedBufferSize];
	CleanupStack::PushL(pSymbianCodedData);
	TUint8* pIndependentCodedData = new(ELeave)TUint8[KCodedBufferSize];
	CleanupStack::PushL(pIndependentCodedData);
	TUint8* pSymbianDecodedData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pSymbianDecodedData);
	TUint8* pIndependentDecodedData = new(ELeave)TUint8[KSrcBufferSize];
	CleanupStack::PushL(pIndependentDecodedData);
	
	TMMFAudioSPcm16ToAlawCodec encoder;
	TMMFAudioALawToS16PcmCodec decoder;
    
	TLawUtility helper;
	TReal symbianCodecSN = 0.0;
	TReal independentCodecSN = 0.0;

	TReal sumRefSig       = 0.0; // sum of sig squared
	TReal sumRefError     = 0.0; // sum of error sig squared
	TReal sumSymbianSig   = 0.0; // sum of sig squared
	TReal sumSymbianError = 0.0; // sum of error sig squared

	//[ interate over a suitable range and process each buffer]
	for( TInt index = KLowerLimit; index < KUpperLimit; index+= KHalfSrcBufferSize )
		{
		TInt16 offset = static_cast<TInt16>( index);
		//[ fill the src buffers ]
		helper.FillSrcBufferL( pSymbianSrcData, KHalfSrcBufferSize, offset );
		helper.FillSrcBufferL( pIndependentSrcData, KHalfSrcBufferSize, offset );

		//[encode the src data ]
		encoder.Convert( pSymbianSrcData, pSymbianCodedData, KHalfSrcBufferSize );
		ConvertPcmALawL(pIndependentSrcData,pIndependentCodedData,KHalfSrcBufferSize );
	
		//[ decode the data ]
        decoder.Convert( pSymbianCodedData, pSymbianDecodedData, KHalfSrcBufferSize );		
	    ConvertALawPcmL( pIndependentCodedData,pIndependentDecodedData,KHalfSrcBufferSize);		
	
		//[ check both codecs code the data similarly]
		 TInt errorCode = helper.CompareCodedDataL(pIndependentCodedData, pSymbianCodedData, KHalfSrcBufferSize );
		 if( errorCode != KErrNone )
			 {
			 INFO_PRINTF1(_L("Forward Transformation for ALaw codec is not conformant to ref codec"));
			 User::LeaveIfError(errorCode);
			 }
	    
		//[ upate running total sums to be used for signal to noise
		// ratio calculations ]
		sumRefSig        += helper.SumSquaredL(pIndependentSrcData, KHalfSrcBufferSize);     
		sumRefError      += helper.SumErrorSquaredL(pIndependentSrcData,pIndependentDecodedData,KHalfSrcBufferSize); 
		sumSymbianSig    += helper.SumSquaredL(pSymbianSrcData,KHalfSrcBufferSize);
		sumSymbianError  += helper.SumErrorSquaredL(pSymbianSrcData,pSymbianDecodedData,KHalfSrcBufferSize);
		}
	const TReal KTolerance = 1; // allow for a 1 db tolerance
	symbianCodecSN     = helper.ComputeSNL(sumSymbianSig,sumSymbianError);
	independentCodecSN = helper.ComputeSNL(sumRefSig, sumRefError);
    // Gamma = (dynamic range of codec /signal std deviation )
	INFO_PRINTF1(_L("We would expect S/N ration to be greater than 30db for an ALaw codec with Gamma = 10"));
	INFO_PRINTF2(_L("Signal/Noise Ratio Symbian Codec %f"), symbianCodecSN ); 
	INFO_PRINTF2(_L("Signal/Noise Ratio Reference Codec %f"), independentCodecSN ); 

	//[ compare the s/n ratio's of the two codec implementations]
	if( !helper.CompareSNRatiosL( symbianCodecSN, independentCodecSN, KTolerance ))
		{
		//[ fail the test because the s/n ratios were divergent ]
		result = EFail;
		}
	
	CleanupStack::PopAndDestroy(6,pSymbianSrcData); //pSymbianSrcData,pIndependentSrcData,
	                                //pSymbianCodedData,pIndependentCodedData
	                                //pSymbianDecodedData,pIndependentDecodedData 
	return result;                  
	}

/**
*
* DoTestStepPreambleL
*
**/
TVerdict CTestALawCodec_U_0004::DoTestStepPreambleL(void)
	{
	TVerdict result = EPass;
	return result; //nothing doing
	}
/**
*
* DoTestStepPostambleL
*
**/
TVerdict CTestALawCodec_U_0004::DoTestStepPostambleL(void)
	{
	TVerdict result = EPass;
	return result; //nothing doing
	}
/**
*
* ALaw Compression Table
*
**/
const TInt8 CTestALawCodec_U_0004::ALawCompressTable[PcmToALawCompressionTableSize] =
{
1,1,2,2,3,3,3,3,
4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7
};

/**
*
* ALaw Decompression Table 
*
**/
const TInt16 CTestALawCodec_U_0004::ALawDecompressTable[ALawToPcmCompressionTableSize] =
{
-5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736,
-7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
-2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368,
-3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392,
-22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944,
-30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136,
-11008,-10496,-12032,-11520,-8960, -8448, -9984, -9472,
-15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568,
-344, -328, -376, -360, -280, -264, -312, -296,
-472, -456, -504, -488, -408, -392, -440, -424,
-88, -72, -120, -104, -24, -8, -56, -40,
-216, -200, -248, -232, -152, -136, -184, -168,
-1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
-1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696,
-688, -656, -752, -720, -560, -528, -624, -592,
-944, -912, -1008, -976, -816, -784, -880, -848,
5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736,
7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784,
2752, 2624, 3008, 2880, 2240, 2112, 2496, 2368,
3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392,
22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944,
30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136,
11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472,
15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568,
344, 328, 376, 360, 280, 264, 312, 296,
472, 456, 504, 488, 408, 392, 440, 424,
88, 72, 120, 104, 24, 8, 56, 40,
216, 200, 248, 232, 152, 136, 184, 168,
1376, 1312, 1504, 1440, 1120, 1056, 1248, 1184,
1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696,
688, 656, 752, 720, 560, 528, 624, 592,
944, 912, 1008, 976, 816, 784, 880, 848
};

/**
*
* CTestIMaadCodec
*
**/
CTestIMaadCodec::CTestIMaadCodec()
	{
	//[ set test name ]
	iTestStepName = _L("MM-MMF-SWCODECDEVICES-U-0022-HP");
	}

/**
*
* DoTestStepL
*
**/
TVerdict CTestIMaadCodec::DoTestStepL()
	{
	__MM_HEAP_MARK;    
	TVerdict result = EPass;
	TInt srcBufferSize;
	TInt sinkBufferSize;
	const TReal KExpectedSNRatioDb = 30.0; //30 db for now
   
	//[ Create coder and decoder codecs ]
	CMMFPcm16ToImaAdpcmHwDevice* pHwDevice = CMMFPcm16ToImaAdpcmHwDevice::NewL();
    CleanupStack::PushL( pHwDevice );

	CMMFSwCodec& theCodec = pHwDevice->Codec();

	CMMFImaAdpcmToPcm16CodecHwDevice* pHwDecoder = CMMFImaAdpcmToPcm16CodecHwDevice::NewL();
    CleanupStack::PushL( pHwDecoder );

	CMMFSwCodec& theDecoder = pHwDecoder->Codec();

	//[ Create data buffers with position != 0]
	srcBufferSize  = 100; // arbitrary non zero size
	sinkBufferSize = 100;
    CMMFDescriptorBuffer* pSrcBuffer =  CMMFDescriptorBuffer::NewL( srcBufferSize ); 
	CleanupStack::PushL( pSrcBuffer );
	
	CMMFDescriptorBuffer* pSinkBuffer = CMMFDescriptorBuffer::NewL( sinkBufferSize ); 
	CleanupStack::PushL( pSinkBuffer );

	//[ trap & check error code ]
	TInt errCode;
	pSrcBuffer->Data().SetLength(srcBufferSize);
	pSinkBuffer->Data().SetLength(sinkBufferSize);
	pSrcBuffer->SetPosition(1);
    TRAP( errCode, theCodec.ProcessL(*pSrcBuffer, *pSinkBuffer));
	if( errCode != KErrArgument )
		{
		result = EFail;
		return result;
		}

  	//[set position of sink buffer to nonzero value]
	pSrcBuffer->SetPosition(0);
    pSinkBuffer->SetPosition(1);
    TRAP( errCode, theCodec.ProcessL(*pSrcBuffer, *pSinkBuffer));
	if( errCode != KErrArgument )
		{
		result = EFail;
		return result;
		}

	//[set position of sink and src to nonzero value ]
	pSrcBuffer->SetPosition(1);
    pSinkBuffer->SetPosition(1);
    TRAP( errCode, theCodec.ProcessL(*pSrcBuffer, *pSinkBuffer));
	if( errCode != KErrArgument )
		{
		result = EFail;
		return result;
		}

	//[ reset the position of both buffers to zero ]
    pSrcBuffer->SetPosition(0);
    pSinkBuffer->SetPosition(0);
    //[ set the src/sink buffer sizes to src and sink
	// buffer sizes and fill src with data ]
	CleanupStack::PopAndDestroy(2, pSrcBuffer); // pSrcBuffer, pSinkBuffer
    
	//[Create Source & Sink and fill source data in ]
	srcBufferSize = theCodec.SourceBufferSize();
	pSrcBuffer  = CMMFDescriptorBuffer::NewL( srcBufferSize );
	CleanupStack::PushL( pSrcBuffer );

	CMMFDescriptorBuffer* pDecodedBuffer = CMMFDescriptorBuffer::NewL( srcBufferSize );
	CleanupStack::PushL( pDecodedBuffer );

	sinkBufferSize = theCodec.SinkBufferSize();
	pSinkBuffer = CMMFDescriptorBuffer::NewL( sinkBufferSize );
    CleanupStack::PushL( pSinkBuffer );

	pSrcBuffer->Data().SetLength(srcBufferSize);
	pDecodedBuffer->Data().SetLength(srcBufferSize);
	pSinkBuffer->Data().SetLength(sinkBufferSize);

	//[ fill src buffer with ramp] 
	FillSrcBufferL( *pSrcBuffer );
    // encode and decode the data
    theCodec.ProcessL(*pSrcBuffer, *pSinkBuffer);
	theDecoder.ProcessL( *pSinkBuffer, *pDecodedBuffer );
    
	if(!CompareResults( KExpectedSNRatioDb, pSrcBuffer, pDecodedBuffer))
		{
		//Test has failed because sn ratio was not good enough
		result = EFail;
		}

	//[ clean up ]
	CleanupStack::PopAndDestroy( 5, pHwDevice ); // pHwDevice, pHwDecoder, pSrcBuffer, pDecodedBuffer, pSinkBuffer 

	__MM_HEAP_MARKEND;
	return result;
	}

/**
*
* DoTestStepPreambleL
*
**/
TVerdict CTestIMaadCodec::DoTestStepPreambleL(void)
	{
    return EPass;
	}

/**
*
* DoTestStepPostambleL
*
**/
TVerdict CTestIMaadCodec::DoTestStepPostambleL(void)
	{
    return EPass;
	}


/**
*
* FillSrcBuffer
* @param aBuffer
* This function fills the buffer with a ramp of linear pcm16 data
*
**/
void CTestIMaadCodec::FillSrcBufferL( CMMFDescriptorBuffer& aBuffer )
	{
	 TInt slope = 2;
     TInt dataLength = aBuffer.Data().Length(); 
	 TUint8* pData = const_cast<TUint8*>(aBuffer.Data().Ptr());
	 TInt noPc16Samples = dataLength/2;
	 ASSERT( noPc16Samples*slope < 32768 );
	 for( TInt16 count = 0; count < noPc16Samples ; count++ )
		 {
		 TInt16 pcmSample = static_cast<TInt16>( count * slope);
          *pData++ = static_cast<TUint8>( pcmSample & 0xFF );
		  *pData++ = static_cast<TUint8>( ( pcmSample >> 8 ));
		 }
	}

/**
*
* CompareResults
* @param aExpectedSNRatioDb
* @param aSrcBuffer
* @param aSinkBuffer
* @result TBool
* This function returns True if the computed Signal to Noise Ratio
* is Greater than or equal to the expected signal to noise ratio.
* The function will also return EFalse if any of the preconditions
* are violated.
* @precondition aSrcBuffer, aSinkBuffer are not NULL
* @precondition aSrcBuffer data lenegth == aSinkBuffer data length
* @precondition the data buffers contain pcm16 data
* 
**/
TBool CTestIMaadCodec::CompareResults( TReal aExpectedSNRatioDb, 
			                  CMMFDescriptorBuffer* aSrcBuffer,     
		                      CMMFDescriptorBuffer* aSinkBuffer)
	{
     TBool result = EFalse;

	 //[ precondition pointers are not NULL ]
	 if( !aSrcBuffer || !aSinkBuffer )
		 return result;

	 //[ precondition buffer lengths are equal ]
     TInt length = aSrcBuffer->Data().Length();
	 if( length != aSinkBuffer->Data().Length() )
		 return result;

	 // buffers must be of even length
	 if( !(length % sizeof(TInt16) == 0 ))
		 return result;

     TInt pcmLength = length/2;
     TReal sumSignalSquared = 0.0;
	 TReal sumNoiseSquared  = 0.0;
     TUint8* pSrcData = const_cast<TUint8*>(aSrcBuffer->Data().Ptr());
	 TUint8* pDecodeData    = const_cast<TUint8*>(aSinkBuffer->Data().Ptr());
     TInt16 sampleOriginal;
	 TInt16 sampleDecode;
	 for( TInt count = 0; count < pcmLength; count++ )
		 {
		  sampleOriginal  = static_cast<TInt16>( pSrcData[0] &KAndMask8bit);  
		  sampleOriginal |=  static_cast<TInt16>((pSrcData[1] << 8 ));	
          sampleDecode    = static_cast<TInt16>( pDecodeData[0] &KAndMask8bit);  
		  sampleDecode   |=  static_cast<TInt16>((pDecodeData[1] << 8 )); 
          pSrcData+=2;
		  pDecodeData+= 2;
		  sumSignalSquared += sampleOriginal * sampleOriginal;
		  TInt noise = sampleOriginal - sampleDecode ;
          sumNoiseSquared  += noise * noise;
		 }
     
	 //[ if the noise is low the signals are equivalent and
	 // overflow can be avoided ]
	 if( sumNoiseSquared < 0.001 )
		 {
		 result = ETrue;
		 return result;
		 }
	 TReal computedSNRatioDb;
	 Math::Log( computedSNRatioDb, sumSignalSquared/sumNoiseSquared );	 
	 computedSNRatioDb *= 10;

	 //[ compare claculated s/n ratio against expected ]
     if( computedSNRatioDb >= aExpectedSNRatioDb )
		 result = ETrue;

     return result;
	}
