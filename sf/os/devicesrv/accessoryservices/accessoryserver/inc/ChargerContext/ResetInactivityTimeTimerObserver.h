/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Callback interface for observer of CAccSrvResetInactivityTimeTimer object.
*
*/




#ifndef M_RESETINACTIVITYTIMETIMEROBSERVER_H
#define M_RESETINACTIVITYTIMETIMEROBSERVER_H


#include <e32def.h>


/**
 *  Callback interface for observer of CAccSrvResetInactivityTimeTimer object. 
 *
 *  This callback interface has to be implemented by observer class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MResetActivityTimeTimerObserver )
    {

public:
 
    /**
     * Function is called by timer watcher every time when timer elapses.
     *
     * @since S60 5.1
     * @param none. 
     * @return none
     */
    virtual void InactivityTimerTimeout() = 0;

    };


#endif // M_RESETINACTIVITYTIMETIMEROBSERVER_H
