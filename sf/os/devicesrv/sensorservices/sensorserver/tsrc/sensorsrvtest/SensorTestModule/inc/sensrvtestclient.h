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
* Description:  Sensor server client side connector class
*
*/


#ifndef SENSRVTESTCLIENT_H
#define SENSRVTESTCLIENT_H

// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class RSessionBase;



// CLASS DECLARATION

/**
*  Client for sensrvtest framework.
*
*  @since S60 5.0
*/
class RSensrvTestClient : public RSessionBase
    {
    public: // New functions

        /**
        * Connects to server
        *
        * @since S60 5.0
        * @return KErrNone if connect succesfully, otherwise system wide error
        */
        TInt Connect();
        
        TInt ShutdownServer();
        
        /**
        * Starts server
        * @since S60 5.0
        * @return KErrNone if started succesfully, otherwise system wide error
        */
        TInt StartServer() const;
        

    private:

        /**
        * Return version of server
        * @since S60 5.0
        * @return version of server
        */
         TVersion ServerVersion() const;
         
         /**
        * Return server location
        * @since S60 5.0
        * @return location of server
        */
         TFullName ServerLocation() const;
         

    };

#endif      // SENSRVTESTCLIENT_H   
            
// End of File
