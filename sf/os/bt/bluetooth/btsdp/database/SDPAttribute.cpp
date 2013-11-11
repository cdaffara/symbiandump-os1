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

#include "SDPAttribute.h"
#include "MAttributeVisitor.h"
#include "attrvalueencoded.h"

// Class CServArrr


// ******************* CSdpAttrValue ********************************

EXPORT_C CSdpAttr *CSdpAttr::NewL(TUint16 aAttrID,MSdpElementBuilder *aParent)
	{
	CSdpAttr *attr=new (ELeave) CSdpAttr(aAttrID,aParent);
	CleanupStack::PushL(attr);
	attr->ConstructL();
	CleanupStack::Pop();
	return attr;
	}

void CSdpAttr::ConstructL()
	{
	}

CSdpAttr::CSdpAttr(TUint16 aAttrID,MSdpElementBuilder *aParent)
	: iParent(aParent),
	iAttrID(aAttrID)
	{
	}

CSdpAttr::~CSdpAttr()
	{
	delete iAttrValue;
	iLink.Deque();
	}

EXPORT_C void CSdpAttr::AcceptVisitorL(MAttributeVisitor& aVisitor)
	{
	aVisitor.VisitAttributeL(*this);
	if (iAttrValue)
		{
		iAttrValue->AcceptVisitorL(aVisitor);		
		}
	//else //do nothing
	}


void CSdpAttr::Reset()
	{
	delete iAttrValue;
	iAttrValue = 0;
	}

/*
EXPORT_C CSdpAttrValue& CSdpAttr::Value()
	{
	__ASSERT_DEBUG(iAttrValue, DbPanic(ESdpNoAttrValue));
	return *iAttrValue;
	}
*/

// FIXME: Need to delete and reset iAttrValue on all the following....

MSdpElementBuilder *CSdpAttr::BuildNilL()
	{
	Reset();
	iAttrValue=CSdpAttrValueNil::NewNilL();
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildUintL(const TDesC8& aUint)
	{
	Reset();
	iAttrValue=CSdpAttrValueUint::NewUintL(aUint);
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildIntL(const TDesC8& aInt)
	{
	Reset();
	iAttrValue=CSdpAttrValueInt::NewIntL(aInt);
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildUUIDL(const TUUID& aUUID)
	{
	Reset();
	iAttrValue=CSdpAttrValueUUID::NewUUIDL(aUUID);
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildStringL(const TDesC8& aString)
	{
	Reset();
	iAttrValue=CSdpAttrValueString::NewStringL(aString);
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildBooleanL(TBool aBool)
	{
	Reset();
	iAttrValue=CSdpAttrValueBoolean::NewBoolL(aBool);
	return iParent;
	}

MSdpElementBuilder* CSdpAttr::BuildDESL()
/**
	CSdpAttr Build DES.
	This function creates a new CSdpAttrValue of type DES and returns 
	a pointer to it as the new parent MSdpElementBuilder
**/
	{
	Reset();
	CSdpAttrValueList *list=CSdpAttrValueDES::NewDESL(iParent);
	iAttrValue=list;
	return list;
	}

MSdpElementBuilder* CSdpAttr::BuildDEAL()
/**
	CSdpAttr Build DEA.
	This function creates a new CSdpAttrValue of type DEA and returns 
	a pointer to it as the new parent MSdpElementBuilder
**/
	{
	Reset(); //FIXME .. added because I do not understand why it was not here!
	CSdpAttrValueList *list=CSdpAttrValueDEA::NewDEAL(iParent);
	iAttrValue=list;
	return list;
	}

MSdpElementBuilder *CSdpAttr::BuildURLL(const TDesC8& aURL)
	{
	Reset(); //FIXME .. added because I do not understand why it was not here!
	iAttrValue=CSdpAttrValueURL::NewURLL(aURL);
	return iParent;
	}

MSdpElementBuilder *CSdpAttr::BuildEncodedL(const TDesC8& aString)
	{
	Reset();
	iAttrValue=CSdpAttrValueEncoded::NewEncodedL(aString);
	return iParent;
	}
