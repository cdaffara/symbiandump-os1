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
// Core SCPR Activities
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORESCPRACTIVITIES_H
#define SYMBIAN_CORESCPRACTIVITIES_H

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/corescprstates.h>

namespace SCprActivities
{

DECLARE_EXPORT_ACTIVITY_MAP(coreSCprActivities)

//-=========================================================
//
//Apply Activity
//
//-=========================================================
namespace Apply
    {
    enum TransitionTags
        {
            ESendApplyResponse
            };
    } //namespace Apply

} //namespace SCprActivities


#endif //SYMBIAN_CORESCPRACTIVITIES_H

