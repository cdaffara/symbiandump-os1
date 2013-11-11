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
* Description:  Support class used to deliver accessory settings related
*        parameters.
*
*/


// INCLUDE FILES
#include "acc_debug.h"
#include <AccModeSettingArray.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAccModeSettingArray::TAccModeSettingArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccModeSettingArray::TAccModeSettingArray()
    {
    API_TRACE_( "[AccFW:Client] TAccModeSettingArray::TAccModeSettingArray()" );

    iIndex = 0;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::AddSetting
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSettingArray::AddSetting( const TAccModeSetting& aSetting )
    {
    API_TRACE_( "[AccFW:Client] TAccModeSettingArray::AddSetting()" );

    TInt err( KErrNone );

    // Make sure that same index isn't yet in use
    for( TInt i=0; i<GetArraySize(); ++i )
        {
        TAccModeSetting setting;
        GetSetting( i, setting );
        if( aSetting.Id() == setting.Id() )
            {
            err = KErrAlreadyExists;
            }
        }

    // Set new setting
    if( err == KErrNone )
        {
        if( iIndex < KMaxAccModeSettings )
            {
            iSettings[iIndex++] = aSetting;
            }
        else
            {
            // Array is full
            err = KErrNotSupported;
            }
        }

    API_TRACE_1( "[AccFW:Client] TAccModeSettingArray::AddSetting() - return %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::GetSetting
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSettingArray::GetSetting( const TInt aIndex,
                                                TAccModeSetting& aSetting ) const
    {
    API_TRACE_( "[AccFW:Client] TAccModeSettingArray::GetSetting()" );

    TInt err( KErrNone );

    // Get setting
    if( aIndex >= 0 && aIndex < KMaxAccModeSettings )
        {
        aSetting = iSettings[aIndex];
        }
    else
        {
        err = KErrNotSupported;
        }

    API_TRACE_1( "[AccFW:Client] TAccModeSettingArray::GetSetting() - return %d", err );

    return err;
    }
    
// -----------------------------------------------------------------------------
// TAccModeSetting::Reset
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccModeSettingArray::Reset()
	{
	API_TRACE_( "[AccFW:Client] TAccModeSettingArray::Reset()" );

	TAccModeSetting emptySetting;
	
	for( TInt i=0; i<KMaxAccModeSettings; ++i )
        {
		iSettings[i] = emptySetting;		
		}
		
	iIndex = 0;
	}

// -----------------------------------------------------------------------------
// TAccModeSetting::GetArraySize
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSettingArray::GetArraySize() const
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSettingArray::GetArraySize() - returns %d", iIndex );

    // iIndex points to next free array slot
    return iIndex;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::GetMaxArraySize
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSettingArray::GetMaxArraySize() const
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSettingArray::MaxArraySize() - return %d", KMaxAccModeSettings );

    return KMaxAccModeSettings;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File

