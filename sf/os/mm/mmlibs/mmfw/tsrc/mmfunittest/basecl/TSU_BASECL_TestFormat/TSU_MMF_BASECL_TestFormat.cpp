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

#include <mmf/server/mmfdatabuffer.h>
#include "TSU_MMF_BASECL_TestFormat.h"


CMMFFormatDecode* CMMF_TSU_BASECL_TestFormatDecode::NewL(MDataSource* aSource)
	{
	CMMF_TSU_BASECL_TestFormatDecode* self = new(ELeave) CMMF_TSU_BASECL_TestFormatDecode();
	CleanupStack::PushL(self);
	self->ConstructL(aSource);
	CleanupStack::Pop();
	return STATIC_CAST( CMMFFormatDecode*, self );
	}

CMMF_TSU_BASECL_TestFormatDecode::CMMF_TSU_BASECL_TestFormatDecode() :
	iState(ELoggedOff), iBufferCreated(EFalse),
	iOwnsBuffer(EFalse), iFourCC('A','B','C','D')
	{
	}
	
void CMMF_TSU_BASECL_TestFormatDecode::ConstructL(MDataSource* aSource) 
	{
	iSource = aSource;
	}


CMMF_TSU_BASECL_TestFormatDecode::~CMMF_TSU_BASECL_TestFormatDecode()
	{
	}

TUint CMMF_TSU_BASECL_TestFormatDecode::Streams(TUid /*aMediaType*/) const
	{
	return 1; 
	}

TTimeIntervalMicroSeconds CMMF_TSU_BASECL_TestFormatDecode::FrameTimeInterval(TMediaId /*aMediaId*/) const
	{
	return TTimeIntervalMicroSeconds(KMmfFormatFrameTimeInterval);
	}

TTimeIntervalMicroSeconds CMMF_TSU_BASECL_TestFormatDecode::Duration(TMediaId /*aMediaId*/) const
	{
	TTimeIntervalMicroSeconds frameTimeInterval = FrameTimeInterval(TMediaId());
	CMMF_TSU_BASECL_TestSource* castedTestDataSource = STATIC_CAST(CMMF_TSU_BASECL_TestSource*, iSource);
	TTimeIntervalMicroSeconds duration = 
		TTimeIntervalMicroSeconds((castedTestDataSource->Frames() * I64LOW(frameTimeInterval.Int64())));
	return duration;
	}

CMMFBuffer* CMMF_TSU_BASECL_TestFormatDecode::CreateSourceBufferL(TMediaId aMediaId, TBool &aReference)
	{
	return (iSource->CreateSourceBufferL(aMediaId, aReference));
	}

TFourCC CMMF_TSU_BASECL_TestFormatDecode::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return iFourCC;
	}

TInt CMMF_TSU_BASECL_TestFormatDecode::SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/)
	{
	iFourCC = aSinkFourCC;
	return KErrNone;
	}


void CMMF_TSU_BASECL_TestFormatDecode::FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId)
	{
	iDataPath = aConsumer;
	iSource->FillBufferL(aBuffer, this, aMediaId);
	}


TBool CMMF_TSU_BASECL_TestFormatDecode::CanCreateSourceBuffer()
	{
	return ETrue;
	}


TInt CMMF_TSU_BASECL_TestFormatDecode::SourceThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iSource->SourceThreadLogon(aEventHandler);
	if (iState == ELoggedOff) iState = ELoggedOn;
	return KErrNone;
	}

void CMMF_TSU_BASECL_TestFormatDecode::SourceThreadLogoff()
	{
	iSource->SourceThreadLogoff();
	}

void CMMF_TSU_BASECL_TestFormatDecode::SourcePrimeL()
	{
	iSource->SourcePrimeL();
	iState = EPrimed;
	}

void CMMF_TSU_BASECL_TestFormatDecode::SourcePauseL()
	{
	iSource->SourcePauseL();
	}

void CMMF_TSU_BASECL_TestFormatDecode::SourcePlayL()
	{
	iSource->SourcePlayL();
	}

