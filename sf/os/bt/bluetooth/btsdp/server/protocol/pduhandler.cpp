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

#include <btsdp.h>
#include "reqhandler.h"
#include "pduhandler.h"
#include "DataEncoder.h"
#include "sdpconsts.h"


CSdpSearchPattern* CSdpPDUHandler::UUIDListLC(const TDesC8& aPdu, const TInt aNextSize, TInt& aLen, TInt& aRem, TInt& aCount)
/**
	Unpack UUID list from request PDU and extracts a 16 bit value following.
	Parameter format is
 @verbatim
		Search UUID list descriptor			DesC
		Next chunk size						TInt (but always 2)
		Descriptor length returned			TInt
		Remaining length returned			TInt
		Next 2 bytes are returned as		TInt
 @endverbatim

	Method will leave if there are parsing problems, or the remaining 
	PDU is smaller than next size.
	Returns :
@verbatim
		UUID list							CSdpSearchPattern object
		total byte length					TInt
		number of bytes unparsed			TInt
		the 16 bit value extracted			TInt
@endverbatim

**/
	{
	CSdpSearchPattern* pattern = CSdpSearchPattern::NewL();
	CleanupStack::PushL(pattern);

	CElementParser* parser = CElementParser::NewL(pattern);
	CleanupStack::PushL(parser);

	aLen = aPdu.Length();
	aRem = parser->ParseElementsL(aPdu);
	CleanupStack::PopAndDestroy(/*parser*/);

	if (aRem < aNextSize)
		{
		User::Leave(KErrArgument);
		}
	aCount = BigEndian::Get16(&aPdu[aLen-aRem]);
	aRem -= aNextSize;
	return pattern;
	}



 CSdpAttrIdMatchList* CSdpPDUHandler::AttrListLC(const TDesC8& aPdu, TInt& aRem)
/**
	Unpack attribute/range list from request PDU.
	Parameter format is
 @verbatim
		Search attrib list descriptor		DesC
		Remaining length returned			TInt
 @endverbatim

	Method will leave if the remaining PDU is smaller than next size.
	Returns :
 @verbatim
		the attribute list					CSdpAttrIdMatchList object
		number of bytes unparsed			TInt.
 @endverbatim


**/
 {
	CSdpAttrIdMatchList* attributeMatchList = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(attributeMatchList);

	CElementParser* parser = CElementParser::NewL(attributeMatchList);
	CleanupStack::PushL(parser);

	aRem = parser->ParseElementsL(aPdu);
	CleanupStack::PopAndDestroy(/*parser*/);
	return attributeMatchList;

 }


TBool CSdpPDUHandler::ContinuationL(const TDesC8& aPdu, const TInt aLen, const TInt aRem)
/**
	Check the continuation. This code will change
 @verbatim
		Current request descriptor		DesC
		Current request length			TInt
		Unparsed length					TInt
 @endverbatim

	Method will leave if the remaining PDU is smaller than any continuation
	Returns :
 @verbatim
		dummy continuation length			TInt.

 @endverbatim
**/
{
	if (aRem < KContStateHeader || 
		aPdu[aLen-aRem] > KSdpContinuationStateLength ||
		aPdu[aLen-aRem] + KContStateHeader != aRem)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 contState = aPdu.Right(aRem-KContStateHeader);
	if(contState.Length() != 0)
		{
		if(contState.Length() != KSdpContinuationStateLength)
			{
			User::Leave(KErrUnknown); // Causes "Bad cont State" error
			}
		iContinuation = BigEndian::Get32(&contState[KContContOff]);
		iLength = BigEndian::Get32(&contState[KContTotOff]);
		iCRC = BigEndian::Get16(&contState[KContCrcOff]);
		if (iLength < iContinuation)
			{
			User::Leave(KErrUnknown); // Causes "Bad cont State" error
			}
		return ETrue;	// we have a continuation situation
		}
	return 0;
}


void CSdpPDUHandler::CompleteOPL(TDes8& aPdu, const TDesC8& aWritePdu, const TInt aMaxAttrCount)
/**
	Verifies the size of the response parameters
	and writes out correct length for the attributes.
 @verbatim
		response descriptor			DesC
		written area (attributes)	DesC
		maximum byte count			TInt
 @endverbatim

	Method will leave if response is bigger than requested or allowed for.
	Returns nothing

**/
{
	TUint16 finalLength = (TUint16)aWritePdu.Length();
	if (finalLength > aMaxAttrCount) User::Leave(KErrNoMemory);
	aPdu.SetLength(KRspAttributeCountSize + finalLength + KContStateHeader);
	BigEndian::Put16(&aPdu[KRspAttrCountOffset], finalLength);
// now need to update the DES size
	if (iDesSize == 3)
	{
		BigEndian::Put16(&aPdu[KRspAttributeListOffset+1], (unsigned short)(finalLength-iDesSize));
	}
	else if (iDesSize == 2)
	{
		if (finalLength > 0xff) User::Leave(KErrNoMemory);
		aPdu[KRspAttributeListOffset+1] = (unsigned char)((finalLength&0xff)-iDesSize);
	}
	else User::Leave(KErrUnknown);   // perhaps this should be a panic
	aPdu[aPdu.Length()-1] = 0; // FIXME: Put contState here
}

TUint32 CSdpPDUHandler::ContinuationOffset()
	{
	return iContinuation;
	}

TUint32 CSdpPDUHandler::FullLength()
	{
	return iLength;
	}

TUint16 CSdpPDUHandler::ReqCRC()
	{
	return iCRC;
	}

// specific test for DES headers only
TInt CSdpPDUHandler::DesSize(TUint aDataSize)
	{
	TInt lDesSize;
	lDesSize = (aDataSize > KMaxTwoByteDESSize) ? KThreeByteDESSize : KTwoByteDESSize;
	lDesSize = (aDataSize > KMaxThreeByteDESSize) ? KFiveByteDESSize : lDesSize;
	return lDesSize;
	}

