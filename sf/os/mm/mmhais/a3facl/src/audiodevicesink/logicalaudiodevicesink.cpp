// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "logicalaudiodevicesink.h"
#include <a3f/audioprocessingunittypeuids.h>

#include <ecom/implementationproxy.h> // For making it ECom plugin

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KAudioDeviceSinkUid,  CLogicalAudioDeviceSink::NewL),
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
CLogicalAudioDeviceSink::CLogicalAudioDeviceSink(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalAudioDeviceSink::CLogicalAudioDeviceSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CLogicalAudioDeviceSink* CLogicalAudioDeviceSink::NewL(TAny* aParam)
	{
	DP_STATIC_CONTEXT(CLogicalAudioDeviceSink::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioDeviceSink* self = new(ELeave)CLogicalAudioDeviceSink(aParam);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CLogicalAudioDeviceSink::~CLogicalAudioDeviceSink()
	{
	DP_CONTEXT(CLogicalAudioDeviceSink::RegisterComponentObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioDeviceSink::IsTypeOf
// ---------------------------------------------------------------------------
TBool CLogicalAudioDeviceSink::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalAudioDeviceSink::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if (iType == aTypeId)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}
	
// end of file
