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
* Name        : MSatNotificationsBase.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications classes
* Version     : 1.0
*
*/




#ifndef MSATNOTIFICATIONSBASE_H
#define MSATNOTIFICATIONSBASE_H


//  INCLUDES
#include <et_tsy.h>

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
	// None
	
// CONSTANTS
	// None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class CSatDataPackage;

// CLASS DECLARATION
    // The following classes are inherited from this class
    // CSatNotifyDisplayText, CSatNotifyGetInkey, CSatNotifyGetInput, 
    // CSatNotifyPlayTone, CSatNotifySetUpMenu, CSatNotifySelectItem, 
    // CSatNotifySendSm, CSatNotifySendSs, CSatNotifySendUssd, 
    // CSatNotifySetUpCall, CSatNotifyRefresh, CSatNotifySimSessionEnd, 
    // CSatNotifySetUpIdleModeText, CSatNotifyLaunchBrowser, 
    // CSatNotifyCallControlRequest, CSatNotifyPollInterval, CSatNotifySendDtmf,
    // CSatNotifySetUpEventList, CSatNotifyPollingOff, CSatNotifyLocalInfo, 
    // CSatNotifyTimerMgmt, CSatNotifyMoreTime, CSatNotifyLanguageNotification, 
    // CSatNotifyMoreTime, CSatNotifySendData, CSatNotifyReceiveData, 
    // CSatNotifyMoSmControlRequest, CSatNotifyGetChannelStatus, 
    // CSatNotifyOpenChannel, CSatNotifyCloseChannel
	
// DESCRIPTION
// SAT notifications base class.
// Pure virtual base class for SAT command notification classes.

class MSatNotificationsBase 
    {
    
    public: // New functions
    
        /**
        * Pure virtual function for derived classes. Notification
        * request.
        * @param aTsyReqHandle: Request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */        
        virtual TInt Notify( const TTsyReqHandle /*aTsyReqHandle*/,
            const TDataPackage& /*aPackage*/ )
		    {
    		return KErrNotSupported;
    		}            

        /**
        * Pure virtual function for derived classes. Cancels notification 
        * request.
        * @param aReqHandle: Notify request handle
        * @return KErrNone
        */
        virtual TInt CancelNotification( const TTsyReqHandle /*aReqHandle*/ )
		    {
    		return KErrNotSupported;
    		}  
    		
        /**
        * Pure virtual function for derived classes. Notification
        * completion method.
        * @param aDataPackage: Pointer to client parameter class
        * @param aErrorCode: status
        * @return KErrNone or status
        */
        virtual TInt CompleteNotifyL( CSatDataPackage* aDataPackage, 
            TInt aErrorCode ) = 0;         

        /**
        * Pure virtual function for derived classes. Handles the 
        * terminal response
        * @param aRsp: response structure
        * @return Success/Failure value
        */
        virtual TInt TerminalResponseL( TDes8* /*aRsp*/ )
		    {
    		return KErrNotSupported;
    		}           

    };              

#endif // MSATNOTIFICATIONSBASE_H
