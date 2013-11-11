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
* Implementation of CSsmPsKeyObserverExactValue class.
*
*/

#include "ssmpskeyobserverexactvalue.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmPsKeyObserverExactValue::CSsmPsKeyObserverExactValue
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserverExactValue::CSsmPsKeyObserverExactValue(
    TUid aCategory,
    TUint aKey,
    TInt aTargetValue )
  : CSsmPsKeyObserver( aCategory, aKey ),
    iTargetValue( aTargetValue )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserverExactValue::~CSsmPsKeyObserverExactValue
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserverExactValue::~CSsmPsKeyObserverExactValue()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserverExactValue::IsMatch
//
// ---------------------------------------------------------------------------
//
TBool CSsmPsKeyObserverExactValue::IsMatch( const TInt aKeyValue ) const
    {
    FUNC_LOG;

    return aKeyValue == iTargetValue;
    }
