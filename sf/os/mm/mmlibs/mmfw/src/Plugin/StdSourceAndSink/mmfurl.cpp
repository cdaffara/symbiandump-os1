// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include "mmfurl.h"


MDataSource* CMMFUrlSource::NewSourceL()
	{
	CMMFUrlSource* self = new(ELeave) CMMFUrlSource;
	return STATIC_CAST(MDataSource*, self);
	}

CMMFUrlSource::CMMFUrlSource() :
	MDataSource(KUidMmfUrlSource)
	{
	}

void CMMFUrlSource::ConstructSourceL(const TDesC8& aInitData)
	{
	RDesReadStream stream(aInitData);
	CleanupClosePushL(stream);

	CMMFUrlParams* params = CMMFUrlParams::NewLC(stream);

	iUrl = params->Url().AllocL();
	iIAPId = params->IAPId();
	iUseIAPId = params->UseIAPId();

	CleanupStack::PopAndDestroy(2);//params, stream
	}

CMMFUrlSource::~CMMFUrlSource()
	{
	delete iUrl;
	}

TFourCC CMMFUrlSource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodeNULL;
	}

TInt CMMFUrlSource::SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/)
	{
	return KErrNotSupported;
	}

void CMMFUrlSource::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/,TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSource::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
	{
	User::Leave(KErrNotSupported);
	}

TBool CMMFUrlSource::CanCreateSourceBuffer()
	{
	return EFalse;
	}

CMMFBuffer* CMMFUrlSource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CMMFBuffer* CMMFUrlSource::CreateSourceBufferL(TMediaId /*aMediaId*/, CMMFBuffer& /*aSinkBuffer*/, TBool& /*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CMMFUrlSource::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{
	return KErrNotSupported;
	}

void CMMFUrlSource::SourceThreadLogoff()
	{
	}

void CMMFUrlSource::NegotiateSourceL(MDataSink& /* aDataSink*/)
	{
	User::Leave(KErrNotSupported);
	}

TBool CMMFUrlSource::SourceSampleConvert()
	{
	return EFalse;
	}

void CMMFUrlSource::SourcePrimeL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSource::SourcePlayL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSource::SourcePauseL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSource::SourceStopL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSource::SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFUrlSource::SourceCustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNotSupported);
	}




MDataSink* CMMFUrlSink::NewSinkL()
	{
	CMMFUrlSink* self = new(ELeave) CMMFUrlSink;
	return STATIC_CAST(MDataSink*, self);
	}

CMMFUrlSink::CMMFUrlSink() :
	MDataSink(KUidMmfUrlSink)
	{
	}

void CMMFUrlSink::ConstructSinkL(const TDesC8& aInitData)
	{
	RDesReadStream stream(aInitData);
	CleanupClosePushL(stream);

	CMMFUrlParams* params = CMMFUrlParams::NewLC(stream);

	iUrl = params->Url().AllocL();
	iIAPId = params->IAPId();
	iUseIAPId = params->UseIAPId();

	CleanupStack::PopAndDestroy(2);//params, stream
	}

CMMFUrlSink::~CMMFUrlSink()
	{
	delete iUrl;
	}

TFourCC CMMFUrlSink::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodeNULL;
	}

TInt CMMFUrlSink::SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/)
	{
	return KErrNotSupported;
	}

void CMMFUrlSink::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::BufferFilledL(CMMFBuffer* /*aBuffer*/)
	{
	User::Leave(KErrNotSupported);
	}

TBool CMMFUrlSink::CanCreateSinkBuffer()
	{
	return EFalse;
	}

CMMFBuffer* CMMFUrlSink::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CMMFUrlSink::SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{
	return KErrNotSupported;
	}

void CMMFUrlSink::SinkThreadLogoff()
	{
	}

void CMMFUrlSink::NegotiateL(MDataSource& /* aDataSource*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::SinkPrimeL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::SinkPlayL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::SinkPauseL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::SinkStopL()
	{
	User::Leave(KErrNotSupported);
	}

void CMMFUrlSink::SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFUrlSink::SinkCustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNotSupported);
	}

