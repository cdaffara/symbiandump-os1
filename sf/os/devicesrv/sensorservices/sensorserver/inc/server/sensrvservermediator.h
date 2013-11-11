/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediates control flow from plugin thread to server main thread
*
*/


#ifndef SENSRVSERVERMEDIATOR_H
#define SENSRVSERVERMEDIATOR_H

#include <e32base.h>
#include "sensrvmediatorbase.h"

class CSensrvTransaction;

/**
 * Mediates control flow from plugin thread to sensor server
 * main thread (aka server thread).
 *
 * @since S60 5.0
 */
class CSensrvServerMediator : public CSensrvMediatorBase
    {
    public:
        
        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aProxy The proxy controlling related plugin.
        * @return New CSensrvServerMediator instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvServerMediator* NewL(CSensrvPluginProxy* aProxy);

        /**
        * Destructor.
        */
        virtual ~CSensrvServerMediator();
        
        
    protected: // from base classes
        
        /**
        * Handles transaction. Handling depends on mediator type,
        * so this must be implemented in inheriting class.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be handled.
        */
        void HandleTransaction(CSensrvTransaction* aTransaction);
                    
        /**
        * Handles notify failure
        *
        * @since S60 5.0
        */
        void HandleNotifyFailure();

    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSensrvServerMediator();

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL(CSensrvPluginProxy* aProxy);

    private: // Member variables

        // None?    
    };



#endif // SENSRVSERVERMEDIATOR_H
