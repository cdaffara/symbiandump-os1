/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of CSsmSystemCommandsBase class.
*
*/

#include "ssmsystemcommandsbase.h"
#include "ssmmapperutility.h"
#include "trace.h"

#include <e32property.h>
// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSsmSystemCommandsBase::~CSsmSystemCommandsBase
// ---------------------------------------------------------------------------
//
CSsmSystemCommandsBase::~CSsmSystemCommandsBase()
    {
    FUNC_LOG;
    delete iMapperUtility;
    }


// ---------------------------------------------------------------------------
// CSsmSystemCommandsBase::CSsmSystemCommandsBase
// ---------------------------------------------------------------------------
//
CSsmSystemCommandsBase::CSsmSystemCommandsBase()
    {
    
    }

// ---------------------------------------------------------------------------
// CSsmSystemCommandsBase::DefineL
// ---------------------------------------------------------------------------
//
void CSsmSystemCommandsBase::DefineL(
    TUid aCategory,
    TUint aKey,
    const TSecurityPolicy& aReadPolicy,
    const TSecurityPolicy& aWritePolicy )
    {
    FUNC_LOG;
    
    INFO_2( "Defining key 0x%08x::0x%08x", aCategory, aKey );

    TInt errorCode = RProperty::Define( MapperUtilityL().PsUid( aCategory ),
                                        aKey,
                                        RProperty::EInt,
                                        aReadPolicy,
                                        aWritePolicy );
    if ( errorCode == KErrAlreadyExists )
        {
        INFO_2( "Key 0x%08x::0x%08x already defined", aCategory, aKey );
        errorCode = KErrNone;
        }

    ERROR_2( errorCode, "Failed to define PS key 0x%08x::0x%08x", aCategory,
                                                                  aKey );
    User::LeaveIfError( errorCode );
    }

// ---------------------------------------------------------------------------
// CSsmSystemCommandsBase::DefineL
// ---------------------------------------------------------------------------
//
void CSsmSystemCommandsBase::SetL( TUid aCategory, TUint aKey, TInt aValue )
    {
    FUNC_LOG;
    INFO_3( "Setting key 0x%08x::0x%08x value %d", aCategory, aKey, aValue );

    TInt errorCode = RProperty::Set( MapperUtilityL().PsUid( aCategory ),
                                     aKey,
                                     aValue );
    ERROR_2( errorCode, "Failed to set PS key 0x%08x::0x%08x", aCategory,
                                                               aKey );
    User::LeaveIfError( errorCode );
    }
// ---------------------------------------------------------------------------
// CSsmSystemCommandsBase::MapperUtilityL
// ---------------------------------------------------------------------------
//
CSsmMapperUtility& CSsmSystemCommandsBase::MapperUtilityL()
    {
    FUNC_LOG;
    if ( !iMapperUtility )
        {
        iMapperUtility = CSsmMapperUtility::NewL();
        }
    return *iMapperUtility;
    }
