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
#include <AccessoryControl.h>
#include <AccConGenericID.h>
#include "pdengine.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MFSMBody;
class RAccessoryServer;
class RAccessoryControl;

enum TAccessoryControlIf
    {
    EAccessoryControlIfNoRequestActive = 10,
    EAccessoryControlIfConnectRequested,
    EAccessoryControlIfDisconnectRequested
    };


// CLASS DECLARATION
/**
 * Accessory Control interface.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CAccessoryControlIf ): public CActive
    {
    public:  // Constructors and destructor

        /**
         * Symbian two phased constructor.
         * 
         * @since S60 TB9.2
         * @param aFSM A FSM.
         * @param aAccessoryServer Accessory Server.
         * @return CAccessoryControlIf*
         */
        static CAccessoryControlIf* NewL(
                MFSMForBody& aFSM,
                RAccessoryServer& aAccessoryServer );

        /**
        * Destructor.
        */
        virtual ~CAccessoryControlIf();
        
    public: // New methods

        /**
         * Sends a connect accessory request to Accessory Server.
         * 
         * @since TB9.2
         * @param aCopyProtectionStatus Current Copy Protection Status.
         * @return void.
         */
        void ConnectAccessoryL( TBool aCopyProtectionStatus );

        /**
         * Sends a disconnect accessory request to Accessory Server.
         * 
         * @since TB9.2
         * @param None.
         * @return void.
         */
        void DisconnectAccessory();
        
        /**
         * Get the Unique ID of connected accessory.
         * 
         * @since TB9.2
         * @param None.
         * @return Unique ID. Zero if don't exist.
         */
        TInt GetUniqueID();

        /**
         * Get the Generic ID of connected accessory.
         * 
         * @since TB9.2
         * @param aAccPolGenericID Object where to copy actual values.
         * @return Error code. KErrNotFound if don't exist.
         */
        TInt GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID );

        /**
         * Notify value change.
         * 
         * @since TB9.2
         * @param aNameRecord Accessory capability.
         * @param aValue      Value for the specific accessory capability.
         * @return TInt Error code. system-wide error code if failed.
         *            Specifically:
         *            KErrArgument if an argument is out of range.
         */
        TInt NotifyValueChange( 
                const TUint32& aName,
                const TAccValueTypeTBool aValue ) const;

        /**
         * Notify value change.
         * 
         * @since TB9.2
         * @param aNameRecord Accessory capability.
         * @param aValue      Value for the specific accessory capability.
         * @return TInt Error code. system-wide error code if failed.
         *            Specifically:
         *            KErrArgument if an argument is out of range.
         */
        TInt NotifyValueChange( 
                const TUint32& aName,
                const CAccPolObjectCon& aValue ) const;
        
    private: // Functions from base classes

        /**
        * RunL()
        */
        void RunL();

        /**
        * DoCancel()
        */
        void DoCancel();

    private:  // Constructors

        /**
        * C++ constructor.
        * @param aFSM A FSM.
        */
        CAccessoryControlIf( MFSMForBody& aFSM );

        /**
        * Symbian 2nd phase constructor.
        * 
        * @since S60 TB9.2
        * @param aAccessoryServer Accessory Server
         */
        void ConstructL(
                RAccessoryServer& aAccessoryServer );


    private:  // Data

        // A FSM pointer.
        // Not own.
        MFSMForBody& iFSM;
        
        // Accessory Control API
        RAccessoryControl iAccessoryControl;
        
        // Generic ID pointer.
        // Own.
        CAccConGenericID* iGenericIDPtr;
        
        // Id of the active (object) request 
        TInt iActiveRequestId;

        // Indicates whether Composite cable has been
        // connected by calling RAccessoryControl::ConnectAccessory()
        TBool iAccessoryConnected; 
    };

#endif // C_ACCESSORYCONTROLIF_H
