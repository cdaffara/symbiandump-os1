// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <btsdp.h>
#include "reqhandler.h"
#include "pduhandler.h"
#include "listener.h"
#include "sdpconsts.h"
#include "SDPDatabase.h"
#include "MAttributeVisitor.h"
#include "ExtractorVisitor.h"
#include "responsesizevisitor.h"
#include "DataEncoder.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP_SERVER);
#endif

// statics

void SdpReqHandler::HandleL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu)
	{
	switch (aReqPdu.iPduId)
		{
	case EServiceSearchRequest: 
		HandleServiceSearchL(aDatabase, aReqPdu, aRespPdu);
		break;
	case EServiceAttributeRequest:
		HandleServiceAttributeL(aDatabase, aReqPdu, aRespPdu);
		break;
	case EServiceSearchAttributeRequest:
		HandleServiceSearchAttributeL(aDatabase, aReqPdu, aRespPdu);
		break;
	default:
		User::Leave(KErrArgument);
		}
	}

#ifdef __FLOGGING__
TInt SdpReqHandler::RunError(TInt aError, const TSdpPdu& aReqPdu, TSdpPdu &aRespPdu)
#else
TInt SdpReqHandler::RunError(TInt aError, const TSdpPdu& /*aReqPdu*/, TSdpPdu &aRespPdu)
#endif
/**
	Send an appropriate error.

	Parameter format is
 @verbatim
		Error Code					TUint16
		Error Info					Variable (0 in 1.0B)
 @endverbatim
**/
	{
	TSdpErrorCodes code;
	switch (aError)
		{
	case KErrNotSupported:
		code = EInvalidSdpVersion;
		break;
	case KErrBadHandle:
		code = EInvalidServiceRecordHandle;
		break;
	case KErrOverflow: 
	case KErrUnderflow:
	case KErrTooBig:
		code = EInvalidPduSize;
		break;
	case KErrNotReady:
	case KErrUnknown:
	case KErrLocked:
		code = EInvalidContinuationState;
		break;
	case KErrNoMemory:
	case KErrHardwareNotAvailable:
		code = EInsufficientResources;
		break;
	case KErrCorrupt:
	case KErrArgument:
	default:
		code = EInvalidRequestSyntax;
		break;
		}
	aRespPdu.iPduId = EErrorResponse;
	aRespPdu.iParams.SetLength(2);
	BigEndian::Put16(&aRespPdu.iParams[0], TUint16(code));

	LOG1(_L("SdpReqHandler::RunError(%d)"), aError);
	
	return KErrNone;
	}


