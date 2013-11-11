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
 @file
 @internalTechnology
*/

#ifndef RESPONSESIZEVISITOR_H
#define RESPONSESIZEVISITOR_H

#include <e32base.h>
#include <btsdp.h>
#include "SDPDatabase.h"
#include "ServiceSearchVisitor.h"
#include "ExtractorVisitor.h"
#include "MAttributeVisitor.h"
#include <e32base_private.h>


class CSdpDatabase;
class CSdpServRecord;
class CSdpAttr;

/** 
	CAttrSizeItem
	contains the attribute ID, the size of the attribute and a pointer to the attribute
	This object is attached to a list (CArrayPtrFlat) contained in CHandleItem object
**/
NONSHARABLE_CLASS(CAttrSizeItem) // not a cbase because no benefit, just overhead
{
friend class CSizeAccumulator;
public:
	CAttrSizeItem(){};
	~CAttrSizeItem(){iAttribute=0;};
	CAttrSizeItem(const TSdpAttributeID aAttributeID, TUint aSize, CSdpAttr* aAttribute):
		iAttributeID(aAttributeID),
		iSize(aSize),
		iAttribute(aAttribute) {};
	inline TSdpAttributeID AttID();
	inline TUint Size();
	inline CSdpAttr* Attr();

private:
	TSdpAttributeID iAttributeID;
	TUint iSize;
	CSdpAttr* iAttribute;
};
inline TSdpAttributeID CAttrSizeItem::AttID() {return iAttributeID;};
inline TUint CAttrSizeItem::Size() {return iSize;};
inline CSdpAttr* CAttrSizeItem::Attr() {return iAttribute;};

// const TUint KSDPAttListGran = 8*(sizeof(CAttrSizeItem*)); granularity for a Seg
const TUint KSDPAttListGran = 8; // granularity for a flat

/**
	CHandleItem
	keeps the Service record handle and optionally manages an array of attributes
	for that record which match the Attribute ID match list
**/
NONSHARABLE_CLASS(CHandleItem) : public CBase
{
friend class CSizeAccumulator;
public:
	CHandleItem(){};
	~CHandleItem();
	IMPORT_C static CHandleItem* NewL();
	IMPORT_C static CHandleItem* NewLC();
	IMPORT_C static CHandleItem* CHandleItemL(TSdpServRecordHandle aHandleID, CSdpServRecord* aRecord);

//	PutRecord(TSdpServRecordHandle aHandleID, const CSdpServRecord* aRecord); // update the NULL data

	void AddAttrItemL(CAttrSizeItem* aItem);
	void AddAttrItemL(const TSdpAttributeID aAttributeID, TUint aSize, CSdpAttr* aAttribute);
	inline TInt Count();
private:
	void ConstructL();
	TSdpServRecordHandle iHandleID;
	CSdpServRecord* iRecord;
	TUint iRecordSize;
	CArrayPtrFlat<CAttrSizeItem>* iAttrSizeList; // stores the attributes found
};
inline TInt CHandleItem::Count() {return iAttrSizeList->Count();};

// const TUint KSDPHandListGran = 8*(sizeof(CHandleItem*)); granularity for a Seg
const TUint KSDPHandListGran = 8; // granularity for a flat

NONSHARABLE_CLASS(CSizeAccumulator) : public CBase
	{
public:
// these methods are used in building this object by searching the database
	~CSizeAccumulator();
	IMPORT_C static CSizeAccumulator* NewL();
	IMPORT_C static CSizeAccumulator* NewLC();
	void AddHandleL(CHandleItem* aHandleItem); // create a new array element
//	void AddHandle(TSdpServRecordHandle aHandleID, CSdpServRecord* aRecord); // create a new array element
//	void AddAttrib(TSdpAttributeID aAttributeID, CSdpAttr* aAttribute, TUint aSize);

// these methods are used in building the server response once the search has been carried out
	IMPORT_C TUint SizeLeft(); // can be called before or after setting the start point
	IMPORT_C TUint16 CrcAttribs(); // CRC of the attribute values, not headers
	IMPORT_C TBool StartAt(TUint aOffset, TUint& aPartSent, TInt& aRec, TInt& aAtt); // calculates and sets the next record and attribute
	IMPORT_C TInt HandleCount();		// returns the count of handles in the array
	IMPORT_C TSdpServRecordHandle HandleAt(TInt aOffset); // returns the Record handle at offset i
	IMPORT_C TUint HandleSize(TInt aOffset);  // returns the size of the record at offset aOffset
	IMPORT_C TInt AttrCount(TInt aOffset);	// returns the count of attributes for record at offset i
	IMPORT_C CAttrSizeItem* AttributeOf(TInt aHandleOffset, TInt aAttOffset); // returns the attribute size object (CAttrSizeItem) at i,j

private:
	CSizeAccumulator():iFirstRec(0),iFirstAtt(0){};
	void ConstructL();

	CArrayPtrFlat<CHandleItem>* iHandleList;	// stores the records found
	TInt iFirstRec;		// offsets for continuation
	TInt iFirstAtt;
	};

/**
	Class response size visitor.
	Supply a CSizeAccumulator object which will have all the necessary sizes on return.
	a) SizeRespSSL is called by service search to get just header list
	b) SizeRespARL is called by Attribute request to get the list of sizes of matching attributes for the one record
	c) SizeRespSAL is called by Service Attribute Search to get the two lists.
	a, c carry out the same UUID search over the attribute tree as ServiceSearchVisitor::SearchDbL
	b carries out the same selection of attributes as CAttrExtractVisitor::EncodeAttributesL

**/
class CSizeEncVisitorAdaptor;

NONSHARABLE_CLASS(CResponseSizeVisitor) : public CBase, public MAttributeVisitor
	{
public:
    ~CResponseSizeVisitor();

	IMPORT_C static void SizeRespSSL(CSdpDatabase& aDb, const CSdpSearchPattern& aPattern, CSizeAccumulator& aCollector);
	IMPORT_C static void SizeRespARL(CSdpServRecord& aRec, const CSdpAttrIdMatchList& aList, CSizeAccumulator& aCollector);
	IMPORT_C static void SizeRespSAL(CSdpDatabase& aDb, const CSdpSearchPattern& aPattern, const CSdpAttrIdMatchList& aList, CSizeAccumulator& aCollector);

	void FoundUUIDL(const TUUID& aUUID); // needs to be public because of the encoder visitor adaptor

private:
	static CResponseSizeVisitor* NewL();
	static CResponseSizeVisitor* NewLC();
    CResponseSizeVisitor();
	void ConstructL();

	void SearchRecordL(CSdpServRecord& aRec);

	// MAttributeVisitor interface
    void VisitAttributeL(CSdpAttr &aAttribute);
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList &aList);
    void EndListL();

private:
	CSizeEncVisitorAdaptor* iAdapter;
	CElementParser* iParser;				// used by the encoded UUID parser
	const CSdpSearchPattern* iSearchPattern;	// a local pointer to the UUID search list
	CSizeAccumulator* iCollector;			// a local pointer to the size collector object supplied
	const CSdpAttrIdMatchList* iAtMatList;		// a local pointer to the attribute match list or NULL
	CHandleItem* iCurrentRec;				// we build the handle and attribute list and perhaps throw it away
	CBitMapAllocator* iFoundIndex;			// a simpler match
	TInt iSearchSize;						// how many items in search pattern
	TBool	iUseThis;						// do we use this record ?
	};


#endif
