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
// This is the class implementation for the Module Information Tests
// EPOC includes.sue
// 
//

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <e32math.h>
#include <e32property.h>
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"
#include "lbsdevloggermacros.h"

// LBS test includes.
#include "ctlbsmolrtracking.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

void CT_LbsMolrTracking::EnablePsy(TInt aPsyUid)
	{
	CPosModules* db = CPosModules::OpenL();
	CleanupStack::PushL(db);
	CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
	
	CPosModuleIdList* prioList = db->ModuleIdListLC();
	
	// Enable the PSY that came as an in parameter
	moduleUpdate->SetUpdateAvailability(ETrue);
	db->UpdateModuleL(TUid::Uid(aPsyUid), *moduleUpdate);
	
	CleanupStack::PopAndDestroy(prioList);
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(db);
}