void SdpReqHandler::HandleServiceSearchL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu)
/**
	Handle Service Search request.
	Request parameter format is
 @verbatim
		Service search pattern		DES
		Max record handle count		TUint16
		Continuation state			1 + 0-16 bytes
 @endverbatim

	Response parameter format is
 @verbatim
		Total record count			TUint16
		Records in this response	TUint16
		Record handle list			TUint32 * N
		Continuation State			1 + 0-16 bytes
 @endverbatim

**/
	{
// Parse the request parameters
	TPtr8 params(aReqPdu.iParams);
	CSdpPDUHandler* pHandler = new 	(ELeave) CSdpPDUHandler();
	CleanupStack::PushL(pHandler);
	TInt maxTotalRecCount;
	TInt len;
	TInt rem;
	TInt sentRecords=0;

	CSizeAccumulator* collector = CSizeAccumulator::NewL();
	CleanupStack::PushL(collector);

	CSdpSearchPattern* pattern = pHandler->UUIDListLC(params, KRecHandleCountSize, len, rem, maxTotalRecCount);

// first level checks for continuation parameter
	TBool inContFlag = pHandler->ContinuationL(params, len, rem);

	CResponseSizeVisitor::SizeRespSSL(aDatabase, *pattern, *collector);
	CleanupStack::PopAndDestroy(/*pattern*/);

	TInt fullSize = collector->HandleCount(); // we only have handles here
	fullSize = Min(fullSize, maxTotalRecCount); // we may not be able to send all the handles anyway
//	TInt16 localCRC = collector->CRC(); this is for attributes
	if (inContFlag)
		{
		if (fullSize != (TInt)pHandler->FullLength()) User::Leave(KErrUnknown);	// continuation check
		sentRecords = pHandler->ContinuationOffset(); // this is in count of handles, not bytes
		if (fullSize <= sentRecords) User::Leave(KErrUnknown);	// continuation check
//		if (localCRC != pHandler->ReqCRC()) User::Leave(KErrUnknown); continuation check for attributes
		}
	else
		sentRecords = 0;

/*
	working out if we can send all or some of the data:
	if there is a maxhandles outstanding, comply with that
	if we can send all the data (handles) fine
	if we can't send all the data, reduce the buffer by the continuation
*/
	TInt pduSize = fullSize - sentRecords;	// the count of handles left to send

	// start with the buffer size less: the minimum allocated, the total and this time handle count size and the empty continuation header
	TInt bufferUsableLen = aRespPdu.iParams.MaxLength() - ((KRspHandleCountSize * 2) + KContStateHeader);

	TBool outContFlag;
	if (bufferUsableLen < (pduSize * KSdpRecordHandleSize)) 
		{// has to be a continuation
		outContFlag = ETrue;
		bufferUsableLen -= KSdpContinuationStateLength;	// we need the header now
/*
	when sending attributes (AR, SAS) make sure we don't leave a single byte
	to be sent in the next continuation. Otherwise, reduce the payload this
	time by 1 to make sure we comply with the specification and send a minimum
	of two bytes in the response.
*/
		}
	else
		{
		outContFlag = EFalse;
// we can complete this request this time
		}

	TUint handleCount = bufferUsableLen >> 2;	// get the length in handle speak
	pduSize = Min(pduSize, handleCount);

// end of common second stage continuation processing

	// Write the response packet
	aRespPdu.iPduId = EServiceSearchResponse;
	aRespPdu.iParams.SetMax();
	TPtr8 responseHandles(0,0);
	responseHandles.Set(&aRespPdu.iParams[0], 0, aRespPdu.iParams.MaxLength());
	TInt oldLen = responseHandles.Length();	// erm, zero
	responseHandles.SetLength(oldLen + (KRspHandleCountSize * 2));
	BigEndian::Put16(&responseHandles[KRspTotalCountOffset], (TUint16)fullSize);
	BigEndian::Put16(&responseHandles[KRspHandleCountOffset], (TUint16)pduSize);
	for (TInt i = 0; i < pduSize; i++)
		{
		oldLen = responseHandles.Length();
		responseHandles.SetLength(oldLen + KSdpRecordHandleSize);
		BigEndian::Put32(&responseHandles[oldLen], collector->HandleAt(sentRecords+i));
		}
	oldLen = responseHandles.Length();
	if (outContFlag)
		{
		responseHandles.SetLength(oldLen + KSdpContinuationStateLength + KContStateHeader);
		responseHandles[oldLen] = KSdpContinuationStateLength;
		BigEndian::Put32(&responseHandles[oldLen + KContContOff + KContStateHeader], (sentRecords + pduSize));
		BigEndian::Put32(&responseHandles[oldLen + KContTotOff + KContStateHeader], fullSize);
		BigEndian::Put16(&responseHandles[oldLen + KContCrcOff + KContStateHeader], 0);		// CRC not used
		}
	else
		{
		responseHandles.SetLength(oldLen + KContStateHeader);
		responseHandles[oldLen] = 0;
		}
	aRespPdu.iParams.SetLength(responseHandles.Length());
	CleanupStack::PopAndDestroy(2 /*collector, pHandler*/);
	}


