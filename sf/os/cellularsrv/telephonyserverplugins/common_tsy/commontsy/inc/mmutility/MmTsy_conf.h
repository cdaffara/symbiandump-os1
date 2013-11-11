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



#ifndef MMTSY_CONF_H
#define MMTSY_CONF_H

//  INCLUDES
#include <etelmm.h>

    /** 
     * Phonet configuration
     * Define the supported protocols. The supported protocols are returned 
     * to the client when it requests them by using the 
     * CMmPhoneTsy::GetMultimodeCaps method.
     */     

    const TUint KMultimodeCaps = RMobilePhone::KCapsGsmSupported + 
                                 RMobilePhone::KCapsGprsSupported +
                                 RMobilePhone::KCapsWcdmaSupported;

    /**
     * Default protocol
     * Define what protocol will be set as the active protocol in the bootup 
     * in TSY. This flag will be used by all modules inside the TSY.
     */     
    const RMobilePhone::TMobilePhoneNetworkMode KDefaultProtocol
        = RMobilePhone::ENetworkModeGsm;
     

    /**
     * Supported Lines and Modules
     * When a const KXXXSupported value has been give value 1 it means that 
     * his feature is supported and this kind of line or subsession can be 
     * opened using the CMmPhoneTsy::OpenNewObjectByNameL method. 
     * These values may have been also used in other places too. If support
     * value is 0 it means that this feature will NOT be supported regardless
     * of all other variables, definitions etc.
     */     

    /** Primary voice line support */
    const TInt KVoice1LineSupported         = 1;  

    /**Secondary voice line support */
    const TInt KVoice2LineSupported         = 1; 

    /** Data line support */
    const TInt KDataLineSupported           = 1;  

    /** Fax line support */
    const TInt KFaxLineSupported            = 1;    

    /** Conference call support */
    const TInt KConferenceCallSupported     = 1; 

    /** SMS support */
    const TInt KSmsMessagingSupported       = 1;

    /** Broadcast messaging support */
    const TInt KBroadcastMessagingSupported = 1;

    /** ON Store support */
    const TInt KONStoreSupported            = 1;

    /** EN Store support */
    const TInt KENStoreSupported            = 1;

    /** SIM phonebook support */
    const TInt KSimPhoneBookSupported       = 1;

    /** Sim Application Toolkit support */
    const TInt KSatToolkitSupported         = 1;
    
    /** GPRS support */
    const TInt KGprsLineSupported           = 1;

    /** USSD support */
    const TInt KUssdMessagingSupported      = 1;

    /** Custom TSY support */
    const TInt KCustomAPISupported          = 1;

    /**
     * Maximum number of phone, line and call objects. 
     * Maximum nuber of supported modules is currently 1. This value has been
     * proposed by Symbian. Maximum number of supported lines is currently 4.
     * Maximum number of supported calls has not been limited at the moment.
     */     
    const TInt KMmTsyNumOfSupportedPhones     = 1;
    const TInt KMmTsyNumOfSupportedLines      = 4;

    /**
     * This constant defines how many call objects can be opened from the TSY. 
     * This guarantees that ETel's clients can not waste resources (memory) of 
     * TSY by opening new call objects by mistake or consiously to harm TSY.
     * The maximum number has been calulated from the following:
     *  - 2 voice call object (1 for each line for incoming calls
     *  - 1 data call object (for incoming data call)
     *  - 1 emergency call object
     *  - 1+7 call objects for Confence call (max number of CC participants)
     *  - phonebook 1
     *  - javaphone 1
     *  - WTA 1
     *  - voice mail box 1
     *  - csd 1
     *  - fax 1
     *  - 3rd party 2
     */     
    const TInt KMaxNumberOfCallObjectsFromTsy = 20;

#endif //  MMTSY_CONF_H
            
// End of File
