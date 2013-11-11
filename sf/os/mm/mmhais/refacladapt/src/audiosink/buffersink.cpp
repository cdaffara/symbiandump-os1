//buffersink.cpp

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



#include "buffersink.h"
#include "moutputport.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CBufferSink::CBufferSink()
	{
	TRACE_CREATE();
	DP_CONTEXT(CBufferSink::CBufferSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
EXPORT_C CBufferSink* CBufferSink::NewL()
	{
	DP_STATIC_CONTEXT(CBufferSink::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CBufferSink* self = new(ELeave)CBufferSink();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CBufferSink::ConstructL()
	{
	DP_CONTEXT(CBufferSink::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CBufferSink::~CBufferSink()
	{
	DP_CONTEXT(CBufferSink::~CBufferSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}
	
// from class MInputPort
// ---------------------------------------------------------------------------
// CBufferSink::GetInputPort
// ---------------------------------------------------------------------------
//
TInt CBufferSink::GetInputPort(MInputPort*& aInput)
	{
	DP_CONTEXT(CBufferSource::GetInputPort *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aInput = this;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSink::EmptyBuffer
// ---------------------------------------------------------------------------
//
TInt CBufferSink::EmptyBuffer(CMMFBuffer* aBuffer, MOutputPort* aSupplier)
	{
	DP_CONTEXT(CBufferSink::EmptyBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if (aSupplier == iOutput)
		{
		iConsumer->BufferToBeEmptied(static_cast<MMMFBufferSink*>(this), aBuffer);
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSink::BufferFilled
// ---------------------------------------------------------------------------
//
TInt CBufferSink::BufferFilled(CMMFBuffer* /*aBuffer*/)
	{
	DP_CONTEXT(CBufferSink::BufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSink::SetOutput
// ---------------------------------------------------------------------------
//
TInt CBufferSink::SetOutput(MOutputPort* aOutput)
	{
	DP_CONTEXT(CBufferSink::SetOutput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iOutput = aOutput;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSink::RemoveOutput
// ---------------------------------------------------------------------------
//
TInt CBufferSink::RemoveOutput(MOutputPort* /*aOutput*/) 
	{
	DP_CONTEXT(CBufferSink::RemoveOutput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iOutput=NULL;
	DP0_RET(KErrNone, "%d");
	}

// from MMMFBufferSink
// ---------------------------------------------------------------------------
// CBufferSink::SetDataConsumer
// ---------------------------------------------------------------------------
//
TInt CBufferSink::SetDataConsumer(MMMFAudioDataConsumer& aConsumer)
	{
	DP_CONTEXT(CBufferSink::SetDataConsumer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iConsumer = &aConsumer;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSink::BufferEmptied
// ---------------------------------------------------------------------------
//
TInt CBufferSink::BufferEmptied(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CBufferSink::BufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// Check if need a switch 
	iBuffer = aBuffer;
	DP_OUT();
	return(iOutput->BufferEmptied(aBuffer));
	}

// ---------------------------------------------------------------------------
// CBufferSink::BuffersDiscarded
// ---------------------------------------------------------------------------
//
TInt CBufferSink::BuffersDiscarded()
	{
	DP_CONTEXT(CBufferSink::BuffersDiscarded *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// Check what we are suppose to do
	if(iBuffer)
		{
		iBuffer = NULL;
		}
	DP0_RET(KErrNone, "%d");
	}

// end of file
