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
#include "ExtractorVisitor.h"
#include <btsdp.h>
#include "DataEncoder.h"
#include "SDPDatabase.h"
#include "sdputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

#ifdef _DEBUG
PANICCATEGORY("extrvst");
#endif

CAttrExtractVisitor *CAttrExtractVisitor::NewLC(TElementEncoder aEncoder)
	{
	CAttrExtractVisitor *self = new(ELeave) CAttrExtractVisitor(aEncoder);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CAttrExtractVisitor *CAttrExtractVisitor::NewL(TElementEncoder aEncoder)
	{
	CAttrExtractVisitor *self = CAttrExtractVisitor::NewL(aEncoder);
	CleanupStack::Pop();
	return self;
	}

CAttrExtractVisitor::CAttrExtractVisitor(TElementEncoder aEncoder)
	: iEncoder(aEncoder)
	{
	}

void CAttrExtractVisitor::ConstructL()
	{
	}

CAttrExtractVisitor::~CAttrExtractVisitor()
	{
	}

// the only entry to this class - creates an instance of itself and then calls the FindAttributes
// method of aList on the record supplied.
EXPORT_C void CAttrExtractVisitor::EncodeAttributesL(TElementEncoder aEncoder, CSdpServRecord &aRec, CSdpAttrIdMatchList &aList)
/**	Encode attributes in aRec that are in the match list aList, using aEncoder.*/
	{
	CAttrExtractVisitor *self = CAttrExtractVisitor::NewLC(aEncoder);

	TRAPD(err, aList.FindAttributesL(aRec, *self));

	// If we ran out of buffer space, a positive leave will have happened in VisitAttributeValueL, below
	// else, the leave was an error, which should be re-thrown
	if (err < 0)
		User::Leave(err);

	CleanupStack::PopAndDestroy(/*self*/);
	}

void CAttrExtractVisitor::AttributeInMatchListL(CSdpAttr &aAttr)
	{
	aAttr.AcceptVisitorL(*this);
	}



void CAttrExtractVisitor::VisitAttributeL(CSdpAttr &aAttribute)
	{
	//FIXME check rem
	iEncoder.WriteUint(aAttribute.AttributeID(), sizeof(TSdpAttributeID));
	}

void CAttrExtractVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
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
		rem = iEncoder.WriteBoolean(aValue.Int());
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
		}
	if (rem)
		User::Leave(rem);
	}

void CAttrExtractVisitor::StartListL(CSdpAttrValueList &/*aList*/)
	{
	}

void CAttrExtractVisitor::EndListL()
	{
	}




//
// CSdpAttrIdMatchList
//

CSdpAttrIdMatchList::CSdpAttrIdMatchList()
	{
	}

