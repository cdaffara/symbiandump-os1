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
// MMFTestDataSourceB.h
// 
//

#include "TS_MMFTestDataSourceB.h"


MMFTestDataSourceB* MMFTestDataSourceB::NewL(TUid aType)
	{
	MMFTestDataSourceB* self = MMFTestDataSourceB::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MMFTestDataSourceB* MMFTestDataSourceB::NewLC(TUid aType)
	{
	MMFTestDataSourceB* self = new (ELeave) MMFTestDataSourceB(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


MMFTestDataSourceB::MMFTestDataSourceB(TUid aType) : MDataSource(aType)
	{
	}

MMFTestDataSourceB::~MMFTestDataSourceB(void)
	{
	}

void MMFTestDataSourceB::ConstructL (void)
	{
	}

void MMFTestDataSourceB::ConstructSourceL(  const TDesC8& /*aInitData */)
	{
	}

TUid MMFTestDataSourceB::DataSourceType() const 
	{
	const TUid KMmfTestDSource = {KMmfTestDataSourceBUid};
	return KMmfTestDSource; //dummy
	}

TFourCC MMFTestDataSourceB::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodePCM16; // dummy
	}

TInt MMFTestDataSourceB::SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/) 
	{
	return KErrNotSupported;
	}

void MMFTestDataSourceB::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/,TMediaId /*aMediaId*/)
	{
	}

void MMFTestDataSourceB::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
	{
	}//called by MDataSink to pass back emptied buffer to the source

TBool MMFTestDataSourceB::CanCreateSourceBuffer()
	{
	return ETrue;
	}

CMMFBuffer* MMFTestDataSourceB::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
	{
	CMMFBuffer* newBuffer = 0; //dummy
	return newBuffer;
	}
	
CMMFBuffer* MMFTestDataSourceB::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
	{
	return CreateSourceBufferL(aMediaId, aReference);
	}

TInt MMFTestDataSourceB::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MMFTestDataSourceB::SourceThreadLogoff()
	{
	}

void MMFTestDataSourceB::NegotiateSourceL(MDataSink& /* aDataSink*/) //called if source setup depends on sink
	{
	}

TBool MMFTestDataSourceB::SourceSampleConvert() 
	{
	return EFalse;
	}

void MMFTestDataSourceB::SourcePrimeL()
	{
	}

void MMFTestDataSourceB::SourcePlayL()
	{
	}

void MMFTestDataSourceB::SourcePauseL()
	{
	}

void MMFTestDataSourceB::SourceStopL()
	{
	}

void MMFTestDataSourceB::SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestDataSourceBUid,	MMFTestDataSourceB::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	


