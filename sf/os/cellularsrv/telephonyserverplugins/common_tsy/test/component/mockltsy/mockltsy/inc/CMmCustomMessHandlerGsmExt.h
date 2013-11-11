// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef CMMCUSTOMMESSHANDLERGSMEXT_H
#define CMMCUSTOMMESSHANDLERGSMEXT_H

//  INCLUDES
#include "cmmmessagerouter.h"
#include "mmockmesshandlerbase.h"
#include <ctsy/rmmcustomapi.h>

//  FORWARD DECLARATIONS
class CMmMessageRouter;
class CMmCustomStubExt;

// CLASS DECLARATION

/**
 * Dummy functionality as Stub.
 */
class CMmCustomMessHandlerGsmExt : public CBase, public MMockMessHandlerBase
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMessageManager Pointer to message manager
         */
        static CMmCustomMessHandlerGsmExt* NewL( 
            CMmMessageRouter* aMessageManager );

        /**
         * Destructor.
         */
        virtual ~CMmCustomMessHandlerGsmExt();

        // Functions from base classes
        
        /**
         * From MMmMessHandlerBase. This method is the single entry point for
         * requests coming from the Symbian OS layer to this message handler
         *          
         * @param aIpc IPC number of the request
         * @return Error value
         */
        virtual TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

  		virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

    private:

        /**
         * C++ default constructor.
         */
        CMmCustomMessHandlerGsmExt();

        /**
         * Initialises object attributes.
         */
        void ConstructL();

    private: // Data

        /*
         * Pointer to the Message Router
         * Own.
         */                  
        CMmMessageRouter* iMessageRouter;
    };

#endif // CMMCUSTOMMESSHANDLERGSMEXT_H

// End of File
 
