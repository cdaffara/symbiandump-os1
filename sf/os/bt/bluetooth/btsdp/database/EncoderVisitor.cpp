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

/**
 @internalTechnology
 @released
*/

#include "EncoderVisitor.h"
#include <btsdp.h>
#include "DataEncoder.h"
#include "SDPDatabase.h"
#include "sdputil.h"

/** Creates a new CAttrEncoderVisitor and leaves it on the cleanup stack.

@param aEncoder a TElementEncoder to be used by this CAttrEncoderVisitor.
@return A new CAttrEncoderVisitor.
*/
EXPORT_C CAttrEncoderVisitor *CAttrEncoderVisitor::NewLC(TElementEncoder aEncoder)
	{
	CAttrEncoderVisitor *self = new(ELeave) CAttrEncoderVisitor(aEncoder);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/** Creates a new CAttrEncoderVisitor.

@param aEncoder a TElementEncoder to be used by this CAttrEncoderVisitor.
@return A new CAttrEncoderVisitor.
*/
EXPORT_C CAttrEncoderVisitor *CAttrEncoderVisitor::NewL(TElementEncoder aEncoder)
	{
	CAttrEncoderVisitor *self = CAttrEncoderVisitor::NewLC(aEncoder);
	CleanupStack::Pop();
	return self;
	}

CAttrEncoderVisitor::CAttrEncoderVisitor(TElementEncoder aEncoder)
	: iEncoder(aEncoder)
	{
	}

void CAttrEncoderVisitor::ConstructL()
	{
	}

CAttrEncoderVisitor::~CAttrEncoderVisitor()
	{
	}

/**
Encode attribute in aSrcAttr into buffer supplied in the encoder.

@param aEncoder A TElementEncoder with a buffer large enough to contain the 
				encoded value.
@leave ESdpUnexpectedAttributeValueType If the attribute is of an unknown type.
@leave Positive value equal to the number of bytes of the encoded value which 
	   did not fit in the write buffer of the TElementEncoder which is provided 
	   as aEncoder.  To calculate the size of write buffer needed  use the 
	   TElementEncoder::EncodedSize function.
@see TElementEncoder::EncodedSize
*/
EXPORT_C void CAttrEncoderVisitor::EncodeAttributeL(TElementEncoder aEncoder, CSdpAttrValue &aSrcAttr)
	{
	CAttrEncoderVisitor *self = CAttrEncoderVisitor::NewLC(aEncoder);
	aSrcAttr.AcceptVisitorL(*self);
	CleanupStack::PopAndDestroy();
	}

/**
Encode attribute in aSrcAttr into buffer supplied at construction.

@leave ESdpUnexpectedAttributeValueType If the attribute is of an unknown type.
@leave Positive value equal to the number of bytes of the encoded value which 
	   did not fit in the write buffer of the TElementEncoder.  This was provided 
	   at construction.  To calculate the size of write buffer needed  use the 
	   TElementEncoder::EncodedSize function.
@see TElementEncoder::EncodedSize
*/
EXPORT_C void CAttrEncoderVisitor::EncodeAttributeL(CSdpAttrValue &aSrcAttr)
	{
	aSrcAttr.AcceptVisitorL(*this);
	}


void CAttrEncoderVisitor::VisitAttributeL(CSdpAttr &aAttribute)
	{
	iEncoder.WriteUint(aAttribute.AttributeID(), sizeof(TSdpAttributeID));
	}

void CAttrEncoderVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	TUint rem=0;
	switch(aType)
		{
	case ETypeNil:
		rem = iEncoder.WriteNil();
		break;
	case ETypeUint:
	case ETypeInt:
	case ETypeUUID:
	case ETypeString:
	case ETypeURL:
		rem = iEncoder.WriteElement(aType, aValue.Des());
		break;
	case ETypeBoolean:
		rem = iEncoder.WriteBoolean(aValue.Bool());
		break;
	case ETypeDES:
		rem = iEncoder.WriteDES(aValue.DataSize());
		break;
	case ETypeDEA:
		rem = iEncoder.WriteDEA(aValue.DataSize());
		break;
	case ETypeEncoded:
		rem= iEncoder.WriteDesc(aValue.Des());
		break;
	default:
		User::Leave(ESdpUnexpectedAttributeValueType);
		}


	if (rem)
		User::Leave(rem);
	}

void CAttrEncoderVisitor::StartListL(CSdpAttrValueList &/*aList*/)
	{
	}

void CAttrEncoderVisitor::EndListL()
	{
	}




