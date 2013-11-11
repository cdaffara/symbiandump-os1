/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Implements the Enhanced SMS Animation Information Element.
*
*/


/**
 *  @file
 *  
 *  Implements  EMSInformationElement class
 */

#include <emsinformationelement.h>
#include <emsformatie.h>
#include <emssoundie.h>
#include <emsanimationie.h>
#include <emspictureie.h>
#include <emsuserpromptie.h>
#include <emsobjectdistributionie.h>


EXPORT_C void CEmsInformationElement::SetStartPosition(TUint aValue)
/**
 *  @capability None
 */
	{
	iStartPosition = aValue;
	}

EXPORT_C TUint CEmsInformationElement::StartPosition() const
/**
 *  @capability None
 */
	{
	return iStartPosition;
	}

EXPORT_C TInt CEmsInformationElement::Length() const
/**
 *  @capability None
 */
	{
	// The length includes the IE header (IEID, IEL) and the
	// start position, if applicable, plus the encoded body length
	TInt len = 2 + iEncodedBodyLength;
	if (iStartPosEncoded)
		{
		++len;
		}

	return len;
	}


/**
 *  Encodes the EmsInformationElement into a CSmsInformationElement
 *  
 *  @param aSmsIe SMS information element to encode into
 */
EXPORT_C void CEmsInformationElement::EncodeInformationElementL()
/**
 *  @capability None
 */
	{
	TInt ieLength = iEncodedBodyLength;

	// if start position is to be encoded, then increment this length
	if (iStartPosEncoded)
		{
		++ieLength;
		}

	// Create a new body for the IE
	NewDataL(ieLength);

	// Get the raw data to be encoded
	TPtr8 ptr(Data());
	ptr.SetLength(0);

	// Encode start position if required
	if (iStartPosEncoded)
		{
		ptr.Append(static_cast<TUint8>(iStartPosition));
		}

	// Call the specific encode body function - false indicates NOT
	// for serialisation
	EncodeBodyL(ptr, EFalse);
	}

CEmsInformationElement* CEmsInformationElement::DecodeInformationElementL(const CSmsInformationElement& aSmsIe)
	{
	// Firstly, create the element given the ID
	CEmsInformationElement* elem = CEmsFactory::CreateIeL(aSmsIe.Identifier());

	if (!elem)
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PushL(elem);

	// now decode the rest of the element
	const TUint8* ptr;
	const TDesC8& des = aSmsIe.Data();
	TInt length = des.Length();

	if (length <= 0)
		User::Leave(KErrCorrupt);

	// If the start position is encoded, then decode it, and add back
	// in the start position offset
	if (elem->iStartPosEncoded)
		{
		elem->iStartPosition = des[0];

		// set the pointer to point to the first element, and
		// reduce the length by 1;
		ptr = &des[1];
		--length;
		}

	else
		{
		// otherwise, set it to the first element
		ptr = &des[0];
		}

	// set the encoded body length to be what's left over
	elem->iEncodedBodyLength = length;

	// Now call the specific decode body function.
	elem->DecodeBodyL(TPtrC8(ptr, length), EFalse);

	CleanupStack::Pop(elem);
	return elem;
	}

void CEmsInformationElement::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(TInt16(iIdentifier));
	aStream.WriteUint16L(iStartPosition);
	aStream.WriteInt16L(iEncodedBodyLength);

	// Now create a buffer to hold the serialised body, and encode
	HBufC8* buf = HBufC8::NewLC(SerialisedBodyLength());
	EncodeBodyL(buf->Des(), ETrue);
	aStream << *buf;
	CleanupStack::PopAndDestroy(buf);
	}

CEmsInformationElement* CEmsInformationElement::InternalizeL(RReadStream& aStream)
	{
	// Get the id and create the appropriate
	TSmsId id;
	id = TSmsId(aStream.ReadInt16L());
	CEmsInformationElement* elem = CEmsFactory::CreateIeL(id);

	if (!elem)
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PushL(elem);

	// now stream into the new element
	elem->iStartPosition = aStream.ReadUint16L();
	elem->iEncodedBodyLength = aStream.ReadInt16L();

	// Create the buffer to hold the body data
	HBufC8* buf = HBufC8::NewLC(aStream, elem->SerialisedBodyLength());
	elem->DecodeBodyL(buf->Des(), ETrue);

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::Pop(elem);

	return elem;
	}

void CEmsInformationElement::CopyL(const CEmsInformationElement& aSrc)
	{
	iIdentifier = aSrc.iIdentifier;
	iStartPosition = aSrc.iStartPosition;
	iEncodedBodyLength = aSrc.iEncodedBodyLength;
	}

