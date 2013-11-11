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
* Name        : CSatNotifyTimerMgmt.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYTIMERMGMT_H
#define CSATNOTIFYTIMERMGMT_H


//  INCLUDES
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Notify Timer management notification class.
*
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyTimerMgmt ) :  public CBase,
						     public MSatNotificationsBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyTimerMgmt*: created object 
        */
        static CSatNotifyTimerMgmt* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyTimerMgmt();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyTimerMgmt( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base class
            
		/**
        * Completes Timer management message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

    public: // New methods 
            
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult );  

    private:    // New methods

        /**
        * Converts time to seconds
        * @param TPtrC8 time: time in semi-octet
        * @return TUint32: time in seconds
        */
        TUint32 ConvertToSeconds( TPtrC8 time );
	
    private: // Data

        // Command qualifier
        TUint8 iCmdQualifier;
        // Timer Value  
        TUint8 iTimerValue[3];            
        // Timer Identifier
        TUint8 iTimerId;               
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
    };


#endif // CSatNotifyTimerMgmt_H
