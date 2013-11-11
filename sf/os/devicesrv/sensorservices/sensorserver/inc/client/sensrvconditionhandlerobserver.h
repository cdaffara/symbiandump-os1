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
* Description:  Channel data observer.
*
*/


#ifndef MSENSRVCONDITIONHANDLEROBSERVER_H
#define MSENSRVCONDITIONHANDLEROBSERVER_H


#include <e32base.h>
#include "sensrvtypes.h"
#include "sensrvchannelconditionset.h"


/**
*  Channel condition observer. Callback interface to indicate when a condition is 
*  met.
* 
*  @lib SensrvClient.lib
*  @since S60 5.0  
*/
class MSensrvConditionHandlerObserver
    {
        public:
        /**
        * Notification about the condition met.
        *
        * @since S60 5.0
        * @param[in] aSetId Condition set id that was met.
        * @param[in] aValue The data value that caused meeting of this condition.
        */
        virtual void ConditionMet( TInt aSetId, 
                                   TDesC8& aValue ) = 0;

        /**
        * Condition listening failed.
        * If error was fatal, channel has also been closed and sensor server 
        * session terminated.
        * If error was minor, some condition meetings have potentially been missed.
        *
        * @since S60 5.0
        * @param[in] aError Error code.
        */
        virtual void ConditionError( TSensrvErrorSeverity aError ) = 0;

        /**
        * Reopen channel.
        * Data listening and property listening need restarting
        *
        * @since S60 5.0        
        */
        virtual void ReopenChannelForConditionListeningL() = 0;
    };

#endif //MSENSRVCONDITIONHANDLEROBSERVER_H
