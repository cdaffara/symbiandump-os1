/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCState class implementation.
*
*/


#include "tcstate.h"
#include "trace.h"

CTCState::CTCState( TCStateType aType,
    TCStateId aId,
    MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    iType( aType ),
    iId( aId ),
    iStatePool( aStatePool ),
    iTransactionHandler( aTransactionHandler ),
    iFlags( 0 )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCState::Type
// ----------------------------------------------------------------------------------
//
CTCState::TCStateType CTCState::Type() const
    {
    FUNC_LOG;
    
    return iType;
    }

// ----------------------------------------------------------------------------------
// CTCState::Id
// ----------------------------------------------------------------------------------
//
CTCState::TCStateId CTCState::Id() const
    {
    FUNC_LOG;
    
    return iId;
    }

// ----------------------------------------------------------------------------------
// CTCState::Set
// ----------------------------------------------------------------------------------
//
void CTCState::Set( TUint32 aFlag )
    {
    FUNC_LOG;
    
    iFlags |= aFlag;
    }

// ----------------------------------------------------------------------------------
// CTCState::Unset
// ----------------------------------------------------------------------------------
//
void CTCState::Unset( TUint32 aFlag )
    {
    FUNC_LOG;
    
    iFlags &=~ aFlag;
    }

// ----------------------------------------------------------------------------------
// CTCState::IsSet
// ----------------------------------------------------------------------------------
//
TBool CTCState::IsSet( TUint32 aFlag )
    {
    FUNC_LOG;
    
    if(iFlags & aFlag)    
        return ETrue;
    else
        return EFalse;
    }

// ----------------------------------------------------------------------------------
// CTCState::Clear
// ----------------------------------------------------------------------------------
//
void CTCState::Clear()
    {
    FUNC_LOG;
    
    iFlags = 0;
    }

// End of File
