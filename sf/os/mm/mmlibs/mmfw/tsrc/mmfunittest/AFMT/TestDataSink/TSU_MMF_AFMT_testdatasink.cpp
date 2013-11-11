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
// MMFTestDataSink.h
// 
//

#include "TSU_MMF_AFMT_testdatasink.h"
#include "../TSU_MMF_AFMT.h"	// For CTestStep_MMF_AFMT definition


MMFTestDataSink* MMFTestDataSink::NewL(TUid aType)
	{
	MMFTestDataSink* self = MMFTestDataSink::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MMFTestDataSink* MMFTestDataSink::NewLC(TUid aType)
	{
	MMFTestDataSink* self = new (ELeave) MMFTestDataSink(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

MMFTestDataSink::MMFTestDataSink(TUid aType) :MDataSink(aType), iTestStep(NULL)
	{
	}

MMFTestDataSink::~MMFTestDataSink(void)
	{
	}

void MMFTestDataSink::ConstructL (void)
	{
	}

void MMFTestDataSink::ConstructSinkL(const TDesC8& aInitData)
	{
	// Store a pointer of the test step that created us...
	iTestStep = (CTestStep_MMF_AFMT*) aInitData.Ptr();
	}

TUid MMFTestDataSink::DataSinkType() const
	{
	const TUid KMmfTestDSink = {KMmfTestDataSinkUid};
	return KMmfTestDSink; //dummy
	}

TFourCC MMFTestDataSink::SinkDataTypeCode(TMediaId /*aMediaId*/) //used by data path MDataSource/Sink for codec matching
	{
	return KMMFFourCCCodePCM16; // dummy
	}
	
TInt MMFTestDataSink::SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/)
	{
	TInt a = 1;
	return a; // dummy
	}

void MMFTestDataSink::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/) //called by a MDataSource to empty a buffer
	{
	}

void MMFTestDataSink::BufferFilledL(CMMFBuffer* aBuffer) //called by MDataSource to pass back full buffer to the sink
	{
	iTestStep->BufferFilledCallback(aBuffer);
	}

TBool MMFTestDataSink::CanCreateSinkBuffer()
	{
	TBool can = true;
	return can;
	}

CMMFBuffer* MMFTestDataSink::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
	{	
	User::Leave(KErrGeneral);	// This funciton should never get called anyway.
	return NULL;	
	}
	
TInt MMFTestDataSink::SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MMFTestDataSink::SinkThreadLogoff()
	{
	}

void MMFTestDataSink::NegotiateL(MDataSource& /* aDataSource*/) //called if sink setup depends on source
	{
	}

void MMFTestDataSink::SinkPrimeL()
	{
	}

void MMFTestDataSink::SinkPlayL()
	{
	}

void MMFTestDataSink::SinkPauseL()
	{
	}

void MMFTestDataSink::SinkStopL()
	{
	}

void MMFTestDataSink::SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestDataSinkUid,	MMFTestDataSink::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
