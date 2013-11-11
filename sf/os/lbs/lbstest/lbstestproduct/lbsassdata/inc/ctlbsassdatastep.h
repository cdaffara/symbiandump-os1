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
// @file ctlbsassdataserver.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_ASS_DATA_STEP_H__
#define __CT_LBS_ASS_DATA_STEP_H__

#include "ctlbsnetsimstep.h"
#include "ctlbsassdataserver.h"


class CT_LbsAssDataStep : public CT_LbsNetSimStep
{

public:
	virtual ~CT_LbsAssDataStep();

protected:
	CT_LbsAssDataStep(CT_LbsAssDataServer& aParent);

	void ConstructL();

	
	CT_LbsAssDataServer& iParent;
};

#endif // __CT_LBS_ASS_DATA_STEP_H__
