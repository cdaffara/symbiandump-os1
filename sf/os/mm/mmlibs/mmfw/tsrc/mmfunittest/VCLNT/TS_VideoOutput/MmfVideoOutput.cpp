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

#include "MmfVideoOutput.h"
#include <mmf/server/mmfdatabuffer.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmffourcc.h>


void Panic(TInt aPanicCode)
	{
	_LIT(KMMFVideoOutputPanicCategory, "MMFVideoOutput");
	User::Panic(KMMFVideoOutputPanicCategory, aPanicCode);
	}

MDataSink* CMMFVideoOutput::NewSinkL()
	{
	CMMFVideoOutput* self = new (ELeave) CMMFVideoOutput ;
	return STATIC_CAST( MDataSink*, self ) ;
	}


void CMMFVideoOutput::ConstructSinkL( const TDesC8& /*aInitData*/ )
	{

	}


/**
 *
 * Send Video to MMFDevsound
 *
 * @param		"CMMFBuffer* aBuffer"
 *				The data to write out
 * @param		"MDataSource* aSupplier"
 * @param		"TMediaId aMediaId"
 *				Type of data supplied - currently ignored.
 */

void CMMFVideoOutput::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{

	}

void CMMFVideoOutput::NegotiateL(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfFormatDecode)
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = ((CMMFFormatDecode&)aSource).SampleRate();
		iSourceChannels = ((CMMFFormatDecode&)aSource).NumChannels();
		iSourceFourCC.Set(aSource.SourceDataTypeCode(TMediaId(KUidMediaTypeVideo)));
		((CMMFFormatDecode&)aSource).SuggestSourceBufferSize(KVideoOutputDefaultFrameSize);
		}
/*	[WINS DevSound supports different sample rates, stereo and different encoding]
	//Use SW conversion but  should configure Dev sound where possible
	// WINS DevSound is 8000 sample rate, mono & pcm16
	if ((iSourceChannels != 1)||(iSourceSampleRate != 8000)) 
		{
		iNeedsSWConversion = ETrue;
		TUint convertedBufferFrameSize = iMMFDevSound->BufferSize( ) / iSourceChannels;
		if (iConvertBuffer) delete iConvertBuffer; 
		iConvertBuffer = NULL;
		iConvertBuffer = CMMFDataBuffer::NewL(convertedBufferFrameSize);
		}
*/
	}

void CMMFVideoOutput::SetSinkPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iPrioritySettings = aPrioritySettings;
	}


void CMMFVideoOutput::SinkPrimeL()
	{
	iFirstBufferSent = EFalse;
	}


void CMMFVideoOutput::SinkPauseL()
	{
	iFirstBufferSent = EFalse;
	iState = EPaused;
	}


void CMMFVideoOutput::SinkPlayL()
	{
	}

void CMMFVideoOutput::SinkStopL()
	{
	}

TInt CMMFVideoOutput::State()
	{
	return iState;
	}


TInt CMMFVideoOutput::SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{
	return KErrNone;
	}


void CMMFVideoOutput::SinkThreadLogoff()
	{
	}


void CMMFVideoOutput::BufferFilledL(CMMFBuffer* /*aBuffer*/)
	{
//	__ASSERT_DEBUG(EFalse, Panic(EMMFVideoOutputPanicBufferFilledLNotSupported));
	}


TFourCC CMMFVideoOutput::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	//hardwire 4CC - DevSound should have an interface to get this value
	return KMMFFourCCCodePCM16;
	}

TBool CMMFVideoOutput::CanCreateSinkBuffer()
	{
	return ETrue;
	}

CMMFBuffer* CMMFVideoOutput::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	iDevSoundBuffer = CMMFDataBuffer::NewL(KVideoOutputDefaultFrameSize);
	aReference = ETrue;
	if ( iNeedsSWConversion )
		return iConvertBuffer;
	else
		return iDevSoundBuffer;
	}


CMMFVideoOutput::~CMMFVideoOutput()
	{
	delete iChannelAndSampleRateConverterFactory;
	delete iConvertBuffer;
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs



const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidVideoOutputInterface,	CMMFVideoOutput::NewSinkL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

