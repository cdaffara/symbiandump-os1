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



#ifndef CMMFAXEXTINTERFACE_H
#define CMMFAXEXTINTERFACE_H

//  INCLUDES
#include <etelmm.h>
#include "cmmcalltsy.h"

// CLASS DECLARATION
 
/**
*  CMmFaxExt contains extended fax call functionality.
*/ 

class CMmFaxExtInterface : public CBase
    {

    public:
		
		/**
         * Two-phased constructor.
         */
        static CMmFaxExtInterface* NewL( CMmCallTsy* aMmCall );

        /**
         * Start waiting for a call. Called from CmmCallTSY object 
         * AnswerIcomingCall method.
         *          
         *
         * @param aTsyReqHandle, reqhandle
         * @return error value
         */
        virtual TInt AnswerIncomingCall( const TTsyReqHandle& aTsyReqHandle ) = 0;

        /**
         * Start Fax call dialing. Call from CMmCallTsy object Dial method.
         *          
         *
         * @param aTsyReqHandle, reqhandle
         * @param aTelNumber, pointer to telnumber 
         * @return error value
         */
        virtual TInt Dial( const TTsyReqHandle& aTsyReqHandle, 
            TDesC* aTelNumber ) = 0;

        /**
         * Set terminate flag true on CMmTsyFax object.
         *          
         *
         */
        virtual void DialCancel() = 0;

        /**  
         * CMmFaxTsy object creation.
         *          
         *
         * @param aName, object name
         * @return CTelObject*, pointer to Etel object
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName ) = 0;

        /**
         * Close fax session.
         *          
         *
         */
        virtual void HangUp() = 0;
        
        /**
         * Transfers fax settings to local variables.
         *          
         *
         * @param aSettings, fax settings
         * @return error value
         */
        virtual TInt SetFaxSettings( const RCall::TFaxSessionSettings* aSettings ) = 0;

        /**
         * Transfers fax settings from variables.
         *          
         *
         * @param aSettings, fax settings
         * @return error value
         */
        virtual TInt GetFaxSettings( RCall::TFaxSessionSettings* aSettings ) = 0;

    };

#endif // CMMFAXEXTINTERFACE_H 
// End of File
