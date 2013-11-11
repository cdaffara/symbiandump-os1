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
// dataparser.cpp - all the sdp parser code
// 
//

#include <bluetooth/logger.h>
#include <btsdp.h>
#include "sdpconsts.h"
#include "sdputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

#ifdef _DEBUG
PANICCATEGORY("sdpdatap");
#endif

static const TInt KLinBufSizeLimit = 0x800;	// 2K

CElementParser::CLinearBuf* CElementParser::CLinearBuf::NewLC(TInt aExpandSize)
/** Element parser constructor. */
	{
	CLinearBuf* self = new(ELeave) CLinearBuf;
	CleanupStack::PushL(self);
	self->ConstructL(aExpandSize);
	return self;
	}

CElementParser::CLinearBuf::~CLinearBuf()
	{
	delete iBuf;
	}

void CElementParser::CLinearBuf::AppendL(const TDesC8& aData)
/** Add aData onto the end of this linear buffer.*/
	{
	if (iStartOffset && aData.Length() / 4 > Size())
		{// Might as well shuffle the buffer up now
		iBuf->Delete(0, iStartOffset);
		iStartOffset = 0;
		}
	iBuf->InsertL(iBuf->Size(), aData, aData.Length());
	}

const TPtrC8 CElementParser::CLinearBuf::Ptr() const
/** Get a constant Ptr to the contents of this buffer.*/
	{
	return iBuf->Ptr(iStartOffset);
	}

void CElementParser::CLinearBuf::Consume(TInt aLength)
/** Consume the first aLength bytes of the buffer.

	Optimised to avoid copies and heap operations on
	frequent calls to this function.*/
	{
	ASSERT_DEBUG(aLength <= Size());
	iStartOffset += aLength;
	if (iStartOffset > iBuf->Capacity() / 2 || Size() == 0)
		{// Actaully delete the stuff
		iBuf->Delete(0, iStartOffset);
		iStartOffset = 0;
		if (iBuf->Capacity() > KLinBufSizeLimit)
			{// Won't leave as we're reducing the reserve.
			TInt err;
			TRAP(err,iBuf->SetReserveL(Max(iBuf->Size(), KLinBufSizeLimit)));
			ASSERT_DEBUG(err == KErrNone);
			}
		}
	}

inline TInt CElementParser::CLinearBuf::Size() const
	{
	return iBuf->Size() - iStartOffset;
	}

CElementParser::CLinearBuf::CLinearBuf()
	{
	}

void CElementParser::CLinearBuf::ConstructL(TInt aExpandSize)
	{
	iBuf = CBufFlat::NewL(aExpandSize);
	}



CElementParser::CElementParser(MSdpElementBuilder* aBuilder)
	: iBuilder(aBuilder)
	{
	}