void SdpReqHandler::HandleServiceAttributeL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu)
/**
	Handle Service Attribute request.
	Request parameter format is
 @verbatim
		Service record handle		TUint32
		Max Attribute byte count	TUint16
		Attribute ID/range list		DES
		Continuation state			1 + 0-16 bytes
 @endverbatim

	Response parameter format is
 @verbatim
		byte count of attr list		TUint16
		Attribute ID & Value		DES
		Continuation State			1 + 0-16 bytes
 @endverbatim

**/
	{
// Parse the request parameters
	TPtr8 fullParams(aReqPdu.iParams);
	if (fullParams.Length() < KRecAttribListOffset)
	    {
	    User::Leave(KErrUnderflow);
	    }
	
	TPtrC8 params = fullParams.Right(fullParams.Length() - KRecAttribListOffset);
	TInt len = params.Length();
	TInt rem;
	TInt sentRecords=0;
	TInt sentAttributes=0;

	CSdpPDUHandler* pHandler = new (ELeave) CSdpPDUHandler();
	CleanupStack::PushL(pHandler);
	CSizeAccumulator* collector = CSizeAccumulator::NewL();
	CleanupStack::PushL(collector);


// record handle to search
	TSdpServRecordHandle recordHandle = BigEndian::Get32(&fullParams[KRecHandleOffset]);
// maximum byte count for results
	TInt maxTotalAttributeCount = BigEndian::Get16(&fullParams[KAttributeCountOffset]);

// list of Attributes or ranges	
	CSdpAttrIdMatchList *attMatchList = pHandler->AttrListLC(params, rem);
// check for the continuation header
	TBool inContFlag = pHandler->ContinuationL(params, len, rem);
// find the record
	CSdpServRecord* theRecord=0;
// changed to allow testing with Tsdpdp.cpp
	for(TServRecordIter recIter(aDatabase.RecordIter()); recIter; recIter++)
	{// Iterate thru records in Db
		if ((*recIter).Handle() == recordHandle)
			{
			theRecord = recIter;
			break;
			}
		}
//  record not found
	if (!theRecord) User::Leave(KErrBadHandle);

// size the response
	CResponseSizeVisitor::SizeRespARL(*theRecord, *attMatchList, *collector);
	CleanupStack::PopAndDestroy(/*attMatchList*/);
// should only have one record in the size list
	if (collector->HandleCount() != 1) User::Leave(KErrArgument);

// some checks on continuation
	TUint fullSize = collector->SizeLeft(); // we can always send all bytes the max is only per PDU
// the sizer includes the DES size, but we need the record size less the header
// NOTE in the SAS case we have a set (DES) of DES, but no DES header in front of them.
	TUint innerRecSize = collector->HandleSize(0); // should be totalSize less the DES
	TInt desSize = CSdpPDUHandler::DesSize(innerRecSize);
	if (fullSize == 0) fullSize += desSize; // give it the smallest header if there are no attributes
	TInt16 localCRC=0;
	TUint sentBytes;
	TUint sentBytesCurAttr;			// bytes left to send on current attribute
	if (inContFlag)
		{
		if (fullSize != pHandler->FullLength()) User::Leave(KErrUnknown);	// continuation check
		sentBytes = pHandler->ContinuationOffset(); // this is in bytes, not handles like service search
		if (fullSize <= sentBytes) User::Leave(KErrUnknown);	// continuation check
// FIXME removed CRC handling until continuation works
//		localCRC = collector->CrcAttribs();						// CRC is only for attributes
		if (localCRC != pHandler->ReqCRC()) User::Leave(KErrUnknown);				// continuation check for attributes
// collector->StartAt uses the bytes already sent to discover how far through an 
// attribute we are at the beginning of a new pdu in a continued response  
		TBool adj = collector->StartAt(sentBytes, sentBytesCurAttr, sentRecords, sentAttributes); // this is for attributes
		if (!adj) User::Leave(KErrUnknown);
		}
	else
		{
		sentBytes = 0;
		sentBytesCurAttr = 0;
		}
/*
	working out if we can send all or some of the data:
	if there is a maxbytes outstanding, comply with that
	if we can send all the data (bytes) fine
	if we can't send all the data, reduce the buffer by the continuation
*/
	TInt pduSize = fullSize - sentBytes;	// the data left to send

// start with the buffer size less: 
//		the byte count size and the empty continuation header
	TInt bufferUsableLen = aRespPdu.iParams.MaxLength() - (KRspAttributeCountSize + KContStateHeader);
	TInt bufferThisSize = Min(maxTotalAttributeCount, bufferUsableLen);

	TBool outContFlag;
	if (bufferThisSize < pduSize) 
		{
		outContFlag = ETrue;
		bufferUsableLen -= KSdpContinuationStateLength;	// we need the header now
		bufferThisSize = Min(maxTotalAttributeCount, bufferUsableLen); // again the smallest
//	when sending attributes (AR, SAS) make sure we don't leave a single byte
//	to be sent in the next continuation. Otherwise, reduce the payload this
//	time by 1 to make sure we comply with the specification and send a minimum
//	of two bytes in the response.
		if ((pduSize - bufferThisSize) == 1) bufferThisSize -= 1;
		if (!inContFlag)
			{
// FIXME CRC removed until continuation works
//			localCRC = collector->CrcAttribs();
			}
		}
	else
		{
		outContFlag = EFalse;
// we can complete this request this time
		}

	pduSize = Min(pduSize, bufferThisSize);

// end of common second stage continuation processing

	TInt writtenSize = pduSize;  // we will be reducing the pduSize

// Write the response packet
	aRespPdu.iPduId = EServiceAttributeResponse;
	aRespPdu.iParams.SetMax();
	TPtr8 responseAttributes(0,0);
	responseAttributes.Set(&aRespPdu.iParams[0], 0, aRespPdu.iParams.MaxLength());
	TElementEncoder attributeEncoder(responseAttributes);
	TInt oldLen = responseAttributes.Length();
	responseAttributes.SetLength(oldLen + KRspAttributeCountSize);
	BigEndian::Put16(&responseAttributes[KRspAttributeCountOffset], (TUint16)(pduSize));
	CAttrSizeItem* currentAttItem = collector->AttributeOf(sentRecords, sentAttributes);
	TInt lastAttr = collector->AttrCount(sentRecords);
	TPtrC8 attrValPtr(0,0);
	TBuf8<1> wBuffer(1); // used for byte insertion
	if (!inContFlag)
		{// write the outer DES
		attributeEncoder.WriteDES(innerRecSize);
		pduSize -= desSize;
		}
	else
		{ // we are writing a continuation so straight into attribute data
		if (sentBytesCurAttr)
			{ // we have to write the rest of a previous attribute
			TPtrC8 partPtr(0,0);
			attrValPtr.Set(currentAttItem->Attr()->Value().Des());
			TInt unsentBytes = attrValPtr.Length() + KAttributeIdSize - sentBytesCurAttr;
			switch(sentBytesCurAttr)
				{ // we may have to send some of the attribute ID
			case 1:
				// Append most significant byte to responseAttributes
				wBuffer[0] = (TUint8)(currentAttItem->AttID() >> 8);
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize--;
				unsentBytes--;
				// No break statement is deliberate because we want to append 
				// the least significant byte to responseAttributes too
			case 2:
				wBuffer[0] = (TUint8)(currentAttItem->AttID() & 0xff);
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize--;
				unsentBytes--;
				partPtr.Set(attrValPtr);	// it's a whole pointer
				break;
			default:
				partPtr.Set(attrValPtr.Right(unsentBytes));
				break;
				}
			if (unsentBytes - pduSize > 0)	
				{// we don't even get to send one complete attribute...
				partPtr.Set(partPtr.Left(pduSize)); // adjust the size of the des we send.
				pduSize = 0;
				}
			else
				{
				pduSize -= unsentBytes;
				sentAttributes++;
				}
				attributeEncoder.WriteDesc(partPtr);
			}
		// what should be here ?
		}
// now send bytes up to what's left of pduSize
	TInt currentAttr = sentAttributes;
	TInt attrSize;
	while (pduSize > 0 && currentAttr < lastAttr)
		{
		if (currentAttr > lastAttr) User::Leave(KErrUnknown); // should go past the last attribute
		currentAttItem = collector->AttributeOf(sentRecords, currentAttr);
		if (pduSize < 3)
			{
			wBuffer[0] = KAttrIdHeader;
			responseAttributes.Append(&wBuffer[0], 1);
			pduSize--;
			if (pduSize == 0) break;
			}
		if (pduSize == 1)
			{
			wBuffer[0] = (TUint8)(currentAttItem->AttID() >> 8);
			responseAttributes.Append(&wBuffer[0], 1);
			pduSize = 0;
			break;
			}
		attributeEncoder.WriteUint(currentAttItem->AttID(), 2);
		pduSize -= KAttributeIdSize; // the attrID with its header
		if (pduSize == 0) break;
		attrSize = currentAttItem->Size();
		attrValPtr.Set(currentAttItem->Attr()->Value().Des());
		if (attrSize > pduSize)
			{
			TPtrC8 partPtr;
			partPtr.Set(attrValPtr.Left(pduSize));
			attributeEncoder.WriteDesc(partPtr);
			pduSize = 0;
			}
		else
			{
			attributeEncoder.WriteDesc(attrValPtr);
			pduSize -= attrSize;
			}
		currentAttr++;
		}
	oldLen = responseAttributes.Length();
	if (outContFlag)
		{
		responseAttributes.SetLength(oldLen + KSdpContinuationStateLength + KContStateHeader);
		responseAttributes[oldLen] = KSdpContinuationStateLength;
		BigEndian::Put32(&responseAttributes[oldLen + KContContOff + KContStateHeader], (sentBytes + writtenSize));
		BigEndian::Put32(&responseAttributes[oldLen + KContTotOff + KContStateHeader], fullSize);
		BigEndian::Put16(&responseAttributes[oldLen + KContCrcOff + KContStateHeader], localCRC);		// CRC not used
		}
	else
		{
		responseAttributes.SetLength(oldLen + KContStateHeader);
		responseAttributes[oldLen] = 0;
		}
	aRespPdu.iParams.SetLength(responseAttributes.Length());
	CleanupStack::PopAndDestroy(2 /*collector, pHandler*/);	
	}

