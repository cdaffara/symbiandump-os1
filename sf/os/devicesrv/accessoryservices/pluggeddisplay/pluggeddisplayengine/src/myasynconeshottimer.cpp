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
 * CMyAsyncOneShotTimer class implementation.
 *
 */


// INCLUDE FILES
#include <e32base.h>
#include "myasynconeshottimercallback.h"
#include "myasynconeshottimer.h"
#include "trace.h"

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


// ----------------------------------------------------------------------------
// CMyAsyncOneShotTimer::CMyAsyncOneShotTimer
//
// ----------------------------------------------------------------------------
//
CMyAsyncOneShotTimer::CMyAsyncOneShotTimer(
        TInt aPriority,
        MMyAsyncOneShotTimerCallback& aAsyncOneShotTimerCallback ):
    CAsyncOneShot( aPriority ),
    iAsyncOneShotTimerCallback( aAsyncOneShotTimerCallback )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CMyAsyncOneShotTimer::~CMyAsyncOneShotTimer()
//
// ----------------------------------------------------------------------------
//
CMyAsyncOneShotTimer::~CMyAsyncOneShotTimer()
    {
    FUNC_LOG;
    Cancel();
    }

// ----------------------------------------------------------------------------
// CMyAsyncOneShotTimer::DoCancel()
//
// ----------------------------------------------------------------------------
//
void CMyAsyncOneShotTimer::DoCancel()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CMyAsyncOneShotTimer::RunL()
//
// ----------------------------------------------------------------------------
//
void CMyAsyncOneShotTimer::RunL()
    {
    FUNC_LOG;
    iAsyncOneShotTimerCallback.OneShotTimeout();
    }