void CMMF_TSU_BASECL_TestFormatDecode::SourceStopL()
	{
	iSource->SourceStopL();
	iState = EStopped;
	}




CMMFFormatEncode* CMMF_TSU_BASECL_TestFormatEncode::NewL(MDataSink* aSink)
	{
	CMMF_TSU_BASECL_TestFormatEncode* self = new(ELeave) CMMF_TSU_BASECL_TestFormatEncode();
	CleanupStack::PushL(self);
	self->ConstructL(aSink);
	CleanupStack::Pop();
	return STATIC_CAST( CMMFFormatEncode*, self ) ;
	}

CMMF_TSU_BASECL_TestFormatEncode::CMMF_TSU_BASECL_TestFormatEncode() :
	iState(ELoggedOff), iBufferCreated(EFalse),
	iOwnsBuffer(EFalse), iFourCC('a','b','c','d')
	{
	}

void CMMF_TSU_BASECL_TestFormatEncode::ConstructL(MDataSink* aSink) 
	{
	iSink = aSink;
	}


CMMF_TSU_BASECL_TestFormatEncode::~CMMF_TSU_BASECL_TestFormatEncode()
	{

	}

TTimeIntervalMicroSeconds CMMF_TSU_BASECL_TestFormatEncode::FrameTimeInterval(TMediaId /*aMediaId*/) const
	{
	return TTimeIntervalMicroSeconds(KMmfFormatFrameTimeInterval);
	}

TTimeIntervalMicroSeconds CMMF_TSU_BASECL_TestFormatEncode::Duration(TMediaId /*aMediaId*/) const
	{
	TTimeIntervalMicroSeconds frameTimeInterval = FrameTimeInterval(TMediaId());
	CMMF_TSU_BASECL_TestSink* castedTestDataSink = STATIC_CAST(CMMF_TSU_BASECL_TestSink*, iSink);
	TTimeIntervalMicroSeconds duration = 
		TTimeIntervalMicroSeconds((castedTestDataSink->Frames() * I64LOW(frameTimeInterval.Int64())));
	return duration;
	}

TFourCC CMMF_TSU_BASECL_TestFormatEncode::SinkDataTypeCode(TMediaId /*aMediaId*/) //used by data path MDataSource/Sink for codec matching
	{
	return iFourCC;
	}

TInt CMMF_TSU_BASECL_TestFormatEncode::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/)
	{
	iFourCC = aSinkFourCC;
	return KErrNone;
	}

CMMFBuffer* CMMF_TSU_BASECL_TestFormatEncode::CreateSinkBufferL(TMediaId aMediaId, TBool &aReference)
	{
	return iSink->CreateSinkBufferL(aMediaId, aReference);
	}


void CMMF_TSU_BASECL_TestFormatEncode::EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId)
	{
	iDataPath = aSupplier;
	iSink->EmptyBufferL(aBuffer, this, aMediaId);
	//BufferEmptiedL use default CMMFFormatEncode implementation
	}


TBool CMMF_TSU_BASECL_TestFormatEncode::CanCreateSinkBuffer()
	{
	return ETrue;
	}


TInt CMMF_TSU_BASECL_TestFormatEncode::SinkThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iSink->SinkThreadLogon(aEventHandler);
	if (iState == ELoggedOff) iState = ELoggedOn;
	return KErrNone;
	}

void CMMF_TSU_BASECL_TestFormatEncode::SinkThreadLogoff()
	{
	iSink->SinkThreadLogoff();
	}

void CMMF_TSU_BASECL_TestFormatEncode::SinkPrimeL()
	{
	iSink->SinkPrimeL();
	iState = EPrimed;
	}

void CMMF_TSU_BASECL_TestFormatEncode::SinkPauseL()
	{
	iSink->SinkPauseL();
	}

void CMMF_TSU_BASECL_TestFormatEncode::SinkPlayL()
	{
	iSink->SinkPlayL();
	}

void CMMF_TSU_BASECL_TestFormatEncode::SinkStopL()
	{
	iSink->SinkStopL();
	iState = EStopped;
	}
