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




#ifndef BTGPSPAIRINGHANDLER_H
#define BTGPSPAIRINGHANDLER_H

//  INCLUDES
#include <e32base.h> 

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;
class RSocketServ;
class RSocket;

// CLASS DECLARATION

/**
*  Connection handler that control pairing operation.
*
*/
class CBTGPSPairingHandler: public CActive
    {
    public: 

        /**
        * Two-phase construction.
		* @param aManagerExt Reference to connect manaager.
        */
        static CBTGPSPairingHandler * NewL(
            MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSPairingHandler();

    private:
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSPairingHandler(
            MBTGPSConnectManagerExt& aManagerExt);
        
    private:
        //Manager Ext
        MBTGPSConnectManagerExt& iManagerExt;
        
        //Socket server
        RSocketServ* iSocketServ;

        //Socket
        RSocket* iSocket;

    };
#endif

// End of File

