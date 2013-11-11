/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



// INCLUDE FILES
#include "H264DecTestEngine.h"
#include "T_DevVideoConstants.h"

#ifdef __CI_HEADERS__
#include <buffermanagementci.h>  // Custom interface buffre management

#endif


#ifdef __SECUREOUTPUTCI__
#include <secureoutputci.h>
#endif

#include <Devvideoplay.h>
#include <Devvideobase.h>
#include <Devvideoconstants.h>
#include <hal.h>
#include <hal_data.h>
//#define __MEM_CHECK_


// Ecom
#include <ecom.h>


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::~CVDecTestEngine()
* Purpose:    Deconstructor
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

CVDecTestEngine::~CVDecTestEngine ()
{
	delete iDecTestAO;
	delete iClock;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine:NewL()
* Purpose:    Create instance of test engine
* Parameters: MVDecEngineObserver& aTestClass
* Return:     CVDecTestEngine*
* --------------------------------------------------------------------------*/

CVDecTestEngine* CVDecTestEngine::NewL(MVDecEngineObserver& aTestClass)
{
	CVDecTestEngine* self = new(ELeave) CVDecTestEngine;

	CleanupStack::PushL( self );
	self->ConstructL(aTestClass);
	CleanupStack::Pop();

	return self;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ConstructL()
* Purpose:    Symbian second phase constructor, which may leave
* Parameters: MVDecEngineObserver& aTestClass
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::ConstructL(MVDecEngineObserver& aTestClass)
{
	iState = EStateNone;
	iTestClass = &aTestClass;
	iScreenDevice = NULL;

	//Create Active object
	iDecTestAO = new(ELeave) CVDecTestAO(this);

	//clock source from system clock
	iClock = CSystemClockSource::NewL();
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::AssertTIntEqualL()
* Purpose:
*
* Parameters: TInt aExpected, TInt aActual
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::AssertTIntEqualL(TInt aExpected, TInt aActual)
{
	if ( aExpected != aActual )
	{
		User::Leave(aActual);
	}
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetUp()
* Purpose:    Create DevVideoPlay, Initilize Engine paramters
* Parameters: TSize aSize, TBool aScreenAccess, TBool aSynchronized, TBool aCIBuffMgmt
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetUpL(TSize aSize, TBool aScreenAccess, TBool aCIBuffMgmt)
{

	iMemAlloc = 0;
	iMemDelete = 0;
	//Create DevVideoPlay
	iDevvp = CMMFDevVideoPlay::NewL(*this);
	PRINT((_L("CVDecTestEngine::SetUpL, DevVideo Created")))
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::SetUpL, mem alloc, iDevvp") ))
	#endif
	iMemAlloc++;
	iState = ECreated;

	// Initilize Engine parameters
	iError = KErrNone;
	iDecHWDevId = 0;
	iPostProcId = 0;
	iInBuffSize = 0;
	iRawDataArea = NULL;
	iCodecType = ENoCodec;
	iInputEnd = EFalse;
	iFrameMatch = EFalse;
	iFrameJump = 0;
	iFrameJumpCounter = 0;
	iCorruptMarker = EFalse;
	iOutCorruptedFileOpen = EFalse;
	iFrameCounter = 0;
	iCorruptCounter = 0;
	iFlvCounter = 0;
	iFrameDropCounter = 0;
	iFrameDropInterval = 0;
	iFrameDropNum = 0;
	iFrameDropMarker = EFalse;
	iCurrentFilePos = 0;
	iFrameDropNumCounter = 0;
	iSetPassword = EFalse;
	iUseSecondScreen = EFalse;


	#ifdef __CI_HEADERS__
	iCIBuffMgmtOn = aCIBuffMgmt;
	#endif

	iDirectScreenAccess = aScreenAccess;
	iSynchronized = EFalse;
	iLandscapeMode =EFalse;

	PRINT((_L("CVDecTestEngine::SetUpL, reserving data chunk")));


	if (aSize.iWidth > 720 && aSize.iWidth > 576 )
	{
		iDataChunkSize = KMP4MaxCodedSize720P*4;
		iDataThreshold = KMP4MaxCodedSize720P;
	}
	else if (aSize.iWidth > 640 && aSize.iWidth > 480 )
	{
		iDataChunkSize = KMP4MaxCodedSizePAL*4;
		iDataThreshold = KMP4MaxCodedSizePAL;
	}
	else if (aSize.iWidth > 352 && aSize.iWidth > 288 )
	{
		iDataChunkSize = KMP4MaxCodedSizeVGA*4;
		iDataThreshold = KMP4MaxCodedSizeVGA;
	}
	else if (aSize.iWidth <= 352 && aSize.iWidth > 176)
	{
		iDataChunkSize = KMP4MaxCodedSizeCIF*4;
		iDataThreshold = KMP4MaxCodedSizeCIF;
	}
	else
	{
		iDataChunkSize = KMP4MaxCodedSizeQCIF*4;
		iDataThreshold = KMP4MaxCodedSizeQCIF;
	}
	PRINT((_L("CVDecTestEngine::SetUpL, Data chunk size is [%d]"), iDataChunkSize));
	PRINT((_L("CVDecTestEngine::SetUpL, Data threshold [%d]"), iDataThreshold));
	iDataChunk = new (ELeave) TUint8[iDataChunkSize];
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::SetUpL, mem alloc, iDataChunk") ));
	#endif
	iReadDataChunk = iDataChunk;
	iWriteDataChunk = iDataChunk;


	iPictureSize.SetSize(aSize.iWidth,aSize.iHeight);
	TRect rect(iPictureSize);
	iDispRect = rect;
	iFrameMeasurement = EFalse;
	iDsaStarted = EFalse;

	if ( aScreenAccess )
	{
		User::LeaveIfError( FbsStartup() );
		PRINT((_L("CVDecTestEngine::SetUpL, Fbs server started:")))
	}

	//Open File session
	TInt err;

	if ( (err = iFs.Connect()) != KErrNone )
	{
		PRINT((_L("CVDecTestEngine::SetUpL, Open File server session fail ")))
		User::Leave(err);
	}
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::EnableFrameMeasurementTest
* Purpose:    Enable Perforamce test flag
* Note
* Parameters: TBool a Enable
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::EnableFrameMeasurementTest(TBool aEnable)
{
	iFrameMeasurement = aEnable;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetInputBufferSize
* Purpose:
* Note
* Parameters: TBool a Enable
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetInputBufferSize(TInt aSize)
{
	iInBuffSize = aSize;
	if ( iCIBuffMgmtOn )  //Custom interfarce Buffer management is used
	{
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, CI Buffer management")))
		#ifdef __CI_HEADERS__
		iCIBuffMgmt = (MMmfVideoBufferManagement*)iDevvp->CustomInterface(iPostProcId, KMmfVideoBuffermanagementUid);
		iCIBuffMgmt->MmvbmSetObserver(this);
		iCIBuffMgmt->MmvbmEnable(ETrue);

		MMmfVideoBufferManagement::TBufferOptions options;
		options.iNumInputBuffers = KNumOfInputBuffers;
		options.iBufferSize = iPictureSize;

		iCIBuffMgmt->MmvbmSetBufferOptionsL(options);

		PRINT((_L("CVDecTestEngine::SetInputBufferSize, CI Buffer management Set, Buff size:[%d],[%d]"),iPictureSize.iWidth,iPictureSize.iHeight))
		#endif
	}

	else  //Use default Devvideo API for buffer management
	{

		iRawInBuffer = new(ELeave) TVideoPicture;
		iMemAlloc++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, mem alloc, iRawInBuffer") ));
		#endif
		TUint8* ptr = new (ELeave) TUint8[iInBuffSize];
		iMemAlloc++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, mem alloc, ptr") ));
		#endif
		TPtr8*  temp = new (ELeave) TPtr8(ptr, 0, iInBuffSize);
		iMemAlloc++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, mem alloc, temp") ));
		#endif
		iRawInBuffer->iData.iRawData = temp;

		iRawInBuffer->iData.iDataSize.SetSize(iPictureSize.iWidth,iPictureSize.iHeight); //set picture size

		iRawInBuffer->iOptions = 0;
		iRawInBuffer->iTimestamp = 0;
		delete ptr;
		iMemDelete++;
		delete temp;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, mem del, ptr & temp") ));
		#endif
		PRINT((_L("CVDecTestEngine::SetInputBufferSize, Buffer created: Size[%d]"),iInBuffSize))
	}
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetHeaderInformationL()
* Purpose:    Ritrieve header  Information
* Parameters: TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataUnitEncapsulation
* Return:     None
*---------------------------------------------------------------------------*/

void CVDecTestEngine::GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataUnitEncapsulation)
{
	TInt err = KErrNone;
	TVideoPictureHeader* headerInfo = NULL;
	HBufC8* tempBuff = NULL;

	//Read Picture header : Size is not known
	TVideoInputBuffer* codedBuffer = new(ELeave) TVideoInputBuffer;
	iMemAlloc++;
	CleanupStack::PushL( codedBuffer );
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem alloc, codedBuffer") ))
	#endif
	iInBuffSize = KMaxCodedSize;
	err = KErrOverflow;

	while ( (err == KErrOverflow) && (iInBuffSize <= KMaxCodedSize) )
	{
		tempBuff = HBufC8::NewL(iInBuffSize);
		iMemAlloc++;
		CleanupStack::PushL( tempBuff );
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem alloc, codedBuffer") ))
		#endif
		codedBuffer->iData.Set(tempBuff->Des());
		if ( iFrameSizeList.Count() > 0 )
		{
			err = ReadOneCodedPicture(codedBuffer, iFrameSizeList[0] );
		}
		else
		{
			err = ReadOneCodedPicture(codedBuffer);
		}

		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, return err [%d]"),err))
		TInt pos =0;
		if ( iInFile.Seek(ESeekStart,pos) )
		{
			err = KErrGeneral;
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, err = KErrGeneral")))
		}

		if ( iInputEnd )
		{
			err = KErrNotFound;
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, err = KErrNotFound")))
		}

		if ( err < 0 )
		{
			CleanupStack::PopAndDestroy(tempBuff);
			iMemDelete++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, tempBuff") ))
			#endif
			iInBuffSize = 4*iInBuffSize;
		}
	}

	// Reitrieve header information from bitstream
	if ( err < 0 )
	{
		CleanupStack::PopAndDestroy(codedBuffer);
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, codedBuffer") ))
		#endif
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Reading input data fail")))
	}
	else
	{
		err = KErrNone;
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, call adaptation layer")))
		headerInfo = iDevvp->GetHeaderInformationL(aDataUnitType,aDataUnitEncapsulation,codedBuffer);

		CleanupStack::PopAndDestroy(tempBuff);
		iMemDelete++;
		CleanupStack::PopAndDestroy(codedBuffer);
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, tempBuff & codedBuffer") ))
		#endif
		//Check Header info
		if ( !err && headerInfo )
		{
			//Size in Memory
			iPictureSize = headerInfo->iDisplayedRect.Size();
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Picture Size :width[%d], height[%d]"),iPictureSize.iWidth,iPictureSize.iHeight))

			// Display rect
			iDispRect = headerInfo->iDisplayedRect;
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Displayed image portion: Width[%d] to Height[%d]"),headerInfo->iDisplayedRect.Width(),headerInfo->iDisplayedRect.Height()))

			//return headerInfo
			iDevvp->ReturnHeader(headerInfo);

		}
		else  //temp modify because MP4HwDevice of Emuzed returns null information in 1st release
		{
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Get header info fail")))
			err = KErrGeneral;
		}
	}

	AssertTIntEqualL(KErrNone, err);

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetHeaderInformationL()
* Purpose:    Retrieve header Information
* Parameters: None
* Return:     None
*---------------------------------------------------------------------------*/

void CVDecTestEngine::GetHeaderInformationL()
{
	PRINT((_L("CVDecTestEngine::GetHeaderInformationL, In")))
	TInt err = KErrNone;
	TVideoPictureHeader* headerInfo = NULL;
	HBufC8* tempBuff = NULL;

	//Read Picture header : Size is not known
	TVideoInputBuffer* codedBuffer = new(ELeave) TVideoInputBuffer;
	iMemAlloc++;
	CleanupStack::PushL( codedBuffer );
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem alloc, codedBuffer") ))
	#endif
	iInBuffSize = KMaxCodedSize;
	err = KErrOverflow;

	while ( (err == KErrOverflow) && (iInBuffSize <= KMaxCodedSize) )
	{
		tempBuff = HBufC8::NewL(iInBuffSize);
		iMemAlloc++;
		CleanupStack::PushL( tempBuff );
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem alloc, tempBuff") ))
		#endif
		codedBuffer->iData.Set(tempBuff->Des());

		if ( iFrameSizeList.Count() > 0 )
		{
			err = ReadOneCodedPicture(codedBuffer, iFrameSizeList[0] );
		}
		else
		{
			err = ReadOneCodedPicture(codedBuffer);
		}


		TInt pos =0;
		if ( iInFile.Seek(ESeekStart,pos) )
		{
			err = KErrGeneral;
		}

		if ( iInputEnd )
		{
			err = KErrNotFound;
		}

		if ( err < 0 )
		{
			CleanupStack::PopAndDestroy(tempBuff);
			iMemDelete++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, tempBuff") ))
			#endif
			iInBuffSize = 4*iInBuffSize;
		}
	}


	// Reitrieve header information from bitstream
	if ( err  < 0 )
	{
		CleanupStack::PopAndDestroy(codedBuffer);
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, codedBuffer") ))
		#endif
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Reading input data fail")))
	}
	else
	{
		err = KErrNone;

		headerInfo = iDevvp->GetHeaderInformationL(EDuCodedPicture,EDuElementaryStream,codedBuffer);

		CleanupStack::PopAndDestroy(tempBuff);
		CleanupStack::PopAndDestroy(codedBuffer);
		iMemDelete++;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::GetHeaderInformationL, mem del, tempBuff & codedBuffer") ))
		#endif

		//Check Header info
		if ( headerInfo )
		{
			//Size in Memory
			iPictureSize = headerInfo->iDisplayedRect.Size();
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Picture Size :width[%d], height[%d]"),iPictureSize.iWidth,iPictureSize.iHeight))

			// Display rect
			iDispRect = headerInfo->iDisplayedRect;
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Displayed image portion: Width[%d] to Height[%d]"),headerInfo->iDisplayedRect.Width(),headerInfo->iDisplayedRect.Height()))

			iDevvp->ReturnHeader(headerInfo);

		}
		else
		{
			PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Get header info fail")))
			err = KErrGeneral;
		}

	}

	// Set size of decoder input buffer
	if ( iCodecType == EH263 )
	{
		if ( (iPictureSize.iWidth <= 176 ) && (iPictureSize.iHeight <= 144) )
		{
			iInBuffSize = KH263MaxCodedSizeQCIF;
		}
		else
		{
			iInBuffSize = KH263MaxCodedSizeCIF;
		}
	}
	else //Mpeg4
	{
		if ( (iPictureSize.iWidth <= 176 ) && (iPictureSize.iHeight <= 144) )
		{
			iInBuffSize = KMP4MaxCodedSizeQCIF;
		}
		else if ( (iPictureSize.iWidth <= 352 ) && (iPictureSize.iHeight <= 288) )
		{
			iInBuffSize = KMP4MaxCodedSizeCIF;
		}

		else
		{
			iInBuffSize = KMP4MaxCodedSizeVGA;
		}

	}

	AssertTIntEqualL(KErrNone, err);
	PRINT((_L("CVDecTestEngine::GetHeaderInformationL, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetCodecType
* Purpose:    Set Cotec type
* Parameters: TVideoCodec aCodec
*
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetCodecType(TVideoCodec aCodec)
{
	iCodecType = aCodec;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetBitstreamCountersL()
* Purpose:
* Parameters:
*
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetBitstreamCountersL()
{
	PRINT((_L("CVDecTestEngine::GetBitstreamCountersL In")))

	CMMFDevVideoPlay::TBitstreamCounters lCounters;

	iDevvp->GetBitstreamCounters(lCounters);

	PRINT((_L("CVDecTestEngine::GetBitstreamCountersL, Lost Packets = %d"),lCounters.iLostPackets))
	PRINT((_L("CVDecTestEngine::GetBitstreamCountersL, Total Packets = %d"), lCounters.iTotalPackets))

	PRINT((_L("CVDecTestEngine::GetBitstreamCountersL Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::PreDecoderBufferBytes()
* Purpose:
* Parameters:
*
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::PreDecoderBufferBytes()
{
	PRINT((_L("CVDecTestEngine::PreDecoderBufferBytes, In")))

	TUint lBufferBytes = iDevvp->PreDecoderBufferBytes();
	PRINT((_L("CVDecTestEngine::PreDecoderBufferBytes, Number of bytes of data in the pre-decoder buffer = %d"), lBufferBytes))

	PRINT((_L("CVDecTestEngine::PreDecoderBufferBytes, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::PictureBufferBytes()
* Purpose:
* Parameters:
*
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::PictureBufferBytes()
{
	PRINT((_L("CVDecTestEngine::PictureBufferBytes, In")))

	TUint buffbyte =  iDevvp->PictureBufferBytes();
	PRINT((_L("CVDecTestEngine::PictureBufferBytes: total amount of memory allocated [%d]"),buffbyte))

	PRINT((_L("CVDecTestEngine::PictureBufferBytes, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetPostProcessTypesL()
* Purpose:    Set combination of postprocessor
*
* Parameters: TUint32 aCombination
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetPostProcessTypesL(TInt aHWDevice, TUint32 aCombination)
{
	PRINT((_L("CVDecTestEngine::SetPostProcessTypesL, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetPostProcessTypesL(iPostProcId,aCombination);
	}
	else
	{
		iDevvp->SetPostProcessTypesL(iDecHWDevId,aCombination);
	}

	PRINT((_L("CVDecTestEngine::SetPostProcessTypesL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetInputCropOptionsL()
* Purpose:    Set input crop
*
* Parameters: TRect aCrop
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetInputCropOptionsL(TInt aHWDevice, TRect aCrop)
{
	PRINT((_L("CVDecTestEngine::SetInputCropOptionsL, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetInputCropOptionsL(iPostProcId,aCrop);
	}
	else
	{
		iDevvp->SetInputCropOptionsL(iDecHWDevId,aCrop);
	}

	PRINT((_L("CVDecTestEngine::SetInputCropOptionsL, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetYuvToRgbOptionsL()
* Purpose:    Set Yuv to Rgb options
*
* Parameters: TYuvToRgbOptions aOptions
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetYuvToRgbOptionsL(TInt aHWDevice, TYuvToRgbOptions aOptions)
{
	PRINT((_L("CVDecTestEngine::SetYuvToRgbOptionsL, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetYuvToRgbOptionsL(iPostProcId, aOptions);
	}
	else
	{
		iDevvp->SetYuvToRgbOptionsL(iDecHWDevId, aOptions);
	}

	PRINT((_L("CVDecTestEngine::SetYuvToRgbOptionsL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetRotateOptionsL()
* Purpose:    Set Rotation
*
* Parameters: TRotationType aRotation
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetRotateOptionsL(TInt aHWDevice, TRotationType aRotation)
{
	PRINT((_L("CVDecTestEngine::SetRotateOptionsL, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetRotateOptionsL(iPostProcId, aRotation);
	}
	else
	{
		iDevvp->SetRotateOptionsL(iDecHWDevId, aRotation);
	}

	PRINT((_L("CVDecTestEngine::SetRotateOptionsL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetScaleOptionsL()
* Purpose:    Set Scale with multiply factor
*
* Parameters: TSize aSize, TBool aAntiAliasFiltering
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetScaleOptionsL(TInt aHWDevice, TSize aSize, TBool aAntiAliasFiltering)
{
	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, In")));

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetScaleOptionsL(iPostProcId, aSize, aAntiAliasFiltering);
	}
	else
	{
		iDevvp->SetScaleOptionsL(iDecHWDevId, aSize, aAntiAliasFiltering);
	}

	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, Scale option is set:width[%d],height[%d]"), aSize.iWidth,aSize.iHeight))

	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, Out")));
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetScaleOptionsL()
* Purpose:    Set Scale with multiply factor
*
* Parameters: TInt aNumFactor,TInt aDenoFactor, TBool aAntiAliasFiltering
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetScaleOptionsL(TInt aNumFactor,TInt aDenoFactor, TBool aAntiAliasFiltering)
{
	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, Out")));

	TInt width =  static_cast<TInt>(iPictureSize.iWidth*aNumFactor/aDenoFactor);
	TInt height = static_cast<TInt>(iPictureSize.iHeight*aNumFactor/aDenoFactor);

	TSize target(width, height);

	iDevvp->SetScaleOptionsL(iPostProcId, target, aAntiAliasFiltering);
	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, Scale option is set:width[%d],height[%d]"),target.iWidth,target.iHeight))

	PRINT((_L("CVDecTestEngine::SetScaleOptionsL, Out")));
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetOutputCropOptionsL()
* Purpose:    Set output crop
*
* Parameters: TRect aCrop
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetOutputCropOptionsL(TInt aHWDevice, TRect aCrop)
{
	PRINT((_L("CVDecTestEngine::SetOutputCropOptionsL, In")));

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetOutputCropOptionsL(iPostProcId, aCrop);
	}
	else
	{
		iDevvp->SetOutputCropOptionsL(iDecHWDevId, aCrop);
	}

	PRINT((_L("CVDecTestEngine::SetOutputCropOptionsL, Out")));
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CommitL()
* Purpose:    commit change of postprocessors after intialization or revert
*
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::CommitL()
{
	PRINT((_L("CVDecTestEngine::CommitL, In")));

	iDevvp->CommitL();

	PRINT((_L("CVDecTestEngine::CommitL, Out")));
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Revert()
* Purpose:    Revert any configuration changes that have not yet been committed
*
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::Revert()
{
	PRINT((_L("CVDecTestEngine::Revert, In")));

	iDevvp->Revert();

	PRINT((_L("CVDecTestEngine::Revert, Out")));
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetClockSource()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetClockSource()
{
	PRINT((_L("CVDecTestEngine::SetClockSource, In")))

	iDevvp->SetClockSource(iClock);

	PRINT((_L("CVDecTestEngine::SetClockSource, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetHrdVbvSpec()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams)
{
	PRINT((_L("CVDecTestEngine::SetHrdVbvSpec, In")))

	iDevvp->SetHrdVbvSpec(aHrdVbvSpec, aHrdVbvParams);

	PRINT((_L("CVDecTestEngine::SetHrdVbvSpec, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetPostProcSpecificOptionsL()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetPostProcSpecificOptionsL(TInt aHWDevice, const TDesC8& aOptions)
{
	PRINT((_L("CVDecTestEngine::SetPostProcSpecificOptionsL, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetPostProcSpecificOptionsL(iPostProcId, aOptions);
	}
	else
	{
		iDevvp->SetPostProcSpecificOptionsL(iDecHWDevId, aOptions);
	}

	PRINT((_L("CVDecTestEngine::SetPostProcSpecificOptionsL, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetScreenClipRegion()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetScreenClipRegion(TRegion& aRegion)
{
	PRINT((_L("CVDecTestEngine::SetScreenClipRegion, In")))

	iDevvp->SetScreenClipRegion(aRegion);

	PRINT((_L("CVDecTestEngine::SetScreenClipRegion, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetPauseOnClipFail()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetPauseOnClipFail(TBool aPause)
{
	PRINT((_L("CVDecTestEngine::SetPauseOnClipFail, In")))

	iDevvp->SetPauseOnClipFail(aPause);

	PRINT((_L("CVDecTestEngine::SetPauseOnClipFail, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::IsPlaying()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::IsPlaying()
{
	PRINT((_L("CVDecTestEngine::IsPlaying, In")))

	TUint playing = iDevvp->IsPlaying();
	PRINT((_L("CVDecTestEngine::IsPlaying, [%d]"), playing))

	PRINT((_L("CVDecTestEngine::IsPlaying, Out")))

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetPictureCounters()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetPictureCounters()
{
	PRINT((_L("CVDecTestEngine::GetPictureCounters, In")))

	CMMFDevVideoPlay::TPictureCounters counters;
	iDevvp->GetPictureCounters(counters);

	PRINT((_L("CVDecTestEngine::GetPictureCounters, iPicturesSkipped, [%d]"), counters.iPicturesSkipped))
	PRINT((_L("CVDecTestEngine::GetPictureCounters, iPicturesDecoded, [%d]"), counters.iPicturesDecoded))
	PRINT((_L("CVDecTestEngine::GetPictureCounters, iPicturesDisplayed, [%d]"), counters.iPicturesDisplayed))
	PRINT((_L("CVDecTestEngine::GetPictureCounters, iTotalPictures, [%d]"), counters.iTotalPictures))

	PRINT((_L("CVDecTestEngine::GetPictureCounters, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::NumFreeBuffers()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::NumFreeBuffers()
{
	PRINT((_L("CVDecTestEngine::NumFreeBuffers, In")))

	TUint playing = iDevvp->NumFreeBuffers();
	PRINT((_L("CVDecTestEngine::NumFreeBuffers, [%d]"), playing))

	PRINT((_L("CVDecTestEngine::NumFreeBuffers, Out")))

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::NumComplexityLevels()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::NumComplexityLevels(TInt aHWDevice)
{
	PRINT((_L("CVDecTestEngine::NumComplexityLevels, In")))

	TUint number = 0;
	if (aHWDevice == EPostProcessor)
	{
		number = iDevvp->NumComplexityLevels(iPostProcId);
	}
	else
	{
		number = iDevvp->NumComplexityLevels(iDecHWDevId);
	}

	PRINT((_L("CVDecTestEngine::NumComplexityLevels, [%d]"), number))

	PRINT((_L("CVDecTestEngine::NumComplexityLevels, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::InputEnd()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::InputEnd()
{
	PRINT((_L("CVDecTestEngine::InputEnd, In")))

	iDevvp->InputEnd();

	PRINT((_L("CVDecTestEngine::InputEnd, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetNewPictureInfo()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetNewPictureInfo(TTimeIntervalMicroSeconds& aEarliestTimestamp,
TTimeIntervalMicroSeconds& aLatestTimestamp)
{
	PRINT((_L("CVDecTestEngine::GetNewPictureInfo, In")))

	TUint numbers = 0;
	iDevvp->GetNewPictureInfo(numbers, aEarliestTimestamp, aLatestTimestamp);
	PRINT((_L("CVDecTestEngine::GetNewPictureInfo, [%d]"), numbers))

	PRINT((_L("CVDecTestEngine::GetNewPictureInfo, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetTimedSnapshotL()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetTimedSnapshotL(const TUncompressedVideoFormat& aFormat,
const TTimeIntervalMicroSeconds& aPresentationTimestamp)
{
	PRINT((_L("CVDecTestEngine::GetTimedSnapshotL, In")))

	iPictureDataSnapshot.iDataFormat = aFormat.iDataFormat;
	iPictureDataSnapshot.iDataSize = iPictureSize;

	PrintUncompressedFormat(aFormat);

	iDevvp->GetTimedSnapshotL(&iPictureDataSnapshot, aFormat, aPresentationTimestamp);

	PRINT((_L("CVDecTestEngine::GetTimedSnapshotL, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetTimedSnapshotL()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetTimedSnapshotL(const TUncompressedVideoFormat& aFormat,
const TPictureId& aPictureId)
{
	PRINT((_L("CVDecTestEngine::GetTimedSnapshotL, In")))

	iPictureDataSnapshot.iDataFormat = aFormat.iDataFormat;
	iPictureDataSnapshot.iDataSize = iPictureSize;

	PrintUncompressedFormat(aFormat);

	iDevvp->GetTimedSnapshotL(&iPictureDataSnapshot, aFormat, aPictureId);

	PRINT((_L("CVDecTestEngine::GetTimedSnapshotL, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CancelTimedSnapshot()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::CancelTimedSnapshot()
{
	PRINT((_L("CVDecTestEngine::CancelTimedSnapshot, In")))

	iDevvp->CancelTimedSnapshot();

	PRINT((_L("CVDecTestEngine::CancelTimedSnapshot, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetSupportedSnapshotFormatsL()
* Purpose:    Set SetHrdVbvSpec
* Note:       This is called before initilaized
* Parameters: THrdVbvSpecification aHrdVbvSpec
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetSupportedSnapshotFormatsL()
{
	PRINT((_L("CVDecTestEngine::GetSupportedSnapshotFormatsL, In")))

	RArray<TUncompressedVideoFormat> formats;
	CleanupClosePushL(formats);

	iDevvp->GetSupportedSnapshotFormatsL(formats);

	for (TUint i = 0; i < formats.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::GetSupportedSnapshotFormatsL, Format No.%d"), i))
		PrintUncompressedFormat(formats[i]);
	}
	CleanupStack::PopAndDestroy(&formats);

	PRINT((_L("CVDecTestEngine::GetSupportedSnapshotFormatsL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetComplexityLevel()
* Purpose:    Set Complexity Level of Decode and postprocessor
* Note:       This is called after initilaized
* Parameters: TInt aDecLevel, TInt aPostProcLevel
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetComplexityLevel(TInt aHWDevice, TInt aLevel)
{
	PRINT((_L("CVDecTestEngine::SetComplexityLevel, In")))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetComplexityLevel(iPostProcId, aLevel);
	}
	else
	{
		iDevvp->SetComplexityLevel(iDecHWDevId, aLevel);
	}

	PRINT((_L("CVDecTestEngine::SetComplexityLevel, Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetComplexityLevelInfo()
* Purpose:    Get number and information of Complexity Level of Decode and postprocessor
* Note:       This is called after initilaized
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetComplexityLevelInfo(TInt aHWDevice, TInt aLevel)
{
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, In")))

	CMMFDevVideoPlay::TComplexityLevelInfo info;

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->GetComplexityLevelInfo(iPostProcId, aLevel, info);
	}
	else
	{
		iDevvp->GetComplexityLevelInfo(iDecHWDevId, aLevel, info);
	}

	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, dec complexy level info[%x]"),info.iOptions))
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, iAvgPictureRate [%f]"),info.iAvgPictureRate))
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, iPictureSize width [%d] height[%d]"),info.iPictureSize.iWidth, info.iPictureSize.iHeight))
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, iRelativeImageQuality [%f]"),info.iRelativeImageQuality))
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, iRequiredMIPS [%d]"),info.iRequiredMIPS))
	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, iRelativeProcessTime [%f]"),info.iRelativeProcessTime))

	PRINT((_L("CVDecTestEngine::GetComplexityLevelInfo, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Redraw()
* Purpose:    Redraw latest picture
* Note:       only applicable when DSA is used and after initilaized
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::Redraw()
{
	PRINT((_L("CVDecTestEngine::Redraw, In")))

	iDevvp->Redraw();

	PRINT((_L("CVDecTestEngine::Redraw, Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetSnapshotL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::GetSnapshotL(TUncompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::GetSnapshotL, In")))

	TInt result;
	TInt err = KErrNone;

	TPictureData picture;

	picture.iDataFormat = aFormat.iDataFormat;
	picture.iDataSize = iPictureSize;

	PRINT((_L("CVDecTestEngine::GetSnapshotL, before new []")))
	TUint8* data = new (ELeave)TUint8[iPictureSize.iWidth*iPictureSize.iHeight*2];
	PRINT((_L("CVDecTestEngine::GetSnapshotL, after new[]")))

	TPtr8 dataPtr(data, iPictureSize.iWidth*iPictureSize.iHeight*2);
	picture.iRawData = &dataPtr;

	CleanupStack::PushL(data);
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::GetSnapshotL, mem alloc, data") ))
	#endif
	PRINT((_L("CVDecTestEngine::GetSnapshotL, before snapshot")))
	result = iDevvp->GetSnapshotL(picture, aFormat);

	PRINT((_L("CVDecTestEngine::GetSnapshotL, snapshot received")))
	if ( !result )
	{
		RFile snapshot;

		TFileName filename;
		TBuf8<128> newFile;
		newFile.Append(iInFileName);
		TBuf16<128> temp2;
		temp2.Copy(newFile);
		PRINT((_L("CVDecTestEngine::GetSnapshotL, Opening the file [%S]"), &temp2))
		newFile.Delete(newFile.Length()-4, 4);
		newFile.Append(_L8("_snapshot"));
		newFile.Append(_L8(".yuv"));
		filename.Copy(newFile);

		TBuf16<128> temp;
		temp.Copy(newFile);

		PRINT((_L("CVDecTestEngine::GetSnapshotL, Opening the file [%S]"), &temp))
		err = snapshot.Replace(iFs, filename, EFileShareExclusive|EFileWrite);

		PRINT((_L("CVDecTestEngine::GetSnapshotL, writing the file")))

		if ( err!= KErrNone )
		{
			PRINT((_L("CVDecTestEngine::GetSnapshotL, snapshot file open failed")))
			snapshot.Close();
			User::Leave(err);
		}
		else
		{
			PRINT((_L("CVDecTestEngine::GetSnapshotL, Picture size %d"),picture.iRawData->Size()));
			PRINT((_L("CVDecTestEngine::GetSnapshotL, Picture length %d"),picture.iRawData->Length()));
			err = snapshot.Write(*(picture.iRawData),picture.iRawData->Size());
			if ( err!= KErrNone )
			{
				PRINT((_L("CVDecTestEngine::GetSnapshotL, snapshot file write failed [%d]"), picture.iRawData->Size()))
				snapshot.Close();
				User::Leave(err);
			}
		}
		PRINT((_L("CVDecTestEngine::GetSnapshotL, Picture size %d"),picture.iRawData->Size()));
		snapshot.Close();

	}
	else
	{
		PRINT((_L("CVDecTestEngine::GetSnapshotL, Error getting snapshot [%d]"), result))
	}
	CleanupStack::PopAndDestroy( data );
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::GetSnapShotL, mem del, ") ))
	#endif
	PRINT((_L("CVDecTestEngine::GetSnapshotL, Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::OpenFileL()
* Purpose:    Open input File
* Note:       DSA is used, output is display
* Parameters: TFileName& aInFileName
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::OpenFileL(TFileName& aInFileName)
{
	PRINT((_L("CVDecTestEngine::OpenFileL, In")))
	TInt err;
	iInFileName.Copy(aInFileName);

	if ( (err = iInFile.Open(iFs, aInFileName, EFileRead | EFileShareReadersOnly)) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::OpenFiles, Input File open Failed")));
		User::Leave(err);
	}
	PRINT((_L("CVDecTestEngine::OpenFileL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::OpenFileL()
* Purpose:    Open input and output File
* Note:       Memory buffer output is used
* Parameters: TFileName& aOutFileName, TFileName& aInFileName
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::OpenFileL(TFileName& aOutFileName, TFileName& aInFileName)
{
	PRINT((_L("CVDecTestEngine::OpenFileL, In")))
	TInt err = KErrNone;

	iInFileName.Copy(aInFileName);
	iOutFileName.Copy(aOutFileName);
	err = iInFile.Open(iFs, aInFileName, EFileRead | EFileShareReadersOnly);

	if ( err != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::OpenFiles, Input File open Failed")));
	}

	else
	{
		iOutFileOpen = ETrue;
		err = iOutFile.Replace(iFs, aOutFileName, EFileShareExclusive|EFileWrite);

		if ( err!= KErrNone )
		{
			PRINT((_L("CVDecTestEngine::OpenFiles, Output File Replace Failed")));
			iInFile.Close();
		}
	}


	AssertTIntEqualL(KErrNone,err);
	PRINT((_L("CVDecTestEngine::OpenFileL, Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CloseFile()
* Purpose:    Close File
* Note:
* Parameters:
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::CloseFile()
{

	iInFile.Close();
	if ( iOutFileOpen )
	{
		iOutFile.Close();

		PRINT((_L("CVDecTestEngine::CloseFile, Output file closed")))
	}

	PRINT((_L("CVDecTestEngine::CloseFile, File Closed")))

	if( iCorruptMarker )
	{
		iFs.Delete( iOutFileName );
	}

}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FrameJump()
* Purpose:    Jump backward or forward a number of frames
* Note:
* Parameters: TInt aNumToJump
* Return:     TInt
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::FrameJump(TInt aNumToJump )
{
	PRINT((_L("CVDecTestEngine::FrameJump, In")))
	TInt err = KErrNone;

	if ( iState == ERunning )
	{
		if ( iSynchronized )
		{
			iClock->Suspend();  //Stop Clock source
		}


		iDevvp->Pause();
		iState = EPaused;
		PRINT((_L("CVDecTestEngine::Pause(), Paused")))


		iFrameJump = aNumToJump;


		if( iFrameJump < 0 )
		{
			TInt startFrame = iSentBuffCount + iFrameJump;
			if( startFrame < 0 )
			{
				PRINT((_L("CVDecTestEngine::FrameJump, bad argument")))
				return KErrArgument;
			}
			else
			{
				iFrameJump = startFrame;
			}
		}
		TInt beginningPos = 0;
		iInFile.Seek(ESeekStart, beginningPos);
		RArray<TInt> vFramesizes;
		ListFrameSizeL( vFramesizes );

		if( iFrameSizeList.Count() > iFrameJump )
		{
			TInt startPosition = 0;
			for( TInt frameCounter = 0; frameCounter < iFrameSizeList.Count();
			frameCounter++ )
			{
				startPosition += iFrameSizeList[ frameCounter ];
			}
			iInFile.Seek(ESeekStart,startPosition);
		}
		else
		{
			err = KErrGeneral;
			PRINT((_L("CVDecTestEngine::FrameJump, iFrameSizeList.Count value:[%ld]"),iFrameSizeList.Count()))
		}

	}
	else
	{
		err = KErrGeneral;
	}

	PRINT((_L("CVDecTestEngine::FrameJump, iFrameJump value:[%ld]"),iFrameJump))
	PRINT((_L("CVDecTestEngine::FrameJump, Out")))

	return err;
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetFrameMatch()
* Purpose:    The test case fails if input and output frame number doesn't
*             match
* Note:
* Parameters: TBool aMatch
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetFrameMatch(TBool aMatch)
{

	iFrameMatch = aMatch;

	PRINT((_L("CVDecTestEngine::SetFrameMatch, frame match is set")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Initialize()
* Purpose:    Initilize Decoder/Postprocessor
* Note:
* Parameters: TBool aSynchronized
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::Initialize()
{
	PRINT((_L("CVDecTestEngine::Initialize, In")))

	iError = KErrNone;

	// Do Initialization
	iDevvp->Initialize();
	PRINT((_L("CVDecTestEngine::Initialize, Initialize() returned")))
	if ( (iState != EInitialized) && (iError == KErrNone ) )
	{
		iScheduler = new (ELeave) CActiveSchedulerWait;
		iMemAlloc++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::Initialize, mem alloc, iScheduler") ))
		#endif
		iRunning = ETrue;
		iScheduler->Start();
	}
	PRINT((_L("CVDecTestEngine::Initialize, out: error[%d]"),iError))
	return iError;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::InitializeAndDelete()
* Purpose:    Initilize Decoder/Postprocessor
* Note:
* Parameters: TBool aSynchronized
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::InitializeAndDelete()
{
	PRINT((_L("CVDecTestEngine::InitializeAndDelete, In")))

	iDeleteDecoderFromInitComp = ETrue;

	TInt err = Initialize();

	PRINT((_L("CVDecTestEngine::InitializeAndDelete, Out")))
	return err;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Start()
* Purpose:    Start Decoding/Postprocessing
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::Start(TBool aInputEnd)
{

	TInt err = KErrNone;
	iDisableInputEnd = aInputEnd;
	
	PRINT((_L("CVDecTestEngine::Start, Devvideo play started")))

	// Start DSA
	if ( iDirectScreenAccess && !iDsaStarted )
	{
		TRAPD(err, StartDirectScreenAccessL());
		if (err != KErrNone)
		{
			return err;
		}

	}

    // Start DevVideoPlay
    iDevvp->Start();
    iState = ERunning;
    
	//Reset Clock at Stream Start
	if ( iSynchronized )
	{
		TTimeIntervalMicroSeconds offset(KStartOffset);
		Int64 time = offset.Int64();
		iClock->Reset(offset);
	}


	// Start Data transfer
	iDecTestAO->RequestData();

	PRINT((_L("CVDecTestEngine::Start, Start data transfer - start () out")))
	return err;

}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Stop()
* Purpose:    Stop Decoding/Postprocessing
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::Stop()
{

	PRINT((_L("CVDecTestEngine::Stop, In ")))

	if ( (iState == ERunning ) || (iState == EPaused) )
	{
		//Stop Devvideoplay
        iDevvp->Stop();
		iState = EStopped;
		PRINT((_L("CVDecTestEngine::Stop, Stopped ")))
	}

	//Stop DSA
	if ( iDirectScreenAccess && iDsaStarted )
	{
		iDevvp->AbortDirectScreenAccess();
		iDsaStarted = EFalse;
		PRINT((_L("CVDecTestEngine::Stop, DSA is aborted ")))

		PRINT((_L("CVDecTestEngine::VDecTestStartL, the number of Pictures sent: [%d]"),iSentBuffCount))

	}
	else if ( iInputEnd )
	{
		if ( iSentBuffCount != (iReturnedBuffCount + iPictureLoss) )
		{
			if( iFrameMatch )
			{
				PRINT((_L("CVDecTestEngine::VDecTestStopL, the number of Pictures sent and returned does not match: ")))
				return KErrGeneral;
			}
			PRINT((_L("CVDecTestEngine::VDecTestStopL, the number of Pictures sent and returned does not match: ")))
			PRINT((_L("The number of pictures, sent : [%d], returned:[%d] "),iSentBuffCount, iReturnedBuffCount ))
		}
	}

	PRINT((_L("CVDecTestEngine::Stop, Out")))
	return iError;
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::TearDown()
* Purpose:    Cleanup resources
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::TearDown()
{
	PRINT((_L("CVDecTestEngine::TearDown(),In")))

	if( iInstantFpsEnabled )
	{
		iFpsFile.Close();
		PRINT((_L("CVDecTestEngine::CloseFile, FPS Output file closed")))
	}

	if ( iDsaStarted )
	{
		iDevvp->AbortDirectScreenAccess();
		PRINT((_L("CVDecTestEngine::TearDown(), Dsa aborted")))
	}

	// Delete Devvideo play
	if ( iDevvp )
	{
		delete iDevvp;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::TearDown, mem del, iDevvp") ))
		#endif
		iDevvp = NULL;
		iState = EStateNone;
		PRINT((_L("CVDecTestEngine::TearDown(),DevVideo deleted")))
	}


	//Postproc input buffer
	if ( !iCIBuffMgmtOn && iRawInBuffer)
	{
		delete (TUint8*)iRawInBuffer->iData.iRawData->Ptr();
		delete iRawInBuffer->iData.iRawData;
		delete iRawInBuffer;
		iMemDelete += 3;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::TearDown, mem del, iRawInBuffer & its 2 pointers") ))
		#endif
		iRawInBuffer = NULL;
	}

	if ( iScreenDevice )
	{
		delete iScreenDevice;
		iScreenDevice = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::TearDown, mem del, iScreenDevice") ))
		#endif
		RFbsSession::Disconnect();
	}

	delete iDataChunk;
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::TearDown, mem del, iDataChunk") ))
	#endif

	iDataChunk = NULL;

	iFrameSizeList.Close();

	iInstantFpsList.Close();

	iFs.Close();

	REComSession::FinalClose();
	PRINT( (_L("CVDecTestEngine::TearDown(), mem allocated: %d "), iMemAlloc))
	PRINT( (_L("CVDecTestEngine::TearDown(), mem deleted: %d "), iMemDelete))
	PRINT((_L("CVDecTestEngine::TearDown(),Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FillAndSendBufferL()
* Purpose:    Fill data into input buffer and send to devvideoplay
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::FillAndSendBufferL()
{
	PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), In")))
	TInt err = KErrNone;

	if ( iInputEnd)
	{
		iLastFrame = EFalse;
		//iCodedInBuffer->iOptions =  TVideoInputBuffer::EDecodingTimestamp;
		//iCodedInBuffer->iDecodingTimestamp = iSentBuffCount+2;
		//ReadOneCodedPicture(iCodedInBuffer, 0); // Read compressed data of one picture from file
		//iDevvp->WriteCodedDataL(iCodedInBuffer);  // Input Data Sent to Decoder
		if (!iDisableInputEnd)
		{
			iDevvp->InputEnd();
			PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Input End")))
		}
		else
		{
			MdvpoStreamEnd();
			PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Stream end")))
		}
		return;
	}

	if ( iDecHWDevId )
	{
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), iDecHWDevID found")))
		if (  iFrameSizeList.Count() > 0 )
		{
			if (iFrameSizeList.Count() <= iSentBuffCount+1)
			{
				iInputEnd = ETrue;
			}
			TInt size = iFrameSizeList[iSentBuffCount];
			err = ReadOneCodedPicture(iCodedInBuffer, size); // Read compressed data of one picture from file

			PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), 1 picture read")))

			if( iFrameDropMarker )
			{
				if( iFrameDropCounter >= iFrameDropInterval )
				{
					TInt dropNum = 0;
					for( ; ;)
					{
						TInt size = iFrameSizeList[iSentBuffCount++];
						err = ReadOneCodedPicture(iCodedInBuffer, size); // Read compressed data of one picture from file
						dropNum++;
						iFrameDropNumCounter++;
						if( iFrameDropNumCounter >= iFrameDropNum )
						{
							iFrameDropCounter = 0;
							iFrameDropNumCounter = 0;
							break;
						}
					}
					PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), [%d] frame(s) dropped"), dropNum))
				}
			}


		}
		else
		{
			err = ReadOneCodedPicture(iCodedInBuffer); // Read compressed data of one picture from file
			PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), one picture read, err: [%d]"), err))

			if( iFrameDropMarker )
			{
				if( iFrameDropCounter >= iFrameDropInterval )
				{
					TInt dropNum  = 0;
					for( ; ;)
					{
						err = ReadOneCodedPicture(iCodedInBuffer);
						dropNum++;
						iFrameDropNumCounter++;
						if( iFrameDropNumCounter >= iFrameDropNum )
						{
							iFrameDropCounter = 0;
							iFrameDropNumCounter = 0;
							break;
						}
					}
					PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), [%d] frame(s) dropped"), dropNum))
				}
			}


		}
	}
	else  // Postproc input case
	{
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), iDecHWDevID not found")))
		err = ReadRawPicture();	// Read raw data for one picture
		if( iFrameDropMarker )
		{
			if( iFrameDropCounter >= iFrameDropInterval )
			{
				TInt dropNum = 0;
				for( ; ;)
				{
					err = ReadRawPicture();	// Read raw data for one picture
					dropNum++;
					iFrameDropNumCounter++;
					if( iFrameDropNumCounter >= iFrameDropNum )
					{
						iFrameDropCounter = 0;
						iFrameDropNumCounter = 0;
						break;
					}
				}
				PRINT((_L("CVDecTestEngine::FillAndSendBufferL(), [%d] raw picture(s) dropped"), dropNum))
			}
		}
	}
	if(err == 0){
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Empty buffer read skipping")))
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Out")))
	}
	else if ( err > 0 )
	{
		// Corrupt the input if user requires
		/*
		if( iCorruptMarker )
		{

		CorruptEngineL();

		}
		*/
		// End of the corrupting operation

		if ( iDecHWDevId )
		{

			iCodedInBuffer->iOptions =  TVideoInputBuffer::EDecodingTimestamp;

			if ( iPostProcId )
			{
				iCodedInBuffer->iOptions |= TVideoInputBuffer::EPresentationTimestamp;
			}



			if ( iSynchronized ) // Clock source is set >> real-time synchronization
			{
				iCodedInBuffer->iDecodingTimestamp = iTimeStamp;
				if ( iPostProcId )
				{
					iCodedInBuffer->iPresentationTimestamp = iTimeStamp;
				}


				PRINT((_L("CVDecTestEngine::FillAndSendBufferL, iTimestamp added:[%ld]"),iTimeStamp))
				iTimeStamp += iFrameTimeInterval;
			}
			else // Clock source is not set
			{
				iCodedInBuffer->iDecodingTimestamp = iSentBuffCount;
				if ( iPostProcId )
				{
					iCodedInBuffer->iPresentationTimestamp = iSentBuffCount;
				}

			}

			TInt substreamID = 0;
			//      	    iCodedInBuffer->iUser = (TAny*)(&substreamID);
			if(iCodedInBuffer->iUser != NULL ){
				*((TInt*)iCodedInBuffer->iUser) = substreamID;
			}
			#if 0
			TUint32 msgSize = iCodedInBuffer->iData.Length();
			TUint8* tmpPtr = const_cast<TUint8 *>(iCodedInBuffer->iData.Ptr());
			while(msgSize > 0)
			{
				if(msgSize >= 16)
				{
					PRINT((_L("TX:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x"),tmpPtr[0],tmpPtr[1],tmpPtr[2],tmpPtr[3],tmpPtr[4],tmpPtr[5],tmpPtr[6],tmpPtr[7],tmpPtr[8],tmpPtr[9],tmpPtr[10],tmpPtr[11],tmpPtr[12],tmpPtr[13],tmpPtr[14],tmpPtr[15]))
					msgSize -= 16;
					tmpPtr += 16;
				}
				else if(msgSize >= 4)
				{
					PRINT((_L("TX:%02x,%02x,%02x,%02x"),tmpPtr[0],tmpPtr[1],tmpPtr[2],tmpPtr[3]))
					msgSize -= 4;
					tmpPtr += 4;
				}
				else
				{
					for(TUint i = 0; i < msgSize; i++)
					{
						PRINT((_L("TX: 0x%02x"),*tmpPtr++))
					}
					msgSize = 0;
				}
			}
			#endif

			iDevvp->WriteCodedDataL(iCodedInBuffer);  // Input Data Sent to Decoder
		    iCodedInBuffer = NULL;
		}
		else
		{

			iRawInBuffer->iData.iDataSize.SetSize(iPictureSize.iWidth,iPictureSize.iHeight);

			iRawInBuffer->iOptions |= (TVideoPicture::ETimestamp);

			if ( iSynchronized )
			{
				//TTimeIntervalMicroSeconds tsValue(iTimeStamp);
				iRawInBuffer->iTimestamp = iTimeStamp;
				iTimeStamp += iFrameTimeInterval;
				PRINT((_L("CVDecTestEngine::FillAndSendBufferL, iTimestamp added:[%ld]"),iTimeStamp))
			}
			else
			{
				iRawInBuffer->iTimestamp = iSentBuffCount;
			}


			PRINT((_L("CVDecTestEngine::FillAndSendBufferL, data size: w[%d] ,h[%d]"),iRawInBuffer->iData.iDataSize.iWidth, iRawInBuffer->iData.iDataSize.iHeight))
			iDevvp->WritePictureL(iRawInBuffer);  // Input Data Sent to PostProcessor


			if (iCIBuffMgmtOn)
			{
				iRawInBuffer = NULL;
			}

		}

		iFrameDropCounter++;
		iSentBuffCount++;
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Sent Frame: [%d] "),iSentBuffCount))

	}
	else  //Error case
	{
		iError = err;
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Error in reading data: [%d] "),iError))
		iInputEnd = ETrue;
		iDevvp->InputEnd();
		return;
	}

	// if ( iFrameMeasurement )
	//    return;

	if (iInputEnd)
	{
		iLastFrame = ETrue;
		PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Input end: sending last frame 0 ")))
		iDecTestAO->RequestData(); // Next Input Data for Decoder / PostProc
		return;
	}

	if ( iState == ERunning )
	{
		if ( iDecHWDevId )
		{
			if (iSetFastClient)
			{
				PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Fast client request")))
				iDecTestAO->RequestData(); // Next Input Data for Decoder / PostProc
			}
			else
			{
				if ( iDevvp->NumFreeBuffers() )
				{
					iDecTestAO->RequestData(); // Next Input Data for Decoder / PostProc
				}
				else
				{
					PRINT((_L("CVDecTestEngine::FillAndSendBufferL, No input buffer available now")))
					iNoBuffAvailable++;
				}
			}
		}
	}


	PRINT((_L("CVDecTestEngine::FillAndSendBufferL, Out")))

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetInputBufferL()
* Purpose:    Ritreave decoder input buffer
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::GetInputBufferL()
{
	TBool buffer = EFalse;

	if ( iState == ERunning )
	{
		if ( iDecHWDevId )
		{
			iCodedInBuffer = iDevvp->GetBufferL(iInBuffSize);
			if ( iCodedInBuffer )
			{
				buffer = ETrue;
			}

		}
		else if ( iCIBuffMgmtOn )
		{
			PRINT((_L("CVDecTestEngine::GetInputBufferL(), Getting Input buffer(CI)...")))
			#ifdef __CI_HEADERS__
			TVideoPicture* inputBuff = iCIBuffMgmt->MmvbmGetBufferL(iPictureSize);
			if ( inputBuff )
			{
				buffer = ETrue;
				iRawInBuffer = inputBuff;
			}
			#endif
		}
	}

	if ( buffer )
	{
		PRINT((_L("CVDecTestEngine::GetInputBufferL(), Got Input buffer")))
		FillAndSendBufferL();
	}
	else
	{
		PRINT((_L("CVDecTestEngine::GetInputBufferL(), Couldn't get input buffer ")))
	}

}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadOneCodedPicture()
* Purpose:    Read one coded picture into decoder input buffer
* Note:
* Parameters: TVideoInputBuffer* aCodedInBuffer
* Return:     TInt
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer)
{
	PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, In")))
	TInt buffLength = iInBuffSize;
	TBool found = EFalse;
	TInt pos =0;
	TUint8* testData =NULL;
	TInt err = KErrNone;

	//Reads VC1 frame
	if ( iCodecType == EVC1 )
	{
		PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, VC1")))
		return LoadNextFrameL(aCodedInBuffer);
	}

	if ( aCodedInBuffer->iData.MaxLength() < buffLength )
	{
		err = KErrOverflow;
		PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Buffer is small, [%d]Byte"),aCodedInBuffer->iData.MaxLength()))

		return err;
	}

	aCodedInBuffer->iData.SetLength(0);

	//Buffer is not full,  reset size of buffer
	if ( aCodedInBuffer->iData.Length() < buffLength)
	{
		buffLength = aCodedInBuffer->iData.Length();
	}

	testData = const_cast<TUint8*>(aCodedInBuffer->iData.Ptr());

	TUint8* iReadDataChunkTemp = iReadDataChunk;

	TUint8 byte = 0;

	//Check the length of one coded picture
	if (iCodecType == EH263)
	{
		TBool firstStartCode = EFalse;
		while (!iInputEnd && !found)
		{
			TInt zeroCounter = 0;
			while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
			{
				zeroCounter++;
				pos++;
			}
			pos++;
			if ((zeroCounter >= 2) && ((byte & 0xFC) == 0x80))
			{
				if (!firstStartCode)
				{
					firstStartCode = ETrue;
				}
				else
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of h263 picture found")))
					found = ETrue;
				}
			}
		}
		if (iInputEnd)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, inputEndFound, %d"), pos))
			pos--;
		}
		else
		{
			pos -= KScLength;
		}
		iReadDataChunk = iReadDataChunkTemp;
		ReadDataToBuffer(aCodedInBuffer, pos);

	}
	else if ( iCodecType == EMPEG4 )
	{
		TBool firstStartCode = EFalse;
		while (!iInputEnd && !found)
		{
			TInt zeroCounter = 0;
			while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
			{
				zeroCounter++;
				pos++;
			}
			pos++;
			if ((zeroCounter >= 2) && (byte == 0x01))
			{
				byte = ReadByteFromFile();

				pos++;
				if (byte == 0xB6 && !firstStartCode)
				{
					firstStartCode = ETrue;
				}
				else if (byte != 0xB1 && firstStartCode)
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of mpeg-4 picture found [%d]"), pos-4))
					found = ETrue;
				}
			}
		}
		if (iInputEnd)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, inputEndFound, %d"), pos))
			pos--;
		}
		else
		{
			pos -= 4;
		}
		iReadDataChunk = iReadDataChunkTemp;
		ReadDataToBuffer(aCodedInBuffer, pos);
	}
	else if ( iCodecType == EVP6 )
	{
		TUint dataCount = 0;
		if (iSentBuffCount == 0)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, First VP6 frame")))
			//Read signature and version
			for (TUint i=0; i<5; i++)
			{
				byte = ReadByteFromFile();
			}
			TUint32 offset = (TUint32)(ReadByteFromFile())<< 24 | (TUint32)(ReadByteFromFile())<< 16 |
			(TUint32)(ReadByteFromFile())<< 8 | (TUint32)(ReadByteFromFile());

			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, offset %d"), offset))
			//Jump data after offset
			for (TUint i=0; i< offset-9; i++ )
			{
				byte = ReadByteFromFile();
			}
			dataCount += 9+offset-9;
			//Read previous tag size
			for (TUint i=0; i< 4; i++ )
			{
				byte = ReadByteFromFile();
			}

		}
		if (!iInputEnd)
		{

			TUint8 tagType = ReadByteFromFile();

			if (!iInputEnd)
			{

				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, tagType %d"), tagType))
				dataCount += 5;
				while (tagType != 9 && !iInputEnd)
				{
					TUint32 a = (TUint32)(ReadByteFromFile());
					TUint32 b = (TUint32)(ReadByteFromFile());
					TUint32 c = (TUint32)(ReadByteFromFile());
					TUint32 dataSize =  (a << 16) | (b << 8) | (c);
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, dataSize %d"), dataSize))
					ReadDataToBuffer(aCodedInBuffer, dataSize+7, ETrue);
					//Previous offset
					for (TUint i=0; i< 4; i++ )
					{
						byte = ReadByteFromFile();
					}
					tagType = ReadByteFromFile();
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, tagType %d"), tagType))
					dataCount += dataSize+15;
				}
				if(!iInputEnd){
				TUint32 a = (TUint32)(ReadByteFromFile());
				TUint32 b = (TUint32)(ReadByteFromFile());
				TUint32 c = (TUint32)(ReadByteFromFile());
				TUint32 dataSize =  (a << 16) | (b << 8) | (c);
				//Discard timestamp and streamID
				for (TUint i=0; i< 7; i++ )
				{
					byte = ReadByteFromFile();
				}
				//Frametype and codec ID
				byte = ReadByteFromFile();
				//Read VP6 frame to buffer
				ReadDataToBuffer(aCodedInBuffer, dataSize-1);
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, VP6 frame size %d"), dataSize))
				dataCount += dataSize+10;
				//Read previous tag size
				for (TUint i=0; i< 4; i++ )
				{
					byte = ReadByteFromFile();
				}
				}
				else{
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, unknown tags at the end of vp6 stream, discard")))
				dataCount = 0;
				}
			}
		}
		if (iInputEnd)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, inputEndFound")))
			//aCodedInBuffer.iData->Zero();
		}
		return dataCount;
	}
	else if ( iCodecType == EFLV )
	{
		// FLV fileformat has 3 first bytes 'FLV'. Detect if file has container.
		PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, find out bitstream type %d == %d."),iFLVFileType,EFLVNotSet));


		if(iFLVFileType == EFLVNotSet)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, find out bitstream type.")));

			byte = ReadByteFromFile();
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, %02x"),byte));
			TUint8 byte2 = ReadByteFromFile();
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, %02x"),byte2));
			TUint8 byte3 = ReadByteFromFile();
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, %02x"),byte3));
			if ( (byte == 0x46) && (byte2 == 0x4C) && (byte3 == 0x56) )
			{
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, file with container detected.")));
				iFLVFileType = EFLVContainer;
			}
			else
			{
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, file raw bitstream detected.")));
				iFLVFileType = EFLVRaw;
			}
			iReadDataChunk = iReadDataChunkTemp;
		}

		if (iFLVFileType == EFLVContainer)
		{
			// FLV frames have 00 00 x2 00 00 8x startcode, parse it.
			TBool firstStartCode = EFalse;
			TBool frameHeaderEnd = EFalse;
			TBool frameHeader = EFalse;
			while (!iInputEnd && !found)
			{
				TInt zeroCounter = 0;
				while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
				{
					zeroCounter++;
					pos++;
				}
				pos++;
				if ((zeroCounter >= 2) && ((byte & 0xF8) == 0x80) && frameHeaderEnd)
				{
					// FLV frames have some bytes in the beginning which needs to be removed
					if (!frameHeader)
					{
						iReadDataChunk = iReadDataChunkTemp;
						ReadDataToBuffer(aCodedInBuffer, pos-3);

						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, [%d] bytes of flash video frame header removed."), (iReadDataChunk - iReadDataChunkTemp)));
						iReadDataChunkTemp = iReadDataChunk;
						zeroCounter = 0;
						pos = 0;
						frameHeader = ETrue;
					}
					else if (!firstStartCode)
					{
						firstStartCode = ETrue;
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, start of flash video picture found [%d]."), pos-3))

					}
					else
					{
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of flash video picture found [%d]"), pos-3))
						found = ETrue;
					}
				}
				else if ((zeroCounter >= 2) && ((byte & 0x03) == 0x02))
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, found frame end code at [%d]"), pos-3))

					frameHeaderEnd = ETrue;
				}
				else
				{
					frameHeaderEnd = EFalse;
				}
			}
			if (iInputEnd)
			{
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, inputEndFound, %d"), pos))
				pos--;

				// FLV file has four bytes which needs to be removed in the end
				pos -= 4;
			}
			else
			{
				// Go back to beginning of start code (3 bytes)
				pos -= 3;
				// Go back to beginning of FLV file frame header(16 bytes)
				pos -= 16;
			}
			iReadDataChunk = iReadDataChunkTemp;
			ReadDataToBuffer(aCodedInBuffer, pos);
		}
		// Raw flash bitstream file
		else
		{
			TBool firstStartCode = EFalse;
			while (!iInputEnd && !found)
			{
				TInt zeroCounter = 0;
				while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
				{
					zeroCounter++;
					pos++;
				}
				pos++;
				if ((zeroCounter >= 2) && ((byte & 0xF8) == 0x80))
				{
					if (!firstStartCode)
					{
						firstStartCode = ETrue;
					}
					else
					{
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of flash video picture found [%d]"), pos-3))

						found = ETrue;
					}
				}
			}
			if (iInputEnd)
			{
				PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, inputEndFound, %d"), pos))
				pos--;
			}
			else
			{
				pos -= 3;
			}
			iReadDataChunk = iReadDataChunkTemp;
			ReadDataToBuffer(aCodedInBuffer, pos);
		}
	}
	else if ( iCodecType == EAVC )
	{
		if (iSetAUFormat)
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, AU format case")))
			TBool auFound = EFalse;
			TBool firstMB = EFalse;
			TUint32 nalUnits = 0;
			TUint32 totalData = 0;
			iNalOffsetArray = new TUint32[KMaxNumberOfNals];
			iNalLengthArray = new TUint32[KMaxNumberOfNals];
			iMemAlloc += 2;
			TInt previousNal = 0;
			while (!auFound)
			{
				TBool firstStartCode = EFalse;
				// Check if start code of Visual Object Sequence
				pos = 0;
				TInt nalStart = 0;
				while (!iInputEnd && !found && !auFound)
				{
					TInt zeroCounter = 0;
					while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
					{
						zeroCounter++;
						pos++;
					}
					pos++;
					if ((zeroCounter >= 2) && (byte  == 0x01))
					{
						if (!firstStartCode)
						{
							firstStartCode = ETrue;
							nalStart = pos - KScLength;
							iNalOffsetArray[nalUnits] = pos+totalData;

							byte = ReadByteFromFile();
							pos++;

							if (((byte & 0x1F) == 7) && ( previousNal == 5 || previousNal == 1 ) )
							{
								auFound = ETrue;
								pos -= 4;
								GoBackInFile(4);
							}
							else if (((byte & 0x1F) == 5) || ((byte & 0x1F) == 1))
							{
								byte = ReadByteFromFile();
								pos++;
								if (byte >= 128)
								{
									if (firstMB)
									{
										//NAL that belongs to the next AU
										PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Next AU Nal Found")))
										auFound = ETrue;
										pos -= 5;
										GoBackInFile(5);
									}
									else
									{
										PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, First MB")))
										firstMB = ETrue;
									}
								}
							}
							previousNal = byte & 0x1F;

						}
						else
						{
							PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, NAL start: [%d]"), nalStart ))
							PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of h264 NAL found at [%d]"), pos ))

							found = ETrue;
							pos -= KScLength;
							GoBackInFile(KScLength); //go back before next start code
						}
					}
				}

				if (!found && iInputEnd)
				{
					auFound = ETrue;
					if ( nalUnits == 0 )
					{
						totalData = pos;
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture: pos: %d"),pos))
						iNalLengthArray[nalUnits] = pos;
						nalUnits++;
					}
				}
				found = EFalse;
				if (!auFound)
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Searching next NAL")))
					totalData += pos;
					iNalLengthArray[nalUnits] = pos - 3 - nalStart;
					nalUnits++;
				}
				else
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, AU Found")))
					iReadDataChunk = iReadDataChunkTemp;
					ReadDataToBuffer(aCodedInBuffer, totalData);
					pos = totalData; //Set amount of data read from a file
					//Optional filler data to align the header at a 32-bit boundary
					while ((totalData % 4) != 0)
					{
						testData[totalData] = 0;
						totalData++;
					}
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Casting headerpointer")))
					TUint8* headerData = reinterpret_cast<TUint8*>(const_cast<TUint8*>(aCodedInBuffer->iData.Ptr()));
					TUint headerStart = totalData; //changing 8bit index to 32bit
					for (TUint i = 0; i < nalUnits; i++)
					{
						*((TUint32*)&headerData[headerStart]) = static_cast<TUint32>(iNalOffsetArray[i]);
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture: offset [%d] | [%d] "), iNalOffsetArray[i], headerData[headerStart]))
						*((TUint32*)&headerData[headerStart+4]) = static_cast<TUint32>(iNalLengthArray[i]);
						PRINT((_L("CVDecTestEngine::ReadOneCodedPicture: size [%d] | [%d][%d][%d][%d] "), iNalLengthArray[i], headerData[headerStart+4],headerData[headerStart+5],headerData[headerStart+6],headerData[headerStart+7]))
						headerStart += 8;
					}
					*((TUint32*)&headerData[headerStart]) = static_cast<TUint32>(nalUnits);
					headerStart += 4;
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, deleting arrays")))
					delete iNalLengthArray;
					iNalLengthArray = NULL;
					iMemDelete++;
					delete iNalOffsetArray;
					iNalOffsetArray = NULL;
					iMemDelete++;
					#ifdef __MEM_CHECK_
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, mem del, iNalLengthArray") ))
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, mem del, iNalOffsetArray") ))
					#endif

					/*for (TUint i = 0; i < 100; i++)
					{
					PRINT((_L("test data End [%d]"), testData[pos-100+i]))
					} */

					//Set Size to length of the header + one picure, and reset position to be read next time
					aCodedInBuffer->iData.SetLength(headerStart);
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture,length of frame: [%d] "),pos))
					//TInt tempLength = pos - buffLength;
					//err = iInFile.Seek(ESeekCurrent,tempLength);
					return pos;
				}
			}
		}
		else
		{
			PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Bytestream case")))
			TBool auFound = EFalse;
			TBool firstMB = EFalse;
			TUint32 totalData = 0;
			TInt previousNal = 0;
			while (!auFound)
			{
				// Check if start code of Visual Object Sequence
				TBool firstStartCode = EFalse;
				pos = 0;
				TInt nalStart = 0;
				while (!iInputEnd && !found)
				{
					TInt zeroCounter = 0;
					while ((byte = ReadByteFromFile()) == 0x00 && !iInputEnd)
					{
						zeroCounter++;
						pos++;
					}
					pos++;
					if ((zeroCounter >= 2) && (byte  == 0x01))
					{
						if (!firstStartCode)
						{
							firstStartCode = ETrue;
							nalStart = pos-KScLength;
							byte = ReadByteFromFile();
							pos++;

							if (((byte & 0x1F) == 7) && ( previousNal == 5 || previousNal == 1 ) )
							{
								auFound = ETrue;
								pos -= 4;
								GoBackInFile(4);
							}
							else if (((byte & 0x1F) == 5) || ((byte & 0x1F) == 1))
							{
								byte = ReadByteFromFile();
								pos++;
								if (byte >= 128)
								{
									if (firstMB)
									{
										//NAL that belongs to the next AU
										PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Next AU Nal Found")))
										auFound = ETrue;
										pos -= 5;
										GoBackInFile(5);
									}
									else
									{
										PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, First MB")))
										firstMB = ETrue;
									}
								}
							}
							previousNal = byte & 0x1F;

						}
						else
						{
							PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, NAL start: [%d]"), nalStart ))
							PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, end of h264 NAL found at [%d]"), pos ))
							found = ETrue;
							pos -= KScLength;
							GoBackInFile(KScLength); //go back before next start code
						}
					}
				}

				if (!found && iInputEnd)
				{
					totalData += pos;
					auFound = ETrue;
				}
				found = EFalse;
				if (!auFound)
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Searching next NAL")))
					totalData += pos;
				}
				else
				{
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, AU Found")))
					iReadDataChunk = iReadDataChunkTemp;
					ReadDataToBuffer(aCodedInBuffer, totalData);
					pos = totalData; //Set amount of data read from a file
					PRINT((_L("CVDecTestEngine::ReadOneCodedPicture,length of frame: [%d] "),pos));
					return pos;
				}
			}

		}
	}
	else
	{
		err = KErrNotSupported;
		PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, codec type unknown")))
	}

	if (err == KErrNone)  // wk45 for listing frame size
	err = pos;

	PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Out")))
	return err;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadOneCodedPicture()
* Purpose:    Read one coded picture into decoder input buffer, frame size is known
* Note:
* Parameters: TVideoInputBuffer* aCodedInBuffer TInt Size
* Return:     TInt
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer, TInt aSize)
{
	TInt err = KErrNone;

	err = iInFile.Read(aCodedInBuffer->iData, aSize);
	PRINT((_L("CVDecTestEngine::ReadOneCodedPicture, Frame Size"),aSize ))

	if (err == KErrNone)
	{
		err = aSize;
	}

	if (aSize == 0)
	{
		iInputEnd = ETrue;
	}

	return err;
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadRawPicture()
* Purpose:    Read one raw picture into postprocessor input buffer
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::ReadRawPicture()
{
	TInt err = KErrNone;

	PRINT((_L("CVDecTestEngine::ReadRawPicture, Maximum data length in video picture: [%d]"),iRawInBuffer->iData.iRawData->MaxLength()))
	iRawInBuffer->iData.iRawData->Set((TUint8*)iRawInBuffer->iData.iRawData->Ptr(), 0, iInBuffSize);


	err = iInFile.Read(*(iRawInBuffer->iData.iRawData),iInBuffSize);
	PRINT((_L("CVDecTestEngine::ReadRawPicture, data length:[%d]"),iRawInBuffer->iData.iRawData->Length()))

	if ( !err )
	{
		if ( (iRawInBuffer->iData.iRawData->Length()) == 0)
		{
			iInputEnd = ETrue;
		}
	}

	PRINT((_L("CVDecTestEngine::ReadRawPicture, Out error:[%d]"),err))
	return err;

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetReturnedPictureL
* Purpose:    Retrieve output picuture
* Note:       Memory buffer output is used
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::GetReturnedPictureL()
{
	PRINT((_L("CVDecTestEngine::GetReturnedPictureL, In")))
	iOutBuffer = iDevvp->NextPictureL();

	if ( iOutBuffer )
	{
		PRINT((_L("CVDecTestEngine::GetReturnedPictureL, data Size in Byte:[%d]"),iOutBuffer->iData.iRawData->Size() ))
		iReturnedBuffCount++;
		SaveAndReturnPicture();
	}
	PRINT((_L("CVDecTestEngine::GetReturnedPictureL, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SaveAndReturnPicture()
* Purpose:    Store output data into a file
* Note:       Memory buffer output is used
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SaveAndReturnPicture()
{
	PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, In")))
	TInt err = KErrNone;

	if (iFrameMeasurement && iReturnedBuffCount == 1)
	{
		iClock->Reset(); // Reset Timer
		PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, clock reset")))
	}
	else
	{
		if ( iFrameMeasurement )
		{
			PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, Frame measurement is on, %d. frame"), iReturnedBuffCount))
			TTimeIntervalMicroSeconds time;
			time = iClock->Time();
			PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, time measured: [%d%d]"), I64HIGH(time.Int64()), I64LOW(time.Int64())))
			iFrameTime = (TReal)time.Int64()/1000000.0;
			if(iInstantFpsEnabled)
			{
				iInstantFpsList.Append(time);
			}

		}
		else
		{
			err = iOutFile.Write(*(iOutBuffer->iData.iRawData),iOutBuffer->iData.iRawData->Size() );
			PRINT((_L("CVDecTestEngine::SaveAndReturnPicture,  Output data stored: count[%d]"),iReturnedBuffCount))


		}
	}

	if (iSetSlowClient)
	{
		if (iOutBufferTemp)
		{
			PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, returning previous buffer (slow client)")))

			iDevvp->ReturnPicture(iOutBufferTemp);
		}
		iOutBufferTemp = iOutBuffer;
	}
	else
	{
		PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, returning buffer")))
		iDevvp->ReturnPicture(iOutBuffer);
	}


	iOutBuffer = NULL;

	if ( err != KErrNone )
	{
		//iInputEnd = ETrue; Fix for MRII-7HW9R3
		iError = err;
		PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, Error in store data: [%d]"),iError))
	}

	PRINT((_L("CVDecTestEngine::SaveAndReturnPicture, Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Pause()
* Purpose:    Pause decoding/postprocessing
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


TInt CVDecTestEngine::Pause()
{
	TInt err = KErrNone;

	if ( iState == ERunning )
	{
		if ( iSynchronized )
		{
			iClock->Suspend();  //Stop Clock source
		}
		iDevvp->Pause();
		iState = EPaused;
		PRINT((_L("CVDecTestEngine::Pause(), Paused")))
	}
	else
	{
		err = KErrGeneral;
	}
	return err;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::Resume()
* Purpose:    Resume decoding/postprocessing
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::Resume()
{

	TInt err = KErrNone;

	if ( iState == EPaused )
	{
		if ( iSynchronized )
		{
			iClock->Resume();  //Restart Clock source
		}
		iDevvp->Resume();
		iState = ERunning;
		PRINT((_L("CVDecTestEngine::Resume(), Resumed")))

		/*if ( iDecHWDevId )
		{
		if ( iDevvp->NumFreeBuffers() )
		{
		iDecTestAO->RequestData();   //Restart data transfer
		}
		else
		{
		PRINT((_L("CVDecTestEngine::Resume, No input buffer available now")));
		}
		}*/
		iDecTestAO->RequestData();
	}
	else
	{
		err = KErrGeneral;
	}

	return err;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FreezePicture()
* Purpose:    Freeze decoding/postprocessing
* Note:
* Parameters: TTimeIntervalMicroSeconds aTimeStamp
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::FreezePicture(TTimeIntervalMicroSeconds aTimeStamp)
{
	iDevvp->FreezePicture(aTimeStamp);
	PRINT((_L("CVDecTestEngine::Freeze(), Freezed: timestamp[%ld]"), aTimeStamp.Int64()))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReleaseFreeze()
* Purpose:    Release Freeze
* Note:
* Parameters: TTimeIntervalMicroSeconds aTimeStamp
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::ReleaseFreeze(TTimeIntervalMicroSeconds aTimeStamp)
{
	iDevvp->ReleaseFreeze(aTimeStamp);
	PRINT((_L("CVDecTestEngine::ReleaseFreeze(), Freeze released")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::StartDirectScreenAccessL
* Purpose:    Start DirectScreenAccess
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::StartDirectScreenAccessL(TRect aVideoRect, TRect aClipRegion)
{
	PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL(), In")))
	if (!iScreenDevice)
	{
		User::LeaveIfError (RFbsSession::Connect());
		PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL, connected to Fbs server :")))
		TInt err = KErrNone;
		if( iUseSecondScreen )
		{
			TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(1, EColor16MU));
		}
		else
		{
			TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MU));
		}
		if( err == KErrNotSupported )
		{
			PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL, EColor16MU is not supported, ")))
			if( iUseSecondScreen )
			{
				TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(1, EColor16MA));
			}
			else
			{
				TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MA));
			}
		}

		if( err != KErrNone )
		{
			PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL, couldn't initialize screen device, err[%d]"),err))
		}

		else
		{
			iMemAlloc++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL, mem alloc, iScreenDevice") ))
			#endif
		}
	}

	TRegionFix<1> region(aClipRegion);

	//Start DSA
	iDevvp->StartDirectScreenAccessL(aVideoRect, *iScreenDevice, region);
	PRINT((_L("CVDecTestEngine::Start, DSA started")))
	iDsaStarted = ETrue;

	PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL(), Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::StartDirectScreenAccessL
* Purpose:    Start DirectScreenAccess
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::StartDirectScreenAccessL()
{
	PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL(), In")))
	if (iDsaStarted)
	{
		PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL(), DSA already started- abort first")))
		AbortDirectScreenAccess();
	}

	if (!iScreenDevice)
	{


		User::LeaveIfError (RFbsSession::Connect());
		PRINT((_L("CVDecTestEngine::Start, connected to Fbs server :")))


		TInt err = KErrNone;
		if( iUseSecondScreen )
		{
			TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(1, EColor16MU));
		}
		else
		{
			TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MU));
		}
		//TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MU));
		if( err == KErrNotSupported )
		{
			PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL, EColor16MU is not supported, ")))
			if( iUseSecondScreen )
			{
				TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(1, EColor16MA));
			}
			else
			{
				TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MA));
			}
			//TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(0, EColor16MA));
		}

		if ( err != KErrNone )
		{
			PRINT((_L("CVDecTestEngine::Start, couldn't initialize screen device, err[%d]"),err))
		}
	}



	PRINT((_L("CVDecTestEngine::Start, disprect x[%d],y[%d] "),iDispRect.iTl.iX, iDispRect.iTl.iY))
	PRINT((_L("CVDecTestEngine::Start, disprect width[%d],height[%d] "),iDispRect.Width(),iDispRect.Height()))


	TRegionFix<1> region(iDispRect);

	//Start DSA
	iDevvp->StartDirectScreenAccessL(iDispRect, *iScreenDevice, region);
	PRINT((_L("CVDecTestEngine::Start, DSA started")))
	iDsaStarted = ETrue;


	PRINT((_L("CVDecTestEngine::StartDirectScreenAccessL(), Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::AbortDirectScreenAccess
* Purpose:    AbortDirectScreenAccess
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::AbortDirectScreenAccess()
{
	if ( iDirectScreenAccess && iDsaStarted )
	{
	iDevvp->AbortDirectScreenAccess();
	iDsaStarted = EFalse;
	PRINT((_L("CVDecTestEngine::AbortDirectScreenAccess, aborted ")))
	}
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetSecureOutputL
* Purpose:    Set sucure output enable
* Note:
* Parameters: TBool aSecure
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::SetSecureOutputL(TBool aSecure)
{
	PRINT((_L("CVDecTestEngine::SetSecureOutputL, In")))

	#ifdef __SECUREOUTPUTCI__

	MMmfVideoSecureOutput* secureOutput = NULL;     // Custom interface Secure output

	secureOutput = (MMmfVideoSecureOutput*)iDevvp->CustomInterface(iPostProcId, KMmfVideoSecureOutputUid);

	if (!secureOutput)
	{
		PRINT((_L("CVDecTestEngine::SetSecureOutput, Failed to retrieve Secure CI")))
		User::Leave(KErrGeneral);
	}

	secureOutput->MmvsoSetSecureOutputL(aSecure);

	#endif

	PRINT((_L("CVDecTestEngine::SetSecureOutputL, Out [%d]"), aSecure))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetPosition
* Purpose:    Set postion
* Note:
* Parameters: TTimeIntervalMicroSeconds aTimeStamp
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition)
{
	PRINT((_L("CVDecTestEngine::SetPosition, In")))

	iDevvp->SetPosition(aPlaybackPosition);

	PRINT((_L("CVDecTestEngine::SetPosition, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ResetPosition
* Purpose:    Reset postion to start
* Note:
* Parameters: TTimeIntervalMicroSeconds aTimeStamp
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::ResetPosition()
{
	PRINT((_L("CVDecTestEngine::ResetPosition, In")))
	TTimeIntervalMicroSeconds starttime(0);

	iDevvp->SetPosition(starttime);
	PRINT((_L("CVDecTestEngine::ResetPosition, Reset Position to start of bitstream")))

	if ( iSynchronized )
	{
		iClock->Reset(starttime);
		iTimeStamp = 0;
		TTimeIntervalMicroSeconds currenttime = iClock->Time();
		PRINT((_L("CVDecTestEngine::ResetPosition, current time[%ld]"),currenttime.Int64() ))
	}
	TInt pos = 0;
	iInFile.Seek(ESeekStart,pos);
	iSentBuffCount = 0;

	PRINT((_L("CVDecTestEngine::ResetPosition, In")))
}




/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::PlaybackPosition
* Purpose:    Ritrieve Playback Position
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


TTimeIntervalMicroSeconds CVDecTestEngine::PlaybackPosition()
{
	PRINT((_L("CVDecTestEngine::PlaybackPosition, In")))

	TTimeIntervalMicroSeconds position = iDevvp->PlaybackPosition();
	PRINT((_L("CVDecTestEngine::PlaybackPosition, Playback Position[%ld]"),position.Int64()))

	PRINT((_L("CVDecTestEngine::PlaybackPosition, Out")))
	return position;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::DecodingPosition
* Purpose:    Ritrieve Decoding Position
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::DecodingPosition()
{
	PRINT((_L("CVDecTestEngine::DecodingPosition, In")))

	TTimeIntervalMicroSeconds position = iDevvp->DecodingPosition();
	PRINT((_L("CVDecTestEngine::DecodingPosition, Decoding Position[%ld]"),position.Int64()))

	PRINT((_L("CVDecTestEngine::DecodingPosition, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetFrameSizeCount
* Purpose:    Get frame size count after frame sizes are listed
* Note:       Called after initialization
* Parameters: None
* Return:     TInt
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::GetFrameSizeCount()
    {
    return iFrameSizeList.Count();
    }

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::EnableSynchronization
* Purpose:    Set Clock and enable synchronization
* Note:       Called before initialization and after Decoder/post-processor selected
* Parameters: TInt aFrameRate
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::EnableSynchronization(TInt aFrameRate)
{
	PRINT((_L("CVDecTestEngine::EnableSynchronization, In")))

	iDevvp->SetClockSource(iClock);
	PRINT((_L("CVDecTestEngine::Synchronization, Clock source set")))

	iSynchronized = ETrue;
	iFrameTimeInterval = (TInt64)(1000000/aFrameRate);
	PRINT((_L("CVDecTestEngine::Synchronization, Frame rate:[%d]"),aFrameRate ))

	PRINT((_L("CVDecTestEngine::EnableSynchronization, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetWindowRect
* Purpose:    Set sucure output enable
* Note:
* Parameters: TInt aX, TInt aY, TSize aSize
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetWindowRect(TInt aX, TInt aY, TSize aSize )
{

	iDispRect.SetRect(TPoint(aX,aY),aSize);

	PRINT((_L("CVDecTestEngine::SetWindowRect, [%d], [%d]"),aX ,aY ))
	PRINT((_L("CVDecTestEngine::SetWindowRect, width[%d], height[%d]"),aSize.iWidth, aSize.iHeight))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ListFrameSizeL
* Purpose:    List size of each frmae of coded bitstream for synchrinized
*             playback
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::ListFrameSizeL(RArray<TInt> aFramesizes)
{

	if ( iCodecType == EVC1 ){

		PRINT((_L("CVDecTestEngine::ListFrameSizeL, not supported for VC1") ))
		User::Leave(KErrGeneral);
	}

	TInt framesize = 0;
	TVideoInputBuffer* videoBuffer = new(ELeave) TVideoInputBuffer;
	iMemAlloc++;
	HBufC8* tempBuff = HBufC8::NewL(iInBuffSize);
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::ListFrameSizeL, mem alloc, videoBuffer & iInBuffSize") ))
	#endif
	videoBuffer->iData.Set(tempBuff->Des());
	//iListFrameSize = ETrue;

	if(aFramesizes.Count() > 0)
	{
		for (TUint i = 0; i < aFramesizes.Count(); i++)
		{
			iFrameSizeList.AppendL(aFramesizes[i]);
			PRINT((_L("CVDecTestEngine::ListFrameSize, user input [%d]"),aFramesizes[i]))
		}

	}
	else
	{
		while (!iInputEnd)
		{
			videoBuffer->iData.SetLength(0);
			framesize = ReadOneCodedPicture(videoBuffer);
			PRINT((_L("CVDecTestEngine::ListFrameSize, frame size[%d]"),framesize))
			// here append frame size in list

			if (framesize >= 0 )
			{
				iFrameSizeList.AppendL(framesize);
				iSentBuffCount++;
			}
			else
			{
				break;
			}

		}
	}
	PRINT((_L("CVDecTestEngine::ListFrameSize, frame count[%d]"),iFrameSizeList.Count()))

	// cleanup
	//Reset file postion to start
	TInt pos =0;
	iSentBuffCount = 0;
	iReadDataChunk = iDataChunk;
	iWriteDataChunk = iDataChunk;
	iDataReadFromFile = 0;
	iInFile.Seek(ESeekStart,pos);

	iInputEnd = EFalse;

	delete tempBuff;
	delete videoBuffer;
	iMemDelete += 2;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::ListFrameSize, mem del, tempBuff & videoBuffer") ))
	#endif
	if ( framesize < 0)
	{
        PRINT((_L("CVDecTestEngine::ListFrameSize, error[%d]"),framesize ))
		User::Leave(framesize);
	}

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetLandscapeMode()
* Purpose:    Set landscape mode flag
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

/*
void CVDecTestEngine::SetLandscapeMode()
{

iLandscapeMode = ETrue;
PRINT((_L("CVDecTestEngine::SetLandscapeMode, Landscape mode is set") ))

}
*/

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SelectDecoderL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SelectDecoderL(const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SelectDecoderL, In") ))

	iDecoderUid = aUid;
	iDecHWDevId = iDevvp->SelectDecoderL(aUid);

	#ifdef __RESOURCENOTIFICATION__
	MMmfVideoResourceHandler* handler = (MMmfVideoResourceHandler*)iDevvp->CustomInterface(iDecHWDevId, KUidMmfVideoResourceManagement );
	handler->MmvrhSetObserver(((MMmfVideoResourceObserver*)(this)));
	#endif

	PRINT((_L("CVDecTestEngine::SelectDecoderL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SelectPostProcessorL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SelectPostProcessorL(const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SelectPostProcessorL, In") ))

	iPostProcessorUid = aUid;
	iPostProcId = iDevvp->SelectPostProcessorL(aUid);

	PRINT((_L("CVDecTestEngine::SelectPostProcessorL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetBufferOptionsL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions)
{
	PRINT((_L("CVDecTestEngine::SetBufferOptionsL, In") ))

	iInBuffSize = aOptions.iMaxInputBufferSize;
	iDevvp->SetBufferOptionsL(aOptions);

	PRINT((_L("CVDecTestEngine::SetBufferOptionsL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetVideoDestScreenL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetVideoDestScreenL(TBool aDest)
{
	PRINT((_L("CVDecTestEngine::SetVideoDestScreenL, In") ))

	iDevvp->SetVideoDestScreenL(aDest);

	PRINT((_L("CVDecTestEngine::SetVideoDestScreenL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SynchronizeDecoding()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SynchronizeDecoding(TBool aSynch)
{
	PRINT((_L("CVDecTestEngine::SynchronizeDecoding, In") ))

	iDevvp->SynchronizeDecoding(aSynch);

	PRINT((_L("CVDecTestEngine::SynchronizeDecoding, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetBufferOptions()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::GetBufferOptions()
{
	PRINT((_L("CVDecTestEngine::GetBufferOptions, In") ))

	CMMFDevVideoPlay::TBufferOptions options;
	iDevvp->GetBufferOptions(options);

	PRINT((_L("CVDecTestEngine::GetBufferOptions: iPreDecodeBufferSize: [%d]\n iMaxPostDecodeBufferSize: [%d]\n"),
	options.iPreDecodeBufferSize, options.iMaxPostDecodeBufferSize))
	PRINT((_L("CVDecTestEngine::GetBufferOptions: iMaxInputBufferSize [%d]\n iMinNumInputBuffers [%d]\n "),
	options.iMaxInputBufferSize, options.iMinNumInputBuffers))
	PRINT((_L("CVDecTestEngine::GetBufferOptions: iPreDecoderBufferPeriod [%d] [%d]"), I64LOW(options.iPreDecoderBufferPeriod.Int64()), I64HIGH(options.iPreDecoderBufferPeriod.Int64())))
	PRINT((_L("CVDecTestEngine::GetBufferOptions: iPostDecoderBufferPeriod [%d] [%d]"), I64LOW(options.iPostDecoderBufferPeriod.Int64()), I64HIGH(options.iPostDecoderBufferPeriod.Int64())))

	PRINT((_L("CVDecTestEngine::GetBufferOptions, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetInputFormatL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetInputFormatL(TInt aHwDev, const TUncompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::SetInputFormatL, In") ))

	if (aHwDev == EPostProcessor)
	{
		iDevvp->SetInputFormatL(iPostProcId, aFormat);
	}
	else
	{
		iDevvp->SetInputFormatL(iDecHWDevId, aFormat);
	}

	PRINT((_L("CVDecTestEngine::SetInputFormatL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetOutputFormatL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetOutputFormatL(TInt aHwDev, const TUncompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::SetOutputFormatL, In") ))

	if (aHwDev == EPostProcessor)
	{
		iDevvp->SetOutputFormatL(iPostProcId, aFormat);
	}
	else
	{
		iDevvp->SetOutputFormatL(iDecHWDevId, aFormat);
	}

	PRINT((_L("CVDecTestEngine::SetOutputFormatL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetInputFormatL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetInputFormatL(TInt aHwDev, const CCompressedVideoFormat& aFormat,
TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation,
TBool aDataInOrder)
{
	PRINT((_L("CVDecTestEngine::SetInputFormatL, In") ))

	if (aHwDev == EPostProcessor)
	{
		iDevvp->SetInputFormatL(iPostProcId, aFormat, aDataUnitType, aEncapsulation, aDataInOrder);
	}
	else
	{
		iDevvp->SetInputFormatL(iDecHWDevId, aFormat, aDataUnitType, aEncapsulation, aDataInOrder);
	}

	PRINT((_L("CVDecTestEngine::SetInputFormatL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FindCommonFormats()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::FindCommonFormatsL()
{
	PRINT((_L("CVDecTestEngine::FindCommonFormatsL, In") ))

	// Set Output Format for Decoder/Input Format for Postprocessor
	RArray<TUncompressedVideoFormat> decformats;
	TUncompressedVideoFormat commonFormat;


	iDevvp->GetOutputFormatListL(iDecHWDevId, decformats);

	CPostProcessorInfo* info;

	info = iDevvp->PostProcessorInfoLC(iPostProcessorUid);

	RArray<TUncompressedVideoFormat> ppformats;
	ppformats = info->SupportedFormats();

	//Find common format for dec output and postproc input
	TBool found = iDevvp->FindCommonFormat(decformats.Array(), ppformats.Array(), commonFormat);

	if (found)
	{
		iDevvp->SetOutputFormatL(iDecHWDevId, commonFormat);
		PRINT((_L("CVDecTestEngine::FindCommonFormatsL, Decoder output Format set")))
		iDevvp->SetInputFormatL(iPostProcId, commonFormat);
		PRINT((_L("CVDecTestEngine::FindCommonFormatsL, Postprocessor input format set.")))
		PrintUncompressedFormat(commonFormat);
	}
	else
	{
		PRINT((_L("CVDecTestEngine::FindCommonFormatsL, Common Format not found")))
		User::Leave( KErrNotFound);
	}
	CleanupStack::PopAndDestroy(info);
	PRINT((_L("CVDecTestEngine::FindCommonFormatsL, closing array") ))
	decformats.Close();
	PRINT((_L("CVDecTestEngine::FindCommonFormatsL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FindDecodersL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::FindDecodersL(const TDesC8& aMimeType, TUint32 aPostProcType, TBool aExactMatch, TUid& aUid )
{
	PRINT((_L("CVDecTestEngine::FindDecodersL, In") ))

	TInt supports = KErrNotSupported;

	//UID is not set
	if (aUid.iUid == 0 && !aExactMatch)
	{
		supports = KErrNone;
	}

	RArray<TUid> decoders;
	CleanupClosePushL(decoders);
	iDevvp->FindDecodersL(aMimeType, aPostProcType, decoders, aExactMatch);
	for (TUint i = 0; i < decoders.Count(); i++)
	{
		if (decoders[i].iUid == aUid.iUid)
		{
			supports = KErrNone;
		}

		PRINT((_L("CVDecTestEngine::FindDecodersL, Uid [%x]"), decoders[i].iUid ))
	}
	CleanupStack::PopAndDestroy(&decoders);

	PRINT((_L("CVDecTestEngine::FindDecodersL, Out") ))

	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::FindPostProcessorsL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::FindPostProcessorsL(TUid& aUid, TUint32 aPostProcType)
{
	PRINT((_L("CVDecTestEngine::FindPostProcessorsL, In") ))

	TInt supports = KErrNotSupported;

	//UID is not set
	if (aUid.iUid == 0)
	{
		supports = KErrNone;
	}

	RArray<TUid> postProcessors;
	CleanupClosePushL(postProcessors);
	iDevvp->FindPostProcessorsL(aPostProcType, postProcessors);
	for (TUint i = 0; i < postProcessors.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::FindPostProcessorsL, Uid [%x]"), postProcessors[i].iUid ))

		if (postProcessors[i].iUid == aUid.iUid)
		{
			supports = KErrNone;
		}

	}
	CleanupStack::PopAndDestroy(&postProcessors);

	PRINT((_L("CVDecTestEngine::FindPostProcessorsL, Out") ))
	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetDecoderListL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::GetDecoderListL()
{
	PRINT((_L("CVDecTestEngine::GetDecoderListL, In") ))

	RArray<TUid> decoders;
	CleanupClosePushL(decoders);
	iDevvp->GetDecoderListL(decoders);
	for (TUint i = 0; i < decoders.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::GetDecoderListL, Uid [%x]"), decoders[i].iUid ))
	}
	CleanupStack::PopAndDestroy(&decoders);

	PRINT((_L("CVDecTestEngine::GetDecoderListL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ConfigureDecoderL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::ConfigureDecoderL(TVideoPictureHeader& aVideoPictureHeader)
{
	PRINT((_L("CVDecTestEngine::ConfigureDecoderL, In") ))

	if( iSetPassword )
	{
		TBuf16<128> tempBuf;
		tempBuf.Copy( iPasswordForFLV);
		PRINT( ( _L( "CVDecTestEngine::ConfigureDecoderL(): password tempBuf: %S." ), &tempBuf ) )

		TPtrC8 metaData((TUint8*)iPasswordForFLV.Ptr(), iPasswordForFLV.Size());

		aVideoPictureHeader.iOptional = &metaData;

	}

	iDevvp->ConfigureDecoderL(aVideoPictureHeader);

	PRINT((_L("CVDecTestEngine::ConfigureDecoderL, Out") ))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ConfigureDecoderL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::ConfigureDecoderL()
{
	PRINT((_L("CVDecTestEngine::ConfigureDecoderL, In") ))

	if (iCodecType == EVC1)
	{
		PRINT((_L("CVDecTestEngine::ConfigureDecoderL, iCodectype VC1") ))
		TInt configPosition = 0;
		TUint configDataSize = 36; // MetaData is 36 Bytes in SP-L0 VC1 .
		TUint8* ptr = new (ELeave) TUint8[configDataSize+8];
		iMemAlloc++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::ConfigureDecoderL, mem alloc, ptr") ))
		#endif
		TUint32* ptr32 = reinterpret_cast<TUint32*>(ptr);
		ptr32[0] = 1;
		ptr32[1] = 44;
		TPtr8 configData((ptr+8), configDataSize);
		for (TUint i=0; i < 8; i++)
		{
			PRINT((_L("CVDecTestEngine::ConfigureDecoderL, beginning [%d]"), ptr[i] ))
		}

		User::LeaveIfError(iInFile.Seek(ESeekStart, configPosition));
		User::LeaveIfError(iInFile.Read(configData, configDataSize));
		TVideoPictureHeader* configHeader = new TVideoPictureHeader;
		iMemAlloc++;
		CleanupStack::PushL(configHeader);
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::ConfigureDecoderL, mem alloc, configHeader") ))
		#endif

		TPtrC8 metaData(ptr,configDataSize+8);


		configHeader->iOptional = &metaData;
		configHeader->iLevel = -1;
		configHeader->iProfile = -1;

		iDevvp->ConfigureDecoderL(*configHeader);


		CleanupStack::Pop();
		delete ptr;
		iMemDelete++;
		delete configHeader;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::ConfigureDecoderL, mem del, ptr & configHeader") ))
		#endif
		return;
	}

	TInt err = KErrNone;
	TVideoPictureHeader* headerInfo = NULL;
	HBufC8* tempBuff = NULL;

	//Read Picture header : Size is not known
	TVideoInputBuffer* codedBuffer = new(ELeave) TVideoInputBuffer;
	CleanupStack::PushL( codedBuffer );
	iInBuffSize = KMaxCodedSize;
	err = KErrOverflow;

	while ( (err == KErrOverflow) && (iInBuffSize <= KMaxCodedSize) )
	{
		tempBuff = HBufC8::NewL(iInBuffSize);
		CleanupStack::PushL( tempBuff );
		codedBuffer->iData.Set(tempBuff->Des());

		if ( iFrameSizeList.Count() > 0 )
		{
			err = ReadOneCodedPicture(codedBuffer, iFrameSizeList[0] );
		}
		else
		{
			err = ReadOneCodedPicture(codedBuffer);
		}
		//err = ReadOneCodedPicture(codedBuffer);

		TInt pos =0;
		if ( iInFile.Seek(ESeekStart,pos) )
		{
			err = KErrGeneral;
		}

		if ( iInputEnd )
		{
			err = KErrNotFound;
		}

		if ( err < 0 )
		{
			CleanupStack::PopAndDestroy(tempBuff);
			iInBuffSize = 4*iInBuffSize;
		}
	}


	// Reitrieve header information from bitstream
	if ( err  < 0 )
	{
		CleanupStack::PopAndDestroy(codedBuffer);
		PRINT((_L("CVDecTestEngine::ConfigureDecoderL, Reading input data fail")))
	}
	else
	{
		err = KErrNone;

		headerInfo = iDevvp->GetHeaderInformationL(EDuCodedPicture,EDuElementaryStream,codedBuffer);

		CleanupStack::PopAndDestroy(tempBuff);
		CleanupStack::PopAndDestroy(codedBuffer);

		//Check Header info
		if ( headerInfo )
		{

			PRINT((_L("CVDecTestEngine::ConfigureDecoderL, Configuring the decoder")))


			if( iSetPassword )
			{
				TBuf16<128> tempBuf;
				tempBuf.Copy( iPasswordForFLV);
				PRINT( ( _L( "CVDecTestEngine::ConfigureDecoderL(): password tempBuf: %S." ), &tempBuf ) )

				TPtrC8 metaData((TUint8*)iPasswordForFLV.Ptr(), iPasswordForFLV.Size());

				headerInfo->iOptional = &metaData;

			}
			iDevvp->ConfigureDecoderL(*headerInfo);

			//return headerInfo
			iDevvp->ReturnHeader(headerInfo);

		}
		else
		{
			PRINT((_L("CVDecTestEngine::ConfigureDecoderL, Get header info fail")))
			err = KErrGeneral;
		}

	}

	// Set size of decoder input buffer
	if ( iCodecType == EH263 )
	{
		if ( (iPictureSize.iWidth <= 176 ) && (iPictureSize.iHeight <= 144) )
		iInBuffSize = KH263MaxCodedSizeQCIF;
		else
		iInBuffSize = KH263MaxCodedSizeCIF;

	}
	else //Mpeg4 and AVC and FLV
	{
		if ( (iPictureSize.iWidth <= 176 ) && (iPictureSize.iHeight <= 144) )
		iInBuffSize = KMP4MaxCodedSizeQCIF;
		else if ( (iPictureSize.iWidth <= 352 ) && (iPictureSize.iHeight <= 288) )
		iInBuffSize = KMP4MaxCodedSizeCIF;
		else if ( (iPictureSize.iWidth <= 640 ) && (iPictureSize.iHeight <= 480) )
		iInBuffSize = KMP4MaxCodedSizeVGA;
		else if  ( (iPictureSize.iWidth <= 720 ) && (iPictureSize.iHeight <= 576) )
		iInBuffSize = KMP4MaxCodedSizePAL;
		else
		iInBuffSize = KMaxCodedSize;
	}

	AssertTIntEqualL(KErrNone, err);

	TInt pos =0;
	iSentBuffCount = 0;
	iReadDataChunk = iDataChunk;
	iWriteDataChunk = iDataChunk;
	iDataReadFromFile = 0;
	iInFile.Seek(ESeekStart,pos);

	PRINT((_L("CVDecTestEngine::ConfigureDecoderL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetOutputFormatListL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::GetOutputFormatListL(TInt aHWDevice)
{
	PRINT((_L("CVDecTestEngine::GetOutputFormatListL, In") ))

	RArray<TUncompressedVideoFormat> formats;
	CleanupClosePushL(formats);
	if (aHWDevice == EPostProcessor)
	{
		iDevvp->GetOutputFormatListL(iPostProcId, formats);
	}
	else
	{
		iDevvp->GetOutputFormatListL(iDecHWDevId, formats);
	}

	for (TUint i = 0; i < formats.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::GetOutputFormatListL, Format No.%d"), i))
		PrintUncompressedFormat(formats[i]);
	}
	CleanupStack::PopAndDestroy(&formats);

	PRINT((_L("CVDecTestEngine::GetOutputFormatListL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsOutputFormatL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::SupportsOutputFormatL(TInt aHwdev, const TUncompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::SupportsOutputFormatL, In") ))

	TInt result = KErrNotSupported;

	RArray<TUncompressedVideoFormat> formats;
	CleanupClosePushL(formats);
	if (aHwdev == EPostProcessor)
	{
		iDevvp->GetOutputFormatListL(iPostProcId, formats);
	}
	else
	{
		iDevvp->GetOutputFormatListL(iDecHWDevId, formats);
	}

	for (TUint i = 0; i < formats.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::SupportsOutputFormatL, Format No.%d"), i))
		if (formats[i] == aFormat)
		{
			result = KErrNone;
		}
		PrintUncompressedFormat(formats[i]);
	}
	CleanupStack::PopAndDestroy(&formats);

	PRINT((_L("CVDecTestEngine::SupportsOutputFormatL, Out") ))

	return result;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GetPostProcessorListL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::GetPostProcessorListL()
{
	PRINT((_L("CVDecTestEngine::GetPostProcessorListL, Out") ))

	RArray<TUid> postProcessors;
	CleanupClosePushL(postProcessors);
	iDevvp->GetPostProcessorListL(postProcessors);
	for (TUint i = 0; i < postProcessors.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::GetPostProcessorListL, Uid [%x]"), postProcessors[i].iUid ))
	}
	CleanupStack::PopAndDestroy(&postProcessors);

	PRINT((_L("CVDecTestEngine::GetPostProcessorListL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::VideoDecoderInfoL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::VideoDecoderInfoL(TUid aVideoDecoder)
{
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, In") ))

	CVideoDecoderInfo* info;
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, 2test") ))
	info = iDevvp->VideoDecoderInfoLC(aVideoDecoder);

	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, test") ))

	TVersion version = info->Version();
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, Manufacturer: %S"), &info->Manufacturer()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, Identifier: %S"), &info->Identifier()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, Version information:")))
	PRINT((_L("Major Version:[%d], Minor Version:[%d], Build number:[%d] "), version.iMajor,version.iMinor,version.iBuild))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, HW Accelerated:[%d]"),info->Accelerated()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, Support Direct Screen:[%d]"),info->SupportsDirectDisplay()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, MaxPictureSize, %dx%d"),info->MaxPictureSize().iWidth, info->MaxPictureSize().iHeight))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, MaxBitrate: [%d]"),info->MaxBitrate()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, SupportsPictureLoss: [%d]"),info->SupportsPictureLoss()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, SupportsSliceLoss: [%d]"),info->SupportsSliceLoss()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, CodingStandardSpecificInfo: %S"), &info->CodingStandardSpecificInfo()))
	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, ImplementationSpecificInfo: %S"), &info->ImplementationSpecificInfo()))

	CleanupStack::PopAndDestroy(info);

	PRINT((_L("CVDecTestEngine::VideoDecoderInfoL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::PostProcessorInfoL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::PostProcessorInfoL(TUid aPostProcessor)
{
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, In") ))

	CPostProcessorInfo* info;
	info = iDevvp->PostProcessorInfoLC(aPostProcessor);

	TVersion version = info->Version();
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Manufacturer: %S"), &info->Manufacturer()))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Identifier: %S"), &info->Identifier()))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Version information:")))
	PRINT((_L("Major Version:[%d], Minor Version:[%d], Build number:[%d] "), version.iMajor,version.iMinor,version.iBuild))

	// combination
	RArray<TUint32> combination = info->SupportedCombinations();

	for (TInt i=0; i < combination.Count(); i++)
	{
		PRINT((_L("CVDecTestEngine::PostProcessorInfoL, combiantion:[%x]"), combination.operator[](i)))
	}

	//Color Conversion YUV to RGV
	TYuvToRgbCapabilities colorconv;
	colorconv = info->YuvToRgbCapabilities();
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, YUV Sampling Pattern bit:[0x%x]"),colorconv.iSamplingPatterns))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, YUV Coefficient bit:[0x%x]"),colorconv.iCoefficients))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, RGB Formats bit:[0x%x]"),colorconv.iRgbFormats))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Lightness Control:[%d]"),colorconv.iLightnessControl))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Saturation Control:[%d]"),colorconv.iSaturationControl))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Contrast Control:[%d]"),colorconv.iContrastControl))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Gamma Correction:[%d]"),colorconv.iGammaCorrection))

	if(colorconv.iDitherTypes  & EDitherNone)
	{
		PRINT((_L("CVDecTestEngine::PostProcessorInfoL, No dither support")))
	}
	if(colorconv.iDitherTypes  & EDitherOrdered)
	{
		PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Ordered Dither")))
	}
	if(colorconv.iDitherTypes  & EDitherErrorDiffusion)
	{
		PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Error diffusion dither")))
	}
	if(colorconv.iDitherTypes  & EDitherOther)
	{
		PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Other hardware device specific dithering type")))
	}


	//HW Accelated
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, HW Accelerated:[%d]"),info->Accelerated()))

	//Support Direct Screen
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Direct Screen:[%d]"),info->SupportsDirectDisplay()))

	//Rotation
	TUint rotation;
	rotation =  info->SupportedRotations();

	if(rotation & 0x00000001)
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Rotation: 90Clockwise ")))
	if(rotation & 0x00000002)
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Rotation: 90Anticlockwise ")))
	if(rotation & 0x00000004)
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Rotation: 180 ")))
	if(!(rotation & 0x00000007))
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, No Rotation supported ")))

	//Arbitrary Scaling
	TInt support = info->SupportsArbitraryScaling();
	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Arbitrary Scaling:[%d]"),support))

	//Scalling Factor
	if ( support )
	{
		RArray<TScaleFactor> scale = info->SupportedScaleFactors();

		for (TInt i=0; i<scale.Count(); i++)
		{
			PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Scalling Factor:[%d]/[%d]"),scale.operator[](i).iScaleNum, scale.operator[](i).iScaleDenom))
		}
	}

	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Support Anti Aliased filter:[%d]"),info->AntiAliasedScaling()))

	CleanupStack::PopAndDestroy(info);

	PRINT((_L("CVDecTestEngine::PostProcessorInfoL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsFormatL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsFormatL(const TUid& aUid, const CCompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::SupportsFormatL(),In")))

	TInt supports = KErrNotSupported;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsFormatL(),Error")))
		return KErrGeneral;
	}

	if (info->SupportsFormat(aFormat))
	{
		PRINT((_L("CVDecTestEngine::SupportsFormatL, Supported format found")))
		supports=KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsFormatL(),Out")))
	return supports;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsFormatInfoL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsFormatInfoL(const TUid& aUid, const CCompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::SupportsFormatInfoL(),In")))

	TInt supports = KErrNotSupported;
	RPointerArray<CCompressedVideoFormat> formats;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsFormatInfoL(),Error")))
		formats.Close();
		return KErrGeneral;
	}

	formats = info->SupportedFormats();

	PRINT((_L("CVDecTestEngine::SupportsFormatInfoL(), supported formats")))

	for (TUint i = 0; i < formats.Count(); i++)
	{
		TBuf16<128> format;
		format.Copy(formats[i]->MimeType());
		PRINT((_L("CVDecTestEngine::SupportsFormatInfoL, Format [%S]"), &format))
		//PRINT((_L("CVDecTestEngine::SupportsFormatInfoL, %d. Format [%S]"), &(formats[i]))
		if (*(formats[i]) == aFormat)
		{
			supports = KErrNone;
			PRINT((_L("CVDecTestEngine::SupportsFormatInfoL, Supported format found")))
		}
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsFormatInfoL(),Out")))
	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsAccelerationL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsAccelerationL(TInt aHwDev, const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SupportsAccelerationL(),In")))

	TInt supports = KErrNotSupported;
	TBool supported = EFalse;

	if (aHwDev == EPostProcessor)
	{
		CPostProcessorInfo* info = iDevvp->PostProcessorInfoLC(aUid);
		if(!info)
		{
			PRINT((_L("CVDecTestEngine::SupportsAccelerationL(),Error")))
			return KErrGeneral;
		}
		supported = info->Accelerated();
	}
	else
	{
		CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
		if(!info)
		{
			PRINT((_L("CVDecTestEngine::SupportsAccelerationL(),Error")))
			return KErrGeneral;
		}

		supported = info->Accelerated();
	}

	if (supported)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsAccelerationL(),Out")))
	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsDirectDisplayL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsDirectDisplayL(TInt aHwDev, const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SupportsDirectDisplayL(),In")))

	TInt supports = KErrNotSupported;
	TBool supported = EFalse;

	if (aHwDev == EPostProcessor)
	{
		CPostProcessorInfo* info = iDevvp->PostProcessorInfoLC(aUid);
		if(!info)
		{
			PRINT((_L("CVDecTestEngine::SupportsDirectDisplayL(),Error")))
			return KErrGeneral;
		}

		supported = info->SupportsDirectDisplay();
	}
	else
	{
		CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
		if(!info)
		{
			PRINT((_L("CVDecTestEngine::SupportsDirectDisplayL(),Error")))
			return KErrGeneral;
		}

		supported = info->SupportsDirectDisplay();
	}

	if (supported)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsDirectDisplayL(),Out")))
	return supports;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsMaxPictureSizeL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsMaxPictureSizeL(const TUid& aUid, TSize aSize)
{
	PRINT((_L("CVDecTestEngine::SupportsMaxPictureSizeL(),In")))

	TInt supports = KErrNotSupported;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsMaxPictureSizeL(),Error")))
		return KErrGeneral;
	}

	TSize size = info->MaxPictureSize();

	if (size.iWidth >= aSize.iWidth && size.iHeight >= aSize.iHeight)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsMaxPictureSizeL(),Out")))
	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsMaxPictureRateL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsMaxPictureRateL(const TUid& aUid, TPictureRateAndSize& aRateAndSize)
{
	PRINT((_L("CVDecTestEngine::SupportsMaxPictureRateL(),In")))

	TInt supports = KErrNotSupported;
	TPictureRateAndSize picRateAndSize;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVEncTestEngine::SupportsMaxPictureRateL(),Error")))
		return KErrGeneral;
	}

	RArray<TPictureRateAndSize> picRateAndSizeArray = info->MaxPictureRates();

	for (TUint i=0; i < picRateAndSizeArray.Count(); i++)
	{
		picRateAndSize = picRateAndSizeArray[i];
		if (aRateAndSize.iPictureRate <= picRateAndSize.iPictureRate && aRateAndSize.iPictureSize.iWidth <= picRateAndSize.iPictureSize.iWidth &&
		aRateAndSize.iPictureSize.iHeight <= picRateAndSize.iPictureSize.iHeight)
		{
			supports = KErrNone;
		}
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsMaxPictureRateL(),Out")))
	return supports;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsSliceLossL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsSliceLossL(const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SupportsSliceLossL(),In")))

	TInt supports = KErrNotSupported;
	TBool supported = EFalse;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsSliceLossL(),Error")))
		return KErrGeneral;
	}

	supported = info->SupportsSliceLoss();

	if (supported)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsSliceLossL(),Out")))
	return supports;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsPictureLossL()
* Purpose:    Retrieves information about an installed video decoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsPictureLossL(const TUid& aUid)
{
	PRINT((_L("CVDecTestEngine::SupportsPictureLossL(),In")))

	TInt supports = KErrNotSupported;
	TBool supported = EFalse;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsPictureLossL(),Error")))
		return KErrGeneral;
	}

	supported = info->SupportsPictureLoss();

	if (supported)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsPictureLossL(),Out")))
	return supports;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SupportsMaxBitrateL()
* Purpose:    Retrieves information about an installed video encoder
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::SupportsMaxBitrateL(const TUid& aUid, TInt aBitrate)
{
	PRINT((_L("CVDecTestEngine::SupportsMaxBitrateL(),In")))

	TInt supports = KErrNotSupported;
	TInt bitrate = 0;

	CVideoDecoderInfo* info = iDevvp->VideoDecoderInfoLC(aUid);
	if(!info)
	{
		PRINT((_L("CVDecTestEngine::SupportsMaxBitrateL(),Error")))
		return KErrGeneral;
	}

	bitrate = info->MaxBitrate();

	if (aBitrate <= bitrate)
	{
		supports = KErrNone;
	}

	CleanupStack::PopAndDestroy();

	PRINT((_L("CVDecTestEngine::SupportsMaxBitrateL(),Out")))
	return supports;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetYuvToRgbOptionsL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetYuvToRgbOptionsL(TInt aHWDevice, const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat)
{
	PRINT((_L("CVDecTestEngine::SetYuvToRgbOptionsL, In") ))

	if (aHWDevice == EPostProcessor)
	{
		iDevvp->SetYuvToRgbOptionsL(iPostProcId, aOptions, aYuvFormat, aRgbFormat);
	}
	else
	{
		iDevvp->SetYuvToRgbOptionsL(iDecHWDevId, aOptions, aYuvFormat, aRgbFormat);
	}

	PRINT((_L("CVDecTestEngine::SetYuvToRgbOptionsL, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetSlowClient()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetSlowClient(TBool aOption)
{
	PRINT((_L("CVDecTestEngine::SetSlowClient, In") ))

	iSetSlowClient = aOption;

	PRINT((_L("CVDecTestEngine::SetSlowClient, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetFastClient()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetFastClient(TBool aOption)
{
	PRINT((_L("CVDecTestEngine::SetFastClient, In") ))

	iSetFastClient = aOption;

	PRINT((_L("CVDecTestEngine::SetFastClient, Out") ))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetAUFormat()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::SetAUFormat()
{
	PRINT((_L("CVDecTestEngine::SetAUFormat, In") ))

	if (iCodecType== EAVC)
	{
		iSetAUFormat = ETrue;

	}
	else
	{
		return KErrNotSupported;
	}

	PRINT((_L("CVDecTestEngine::SetAUFormat, Out") ))

	return KErrNone;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MoveFileL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::MoveFileL(const TDesC &anOld, const TDesC &aNew)
{
	PRINT((_L("CVDecTestEngine::MoveFileL, In") ))

	CFileMan* fileman = CFileMan::NewL(iFs);
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::MoveFileL, mem alloc, fileman") ))
	#endif
	PRINT((_L("CVDecTestEngine::MoveFileL, fileman [%x]"), fileman))

	CloseFile();

	PRINT((_L("CVDecTestEngine::MoveFileL, File [%s] moving to [%s]"), &anOld, &aNew ))
	fileman->Move(anOld, aNew);

	PRINT((_L("CVDecTestEngine::MoveFileL, File [%s] moved to [%s]"), &anOld, &aNew ))

	delete fileman;
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::MoveFileL, mem del, fileman") ))
	#endif
	PRINT((_L("CVDecTestEngine::MoveFileL, Out") ))

	return KErrNone;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CopyFileL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::CopyFileL(const TDesC &anOld, const TDesC &aNew)
{
	PRINT((_L("CVDecTestEngine::CopyFileL, In") ))

	CFileMan* fileman = CFileMan::NewL(iFs);
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::MoveFileL, mem alloc, fileman") ))
	#endif
	PRINT((_L("CVDecTestEngine::CopyFileL, fileman [%x]"), fileman))

	//CloseFile();

	PRINT((_L("CVDecTestEngine::CopyFileL, File [%s] copying to [%s]"), &anOld, &aNew ))
	fileman->Copy(anOld, aNew);

	PRINT((_L("CVDecTestEngine::CopyFileL, File [%s] copied to [%s]"), &anOld, &aNew ))

	delete fileman;
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::CopyFileL, mem del, fileman") ))
	#endif
	PRINT((_L("CVDecTestEngine::CopyFileL, Out") ))

	return KErrNone;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::HandleNewBufferL()
* Purpose:    Get Next input data
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::HandleNewBufferL()
{

	if ( iState == ERunning && !iInputEnd )
	{
		if ( iDecHWDevId || iCIBuffMgmtOn ) //Retrieve buffer from Hwdevice
		{
			GetInputBufferL();
		}
		else
		{
			FillAndSendBufferL();   // Postproc input
		}

	}
	else
	{
		PRINT((_L("CVDecTestEngine::HandleNewBufferL, inputEnd found") ))
		if (iLastFrame)
		{
			PRINT((_L("CVDecTestEngine::HandleNewBufferL, Getting last input buffer") ))
			GetInputBufferL();
		}
	}

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::LoadNextFrameL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::LoadNextFrameL(TVideoInputBuffer* aCodedInBuffer)
{
	// Normal VC1 video reading
	// Read file after the file header
	PRINT((_L("CVDecTestEngine::LoadNextFrameL, In")));
	unsigned int frameDataSize = 4;     //4 Bytes to obain the framesize
	unsigned int frameSize=0;
	unsigned int Timestamp;
	TInt err = 0;
	TUint8* ptr = new (ELeave) TUint8[frameDataSize]; // dynamic alloc 1
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem alloc, ptr") ))
	#endif
	TPtr8 frameData(ptr, frameDataSize);
	if ( (err = iInFile.Read(frameData, frameDataSize)) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, Error reading file") ))
		delete ptr;
		ptr = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr") ))
		#endif

		return err;
	}

	if (frameData.Length() < 4)
	{
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, File end found") ))
		aCodedInBuffer->iData.SetLength( 0 );
		iInputEnd = ETrue;
		delete ptr;
		ptr = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr") ))
		#endif
		return KErrNone;
	}

	PRINT((_L("CVDecTestEngine::LoadNextFrameL, Reading Item") ))
	ReadItemLE(ptr, 4, (TUint8 *)&frameSize);
	PRINT((_L("CVDecTestEngine::LoadNextFrameL, before AND frameSize: %x"), frameSize ))

	if (frameData.Length() < 4)
	{
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, File end found") ))
		iInputEnd = ETrue;
		delete ptr;
		ptr = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr") ))
		#endif
		return KErrNone;
	}

	//RCV_V2_FRAMESIZE_FLAGS
	frameSize = frameSize & ~(0xff000000);

	PRINT((_L("CVDecTestEngine::LoadNextFrameL, frameSize: %d"), frameSize ))

	//Now obtain the Timestamp of the frame
	if ( (err = iInFile.Read(frameData, frameDataSize)) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, Error reading file %d"), err ))
		return err;
	}

	PRINT((_L("CVDecTestEngine::LoadNextFrameL, Reading Item2") ))
	ReadItemLE(ptr, 4, (TUint8 *)&Timestamp);
	PRINT((_L("CVDecTestEngine::LoadNextFrameL, Timestamp: %d"), Timestamp ))

	delete ptr;
	ptr = NULL;
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr") ))
	#endif

	if (frameSize != 0)
	{

		tempFrame += frameSize + 8;
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, tota data %d"), tempFrame ))
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, frame"), iFrame))

		TInt currentPos = 0;
		iInFile.Seek(ESeekCurrent, currentPos );
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, currentPos %d"), currentPos ))
		currentPos = 0;

		//Send metadata in the first frame
		if (iFrame==0)
		{
			TInt pos = 0;
			//err = iInFile.Seek(ESeekCurrent,pos);
			err = iInFile.Seek(ESeekStart,pos);
			//        	pos -= 44; //header (36) and framesize+timestamp (8)
			//        	err = iInFile.Seek(ESeekCurrent,pos);
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, first frame")))
			if ( (err = iInFile.Read(aCodedInBuffer->iData, frameSize+44)) != KErrNone)
			{
				PRINT((_L("CVDecTestEngine::LoadNextFrameL, Error reading file %d"), err ))
				return err;
			}

			TUint8* ptr2 = new(ELeave)TUint8[4];       // dynamic alloc 2
			iMemAlloc++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem alloc, ptr2") ))
			#endif
			Mem::FillZ(ptr2,4);
			TPtr8 firstPtr(ptr2,4,4);
			//			aCodedInBuffer->iData.Insert(35,firstPtr);
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, frame1"), iFrame))
			TUint8* ptr = new ( ELeave ) TUint8[8];    // dynamic alloc 3
			iMemAlloc++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem alloc, ptr") ))
			#endif

			Mem::FillZ( ptr, 8 );
			ptr[0] = 1;
			ptr[4] = 44;
			TPtr8 anotherPtr(ptr, 8, 8);
			aCodedInBuffer->iData.Insert(0, anotherPtr );
			delete ptr2;
			ptr2 = NULL;
			delete ptr;
			ptr = NULL;
			iMemDelete += 2;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr2 & ptr") ))
			#endif
		}
		else
		{
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, iFrame num != 0: %d"), iFrame ))
			TInt pos = -8;

			err = iInFile.Seek(ESeekCurrent, pos);
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, read file: %d"), err ))
			if ( (err = iInFile.Read(aCodedInBuffer->iData, frameSize+8)) != KErrNone)
			{
				PRINT((_L("CVDecTestEngine::LoadNextFrameL, Error reading file %d"), err ))
				return err;
			}

			TUint8* ptr2 = new(ELeave)TUint8[4];         // dynamic alloc 4
			iMemAlloc++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem alloc, ptr2") ))
			#endif
			Mem::FillZ(ptr2,4);
			TPtr8 firstPtr(ptr2,4,4);
			//			aCodedInBuffer->iData.Insert(0, firstPtr);
			delete ptr2;
			iMemDelete++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, mem del, ptr2") ))
			#endif
		}

		TInt substreamID = 0;
		if(aCodedInBuffer->iUser != NULL){
			*((TInt*)aCodedInBuffer->iUser) = substreamID;
		}
		iInFile.Seek(ESeekCurrent, currentPos );
		PRINT((_L("CVDecTestEngine::LoadNextFrameL, currentPos %d"), currentPos ))
		currentPos = 0;

		aCodedInBuffer->iSequenceNumber = iFrame;
		aCodedInBuffer->iOptions = TVideoInputBuffer::EPresentationTimestamp;

		aCodedInBuffer->iPresentationTimestamp = Timestamp;
		iTimeToPlay = aCodedInBuffer->iPresentationTimestamp ;


		aCodedInBuffer->iDecodingTimestamp = Timestamp;
		iPresentationTimestamp = aCodedInBuffer->iPresentationTimestamp ;



	}
	iFrame++;
	PRINT(_L("CVDecTestEngine::LoadNextFrameL.. Out"));
	return frameSize;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadItemLE()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
//Currently Supporting only upto 4Bytes
TBool CVDecTestEngine::ReadItemLE(unsigned char *readBuffer, int Size,
unsigned char* pItem)
{
	PRINT((_L("CVDecTestEngine::ReadItemLE, In")));
	unsigned int High=0, Low=0;
	unsigned char m_char;
	int shift = 0;
	int numbytes = Size;
	int i=0;

	/* Read a little-endian file value into a variable, regardless of host endianness */
	while (numbytes-- >0)
	{
		m_char  = readBuffer[i++];

		if (shift >= 32)
		{
			High |= m_char << (shift-32);
		}
		else
		{
			Low  |= m_char << shift;
		}
		shift += 8;
	}

	switch (Size)
	{
		case 1:
		*pItem = (unsigned char)Low;
		break;

		case 2:
		*(unsigned short *)pItem = (unsigned short)Low;
		break;

		case 4:
		*(unsigned int *)pItem = (unsigned int)Low;
		break;

		default:
		//WARN("Unknown size (%d) item read requested",Size);
		//return false;
		return EFalse;
	}

	PRINT((_L("CVDecTestEngine::ReadItemLE, Out")));
	return TRUE;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::GoBackInFile()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::GoBackInFile(TUint aBytes)
{
	PRINT((_L("CVDecTestEngine::GoBackInFile, In")));

	//If read pointer is in the end of the buffer change it in the beginning
	if (iReadDataChunk - iDataChunk >= aBytes)
	{
		iReadDataChunk -= aBytes;
	}
	else
	{
		TInt endBytes = aBytes - (iReadDataChunk - iDataChunk);
		iReadDataChunk = iDataChunk+iDataChunkSize-endBytes;
	}

	PRINT((_L("CVDecTestEngine::GoBackInFile, Out, ")));
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadByteFromFile()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TUint8 CVDecTestEngine::ReadByteFromFile()
{
	//PRINT((_L("CVDecTestEngine::ReadByteFromFile, In")));
	if (iDataReadFromFile == 0)
	{
		TInt err = CheckAndFillDataChunk();
		if (err )
		{
			PRINT((_L("CVDecTestEngine::ReadByteFromFile, ERROR [%d]"), err));
			return 0;
		}
	}

	if (iReadDataChunk >= iDataChunk+iDataChunkSize)
	{
		iReadDataChunk = iDataChunk;
	}

	if (iEndOfData && iReadDataChunk == iWriteDataChunk && iDataReadFromFile != 0)
	{
		PRINT((_L("CVDecTestEngine::ReadByteFromFile, Input End found")));
		iInputEnd = ETrue;
		TUint8 value = 0;
		return value;
	}


	TUint8 byte = iReadDataChunk[0];
	iReadDataChunk++;
	return byte;

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadDataToBuffer()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::ReadDataToBuffer(TVideoInputBuffer* aCodedInBuffer, TInt aSize, TBool aDiscard)
{
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, In")));
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, iWriteDataChunk [%x]"), iWriteDataChunk));
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, iReadDataChunk [%x]"), iReadDataChunk));
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, iDataChunk [%x]"), iDataChunk));
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, size [%d]"), aSize));



	TInt err = CheckAndFillDataChunk();
	if (err )
	{
		PRINT((_L("CVDecTestEngine::ReadDataToBuffer, ERROR [%d]"), err));
		return;
	}

	aCodedInBuffer->iData.Zero();

	if (iEndOfData)
	{
		if (iReadDataChunk == iWriteDataChunk && iDataReadFromFile != 0)
		{
			PRINT((_L("CVDecTestEngine::ReadByteFromFile, Input End found")));
			iInputEnd = ETrue;
			return;
		}

		if (iReadDataChunk+aSize > iWriteDataChunk && iReadDataChunk < iWriteDataChunk)
		{
			PRINT((_L("CVDecTestEngine::ReadByteFromFile, Input End found")));
			if (!aDiscard)
			{
				aCodedInBuffer->iData.Append(iReadDataChunk, (iWriteDataChunk-iReadDataChunk));
			}
			iReadDataChunk += aSize;
			iInputEnd = ETrue;
			return;
		}
		else if (iReadDataChunk+aSize > iDataChunk+iDataChunkSize && iReadDataChunk > iWriteDataChunk &&
		iDataChunk+aSize-(iDataChunkSize+iDataChunk-iReadDataChunk) > iWriteDataChunk)
		{
			PRINT((_L("CVDecTestEngine::ReadByteFromFile, Input End found")));
			if (!aDiscard)
			{
				aCodedInBuffer->iData.Append(iReadDataChunk, (iDataChunkSize+(iDataChunk-iReadDataChunk)));
			}
			iReadDataChunk = iDataChunk;
			if (!aDiscard)
			{
				aCodedInBuffer->iData.Append(iReadDataChunk, (iWriteDataChunk-iReadDataChunk));
			}
			iReadDataChunk += iWriteDataChunk-iReadDataChunk;
			iInputEnd = ETrue;
			return;
		}
	}

	if (iReadDataChunk+aSize <= iDataChunk+iDataChunkSize)
	{
		if (!aDiscard)
		{
			aCodedInBuffer->iData.Append(iReadDataChunk, aSize);
		}
		iReadDataChunk +=aSize;
	}
	else if (iReadDataChunk+aSize > iDataChunk+iDataChunkSize)
	{
		PRINT((_L("CVDecTestEngine::ReadDataToBuffer, iReadDataChunk+aSize > iDataChunk+iDataChunkSize")));
		if (!aDiscard)
		{
			aCodedInBuffer->iData.Append(iReadDataChunk, (iDataChunkSize+iDataChunk-iReadDataChunk));
		}
		TInt end = aSize - ( iDataChunkSize+iDataChunk - iReadDataChunk );
		iReadDataChunk = iDataChunk;
		PRINT( ( _L( "CVDecTestEngine::ReadDataToBuffer, iReadDataChunk = iDataChunk") ) );
		if (!aDiscard)
		{
			aCodedInBuffer->iData.Append(iReadDataChunk, end);
		}
		iReadDataChunk += end;
	}

	err = CheckAndFillDataChunk();
	if (err )
	{
		PRINT((_L("CVDecTestEngine::ReadDataToBuffer, ERROR [%d]"), err));

		return;
	};
	PRINT((_L("CVDecTestEngine::ReadDataToBuffer, Out")));
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CheckAndFillDataChunk()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
TInt CVDecTestEngine::CheckAndFillDataChunk()
{
	PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, In")));

	TInt err = KErrNone;

	//If all the data is read return KerrNone
	if (iEndOfData)
	{
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, End of data")));
		return KErrNone;
	}

	TInt32 read = (TInt32)(iReadDataChunk);
	TInt32 write = (TInt32)(iWriteDataChunk);
	TInt32 chunk = (TInt32)(iDataChunkSize);
	TInt32 thres = (TInt32)(iDataThreshold);
	//First time fill the whole buffer

	PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, read %d, write %d, chunk %d, thres %d"), read, write, chunk, thres));

	if (iDataReadFromFile == 0)
	{
		TPtr8 temp(iWriteDataChunk, iDataChunkSize, iDataChunkSize);
		if ( (err = iInFile.Read(temp, iDataChunkSize)) != KErrNone)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Infile read [%d]"), err));
			return err;
		}
		if (temp.Length() < iDataChunkSize)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, end of file found [%d]"), temp.Length()));
			if(temp.Length()== 0){
				PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Error! Trying to read zero-sized input file")));
				iInputEnd = ETrue;
			}
			iEndOfData = ETrue;
			iWriteDataChunk += temp.Length();
			iDataReadFromFile += temp.Length();
			return KErrNone;
		}
		iDataReadFromFile += iDataChunkSize;
		for (TUint i = 0; i < 20; i++)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, [%x]"), iReadDataChunk[i]));
		}
	}
	else if (((read-write) > (chunk-thres)) && (iSentBuffCount != 0))
	{
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iReadDataChunk-iWriteDataChunk > iDataChunkSize-iDataThreshold")));
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iWriteDataChunk [%x]"), iWriteDataChunk));
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iReadDataChunk [%x]"), iReadDataChunk));
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iDataChunk [%x]"), iDataChunk));
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iReadDataChunk-iWriteDataChunk [%d]"), iReadDataChunk-iWriteDataChunk));
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iDataChunkSize-iDataThreshold [%d]"), iDataChunkSize-iDataThreshold));

		TPtr8 temp(iWriteDataChunk, iReadDataChunk-iWriteDataChunk, iReadDataChunk-iWriteDataChunk);
		if ( (err = iInFile.Read(temp, (iReadDataChunk-iWriteDataChunk))) != KErrNone)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Infile read [%d]"), err));
			return err;
		}
		if (temp.Length() < iReadDataChunk-iWriteDataChunk)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, End of file found [%d]"), temp.Length()));
			iEndOfData = ETrue;
			iWriteDataChunk += temp.Length();
			return KErrNone;
		}
		iWriteDataChunk += iReadDataChunk-iWriteDataChunk;
		iDataReadFromFile += iReadDataChunk-iWriteDataChunk;
	}
	else if (((write-read) < thres) && (iSentBuffCount != 0) && ((write-read) > 0))
	{
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, iReadDataChunk-iWriteDataChunk > -iDataThreshold")));
		//fill end of the buffer
		TPtr8 end(iWriteDataChunk, iDataChunk+iDataChunkSize-iWriteDataChunk, iDataChunk+iDataChunkSize-iWriteDataChunk);
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, TPtr created")));
		if ( (err = iInFile.Read(end, (iDataChunkSize+iDataChunk-iWriteDataChunk))) != KErrNone)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Infile read [%d]"), err));
			return err;
		}
		if (end.Length() < (iDataChunkSize+iDataChunk-iWriteDataChunk))
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, End of file found [%d]"), end.Length()));
			iEndOfData = ETrue;
			iWriteDataChunk += end.Length();
			return KErrNone;
		}

		//fill beginning of the buffer
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, end of data chunk filled [%d]"), end.Length()));
		TPtr8 begin(iDataChunk, iReadDataChunk-iDataChunk, iReadDataChunk-iDataChunk);
		if ( (err = iInFile.Read(begin, iReadDataChunk-iDataChunk)) != KErrNone)
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Infile read [%d]"), err));
			return err;
		}
		if (begin.Length() < (iReadDataChunk-iDataChunk))
		{
			PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, End of file found [%d]"), begin.Length()));
			iEndOfData = ETrue;
			iWriteDataChunk = iDataChunk+begin.Length();
			return KErrNone;
		}
		iWriteDataChunk = iDataChunk+begin.Length();
		PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, beginning of data chunk filled [%d]"), begin.Length()));
		iDataReadFromFile += iDataChunkSize-(iWriteDataChunk-iReadDataChunk);
	}
	else if (iWriteDataChunk == iDataChunk+iDataChunkSize)
	{
		iWriteDataChunk = iDataChunk;
	}

	PRINT((_L("CVDecTestEngine::CheckAndFillDataChunk, Out")));
	return err;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::PrintUncompressedFormat()
* Purpose:    Get Next input data
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::PrintUncompressedFormat(const TUncompressedVideoFormat& aFormat)
{
	PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, In")))

	switch (aFormat.iDataFormat)
	{
		case ERgbRawData:
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataFormat = ERgbRawData")))
		case ERgbFbsBitmap:
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataFormat = ERgbFbsBitmap")))
		switch(aFormat.iRgbFormat)
		{
			case ERgb16bit444:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = ERgb16bit444")))
			break;
			case ERgb16bit565:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = ERgb16bit565")))
			break;
			case ERgb32bit888:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = ERgb32bit888")))
			break;
			case EFbsBitmapColor4K:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = EFbsBitmapColor4K")))
			break;
			case EFbsBitmapColor64K:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = EFbsBitmapColor64K")))
			break;
			case EFbsBitmapColor16M:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = EFbsBitmapColor16M")))
			break;
			case EFbsBitmapColor16MU:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iRgbFormat = EFbsBitmapColor16MU")))
			break;
			default:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Error reading iRgbFormat [%x]"), aFormat.iRgbFormat))
		}
		break;
		case EYuvRawData:
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataFormat = EYuvRawData")))
		switch(aFormat.iYuvFormat.iCoefficients)
		{
			case EYuvRange0:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvRange0")))
			break;
			case EYuvRange1:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvRange1")))
			break;
			case EYuvBt709:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt709")))
			break;
			case EYuvBt709Range0:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt709Range0")))
			break;
			case EYuvBt709Range1:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt709Range1")))
			break;
			case EYuvBt601:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt601")))
			break;
			case EYuvBt601Range0:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt601Range0")))
			break;
			case EYuvBt601Range1:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = EYuvBt601Range1")))
			break;
			case ECustomYuvMatrix:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iCoefficients = ECustomYuvMatrix")))
			break;
			default:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Error reading iCoefficients [%x]"), aFormat.iYuvFormat.iCoefficients))
		}
		switch(aFormat.iYuvFormat.iPattern)
		{
			case EYuv420Chroma1:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iPattern = EYuv420Chroma1")))
			break;
			case EYuv420Chroma2:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iPattern = EYuv420Chroma2")))
			break;
			case EYuv420Chroma3:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iPattern = EYuv420Chroma3")))
			break;
			case EYuv422Chroma1:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iPattern = EYuv422Chroma1")))
			break;
			case EYuv422Chroma2:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iPattern = EYuv422Chroma2")))
			break;
			default:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Error reading iPattern [%x]"), aFormat.iYuvFormat.iPattern))
		}
		switch(aFormat.iYuvFormat.iDataLayout)
		{
			case EYuvDataPlanar:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataLayout = EYuvDataPlanar")))
			break;
			case EYuvDataInterleavedLE:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataLayout = EYuvDataInterleavedLE")))
			break;
			case EYuvDataInterleavedBE:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataLayout = EYuvDataInterleavedBE")))
			break;
			case EYuvDataSemiPlanar:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iDataLayout = EYuvDataSemiPlanar")))
			break;
			default:
			PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Error reading iDataLayout [%x]"), aFormat.iYuvFormat.iDataLayout))
		}
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iAspectRatioNum [%d]"), aFormat.iYuvFormat.iAspectRatioNum))
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, iAspectRatioDenom [%d]"), aFormat.iYuvFormat.iAspectRatioDenom))
		break;
		default :
		PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Error reading iDataFormat [%x]"), aFormat.iDataFormat))
	}

	PRINT((_L("CVDecTestEngine::PrintUncompressedFormat, Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ConvertToRealL()
* Purpose:    Converts the given descriptor to TReal
* Parameters: const TDesC& aPtr
* Return:     TReal
* --------------------------------------------------------------------------*/

TReal CVDecTestEngine::ConvertToRealL(const TDesC& aPtr)
{
	PRINT((_L("CVDecTestEngine::ConvertToReal(),In")))

	TInt point = aPtr.Find(_L("."));

	TBufC<7> integer;
	TBufC<7> decimal;
	if (point != KErrNotFound)
	{

		integer = aPtr.Mid(0, point);
		decimal = aPtr.Mid(point+1);
	}
	else
	{
		User::Leave(KErrArgument);
	}
	TInt decimalInt = 0;
	TInt integerInt = 0;
	TLex lexInt(integer);
	TLex lexDec(decimal);

	if (lexInt.Val(integerInt) != KErrNone || lexDec.Val(decimalInt) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::ConvertToReal(), error in descriptor")))
		User::Leave(KErrArgument);
	}


	TReal real = 1.0;
	TReal length = static_cast<TReal>(aPtr.Length()-point-1);
	TReal denom = pow(10.0, length);


	real = (static_cast<TReal>(integerInt)) + (static_cast<TReal>(decimalInt)/denom);

	PRINT((_L("CVDecTestEngine::ConvertToReal(),integer [%d]"), integerInt))
	PRINT((_L("CVDecTestEngine::ConvertToReal(),decimal [%d]"), decimalInt))
	PRINT((_L("CVDecTestEngine::ConvertToReal(),denom [%f]"), denom))
	PRINT((_L("CVDecTestEngine::ConvertToReal(),real [%f]"), real))

	PRINT((_L("CVDecTestEngine::ConvertToReal(),Out")))
	return real;
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoNewBuffers()
* Purpose:    Availability of Next buffer is notified
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::MdvpoNewBuffers()
{
	PRINT((_L("CVDecTestEngine::MdvpoNewBuffers(), Newbuffer is available")))

	//if ( iFrameMeasurement )
	//    return;

	if ( iState == ERunning)
	{
		iDecTestAO->RequestData();
	}


}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoReturnPicture()
* Purpose:    Postprocessor Input buffer is returned
* Note:       Used when input to Postprocessing
* Parameters: TVideoPicture* aPicture
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::MdvpoReturnPicture(TVideoPicture* aPicture)
{
	if ( iRawInBuffer == aPicture )
	{
		PRINT((_L("CVDecTestEngine::MdvpoReturnPicture, a Picture returned ")))
		iDecTestAO->RequestData();

	}
	else
	{
		PRINT((_L("CVDecTestEngine::MdvpoReturnPicture, Returned picture unknown")))
		iError = KErrGeneral;
	}
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoSupplementalInformation()
* Purpose:    Supplementary information is notified
* Note:
* Parameters: const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp, const TPictureId& aPictureId
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::MdvpoSupplementalInformation(const TDesC8& /*aData*/, const TTimeIntervalMicroSeconds& /*aTimestamp*/, const TPictureId& /*aPictureId*/)
{
	PRINT((_L("CVDecTestEngine::MdvpoSupplementalInformation  ")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoPictureLoss()
* Purpose:    Loss of a picture is notified
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoPictureLoss()
{
	iPictureLoss++;
	PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, A picture lost. ")))

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoPictureLoss()
* Purpose:    Loss of pictures is notified
* Note:
* Parameters: const TArray<TPictureId>& aPictures
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoPictureLoss(const TArray<TPictureId>& aPictures)
{
	TInt count = aPictures.Count();
	iPictureLoss += count;
	PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, [%d] pictures lost. "),count))

	for ( TInt i =0; i<count;i++ )
	{
		TPictureId id = aPictures[i];
		PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, Picure:[%d]"),i))

		/*       if(id.iIdType == TPictureId::EPictureNumber)
		PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, Picture Type:Picture Number, ID:[%d]"),id.iId))
		else if (id.iIdType == TPictureId::ETemporalReference)
		PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, Picture Type:Temporal Reference, ID:[%d]"),id.iId))
		else
		PRINT((_L("CVDecTestEngine::MdvpoPictureLoss, Picture Type:Unidentified piture")))
		*/
	}
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoSliceLoss
* Purpose:    Loss of slice is notified
* Note:
* Parameters: TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
{

	PRINT((_L("CVDecTestEngine::MdvpoSliceLoss, In")))

	//    PRINT((_L("CVDecTestEngine::MdvpoSliceLoss, [%d] consecutive Mactoblocks from [%d] in raster-scan order are lost"),aPicture.aNumMacroblocks ,aPicture.aFirstMacroblock))
	/*    if( aPicture.iIdType == TPictureId::EPictureNumber)
	PRINT((_L("CVDecTestEngine::MdvpoSliceLoss, Picture Type:Picture Number, ID:[%d]"),aPicture.iId))
	else if (aPicture.iIdType == TPictureId::ETemporalReference)
	PRINT((_L("CVDecTestEngine::MdvpoSliceLoss, Picture Type:Temporal Reference, ID:[%d]"),aPicture.iId))
	else
	PRINT((_L("CVDecTestEngine::MdvpoSliceLoss, Picture Type:Unidentified piture")))
	*/

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoReferencePictureSelection
* Purpose:
* Note:
* Parameters: const TDesC8& aSelectionData
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
{
	PRINT((_L("CVDecTestEngine::MdvpoReferencePictureSelection ")))

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoTimedSnapshotComplete()
* Purpose:
* Note:
* Parameters: TInt aError, TPictureData* aPictureData, const TTimeIntervalMicroSeconds& aPresentationTimestamp, const TPictureId& aPictureId
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoTimedSnapshotComplete(TInt aError, TPictureData* aPictureData, const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, const TPictureId& aPictureId)
{
	PRINT((_L("CVDecTestEngine::MdvpoTimedSnapshotComplete(),In, error [%d]"), aError))

	if ( aError == KErrNone )
	{
		RFile snapshot;

		TFileName filename;
		TBuf8<128> newFile;
		newFile.Delete(newFile.Length()-5, 4);
		newFile.Append(_L("_id_"));
		newFile.AppendNum(aPictureId.iId);
		newFile.Append(_L(".yuv"));
		filename.Copy(newFile);

		TInt err = snapshot.Replace(iFs, filename, EFileShareExclusive|EFileWrite);

		if ( err != KErrNone )
		{
			PRINT((_L("CVDecTestEngine::GetSnapshotL, snapshot file open failed")))
			snapshot.Close();
			User::Leave(err);
		}
		else
		{
			err = snapshot.Write(*(aPictureData->iRawData),aPictureData->iRawData->Size());
			if ( err!= KErrNone )
			{
				PRINT((_L("CVDecTestEngine::GetSnapshotL, snapshot file write failed")))
				snapshot.Close();
				User::Leave(err);
			}
		}
		PRINT((_L("CVDecTestEngine::GetSnapshotL, Picture size"),aPictureData->iRawData->Size()));
		snapshot.Close();
	}
	PRINT((_L("CVDecTestEngine::MdvpoTimedSnapshotComplete(),Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoNewPictures()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoNewPictures()
{

	PRINT((_L("CVDecTestEngine::MdvpoNewPictures(),In, New output picture is available")))

	GetReturnedPictureL();
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoFatalError()
* Purpose:    Fatal error is notified
* Note:       Decoder/Postprocessor to be deleted
* Parameters: TInt aError
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoFatalError(TInt aError)
{
	iError = aError;
	PRINT((_L("CVDecTestEngine::MdvpoFatalError, FATAL ERROR [%d]"),aError))
	iState = EStopped;
	MdvpoStreamEnd();

	iTestClass->MvdeSetError(aError);
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoInitComplete()
* Purpose:    Completion of initialization is notified
* Note:
* Parameters: TInt aError
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoInitComplete(TInt aError)
{
	PRINT((_L("CVDecTestEngine::MdvpoInitComplete, In")))
	if ( iRunning )
	{
		PRINT((_L("CVDecTestEngine::MdvpoInitComplete, Stopping SchedulerWait")))
		iScheduler->AsyncStop();
		delete iScheduler;
		iScheduler = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::MdvpoInitComplete, mem del, iScheduler") ))
		#endif
	}

	if ( aError != KErrNone )
	{
		iError = aError;
		PRINT((_L("CVDecTestEngine::MdvpoInitComplete, Error code [%d]"),aError))
		return;
	}

	if (iDeleteDecoderFromInitComp)
	{
		//TearDown();
		delete iDevvp;
		iDevvp = NULL;
		iMemDelete++;
		#ifdef __MEM_CHECK_
		PRINT((_L("CVDecTestEngine::MdvpoInitComplete, mem del, iDevvp") ))
		#endif
		iState = EStateNone;
		PRINT((_L("CVDecTestEngine::MdvpoInitComplete,DevVideo deleted")))
	}

	iState = EInitialized;
	PRINT((_L("CVDecTestEngine::MdvpoInitComplete, Initialized")))

	//Initilize parameters for Decoding/Playback
	iCodedInBuffer = NULL;
	iOutBuffer =NULL;
	iSentBuffCount = 0;
	iReturnedBuffCount = 0;
	iPictureLoss =0;
	iInputEnd = EFalse;
	iStreamEnd = EFalse;
	iTimeStamp = 0;
	iNoBuffAvailable = 0;

	iTestClass->MvdeStreamEndReached();  // Notify observer data transfer is complete
	PRINT((_L("CVDecTestEngine::MdvpoInitComplete, Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MdvpoStreamEnd()
* Purpose:    End of stream is notified
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::MdvpoStreamEnd()
{
	iStreamEnd = ETrue;
	if (iOutBufferTemp)
	{
		iDevvp->ReturnPicture(iOutBufferTemp);
	}
	PRINT((_L("CVDecTestEngine::MdvpoStreamEnd(), Stream end")))

	if (iFrameMeasurement && iFrameTime != 0)
	{
		TReal framerate = (TReal)(iReturnedBuffCount-1) / iFrameTime;
		PRINT((_L("CVDecTestEngine::MdvpoStreamEnd(), Frame rate: [%f] fps"), framerate))
	}
	if(iInstantFpsEnabled)
	{
		TInt err = KErrNone;
		for(TUint i=0; i < iInstantFpsList.Count(); i++ )
		{
			_LIT(KEnd, "\n");
			TBuf8 <0x100> tempBuff;
			TTimeIntervalMicroSeconds time = iInstantFpsList[ i ];
			tempBuff.AppendNum((TInt64)time.Int64());
			tempBuff.Append(KEnd);
			err = iFpsFile.Write(tempBuff,tempBuff.Size() );
			if( err != KErrNone )
			{
				PRINT((_L("CVDecTestEngine::MdvpoStreamEnd(), iFpsFile write error: [%d]"), err))
			}
		}
	}
	iTestClass->MvdeStreamEndReached();  // Notify observer data transfer is complete
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MmvbmoNewBuffers()
* Purpose:    Observer callback in Custom interface Buffer management
*             Client is notified a new buffer(s) is available
* Note:       This is used when custom interface buffer management is used
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

#ifdef __CI_HEADERS__

void CVDecTestEngine::MmvbmoNewBuffers()
{

	PRINT((_L("CVDecTestEngine::MmvbmoNewBuffers(), New input buffer is available")))
	if ( (iState == ERunning) && iCIBuffMgmtOn )
	{
		iDecTestAO->RequestData();
	}

}
#endif
/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::MmvbmoReleaseBuffers()
* Purpose:    Observer callback in Custom interface Buffer management
*             Client is notified to release all buffers it has, using MmvbmReleaseBuffer
* Note:       This is used when custom interface buffer management is used
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
#ifdef __CI_HEADERS__
void CVDecTestEngine::MmvbmoReleaseBuffers()
{
	if (iRawInBuffer)
	{
		iCIBuffMgmt->MmvbmReleaseBuffer(iRawInBuffer);
		iRawInBuffer = NULL;
	}
	PRINT((_L("CVDecTestEngine::MmvbmoReleaseBuffers(), buffer released")))
}

#endif

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::
* Purpose:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
#ifdef __RESOURCENOTIFICATION__
void CVDecTestEngine::MmvroResourcesLost(TUid aMediaDevice)
{
	PRINT((_L("CVDecTestEngine::MmvroResourcesLost(), Uid [%x]"), aMediaDevice.iUid))

	if (iDecoderUid.iUid != aMediaDevice.iUid)
	{
		PRINT((_L("CVDecTestEngine::MmvroResourcesLost, UID is different")))
		return;
	}

	Pause();

	iDecTestAO->Cancel();
	if ( iDirectScreenAccess && iDsaStarted )
	{
		iDevvp->AbortDirectScreenAccess();
		iDsaStarted = EFalse;
		PRINT((_L("CVDecTestEngine::MmvroResourcesLost, DSA is aborted ")))
	}


	PRINT((_L("CVDecTestEngine::MmvroResourcesLost, Out ")))
}
#endif

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::
* Purpose:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/
#ifdef __RESOURCENOTIFICATION__
void CVDecTestEngine::MmvroResourcesRestored(TUid aMediaDevice)
{
	PRINT((_L("CVDecTestEngine::MmvroResourcesRestored(), Uid [%x]"), aMediaDevice.iUid))

	if (iDecoderUid.iUid != aMediaDevice.iUid)
	{
		PRINT((_L("CVDecTestEngine::MmvroResourcesRestored, UID is different")))
		return;
	}

	if ( iDirectScreenAccess && !iDsaStarted )
	{
		TRAPD(err, StartDirectScreenAccessL());
		if (err != KErrNone)
		{
			PRINT((_L("CVDecTestEngine::MmvroResourcesRestored, couldn't start DSA, err[%d]"),err))
			User::Leave(err);
		}
		else
		{
			PRINT((_L("CVDecTestEngine::MmvroResourcesRestored, DSA started")))
		}
	}


	// Start DevVideoPlay
	Resume();

	PRINT((_L("CVDecTestEngine::MmvroResourcesRestored(), Out")))
}

#endif

/* ----------------------------------------------------------------------------
* Name:       CVDecTestAO::CVDecTestAO()
* Purpose:    Constructor
* Parameters: CVDecTestEngine* aTestEngine
* Return:     None
* --------------------------------------------------------------------------*/

CVDecTestAO::CVDecTestAO(CVDecTestEngine* aTestEngine) :
CActive(EPriorityStandard)
{
	iTestEngine = aTestEngine;
	CActiveScheduler::Add(this);

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestAO::~CVDecTestAO()
* Purpose:    Deconstructor
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

CVDecTestAO::~CVDecTestAO()
{

	Cancel();

}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestAO::RequestData()
* Purpose:    Asyncronous request of next input data
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestAO::RequestData()
{

	if ( !IsActive() )
	{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);

		SetActive();
	}
	else
	{
		PRINT((_L("CVDecTestAO::RequestData(), Async request already active")))
	}
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestAO::RunL()
* Purpose:    Active object RunL
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestAO::RunL()
{
	if (iTestEngine->iState == EPaused || iTestEngine->iState == EStopped)
	{
		return;
	}
	iTestEngine->HandleNewBufferL();
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestAO::DoCancel()
* Purpose:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestAO::DoCancel()
{

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CorruptInput()
* Purpose:    Make the input file corrupted to test codec's robustness
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::CorruptInput(TInt aFilter, TInt aInterval,
TInt aFrameNum, TInt aPct)
{
	PRINT((_L("CVDecTestEngine::CorruptInput(),In")))

	iCorruptMarker = ETrue;
	iCorruptCounter = 0;

	iFilter = aFilter;
	iInterval = aInterval;
	iFrameNum = aFrameNum;
	iPercentage = aPct;

	PRINT((_L("CVDecTestEngine::CorruptInput(),Out")))
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CorruptEngine()
* Purpose:    Make the content in the buffer corrupted
* Note:
* Parameters:
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::CorruptEngineL()
{
	PRINT((_L("CVDecTestEngine::CorruptEngin(), In")))
	if( iInputEnd )
	{
		return;
	}
	if( iFrameCounter <= iInterval )
	{
		if( iFrameCounter < iInterval )
		{
			iFrameCounter++;
			return;
		}
		else
		{
			iCorruptCounter++;
		}
	}
	else
	{
		User::Leave( KErrGeneral);
	}
	TInt frameLength = iCodedInBuffer->iData.Size();
	TInt maxLength = iCodedInBuffer->iData.MaxLength();

	TUint8* memoryLocation = NULL;
	TPtr8 tmpPointer(memoryLocation, frameLength, maxLength );
	HBufC8* tempBuff = NULL;
	tempBuff = HBufC8::NewL(iInBuffSize);
	CleanupStack::PushL( tempBuff );
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::CorruptEngineL, mem alloc, tempBuff") ))
	#endif
	tmpPointer.Set(tempBuff->Des());

	if( iFilter == 0 )
	{
		//Filter 0
		PRINT((_L("CVDecTestEngine::CorruptEngin(),Filter 0, In")))


		TInt lengthCopy = (TInt)( frameLength * iPercentage / 100 );


		tmpPointer.Set(iCodedInBuffer->iData);

		TInt corruptLength = frameLength - lengthCopy;
		TInt corruptLen = 0;
		for( TInt i = 0; i < corruptLength; i++ )
		{
			tmpPointer.Append('F');
			corruptLen++;
		}
		PRINT((_L("CVDecTestEngine::CorruptEngin(),[%d] bytes are corrupted"), corruptLen ) )
		iCodedInBuffer->iData.Set(tmpPointer);

		PRINT((_L("CVDecTestEngine::CorruptEngin(),Filter 0, Out")))
	}
	else
	{
		// Filter 1
		PRINT((_L("CVDecTestEngine::CorruptEngin(),Filter 1, In")))
		// To be added here
		TInt headerLen = 0;
		if( iFrame == 0 )
		{
			headerLen = 16;

		}
		else
		{
			headerLen = 8;
		}
		TInt increment = (TInt)( ( frameLength - headerLen )/ ( ( frameLength - headerLen) * iPercentage / 100) ) - 1;

		for(TInt i = headerLen; i < frameLength;  )
		{

			TUint8* memorySeg = NULL;
			TPtr8 segCopy(memorySeg,increment);
			HBufC8* segBuff = NULL;
			segBuff = HBufC8::NewL( increment );
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::CorruptEngine, mem alloc, segBuff") ))
			#endif
			iMemAlloc++;

			CleanupStack::PushL( segBuff );
			segCopy.Set(segBuff->Des());

			if( i < frameLength - increment - 1 )
			{
				segCopy = iCodedInBuffer->iData.MidTPtr(i, increment );
				segCopy.Append('F');
				i++;
				PRINT((_L("CVDecTestEngine::CorruptEngin(),[%d]th byte is corrupted"), i ) )
				tmpPointer.Append( segCopy );
				i += increment;
			}
			else // end of the frame
			{
				segCopy = iCodedInBuffer->iData.MidTPtr(i, (frameLength - i) );
				tmpPointer.Append( segCopy );
				i = frameLength;
			}

			CleanupStack::PopAndDestroy( segBuff );
			iMemDelete++;
			#ifdef __MEM_CHECK_
			PRINT((_L("CVDecTestEngine::CorruptEngine, mem del, segBuff") ))
			#endif
		}
		iCodedInBuffer->iData.Set(tmpPointer);

		PRINT((_L("CVDecTestEngine::CorruptEngin(),Filter 1, Out")))
	}
	if( iCorruptCounter == iFrameNum )
	{
		iFrameCounter = 0;
		iCorruptCounter = 0;
	}
	CleanupStack::PopAndDestroy( tempBuff );
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::CorruptEngine, mem del, tempBuff") ))
	#endif
	PRINT((_L("CVDecTestEngine::CorruptEngin(), Out")))
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CorruptEngineL(TVideoInputBuffer*
aCodedInBuffer)
* Purpose:    Make the content in the buffer corrupted
* Note:
* Parameters: TVideoInputBuffer* aCodedInBuffer
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::CorruptEngineL( TVideoInputBuffer* aCodedInBuffer )
{
	PRINT((_L("CVDecTestEngine::CorruptEngin(TVideoInputBuffer* aCodedInBuffer), In")))
	iFrameWriteLength = aCodedInBuffer->iData.Length();
	if( iInputEnd )
	{
		return;
	}
	if( iFrameCounter <= iInterval )
	{
		if( iFrameCounter < iInterval )
		{
			iFrameCounter++;
			PRINT((_L("CVDecTestEngine::CorruptEngin(TVideoInputBuffer* aCodedInBuffer), Out")))
			return;
		}
		else
		{
			iCorruptCounter++;
		}
	}
	else
	{
		User::Leave( KErrGeneral);
	}

	TInt frameLength = aCodedInBuffer->iData.Size();
	TInt maxLength = aCodedInBuffer->iData.MaxLength();

	TUint8* memoryLocation = NULL;
	TPtr8 tmpPointer(memoryLocation, frameLength, maxLength );
	HBufC8* tempBuff = NULL;
	tempBuff = HBufC8::NewL(iInBuffSize);
	iMemAlloc++;
	CleanupStack::PushL( tempBuff );
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::CorruptEngine, mem alloc, tempBuff") ))
	#endif
	tmpPointer.Set(tempBuff->Des());

	if( iFilter == 0 )
	{
		//Filter 0
		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 0, In")))
		tmpPointer.Set( aCodedInBuffer->iData );
		PRINT((_L("Length of tmpPointer, %d"), tmpPointer.Length() ) )

		TInt corruptLength = (TInt)( ( frameLength - 8 )* iPercentage / 100 );

		TInt copyLength = frameLength - corruptLength;

		TInt corruptLen = 0;

		for( TInt i = copyLength; i < frameLength; i++ )
		{
			// Corrupt the byte by chaning its value instead of setting "zero"
			if( tmpPointer[i] < 0x05 )
			{
				tmpPointer[i] += 0x05;
			}
			else
			{
				tmpPointer[i] -= 0x05;
			}
			corruptLen++;
		}
		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),[%d] bytes are corrupted"), corruptLen ) )

		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 0, Out")))
	}
	else if( iFilter == 1)
	{
		// Filter 1
		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 1, In")))
		tmpPointer.Set( aCodedInBuffer->iData );
		PRINT((_L("Length of tmpPointer, %d"), tmpPointer.Length() ) )
		// To be added here
		// first 8 bytes of each frame should not be corrupted
		TInt headerLen = 8;

		TInt increment = (TInt)( ( frameLength - headerLen )/ ( ( frameLength - headerLen) * iPercentage / 100) ) - 1;
		TInt corruptLen = 0;
		for(TInt i = headerLen + 1; i < frameLength;  i += increment)
		{
			// Corrupt the byte by chaning its value instead of setting "zero"
			if( tmpPointer[i] < 0x05 )
			{
				tmpPointer[i] += 0x05;
			}
			else
			{
				tmpPointer[i] -= 0x05;
			}
			// tmpPointer[i] = 0x00;
			corruptLen ++;
		}
		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),[%d] bytes are corrupted"), corruptLen ) )

		PRINT(_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 1, Out"))
	}
	else if( iFilter == 2)
	{
		PRINT(_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 2, In"))
		tmpPointer.Set( aCodedInBuffer->iData );
		PRINT((_L("Length of tmpPointer, %d"), tmpPointer.Length() ) )

		TInt corruptLength = (TInt)( ( frameLength - 8 )* iPercentage / 100 );

		//TInt copyLength = frameLength - corruptLength;
		iFrameWriteLength -= corruptLength;
		PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),[%d] bytes are removed"), corruptLength ) )
		// Modify the framesize value in the frame header
		TUint32 finalSize = (TUint32)( iFrameWriteLength - 8 );
		for( TInt i = 0; i < 3; i++ )
		{
			tmpPointer[i] = (TUint8) ( (  finalSize & ( (0x000000ff)<<(8*i) ) )>> (i*8) );
		}
		/* the above assignment equals followings
		tmpPointer[0] = (TUint8)( finalSize & ~(0xffffff00) );
		tmpPointer[1] = (TUint8)( ( finalSize & ~(0xffff00ff))>>8 );
		tmpPointer[2] = (TUint8)( ( finalSize & ~(0xff00ffff))>>16 );
		*/

		PRINT(_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 2, Out"))
	}
	else      // iFilter == 3
	{
		PRINT(_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 3, In"))
		tmpPointer.Set( aCodedInBuffer->iData );
		TInt corruptLength = (TInt)( ( frameLength - 8 )* iPercentage / 100 );

		//TInt copyLength = frameLength - corruptLength;
		iFrameWriteLength -= corruptLength;
		// Modify the framesize value in the frame header
		TUint32 finalSize = (TUint32)( iFrameWriteLength - 8 );
		for( TInt i = 0; i < 3; i++ )
		{

			tmpPointer[i] = (TUint8) ( (  finalSize & ( (0x000000ff)<<(8*i) ) )>> (i*8) );
		}
		TInt headerLen = 8;
		// Copy the other bytes for the header
		for( TInt i = 3; i < headerLen; i++ )
		{

			tmpPointer[i] = aCodedInBuffer->iData[i];
		}
		TInt increment = (TInt)( ( frameLength - headerLen )/ ( ( frameLength - headerLen) * iPercentage / 100) ) - 1;
		TInt incrementCounter = 0;
		TInt byteCounter = 0;
		TInt j = headerLen;
		for( TInt i = headerLen; i < iFrameWriteLength; i++ )
		{

			// metadata assignment
			tmpPointer[i] = aCodedInBuffer->iData[j];
			if( byteCounter < corruptLength )
			{
				incrementCounter++;
			}

			j++;
			if( incrementCounter == increment )
			{
				j++;
				incrementCounter = 0;
				byteCounter++;
			}
		}


		PRINT(_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer),Filter 3, Out"))
	}



	aCodedInBuffer->iData.Set(tmpPointer);
	if( iCorruptCounter == iFrameNum )
	{
		iFrameCounter = 0;
		iCorruptCounter = 0;
	}
	CleanupStack::PopAndDestroy( tempBuff );
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::CorrupEngineL, mem del, tempBuff") ))
	#endif
	PRINT((_L("CorruptEngineL(TVideoInputBuffer* aCodedInBuffer), Out")))
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::CreateCorruptedFile()
* Purpose:    Make the content in the buffer corrupted and output to a file
* Note:
* Parameters:
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::CreateCorruptedFileL()
{
	PRINT((_L("CVDecTestEngine::CreateCorruptedFileL, In")));
	// Only support VC-1 decoder

	// make new buffer for the data read from the file
	HBufC8* tmpBuff = NULL;

	TVideoInputBuffer* codedBuffer = new(ELeave) TVideoInputBuffer;
	CleanupStack::PushL( codedBuffer );

	tmpBuff = HBufC8::NewL(iInBuffSize);
	CleanupStack::PushL( tmpBuff );
	codedBuffer->iData.Set(tmpBuff->Des());
	// read content of the file in the buffer
	TInt err = 0;

	TInt frameDropNumCounter = 0;
	while ( !iInputEnd )
	{
		// Read the VC1 input video from the very beginning
		if( iCodecType == EVC1)
		{
			err = ReadVC1FrameL( codedBuffer );
		}
		else if( iCodecType == EMPEG4 )
		{
			err = ReadMPEG4FrameL( codedBuffer );
		}
		else
		{

		}


		if( err < 0)
		{
			PRINT((_L("CVDecTestEngine::CreateCorruptedFileL, Error reading file") ))
			return KErrGeneral;
		}
		if( err == 0 )
		{
			PRINT((_L("CVDecTestEngine::LoadNextFrameL, End of file is found") ))
			break;
		}
		else
		{
			// the frame is successfully read and ready to be corrupted

			// make some bytes corrupted
			if( !iFrameDropMarker )
			{
				CorruptEngineL( codedBuffer );
				PRINT((_L("Corruption is done")));
			}
		}

		// Prepare a buffer for copying the bytes from the frame buffer
		/*
		TInt copyLen = codedBuffer->iData.Size();

		TUint8* memorySeg = NULL;
		TPtr8 segCopy(memorySeg,copyLen);
		HBufC8* segBuff = NULL;
		segBuff = HBufC8::NewL( copyLen );
		CleanupStack::PushL( segBuff );
		segCopy.Set(segBuff->Des());
		*/
		if( iFrameDropMarker )
		{
			iFrameWriteLength = codedBuffer->iData.Length();
			// drop the frame specified by the user
			if( iFrameDropCounter < iFrameDropInterval )
			{
				PRINT((_L("CVDecTestEngine::CreateCorruptedFileL,iFrameWriteLength [%d]"), iFrameWriteLength ) )
				err = iOutCorruptedFile.Write(codedBuffer->iData, iFrameWriteLength );
				PRINT((_L("Output to the file for frame drop successful")));
				if ( err!= KErrNone )
				{
					PRINT((_L("Output corrupted File write Failed")));
					iOutCorruptedFile.Close();
				}



			}
			else
			{
				frameDropNumCounter++;
				PRINT((_L("Frame is dropped!")));
			}

			if( frameDropNumCounter == iFrameDropNum )
			{
				iFrameDropCounter = 0;
				frameDropNumCounter = 0;
			}
		}
		else //write corrupted frame to the file
		{
			// Prepare a buffer for copying the bytes from the frame buffer
			/*         TInt copyLen = codedBuffer->iData.Size();

			TUint8* memorySeg = NULL;
			TPtr8 segCopy(memorySeg,copyLen);
			HBufC8* segBuff = NULL;
			segBuff = HBufC8::NewL( copyLen );
			CleanupStack::PushL( segBuff );
			segCopy.Set(segBuff->Des());

			*/
			PRINT((_L("CVDecTestEngine::CreateCorruptedFileL,iFrameWriteLength [%d]"), iFrameWriteLength ) )
			err = iOutCorruptedFile.Write(codedBuffer->iData, iFrameWriteLength );

			if ( err!= KErrNone )
			{
				PRINT((_L("Output corrupted File write Failed")));
				iOutCorruptedFile.Close();
			}
			PRINT((_L("Output corrupted File is successful")));

		}

	}
	iOutCorruptedFile.Close();
	CleanupStack::PopAndDestroy( tmpBuff );
	CleanupStack::PopAndDestroy( codedBuffer );
	PRINT((_L("CVDecTestEngine::CreateCorruptedFileL, Out")));
	return KErrNone;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::OpenFileToWriteL()
* Purpose:    Open output File To Write
* Note:       Memory buffer output is used
* Parameters: TFileName& aOutFileName
* Return:     None
* --------------------------------------------------------------------------*/


void CVDecTestEngine::OpenFileToWriteL(TFileName& aInFileName)
{
	PRINT((_L("CVDecTestEngine::OpenFileToWrite, In")));
	TInt err = KErrNone;

	iOutCorruptedFileName.Copy(aInFileName);

	err = iOutCorruptedFile.Replace(iFs, iOutCorruptedFileName, EFileShareExclusive|EFileWrite);

	iOutCorruptedFileOpen = ETrue;


	if ( err!= KErrNone )
	{
		PRINT((_L("CVDecTestEngine::OpenFileToWrite, Output corrupted File Replace Failed")));
		iOutCorruptedFile.Close();
	}

	AssertTIntEqualL(KErrNone,err);
	PRINT((_L("CVDecTestEngine::OpenFileToWrite, Out")));
}


/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadVC1FrameL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::ReadVC1FrameL(TVideoInputBuffer* aCodedInBuffer)
{
	// Note: this function is used as part of corrupt engine to make input
	// VC1 video corrupted for codec robustness test
	// Reading VC1 input video from the very beginning of the file
	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, In")));
	unsigned int frameDataSize = 4;     //4 Bytes to obain the framesize
	unsigned int frameSize=0;
	unsigned int Timestamp;
	TInt err = 0;
	TUint8* ptr = new (ELeave) TUint8[frameDataSize];
	iMemAlloc++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, mem alloc, ptr") ))
	#endif
	TPtr8 frameData(ptr, frameDataSize);
	if ( (err = iInFile.Read(frameData, frameDataSize)) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Error reading file") ))
		return err;
	}

	if (frameData.Length() < 4)
	{
		PRINT((_L("CVDecTestEngine::ReadVC1FrameL, File end found") ))
		aCodedInBuffer->iData.SetLength( 0 );
		delete ptr;
		ptr = NULL;
		iMemDelete++;
		iInputEnd = ETrue;
		return KErrNone;
	}

	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Reading Item") ))
	ReadItemLE(ptr, 4, (TUint8 *)&frameSize);
	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, before AND frameSize: %x"), frameSize ))

	//RCV_V2_FRAMESIZE_FLAGS
	frameSize = frameSize & ~(0xff000000);

	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, frameSize: %d"), frameSize ))

	//Now obtain the Timestamp of the frame
	if ( (err = iInFile.Read(frameData, frameDataSize)) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Error reading file %d"), err ))
		return err;
	}

	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Reading Item2") ))
	ReadItemLE(ptr, 4, (TUint8 *)&Timestamp);
	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Timestamp: %d"), Timestamp ))
	delete ptr;
	iMemDelete++;
	#ifdef __MEM_CHECK_
	PRINT((_L("CVDecTestEngine::ReadVC1FrameL, mem del, ptr") ))
	#endif
	if (frameSize != 0)
	{
		//Send metadata in the first frame
		if (iFrame==0)
		{
			PRINT((_L("CVDecTestEngine::ReadVC1FrameL, iFrame num == 0, %d"), iFrame ))
			TInt pos = - ( KVc1HeaderLength + 8 );  // assume the codec is configured
			err = iInFile.Seek(ESeekCurrent,pos);

			// header length is 36, KVc1HeaderLength = 36
			PRINT((_L("CVDecTestEngine::ReadVC1FrameL, file header, 36 bytes")))
			if ( (err = iInFile.Read(aCodedInBuffer->iData, KVc1HeaderLength)) != KErrNone)
			{
				PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Error reading file %d"), err ))
				return err;
			}

		}
		else
		{
			PRINT((_L("CVDecTestEngine::ReadVC1FrameL, iFrame num = %d"), iFrame ))
			TInt pos = -8;
			err = iInFile.Seek(ESeekCurrent, pos);
			PRINT((_L("CVDecTestEngine::ReadVC1FrameL, read file: %d"), err ))
			if ( (err = iInFile.Read(aCodedInBuffer->iData, frameSize+8)) != KErrNone)
			{
				PRINT((_L("CVDecTestEngine::ReadVC1FrameL, Error reading file %d"), err ))
				return err;
			}
		}

		TInt currentPos = 0;
		iInFile.Seek(ESeekCurrent, currentPos );
		PRINT((_L("CVDecTestEngine::ReadVC1FrameL, currentPos %d"), currentPos ))
		currentPos = 0;

		/*	Not necessary
		aCodedInBuffer->iSequenceNumber = iFrame;
		aCodedInBuffer->iOptions = TVideoInputBuffer::EPresentationTimestamp;

		aCodedInBuffer->iPresentationTimestamp = Timestamp;
		iTimeToPlay = aCodedInBuffer->iPresentationTimestamp ;


		aCodedInBuffer->iDecodingTimestamp = Timestamp;
		iPresentationTimestamp = aCodedInBuffer->iPresentationTimestamp ;
		*/


	}

	iFrame++;
	iFrameDropCounter++;
	PRINT(_L("CVDecTestEngine::ReadVC1FrameL.. Out"));
	return frameSize;
}



/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::ReadMPEG4FrameL()
* Purpose:
* Note:
* Parameters: None
* Return:     None
* --------------------------------------------------------------------------*/

TInt CVDecTestEngine::ReadMPEG4FrameL(TVideoInputBuffer* aCodedInBuffer)
{
	// Note: this function is used as part of corrupt engine to make input
	// MPEG4 video corrupted for codec robustness test
	// Reading MPEG4 input video from the very beginning of the file
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, In")));
	TInt buffLength = iInBuffSize;

	//TUint8* testData =NULL;
	TInt err = KErrNone;

	if ( aCodedInBuffer->iData.MaxLength() < buffLength )
	{
		err = KErrOverflow;
		PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, Buffer is small, [%d]Byte"),aCodedInBuffer->iData.MaxLength()))
		return err;
	}

	aCodedInBuffer->iData.SetLength(0);

	//Buffer is not full,  reset size of buffer
	if ( aCodedInBuffer->iData.Length() < buffLength)
	{
		buffLength = aCodedInBuffer->iData.Length();
	}



	TInt pos = 0;
	TBool found = EFalse;
	TBool firstStartCode = EFalse;
	//TUint8 byte = 0;
	TInt currentPos = 0;
	iInFile.Seek(ESeekCurrent, currentPos );
	if( currentPos != 0 && currentPos == iCurrentFilePos )
	{
		iInputEnd = ETrue;
	}
	else
	{
		iCurrentFilePos = currentPos;
	}
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, currentPos %d"), currentPos ))
	while (!iInputEnd && !found)
	{
		TInt zeroCounter = 0;
		err = iInFile.Read(aCodedInBuffer->iData, 1 ); // read one byte
		if( aCodedInBuffer->iData.Length() == 0 )
		{
			pos++;
			iInputEnd = ETrue;
			break;
		}

		while( aCodedInBuffer->iData[0] == 0x00 && !iInputEnd )
		{
			zeroCounter++;
			pos++;
			err = iInFile.Read(aCodedInBuffer->iData, 1 ); // read one byte
		}
		pos++;
		if ((zeroCounter >= 2) && (aCodedInBuffer->iData[0] == 0x01))
		{
			//byte = ReadByteFromFile();
			err = iInFile.Read(aCodedInBuffer->iData, 1 ); // read one byte
			//PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, iData %d, f2"), aCodedInBuffer->iData[0] ))
			pos++;
			//if (byte == 0xB6 && !firstStartCode)
			if( aCodedInBuffer->iData[0] == 0xB6 && !firstStartCode )
			{
				firstStartCode = ETrue;
			}
			//else if (byte != 0xB1 && firstStartCode)
			else if( aCodedInBuffer->iData[0]!= 0xB1 && firstStartCode )
			{
				PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, end of mpeg-4 picture found [%d]"), pos-4))
				found = ETrue;
			}
		}
	}
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, pos (read length), %d"), pos))
	TInt backward = 0 - pos;
	err = iInFile.Seek(ESeekCurrent, backward );// go back

	if( err != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, Error seeking file %d"), err ))
		return err;
	}

	if (iInputEnd)
	{
		PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, inputEndFound, %d"), pos))
		pos--;
	}
	else
	{
		pos -= 4;
	}


	currentPos = 0;
	err = iInFile.Seek( ESeekCurrent, currentPos);
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, position goes back to the beginning, %d"), currentPos))
	if ( (err = iInFile.Read(aCodedInBuffer->iData, pos )) != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, Error reading file %d"), err ))
		return err;
	}
	if( aCodedInBuffer->iData.Length() < 4 )
	{
		iInputEnd = ETrue;
	}
	iFrameDropCounter++;
	iFrame++;
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, iFrameDropCounter, %d"), iFrameDropCounter))
	PRINT((_L("CVDecTestEngine::ReadMPEG4FrameL, Out")));
	return pos;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetFrameDrop()
* Purpose:
* Note:
* Parameters: TInt aFrameInterval, TInt aFrameNum, TBool aFrameDropMarker
* Return:     None
* --------------------------------------------------------------------------*/

void CVDecTestEngine::SetFrameDrop(TInt aFrameInterval, TInt aFrameNum, TBool aFrameDropMarker )
{
	PRINT(_L("CVDecTestEngine::SetFrameDrop, In"));


	iFrameDropInterval = aFrameInterval;
	iFrameDropNum = aFrameNum;
	iFrameDropMarker = aFrameDropMarker;


	PRINT(_L("CVDecTestEngine::SetFrameDrop, Out"));
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetFLVPassword()
* Purpose:
* Note:
* Parameters: TDesC8& aPassword
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetFLVPassword(TDesC8& aPassword)
{
	PRINT(_L("CVDecTestEngine::SetFLVPassword, In"));

	_LIT8(KNullTerminate, "\0");
	iSetPassword = ETrue;
	iPasswordForFLV.Copy( aPassword);
	iPasswordForFLV.Append( KNullTerminate );
	TBuf16<128> passwordPrint;
	passwordPrint.Copy( iPasswordForFLV );

	PRINT((_L("CVDecTestEngine::SetFLVPassword, iPasswordForFLV [%S]"), &passwordPrint))
	PRINT(_L("CVDecTestEngine::SetFLVPassword, Out"));

}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::EnablingInstantFpsL()
* Purpose:
* Note:
* Parameters: TFileName& aOutFileName
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::EnableInstantFpsL(TFileName& aOutFileName)
{
	TInt err = KErrNone;
	if (!iFrameMeasurement)
	{

		PRINT((_L("CVDecTestEngine::EnableInstantFpsL(), Frame measurement is not enabled")))

		User::Leave(KErrGeneral);
	}

	err = iFpsFile.Replace(iFs, aOutFileName, EFileShareExclusive|EFileWrite);

	if ( err  != KErrNone)
	{
		PRINT((_L("CVDecTestEngine::EnableInstantFpsL, Output File open Failed")));
		User::Leave(err);
	}

	iInstantFpsEnabled = ETrue;
}

/* ----------------------------------------------------------------------------
* Name:       CVDecTestEngine::SetSecondScreenAccess
* Purpose:    Set second screen
* Note:
* Parameters: TBool aStatus
* Return:     None
* --------------------------------------------------------------------------*/
void CVDecTestEngine::SetSecondScreenAccess(TBool aStatus)
{
	iUseSecondScreen = aStatus;
}
//  End	of File
