/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMPOWERSTATEPLUGINHANDLER_H
#define HWRMPOWERSTATEPLUGINHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "HWRMPowerStatePlugin.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATIONS

/**
*  Hardware Resource Manager server side power state plugin
*  handler implementation.
*
*/
class CHWRMPowerStatePluginHandler : public CBase,
                                     public MHWRMPowerStatePluginCallback
                                 
    {
    public: // Constructors and Destructor
    
        /**
        * Constructor method for instance.
        * @param aChargerBlockVibra ETrue if charger connection should block vibra
        * @param aChargerBlockFeedback ETrue if charger connection should block feedback vibration
        */
        static CHWRMPowerStatePluginHandler* NewL(TBool aChargerBlockVibra, 
            TBool aChargerBlockFeedbackVibration);

        /**
        * Destructor.
        */
        virtual ~CHWRMPowerStatePluginHandler();
    
    public: // New functions

        /**
        * Implementation of power state callback method
        *
        * @param aKey   The key that has changed state
        * @param aValue New value of key
        */
        void NotifyStateChange( const TUint32 aKey, TInt aValue );

    private: 
    
        /**
        * C++ default constructor.
        */        
        CHWRMPowerStatePluginHandler(TBool aChargerBlockVibra, TBool aChargerBlockFeedbackVibration);
        /**
        * This 2nd phase constructor.
        */
        void ConstructL();
        
	private:   // data
	
	    // Reference to plugin
	    CHWRMPowerStatePlugin* iPlugin;
	    
	    // Property for each key
	    RProperty iBattLevelProp;
	    RProperty iBattStatusProp;
	    RProperty iChargingStatusProp;
	    RProperty iExtendedBattStatusProp;
        RProperty iExtendedChargingStatusProp;
	    
	    // If ETrue, connecting charger blocks vibra
	    TBool iChargerBlockVibra;
	    // If ETrue, connecting charger blocks feedback vibration
	    TBool iChargerBlockVibraFeedback;
    };

#endif  // HWRMPOWERSTATEPLUGINHANDLER_H
            
// End of File
