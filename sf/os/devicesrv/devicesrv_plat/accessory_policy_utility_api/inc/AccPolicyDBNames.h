/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions of Accessory database handling.
*
*/


#ifndef ACCDBNAMES_H
#define ACCDBNAMES_H

// INCLUDES
#include <e32std.h>

// CONSTANTS

_LIT( KAccDbSecureFormat, "secure[10205030]" );

_LIT( KImportDBName, "c:\\system\\data\\DBS_10205030_accpolicydb.db" );

_LIT( KDBPath, "\\private\\100012a5\\DBS_10205030_accpolicydb.db" );

_LIT( KDBNameInROM, "z:accpolicydb.db" );

enum TAccGIDtype
    {    
    EAccNormal,     // This row is normal genericID in GIDInteger table.
    EAccRule,       // This row is contains one conection rule. 
    EAccSelection,  // This row is contains one generic id for UI selection.
    EAccUnknown     // This row is not used in DB. Type is not known and can be any of above.
    };

// GIDInteger table
//-----------------------------------------------------------------
_LIT( KACCDBGIDIntTable, "gIntTbl" );

// Device ID column of GIDInteger table
_LIT( KACCDBGIDIntDeviceIDCol, "gIntDEVCol" );

// GID Integer column of GIDInteger table
_LIT( KACCDBGIDIntGIDCol, "gIntGIDCol" );

// Type column of GIDInteger 
_LIT( KACCDBGIDIntKeyIDCol, "gIntKEYCol" );

// Capability rules table
//-----------------------------------------------------------------

// Capability rules table
_LIT( KACCDBCapRulesTable, "capRulesTbl" );

// Name column of CapRules table
_LIT( KACCDBCapRulesName, "capRulesNameCol" );

// Value column of CapRules table
_LIT( KACCDBCapRulesValue, "capRulesValueCol" );

// Selection table:
//-----------------------------------------------------------------

// Selection table
_LIT( KACCDBSelectionTable, "capSelectionTbl" );

// Selection index column of selection table
_LIT( KACCDBSelectionIndex, "capSelectionIndexCol" );

// Value column of selection table
_LIT( KACCDBCapSelectionValue, "capSelectionValueCol" );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // ACCDBNAMES_H

// End of File
