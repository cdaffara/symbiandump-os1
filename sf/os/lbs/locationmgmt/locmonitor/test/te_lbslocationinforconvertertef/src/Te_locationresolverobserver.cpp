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
#include "Te_locationresolverobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTriggerFireObserver::CTriggerFireObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTeLocationResolverObserver::CTeLocationResolverObserver(  ):CTimer( CTimer::EPriorityStandard )
    {
    
        CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTriggerFireObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTeLocationResolverObserver::ConstructL()
    {
        iStatusCode=KErrGeneral;
     	CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CTriggerFireObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTeLocationResolverObserver* CTeLocationResolverObserver::NewL( )
    {
    
    CTeLocationResolverObserver* self = new (ELeave) CTeLocationResolverObserver(  );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;

    }

// Destructor
CTeLocationResolverObserver::~CTeLocationResolverObserver()
    { 
	    Cancel(); 
	}


void CTeLocationResolverObserver::OnResolveLocationComplete(TInt aStatus, const TLocality& aLocality)
    {
    iStatusCode = aStatus;
 
    if (iStatusCode == KErrNone)
    	{
    	TReal64 lat = aLocality.Latitude();
    	TReal64 lng = aLocality.Longitude();
    	TReal32 alt =aLocality.Altitude();
    	TPositionDatumId datum = aLocality.Datum();
    	TReal32 horAcc = aLocality.HorizontalAccuracy();
    	TReal32 vertAcc = aLocality.VerticalAccuracy();

    	iLocality = aLocality;
             	
    	// the horAcc is interesting ...
    	// when the locality is not delivered from the cache then the hor acc is that returned by the test plugin
    	// when it comes out of the cahche then it's the loc monitor that sets it according to 
    	// the country code e.t.c
    	// similarly for the vertical accuracy ... its set to Nan when brought out of the cache
    	// but its whatever came from the converter otherwise.
 
    	
		// this test ensures that when a locality comes out of the cash that 
    	// vertical accuracy is a Nan. So we can use this to check that 
    	// values come out of the cache when they should.
    	if	(Math::IsNaN(vertAcc))
			{
			iFromCache = ETrue;
			}
    	else
    		{
			iFromCache = EFalse;
    		}
    	}
    CActiveScheduler::Stop();
    }



TInt CTeLocationResolverObserver::ConversionResult()
    {
    return iStatusCode;
    }
   
TInt CTeLocationResolverObserver::ConversionLocality(TLocality& aLocality)
    {
	aLocality = iLocality;
    return iFromCache;
    }


void CTeLocationResolverObserver::RunL( )
    {
    iStatusCode = KErrCancel;
    CActiveScheduler::Stop();
      
    }

void CTeLocationResolverObserver::DoCancel( )
    {
     
     
    }

//End of file
