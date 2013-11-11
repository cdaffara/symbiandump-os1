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
 * CTVOutConfigForHDMI class declaration.
 *
 */

#ifndef MYASYNCONESHOTTIMER_H
#define MYASYNCONESHOTTIMER_H

// INCLUDES
#include <e32base.h>
#include "myasynconeshottimercallback.h"



// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 *  Asynchronous one shot timer implementation.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CMyAsyncOneShotTimer ): public CAsyncOneShot
    {
    public:
        CMyAsyncOneShotTimer( TInt aPriority,
                MMyAsyncOneShotTimerCallback& aAsyncOneShotTimerCallback );
        ~CMyAsyncOneShotTimer();
    private:
        void RunL();
        void DoCancel();
        
    private: // Data
        MMyAsyncOneShotTimerCallback& iAsyncOneShotTimerCallback;
    };

#endif // MYASYNCONESHOTTIMER_H
