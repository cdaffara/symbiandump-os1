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
// @file ctlbsassdatastep.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsassdatastep.h"


CT_LbsAssDataStep::~CT_LbsAssDataStep()
	{
	}


CT_LbsAssDataStep::CT_LbsAssDataStep(CT_LbsAssDataServer& aParent) : CT_LbsNetSimStep(), iParent(aParent)
	{
	}

void CT_LbsAssDataStep::ConstructL()
	{
	// Create the base class objects.
	CT_LbsNetSimStep::ConstructL();
	}
