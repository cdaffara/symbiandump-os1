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
// TSI_MMFDataSink.h
// 
//

#include "TSI_MMFDataSink.h"



MTSIMmfDataSink* MTSIMmfDataSink::NewL(TUid aType)
	{
	MTSIMmfDataSink* self = MTSIMmfDataSink::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MTSIMmfDataSink* MTSIMmfDataSink::NewLC(TUid aType)
	{
	MTSIMmfDataSink* self = new (ELeave) MTSIMmfDataSink(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

MTSIMmfDataSink::MTSIMmfDataSink(TUid aType) :MDataSink(aType)
	{
	}

MTSIMmfDataSink::~MTSIMmfDataSink(void)
	{
	
	}

void MTSIMmfDataSink::ConstructL (void)
	{
	}

void MTSIMmfDataSink::ConstructSinkL(const TDesC8& /* aInitData */)
	{
	}

TUid MTSIMmfDataSink::DataSinkType() const
	{
	const TUid KTSIMmfDataSink = {KTSIMmfDataSinkUid};
	return KTSIMmfDataSink; //dummy
	}

//used by data path MDataSource/Sink for codec matching
TFourCC MTSIMmfDataSink::SinkDataTypeCode(TMediaId /*aMediaId*/) 
	{
	return KMMFFourCCCodePCM16; // dummy
	}
	
TInt MTSIMmfDataSink::SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/)
	{
	return 1; // dummy
	}

void MTSIMmfDataSink::EmptyBufferL(CMMFBuffer* /* aBuffer */, MDataSource* /* aSupplier */, TMediaId /*aMediaId*/) //called by a MDataSource to empty a buffer
	{
	}

//called by MDataSource to pass back full buffer to the sink
void MTSIMmfDataSink::BufferFilledL(CMMFBuffer* /* aBuffer */) 
	{
	}

TBool MTSIMmfDataSink::CanCreateSinkBuffer()
	{
	return ETrue;
	}

CMMFBuffer* MTSIMmfDataSink::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /* aReference */)
	{
	CMMFBuffer* newBuffer = NULL;
	return newBuffer;
	}
	
TInt MTSIMmfDataSink::SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MTSIMmfDataSink::SinkThreadLogoff()
	{
	}

//called if sink setup depends on source
void MTSIMmfDataSink::NegotiateL(MDataSource& /* aDataSource*/) 
	{
	}

void MTSIMmfDataSink::SinkPrimeL()
	{
	}

void MTSIMmfDataSink::SinkPlayL()
	{
	}

void MTSIMmfDataSink::SinkPauseL()
	{
	}

void MTSIMmfDataSink::SinkStopL()
	{
	}

void MTSIMmfDataSink::SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTSIMmfDataSinkUid,	MTSIMmfDataSink::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
