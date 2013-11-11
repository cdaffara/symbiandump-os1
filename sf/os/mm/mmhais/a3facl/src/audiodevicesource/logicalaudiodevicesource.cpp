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
//



#include <ecom/implementationproxy.h> // For making it ECom plugin
#include <a3f/mbuffersource.h>

#include "logicalaudiodevicesource.h"

#include <a3f/audioprocessingunittypeuids.h>


// ======== LOCAL FUNCTIONS ========
// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KAudioDeviceSourceUid, CLogicalAudioDeviceSource::NewL), //KUidAudioDeviceSource
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CLogicalAudioDeviceSource::CLogicalAudioDeviceSource(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalAudioDeviceSource::CLogicalAudioDeviceSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
CLogicalAudioDeviceSource* CLogicalAudioDeviceSource::NewL(TAny* aParam)
	{
	DP_STATIC_CONTEXT(CLogicalAudioDeviceSource::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioDeviceSource* self = new(ELeave)CLogicalAudioDeviceSource(aParam);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
void CLogicalAudioDeviceSource::ConstructL()
	{
	DP_CONTEXT(CLogicalAudioDeviceSource::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CLogicalAudioDeviceSource::~CLogicalAudioDeviceSource()
	{
	DP_CONTEXT(CLogicalAudioDeviceSource::~CLogicalAudioDeviceSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

TBool CLogicalAudioDeviceSource::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalAudioDeviceSource::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if (iType == aTypeId)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}

// end of file
