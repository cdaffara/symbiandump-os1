/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Internal storage definitions file
*
*/


#ifndef PSMSTORAGEDEFINES_H
#define PSMSTORAGEDEFINES_H

#include <e32base.h>
#include <gmxmldocument.h>
#include <gmxmlelement.h>

// ======== CONSTANTS =======
// Maximum attribute lenghth
const TInt KPsmMaxAttribLength = 160;

const TInt KPsmCurrentSetMode = 99;

const TInt KPsmMaxPathLength = 50;

// Storage files
_LIT( KPsmPrivateDrive, "z:" );
_LIT( KPsmBackupDrive, "c:" );
_LIT( KPsmBackupFolder, "backup\\" );
_LIT( KPsmCenRepFolder, "cenrep\\" );

_LIT( KPsmStorageExtension, ".xml" );

// TAG DEFINITIONS
_LIT8( KPsmConfigRootTag, "PowerSaveModeConfig" );           // PSM Config root tag
_LIT8( KPsmBackupRootTag, "PowerSaveModeBackup" );           // PSM Backup root tag

_LIT8( KPsmPluginGroupTag, "Setting" );                      // PSM Plugin group tag
_LIT8( KPsmPluginGroupAttribute, "mode" );                   // PSM Plugin group attribute

_LIT8( KPsmSetItem, "SetItem" );                             // SetItem tag
_LIT8( KPsmSetItemKey, "key" );                              // SetItem key attribute
_LIT8( KPsmSetItemType, "type" );                            // SetItem type attribute
_LIT8( KPsmSetItemValue, "value" );                          // SetItem value attribute
_LIT8( KPsmSetItemBlock, "block" );                          // SetItem block attribute

#endif // PSMSTORAGEDEFINES_H
