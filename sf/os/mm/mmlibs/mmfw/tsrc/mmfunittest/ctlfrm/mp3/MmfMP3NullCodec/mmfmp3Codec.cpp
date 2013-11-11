// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is a test codec which takes MP3 data & gives it back to the datapath
// pretending it has converted it to PCM16 data thus producing lots of noise
// 
//

#include <e32std.h>

#include <mmf/plugin/mmfcodecimplementationuids.hrh>
#include <ecom/implementationproxy.h>

#include "MP3Uids.hrh"

#include "mmfmp3Codec.h"

//*******************************************************************
//*  MP3 Codec to 16 bit PCM Class:
//*******************************************************************

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFMP3To16PcmCodec::NewL(TAny* aInitParams)
	{
	CMMFMP3To16PcmCodec* self=new(ELeave) CMMFMP3To16PcmCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFMP3To16PcmCodec::~CMMFMP3To16PcmCodec()
	{
	}

CMMFMP3To16PcmCodec::CMMFMP3To16PcmCodec()
	{

	}

void CMMFMP3To16PcmCodec::ConstructL(TAny* /*aInitParams*/)
	{
	
	}

void CMMFMP3To16PcmCodec::ResetL()
	{
	}


TCodecProcessResult CMMFMP3To16PcmCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{

	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete; // dest full
	result.iSrcBytesProcessed = 0;
	result.iDstBytesAdded = 0;

	// convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	TUint srcLen = src->Data().Length() - src->Position();
	TUint dstMaxLen = dst->Data().MaxLength() - dst->Position();
	TUint dstLen = dstMaxLen;
	
	dst->Data().SetLength(0);

	if (srcLen == 0)
		{
		result.iStatus = TCodecProcessResult::EDstNotFilled;	// get some more source data
		}
	else
		{
		TPtrC8 srcPtrC(src->Data().Ptr(), dstLen);

		dst->Data() = srcPtrC;
		
		result.iSrcBytesProcessed = srcLen;
		result.iDstBytesAdded = dst->Data().Length();
		result.iStatus = TCodecProcessResult::EProcessIncomplete; // dest full
		}

	return result;
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidMP3ImplementationCodec,	CMMFMP3To16PcmCodec::NewL)
	};



EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

