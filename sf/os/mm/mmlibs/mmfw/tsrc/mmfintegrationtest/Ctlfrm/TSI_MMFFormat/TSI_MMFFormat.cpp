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
//

#include <ecom/ecom.h>
#include <mmf/common/mmffourcc.h>

#include "MMFFormat.hrh"
#include "TSI_MMFFormat.h"


/**
*
* NewL
* @param aSource
* @return CMMFFormatDecode* 
* 
*/
CMMFReadFormat* CMMFReadFormat::NewL()
	{
	CMMFReadFormat* self = new(ELeave)CMMFReadFormat();
	return self;
	}

CMMFReadFormat::CMMFReadFormat()
	{
	}

/**
*
* ~CMMFWavFormatRead
*
*/
CMMFReadFormat::~CMMFReadFormat()
	{
	}
	
/**
*
* SourceThreadLogon
* @param aEventHandler
*
*/
TInt CMMFReadFormat::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{//pass through to source clip
	return KErrNone;
	}

/**
*
* SourcePrimeL
*
*/
void CMMFReadFormat::SourcePrimeL()
	{
	}

/**
*
* SourcePlayL
*
*/
void CMMFReadFormat::SourcePlayL()
	{
	}

/**
*
* SourcePauseL
*
*/
void CMMFReadFormat::SourcePauseL()
	{
	}

/**
*
* SourceStopL
*
*/
void CMMFReadFormat::SourceStopL()
	{
	}

/**
*
* SourceThreadLogoff
*
*/
void CMMFReadFormat::SourceThreadLogoff()
	{
	}

/**
*
* Streams
* @param aMediaType
* @returns TUint
*
*/

TUint CMMFReadFormat::Streams(TUid /*aMediaType*/) const
	{
	return 0;
	}

/**
*
* FrameTimeInterval
* @param  aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/

TTimeIntervalMicroSeconds CMMFReadFormat::FrameTimeInterval(TMediaId /*aMediaId*/) const
	{
	return TTimeIntervalMicroSeconds(0);
	}

/**
*
* NegotiateSourceL
* @param aSink
* 
*/
void CMMFReadFormat::NegotiateSourceL(MDataSink& /*aSink*/)
	{
	}
/**
*
* Duration
* @param aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/


TTimeIntervalMicroSeconds CMMFReadFormat::Duration(TMediaId /*aMediaId*/) const
	{
	return (TTimeIntervalMicroSeconds(0));
	}
	
/**
*
* SetSourceDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @returns TInt
*
*/

TInt CMMFReadFormat::SetSourceDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/)
	{
	return KErrNotSupported;
	}

/**
*
* FillBufferL
* @param aBuffer
* @param aConsumer
* @param aMediaId
*
*/
void CMMFReadFormat::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/, TMediaId /*aMediaId*/)
	{
	}

/**
*
* CreateSourceBufferL
* @param aMediaId
* @param aReference
*
*/
CMMFBuffer* CMMFReadFormat::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
	{
	CMMFBuffer* buffer = NULL;
	return buffer;
	}

/**
*
* CreateSourceBufferL
* @param aMediaId
* @param aSinkBuffer
* @param aReference
*
*/
CMMFBuffer* CMMFReadFormat::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
	{
	return CreateSourceBufferL(aMediaId, aReference);
	}

/**
*	
* SourceDataTypeCode
* @param aMediaId
*
*/
TFourCC CMMFReadFormat::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return KMMFFourCCCodePCM16;
	}

//called by MDataSink to pass back emptied buffer to the source
void CMMFReadFormat::BufferEmptiedL(CMMFBuffer* /* aBuffer */)
	{
	}
	
TBool CMMFReadFormat::CanCreateSourceBuffer()
	{
	return ETrue;
	}
	
void CMMFReadFormat::ConstructSourceL( const TDesC8& /* aInitData */)
	{
	}
	
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidABCImplementationFormat,	CMMFReadFormat::NewL)
	};

/**
* 
* ImplementationGroupProxy
* @param aTableCount
* @return TImplementationProxy*
*
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
