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

#ifndef SDPSERVICERECORD_H
#define SDPSERVICERECORD_H

// CSDPAttribute
#include <btsdp.h>
#include "EncoderVisitor.h"
#include "SDPAttribute.h"
#include "sdputil.h"

class CSdpServRecord;

typedef TOrderedQue<CSdpServRecord> TServRecordList;
typedef TDblQueIter<CSdpServRecord> TServRecordIter;

NONSHARABLE_CLASS(CSdpServRecord) : public CBase, public MSdpElementBuilder
	{
public:
	IMPORT_C static CSdpServRecord* NewL();
	IMPORT_C static CSdpServRecord* NewServerSideL(const TUid& aUid);
	~CSdpServRecord();

	// all universal attr getters
	IMPORT_C TSdpServRecordHandle Handle() const;
	IMPORT_C TUid ClientUid() const;
	//void Description();
	//void ProtocolList();
	//void Icon();
	IMPORT_C void AddAttribute(CSdpAttr* aAttr);
	IMPORT_C void RecordStateChange();

	inline TBool operator>(const CSdpServRecord& aRec) const;
	inline TServAttrIter AttributeIter();
	inline static TInt LinkOffset();

	// MSdpElementBuilder interface definition
	// Most functions are errors, so not implemented
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
private:
	CSdpServRecord(const TUid& aClientUid);
	void ConstructL();
private:
	TServAttrList iAttributes;
	TDblQueLink iLink;
	//TUint16 iHandle;	// UPGRADE: Get this out of attributes??
	TUid iClientUid;
	CSdpAttrValueUint* iRecordState;// This value is used to update a state attribute whenever the 
									// record is modified to aid caching by remotes.
	CAttrEncoderVisitor* iEncoderVisitor;	// We maintain a pre-alloced encoder visitor so we can update 
									// attributes without having to allocate memory.
	RBuf8 iEncodeBuf;				// Used by the encoder to encode into
	};

inline TBool CSdpServRecord::operator>(const CSdpServRecord& aRec) const
	{
	return Handle() > aRec.Handle();
	}

/**
@see RecordStateChange
*/
inline TServAttrIter CSdpServRecord::AttributeIter()
	{
	return TServAttrIter(iAttributes);
	}

inline TInt CSdpServRecord::LinkOffset()
	{
	return _FOFF(CSdpServRecord, iLink);
	}

// new class to decode stuff from encoded records.
NONSHARABLE_CLASS(CRecordAttr) : public CBase, public MSdpElementBuilder
	{
public:
	IMPORT_C CRecordAttr();
	IMPORT_C ~CRecordAttr();

	inline TUint GetUInt();
	// MSdpElementBuilder interface definition
	// Most functions are errors, so not implemented
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
private:
	TUint iValue;
	};

inline TUint CRecordAttr::GetUInt()
	{
	return iValue;
	}




#endif
