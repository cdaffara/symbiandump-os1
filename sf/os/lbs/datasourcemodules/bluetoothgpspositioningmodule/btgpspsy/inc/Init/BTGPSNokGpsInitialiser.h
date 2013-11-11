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




#ifndef BTGPSNOKGPSINITIALISER_H
#define BTGPSNOKGPSINITIALISER_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSMessageListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;
class CBTGPSSettingManager;

// CLASS DECLARATION

/**
*  This class defines object that initialise BT GPS device that support Nokia proprietary 
*  feature. This is a self destruction class.
*  
*/
class CBTGPSNokGpsInitialiser: public CBase, private MBTGPSMessageListener
    {
    public: 

        /**
        * Two-phase construction.
		* @param aConnectManager Reference to connect manager
		* @param aSettingManager Reference to setting manager
		* @param aOwner Reference to owner pointer. The owner pointer will be nullified
		*               when this object is destructed.
        */
        static CBTGPSNokGpsInitialiser * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSSettingManager& aSettingManager,
            CBTGPSNokGpsInitialiser*& aOwner);

        /**
        * Two-phase construction. Same as NewL function, unless it won't leave.
        * @return NULL If the object can't be constructed.
        */
        static CBTGPSNokGpsInitialiser * New(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSSettingManager& aSettingManager,
            CBTGPSNokGpsInitialiser*& aOwner);

        /**
        * Destructor
        */
        virtual ~CBTGPSNokGpsInitialiser();
    
    private:
        /**
        * From MBTGPSMessageListener
        */
        virtual void HandleMessage(const TBTGPSNmeaParser& aParser);
           
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSNokGpsInitialiser(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSSettingManager& aSettingManager,
            CBTGPSNokGpsInitialiser*& aOwner);
            
    private:
        //Connect Manager
        CBTGPSConnectManager& iConnectManager;
        
        //Setting manager
        CBTGPSSettingManager& iSettingManager;
        
        //The owner pointer
        CBTGPSNokGpsInitialiser*& iOwner;
        
        //Init step
        TInt iStep;

    };
#endif
// End of File

