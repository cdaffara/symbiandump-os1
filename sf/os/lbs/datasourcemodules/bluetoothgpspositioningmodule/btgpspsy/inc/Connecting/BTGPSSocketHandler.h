/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSSOCKETHANDLER_H
#define BTGPSSOCKETHANDLER_H

//  INCLUDES
#include <e32base.h> 
#include <bt_sock.h>

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;

// CLASS DECLARATION

/**
*  Connection handler that controls Socket server and socket operation.
*
*/
class CBTGPSSocketHandler: public CActive
    {
    public: 

        /**
        * Two-phase construction.
		* @param aManagerExt Reference to connect manager.
        */
        static CBTGPSSocketHandler * NewL(
            MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSSocketHandler();

    
    private:
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError(TInt aError);

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSSocketHandler(
            MBTGPSConnectManagerExt& aManagerExt);
        
    private:
        //Manager Ext
        MBTGPSConnectManagerExt& iManagerExt;

        //Socket Server
        RSocketServ* iSocketServ;

        //Socket
        RSocket* iSocket;

    };
#endif
// End of File

