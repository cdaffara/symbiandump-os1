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
// * INCLUDE FILES:
// 
//

// Standard includes
#include <e32std.h>

// Private Generic Library includes
#include <ecom/implementationproxy.h>
#include "GSM610.H"
#include "gsm610fr.h"
#include <mmf/plugin/mmfcodecimplementationuids.hrh>


//*******************************************************************
//*  GSM Codec to 16 bit PCM Class:
//*******************************************************************

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFGsmTo16PcmCodec::NewL(TAny* aInitParams)
	{
	CMMFGsmTo16PcmCodec* self=new(ELeave) CMMFGsmTo16PcmCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFGsmTo16PcmCodec::~CMMFGsmTo16PcmCodec()
	{
	delete iCodecPtr;
	}

CMMFGsmTo16PcmCodec::CMMFGsmTo16PcmCodec()
	{

	}

void CMMFGsmTo16PcmCodec::ConstructL(TAny* /*aInitParams*/)
	{
	iCodecPtr = new (ELeave) CGsmTo16PcmWavCodec();
	iCodecPtr->ConstructL();
	iLastFrameNumber = 0;
	}


//handy porting stuff from old MS

//to convert from a CMMFBuffer to a TUint8*
//---	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
//---	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());

//to convert from a CMMFBuffer to a TMMFPtr8 (TMMFPtr8)
//---	TMMFPtr8 codecSrc;
//---	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
//---	codecSrc.Set(iSrc->Data());


TCodecProcessResult CMMFGsmTo16PcmCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{

	TMMFPtr8 codecSrc;
	TMMFPtr8 codecDst;

	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint srcLen = iSrc->Data().Length();
	const TUint dstMaxLen = iDst->Data().MaxLength() - iDst->Position();

	// This is checked only on the first frame.
	if ((dstMaxLen < KPcmInputFrameSize) && (iSrc->FrameNumber() <= 1))
		User::Leave(KErrArgument);
	 
	TUint dstAccumulator = 0;
	TUint srcAccumulator = 0;

	TUint dstAdded = KPcmInputFrameSize;
	TUint srcAdded = 0;

	if ((iSrc->FrameNumber() == 1) && (iSrc->Position() == 0))
		{
		iCodecPtr->ResetAllL();
		}

	if ((iLastFrameNumber != 0) && (iSrc->FrameNumber() <= 1) && 
		(iDst->Position() == 0) && (iSrc->Position() == 0))
		{
		iCodecPtr->ResetAllL();
		}

	iLastFrameNumber = iSrc->FrameNumber();

	TInt srcShift = iSrc->Position();
	TInt dstShift = iDst->Position(); // Add offset for Dst Buffer

	while ((dstAccumulator < dstMaxLen) && (dstAdded))
		{
		//this code chops the data buffer into 65 (or less) byte chunks
		codecSrc.Set(iSrc->Data());
		codecDst.Set(iDst->Data());
		codecDst.SetLength(iDst->Data().MaxLength());
		//move the data to the end of the last bit of the buffer processed
		codecSrc.Shift(srcAccumulator + srcShift);

		//codecDst.Shift(dstAccumulator);
		codecDst.Shift(dstAccumulator + dstShift); // Add offset for Dst Buffer
		
		iCodecPtr->ProcessL(&codecSrc, &codecDst);

		dstAdded = codecDst.Length();
		srcAdded = codecSrc.Length();

		//rules to trigger a codec reset
		if (dstAdded == KPcmInputFrameSize)
			iCodecPtr->Reset();
		if (srcAdded == 0)
			iCodecPtr->Reset();
	
		dstAccumulator += codecDst.Length();
		srcAccumulator += codecSrc.Length();

		}

	//iDst->Data().SetLength(dstAccumulator);
	iDst->Data().SetLength(dstAccumulator + dstShift); // Add offset for Dst Buffer
	
	result.iSrcBytesProcessed = srcAccumulator;
	result.iDstBytesAdded = dstAccumulator;

	if (!srcAdded)
		{
		srcAdded = srcAdded;
		}
	
	if (result.iSrcBytesProcessed + iSrc->Position() >= srcLen)
		result.iStatus = TCodecProcessResult::EProcessComplete;
		
	if (result.iDstBytesAdded < dstMaxLen)
		result.iStatus = TCodecProcessResult::EDstNotFilled;

	return result;
	}


//*******************************************************************
//* 16 bit PCM to GSM Codec Class:
//*******************************************************************

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMF16PcmToGsmCodec::NewL(TAny* aInitParams)
	{
	CMMF16PcmToGsmCodec* self=new(ELeave) CMMF16PcmToGsmCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMF16PcmToGsmCodec::~CMMF16PcmToGsmCodec()
	{
	delete iCodecPtr;
	}

CMMF16PcmToGsmCodec::CMMF16PcmToGsmCodec()
	{

	}

void CMMF16PcmToGsmCodec::ConstructL(TAny* /*aInitParams*/)
	{
	iCodecPtr = new (ELeave) C16PcmToGsmWavCodec();
	iCodecPtr->ConstructL();
	iLastFrameNumber = 0;
	}

//handy porting stuff from old MS

//to convert from a CMMFBuffer to a TUint8*
//---	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
//---	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());

//to convert from a CMMFBuffer to a TMMFPtr8 (TMMFPtr8)
//---	TMMFPtr8 codecSrc;
//---	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
//---	codecSrc.Set(iSrc->Data());



TCodecProcessResult CMMF16PcmToGsmCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{

	TMMFPtr8 codecSrc;
	TMMFPtr8 codecDst;


	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	//const TUint srcLen = iSrc->Data().Length();
	TUint srcLen = iSrc->Data().Length();
	const TUint dstMaxLen = iDst->Data().MaxLength() - iDst->Position();

	// This is checked only on the first frame.
	if ((dstMaxLen < KGsmEncodedFrameSize) && (iSrc->FrameNumber() <= 1))
		User::Leave(KErrArgument);


	TUint dstAccumulator = 0;
	TUint srcAccumulator = iSrc->Position();

	TUint dstAdded = 0;
	TUint srcAdded = 0;
	
	if ((iSrc->FrameNumber() == 1) && (iSrc->Position() == 0))
		{
		iCodecPtr->ResetAllL();
		}

	if ((iLastFrameNumber != 0) && (iSrc->FrameNumber() <= 1) && 
		(iDst->Position() == 0) && (iSrc->Position() == 0))
		{
		iCodecPtr->ResetAllL();
		}

	iLastFrameNumber = iSrc->FrameNumber();

//	TInt dstShift = iSrc->Position();
	TInt dstShift = iDst->Position();

	while (srcAccumulator < srcLen)
		{
		// Create a copy of iSrc and pass that to the Codec as the Codec, 
		// for some reason, alters the source(iSrc).
		// Easier (& quicker) to make copy than to fix the actual Codec as the code is 
		// old and the GSM610 Codec is already quite slow.
		HBufC* copySrc = HBufC::NewLC(srcLen);
		TPtr8 copySrcPtr((TUint8*)copySrc->Ptr(), srcLen, srcLen);
		copySrcPtr.Copy((TUint8*)iSrc->Data().Ptr(), srcLen); 

		//this code chops the data buffer into 640 (or less) byte chunks
		codecSrc.Set(copySrcPtr/*iSrc->Data()*/);

		//codecSrc.Set(iSrc->Data());
		codecDst.Set(iDst->Data());
		codecDst.SetLength(iDst->Data().MaxLength());
		//move the data to the end of the last bit of the buffer processed
		codecSrc.Shift(srcAccumulator);
		codecDst.Shift(dstAccumulator + dstShift);
		
		iCodecPtr->ProcessL(&codecSrc, &codecDst);

		dstAdded = codecDst.Length();
		srcAdded = codecSrc.Length();

		//rules to trigger a codec reset
		if ((dstAdded == KGsmEncodedFrameSize) && (srcAdded == KPcmInputFrameSize))
			iCodecPtr->Reset();
		if (srcAdded == 0)
			iCodecPtr->Reset();
	
		dstAccumulator += codecDst.Length();
		srcAccumulator += codecSrc.Length();

		CleanupStack::PopAndDestroy(copySrc);

		//need to check that we don't process too much of the destination such that it won't fit in the dst buf
		if ((dstAccumulator + KGsmEncodedFrameSize) > dstMaxLen)
			break;
		}

	iDst->Data().SetLength(dstAccumulator + iDst->Position());
	
	result.iSrcBytesProcessed = srcAccumulator;
	result.iDstBytesAdded = dstAccumulator;

	if (!srcAdded)
		{
		srcAdded = srcAdded;
		}
	
	if (result.iSrcBytesProcessed + iSrc->Position() >= srcLen)
		result.iStatus = TCodecProcessResult::EProcessComplete; 
		
	if (result.iDstBytesAdded < dstMaxLen)
		{ //need to check that there is space remaining in the destination buffer to process more
		//need at least KGsmEncodeFrameSize bytes
		if ((dstMaxLen - result.iDstBytesAdded) >= KGsmEncodedFrameSize)
				result.iStatus = TCodecProcessResult::EDstNotFilled; //still space for more frames
//		else result.iStatus = TCodecProcessResult::EProcessComplete; //can't do anything more with src or dest
		}

	return result;
	}



// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecGSM610ToPCM16,	CMMFGsmTo16PcmCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM16ToGSM610,	CMMF16PcmToGsmCodec::NewL)

	};



EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::CGsmTo16PcmWavCodec
//*   Constructor for GSM to 16 bit PCM Codec.
//*******************************************************************

CGsmTo16PcmWavCodec::CGsmTo16PcmWavCodec()
	{

	Reset();

	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::~CGsmTo16PcmWavCodec
//*   Destructor for GSM to 16 bit PCM Codec.
//*******************************************************************

CGsmTo16PcmWavCodec::~CGsmTo16PcmWavCodec()
	{

	delete iGsmDecoder;

	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::ConstructL
//*   Perform 2nd pass of object construction (allocating data on heap).
//*******************************************************************

void CGsmTo16PcmWavCodec::ConstructL()
	{
	iGsmDecoder = new (ELeave) CGSM610FR_Decoder;
	iGsmDecoder->ConstructL();
	iGsmDecoder->StartL();

	iInBufferCount  = 0;
	iOutBufferCount = 0;
	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::Reset
//*   Reset the input/output buffer states.
//*******************************************************************

void CGsmTo16PcmWavCodec::Reset()
	{
	iInBufferPtr    = iInBuffer;
	iOutBufferPtr   = iOutBuffer;
	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::Reset
//*   Reset the input/output buffer states.
//*******************************************************************

void CGsmTo16PcmWavCodec::ResetAllL()
	{
	Reset();
	iInBufferCount  = 0;
	iOutBufferCount = 0;
	
	iGsmDecoder->StartL();
	}


//*******************************************************************
//* FUNCTION:  CGsmTo16PcmWavCodec::ProcessL
//*   Read GSM data (contained in aSrc) and convert it to raw 16 bit
//*   PCM data (contained in aDst).
//*
//*   NOTE that amount of data contained in input/output buffers
//*   varies, so if there is not enough input data or not enough
//*   room for the output data, then this function must buffer
//*   the data until enough data is received (or removed).
//*
//*   NOTE that this function only buffers input (or output) data
//*   if there is not enough data (or room for output) available
//*   in the input (output) stream.  Otherwise, the data is accessed
//*   directly from the input (output) stream which saves unnecessary
//*   copying of data.
//*******************************************************************

void CGsmTo16PcmWavCodec::ProcessL(TMMFPtr8* aSrc, TMMFPtr8* aDst)
	{

	TUint8* srcPtr  = NULL;
	TUint8* dstPtr  = NULL;
	const TUint    srcLen  = aSrc->Length();
	const TUint    dstLen  = aDst->Length();
	TInt	srcUsed = 0;
	TInt	dstUsed = 0;


	//**************************************************
	//* Get Input Data:  Only want to process more input
	//* data if the output buffer is empty.
	//**************************************************

	if ( iOutBufferCount == 0 )
		{
		srcPtr = CONST_CAST(TUint8*, aSrc->Ptr());


		//*************************************************
		//* If the input buffer is empty, and there is
		//* enough data in the input stream, then use the
		//* data from the input stream.  Else must append
		//* available input data to the input buffer.
		//*************************************************

		if ( (iInBufferCount == 0) && (srcLen >= KGsmEncodedFrameSize) )
			{
			srcUsed = KGsmEncodedFrameSize;
			}
		else
			{
			TInt canCache = KGsmEncodedFrameSize - iInBufferCount;
			srcUsed = Min (canCache, srcLen);

			for (TInt count = 0; count < srcUsed; count++)
				{
				//not worth use a mem copy
				*iInBufferPtr++ = *srcPtr++;
				}


			//*************************************************
			//* If the input buffer is now full, then we can
			//* process this data. Otherwise, we don't have
			//* enough data so set srcPtr to NULL to indicate
			//* that there's no data to process yet.
			//*************************************************

			srcPtr = NULL;
			iInBufferCount += srcUsed;
			if ( iInBufferCount == KGsmEncodedFrameSize )
				{
				srcPtr         = iInBuffer;
				iInBufferPtr   = iInBuffer;
				iInBufferCount = 0;
				}
			}
		}


	//*************************************************
	//* If there is enough input data, then determine
	//* where the output should go (output stream or
	//* output buffer if output stream doesn't have
	//* enough room).  Then decode the data.
	//*************************************************

	if ( srcPtr != NULL )
		{
		// Determine where to put output data
		dstPtr = CONST_CAST(TUint8*, aDst->Ptr());
		dstUsed = KPcmInputFrameSize;

		if ( dstLen < KPcmInputFrameSize )
			{
			dstPtr = iOutBuffer;
			iOutBufferCount = KPcmInputFrameSize;
			}

		// Decode the data
		iGsmDecoder->ExecuteL (srcPtr, dstPtr);
		}


	//*************************************************
	//* If any data is stored in the output buffer,
	//* then output as much of it as well fit in the
	//* output stream.
	//*************************************************

	if ( iOutBufferCount > 0 )
		{
		dstUsed = Min ((TInt)iOutBufferCount, (TInt)dstLen);
		TPtrC8 outPtr (iOutBufferPtr, dstUsed);

		aDst->SetLength(0);
		aDst->Append (outPtr);

		iOutBufferCount -= dstUsed;
		iOutBufferPtr   += dstUsed;

		if ( iOutBufferCount == 0 )
			{
			iOutBufferPtr = iOutBuffer;
			}
		}


	//*************************************************
	//* Modify the length attributes of the source and
	//* destination data streams to inform the caller
	//* of how much data was used in each buffer.
	//*************************************************

	aSrc->SetLength(srcUsed); 
	aDst->SetLength(dstUsed); 

	}


//*******************************************************************
//* FUNCTION:  C16PcmToGsmWavCodec::C16PcmToGsmWavCodec
//*   Constructor for 16 bit PCM to GSM Codec.
//*******************************************************************

C16PcmToGsmWavCodec::C16PcmToGsmWavCodec()
	{

	Reset();

	}


//*******************************************************************
//* FUNCTION:  C16PcmToGsmWavCodec::~C16PcmToGsmWavCodec
//*   Destructor for 16 bit PCM to GSM Codec.
//*******************************************************************

C16PcmToGsmWavCodec::~C16PcmToGsmWavCodec()
	{

	delete iGsmEncoder;

	}


//*******************************************************************
//* FUNCTION:  C16PcmToGsmWavCodec::ConstructL
//*   Perform 2nd pass of object construction (allocating data on heap).
//*******************************************************************

void C16PcmToGsmWavCodec::ConstructL()
	{

	iGsmEncoder = new (ELeave) CGSM610FR_Encoder;
	iGsmEncoder->ConstructL();
	iGsmEncoder->StartL();
	iInBufferCount  = 0;
	iOutBufferCount = 0;

	}


//*******************************************************************
//* FUNCTION:  C16PcmToGsmWavCodec::Reset
//*   Reset the input/output buffer states.
//*******************************************************************

void C16PcmToGsmWavCodec::Reset()
	{

	iInBufferPtr    = iInBuffer;
	iOutBufferPtr   = iOutBuffer;
	}


void C16PcmToGsmWavCodec::ResetAllL()
	{
	Reset();
	iInBufferCount  = 0;
	iOutBufferCount = 0;
 
	iGsmEncoder->StartL();
	}


//*******************************************************************
//* FUNCTION:  C16PcmToGsmWavCodec::ProcessL
//*   Read raw 16 bit PCM data (contained in aSrc) and convert to
//*   GSM data and return to caller (in aDst).
//*
//*   NOTE that amount of data contained in input/output buffers
//*   varies, so if there is not enough input data or not enough
//*   room for the output data, then this function must buffer
//*   the data until enough data is received (or removed).
//*
//*   NOTE that this function only buffers input (or output) data
//*   if there is not enough data (or room for output) available
//*   in the input (output) stream.  Otherwise, the data is accessed
//*   directly from the input (output) stream which saves unnecessary
//*   copying of data.
//*******************************************************************


void C16PcmToGsmWavCodec::ProcessL(TMMFPtr8* aSrc, TMMFPtr8* aDst)
	{
	TUint8* srcPtr  = NULL;
	TUint8* dstPtr  = NULL;
	const TUint    srcLen  = aSrc->Length();
	const TUint    dstLen  = aDst->Length();
	TInt	srcUsed = 0;
	TInt	dstUsed = 0;


	//**************************************************
	//* Get Input Data:  Only want to process more input
	//* data if the output buffer is empty.
	//**************************************************

	if ( iOutBufferCount == 0 )
		{
		srcPtr = CONST_CAST(TUint8*, aSrc->Ptr());


		//*************************************************
		//* If the input buffer is empty, and there is
		//* enough data in the input stream, then use the
		//* data from the input stream.  Else must append
		//* available input data to the input buffer.
		//*************************************************

		if ( (iInBufferCount == 0) && (srcLen >= KPcmInputFrameSize) )
			{
			srcUsed = KPcmInputFrameSize;
			}
		else
			{
			TInt canCache = KPcmInputFrameSize - iInBufferCount;
			srcUsed = Min (canCache, srcLen);

			for (TInt count = 0; count < srcUsed; count++)
				{
				*iInBufferPtr++ = *srcPtr++;
				}


			//*************************************************
			//* If the input buffer is now full, then we can
			//* process this data. Otherwise, we don't have
			//* enough data so set srcPtr to NULL to indicate
			//* that there's no data to process yet.
			//*************************************************

			srcPtr = NULL;
			iInBufferCount += srcUsed;
			if ( iInBufferCount == KPcmInputFrameSize )
				{
				srcPtr         = iInBuffer;
				iInBufferPtr   = iInBuffer;
				iInBufferCount = 0;
				}
			}
		}


	//*************************************************
	//* If there is enough input data, then determine
	//* where the output should go (output stream or
	//* output buffer if output stream doesn't have
	//* enough room).  Then encode the data.
	//*************************************************

	if ( srcPtr != NULL )
		{
		// Determine where to put output data
		dstPtr = CONST_CAST(TUint8*, aDst->Ptr());
		dstUsed = KGsmEncodedFrameSize;

		if ( dstLen < KGsmEncodedFrameSize )
			{
			dstPtr = iOutBuffer;
			iOutBufferCount = KGsmEncodedFrameSize;
			}

		// Encode the data
		iGsmEncoder->ExecuteL (srcPtr, dstPtr);
		}


	//*************************************************
	//* If any data is stored in the output buffer,
	//* then output as much of it as well fit in the
	//* output stream.
	//*************************************************

	if ( iOutBufferCount > 0 )
		{
		dstUsed = Min ((TInt)iOutBufferCount, (TInt)dstLen);
		TPtrC8 outPtr (iOutBufferPtr, dstUsed);

		aDst->SetLength(0);
		aDst->Append (outPtr);

		iOutBufferCount -= dstUsed;
		iOutBufferPtr   += dstUsed;

		if ( iOutBufferCount == 0 )
			{
			iOutBufferPtr = iOutBuffer;
			}
		}


	//*************************************************
	//* Modify the length attributes of the source and
	//* destination data streams to inform the caller
	//* of how much data was used in each buffer.
	//*************************************************

	aSrc->SetLength(srcUsed); 
	aDst->SetLength(dstUsed); 


	}

