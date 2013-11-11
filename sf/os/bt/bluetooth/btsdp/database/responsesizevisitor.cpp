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
#include "responsesizevisitor.h"
#include "SDPDatabase.h"
#include "mignorer.h"
#include "DataEncoder.h"


// Class CHandleItem
EXPORT_C CHandleItem* CHandleItem::NewLC()
	{
	CHandleItem* self = new(ELeave)CHandleItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CHandleItem* CHandleItem::NewL()
	{
	CHandleItem* self = CHandleItem::NewLC();
	CleanupStack::Pop();
	return self;
	}

void CHandleItem::ConstructL()
	{
	iAttrSizeList = new (ELeave) CArrayPtrFlat<CAttrSizeItem>(KSDPAttListGran);
	}

CHandleItem::~CHandleItem()
	{
	if (iAttrSizeList)
		{
		iAttrSizeList->ResetAndDestroy();
		delete iAttrSizeList;
		}
	}

EXPORT_C CHandleItem* CHandleItem::CHandleItemL(TSdpServRecordHandle aHandleID, CSdpServRecord* aRecord)
	{
	CHandleItem* self = NewL();
	self->iHandleID = aHandleID;
	self->iRecord = aRecord;
	self->iRecordSize = 0;
	return self;
	}

void CHandleItem::AddAttrItemL(CAttrSizeItem* aItem)
	{
	iAttrSizeList->AppendL(aItem);
	iRecordSize += aItem->Size() + 3; // add the attrID header size;
	}

void CHandleItem::AddAttrItemL(const TSdpAttributeID aAttributeID, TUint aSize, CSdpAttr* aAttribute)
	{
	CAttrSizeItem* sizeItem = new (ELeave) CAttrSizeItem(aAttributeID, aSize, aAttribute);
	AddAttrItemL(sizeItem);
	}


// Class CSizeAccumulator

EXPORT_C CSizeAccumulator* CSizeAccumulator::NewLC()
	{
	CSizeAccumulator* self = new(ELeave)CSizeAccumulator();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSizeAccumulator* CSizeAccumulator::NewL()
	{
	CSizeAccumulator* self = CSizeAccumulator::NewLC();
	CleanupStack::Pop();
	return self;
	}

void CSizeAccumulator::ConstructL()
	{
	iHandleList = new (ELeave) CArrayPtrFlat<CHandleItem>(KSDPHandListGran);
	}

CSizeAccumulator::~CSizeAccumulator()
	{
	if ( iHandleList )
		{
		iHandleList->ResetAndDestroy();
		delete iHandleList;
		}
	}

void CSizeAccumulator::AddHandleL(CHandleItem* aHandleItem)
	{
	iHandleList->AppendL(aHandleItem);
	}

// do a crc across all the attributes
EXPORT_C TUint16 CSizeAccumulator::CrcAttribs()
	{
	TUint16 totalCrc=0;
	for (TInt i = 0; i<HandleCount(); i++)
		{
		for (TInt j = 0; j<AttrCount(i); j++)
			{
			CAttrSizeItem* attr = AttributeOf(i,j);
			CSdpAttrValue& theVal = attr->Attr()->Value();
			Mem::Crc(totalCrc, reinterpret_cast<const TAny*>(theVal.Des()[0]), attr->Size());
			}
		}
	return totalCrc;

	}

/**
	CSizeAccumulator::SizeLeft
	calculate the size of the attribute list.

	Foundation of the continuation handling. 
	It is used together	with StartAt.
	It walks through the list of records, each should contain
	a list of attributes. There are internal start offsets for
	both the record and attribute lists. These are set with StartAt.

	The method returns the size (in bytes) of the attributes, together with
	3 bytes for each attribute ID (1 byte header, 2 byte UInt) and the correct
	number of bytes for each DES header for the attributes of each record.

	It does NOT calculate the DES header for a list of records. So an extra
	DES must be created for Searvice Search Attribute.

	This allows the total length to be written before the rest of the response
	packet. It also allows the request handler to decide if it can fit the whole
	response in to the response packet with or without continuation.
	Return format is
 @verbatim
		Size left					TUint
 @endverbatim
**/
EXPORT_C TUint CSizeAccumulator::SizeLeft()
	{
	TUint totalSize=0;
	TInt handleCount = HandleCount();	// for testing
	for (TInt i = iFirstRec; i < handleCount; i++)
		{
		TUint recordSize = 0;
		TInt attrCount = AttrCount(i); // for testing
		for (TInt j = iFirstAtt; j < attrCount; j++)
			{
			CAttrSizeItem* attr = AttributeOf(i,j);
			recordSize += attr->Size();
			recordSize +=3;		// the size of a attribute ID in the list
			}
		if (iFirstAtt == 0)
			{ // are we sizing the whole record ?
			TUint recRecSize = iHandleList->At(i)->CHandleItem::iRecordSize; // for testing
			if (recordSize != recRecSize) DbPanic(ESdpDbBadSearchPattern);
			}
		if (recordSize)
			{ // only add the header if there is any data
			TUint desSize;
			desSize = (recordSize > 0xff) ? 3 : 2;
			desSize = (recordSize > 0xffff) ? 5 : desSize;
			totalSize += recordSize + desSize;	// size of the DES header for this record
			}
		}
	return totalSize;

	}

	
/**
	CSizeAccumulator::StartAt
	Supplied with the data sent so far.
	This walks through the list of service search record handles
	and the the list of attributes associated with each record.

	Once it has found the location where the size is less than or
	equal to the parameter supplied, it sets CSizeAccumulators
	internal record and attribute offsets to the point it has
	reached. It will not update the offsets if the data is greater
	than the input offset.
	
	The method returns the two offsets and the data already sent
	from the current attribute.

	The method counts the DES header at the start of any record. It
	does NOT count the overall DES header if there is more than one
	record. To do this would require a count of all bytes to evaluate
	the DES header size. This action is the same as that of SizeLeft.

	******************************************************** 
	IMPORTANT NOTE: If aOffset traverses the database and  
	STOPS IN THE MIDDLE OF A NEW RECORD DES HEADER, it will return 
	THE DES HEADER OFFSET in the ATTRIBUTE OFFSET PARAMETER.
	This is ambiguous...
	****A calling function should ensure aOffset cannot do this***

	The method returns a bool to indicate the setting was OK. It will
	be false if the two index parameters were used and they would
	have set the lists beyond their end. Also it returns false if the
	offset was beyond the end of the data. If the method returns EFalse,
	the two index parameters are set to 0.

	This method can reset the parameters by being called with offset 0.
	However it will then return false.
	Parameter format is
 @verbatim
		Offset in bytes from start of attribute data		TUint
		Record index (0...)									TInt
		Attribute index (0...)								TInt
 @endverbatim
	Return format is
 @verbatim
		Were the offsets set OK								TBool
		Record index is set									TInt
		Attribute index is set								TInt
 @endverbatim

**/
EXPORT_C TBool CSizeAccumulator::StartAt(TUint aOffset, TUint& aPartSent, TInt& aRec, TInt& aAtt)
	{
	// calculates and sets the next record and attribute
	if (aOffset == 0)
		{
		aRec = 0; iFirstRec = 0;
		aAtt = 0; iFirstAtt = 0;
		aPartSent = 0;
		return EFalse;
		}
	TUint totalSize=0;
	TBool more = ETrue;
	TInt topRec = HandleCount();
	if (aRec > topRec)
		{
		aRec = 0; iFirstRec = 0;
		aAtt = 0; iFirstAtt = 0;
		aPartSent = 0;
		return EFalse;
		}
	else if (aRec == topRec)
		{
		TInt partAtt = AttrCount(topRec);
		if (aAtt > partAtt)
			{
			aRec = 0; iFirstRec = 0;
			aAtt = 0; iFirstAtt = 0;
			aPartSent = 0;
			return EFalse;
			}
		}
	iFirstRec = aRec;
	TInt firstAttForNextRecord = aAtt;
	while (more && (iFirstRec < topRec))
		{
		TUint recordSize = iHandleList->At(iFirstRec)->CHandleItem::iRecordSize;
		TInt topAtt = AttrCount(iFirstRec);
		iFirstAtt = firstAttForNextRecord < topAtt ? firstAttForNextRecord : topAtt;
// except for first record we always start with a record's first attribute
		firstAttForNextRecord = 0; 
// add in the DES header size for this record
		TUint desSize;
		desSize = (recordSize > 0xff) ? 3 : 2;
		desSize = (recordSize > 0xffff) ? 5 : desSize;
		if (totalSize + desSize > aOffset)
			//see "IMPORTANT NOTE" above this method
			{
			more = EFalse;
			break;
			}
		totalSize += desSize;
		while (more && (iFirstAtt < topAtt))
			{
			CAttrSizeItem* attr = AttributeOf(iFirstRec, iFirstAtt);
			TUint nextSize = attr->Size();
			nextSize += 3;	// Attribute ID and its header
			if (totalSize + nextSize > aOffset)
				{
				more = EFalse;
				break;
				}
			totalSize += nextSize;
			iFirstAtt++;
			}
		if (more) iFirstRec++;
		}
	if (totalSize == aOffset) more = EFalse;
	if (more)
		{
		aRec = 0; iFirstRec = 0;
		aAtt = 0; iFirstAtt = 0;
		aPartSent = 0;
		return EFalse;
		}
	aRec = iFirstRec;
	aAtt = iFirstAtt;
	aPartSent = aOffset - totalSize;
	return ETrue;
//	__ASSERT_DEBUG(totalSize != aOffset, DbPanic(ESdpDbBadSearchPattern));
	}

EXPORT_C TInt CSizeAccumulator::HandleCount()
	{
	return iHandleList->Count();
	}

EXPORT_C TSdpServRecordHandle CSizeAccumulator::HandleAt(TInt aOffset)
	{
	__ASSERT_DEBUG(aOffset <= iHandleList->Count(), DbPanic(ESdpDbBadSearchPattern)); // FIXME different panic code please
	return iHandleList->At(aOffset)->CHandleItem::iHandleID;
	}

EXPORT_C TUint CSizeAccumulator::HandleSize(TInt aOffset)
	{
	return iHandleList->At(aOffset)->CHandleItem::iRecordSize;
	}

EXPORT_C TInt CSizeAccumulator::AttrCount(TInt aOffset)
	{
	if (iHandleList->Count() == 0) return 0;
	__ASSERT_DEBUG(aOffset <= iHandleList->Count(), DbPanic(ESdpDbBadSearchPattern));
	if (iHandleList->At(aOffset) == NULL) return 0;
	return iHandleList->At(aOffset)->CHandleItem::iAttrSizeList->Count();
	}

EXPORT_C CAttrSizeItem* CSizeAccumulator::AttributeOf(TInt aHandleOffset, TInt aAttOffset)
	{
	if (iHandleList->Count() == 0) return (CAttrSizeItem*)0;
	__ASSERT_DEBUG(aHandleOffset < iHandleList->Count(), DbPanic(ESdpDbBadSearchPattern));
	CHandleItem* hnd = iHandleList->At(aHandleOffset);
	if (hnd->iAttrSizeList->Count() == 0) return (CAttrSizeItem*)0;
	__ASSERT_DEBUG(aAttOffset < hnd->iAttrSizeList->Count(), DbPanic(ESdpDbBadSearchPattern));
//	return iHandleList->At(aHandleOffset)->CHandleItem::iAttrSizeList->At(aAttOffset);
	return hnd->iAttrSizeList->At(aAttOffset);
	}



// need this because CResponseSizeVisitor already has start list method...
class CSizeEncVisitorAdaptor : public MIgnorer
	{
public:
	CSizeEncVisitorAdaptor(CResponseSizeVisitor& aVisitor)
		:iVisitor(aVisitor)
		{}
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID)
		{
		iVisitor.FoundUUIDL(aUUID);
		return this;
		}
private:
	CResponseSizeVisitor& iVisitor;
	};




// Class CResponseSizeVisitor 

CResponseSizeVisitor::CResponseSizeVisitor()
	{
	}

CResponseSizeVisitor::~CResponseSizeVisitor()
	{
	delete iFoundIndex;
	delete iCurrentRec;
	delete iAdapter;
	delete iParser;
	}

CResponseSizeVisitor* CResponseSizeVisitor::NewLC()
	{
	CResponseSizeVisitor* self = new(ELeave)CResponseSizeVisitor();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CResponseSizeVisitor* CResponseSizeVisitor::NewL()
	{
	CResponseSizeVisitor* self = CResponseSizeVisitor::NewLC();
	CleanupStack::Pop();
	return self;
	}

void CResponseSizeVisitor::ConstructL()
	{
	iAdapter = new(ELeave) CSizeEncVisitorAdaptor(*this);
	iParser = CElementParser::NewL(iAdapter);
	}

// Iterate thru attributes in record
/* Search through all attributes in a record
   either to find if it matches the UUIDlist,
   or if the attributes match the AttrIdMatchList
   or both. If there is no UUIDList then the sizes
   of matched attributes is saved to the size collector
*/
void CResponseSizeVisitor::SearchRecordL(CSdpServRecord& aRec)
	{
// could be in calling routine
	if (iSearchPattern)
	{
		if (iFoundIndex)
			{
			delete iFoundIndex;
			iFoundIndex = 0;
			}
		iFoundIndex = CBitMapAllocator::NewL(iSearchSize);
		iUseThis = EFalse;
	}
	else
		iUseThis = ETrue;
//	if (iCurrentRec) delete iCurrentRec;	// I want a reset.
	iCurrentRec = CHandleItem::CHandleItemL(aRec.Handle(), &aRec);
	
	for(TServAttrIter attrIter(aRec.AttributeIter()); attrIter; attrIter++)
		{
/* 
	we are checking every attribute, searching for the UUIDs. We don't exit if
	we find them all because we also want to get the sizes of any attributes which
	match our list if we have one.
	The array of sizes is built, then thrown away if it doesn't match.
 
*/
		if (iAtMatList)
			{
			TSdpAttributeID theAttrID = (*attrIter).AttributeID();
			if(iAtMatList->InMatchList(theAttrID))
				{// create a new array entry
				TUint size = (*attrIter).Value().DataSize();
				TSdpElementType type = (*attrIter).Value().Type();
				if (type != ETypeEncoded) 
					{
// some server records may be un-encoded
					size += TElementEncoder::HeaderSize(type, size); // add the header size
					}
				if(!(type==ETypeNil)&&!(type==ETypeEncoded&&size<=1))
				{ // only include attribute if not either null or encoded null ...
				  // (size of encoded (which INCLUDES header) in all non-null cases is > 1
				  //  because other attribute types either have
				  //  to have a header plus at least one byte, or they have at least a two byte header.)
					iCurrentRec->AddAttrItemL(theAttrID, size, attrIter);
					}
				}
			}
		// check if any UUIDs match only if we have a UUIDList.
		if (iSearchPattern) (*attrIter).AcceptVisitorL(*this);
		}
	if (iUseThis)
		{
		// now we have to copy the array(s) we've built up.
		iCollector->AddHandleL(iCurrentRec);
		iCurrentRec = 0;
		}
	else
		{
		delete iCurrentRec;	// I don't want a heap fault.
		iCurrentRec = 0;
		}
	}


//	static void SizeRespSSL(CSdpDatabase& aDb, const CSdpSearchPattern &aPattern, const CSizeAccumulator& aCollector);
//	static void SizeRespARL(CSdpServRecord& aRec, CSdpAttrIdMatchList &aList, const CSizeAccumulator& aCollector);
//	static void SizeRespSAL(CSdpDatabase &aDb, const CSdpSearchPattern &aPattern, CSdpAttrIdMatchList &aList, const CSizeAccumulator& aCollector);
// we need three of these calls, all slightly different
	EXPORT_C void CResponseSizeVisitor::SizeRespSSL(CSdpDatabase& aDb, const CSdpSearchPattern& aPattern, CSizeAccumulator& aCollector)
   	{
	if (aPattern.Count() == 0) User::Leave(KErrArgument); // this is part of spec.
//	SDP_DEBUG(3, FPrint(_L("Sizing SDP DB for pattern with %d entries\n"), aPattern.Count()));
	CResponseSizeVisitor* theVisitor = CResponseSizeVisitor::NewLC();
	theVisitor->iCollector = &aCollector;
	theVisitor->iAtMatList = NULL;		// using it as a flag...
	theVisitor->iSearchPattern = &aPattern;
	theVisitor->iSearchSize = theVisitor->iSearchPattern->Count();

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		theVisitor->SearchRecordL(*recIter);
		}
	CleanupStack::PopAndDestroy();//theVisitor
	}

	EXPORT_C void CResponseSizeVisitor::SizeRespARL(CSdpServRecord& aRec, const CSdpAttrIdMatchList& aList, CSizeAccumulator& aCollector)
	{
//	SDP_DEBUG(3, FPrint(_L("Sizing SDP DB for single record\n")));
	CResponseSizeVisitor* theVisitor = CResponseSizeVisitor::NewLC();
	theVisitor->iCollector = &aCollector;
	theVisitor->iAtMatList = &aList;		// using it as a flag...
	theVisitor->iSearchPattern = NULL;
	theVisitor->iSearchSize = 0;
	theVisitor->SearchRecordL(aRec);
	CleanupStack::PopAndDestroy();//theVisitor
	}

	EXPORT_C void CResponseSizeVisitor::SizeRespSAL(CSdpDatabase &aDb, const CSdpSearchPattern &aPattern, const CSdpAttrIdMatchList& aList, CSizeAccumulator& aCollector)
	{
	if (aPattern.Count() == 0) User::Leave(KErrArgument); // this is part of spec.
//	SDP_DEBUG(3, FPrint(_L("Sizing SDP DB (and attribs) with %d UUIDs\n"), aPattern.Count()));
	CResponseSizeVisitor* theVisitor = CResponseSizeVisitor::NewLC();
	theVisitor->iCollector = &aCollector;
	theVisitor->iAtMatList = &aList;		// using it as a flag...
	theVisitor->iSearchPattern = &aPattern;
	theVisitor->iSearchSize = theVisitor->iSearchPattern->Count();

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		theVisitor->SearchRecordL(*recIter);
		}
	CleanupStack::PopAndDestroy();//theVisitor
	}



void CResponseSizeVisitor::VisitAttributeL(CSdpAttr& /*aAttribute*/)
	{
	}

void CResponseSizeVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	switch (aType)
		{
	case ETypeUUID:
		FoundUUIDL(aValue.UUID());
		break;
		case ETypeEncoded:
// parse out any UUIDs in this encoded attribute
		iParser->Reset();
		/*rem = */iParser->ParseElementsL(aValue.Des());
		break;
	default:
		return;
		}
	}

void CResponseSizeVisitor::StartListL(CSdpAttrValueList &/*aList*/)
	{
	}

void CResponseSizeVisitor::EndListL()
	{
	}

void CResponseSizeVisitor::FoundUUIDL(const TUUID& aUUID)
	{
	TInt pos;
	if (iSearchPattern->Find(aUUID, pos)==0 &&
		iFoundIndex->IsFree(pos))
		{
		iFoundIndex->AllocAt(pos);
		if (iFoundIndex->Avail() == 0)
			{// We've found what we were searching for.
			iUseThis = ETrue;
			}
		}
	}
