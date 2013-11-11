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



#ifndef CMMCUSTOMGSMEXT_H
#define CMMCUSTOMGSMEXT_H

// INCLUDES
#include "CMmCustomExtInterface.h"
#include "cmmphonefactorytsy.h"

// FORWARD DECLARATIONS
class CMmCustomTsy;

// CLASS DECLARATION

/**
*  CMmCustomGsmExt contains GSM mode dependent custom functionality.
*  Extension request handles are stored in MM Custom object.
*/
NONSHARABLE_CLASS( CMmCustomGsmExt ) : public CMmCustomExtInterface
    {
    public:

        /**
         * Destructor.
         */
        virtual ~CMmCustomGsmExt();

        /**
         * Creates a new GSM Custom specific object instance.
         *          
         * @param aMmPhoneTsy a pointer to MM Phone Tsy object
         * @param aMmCustomTsy a pointer to MM Custom Tsy object
         */
        static CMmCustomGsmExt* NewL( CMmPhoneTsy* aMmPhoneTsy,
            CMmCustomTsy* aMmCustomTsy );

        /**
         * Sets if local remote alerting tone playing is required
         *          
         * @param aAlertingToneStatus to play locally or not
         */
        void SetLocalRemoteAlertToneRequired( TBool aAlertingToneStatus );

        /**
         * Returns remote alerting tone status
         *          
         * @param aStatus Pointer to remote alert tone status
         * @return KErrNone
         */
        TInt GetRemoteAlertingToneStatus(
            RMmCustomAPI::TRemoteAlertingToneStatus* aStatus );

    private:

        /**
         * For initialising object attributes.
         *          
         * @param aMmCustomTsy A pointer to MM Custom Tsy object
         */
         void ConstructL( CMmCustomTsy* aMmCustomTsy );

        /**
         * Constructor.
         *          
         * @param aMmPhoneTsy A pointer to MM Phone Tsy object
         */
        CMmCustomGsmExt( CMmPhoneTsy* aMmPhoneTsy );

  private:

        /**
         *  Pointer to the Custom TSY object
         *  Not own.         
         */
        CMmCustomTsy* iMmCustomTsy;

        /**
         * Remote alerting status         
         */
        RMmCustomAPI::TRemoteAlertingToneStatus iRemoteAlertingToneStatus;

        /**
         * Ptr to CMmPhoneTsy object
         * Not own.         
         */        
        CMmPhoneTsy* iMmPhoneTsy;

    };

#endif  // CMMCUSTOMGSMEXT_H

//  End of File
