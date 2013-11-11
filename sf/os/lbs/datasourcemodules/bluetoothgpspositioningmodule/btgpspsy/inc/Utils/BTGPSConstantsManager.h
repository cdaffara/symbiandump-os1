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




#ifndef BTGPSCONSTANTSMANAGER_H
#define BTGPSCONSTANTSMANAGER_H

//  INCLUDES

#include <e32base.h>
#include <f32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RResourceFile;

// CLASS DECLARATION

/**
*  Manager class for global defined contants. This class read all
*  the pre-defined constants from resource file on construction.
*  
*/
class CBTGPSConstantsManager: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSConstantsManager * NewL();

        /**
        * Destructor
        */
        virtual ~CBTGPSConstantsManager();

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
        CBTGPSConstantsManager();

        /**
        * Get localisable resource name
        * This function leaves if no localisable resource is found
        */
        TFileName* GetLocalisableResourceNameL(RFs& aFs);
        
        /**
        * Get TInt from Central Repositary
        */
        void GetIntL();
            
        /**
        * Get text from resource
        */
        HBufC* GetTextL(
            RResourceFile& aResourceFile, 
            TInt aId);
        
        /**
        * Check Range
        */
        template<class T> void CheckRangeL(
            T aValue, 
            T aMin, 
            T aMax);
        
    public:
        ///Send delay
        TInt iSendDelay;

        ///The user equivalent range error used in GPS data 
        ///error calculations in meters
        TReal iUserEquivalentRangeError; 
        
        ///The number of test NMEA sentences needed to receive during the 
        ///iTimeIntervalForDeviceTypeDetect of time.
        TInt iNumberOfTestNmeaSentences;
        
        ///Delay between the sending of initialisation messages
        TInt iDelayBetweenGpsInitMessages;

        ///The number of messages to repeately send to
        ///the GPS in initialisation to set it to full power mode
        TInt iNumberOfSetFullPowerMessage;

        ///Delay for waiting PNOK compatible BT GPS response PNOK polling message
        TInt iDelayForPNokGpsDetect;
        
        ///Inactive delay
        TInt iInactivityDelay; 
        
        ///Utilise power saving mode
        TInt iUtilisePowerSaving;
        
        ///Satellite Information(GSV sentences) life time
        TInt iSatelliteInfoLifeTime;
        
        ///Size of NMEA data buffer
        TInt iNmeaBufferSize;

		///The text for battery low dialog
		HBufC*   iBatteryLowDialogText;
		
		///The text for battery full dialog
		HBufC*   iBatteryFullDialogText;
		
		///The text for ext. antenna connected dialog
		HBufC*   iExtAntennaConnectedDialogText;
		
		///The text for ext. antenna disconnected dialog     
		HBufC*   iExtAntennaDisconnectedDialogText;
		
		///The text for ext. antenna connected dialog
		HBufC*   iExtPowerConnectedDialogText;
		
		///The text for ext. antenna disconnected dialog
		HBufC*   iExtPowerDisconnectedDialogText;
		       
    };
#endif
// End of File

