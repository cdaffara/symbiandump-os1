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
// TSU_MMF_AFMT_TestDataSource.h
// 
//

#include "TSU_MMF_AFMT_TestDataSource.h"
#include "../TSU_MMF_AFMT.h"	// For CTestStep_MMF_AFMT definition



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

MMFTestDataSource::MMFTestDataSource(TUid aType) : MDataSource(aType), iTestStep(NULL)
	{
	}

MMFTestDataSource::~MMFTestDataSource(void)
	{
	}

void MMFTestDataSource::ConstructL (void)
	{
	}

void MMFTestDataSource::ConstructSourceL(  const TDesC8& aInitData )
	{
	// Store a pointer of the test step that created us...
	iTestStep = (CTestStep_MMF_AFMT*) aInitData.Ptr();
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

void MMFTestDataSource::FillBufferL(CMMFBuffer* /*aBuffer8*/, MDataSink* /*aConsumer*/,TMediaId /*aMediaId*/)
	{
	}

void MMFTestDataSource::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
	iTestStep->BufferEmptiedCallback(aBuffer);
	}//called by MDataSink to pass back emptied buffer to the source

TBool MMFTestDataSource::CanCreateSourceBuffer()
	{
	TBool can = true; // dummy
	return can;
	}

CMMFBuffer* MMFTestDataSource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
	{	
	User::Leave(KErrGeneral);	// This funciton should never get called anyway.
	return NULL;	
	}
	
	//XXX optimal source buffer size creation may depend on the sink buffer & vice versa
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
	

