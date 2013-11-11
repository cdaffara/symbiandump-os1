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
// @file CTLbsAgpsHAIValidateStepBase.h
// 
//

#ifndef __CT_LBSAGPSHAIVALIDATE_STEPBASE_H__
#define __CT_LBSAGPSHAIVALIDATE_STEPBASE_H__


#include "ctlbsteststepparsingbase.h"

class CT_LbsAgpsHAIValidateStepBase : public CT_LbsTestStepParsingBase
{
public:
	virtual ~CT_LbsAgpsHAIValidateStepBase();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsAgpsHAIValidateStepBase();

};

#endif // __CT_LBSAGPSHAIVALIDATE_STEPBASE_H__
