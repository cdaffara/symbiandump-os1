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




#ifndef BTGPSREQUESTMANAGER_H
#define BTGPSREQUESTMANAGER_H

//  INCLUDES

#include <e32base.h> 

#include "BTGPSMessageListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;
class CBTGPSConstantsManager;
class MBTGPSLocationFixListener;
class CBTGPSFix;
class CBTGPSNmeaBuffer;

// CLASS DECLARATION

/**
*  This class defines locaiton request managers. It's responsible of 
*  receiving NMEA sentence and retrieving fix information for sentence
*  stream. It also manage GSV sentence life time information.
* 
*/
class CBTGPSRequestManager: public CBase, private MBTGPSMessageListener
    {
    public: 

        /**
        * Two-phase construction.
		* @param aConnectManager Reference to connect manager.
        */
        static CBTGPSRequestManager * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSConstantsManager& aConstantsManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSRequestManager();

    public:
        /**
        * Start to retrieve fix
        */
        void RetrieveFixL();

        /**
        * Stop retrieving fix
        */
        void StopRetrievingFix();
        
        /**
        * Receive last location
		* @param aFix On return contains pointer to fix information
		* @param aMaxAge Max age tath defined by client
		* @param aAcceptPartial If the client accept partial updates.
        */
        TInt LastLocation(
            const CBTGPSFix*& aFix,
            const TTime& aMaxAge,
            const TBool aAcceptPartial);
            
        /**
        * Invalid last location and GSV life time. This function is used 
        * by CBTGPSEnvChangeHandler. When system time is changed
        * last location shall be invalidated.
        */
        void SystemTimeChange();
        
        /**
        * Add listener
		* @param aListener Reference to listener
        */
        void AddListenerL(MBTGPSLocationFixListener& aListener);
        
        /**
        * Remove listener
		* @param aListener Reference to listener
        */
        void RemoveListener(MBTGPSLocationFixListener& aListener);
        
        /**
        * Get previous NMEA buffer index
        */
        TInt GetPreviousNmeaBufferBottom() const;

    private:
        /**
        * From MBTGPSMessageListener
        */
        void HandleMessage(const TBTGPSNmeaParser& aParser);
    
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSRequestManager(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSConstantsManager& aConstantsManager);

        /**
        * Complete all request
        */
        void CompleteAllReqeust(TInt aErr);
        
        /**
        * Inform listners on new fix information
        */
        void InformListeners();
        
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
        
        //Constants Manager
        CBTGPSConstantsManager& iConstantsManager;

        //Location fix listener
        RArray<MBTGPSLocationFixListener*> iListenerArray;

        //NMEA buffer. It's a FIFO circulate buffer that
        //holds all received NMEA message
        CBTGPSNmeaBuffer* iNmeaBuffer;
        
        //Index to the previous bottom in NMEA buffer
        TInt iPrevBottom;
        
        //Current Fix
        CBTGPSFix* iFix;
        
        //Last fix
        CBTGPSFix* iLastFix;

        //Start to receive message
        TBool iReceivingMessage;
        
        //GSV Timer
        CPeriodic* iTimer;
    };
#endif
// End of File

