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


#ifndef SERVICESEARCHVISITOR_H
#define SERVICESEARCHVISITOR_H

#include <e32base.h>
#include <btsdp.h>
#include "SDPDatabase.h"
#include "MAttributeVisitor.h"
#include "mignorer.h"
#include <e32base_private.h>

class CSdpServRecord;
class MServiceSearchHandler
	{
public:
	virtual void RecordFoundL(CSdpServRecord& aMatchingRec)=0;
	};

class CEncodedVisitorAdapter;
	
/**
	Class service search visitor.
	Carries out the UUID search pattern search over the
	attribute tree.
	Will leave on a VisitAttributeValueL call, with KErrCompletion (!)
	when the final UUID in the pattern is matched.
	If the pattern is  null, it will never leave.
	If an attribute value visited is in encoded form, it builds a parser
	and sets itself as visitor.
**/
NONSHARABLE_CLASS(CServiceSearchVisitor) : public CBase, private MAttributeVisitor
	{
public:
	IMPORT_C static CServiceSearchVisitor* NewL(const CSdpSearchPattern& aPattern);
	IMPORT_C static CServiceSearchVisitor* NewLC(const CSdpSearchPattern& aPattern);
    ~CServiceSearchVisitor();

	IMPORT_C static void SearchDbL(CSdpDatabase &aDb, const CSdpSearchPattern &aPattern, MServiceSearchHandler& aObserver);

	void FoundUUIDL(const TUUID& aUUID);

private:
    CServiceSearchVisitor();
	void ConstructL(const CSdpSearchPattern& aPattern);

	void SearchRecordL(CSdpServRecord& aRec);

	// MAttributeVisitor interface
    void VisitAttributeL(CSdpAttr &aAttribute);
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList &aList);
    void EndListL();

private:
	const CSdpSearchPattern *iSearchPattern;
	CBitMapAllocator *iFoundIndexs;

	CEncodedVisitorAdapter* iAdapter;
	CElementParser* iParser;
	};

#endif
