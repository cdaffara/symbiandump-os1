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
// MMFTestDataSource.h
// 
//

#include "TS_MMFTestDataSource.h"


MMFTestDataSource* MMFTestDataSource::NewL(TUid aType)
	{
	MMFTestDataSource* self = MMFTestDataSource::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MMFTestDataSource* MMFTestDataSource::NewLC(TUid aType)
	{
	MMFTestDataSource* self = new (ELeave) MMFTestDataSource(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


MMFTestDataSource::MMFTestDataSource(TUid aType) : MDataSource(aType)
	{
	}

MMFTestDataSource::~MMFTestDataSource(void)
	{
	}

void MMFTestDataSource::ConstructL (void)
	{
	}

void MMFTestDataSource::ConstructSourceL(  const TDesC8& /*aInitData*/ )
	{
	}

TUid MMFTestDataSource::DataSourceType() const 
	{
	const TUid KMmfTestDSource = {KMmfTestDataSourceUid};
	return KMmfTestDSource; //dummy
	}

TFourCC MMFTestDataSource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodePCM16; // dummy
	}

TInt MMFTestDataSource::SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/) 
	{
	return KErrNotSupported;
	}

void MMFTestDataSource::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/,TMediaId /*aMediaId*/)
	{
	}

void MMFTestDataSource::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
	{
	}//called by MDataSink to pass back emptied buffer to the source

TBool MMFTestDataSource::CanCreateSourceBuffer()
	{
	return ETrue;
	}

CMMFBuffer* MMFTestDataSource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
	{
	CMMFBuffer* newBuffer = 0; //dummy
	return newBuffer;
	}
	
CMMFBuffer* MMFTestDataSource::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
	{
	return CreateSourceBufferL(aMediaId, aReference);
	}

TInt MMFTestDataSource::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MMFTestDataSource::SourceThreadLogoff()
	{
	}

void MMFTestDataSource::NegotiateSourceL(MDataSink& /* aDataSink*/) //called if source setup depends on sink
	{
	}

TBool MMFTestDataSource::SourceSampleConvert() 
	{
	return EFalse;
	}

void MMFTestDataSource::SourcePrimeL()
	{
	}

void MMFTestDataSource::SourcePlayL()
	{
	}

void MMFTestDataSource::SourcePauseL()
	{
	}

void MMFTestDataSource::SourceStopL()
	{
	}

void MMFTestDataSource::SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestDataSourceUid,	MMFTestDataSource::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	


