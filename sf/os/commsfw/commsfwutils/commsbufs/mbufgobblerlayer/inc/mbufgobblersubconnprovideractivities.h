// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Sub Connection Provider Mesh Activities
//  (control plane)
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

#ifndef MBUFGOBBLERSUBCONNPROVIDERACTIVITIES_H
#define MBUFGOBBLERSUBCONNPROVIDERACTIVITIES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <comms-infras/ss_activities.h>
#include <elements/mm_activities.h>

// CLASS DECLARATION

namespace MbufGobblerSCprActivities
{


enum 
	{
	/*
	 * Add new actviity id's here. I think they must be offset from ECFActivityCustom.
	 */
	
	/*
	 * This is just an example activity
	 */
	EMbufGobblerSCprSomeActivity = ESock::ECFActivityCustom + 1,
	};

DECLARE_ACTIVITY_MAP(MbufGobblerSCprActivitiesMap)
}

#endif // MBUFGOBBLERSUBCONNPROVIDERACTIVITIES_H
