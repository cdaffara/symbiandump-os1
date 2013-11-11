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
// MMFTestDataSinkB.h
// 
//

#include "TS_MMFTestDataSinkB.h"


MMFTestDataSinkB* MMFTestDataSinkB::NewL(TUid aType)
	{
	MMFTestDataSinkB* self = MMFTestDataSinkB::NewLC(aType);
	CleanupStack::Pop();
	return self;
	}

MMFTestDataSinkB* MMFTestDataSinkB::NewLC(TUid aType)
	{
	MMFTestDataSinkB* self = new (ELeave) MMFTestDataSinkB(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

MMFTestDataSinkB::MMFTestDataSinkB(TUid aType) :MDataSink(aType)
	{
	}

MMFTestDataSinkB::~MMFTestDataSinkB(void)
	{
	
	}

void MMFTestDataSinkB::ConstructL (void)
	{
	}

void MMFTestDataSinkB::ConstructSinkL(const TDesC8& /*aInitData*/)
	{
	}

TUid MMFTestDataSinkB::DataSinkType() const
	{
	const TUid KMmfTestDSink = {KMmfTestDataSinkBUid};
	return KMmfTestDSink; //dummy
	}

TFourCC MMFTestDataSinkB::SinkDataTypeCode(TMediaId /*aMediaId*/) //used by data path MDataSource/Sink for codec matching
	{
	return KMMFFourCCCodePCM16; // dummy
	}
	
TInt MMFTestDataSinkB::SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/)
	{
	return 1; // dummy
	}

void MMFTestDataSinkB::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/) //called by a MDataSource to empty a buffer
	{
	}

void MMFTestDataSinkB::BufferFilledL(CMMFBuffer* /*aBuffer*/) //called by MDataSource to pass back full buffer to the sink
	{
	}

TBool MMFTestDataSinkB::CanCreateSinkBuffer()
	{
	return ETrue;
	}

CMMFBuffer* MMFTestDataSinkB::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	CMMFBuffer* newBuffer = 0;
	return newBuffer;
	}
	
TInt MMFTestDataSinkB::SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/) 
	{
	return KErrNone;
	}

void MMFTestDataSinkB::SinkThreadLogoff()
	{
	}

void MMFTestDataSinkB::NegotiateL(MDataSource& /* aDataSource*/) //called if sink setup depends on source
	{
	}

void MMFTestDataSinkB::SinkPrimeL()
	{
	}

void MMFTestDataSinkB::SinkPlayL()
	{
	}

void MMFTestDataSinkB::SinkPauseL()
	{
	}

void MMFTestDataSinkB::SinkStopL()
	{
	}

void MMFTestDataSinkB::SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestDataSinkBUid,	MMFTestDataSinkB::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
