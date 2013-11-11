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
// @file ctlbsassdatashareddata.h
// 
//


#ifndef __T_LBS_ASS_DATA_SHARED_DATA_H__
#define __T_LBS_ASS_DATA_SHARED_DATA_H__

// system includes

// Lbs includes
#include "ctlbsshareddata.h"

class CT_LbsAssDataSharedData : public CT_LbsSharedData
{
public:
	static CT_LbsAssDataSharedData* NewL();
	CT_LbsAssDataSharedData();
	~CT_LbsAssDataSharedData();

protected:
	void ConstructL();

public:
	// Add any other things we may need.
};

#endif // __T_LBS_ASS_DATA_SHARED_DATA_H__
