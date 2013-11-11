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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include <psmsettingsprovider.h>
#include "PSMPluginStubFirst.h"

// CONSTANT DEFINITIONS
const TUint32 KCenRepUid = 0x10200EE1;

enum TPsmPluginFirstKeys
    {
    EPsmPluginFirstKey1 = 1,
    EPsmPluginFirstKey2,
    EPsmPluginFirstKey3,
    EPsmPluginFirstKey4
    };

_LIT8( KOriginalValue4, "Four" );

//
// ----------------------------------------------------------------------------------
// CPSMPluginFirst::CPSMPluginFirst()
// ----------------------------------------------------------------------------------
//  
CPSMPluginFirst::CPSMPluginFirst( TPsmPluginCTorParams& aInitParams ) :
    CPsmPluginBase( aInitParams )
	{	
	}

// -----------------------------------------------------------------------------
// CPSMPluginFirst::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPSMPluginFirst::ConstructL()
    {
    }
    
//
// ----------------------------------------------------------------------------------
// CPSMPluginFirst::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CPSMPluginFirst* CPSMPluginFirst::NewL( TPsmPluginCTorParams& aInitParams )
	{
	CPSMPluginFirst* self = new ( ELeave ) CPSMPluginFirst( aInitParams );

	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CPSMPluginFirst::~CPSMPluginFirst()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CPSMPluginFirst::~CPSMPluginFirst()
	{
    RDebug::Print( _L( "CPSMPluginFirst::~CPSMPluginFirst()" ) );
	}

//
// ----------------------------------------------------------------------------------
// CPSMPluginFirst::NotifyModeChangeL()
// ----------------------------------------------------------------------------------
//  
void CPSMPluginFirst::NotifyModeChange( const TInt aMode )
    {
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL()" ) );

    RConfigInfoArray infoArray;

    TPsmsrvConfigInfo info1;
    info1.iConfigId = EPsmPluginFirstKey1;
    info1.iConfigType = EConfigTypeInt;
    info1.iIntValue = 1;
    infoArray.Append( info1 );

    TPsmsrvConfigInfo info2;
    info2.iConfigId = EPsmPluginFirstKey2;
    info2.iConfigType = EConfigTypeReal;
    info2.iRealValue = 2.2;
    infoArray.Append( info2 );

    TPsmsrvConfigInfo info3;
    info3.iConfigId = EPsmPluginFirstKey3;
    info3.iConfigType = EConfigTypeInt;
    info3.iIntValue = 3;
    infoArray.Append( info3 );

    TPsmsrvConfigInfo info4;
    info4.iConfigId = EPsmPluginFirstKey4;
    info4.iConfigType = EConfigTypeBuf;
    info4.iDesValue.Append( KOriginalValue4 );
    infoArray.Append( info4 );

    // For printing desc. values
    TBuf16<256> buffer( KNullDesC );
    buffer.Copy( info4.iDesValue );

    // Print old values from array
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): oldValue info1: %d" ), infoArray[0].iIntValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): oldValue info2: %f" ), infoArray[1].iRealValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): oldValue info3: %d" ), infoArray[2].iIntValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): oldValue info4: %S" ), &buffer );

    // Backup settings...
    TRAP_IGNORE( iSettingsProvider.BackupSettingsL( infoArray, KCenRepUid ) );

    // ...And get new ones
    TRAP_IGNORE( iSettingsProvider.GetSettingsL( infoArray, KCenRepUid ) );

    buffer.Copy( infoArray[3].iDesValue );

    // Print new values from array
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): newValue info1: %d" ), infoArray[0].iIntValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): newValue info2: %f" ), infoArray[1].iRealValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): newValue info3: %d" ), infoArray[2].iIntValue );
    RDebug::Print( _L( "CPSMPluginFirst::NotifyModeChangeL(): newValue info4: %S" ), &buffer );

    infoArray.Reset();
    }


