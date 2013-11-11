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




#ifndef BTGPSENVCHANGEHANDLER_H
#define BTGPSENVCHANGEHANDLER_H

//  INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSRequestManager;
class CEnvironmentChangeNotifier;

// CLASS DECLARATION

/**
*  Handler for getting system time change event. 
*  
*/
class CBTGPSEnvChangeHandler: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        * @param aRequestManager Reference to request manager.
        */
        static CBTGPSEnvChangeHandler * NewL(
            CBTGPSRequestManager& aRequestManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSEnvChangeHandler();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSEnvChangeHandler(
            CBTGPSRequestManager& aRequestManager);

        /**
        * Callback function for environment change notifier.
        */
        static TInt NotifierCallBack(TAny* aPtr);

        /**
        * This function is called whenever environment change notifier
        * reports that something has changed. The only change we are
        * interested of is the system time change.
        */
        void HandleEnvironmentChange();

    private:
        //Reference to request manager
        CBTGPSRequestManager& iRequestManager;

        //Environment change notifier
        CEnvironmentChangeNotifier* iEnvChangeNotifier;

        //Environment change initialized
        TBool iEnvChangeNotifierInitialized;
    };
#endif
// End of File

