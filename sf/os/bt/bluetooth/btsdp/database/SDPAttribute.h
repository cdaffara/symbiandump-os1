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


#ifndef SDPATTRIBUTE_H
#define SDPATTRIBUTE_H

#include <btsdp.h>
#include "sdputil.h"

class MAttributeVisitor;

class CSdpAttr;

typedef TOrderedQue<CSdpAttr> TServAttrList;
typedef TDblQueIter<CSdpAttr> TServAttrIter;

/**
	SDP Service Attribute.
	Contains the Attribute ID and value pair.
**/
NONSHARABLE_CLASS(CSdpAttr) : public CBase, public MSdpElementBuilder
	{
public:
	IMPORT_C static CSdpAttr* NewL(TUint16 aAttrID,MSdpElementBuilder* aParent);
	~CSdpAttr();
	inline TSdpAttributeID AttributeID() const;
	inline const CSdpAttrValue& Value() const;
	inline CSdpAttrValue& Value();
	inline CSdpAttrValue* ReleaseValue();
	inline TBool operator>(const CSdpAttr& aAttr) const;
	inline static TInt LinkOffset();

	IMPORT_C void AcceptVisitorL(MAttributeVisitor& aVisitor);

	// MSdpElementBuilder interface
	virtual MSdpElementBuilder* BuildNilL();
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	virtual MSdpElementBuilder* BuildDESL();
	virtual MSdpElementBuilder* BuildDEAL();
	virtual MSdpElementBuilder* BuildURLL(const TDesC8& aString);
	virtual MSdpElementBuilder* BuildEncodedL(const TDesC8& aString);
private:
	CSdpAttr(TUint16 aAttrID,MSdpElementBuilder* aParent);
	void ConstructL();
	void Reset();

private:
	MSdpElementBuilder* iParent;
	CSdpAttrValue* iAttrValue;
    TSdpAttributeID iAttrID;
	TDblQueLink iLink;
	};

inline TSdpAttributeID CSdpAttr::AttributeID() const
	{
	return iAttrID;
	}

inline const CSdpAttrValue& CSdpAttr::Value() const
	{
	return const_cast<CSdpAttr*>(this)->Value();
	}


inline CSdpAttrValue& CSdpAttr::Value()
	{
	__ASSERT_DEBUG(iAttrValue, User::Panic(_L("Sdp Database"), ESdpNoAttrValue));
	return *iAttrValue;
	}

inline CSdpAttrValue* CSdpAttr::ReleaseValue()
	{
	CSdpAttrValue* p = iAttrValue;
	iAttrValue = 0;
	return p;
	}

inline TBool CSdpAttr::operator>(const CSdpAttr& aAttr) const
	{
	return iAttrID > aAttr.iAttrID;
	}

inline TInt CSdpAttr::LinkOffset()
	{
	return _FOFF(CSdpAttr, iLink);
	}

#endif
