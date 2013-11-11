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
// cservicesearchvisitor.cpp
// 
//

#include <btsdp.h>
#include "SDPDatabase.h"
#include "ServiceSearchVisitor.h"
#include "mignorer.h"

EXPORT_C CSdpSearchPattern* CSdpSearchPattern::NewL()
/** Allocates and constructs a new CSdpSearchPattern object.

@return New CSdpSearchPattern object */
	{
	CSdpSearchPattern* self = new(ELeave) CSdpSearchPattern();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C void CSdpSearchPattern::ConstructL()
/** Allocates a new UUID array. */
	{
	iUUIDArray = new(ELeave) CArrayFixFlat<TUUID>(1);
	}

EXPORT_C CSdpSearchPattern::~CSdpSearchPattern()
/** Destructor

Destroys the UUID array. */
	{
	delete iUUIDArray;
	}

CSdpSearchPattern::CSdpSearchPattern()
	{
	}

EXPORT_C TInt CSdpSearchPattern::AddL(const TUUID& aUUID)
/** Adds a UID to the list.

@param aUUID UUID to add
@return Position in the list that the UUID is inserted at, or KErrAlreadyExists 
if it's already in the list */
	{
	TInt pos;
	if(Find(aUUID, pos) != 0)
		{
		// The UUID is not currently in the pattern.  Try to add it.
		iUUIDArray->InsertL(pos, aUUID);
		}
	else
		{
		pos = KErrAlreadyExists;
		}

	return pos;
	}

EXPORT_C TInt CSdpSearchPattern::Count() const
/** Gets the number of UUIDs in the list.

@return Number of UUIDs in the list */
	{
	return iUUIDArray->Count();
	}

EXPORT_C const TUUID CSdpSearchPattern::At(TInt anIndex) const
/** Gets the UUID at the specified position in the list.

@param anIndex Position of the UUID to get
@return UUID at specified position */
	{
	return iUUIDArray->At(anIndex);
	}

EXPORT_C void CSdpSearchPattern::Reset()
/** Removes all UUIDs from the list. */
	{
	iUUIDArray->Reset();
	}

EXPORT_C TInt CSdpSearchPattern::Remove(const TUUID& aUUID)
/** Removes a UUID from the list.

@param aUUID UUID to remove
@return Position in the list of the UUID, or KErrNotFound if it's not in the 
list */
	{
	TInt pos;
	if (Find(aUUID, pos)==0)
		{
		iUUIDArray->Delete(pos);
		return pos;
		}
	return KErrNotFound;
	}

EXPORT_C TInt CSdpSearchPattern::Find(const TUUID& aUUID, TInt &aPos) const
/** Gets the position of the specified UUID in the list.

@param aUUID UUID to find
@param aPos Position of the UUID if it is in the list, otherwise the position 
where it would be inserted
@return 0 if aUUID is found, otherwise non-zero */
	{ // first parameter is Offset of data in UUID
	TInt result;
	for(TInt i=0;i<iUUIDArray->Count();i++)
		{
		result = aUUID.LongForm().Compare(iUUIDArray->At(i).LongForm());
		
		if(result == 0)
			{
			aPos = i;
			return 0;	// Inicates that the UUID was found.
			}
		else
			{
			// The UUID being search for is less than the UUID at the
			// current array index (i).  The search is over.  Set the
			// position reference (aPos) to reflect that the current
			// index is where the UUID should be inserted if required.
			if(result < 0)
				{
				aPos = i;
				return KErrNotFound;
				}
			}
		}
		
	// Either the UUID array is empty, or the search UUID is greater than
	// all the UUIDs in the array.
	aPos = iUUIDArray->Count();
	return KErrNotFound;
	}

EXPORT_C TBool CSdpSearchPattern::IsEmpty()
	{
	return Count()==0;
	}
	
MSdpElementBuilder* CSdpSearchPattern::BuildUUIDL(const TUUID& aUUID)
	{
	AddL(aUUID);
	// Could error on repeated UUIDs (we ignore them anyway though)
	return this;
	}

MSdpElementBuilder* CSdpSearchPattern::BuildDESL()
	{
	return this;
	}

MSdpElementBuilder* CSdpSearchPattern::StartListL()
	{
	return this;
	}

MSdpElementBuilder* CSdpSearchPattern::EndListL()
	{//Service search pattern is complete, so terminate this parse
	return 0;
	}

class CEncodedVisitorAdapter : public CBase, public MIgnorer
	{
public:
	CEncodedVisitorAdapter(CServiceSearchVisitor& aVisitor)
		:iVisitor(aVisitor)
		{}
	virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID)
		{
		iVisitor.FoundUUIDL(aUUID);
		return this;
		}
private:
	CServiceSearchVisitor& iVisitor;
	};


