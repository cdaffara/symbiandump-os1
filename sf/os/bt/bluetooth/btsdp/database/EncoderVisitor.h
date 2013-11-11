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

#ifndef ENCODERVISITOR_H
#define ENCODERVISITOR_H

#include <e32base.h>
#include <btsdp.h>
#include "sdputil.h"
#include "MAttributeVisitor.h"
#include "DataEncoder.h"

class CSdpAttr;
class CSdpServRecord;

NONSHARABLE_CLASS(CAttrEncoderVisitor) : public CBase, private MAttributeVisitor
	{
public:
	IMPORT_C static CAttrEncoderVisitor *NewLC(TElementEncoder aEncoder);
	IMPORT_C static CAttrEncoderVisitor *NewL(TElementEncoder aEncoder);
	~CAttrEncoderVisitor();

	IMPORT_C static void EncodeAttributeL(TElementEncoder aEncoder, CSdpAttrValue &aSrcAttr);
	IMPORT_C void EncodeAttributeL(CSdpAttrValue &aSrcAttr);

private:
	CAttrEncoderVisitor(TElementEncoder aEncoder);
	void ConstructL();

	// Implementation of MAttributeVisitor interface
    void VisitAttributeL(CSdpAttr &aAttribute);
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList &aList);
    void EndListL();

private:
	TElementEncoder iEncoder;
	};


#endif
