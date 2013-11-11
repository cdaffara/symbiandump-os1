/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It is the class for active object for making calls.
*
*/


#ifndef CPHCLTEXTPHONE_H   
#define CPHCLTEXTPHONE_H   

//  INCLUDES
#include <e32base.h>
#include "CPhCltExtPhoneBase.h"
#include "RPhCltServer.h"
#include "RPhCltExtCall.h"


// FORWARD DECLARATIONS
class MPhCltExtPhoneObserver;


// CLASS DECLARATION

/**
*  It is the class for active object for making calls.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCltExtPhone )
    : public CPhCltExtPhoneBase        
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        *
        * @param aObserver It is the observer for the instance.
        * @return Pointer to created CPhCltExtPhone instance.
        */
        static CPhCltExtPhone* NewL( MPhCltExtPhoneObserver* aObserver );

        /**
        * Destructor.
        */
        ~CPhCltExtPhone(); 


    public: // From baseclass


        /**
        * From CPhCltExtPhoneBase, Makes a new call.
        *
        * @param aData The populated dialing data.
        */
        virtual void DialL( CPhCltExtPhoneDialData& aData );


    private:
    
        /**
        * C++ constructor.
        */
        CPhCltExtPhone( MPhCltExtPhoneObserver* aObserver );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();   

        /**
        * From CActive, it is called when object is active and 
        * request has been processed.
        */
        void RunL();

        /**
        * From CActive, calcels pending request.
        */
        void DoCancel();

        /**
        * Check that suitable preconditions have been satisified
        * before attempting a dial
        */
        void DialPreconditionCheckL();


    private:  // Data

        // Enumeration for the dialing operation modes.
        enum TMode 
            {
            EIdle,     // In idle state.
            EDialling  // In dialling state.
            };
        
        // Status information of the phone.
        // Not used. Just for BC.
        TPckgBuf< TInt > iPhoneStatus;

        // Call instance to perform dialing.
        RPhCltExtCall iCall;
        
        // Server that is used in dialing.
        RPhCltServer iServer;

        // Mode of the dialing operation.
        TMode iMode;    

    };

#endif      // CPHCLTEXTPHONE_H   
            
// End of File
