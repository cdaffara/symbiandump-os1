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
//

#ifndef __SUSADAPTATIONPLUGINLOADER_H__
#define __SUSADAPTATIONPLUGINLOADER_H__

#include <e32base.h>
#include <e32std.h> 
#include "ssmadaptation.h"

/**
 @internalComponent
 @released
 */

_LIT(KStateAdaptationPlugin, "stateadaptation.dll");
_LIT(KSimAdaptationPlugin, "simadaptation.dll");
_LIT(KRtcAdaptationPlugin, "rtcadaptation.dll");
_LIT(KMiscAdaptationPlugin, "miscadaptation.dll");
_LIT(KEmergencyCallRfAdaptationPlugin, "emergencycallrfadaptation.dll");

_LIT(KRefStateAdaptationPlugin, "stateadaptationref.dll");
_LIT(KRefSimAdaptationPlugin, "simadaptationref.dll");
_LIT(KRefRtcAdaptationPlugin, "rtcadaptationref.dll");
_LIT(KRefMiscAdaptationPlugin, "miscadaptationref.dll");
_LIT(KRefEmergencyCallRfAdaptationPlugin, "emergencycallrfadaptationref.dll");

class CSusAdaptationPluginLoader : public CBase
	{
public:

	static CSusAdaptationPluginLoader* NewL();
	~CSusAdaptationPluginLoader();

	MEmergencyCallRfAdaptation* CreateEmergencyCallRfAdaptationL();
	MStateAdaptation* CreateStateAdaptationL();
	MSimAdaptation* CreateSimAdaptationL();
	MMiscAdaptation* CreateMiscAdaptationL();
	MRtcAdaptation* CreateRtcAdaptationL();

private:
	CSusAdaptationPluginLoader();

private:
	RLibrary ilibEmergencyCallRfAdaptation;
	RLibrary ilibStateAdaptation;
	RLibrary ilibSimAdaptation;
	RLibrary ilibMiscAdaptation;
	RLibrary ilibRtcAdaptation;
	};

#endif //__SUSADAPTATIONPLUGINLOADER_H__
