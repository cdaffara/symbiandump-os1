/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef CMMCUSTOMEXTINTERFACE_H
#define CMMCUSTOMEXTINTERFACE_H

// INCLUDES
#include "CMmCustomTsy.h"

// CLASS DECLARATION

/**
*  CMmCustomExtInterface is used to provide an extension interface for 
*  mode specific custom object extensions.
*
*/
class CMmCustomExtInterface : public CBase
    {
    public:

        /**
         * Destructor.
         */
        virtual ~CMmCustomExtInterface() { return; }

        /**
         * Returns remote alerting tone status, default implementation.
         *          
         * @param aStatus Pointer to remote alert tone status
         * return KErrNotSupported
         */
        virtual TInt GetRemoteAlertingToneStatus( 
            RMmCustomAPI::TRemoteAlertingToneStatus* /*aStatus*/ )
                { return KErrNotSupported; }

    protected:
        /**
         * Constructor.
         */
        CMmCustomExtInterface() { return; };

    private:

        // ConstructL creates any class attributes
        void ConstructL() { return; };

};

#endif  // CMMCUSTOMEXTINTERFACE_H

//  End of File

