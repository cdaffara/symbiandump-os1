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
* Description:  PS' integer property observer's callback interface.
*
*/




#ifndef M_ACCSRVPSINTPROPERTYOBSERVER_H
#define M_ACCSRVPSINTPROPERTYOBSERVER_H


#include <e32def.h>


/**
 *  Publish and Subscribe integer property observer's callback interface. 
 *
 *  This callback interface has to be implemented by observer class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MAccSrvPSIntPropertyObserver )
    {

public:
 
    /**
     * Function is called by property watcher every time property's value changes.
     *
     * @since S60 5.1
     * @param aCategory The UID that identifies the property category.
     * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
     * @param aNewStatus New property status value. 
     * @return none
     */
    virtual void PSPropertyChanged( 
            TUid    aCategory,
            TInt    aKey,
            TInt    aNewStatus ) = 0;

    /**
     * Function is called by property watcher when property is not found anymore.
     *
     * @since S60 5.1
     * @param aCategory The UID that identifies the property category.
     * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
     * @return none
     */
    virtual void PSPropertyDeleted(
            TUid    aCategory,
            TInt    aKey ) = 0;
    };


#endif // M_ACCSRVPSINTPROPERTYOBSERVER_H