EXPORT_C CElementParser* CElementParser::NewL(MSdpElementBuilder* aBuilder)
/** Allocates and constructs a new CElementParser object.

@param aBuilder Object into which to build data element
@return New CElementParser object */
	{
	CElementParser* self = new(ELeave) CElementParser(aBuilder);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CElementParser::ConstructL()
	{
	this->iListStack = new(ELeave) CSdpStackFix<TInt>;
	}

EXPORT_C CElementParser::~CElementParser()
/** Destructor. */
	{
	delete iListStack;
	}


TBool CElementParser::IsComposite(TUint8 aType)
	{
	return aType == ETypeDES || aType == ETypeDEA;
	}

EXPORT_C TInt CElementParser::ParseElementsL(const TDesC8& aData)
/** Parses a data buffer into the currently set MSdpElementBuilder interface.

If a whole number of data elements can not be parsed out of aData, the number 
of bytes left unparsed is returned. Those bytes should be prepended to the 
buffer when the function is next called. 

@param aData Buffer to parse
@return Number of bytes not consumed */
	{
	TUint8* ptr = (TUint8*)aData.Ptr();
	const TUint8* ptrE = ptr + aData.Length();

	CloseListsL();
	while ((ptr < ptrE) && (iBuilder != NULL))
		{

		// Remember start of this data element
		TUint8* ptrStart = ptr;

		// We know there's at least one byte already..
		TUint8 type = (TUint8)(*ptr>>KSdpElemHdrTypeShift);
		TUint8 sizedesc  = (TUint8)(*ptr&KSdpElemHdrSizeMask);
		++ptr;

		if ( type >= sizeof(KSdpElementValidSizes) ||
			 !(KSdpElementValidSizes[type]&(1<<sizedesc)) )
			{// Invalid size desc for type.
			LOG2(_L("SDP: Invalid size descriptor %d for type %d!"), sizedesc, type);
			User::Leave(KErrSdpParserInvalidSizeForType);
			}

		TUint elementLen;
		if (type == ETypeNil)
			{// we've already checked that sizedesc == 0
			elementLen = 0;
			}
		else if(sizedesc<=ESizeSixteenBytes)
			{
			elementLen = 1<<sizedesc;
			}
		else
			{
			TUint addSzBytes = 1<<(sizedesc-ESizeOneAdditional);
			if((addSzBytes)>TUint(ptrE-ptr))
				return (ptrE-ptrStart);
			elementLen = SdpUtil::GetUint(TPtrC8(ptr, addSzBytes));
			ptr += addSzBytes;
			}

		if (!iListStack->IsEmpty() && 
			(TInt)((ptr-ptrStart)+elementLen) > iListStack->Head())
			{// Error! This item is longer than the list it is in!!
			User::Leave(KErrSdpParserInvalidSizeForParentList);
			}

		TUint compositeLen = 0;
		if (IsComposite(type))
			{//Composite node. We'll parse down into it.
			compositeLen = elementLen;
			elementLen = 0;		// composites have no element length.
			}

		if((elementLen)>TUint(ptrE-ptr))return (ptrE-ptrStart);

		TPtrC8 data(ptr,elementLen);

		switch (type)
			{
		case ETypeNil:
			iBuilder=iBuilder->BuildNilL();
			break;
		case ETypeUint:
			iBuilder=iBuilder->BuildUintL(data);
			break;
		case ETypeInt:
			iBuilder=iBuilder->BuildIntL(data);
			break;
		case ETypeUUID:
			{
			TUUID uuid;
			uuid.SetL(data);
			iBuilder=iBuilder->BuildUUIDL(uuid);
			}
			break;
		case ETypeString:
			iBuilder=iBuilder->BuildStringL(data);
			break;
		case ETypeBoolean:
			iBuilder=iBuilder->BuildBooleanL(SdpUtil::GetUint(data));
			break;		
		case ETypeURL:
			iBuilder=iBuilder->BuildURLL(data);
			break;
		case ETypeDES:
			iBuilder=iBuilder->BuildDESL();
			break;
		case ETypeDEA:
			iBuilder=iBuilder->BuildDEAL();
			break;
		default:
			iBuilder=iBuilder->BuildUnknownL(type, sizedesc, data);
			break;
			};

		ptr += elementLen;


		if (!iListStack->IsEmpty())
			{// Adjust size of existing head entry so we know when the DES/DEA is finished
			iListStack->Head() -= (ptr-ptrStart)+compositeLen;
			}

		if (IsComposite(type))
			{ // push the length of the DES/DEA on top of the stack and mark the list
			iBuilder=iBuilder->StartListL();
			iListStack->PushL(compositeLen);
			}

		CloseListsL();
		}

	__ASSERT_DEBUG(ptr<=ptrE, ParsePanic(EFrameOverrun));

	return ptrE-ptr;
	}

EXPORT_C TBool CElementParser::BufferedParseL(const TDesC8& aData)
/** Parse out some encoded data, with transparent buffering of unparsed data between 
calls.

The function stores any incompletely parsed data from a previous call, and 
automatically prepends to the data buffer when it is next called.

If the function leaves, the unparsed data will be deleted. This means it will 
not be possible to continue with this data stream; Reset() should be called 
before the next call to this function.

@param aData Buffer to parse
@return True if a whole number of data elements have not been parsed, so more 
data is expected. False if a whole number has been parsed. */
	{
	TPtrC8 data(aData);
	CLinearBuf* remBuf = 0;

	if(iRemainderBuf)
		{//Need to append new bit onto remainder from previous response
		remBuf = iRemainderBuf;
		iRemainderBuf = 0;
		CleanupStack::PushL(remBuf);
		remBuf->AppendL(data);
		data.Set(remBuf->Ptr());
		}

	TInt rem = ParseElementsL(data);

	if(remBuf || rem > 0)
		{
		if(remBuf && rem > 0)
			{// skip the bit just parsed
			remBuf->Consume(remBuf->Size() - rem);
			}
		else if (remBuf) // && rem == 0
			{// no remainder -- remBuf no longer needed
			CleanupStack::PopAndDestroy(); // remBuf
			remBuf = 0;
			}
		else // rem > 0 && remBuf == 0
			{// Store remainder in a new remainder buf
			remBuf = CLinearBuf::NewLC(rem);
			remBuf->AppendL(data.Right(rem));
			}

		if(remBuf)
			{
			CleanupStack::Pop(); // remBuf
			iRemainderBuf = remBuf;
			}
		}
	ASSERT_DEBUG(rem == 0 || rem == ((iRemainderBuf != NULL) ? iRemainderBuf->Size() : 0));
	return iRemainderBuf || !iListStack->IsEmpty();
	}

EXPORT_C void CElementParser::Reset()
/** Resets the parser's buffers to be empty. */
	{
	iListStack->Reset();
	delete iRemainderBuf;
	iRemainderBuf = 0;
	}

EXPORT_C void CElementParser::Reset(MSdpElementBuilder* aBuilder)
/** Resets the parser's buffers to be empty, and resets the element builder object 
used.

@param aBuilder Element builder object to use */
	{
	Reset();
	iBuilder=aBuilder;
	}

void CElementParser::CloseListsL()
	{
	while (!iListStack->IsEmpty() && iListStack->Head() == 0 && iBuilder)
		{// List has ended
		iListStack->Pop();
		iBuilder=iBuilder->EndListL();
		}
	}

EXPORT_C void CElementParser::SetBuilder(MSdpElementBuilder* aBuilder)
	{
	iBuilder = aBuilder;
	}

EXPORT_C MSdpElementBuilder* CElementParser::Builder()
	{
	return iBuilder;
	}
	
// EOF