void SdpReqHandler::HandleServiceSearchAttributeL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu)
/**
	Handle Service Attribute Search request.
	Request parameter format is
 @verbatim
		Service search pattern		DES
		Max Attribute byte count	TUint16
		Attribute ID/range list		DES
		Continuation state			1 + 0-16 bytes
 @endverbatim

	Response parameter format is
 @verbatim
		Total byte count			TUint16
		for each record matched		DES of
		Attribute ID & Value		DES
		Continuation State			1 + 0-16 bytes
 @endverbatim

**/
	{
// Parse the request parameters
	TPtr8 params(aReqPdu.iParams);
	TInt len = params.Length();
	TInt rem;
	TInt sentRecords=0;
	TInt sentAttributes=0;
	TInt maxTotalAttributeCount = 0;

	CSdpPDUHandler* pHandler = new (ELeave)	CSdpPDUHandler();
	CleanupStack::PushL(pHandler);
	CSizeAccumulator* collector = CSizeAccumulator::NewL();
	CleanupStack::PushL(collector);


	CSdpSearchPattern* pattern = pHandler->UUIDListLC(params, KRecHandleCountSize, len, rem, maxTotalAttributeCount);

// list of Attributes or ranges	
	TPtrC8 attrParams = params.Right(rem);
	CSdpAttrIdMatchList *attMatchList = pHandler->AttrListLC(attrParams, rem);
// check for the continuation header
	TBool inContFlag = pHandler->ContinuationL(params, len, rem);
// size the response
	CResponseSizeVisitor::SizeRespSAL(aDatabase, *pattern, *attMatchList, *collector);
	CleanupStack::PopAndDestroy(2 /*attMatchList, pattern*/);
	TInt totalHandles = collector->HandleCount();

// some checks on continuation
	TUint fullSize = collector->SizeLeft(); // we can always send all bytes the max is only per PDU
// the sizer includes the DES size, but we need the record size less the header
// NOTE in the SAS case we have a set (DES) of DES, but no DES header in front of them.
	TInt desSize = CSdpPDUHandler::DesSize(fullSize);// the DES of DES length is not calculated by SizeLeft
	fullSize += desSize;		// this avoids the case of zero attributes
	TInt16 localCRC = 0;
	TUint sentBytes;
	TUint sentBytesCurAttr;			// bytes on current attribute already sent
	if (inContFlag)
		{
		if (fullSize != pHandler->FullLength()) User::Leave(KErrUnknown);	// continuation check
		sentBytes = pHandler->ContinuationOffset(); // this is in bytes, not handles like service search
		if (fullSize <= sentBytes) User::Leave(KErrUnknown);	// continuation check

// FIXME removed CRC handling until continuation works
//		localCRC = collector->CrcAttribs();						// CRC is only for attributes
		if (localCRC != pHandler->ReqCRC()) User::Leave(KErrUnknown);				// continuation check for attributes

// collector->StartAt uses the bytes already sent to discover how far through an 
// attribute we are at the beginning of a new pdu in a continued response....   
// ....in SAS we need to remove the desSize from any sent bytes as the outer DES is not
// counted in StartAt.
// ....also NOTE: because further down we ensure that no new record DES or part thereof)
// is sent at the end of a pdu any non-zero value returned in sentBytesCurAttr will not be
// just that (bytes already sent of current attribute split in process of continuation)
		TBool adj = collector->StartAt(sentBytes - desSize, sentBytesCurAttr, sentRecords, sentAttributes); // this is for attributes
		if (!adj) User::Leave(KErrUnknown);
		}
	else
		{
		sentBytes = 0;
		sentBytesCurAttr = 0;
		}

/*
	working out if we can send all or some of the data:
	if there is a maxbytes outstanding, comply with that
	if we can send all the data (bytes) fine
	if we can't send all the data, reduce the buffer by the continuation
	and then take the minimum of the maxbytes or what is left of the buffer
*/
	TInt pduSize = fullSize - sentBytes;	// the data left to send

// start with the buffer size less: 
//		the byte count size and the empty continuation header
	TInt bufferUsableLen = aRespPdu.iParams.MaxLength() - (KRspAttributeCountSize + KContStateHeader);
	TInt bufferThisSize = Min(maxTotalAttributeCount, bufferUsableLen);

	TBool outContFlag;
	if (bufferThisSize < pduSize) 
		{
		outContFlag = ETrue;
		bufferUsableLen -= KSdpContinuationStateLength;	// we need the header now
		bufferThisSize = Min(maxTotalAttributeCount, bufferUsableLen); // again the smallest
//	when sending attributes (AR, SAS) make sure we don't leave a single byte
//	to be sent in the next continuation. Otherwise, reduce the payload this
//	time by 1 to make sure we comply with the specification and send a minimum
//	of two bytes in the response.
		if ((pduSize - bufferThisSize) == 1) bufferThisSize -= 1;
		if (!inContFlag)
			{
// FIXME CRC removed until continuation works
//			localCRC = collector->CrcAttribs();
			}
		}
	else
		{
		outContFlag = EFalse;
// we can complete this request this time
		}

	pduSize = Min(pduSize, bufferThisSize);

// end of common second stage continuation processing

	TInt writtenSize = pduSize;  // we will be reducing the pduSize

// Write the response packet
	aRespPdu.iPduId = EServiceSearchAttributeResponse;
	aRespPdu.iParams.SetMax();
	TPtr8 responseAttributes(0,0); //used to point along aRespPdu.iParams, and manage this
	responseAttributes.Set(&aRespPdu.iParams[0], 0, aRespPdu.iParams.MaxLength());
	//NB attributeEncoder below writes to responseAttributes (pointing at response PDU)...
	TElementEncoder attributeEncoder(responseAttributes);
	TInt oldLen = responseAttributes.Length();
	responseAttributes.SetLength(oldLen + KRspAttributeCountSize);
	BigEndian::Put16(&responseAttributes[KRspAttributeCountOffset], (TUint16)(pduSize));
	CAttrSizeItem* currentAttItem = collector->AttributeOf(sentRecords, sentAttributes);
	TInt lastAttr = collector->AttrCount(sentRecords);
	TPtrC8 attrValPtr(0,0);
	TBuf8<1> wBuffer(1); // used for byte insertion
	if (!inContFlag)
		{// write the outer DES
		attributeEncoder.WriteDES(fullSize - desSize);
		pduSize -= desSize;
		}
	else
		{ // we are writing a continuation so straight into attribute data
		if (sentBytesCurAttr)
			{ // we have to write the rest of a previous attribute
			TPtrC8 partPtr(0,0);
			attrValPtr.Set(currentAttItem->Attr()->Value().Des());
			TInt unsentBytes = attrValPtr.Length() + KAttributeIdSize - sentBytesCurAttr;
			switch(sentBytesCurAttr)
				{ // we may have to send some of the attribute ID
			// coverity[unterminated_case]
			case 1:
				wBuffer[0] = (TUint8)(currentAttItem->AttID() >> 8);
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize--;
				unsentBytes--;
			// coverity[fallthrough]
			case 2:
				wBuffer[0] = (TUint8)(currentAttItem->AttID() & 0xff);
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize--;
				unsentBytes--;
				partPtr.Set(attrValPtr); // it's a whole pointer
				break;
			default:
				partPtr.Set(attrValPtr.Right(unsentBytes));
				break;
				}
			if (unsentBytes - pduSize > 0)	
				{// we don't even get to send one complete attribute...
				partPtr.Set(partPtr.Left(pduSize)); // adjust the size of the des we send.
				pduSize = 0;
				}
			else
				{
				pduSize -= unsentBytes;
				sentAttributes++;
				}
			attributeEncoder.WriteDesc(partPtr);
			}
		// what should be here ?
		}
// now send bytes up to what's left of pduSize
	TInt currentAttr = sentAttributes;
	TInt currentRec = sentRecords;
	TInt attrSize;
	while (pduSize > 0 && currentRec < totalHandles)
		{ // for all handles
		if (currentAttr == 0)
			{ // write the DES header for this list of attributes
			TUint lRecordSize = collector->HandleSize(currentRec);
			TInt lDesSize = CSdpPDUHandler::DesSize(lRecordSize);
			if (pduSize <= lDesSize)
				{ // a new record DES: don't write it if it or part of it would
				  // come on the end of the data part of a pdu - leave it
				  // for the next pdu
				writtenSize -= pduSize;
				BigEndian::Put16(&responseAttributes[KRspAttributeCountOffset], (TUint16)(writtenSize));
				pduSize = 0;
				break;
				}
			if(lRecordSize)
				{//only if record has any attributes to return 
				 //(=> lastAttr will be > 0 when calculated below)
				attributeEncoder.WriteDES(lRecordSize);
				pduSize -= lDesSize;
				}
			}
		lastAttr = collector->AttrCount(currentRec);
		while (pduSize > 0 && currentAttr < lastAttr)
			{ // for all attributes
			currentAttItem = collector->AttributeOf(currentRec, currentAttr);
			//if clauses below put part of attribute id on the end of a pdu
			if (pduSize < 3)
				{
				wBuffer[0] = KAttrIdHeader;
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize--;
				if (pduSize == 0) break;
				}
			if (pduSize == 1)
				{
				wBuffer[0] = (TUint8)(currentAttItem->AttID() >> 8);
				responseAttributes.Append(&wBuffer[0], 1);
				pduSize = 0;
				break;
				}
			//if we've got here we can at least write a complete attr id onto the pdu
			attributeEncoder.WriteUint(currentAttItem->AttID(), 2);
			pduSize -= KAttributeIdSize; // the attrID with its header
			if (pduSize == 0) break;
			attrSize = currentAttItem->Size();
			attrValPtr.Set(currentAttItem->Attr()->Value().Des());
			if (attrSize > pduSize)
				{
				TPtrC8 partPtr;
				partPtr.Set(attrValPtr.Left(pduSize)); //from left pduSize bytes
				attributeEncoder.WriteDesc(partPtr);
				pduSize = 0;
				}
			else
				{
				attributeEncoder.WriteDesc(attrValPtr);
				pduSize -= attrSize;
				}
			currentAttr++;
			}
		if (pduSize == 0) break;
		currentRec++;
		currentAttr = 0;
		}
	oldLen = responseAttributes.Length();
	if (outContFlag)
		{
		responseAttributes.SetLength(oldLen + KSdpContinuationStateLength + KContStateHeader);
		responseAttributes[oldLen] = KSdpContinuationStateLength;
		BigEndian::Put32(&responseAttributes[oldLen + KContContOff + KContStateHeader], (sentBytes + writtenSize));
		BigEndian::Put32(&responseAttributes[oldLen + KContTotOff + KContStateHeader], fullSize);
		BigEndian::Put16(&responseAttributes[oldLen + KContCrcOff + KContStateHeader], localCRC);		// CRC not used
		}
	else
		{
		responseAttributes.SetLength(oldLen + KContStateHeader);
		responseAttributes[oldLen] = 0;
		}
	aRespPdu.iParams.SetLength(responseAttributes.Length());
	CleanupStack::PopAndDestroy(2 /*collector, pHandler*/);	
	}




