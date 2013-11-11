// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core PR Activities related to User Prompt Service (UPS)
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_UPSPRACTIVITIES_H
#define SYMBIAN_UPSPRACTIVITIES_H

// @TODO PREQ1116 - do we need all of these ?

#include <comms-infras/ss_coreprstates.h>
//#include <comms-infras/ss_mmnode.h>
//#include <comms-infras/ss_coreactivitymutexpolicies.h>

namespace UpsActivities
{
	//Activity Map provided by UPS CorePr to be used by SCprs.
	//(it is further extended in CoreSCpr).
	DECLARE_EXPORT_ACTIVITY_MAP(upsActivitiesSCpr)

	//Activity Map provided by UPS CorePr to be used by Cprs.
	//(it is further extended in CoreCpr).
	DECLARE_EXPORT_ACTIVITY_MAP(upsActivitiesCpr)
}


#endif //SYMBIAN_UPSPRACTIVITIES_H

