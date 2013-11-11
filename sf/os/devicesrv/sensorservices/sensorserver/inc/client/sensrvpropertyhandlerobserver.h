/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel property observer.
*
*/


#ifndef MSENSRVPROPERTYHANDLEROBSERVER_H
#define MSENSRVPROPERTYHANDLEROBSERVER_H

#include <e32base.h>
#include "sensrvproperty.h"
#include "sensrvtypes.h"


/**
 * Channel property observer. Callback interface to indicate when a property has been changed.
 *  
 * @lib SensrvClient.lib
 * @since S60 5.0 
 */

class MSensrvPropertyHandlerObserver
    {
    public:
    /**
    * Notification about the changed value of a property.
    *
    * @since S60 5.0
    * @param[in] aChangedProperty Changed property.
    */  
    virtual void PropertyChanged( const TSensrvProperty& aChangedProperty ) = 0;

    /**
    * Property listening failed.
    * If error was fatal, channel has also been closed and sensor server session terminated.
    * If error was minor, some property changes have potentially been missed.
    *
    * @since S60 5.0
    * @param[in] aError Error code. 
    */
    virtual void PropertyError( TSensrvErrorSeverity aError ) = 0;
    
    /**
    * Reopen channel.
    * Data listening and condition listening need restarting
    * 
    * @since S60 5.0    
    * @param aMaxBufferCount New maximum client buffer count
    * @return void 
    */
    virtual void ReopenChannelForPropertyListeningL() = 0;
    
    /**
    * Notification about change in property setting avaibility.
    *
    * @since S60 5.0
    * @param[in] aIndication holds the new indicator value.
    */
    virtual void SetPropertySuccessIndicationChanged( 
                 TSetPropertySuccessIndicator aIndication ) = 0;
    
    };

#endif //MSENSRVPROPERTYHANDLEROBSERVER_H
