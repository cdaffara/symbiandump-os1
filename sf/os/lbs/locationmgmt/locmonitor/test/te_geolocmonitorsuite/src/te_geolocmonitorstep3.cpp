/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file te_locmonitorstep3.cpp
*/
 
#include <e32svr.h>
#include <s32mem.h>

#include "te_geolocmonitorstep3.h"
#include "te_geolocmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocationinfo.h>

extern void CleanUpAreaInfoArray(TAny* aArray);

CTe_LocMonitorStep3::~CTe_LocMonitorStep3()
	{
	}

CTe_LocMonitorStep3::CTe_LocMonitorStep3()
	{
	SetTestStepName(KLocMonitorStep3);
	}

TVerdict CTe_LocMonitorStep3::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Call to GetLastKnownPositionArea using a TPositionInfo
// and a TPositionAreaInfo object.
//
// This test checks that it is possible to request the last known
// position with area information from the Location Monitor using
// the above mentioned data types.
//
TVerdict CTe_LocMonitorStep3::doTestStepL()
	{
	TPosition expectedPosition, receivedPosition;
	TPositionAreaInfo expectedAreaInfo;

 	if (TestStepResult()==EPass)
 		{
		INFO_PRINTF1(_L("TestLocInfoAddCoordL()"));

		TInt result;
	    TRAP( result, TestLocInfoAddCoordL(); );
	    if (KErrNone == result)
	    	{
			INFO_PRINTF2(_L("TestLocInfoAddCoordL passed. Got %d as expected"),KErrNone);
	    	}
	    else
	    	{
			INFO_PRINTF3(_L("TestLocInfoAddCoordL failed, got error %d and was expecting %d "), result, KErrNone);
			TESTL(0);
	    	}
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep3::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

TInt CTe_LocMonitorStep3::TestLocInfoAddCoordL()
    {
    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupDeletePushL( buffer );
    CLbsLocationInfo* locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    TCoordinate coord( 12.9765, 77.1423, 3000.0 );
    TLocality src( coord, 12.0 );
    src.SetVerticalAccuracy( 10.0 );
    CLbsPositionInfo* pos = CLbsPositionInfo::NewL( src );
    CleanupStack::PushL( pos );
    locInfo->AddAreaInfoL( pos );
    CleanupStack::Pop();    // pos; ownership transferred to instance of CLbsLocationInfo
        
    RBufWriteStream wStream;
    wStream.Open( *buffer );
    CleanupClosePushL( wStream );
    ( (CLbsLocationInfoBase*)locInfo )->ExternaliseL( wStream );
    wStream.CommitL();
    
    CleanupStack::PopAndDestroy();    // wStream
    CleanupStack::PopAndDestroy();    // locInfo    
    locInfo = NULL;
     
    // Read from stream
    RBufReadStream rStream;
    rStream.Open( *buffer );
    
    locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    locInfo->InternaliseL( rStream );
      
    RLbsAreaInfoBaseArray coords;
	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &coords ));
    
    locInfo->GetAreaInfoL( coords, CLbsLocationInfo::ELbsPosInfo );
    CLbsPositionInfo* posInfo = static_cast<CLbsPositionInfo*>( coords[0] );
    TLocality res;
    posInfo->GetLocality( res );
    
    if( 12.9765 != res.Latitude() || 
            77.1423 != res.Longitude() || 
            3000.0 != res.Altitude() || 
            12.0 != res.HorizontalAccuracy() || 
            10.0 != res.VerticalAccuracy() )
        {
		CleanupStack::PopAndDestroy(&coords);
		//CleanupStack::PopAndDestroy( &coords );
        CleanupStack::PopAndDestroy( 2 );
        return -1;
        }
    
    // Delete coordinate info
    locInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
  
    CleanupStack::PopAndDestroy(&coords);
    CleanupStack::PopAndDestroy(2);  // locInfo and buffer
    return KErrNone;
    }


