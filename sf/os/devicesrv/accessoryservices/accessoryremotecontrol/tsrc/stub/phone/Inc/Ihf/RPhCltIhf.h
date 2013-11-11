/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Session for Ihf mode handling in phone client.
*
*/

#ifndef RPHCLTIHF_H
#define RPHCLTIHF_H

// INCLUDES
#include    <etelmm.h>            // ETel

#include "PhCltTypes.h"

// FORWARD DECLARATIONS
class RWsSession;
class RPhCltServer;

// CLASS DECLARATION
/**
*  Session for Ihf mode handling in phone client.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltIhf : public RSubSessionBase
    {

    public:  // Constructors and destructor  

        /**
        * Constructor.
        */      
        IMPORT_C RPhCltIhf();


    public: // New functions    

        /**
        * Creates a subsession with the phone server.
        * 
        * @param aServer Pre-opened server session.
        * @param aPriority Priority for the active object handling notify 
        *                  events. Not used.
        * @return Symbian OS standard error code.
        */      
        IMPORT_C TInt Open( 
            RPhCltServer& aServer, 
            TInt aPriority = CActive::EPriorityStandard );

        /**
        * Closes the subsession. Cancels all notify requests.
        * Remember to call this to a connected session in some point.
        */
        IMPORT_C void Close();


    public: // API
        
        /**
        * Set Ihf mode
        *
        * @param aMode ETrue to set IHF mode on, otherwise set IHF mode off
        * @param aShowNote ETrue if the IHF activation/deactivation note 
        *                  should be shown
        * @return Standard Symbian OS error code. Note, if the note can't be 
        *         shown, the system will not return any error code. The error 
        *         code just means the IHF mode changing is failed. If aMode is 
        *         same as current IHF mode of the system, KErrAlreadyExist will
        *         be returned.
        *         If the headset or other audio accessory is connected, then 
        *         IHF mode can't be activated, and KErrAccessDenied will be 
        *         returned.
        */
        IMPORT_C TInt SetIhfMode( 
            TBool aMode, 
            TBool aShowNote = EFalse );
    
        /**
        * Set Ihf mode for Phone application only.
        *
        * @param aMode ETrue to set IHF mode on, otherwise set IHF mode off
        * @param aShowNote ETrue if the IHF activation/deactivation note should
        *                  be shown
        * @return Standard Symbian OS error code. Note, if the note can't be 
        *         shown, the system will not return any error code. The error 
        *         code just means the IHF mode changing is failed. If aMode is 
        *         same as current IHF mode of the system, KErrAlreadyExist will
        *         be returned.
        *         If the headset or other audio accessory is connected, then IHF
        *         mode can't be activated, and KErrAccessDenied will be 
        *         returned.
        */
        IMPORT_C TInt SetIhfModeFromPhone( 
            TBool aMode, 
            TBool aShowNote = EFalse );

    };

#endif // RPHCLTIHF_H