EXPORT_C CSdpAttrIdMatchList* CSdpAttrIdMatchList::NewL()
/** Allocate and construct a new CSdpAttrIdMatchList object.

@return New CSdpAttrIdMatchList object */
	{
	CSdpAttrIdMatchList* self = new (ELeave) CSdpAttrIdMatchList;
	CleanupStack::PushL(self);
	self->iList = new (ELeave) CArrayFixFlat<TAttrRange> (2);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSdpAttrIdMatchList* CSdpAttrIdMatchList::NewL(const CSdpAttrIdMatchList& aAttrMatchList)
/** Allocate and construct a new CSdpAttrIdMatchList object as a copy of another 
CSdpAttrIdMatchList.

@param aAttrMatchList Object to copy
@return New CSdpAttrIdMatchList object */
	{
	CSdpAttrIdMatchList* self = new (ELeave) CSdpAttrIdMatchList;
	CleanupStack::PushL(self);
	self->iList = new (ELeave) CArrayFixFlat<TAttrRange> (2);
	static_cast<CArrayFixFlat<TAttrRange>*>(self->iList)->SetReserveL(aAttrMatchList.Count());
	for(TInt i = 0; i < aAttrMatchList.Count(); ++i)
		{
		self->AddL((aAttrMatchList.iList)->At(i));
		}
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CSdpAttrIdMatchList::~CSdpAttrIdMatchList()
/** Destructor. */
	{
	delete iList;
	}

EXPORT_C void CSdpAttrIdMatchList::AddL(TAttrRange aRange)
/** Adds a range of IDs to the list.

@param aRange Range to add */
	{
	if (aRange.iStart > aRange.iEnd)
		{// Bad range
		User::Leave(KErrArgument);
		}
		
		
	// Look for insertion point (pos) in the match list. 
		
	TInt pos = 0; 	// this new item will start the list unless we find an existing
					// range for it to overlap or follow

	// If the start of the range 'aRange' is contiguous with one of the existing match list ranges
	// then the insertion point is the index of that match list range.
	// If not it is one more than the index of the match list range that
	// precedes 'aRange'.
	// N.B. The ranges are stored in ascending order

	TBool bPositionFound = EFalse;
	TBool bStartInRangeOrContiguous = EFalse;	

	// Find the first position in the list in which (or contiguous with) this range lies
	if (FindInOrContiguousWith(aRange.iStart, pos) == 0)
		{
		// the start of this new range overlaps with an existing one (returned in pos) or is contiguous with it
		bPositionFound = ETrue;
		bStartInRangeOrContiguous = ETrue;
		}	
	else
		{
		// find which range this new one will be after
		for (TInt i=iList->Count()-1; i>=0 && !bPositionFound; i-- )
			{
			if (aRange.iStart > (iList->At(i).iEnd + 1 ))
				{
				pos = i+1;
				bPositionFound = ETrue;
				}
			}
		}

	TAttrRange *insert;
	
	if (bStartInRangeOrContiguous)
		{// Don't actually insert new entry, just grow the existing entry
		insert = &iList->At(pos);
		
		// NB If the range to add has a start ID which is one less than the current start of range, 
		// the range may grow "downwards", but will never join up with the preceding range because
		// the start ID is known not to be contiguous with THAT range.
		insert->iStart = Min(insert->iStart, aRange.iStart); 
		insert->iEnd = Max(insert->iEnd, aRange.iEnd);
		}
	else
		{// Add a new entry
		iList->InsertL(pos, aRange);
		insert = &iList->At(pos);
		}

	while (pos < iList->Count()-1 && insert->IsContiguousWith(iList->At(pos+1).iStart) )
		{// There's now overlap with following item
		insert->iEnd = Max(insert->iEnd, iList->At(pos+1).iEnd);
		iList->Delete(pos+1);	// which leaves a new range to be checked at (pos+1), so no incrementing of pos is required	
		}
	__TEST_INVARIANT;
	}

EXPORT_C void CSdpAttrIdMatchList::RemoveL(TAttrRange aRange)
/** Removes a range of IDs from the list.

@param aRange Range to remove */
	{
	if (aRange.iStart > aRange.iEnd)
		{// Bad range
		User::Leave(KErrArgument);
		}
		
	TInt pos = 0; // the deletion point	

	TAttrRange *trim;
	
	// Find position for the start of the exclude range - is it partway through a current range?
	if ((Find(aRange.iStart, pos) == 0) && (aRange.iStart > (&iList->At(pos))->iStart))
		{
		// Trim a bit off this entry
		trim = &iList->At(pos);
		
		if (trim->iEnd > aRange.iEnd)
			{
			//It's bigger. Need to split in two
			// Grow first, in case it fails
			iList->InsertL(pos+1, TAttrRange(TSdpAttributeID(aRange.iEnd+1), trim->iEnd));
			trim->iEnd = TSdpAttributeID(aRange.iStart - 1);
			__TEST_INVARIANT;
			return;	// We know this remove range is fully handled now
			}
		trim->iEnd = TSdpAttributeID(aRange.iStart - 1);
		}
		
	// whether or not the trimming has already started, now move through the rest of the ranges	
	// removing entire entries which lie within the deletion range.
	while (pos < iList->Count() && (aRange.iEnd >= iList->At(pos).iEnd))
		{
		if (aRange.iStart <= iList->At(pos).iStart) 
			{
			// This complete item is not needed
			iList->Delete(pos);		
			}
		else
			{
			// this item is not affected, because we have not yet got to the start of the exclude range
			pos++; 
			}	 
		}

	// Does the end of the deletion range lie within this entry?
	if ( pos < iList->Count() )
		{
		trim = &iList->At(pos);
		if ((trim->iStart >= aRange.iStart) && (trim->iStart <= aRange.iEnd))
			{// The following item is too long -- trim it
			trim->iStart = TSdpAttributeID(aRange.iEnd + 1);
			}
		}
	__TEST_INVARIANT;
	}

TInt CSdpAttrIdMatchList::Find(TSdpAttributeID aAttrId, TInt &aPos) const
/** Tests if the specified attribute ID is in the list, and gets its position.

@param aAttrId Attribute ID to test
@param aPos Position of ID in the list, or unchanged if attribute not found
@return KErrNone if the attribute is in the list, else KErrNotFound */
	{
	__TEST_INVARIANT;
	TAttrRange searchRange;
	
	TBool bAttribInRange = EFalse;
	TInt ret=KErrNotFound;

 	for ( TInt i=0; (i<iList->Count()) && !bAttribInRange; i++ )
 		{
 		searchRange = iList->At(i);
 		if (searchRange.IsInRange(aAttrId) )
 			{
 			bAttribInRange = ETrue;
 			aPos = i;
 			ret = KErrNone;
 			}
 		}
	return ret;	
	}

TInt CSdpAttrIdMatchList::FindInOrContiguousWith(TSdpAttributeID aAttrId, TInt &aPos) const
/** Tests if the specified attribute ID is in the list (or next to a range in the list), and gets its position.

@param aAttrId Attribute ID to test
@param aPos Position in the list if the ID is found in a current range, or the lowest range with which the ID is contiguous, or unchanged if neither.
@return KErrNone if the attribute is in the list, else KErrNotFound */
	{
	__TEST_INVARIANT;
	TAttrRange searchRange;
	
	TBool bAttribInRange = EFalse;
	TInt ret=KErrNotFound;

 	for ( TInt i=0; (i<iList->Count()) && !bAttribInRange; i++ )
 		{
 		searchRange = iList->At(i);
 		if (searchRange.IsContiguousWith(aAttrId) )
 			{
 			bAttribInRange = ETrue;
 			aPos = i;
 			ret = KErrNone;
 			}
 		}
	return ret;	
	}


EXPORT_C TBool CSdpAttrIdMatchList::InMatchList(TSdpAttributeID aAttrId, TInt &aPos) const
/** Tests if the specified attribute ID is in the list, and gets its position.
	Do not use this - it gives an off-by-one error for aPos
	unless the attribute is the first one in a contiguous range.
	Use InMatchListRange() instead

@param aAttrId Attribute ID to test
@param aPos Position of ID in the list with an off-by-one error for IDs not at the start of a range, or 0 if not found
@return ETrue if the attribute is in the list, else EFalse */
	{
	TKeyArrayFix key (_FOFF(TAttrRange, iStart), ECmpTUint16);
	TAttrRange search(aAttrId);
	TInt ret = iList->FindIsq(search, key, aPos);
	return (ret == 0) || (aPos > 0 && iList->At(aPos-1).IsInRange(aAttrId));
	}

EXPORT_C TBool CSdpAttrIdMatchList::InMatchListRange(TSdpAttributeID aAttrId, TInt &aRange) const
	{
/* Tests if the specified attribute ID is in the list, and gets its position.

Param aAttrId: Attribute ID to test
Param aPos: Position of ID in the list, or 0 if not found
Returns ETrue if the attribute is in the list, else EFalse */

	
	aRange = 0;	// will not be changed unless the attribute has a match in the list
	TBool bAttribInRange = EFalse;
	TInt ret = Find(aAttrId, aRange);
	
	if (ret == KErrNone)	
		{
		bAttribInRange = ETrue;  
		}
		
	return bAttribInRange;
	}

EXPORT_C TUint CSdpAttrIdMatchList::EncodeL(TElementEncoder aEncoder) const
/** Supplies encoder with attribute Ids in the match list

@param aEncoder The encoder set to encode the attribute Ids in this match list
@return Number of bytes of this CSdpAttrIdMatchList for which there was NO space in the encoder
*/
	{
	TUint size = 0;
	TInt i;
	for (i = 0; i < iList->Count(); ++i)
		{
		TAttrRange& range = (*iList)[i];
		if (range.iStart == range.iEnd)
			size += 3; // 2 byte uint + header
		else
			size += 5; // 4 byte uint + header
		}

	TInt rem = aEncoder.WriteDES(size);
	for (i = 0; i < iList->Count(); ++i)
		{
		TAttrRange& range = (*iList)[i];
		if (range.iStart == range.iEnd)
			{
			rem += aEncoder.WriteUint(range.iStart, 2);
			}
		else
			{
			rem += aEncoder.WriteUint((range.iStart << 16) | range.iEnd, 4);
			}
		}	
	return rem;
	}


EXPORT_C TBool CSdpAttrIdMatchList::InMatchList(TSdpAttributeID aAttrId) const
	{
	TInt pos;
	return InMatchListRange(aAttrId, pos);
	}

EXPORT_C TInt CSdpAttrIdMatchList::Count() const
	{
	return iList->Count();
	}
	
void CSdpAttrIdMatchList::FindAttributesL(CSdpServRecord &aRec, MAttributeMatchHandler &aHandler) const
	{
	for(TServAttrIter attrIter(aRec.AttributeIter()); attrIter; attrIter++)
		{// Iterate thru attributes in record
		if (InMatchList((*attrIter).AttributeID()))
			{// this attribute is is the list -- signal it
			aHandler.AttributeInMatchListL(*attrIter);
			}
		}
	}

MSdpElementBuilder* CSdpAttrIdMatchList::BuildUintL(const TDesC8& aUint)
/** This should be one or two 16 bit integers, representing an AttrId of range thereof respectively. */
	{
	TInt len = aUint.Length();
	if (len != 2 && len != 4)
		{// Bad Uint size
		User::Leave(KErrCorrupt);
		}

	// Get either the two or four byte version
	TUint attrId = SdpUtil::GetUint(aUint);
	TAttrRange range;
	range.iStart = TUint16(len == 4 ? attrId >> 16 : attrId);
	range.iEnd = TUint16(attrId);

	// Add in place (optimised for incoming stream ordered -- but we will cope if isn't
	// Will leave if a bad range is supplied
	AddL(range);
	return this;
	}


MSdpElementBuilder* CSdpAttrIdMatchList::BuildDESL()
	{
	// Could assert we only get one of these
	return this;
	}

MSdpElementBuilder* CSdpAttrIdMatchList::StartListL()
	{
	return this;
	}

MSdpElementBuilder* CSdpAttrIdMatchList::EndListL()
/**	Attribute list now complete. Stop the parser by returning 0. */
	{
	return 0;
	}

EXPORT_C void CSdpAttrIdMatchList::__DbgTestInvariant() const
	{
#ifdef _DEBUG
	TInt count = iList->Count();
	for (TInt i = 0; i < count; ++i)
		{
		TAttrRange& range = iList->At(i);
		ASSERT_DEBUG(range.iStart <= range.iEnd);
		if (i < count - 1)
			{
			ASSERT_DEBUG(range.iEnd + 1 < iList->At(i+1).iStart);
			}
		}
#endif
	}

