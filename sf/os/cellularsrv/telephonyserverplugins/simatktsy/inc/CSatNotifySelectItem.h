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
* Name        : CSatNotifySelectItem.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYSELECTITEM_H
#define CSATNOTIFYSELECTITEM_H

//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;
class CBerTlv;
/**
* DESCRIPTION
* SAT Select Item notification class.
* Created when client requests to be notified when Select Item
* command arrives from SIM.
*
* The SIM shall supply a set of items from which the user may
* choose one. Each item comprises a short identifier
* (used to indicate the selection) and a text string.
* Optionally the SIM may include an alpha identifier. This is intended
* to act as a title for the list of items.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifySelectItem ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifySelectItem*: created object 
        */
        static CSatNotifySelectItem* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifySelectItem();

	private:
	
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifySelectItem( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Select Item Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );
         
        /**
        * Cancels notification request for Select Item.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes Select Item message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Select Item terminal response 
        * coming from client.
        * @param aRsp: Response structure
        * @return KErrNone/ KErrCorrupt
        */
        TInt TerminalResponseL( TDes8* aRsp );

	public: // New functions
	
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @param aAdditionalInfo: Additional info for terminal response
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( 
            TUint8 aPCmdNumber, TUint8 aGeneralResult,
            TDesC16& aAdditionalInfo );  

	private:

        /**
        * Handles menu items
        * @param aPCmdNumber: command number
        * @param aBerTlv: bertlv data instance
        * @param aRet: return value
        * @param aIconIdList: pointer to icon id list   
        * @param aSelectItemV2: item structure           
        * @return success/failure value
        */	
		TInt HandleItemsL( TUint8 aPCmdNumber, CBerTlv* aBerTlv, TInt aRet, 
			TPtrC8 aIconIdList, RSat::TSelectItemV2* aSelectItemV2 );
		
    private: // Data

        // Package received from ETel
        RSat::TSelectItemV2Pckg* iSelectItemV2Pckg;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
        // Flag for items next indicator     
        TBool iItemNextIndicatorRemoved;
        TBool iItemsIconIdListRemoved;

    };


#endif // CSATNOTIFYSELECTITEM_H