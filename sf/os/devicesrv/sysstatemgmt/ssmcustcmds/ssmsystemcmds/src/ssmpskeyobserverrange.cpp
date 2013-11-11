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
* Implementation of CSsmPsKeyObserverRange class.
*
*/

#include "ssmpskeyobserverrange.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmPsKeyObserverRange::CSsmPsKeyObserverRange
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserverRange::CSsmPsKeyObserverRange(
    TUid aCategory,
    TUint aKey,
    TInt aTargetLow,
    TInt aTargetHigh )
  : CSsmPsKeyObserver( aCategory, aKey ),
    iTargetLow( aTargetLow ),
    iTargetHigh( aTargetHigh )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserverRange::~CSsmPsKeyObserverRange
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserverRange::~CSsmPsKeyObserverRange()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserverRange::IsMatch
//
// ---------------------------------------------------------------------------
//
TBool CSsmPsKeyObserverRange::IsMatch( const TInt aKeyValue ) const
    {
    FUNC_LOG;

    return ( aKeyValue >= iTargetLow && aKeyValue <= iTargetHigh );
    }
