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
* Description: Callback interface for observer of CCentralRepositoryWatch object.
*
*/




#ifndef M_CENTRALREPOSITORYOBSERVER_H
#define M_CENTRALREPOSITORYOBSERVER_H


#include <e32def.h>
#include <e32cmn.h>


/**
 *  Callback interface for observer of CCentralRepositoryWatch object. 
 *  This callback interface has to be implemented by observer class.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( MCentralRepositoryObserver )
    {

public:
 
    /**
     * Function is called by watcher every time when key value has changed
     * in CR.
     *
     *  @since TB 9.2
     * @param aRepositoryId Changed repository. 
     * @param aRepositoryId Changed key. 
     * @return none
     */
    virtual void CentRepKeyChanged(
            TUid    aRepositoryId,
            TUint32 aId ) = 0;

    /**
     * Function is called by property watcher when reading the integer property's
     * current value fails.
     *
     *  @since TB 9.2
     * @param aRepositoryId Changed repository. 
     * @param aRepositoryId Changed key. 
     * @param aError Error code:
     *      KErrAbort if in a transaction that has previously failed, 
     *      KErrPermissionDenied if caller fails capability check, 
     *      KErrNotFound if the setting does not exist, 
     *      KErrArgument if the setting exists but is not an integer, plus other system-wide error codes. 
     * . 
     * @return none
     */
    virtual void CentRepGetKeyValueFailed( 
            TUid    aRepositoryId,
            TUint32 aId,
            TInt aError ) = 0;

    };


#endif // M_CENTRALREPOSITORYOBSERVER_H

