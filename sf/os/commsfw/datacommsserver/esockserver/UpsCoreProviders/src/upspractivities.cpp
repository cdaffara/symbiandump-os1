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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalTechnology
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/mobilitycpractivities.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_activities.h>
#include <comms-infras/upsprstates.h>
#include <comms-infras/upspractivities.h>
#include <comms-infras/upsmessages.h>

using namespace MeshMachine;
using namespace NetStateMachine;
using namespace ESock;

namespace UpsPrPolicyCheckRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityPolicyCheckRequest, UpsPrPolicyCheckRequest, UpsMessage::TPolicyCheckRequest, CNodeParallelActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(UpsStates::TAwaitingPolicyCheckRequest, UpsStates::TControlOrDataClientOriginator)
	NODEACTIVITY_ENTRY(UpsStates::KUpsControlClientOriginator, UpsStates::TPostToServiceProvider, UpsStates::TAwaitingPolicyCheckResponse, TNoTag)
	NODEACTIVITY_ENTRY(UpsStates::KUpsDataClientClientOriginator, UpsStates::TPostToControlProvider, UpsStates::TAwaitingPolicyCheckResponse, TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TPostToOriginators)
NODEACTIVITY_END()
}

namespace UpsActivities
{
//Activity Map provided by UPS CorePr to be used by SCprs.
//(it is further extended in CoreSCpr).
DEFINE_EXPORT_ACTIVITY_MAP(upsActivitiesSCpr)
	ACTIVITY_MAP_ENTRY(UpsPrPolicyCheckRequestActivity, UpsPrPolicyCheckRequest)	// UPS
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)

//Activity Map provided by UPS CorePr to be used by Cprs.
//(it is further extended in CoreCpr).
DEFINE_EXPORT_ACTIVITY_MAP(upsActivitiesCpr)
	ACTIVITY_MAP_ENTRY(UpsPrPolicyCheckRequestActivity, UpsPrPolicyCheckRequest)	// UPS
ACTIVITY_MAP_END_BASE(MobilityCprActivities, mobilityCprActivities)
} //UpsPRActivities


