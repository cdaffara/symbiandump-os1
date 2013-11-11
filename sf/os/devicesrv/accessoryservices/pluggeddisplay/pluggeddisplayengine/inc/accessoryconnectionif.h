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
 * Description:
 * CAccessoryControlIf class declaration.
 *
 */

#ifndef C_ACCESSORYCONTROLIF_H
#define C_ACCESSORYCONTROLIF_H

// INCLUDES
#include <e32base.h>
#include <AccessoryConnection.h>
#include <AccConGenericID.h>
#include "multifinitestatemachine.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MFSMBody;
class RAccessoryServer;
class RAccessoryConnection;


// CLASS DECLARATION
/**
 * Accessory Connection interface.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CAccessoryConnectionIf ): public CActive
    {
    public:  // Constructors and destructor

        /**
         * Symbian two phased constructor.
         * 
         * @since S60 TB9.2
         * @param aFSM A FSM.
         * @param aAccessoryServer Accessory Server.
         * @return CAccessoryConnectionIf*
         */
        static CAccessoryConnectionIf* NewL(
                MFSMForBody& aFSM,
                RAccessoryServer& aAccessoryServer );

        /**
        * Destructor.
        */
        virtual ~CAccessoryConnectionIf();
        
    public: // New methods

        /**
         * Get the Generic ID of connected accessory.
         * 
         * @since TB9.2
         * @param aAccPolGenericID Object where to copy actual values.
         * @return Error code. KErrNotFound if don't exist.
         */
        TInt GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID );
        
    private: // Functions from base classes

        /**
        * RunL()
        */
        void RunL();

        /**
        * DoCancel()
        */
        void DoCancel();

        /**
        * RunError()
        */
        TInt RunError( TInt aError );
        
    private:  // Constructors

        /**
        * C++ constructor.
        * @param aFSM A FSM.
        */
        CAccessoryConnectionIf( MFSMForBody& aFSM );

        /**
        * Symbian 2nd phase constructor.
        * 
        * @since S60 TB9.2
        * @param aAccessoryServer Accessory Server
        */
        void ConstructL(
                RAccessoryServer& aAccessoryServer );


    private:  // Data

        // A FSM.
        // Not own.
        MFSMForBody& iFSM;
        
        // Accessory Connection API
        RAccessoryConnection iAccessoryConnection;
        
        // Generic ID Array.
        TAccPolGenericIDArray   iGenericIDarray;
        
        // Id of the active (object) request 
        TInt iActiveRequestId;
        
        // Generic ID of connected cable
        TAccPolGenericID* iAccPolGenericIDPtr;
        
                        
    };

#endif // C_ACCESSORYCONTROLIF_H
