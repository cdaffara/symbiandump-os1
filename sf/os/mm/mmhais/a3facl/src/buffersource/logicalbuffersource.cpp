// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : logicalbuffersource.cpp
// Part of     : ACL Logical Layer
//



#include "logicalbuffersource.h"
#include <a3f/audioprocessingunittypeuids.h>
#include <ecom/implementationproxy.h> // For making it ECom plugin

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfBufferSourceUid,  CLogicalBufferSource::NewL),
	};

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogicalBufferSource::CLogicalBufferSource(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalBufferSource::CLogicalBufferSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CLogicalBufferSource* CLogicalBufferSource::NewL(TAny* aParameters)
	{
	DP_STATIC_CONTEXT(CLogicalBufferSource::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalBufferSource* self = new(ELeave)CLogicalBufferSource(aParameters);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CLogicalBufferSource::ConstructL()
	{
	DP_CONTEXT(CLogicalBufferSource::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogicalBufferSource::~CLogicalBufferSource()
	{
	DP_CONTEXT(CLogicalBufferSource::~CLogicalBufferSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// from MMMFBufferSource
// ---------------------------------------------------------------------------
// CLogicalBufferSource::SetDataSupplier
// ---------------------------------------------------------------------------
//  
TInt CLogicalBufferSource::SetDataSupplier(MMMFAudioDataSupplier& aSupplier)
	{
	DP_CONTEXT(CLogicalBufferSource::SetDataSupplier *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	iSupplier = &aSupplier;
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::BufferFilled
// ---------------------------------------------------------------------------
//  
TInt CLogicalBufferSource::BufferFilled(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CLogicalBufferSource::BufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = iAdaptationBufferSource->BufferFilled(aBuffer);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::BuffersDiscarded
// ---------------------------------------------------------------------------
//  
TInt CLogicalBufferSource::BuffersDiscarded()
	{
	DP_CONTEXT(CLogicalBufferSource::BuffersDiscarded *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = iAdaptationBufferSource->BuffersDiscarded();
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::IsTypeOf
// ---------------------------------------------------------------------------
//  
TBool CLogicalBufferSource::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalBufferSource::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if (iType == aTypeId)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::Interface
// ---------------------------------------------------------------------------
// 
TAny* CLogicalBufferSource::Interface(TUid aType)
	{
	DP_CONTEXT(CLogicalBufferSource::Interface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAny* interface = NULL;
	if(aType==KUidMmfBufferSource)
		{
		interface = static_cast<MMMFBufferSource*>(this);
		}
	DP0_RET(interface, "%d");
	}

// From MMMFAudioDataSupplier
// ---------------------------------------------------------------------------
// CLogicalBufferSource::BufferToBeFilled
// ---------------------------------------------------------------------------
// 
void CLogicalBufferSource::BufferToBeFilled(MMMFBufferSource* /*aSource*/, CMMFBuffer* aBuffer)
	{
	iSupplier->BufferToBeFilled(this, aBuffer);
	}

// ---------------------------------------------------------------------------
// CLogicalBufferSource::DiscardBuffers
// ---------------------------------------------------------------------------
// 
void CLogicalBufferSource::DiscardBuffers(MMMFBufferSource* /*aSource*/)
	{
	iSupplier->DiscardBuffers(this);
	}

// end of file
