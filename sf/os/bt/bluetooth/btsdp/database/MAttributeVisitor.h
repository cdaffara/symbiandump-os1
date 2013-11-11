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

#ifndef MATTRIBUTEVISITOR_H
#define MATTRIBUTEVISITOR_H

#include "sdpconsts.h"
#include <btsdp.h>

class CSdpAttr;
class CSdpAttrValue;
class CSdpAttrValueList;

class MAttributeVisitor : public MSdpAttributeValueVisitor
	{
public:
    virtual void VisitAttributeL(CSdpAttr &aAttribute)=0;
	};

#endif
