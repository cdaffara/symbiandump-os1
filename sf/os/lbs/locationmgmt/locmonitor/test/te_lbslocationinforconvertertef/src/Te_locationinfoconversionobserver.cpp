/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test module to implement Advanced trigger supervision cases
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "Te_locationinfoconversionobserver.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTriggerFireObserver::CTriggerFireObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelocationinfoconversionobserver::CTelocationinfoconversionobserver(  ):CTimer( CTimer::EPriorityStandard )
    {
    
        CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTriggerFireObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelocationinfoconversionobserver::ConstructL()
    {
        iStatusCode=KErrGeneral;
     	CTimer::ConstructL();
     	
    }

// -----------------------------------------------------------------------------
// CTriggerFireObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTelocationinfoconversionobserver* CTelocationinfoconversionobserver::NewL( )
    {
    
    CTelocationinfoconversionobserver* self = new (ELeave) CTelocationinfoconversionobserver(  );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;

    }

// Destructor
CTelocationinfoconversionobserver::~CTelocationinfoconversionobserver()
    { 
	    Cancel(); 
	}

void CTelocationinfoconversionobserver::OnConversionComplete( TInt aStatusCode )
	{
	iStatusCode = aStatusCode;
	CActiveScheduler::Stop();
	}

TInt CTelocationinfoconversionobserver::ConversionResult()
    {
    return iStatusCode;
    }
    


void CTelocationinfoconversionobserver::RunL( )
    {
    iStatusCode = KErrCancel;
    CActiveScheduler::Stop();
      
    }

void CTelocationinfoconversionobserver::DoCancel( )
    {
     
     
    }

//End of file
