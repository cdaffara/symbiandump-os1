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


#ifndef SENSRVCLIENT_H
#define SENSRVCLIENT_H

#include <e32base.h>

#include "sensrvtypes.h"
#include "sensrvclientserver.h"


class RSessionBase;


/**
*  Base class of the client-side handle to a session with the Sensor Server.
*
*  @lib sensrvclient.lib
*  @since S60 5.0
*/
class RSensrvClient : public RSessionBase
    {
    public: // New functions

        /**
        * Connects to server
        *
        * @since S60 5.0
        * @return KErrNone if connect succesfully, otherwise system wide error
        */
        TInt Connect();


        /**
        * Send message to server
        *
        * @since S60 5.0
        * @param aRequest The function number identifying the request.
        * @param aArgs A set of up to arguments
        * @return KErrNone if connect succesfully, otherwise system wide error
        */
        TInt SendReceiveSync( TSensrvServerRequest aRequest, const TIpcArgs& aArgs ) const;

        /**
        * Send asynchronous message to server
        *
        * @since S60 5.0
        * @param aRequest The function number identifying the request.
        * @param aArgs A set of up to arguments
        * @param aStatus The request status object used to contain the completion status of the request.
        * @return void
        */
        void SendReceiveAsync( TSensrvServerRequest aRequest, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const;

    private:

        /**
        * Return server location
        * @since S60 5.0
        * @return location of server
        */
         TFullName ServerLocation() const;

        /**
        * Return version of server
        * @since S60 5.0
        * @return version of server
        */
         TVersion ServerVersion() const;

        /**
        * Starts server
        * @since S60 5.0
        * @return KErrNone if started succesfully, otherwise system wide error
        */
        TInt StartServer() const;

    };

#endif      // SENSRVCLIENT_H

// End of File
