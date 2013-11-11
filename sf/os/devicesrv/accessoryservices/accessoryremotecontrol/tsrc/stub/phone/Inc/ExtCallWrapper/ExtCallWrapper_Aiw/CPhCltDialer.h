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
* Description:  It is dialer class.
*
*/


#ifndef CPHCLTDIALER_H
#define CPHCLTDIALER_H

//  INCLUDES
#include    <CPhCltExtPhoneBase.h>
#include    <CPhCltExtPhoneDialData.h>

// FORWARD DECLARATIONS
class CPhCltDialerDlg;

// CLASS DECLARATION

/**
*  It is dialer class.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltDialer
    : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @return Pointer to created CPhCltDialer instance.
        */
        IMPORT_C static CPhCltDialer* NewL();
        
        /**
        * Destructor.
        */
        ~CPhCltDialer();


    public: // New functions

        /** 
        * Dial parameters.
        *
        * @return Returns dial data.
        */
        IMPORT_C CPhCltExtPhoneDialData& DialData();

        /**
        * Performs dialing with the dial data.
        */
        IMPORT_C void ExecuteLD();


    private:

        /**
        * C++ constructor.
        */
        CPhCltDialer();
        
        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * Cleanup action. Clears iDestroyed & iDialDlg.
        */
        static void DoClear( TAny* aAny );


    private:    // Data

        // Pointer to boolean indicating if this has been destroyed already.
        TBool* iDestroyed;

        // Dialer dialog
        CPhCltDialerDlg* iDialDlg;

    };

#endif      // CPHCLTDIALER_H
            
// End of File
