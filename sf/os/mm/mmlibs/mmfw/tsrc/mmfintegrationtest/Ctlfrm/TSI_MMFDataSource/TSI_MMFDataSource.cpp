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
// TSI_MMFDataSource.h
// 
//

#include "TSI_MMFDataSource.h"


MTSIMmfDataSource* MTSIMmfDataSource::NewL(TUid aType)
	{
	MTSIMmfDataSource* self = MTSIMmfDataSource::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MTSIMmfDataSource* MTSIMmfDataSource::NewLC(TUid aType)
	{
	MTSIMmfDataSource* self = new (ELeave) MTSIMmfDataSource(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


MTSIMmfDataSource::MTSIMmfDataSource(TUid aType) : MDataSource(aType)
	{
	}

MTSIMmfDataSource::~MTSIMmfDataSource()
	{
	}

void MTSIMmfDataSource::ConstructL()
	{
	}

void MTSIMmfDataSource::ConstructSourceL( const TDesC8& /* aInitData */)
	{
	}

TUid MTSIMmfDataSource::DataSourceType() const 
	{
	const TUid KTSIMmfDataSource = {KTSIMmfDataSourceUid};
	return KTSIMmfDataSource; //dummy
	}

TFourCC MTSIMmfDataSource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodePCM16; // dummy
	}

TInt MTSIMmfDataSource::SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/) 
	{
	return KErrNotSupported;
	}

void MTSIMmfDataSource::FillBufferL(CMMFBuffer* /* aBuffer */, MDataSink* /* aConsumer */,TMediaId /*aMediaId*/)
	{
	}

//called by MDataSink to pass back emptied buffer to the source
void MTSIMmfDataSource::BufferEmptiedL(CMMFBuffer* /* aBuffer */)
	{
	}

TBool MTSIMmfDataSource::CanCreateSourceBuffer()
	{
	return ETrue;
	}

CMMFBuffer* MTSIMmfDataSource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /* aReference */)
	{
	CMMFBuffer* newBuffer = NULL; //dummy
	return newBuffer;
	}
	
CMMFBuffer* MTSIMmfDataSource::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
	{
	return CreateSourceBufferL(aMediaId, aReference);
	}

TInt MTSIMmfDataSource::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MTSIMmfDataSource::SourceThreadLogoff()
	{
	}

//called if source setup depends on sink
void MTSIMmfDataSource::NegotiateSourceL(MDataSink& /* aDataSink*/) 
	{
	}

TBool MTSIMmfDataSource::SourceSampleConvert() 
	{
	return EFalse;
	}

void MTSIMmfDataSource::SourcePrimeL()
	{
	}

void MTSIMmfDataSource::SourcePlayL()
	{
	}

void MTSIMmfDataSource::SourcePauseL()
	{
	}

void MTSIMmfDataSource::SourceStopL()
	{
	}

void MTSIMmfDataSource::SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTSIMmfDataSourceUid,	MTSIMmfDataSource::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
