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
#include "PSMPluginStubSecond.h"

// CONSTANT DEFINITIONS
const TUint32 KCenRepUid = 0x10200EE2;
_LIT8( KOriginalValue4, "SecondFour" );

enum TPsmPluginFirstKeys
    {
    EPsmPluginSecondKey1 = 11,
    EPsmPluginSecondKey2,
    EPsmPluginSecondKey3 = 23,
    EPsmPluginSecondKey4
    };

// ----------------------------------------------------------------------------------
// CPSMPluginSecond::CPSMPluginSecond()
// ----------------------------------------------------------------------------------
//  
CPSMPluginSecond::CPSMPluginSecond( TPsmPluginCTorParams& aInitParams ) :
    CPsmPluginBase( aInitParams )
	{	
	}

// -----------------------------------------------------------------------------
// CPSMPluginSecond::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPSMPluginSecond::ConstructL()
    {
    }
    
//
// ----------------------------------------------------------------------------------
// CPSMPluginSecond::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CPSMPluginSecond* CPSMPluginSecond::NewL( TPsmPluginCTorParams& aInitParams )
	{
	CPSMPluginSecond* self = new ( ELeave ) CPSMPluginSecond( aInitParams );

	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CPSMPluginSecond::~CPSMPluginSecond()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CPSMPluginSecond::~CPSMPluginSecond()
	{
    RDebug::Print( _L( "CPSMPluginSecond::~CPSMPluginSecond()" ) );
    }

//
// ----------------------------------------------------------------------------------
// CPSMPluginSecond::NotifyModeChangeL()
// ----------------------------------------------------------------------------------
//  
void CPSMPluginSecond::NotifyModeChange( const TInt aMode )
    {
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL()" ) );

    RConfigInfoArray infoArray;

    TPsmsrvConfigInfo info1;
    info1.iConfigId = EPsmPluginSecondKey1;
    info1.iConfigType = EConfigTypeInt;
    info1.iIntValue = 1;
    info1.iBlocked = ETrue;
    infoArray.Append( info1 );

    TPsmsrvConfigInfo info2;
    info2.iConfigId = EPsmPluginSecondKey2;
    info2.iConfigType = EConfigTypeReal;
    info2.iRealValue = 2.2;
    info2.iBlocked = EFalse;
    infoArray.Append( info2 );

    TPsmsrvConfigInfo info3;
    info3.iConfigId = EPsmPluginSecondKey3;
    info3.iConfigType = EConfigTypeInt;
    info3.iIntValue = 3;
    info3.iBlocked = EFalse;
    infoArray.Append( info3 );

    TPsmsrvConfigInfo info4;
    info4.iConfigId = EPsmPluginSecondKey4;
    info4.iConfigType = EConfigTypeBuf;
    info4.iDesValue.Append( KOriginalValue4 );
    info4.iBlocked = ETrue;
    infoArray.Append( info4 );

    // For printing desc. values
    TBuf16<256> buffer( KNullDesC );
    buffer.Copy( info4.iDesValue );

    // Print old values from array
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): oldValue info1: %d" ), infoArray[0].iIntValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info1: %i" ), infoArray[0].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): oldValue info2: %f" ), infoArray[1].iRealValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info2: %i" ), infoArray[1].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): oldValue info3: %d" ), infoArray[2].iIntValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info3: %i" ), infoArray[2].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): oldValue info4: %S" ), &buffer );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info4: %i" ), infoArray[3].iBlocked );


    TRAP_IGNORE( iSettingsProvider.BackupAndGetSettingsL( infoArray, KCenRepUid ) );

    buffer.Copy( infoArray[3].iDesValue );

    // Print new values from array
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): newValue info1: %d" ), infoArray[0].iIntValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info1: %i" ), infoArray[0].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): newValue info2: %f" ), infoArray[1].iRealValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info2: %i" ), infoArray[1].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): newValue info3: %d" ), infoArray[2].iIntValue );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info3: %i" ), infoArray[2].iBlocked );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): newValue info4: %S" ), &buffer );
    RDebug::Print( _L( "CPSMPluginSecond::NotifyModeChangeL(): blocked info4: %i" ), infoArray[3].iBlocked );

    infoArray.Reset();
    }


