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
* Description:  It is the client side remote class of call subsession object.
*
*/


#ifndef RPHCLTEXTCALL_H
#define RPHCLTEXTCALL_H

//  INCLUDES
#include <e32std.h>
#include "PhCltTypes.h"
#include "RPhCltServer.h"

// FORWARD DECLARATIONS
class CPhCltExtPhoneDialData;

// CLASS DECLARATION

/**
*  It is the client side remote class of call subsession object.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltExtCall : public RSubSessionBase
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        RPhCltExtCall();
   
        
    public:  // New functions

        /**
         * Opens the subsession.
         *
         * @param aServer It is the session of which subsession this is.
         * @return Returns error code.
         */       
        TInt Open( RPhCltServer& aServer );

        /**
         * Closes the subsession.
         */       
        void Close();
            	
        /**
        * Makes a call (asynchronous).
        *
        * @param aStatus It is used to notify the completion of the request. 
        *                Then it indicates the success/failure of the operation.
        * @param aData The object containing the dial parameters
        * @param aLength dial data package length
        */       
        void Dial( 
            TRequestStatus& aStatus, 
            TDesC8& aData, TInt aLength );

        /**
        * Cancels the dialling request.
        */       
        void CancelDial() const;

    private:
                
        // By default, prohibit copy constructor
        RPhCltExtCall( const RPhCltExtCall& );
        // Prohibit assigment operator
        RPhCltExtCall& operator= ( const RPhCltExtCall& );
        
	

       
   
    private: // Data

        

    };
#endif      // RPHCLTEXTCALL_H   
            
// End of File
