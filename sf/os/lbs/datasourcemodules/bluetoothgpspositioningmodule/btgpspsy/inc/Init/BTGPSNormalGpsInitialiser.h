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




#ifndef BTGPSNORMALGPSINITIALISER_H
#define BTGPSNORMALGPSINITIALISER_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSMessageListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;

// CLASS DECLARATION

/**
*  This class defines object that intialise normal BT GPS device. 
*  This is a self destructive class
*  
*/
class CBTGPSNormalGpsInitialiser: public CBase
    {
    public: 

        /**
        * Two-phase construction.
		* @param aConnectManager Reference to connect manager
		* @param aOwner Refernce to owner pointer. The owner pointer will be 
		*        nullified when the object is destructed.
        */
        static CBTGPSNormalGpsInitialiser * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSNormalGpsInitialiser*& aOwner);

        /**
        * Two-phase non-leave construction. 
        */
        static CBTGPSNormalGpsInitialiser * New(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSNormalGpsInitialiser*& aOwner);

        /**
        * Destructor
        */
        virtual ~CBTGPSNormalGpsInitialiser();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        * @param aId Timer id.
        * @param aListener The timer event listener.
        */
        CBTGPSNormalGpsInitialiser(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSNormalGpsInitialiser*& aOwner);
            
        /**
        * Timer callback
        */
        void TimerTick();
        
        /**
        * Static timer callback
        */
        static TInt TimerCallback(TAny* aAny);

    private:
        //Connect Manager
        CBTGPSConnectManager& iConnectManager;
        
        //Owner
        CBTGPSNormalGpsInitialiser*& iOwner;
        
        //Timer
        CPeriodic* iTimer;
        
        //Init step
        TInt iStep;
        
        //Message counter
        TInt iCount;

    };
#endif
// End of File

