/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Interface   : Private, Starter Configuration API.
* Private Central Repository definitions of Starter component.
*
*/



#ifndef STARTERPRIVATECRKEYS_H
#define STARTERPRIVATECRKEYS_H

#include <centralrepository.h>
#include <ssm/starterdomaincrkeys.h>

/*
* @publishedPartner
* @released
*/

// =============================================================================
// Starter Configuration API
// =============================================================================
//
static const TUid KCRUidStarterConf = { 0x2000B11D };
	
/**
* Used for determining if a component has been running for a long time or if it
* has been started just recently. This information is required when deciding
* whether to try to re-start the component if it dies for some reason.
* Key value is a time limit (in seconds) which determines whether the item
* has been started recently or a while ago.
*/
const TUint32 KStarterMonitoringTimeLimit = 0x00000001;

/**
* Enable / disable SIMless Offline Mode.
* Old Shared Data constant name: KCoreAppUIsSimlessStartup
*/
const TUint32 KStartupSimlessOffline = 0x00000002;
enum TSimlessOfflineMode
    {
    ESimlessOfflineDisabled = 0,
    ESimlessOfflineEnabled
    };

/**
* String that defines the path to first startup list extension resource file.
* Empty value indicates that there is no startup list extension resource file.
*/
const TUint32 KStartupListExtension1Path = 0x00000003;

/**
* String that defines the path to second startup list extension resource file.
* Empty value indicates that there is no startup list extension resource file.
*/
const TUint32 KStartupListExtension2Path = 0x00000004;

// =============================================================================
// Startup Adaptation Configuration API
// =============================================================================
//
// Use KCRUidStarterConf

const TInt KStartupAdaptationConfigurationAPIBase = 0x00000101;

/**
* Indicates the timeout for plug-in calls in milliseconds.
*/
const TUint32 KStartupPluginTimeout = KStartupAdaptationConfigurationAPIBase;

/**
* Indicates the amount of subsequent resets.
*/
const TUint32 KStartupResetCounter = KStartupAdaptationConfigurationAPIBase + 1;

/**
* Indicates the maximum amount of subsequent resets allowed.
*/
const TUint32 KStartupResetLimit = KStartupAdaptationConfigurationAPIBase + 2;

// =============================================================================
// SIM Language API
// =============================================================================
//
// Use KCRUidStartup

const TInt KSIMLanguageAPIBase = 0x00010001;

/**
* The selected SIM Language.
* Possible values: 0 (default, no selection) or one of TLanguage values.
* Used by automatic language selection to store the last selected language
* code.
*/
const TUint32 KStartupSimLanguage = KSIMLanguageAPIBase;

#endif // STARTERPRIVATECRKEYS_H

// End of File

