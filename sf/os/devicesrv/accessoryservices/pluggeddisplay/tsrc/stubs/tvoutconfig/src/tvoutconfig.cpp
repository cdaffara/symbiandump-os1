/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file implements the needed functions defined in
                 tvoutconfig.h part of TVOutConfig Dummy Implementation.
*
*/


//- Include Files  ----------------------------------------------------------

#include "TvOutConfig.h"
#include "TvOutConfigImp.h"

//- External Data -----------------------------------------------------------

//- External Function Prototypes --------------------------------------------

//- Constants ---------------------------------------------------------------

//- Macros ------------------------------------------------------------------

//- Global and Local Variables ----------------------------------------------

//- Local Function Prototypes -----------------------------------------------

//- Local Functions ---------------------------------------------------------

//- Member Functions --------------------------------------------------------

//=============================================================================

//---------------------------------------------------------------------------
/**
    @see TvOutConfig.h
*/
//---------------------------------------------------------------------------
/**
    @see TvOutConfig.h
*/
EXPORT_C CTvOutConfig* CTvOutConfig::NewL()
    {

    CTvOutConfigImp* result = new ( ELeave ) CTvOutConfigImp;

    CleanupStack::PushL( result );
    result->ConstructL();
    CleanupStack::Pop();

    return result;
    }

//------------------------------------------------------------------------------
// CTvOutConfig::ApiVersion
//------------------------------------------------------------------------------
//
EXPORT_C TUint CTvOutConfig::ApiVersion()
    {
    return KTvOutConfigVersion;
    }

// End of File
