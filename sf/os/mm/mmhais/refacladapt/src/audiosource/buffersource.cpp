//buffersource.cpp

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



#include "buffersource.h"
#include "minputport.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CBufferSource::CBufferSource()
	: CActive(EPriorityStandard)
	{
	TRACE_CREATE();
	DP_CONTEXT(CBufferSource::CBufferSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
EXPORT_C CBufferSource* CBufferSource::NewL()
	{
	DP_STATIC_CONTEXT(CBufferSource::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CBufferSource* self = new(ELeave)CBufferSource();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CBufferSource::ConstructL()
	{
	DP_CONTEXT(CBufferSource::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CActiveScheduler::Add(this);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CBufferSource::~CBufferSource()
	{
	DP_CONTEXT(CBufferSource::~CBufferSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}


// from MMMFBufferSource
// ---------------------------------------------------------------------------
// CBufferSource::SetDataSupplier
// ---------------------------------------------------------------------------
TInt CBufferSource::SetDataSupplier(MMMFAudioDataSupplier& aSupplier)
	{
	DP_CONTEXT(CBufferSource::SetDataSupplier *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iSupplier = &aSupplier;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::BufferFilled
// ---------------------------------------------------------------------------
TInt CBufferSource::BufferFilled(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CBufferSource::BufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBuffer = aBuffer;
	DP_OUT();
	return iInput->BufferFilled(aBuffer);
	}

// ---------------------------------------------------------------------------
// CBufferSource::BuffersDiscarded
// ---------------------------------------------------------------------------
TInt CBufferSource::BuffersDiscarded()
	{
	DP_CONTEXT(CBufferSource::BuffersDiscarded *CD1*, CtxDevSound, DPLOCAL);
	// Check what we are suppose to do
	DP_IN();
	if(iBuffer)
		{
		iBuffer = NULL;
		}
	DP0_RET(KErrNone, "%d");
	}


//From MOutputPort	
// ---------------------------------------------------------------------------
// CBufferSource::FillBuffer
// ---------------------------------------------------------------------------
//
TInt CBufferSource::FillBuffer(CMMFBuffer* aBuffer, MInputPort* aConsumer)
	{
	DP_CONTEXT(CBufferSource::FillBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if (aConsumer == iInput)
		{
		iSupplier->BufferToBeFilled(static_cast<MMMFBufferSource*>(this), aBuffer);
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::BufferEmptied
// ---------------------------------------------------------------------------
//
TInt CBufferSource::BufferEmptied(CMMFBuffer* /*aBuffer*/)
	{
	DP_CONTEXT(CBufferSource::BufferEmptied *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::SetInput
// ---------------------------------------------------------------------------
//
TInt CBufferSource::SetInput(MInputPort* aInput)
	{
	DP_CONTEXT(CBufferSource::SetInput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iInput = aInput;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::RemoveInput
// ---------------------------------------------------------------------------
//
TInt CBufferSource::RemoveInput(MInputPort* /*aInput*/)
	{
	DP_CONTEXT(CBufferSource::RemoveInput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iInput = NULL;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::GetOutputPort
// ---------------------------------------------------------------------------
//
TInt CBufferSource::GetOutputPort(MOutputPort*& aOutputPort)
	{
	DP_CONTEXT(CBufferSource::GetOutputPort *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aOutputPort = this;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CBufferSource::FlushBuffer
// ---------------------------------------------------------------------------
//
TInt CBufferSource::FlushBuffer(MFlushHandlerObserver* aFlushObserver)
	{
	DP_CONTEXT(CBufferSource::FlushBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iObserver = aFlushObserver;
	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	DP0_RET(KErrNone, "%d");
	}

// From CActive
// ---------------------------------------------------------------------------
// CBufferSource::DoCancel
// ---------------------------------------------------------------------------
//
void CBufferSource::DoCancel()
	{
	DP_CONTEXT(CBufferSource::FlushBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Nothing to do

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CBufferSource::RunL
// ---------------------------------------------------------------------------
//
void CBufferSource::RunL()
	{
	DP_CONTEXT(CBufferSource::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iSupplier->DiscardBuffers(this);
	iObserver->FlushComplete(KErrNone);

	DP_OUT();
	}


// end of file
