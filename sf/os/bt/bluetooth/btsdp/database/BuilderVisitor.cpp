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


#include <es_sock.h>
#include <btsdp.h>
#include "BuilderVisitor.h"
#include "attrvalueencoded.h"
#include "SDPDatabase.h"
#include "sdputil.h"


EXPORT_C CBuilderAdapterVisitor::CBuilderAdapterVisitor(MSdpElementBuilder* aBuilder)
: iBuilder(aBuilder)
	{
	}

void CBuilderAdapterVisitor::VisitAttributeL(CSdpAttr &aAttribute)
	{
	if(!iBuilder)
		return;
	TBuf8<2> attrId(2);
	BigEndian::Put16(&attrId[0], aAttribute.AttributeID());
	iBuilder = iBuilder->BuildUintL(attrId);
	}

void CBuilderAdapterVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	if(!iBuilder)
		return;
	switch(aType)
		{
	case ETypeNil:
		iBuilder = iBuilder->BuildNilL();
		break;
	case ETypeUint:
		iBuilder = iBuilder->BuildUintL(aValue.Des());
		break;
	case ETypeInt:
		iBuilder = iBuilder->BuildIntL(aValue.Des());
		break;
	case ETypeUUID:
		iBuilder = iBuilder->BuildUUIDL(aValue.UUID());
		break;
	case ETypeString:
		iBuilder = iBuilder->BuildStringL(aValue.Des());
		break;
	case ETypeEncoded:
		{
		CElementParser* parser = CElementParser::NewL(iBuilder);
		CleanupStack::PushL(parser);
		parser->ParseElementsL(aValue.Des());
		iBuilder = parser->Builder();
		CleanupStack::PopAndDestroy();// parser
		}
		break;
	case ETypeURL:
		iBuilder = iBuilder->BuildURLL(aValue.Des());
		break;
	case ETypeBoolean:
		iBuilder = iBuilder->BuildBooleanL(aValue.Uint());
		break;
	case ETypeDES:
		iBuilder = iBuilder->BuildDESL();
		break;
	case ETypeDEA:
		iBuilder = iBuilder->BuildDEAL();
		break;
	default:
		User::Leave(ESdpUnexpectedAttributeValueType);
		}
	}

void CBuilderAdapterVisitor::StartListL(CSdpAttrValueList& /*aList*/)
	{
	if(!iBuilder)
		return;
	iBuilder = iBuilder->StartListL();
	}

void CBuilderAdapterVisitor::EndListL()
	{
	if(!iBuilder)
		return;
	iBuilder = iBuilder->EndListL();
	}