TInt CEmsInformationElement::SerialisedBodyLength() const
	{
	// default behaviour for this is to return the encoded body length.
	// This will be overriden by elements which differ (ie FormatIE)
	return iEncodedBodyLength;
	}

/**
 *  Internalize all Ems Objects from a given stream. This is the flipside of the factorys
 *  externalise method.
 *  
 *  @param aStream stream to restore from.
 *  @return CEmsInformationElement* newly created EMS object.
 */

/**
 *  Creates an information element from the data given
 *  
 *  @param aId EMS object id.
 *  @param aData Information Elements raw data.
 *  @return CEmsInformationElement* newly created EMS object.
 */
CEmsInformationElement* CEmsFactory::CreateIeL(TSmsId aSmsId)
	{
	CEmsInformationElement* pObj=NULL;
	switch (aSmsId)
		{
		case CSmsInformationElement::ESmsEnhancedTextFormatting:
			pObj = CEmsFormatIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedPredefinedSound:
			pObj = CEmsPreDefSoundIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
			pObj = CEmsSoundIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
			pObj = CEmsPreDefAnimationIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedLargeAnimation:
		case CSmsInformationElement::ESmsEnhancedSmallAnimation:
			pObj = CEmsAnimationIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedLargePicture:
		case CSmsInformationElement::ESmsEnhancedSmallPicture:
		case CSmsInformationElement::ESmsEnhancedVariablePicture:
			pObj = CEmsPictureIE::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
			pObj = CEmsUserPrompt::NewL();
			break;
		case CSmsInformationElement::ESmsEnhancedODI:
			pObj = CEmsObjectDistribution::NewL();
			break;
		default:
			break;
		}
	if(pObj)
		{
		// set the id of the newly created EMS information element
		pObj->iIdentifier = aSmsId;
		}
	return pObj;
	}

/**
 *  Externalises all the objects in a given object array. Adds extra information into externalised data
 *  so that objects can be readback generically (done this was as All streams do not support seek operatoions).
 *  This is the flipside of the Factorys internalise method.
 *  
 *  @param aObjectStore Array of objects to externalize.
 *  @param aStream stream to externalize object to.
 */
void CEmsFactory::ExternalizeL(RPointerArray<CEmsInformationElement>& aObjectStore, RWriteStream& aStream)
	{
	TUint count= aObjectStore.Count();
	aStream.WriteUint32L(count);
	for (TUint i=0; i< count; i++)
		{
		aObjectStore[i]->ExternalizeL(aStream);
		}
	}

void CEmsFactory::InternalizeL(RPointerArray<CEmsInformationElement>& aObjectStore, RReadStream& aStream)
	{
	// Retain store compat. with other Hur. 7 messaging stores.
	TInt count=0;
	TRAPD(test, count = aStream.ReadUint32L());
	if(test == KErrEof)
		test=KErrNone;
	User::LeaveIfError(test);

	CEmsInformationElement* ie = NULL;

	while(count--)
		{
		TRAPD(ret, (ie = CEmsInformationElement::InternalizeL(aStream)));
		if(ret==KErrNone)
			{
			CleanupStack::PushL(ie);
			User::LeaveIfError(aObjectStore.Append(ie));
			CleanupStack::Pop(ie);
			}
		else if(ret!=KErrCorrupt)
			User::Leave(ret);
		}
	}

CEmsInformationElement* CEmsFactory::CreateReceivedEmsIEL(const CSmsInformationElement& aIE,TInt aBaseAddr)
	{
	CEmsInformationElement* newIE =
		CEmsInformationElement::DecodeInformationElementL(aIE);

	newIE->iStartPosition += aBaseAddr;

	return newIE;
	}


TBool CEmsFactory::Supported(TSmsId aId)
	{
	TBool ret=ETrue;
	switch (aId)
		{
	case CSmsInformationElement::ESmsEnhancedTextFormatting:
	case CSmsInformationElement::ESmsEnhancedPredefinedSound:
	case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
	case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
	case CSmsInformationElement::ESmsEnhancedLargeAnimation:
	case CSmsInformationElement::ESmsEnhancedSmallAnimation:
	case CSmsInformationElement::ESmsEnhancedLargePicture:
	case CSmsInformationElement::ESmsEnhancedSmallPicture:
	case CSmsInformationElement::ESmsEnhancedVariablePicture:
	case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
	case CSmsInformationElement::ESmsEnhancedODI:
		ret=ETrue;
		break;
	default:
		ret=EFalse;
		}
	return ret;
	}
