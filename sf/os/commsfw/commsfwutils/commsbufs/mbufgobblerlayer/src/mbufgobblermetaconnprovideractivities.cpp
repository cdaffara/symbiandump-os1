// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Meta Connection Provider Mesh Activities
//  (management plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#include "mbufgobblermetaconnprovideractivities.h"
#include "mbufgobblerlog.h"

#include <comms-infras/coremcpractivities.h>


namespace MbufGobblerMCprActivities
{
DEFINE_ACTIVITY_MAP(MbufGobblerMCprActivitiesMap)
	//no activities defiend here yet. This define is to show where to put activities
	//so for now just inherit from core activities
ACTIVITY_MAP_END_BASE(MCprActivities, coreMCprActivities)
}