// Class CServiceSearchVisitor 

CServiceSearchVisitor::CServiceSearchVisitor()
	{
	}

CServiceSearchVisitor::~CServiceSearchVisitor()
	{
//	delete iSearchPattern;
	delete iFoundIndexs;
	delete iAdapter;
	delete iParser;
	}

EXPORT_C CServiceSearchVisitor* CServiceSearchVisitor::NewLC(const CSdpSearchPattern& aPattern)
	{
	CServiceSearchVisitor* self = new(ELeave)CServiceSearchVisitor();
	CleanupStack::PushL(self);
	self->ConstructL(aPattern);
	return self;
	}

EXPORT_C CServiceSearchVisitor* CServiceSearchVisitor::NewL(const CSdpSearchPattern& aPattern)
	{
	CServiceSearchVisitor* self = CServiceSearchVisitor::NewLC(aPattern);
	CleanupStack::Pop();
	return self;
	}

void CServiceSearchVisitor::ConstructL(const CSdpSearchPattern& aPattern)
/** 2nd phase constructor. */
	{
	iSearchPattern = &aPattern;
	iAdapter = new(ELeave) CEncodedVisitorAdapter(*this);
	iParser = CElementParser::NewL(iAdapter);
	}

void CServiceSearchVisitor::SearchRecordL(CSdpServRecord& aRec)
	{
	if (iFoundIndexs)
		{
		delete iFoundIndexs;
		iFoundIndexs = 0;
		}
	iFoundIndexs = CBitMapAllocator::NewL(iSearchPattern->Count());
	for(TServAttrIter attrIter(aRec.AttributeIter()); attrIter; attrIter++)
		{// Iterate thru attributes in record
		(*attrIter).AcceptVisitorL(*this);
		}
	}

EXPORT_C void CServiceSearchVisitor::SearchDbL(CSdpDatabase &aDb, const CSdpSearchPattern &aPattern, MServiceSearchHandler& aObserver)
   	{
	__ASSERT_DEBUG(aPattern.Count() > 0, DbPanic(ESdpDbBadSearchPattern));
//	SDP_DEBUG(3, FPrint(_L("Searching SDP DB for pattern with %d entries\n"), aPattern.Count()));
	CServiceSearchVisitor* theVisitor = CServiceSearchVisitor::NewLC(aPattern);

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		TRAPD(err, theVisitor->SearchRecordL(*recIter));
		if (err == KErrCompletion)
			{// Found!
//			SDP_DEBUG(3, FPrint(_L("Match found in record 0x%x\n"), (*recIter).Handle()));
			aObserver.RecordFoundL(*recIter);
			}
		else if(err != KErrNone)
			{
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy();//theVisitor
	}

void CServiceSearchVisitor::VisitAttributeL(CSdpAttr &/*aAttribute*/)
	{
	}

void CServiceSearchVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	switch (aType)
		{
	case ETypeUUID:
		FoundUUIDL(aValue.UUID());
		break;
	case ETypeEncoded:
		{
// parse out any UUIDs in this encoded attribute
//		TInt rem;
		iParser->Reset();
		/*rem = */iParser->ParseElementsL(aValue.Des());
		break;
		}
	default:
		break;
		}
	}

void CServiceSearchVisitor::StartListL(CSdpAttrValueList &/*aList*/)
	{
	}

void CServiceSearchVisitor::EndListL()
	{
	}


void CServiceSearchVisitor::FoundUUIDL(const TUUID& aUUID)
	{
	TInt pos;
	if (iSearchPattern->Find(aUUID, pos)==0 &&
		iFoundIndexs->IsFree(pos))
		{
		iFoundIndexs->AllocAt(pos);
		if (iFoundIndexs->Avail() == 0)
			{// We've found what we were searching for. Quick complete with a leave
			User::Leave(KErrCompletion); //Hmmmm!!!
			}
		}
	}
