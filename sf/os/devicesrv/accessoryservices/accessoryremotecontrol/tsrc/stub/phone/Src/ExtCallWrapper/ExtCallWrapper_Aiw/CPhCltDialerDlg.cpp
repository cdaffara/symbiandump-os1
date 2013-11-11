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
* Description:  Dialer Dialog
*
*/



// INCLUDE FILES
#include    "CPhCltDialerDlg.h"
#include    <phoneclient.rsg>
#include    <CPhCltExtPhoneBase.h>

#include    <avkon.hrh>


// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CPhCltDialerDlg::~CPhCltDialerDlg()
    {
    delete iPhoneClient;
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::DialData
//
// -----------------------------------------------------------------------------
//
CPhCltExtPhoneDialData& CPhCltDialerDlg::DialData()
    {
    return *iData;
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::ExecuteDialLD
//
// -----------------------------------------------------------------------------
//
void CPhCltDialerDlg::ExecuteDialLD()
    {
    TInt error = KErrNone;
    iError = &error;
 	if (!iData->Emergency())
 		{
 		ExecuteLD( R_PHCLT_BLANK_DLG );
 		}
    else
		{
		iPhoneClient = CPhCltExtPhoneBase::NewL( this );
		iPhoneClient->DialL( *iData );
		}
    User::LeaveIfError( error );
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::PreLayoutDynInitL
//
// -----------------------------------------------------------------------------
//
void CPhCltDialerDlg::PreLayoutDynInitL()
    {
    MakeVisible( EFalse );
    ButtonGroupContainer().MakeVisible( EFalse );

    // Create phone client and pass observer.
    iPhoneClient = CPhCltExtPhoneBase::NewL( this );
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::PostLayoutDynInitL
//
// -----------------------------------------------------------------------------
//
void CPhCltDialerDlg::PostLayoutDynInitL()
    {
    iPhoneClient->DialL( *iData );
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::OkToExitL
//
// -----------------------------------------------------------------------------
//
TBool CPhCltDialerDlg::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyEmpty )
        {
        return EFalse;
        }
    else
        {
        return CEikDialog::OkToExitL( aButtonId );
        }
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::Draw
//
// -----------------------------------------------------------------------------
//
void CPhCltDialerDlg::Draw( const TRect& /*aRect*/ ) const
    {
    }


// -----------------------------------------------------------------------------
// CPhCltDialerDlg::HandleDialL
//
// -----------------------------------------------------------------------------
//
void CPhCltDialerDlg::HandleDialL( const TInt aStatus )
    {
    if ( iError )
        {
        *iError = aStatus;
        iError = NULL;
        }

    delete this;
    }


//  End of File
