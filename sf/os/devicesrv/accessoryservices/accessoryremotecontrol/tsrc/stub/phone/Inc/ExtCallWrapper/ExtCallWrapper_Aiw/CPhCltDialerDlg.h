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


#ifndef CPHCLTDIALERDLG_H
#define CPHCLTDIALERDLG_H

//  INCLUDES
#include    <eikdialg.h>
#include    <MPhCltExtPhoneObserver.h>
#include    <CPhCltExtPhoneBase.h>
#include    <CPhCltExtPhoneDialData.h>

// FORWARD DECLARATIONS
class CPhCltExtPhoneBase;

// CLASS DECLARATION

/**
*  It is dialer class.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCltDialerDlg )
    : public CEikDialog,
      private MPhCltExtPhoneObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        ~CPhCltDialerDlg();


    public: // New functions

        /** 
        * Dial parameters.
        *
        * @returns Returns dial data.
        */
        CPhCltExtPhoneDialData& DialData();

        /**
        * Dial using given information.
        */
        void ExecuteDialLD();


    private:

        /**
        * From CEikDialog, called before layout.
        */
        void PreLayoutDynInitL();
        
        /**
        * From CEikDialog, called after layout.
        */
        void PostLayoutDynInitL();

        /**
        * Check if it's ok to exit.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CEikDialog, draw.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From MPhCltExtPhoneObserver, handles status of dial.
        *
        * @param aStatus status.
        */
        void HandleDialL( const TInt aStatus );


    private:    // Data

        // Pointer to error.
        TInt* iError;

        // Reference to dial data.
        CPhCltExtPhoneDialData* iData;

        // Instance of phone client.
        CPhCltExtPhoneBase* iPhoneClient;

    };

#endif      // CPHCLTDIALERDLG_H
            
// End of File
