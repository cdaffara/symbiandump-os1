// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsmtlrserver.h
// This contains the header file for LBS MTLR Test Step Base
// 
//

#ifndef __CT_LBS_MTLR_STEP_H__
#define __CT_LBS_MTLR_STEP_H__

#include "ctlbsnetsimstep.h"
#include "ctlbsmtlrserver.h"


class CT_LbsMTLRStep : public CT_LbsNetSimStep
{

public:
	virtual ~CT_LbsMTLRStep();

protected:
	CT_LbsMTLRStep(CT_LbsMTLRServer& aParent);

	void ConstructL();

	
	CT_LbsMTLRServer& iParent;
};

#endif // __CT_LBS_MTLR_STEP_H__
