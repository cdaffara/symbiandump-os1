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




#ifndef M_ACCSRVCENTRALREPOSITORYOBSERVER_H
#define M_ACCSRVCENTRALREPOSITORYOBSERVER_H


#include <e32def.h>
#include <e32cmn.h>


/**
 *  Callback interface for observer of CAccSvrCentralRepositoryObserver object. 
 *
 *  This callback interface has to be implemented by observer class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MAccSrvCentralRepositoryObserver )
    {

public:
 
    /**
     * Function is called by watcher every time when key value has changed
     * in CR.
     *
     * @since S60 5.1
     * @param aRepositoryId Changed repository. 
     * @param aRepositoryId Changed key. 
     * @param aRepositoryId Change value. 
     * @return none
     */
    virtual void CentRepKeyChanged(
            TUid    aRepositoryId,
            TUint32 aId,
            TInt    aValue) = 0;

    /**
     * Function is called by property watcher when reading the integer property's
     * current value fails.
     *
     * @since S60 5.1
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


#endif // M_ACCSRVCENTRALREPOSITORYOBSERVER_H

