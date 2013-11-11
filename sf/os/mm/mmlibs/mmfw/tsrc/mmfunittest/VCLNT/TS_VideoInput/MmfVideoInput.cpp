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

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatabuffer.h>
#include "MmfVideoInput.h"



void Panic(TInt aPanicCode)
	{
	_LIT(KMMFVideoInputPanicCategory, "MMFVideoInput");
	User::Panic(KMMFVideoInputPanicCategory, aPanicCode);
	}

MDataSource* CMMFVideoInput::NewSourceL() 
	{
	CMMFVideoInput* self = new (ELeave) CMMFVideoInput ;
	return  STATIC_CAST( MDataSource*, self );
	}

CMMFVideoInput::~CMMFVideoInput()
	{
#ifdef KVideoInputCanResample
	delete iChannelAndSampleRateConverterFactory; 
	delete iConvertBuffer; 
#endif
	}


void CMMFVideoInput::ConstructSourceL( const TDesC8& /*aInitData*/ ) 
	{

	}


void CMMFVideoInput::HWFillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/)
	{

	}


/**
 * 
 * Get audio from MMFDevsound
 *
 * @param		"CMMFBuffer* aBuffer"
 *				The data to write out. Assumed to really be CMMFBitmapFrameBuffer*.
 * @param		"MDataSink* aConsumer"
 * @param		"TMediaId aMediaId"
 *				Type of data supplied - currently ignored.
 */

void CMMFVideoInput::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/,TMediaId)
	{
	}

void CMMFVideoInput::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
	{

	}


TBool CMMFVideoInput::CanCreateSourceBuffer()
	{
	return ETrue;
	}

CMMFBuffer* CMMFVideoInput::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	CMMFDataBuffer* buf = CMMFDataBuffer::NewL(KVideoInputDefaultFrameSize);

	aReference = ETrue; // This is a reference from DevSound
	return buf;
	}

CMMFBuffer* CMMFVideoInput::CreateSourceBufferL(TMediaId /*aMediaId*/)
	{
	//0x1000 is what WINS DevSound expects
	CMMFDataBuffer* buf = CMMFDataBuffer::NewL(KVideoInputDefaultFrameSize);
	return buf;
	}



void CMMFVideoInput::SourcePrimeL()
	{

	}

void CMMFVideoInput::SourceStopL()
	{
	iStopped = ETrue;
	}


void CMMFVideoInput::SourcePlayL()
	{
#ifdef KVideoInputCanResample
	if ((iNeedsSWConversion)&&(iSinkChannels>0))
		{//can only do SW convert  - therefore need to do a conversion 
		//currently only pcm16 is supported so return with an error if format not pcm16
		iChannelAndSampleRateConverterFactory 
			= new(ELeave)CMMFChannelAndSampleRateConverterFactory;
		iChannelAndSampleRateConverter = 
			iChannelAndSampleRateConverterFactory->CreateConverterL( iSinkSampleRate, iSinkChannels, 
																	8000, 1);
		//need to create an intermediate buffer in which to place the converted data

		TUint convertedBufferFrameSize = KVideoInputDefaultFrameSize/iSinkChannels;
		if (iConvertBuffer) delete iConvertBuffer; 
		iConvertBuffer = NULL;
		iConvertBuffer = CMMFDataBuffer::NewL(convertedBufferFrameSize);
		}
#endif
	}

TInt CMMFVideoInput::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{
	return KErrNone;
	}

void CMMFVideoInput::SourceThreadLogoff()
	{
	}

void CMMFVideoInput::NegotiateL(MDataSink& aSink)
	{
	aSink = aSink; //to keep compiler happy
#ifdef KVideoInputCanResample
	if (aSink.DataSinkType() == KUidMmfFormatDecode) 
		{//source is a clip so for now set sink settings to match source
		iSinkSampleRate = ((CMMFFormatDecode&)aSink).SampleRate(); 
		iSinkChannels = ((CMMFFormatDecode&)aSink).NumChannels();
		iSinkFourCC.Set(aSink.SinkDataTypeCode(TMediaId(KUidMediaTypeVideo)));
		((CMMFFormatDecode&)aSink).SuggestSourceBufferSize(KVideoInputDefaultFrameSize); 
		}

	//Use SW conversion but  should configure Dev sound where possible
	//WINS DevSound is always 8000 sample rate, mono & pcm16
	if ((iSinkChannels != 1)||(iSinkSampleRate != 8000)) iNeedsSWConversion = ETrue;
	else iNeedsSWConversion = EFalse; //incase it was set to true
#endif
	}

void CMMFVideoInput::SetSourcePrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iPrioritySettings = aPrioritySettings;
	}


TFourCC CMMFVideoInput::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	//indicate we can only handle PCM16, although low-level WINS LDD can take Alaw directly
	return KMMFFourCCCodePCM16;
	}


TInt CMMFVideoInput::BytesPlayed()
	{
	return 0;
	}

// _________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidVideoInputInterface,	CMMFVideoInput::NewSourceL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

