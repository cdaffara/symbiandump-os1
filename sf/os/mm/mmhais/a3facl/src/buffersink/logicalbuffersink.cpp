// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "logicalbuffersink.h"
#include <a3f/audioprocessingunittypeuids.h>
#include <ecom/implementationproxy.h> // For making it ECom plugin

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfBufferSinkUid, CLogicalBufferSink::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogicalBufferSink::CLogicalBufferSink(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalBufferSink::CLogicalBufferSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CLogicalBufferSink* CLogicalBufferSink::NewL(TAny* aParameters)
	{
	DP_STATIC_CONTEXT(CLogicalBufferSink::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalBufferSink* self = new(ELeave)CLogicalBufferSink(aParameters);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CLogicalBufferSink::ConstructL()
	{
	DP_CONTEXT(CLogicalBufferSink::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogicalBufferSink::~CLogicalBufferSink()
	{
	DP_CONTEXT(CLogicalBufferSink::~CLogicalBufferSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// from class MMMFBufferSink
// ---------------------------------------------------------------------------
// CLogicalBufferSink::SetDataConsumer
// ---------------------------------------------------------------------------
//
TInt CLogicalBufferSink::SetDataConsumer(MMMFAudioDataConsumer& aConsumer)
	{
	DP_CONTEXT(CLogicalBufferSink::SetDataConsumer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iConsumer = &aConsumer;
	DP0_RET(KErrNone,"%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSink::BufferEmptied
// ---------------------------------------------------------------------------
//
TInt CLogicalBufferSink::BufferEmptied(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CLogicalBufferSink::BufferEmptied *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = iAdaptationBufferSink->BufferEmptied(aBuffer);
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSink::BuffersDiscarded
// ---------------------------------------------------------------------------
//
TInt CLogicalBufferSink::BuffersDiscarded()
	{
	DP_CONTEXT(CLogicalBufferSink::BuffersDiscarded *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = iAdaptationBufferSink->BuffersDiscarded();
	DP0_RET(err,"%d");
	}

// From CAudioProcessingUnit
// ---------------------------------------------------------------------------
// CLogicalBufferSink::IsTypeOf
// ---------------------------------------------------------------------------
//  
TBool CLogicalBufferSink::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalBufferSink::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if (iType == aTypeId)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}
    
// ---------------------------------------------------------------------------
// CLogicalBufferSink::Interface
// ---------------------------------------------------------------------------
//
TAny* CLogicalBufferSink::Interface(TUid aType)
	{
	DP_CONTEXT(CLogicalBufferSink::Interface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MMMFBufferSink* ptr = NULL;
	if(aType==KUidMmfBufferSink)
		{
		ptr = static_cast<MMMFBufferSink*>(this);
		}
	DP_OUT();
	return ptr;
	}

// From MMMFAudioDataConsumer
// ---------------------------------------------------------------------------
// CLogicalBufferSink::BufferToBeFilled
// ---------------------------------------------------------------------------
// 
void CLogicalBufferSink::BufferToBeEmptied(MMMFBufferSink* /*aSink*/, CMMFBuffer* aBuffer)
	{
	iConsumer->BufferToBeEmptied(this, aBuffer);
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::DiscardBuffers
// ---------------------------------------------------------------------------
// 
void CLogicalBufferSink::DiscardBuffers(MMMFBufferSink* /*aSink*/)
	{
	iConsumer->DiscardBuffers(this);
	}

// end of file
