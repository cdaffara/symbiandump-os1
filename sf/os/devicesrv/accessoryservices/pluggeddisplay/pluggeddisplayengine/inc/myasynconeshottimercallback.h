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
* Description: Callback interface for CAsyncOneShotTimer derived class.
*
*/

#ifndef M_MYASYNCONESHOTTIMERCALLBACK_H
#define M_MYASYNCONESHOTTIMERCALLBACK_H


#include <e32def.h>
#include <e32cmn.h> 
#include <e32des8.h>



// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Callback interface for CAsyncOneShotTimer derived class.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( MMyAsyncOneShotTimerCallback )
    {
public:
    /**
     * This is called when one shot asyncronous timer has elapsed.
     * 
     * @since TB9.2
     * @param  None.
     * @return void.
     *           
     */
    virtual void OneShotTimeout() = 0;
    };


#endif // M_MYASYNCONESHOTTIMERCALLBACK_H
