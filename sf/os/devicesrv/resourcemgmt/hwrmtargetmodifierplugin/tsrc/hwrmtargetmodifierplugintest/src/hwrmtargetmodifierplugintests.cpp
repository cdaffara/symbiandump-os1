/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*  %version:  bh1dsgas#3 %, % date_modified: % by %derived_by: ravindrr %
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32debug.h>
#include <stiftestinterface.h>
#include <hwrmtargetmodifierplugin.h>
#include <hwrmlight.h>
#include "hwrmtargetmodifierplugintest.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

_LIT8( KLightMatch, "HWRMLight" );

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::CreateAndDestroy
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::CreateAndDestroy(TTestResult& aResult)
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"), 
        _L("CHwrmTargetModifierPluginTests::CreateAndDestroy"));

    aResult.iResult = KErrNone;
    CHWRMTargetModifierPlugin* plugin = NULL;
    TRAPD( err, plugin = CHWRMTargetModifierPlugin::NewL( KLightMatch ) );
    delete plugin;
    RDebug::Print( _L("CHwrmTargetModifierPluginTests::CreateAndDestroy err %d"), err );
    if ( err != KErrNone )
        {
        aResult.iResultDes.Copy( _L("CreateAndDestroy() failed") );
        aResult.iResult = err;
        }
    REComSession::FinalClose();
    return KErrNone;
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::GetFullSystemTarget
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::GetFullSystemTarget(TTestResult& aResult)
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"), 
        _L("CHwrmTargetModifierPluginTests::GetFullSystemTarget"));

    aResult.iResult = KErrNone;
    CHWRMTargetModifierPlugin* plugin = NULL;
    TRAPD( err, plugin = CHWRMTargetModifierPlugin::NewL( KLightMatch ) );
    TInt target = plugin->GetFullSystemTarget();
    delete plugin;
    RDebug::Print( _L("CHwrmTargetModifierPluginTests::GetFullSystemTarget target %d err %d"), target, err );
    if ( target && err != KErrNone )
        {
        aResult.iResultDes.Copy( _L("GetFullSystemTarget() failed") );
        aResult.iResult = err;
        }
    REComSession::FinalClose();
    return KErrNone;
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::ModifyTarget
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::ModifyTarget(TTestResult& aResult)
    {
    TestModuleIf().Printf( 0, _L("RunTestCase"), 
        _L("CHwrmTargetModifierPluginTests::ModifyTarget"));

    aResult.iResult = KErrNone;
    CHWRMTargetModifierPlugin* plugin = NULL;
    TRAPD( err, plugin = CHWRMTargetModifierPlugin::NewL( KLightMatch ) );
    if ( err == KErrNone )
        {
        TInt in( CHWRMLight::ESystemTarget );
        RDebug::Print( _L("CHwrmTargetModifierPluginTests::ModifyTarget in %d"), in );
        TInt out( 0 );
        TRAP( err, out = plugin->ModifyTargetL( in ) );
        RDebug::Print( _L("CHwrmTargetModifierPluginTests::ModifyTarget out %d"), out );
        }
    delete plugin;
    RDebug::Print( _L("CHwrmTargetModifierPluginTests::ModifyTarget err %d"), err );
    if ( err != KErrNone )
        {
        aResult.iResultDes.Copy( _L("ModifyTarget() failed") );
        aResult.iResult = err;
        }
    REComSession::FinalClose();
    return KErrNone;
    }
