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

#ifndef EXTRACTORVISITOR_H
#define EXTRACTORVISITOR_H

#include <e32base.h>
#include <btsdp.h>
#include "sdputil.h"
#include "MAttributeVisitor.h"
#include "DataEncoder.h"

class CSdpAttr;
class CSdpServRecord;
class CSdpAttrIdMatchList;

class MAttributeMatchHandler
	{
public:
	virtual void AttributeInMatchListL(CSdpAttr &aAttr)=0;
	};


NONSHARABLE_CLASS(CAttrExtractVisitor) : public CBase, private MAttributeVisitor, public MAttributeMatchHandler
	{
public:
	static CAttrExtractVisitor *NewLC(TElementEncoder aEncoder);
	static CAttrExtractVisitor *NewL(TElementEncoder aEncoder);
	~CAttrExtractVisitor();

	IMPORT_C static void EncodeAttributesL(TElementEncoder aEncoder, CSdpServRecord &aRec, CSdpAttrIdMatchList &aList);

private:
	CAttrExtractVisitor(TElementEncoder aEncoder);
	void ConstructL();



	// Implementation of MAttributeVisitor interface
    void VisitAttributeL(CSdpAttr &aAttribute);
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList &aList);
    void EndListL();

	// Implementation of MAttributeMatchHandler
	void AttributeInMatchListL(CSdpAttr &aAttr);

private:
	TElementEncoder iEncoder;
	};

#endif
