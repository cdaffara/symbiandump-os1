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
 @file te_locmonitorstep4.cpp
*/
 
#include <e32svr.h>
#include <s32mem.h>

#include "te_geolocmonitorstep4.h"
#include "te_geolocmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocationinfo.h>

extern void CleanUpAreaInfoArray(TAny* aArray);

CTe_LocMonitorStep4::~CTe_LocMonitorStep4()
	{
	}

CTe_LocMonitorStep4::CTe_LocMonitorStep4()
	{
	SetTestStepName(KLocMonitorStep4);
	}

TVerdict CTe_LocMonitorStep4::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


void CTe_LocMonitorStep4::ReadIniFileValues()
    {
	_LIT(KPROCEDURE,"PROCEDURE");
	iProcedure = -1;
    GetIntFromConfig(ConfigSection(), KPROCEDURE,  iProcedure );
    }


TVerdict CTe_LocMonitorStep4::doTestStepL()
	{
	TPosition expectedPosition, receivedPosition;
	TPositionAreaInfo expectedAreaInfo;

 	if (TestStepResult()==EPass)
 		{
 	
		// here, read from .ini file
		ReadIniFileValues();
		INFO_PRINTF2(_L("Procedure %d"), iProcedure);
		
		TInt result=KErrNotFound;
		
		switch(iProcedure)
			{
			case 1:
				result = TestGSMNewL();
				break;
			case 2:
				result = TestGSMSetTA();
				break;
			case 3:
				result = TestGSMAddNCellL();
				break;
			case 4:
				result = TestValidateGSMInfoHierarchyL();
				break;
			case 5:
				result = TestWcdmaNewL();
				break;
			case 6:
				result = TestWcdmaSetSC();
				break;
			case 7:
				result = TestWcdmaAddNCellL();
				break;
			case 8:
				result = TestValidateWCDMAInfoHierarchyL();
				break;
			case 9:
				result = TestCoordNewL();
				break;
			case 10:
				result = TestValidateCoordInfoL();
				break;
			case 11:
				result = TestWlanNewL();
				break;
			case 12:
				result = TestValidateWlanInfoL();
				break;
			case 13:
				result = TestLocInfoAddCoordL();
				break;
			case 14:
				result = TestLocInfoAddGsmCellL();
				break;
			case 15:
				result = TestLocInfoAddWcdmaCellL();
				break;
			case 16:
				result = TestLocInfoAddWlanL();
				break;
				
			default:
				INFO_PRINTF2(_L("Bad Procedure number %d - Not supported"), iProcedure);
				TESTL(0);
				break;
				
			}
	
	    if (KErrNone == result)
	    	{
			INFO_PRINTF3(_L("Procedure %d passed. Got %d as expected"),iProcedure, KErrNone);
			
	    	}
	    else
	    	{
			INFO_PRINTF4(_L("Procedure %d failed, got error %d and was expecting %d "), iProcedure, result, KErrNone);
			TESTL(0);
	    	}
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep4::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}



TInt CTe_LocMonitorStep4::TestGSMNewL(  )
   {
	INFO_PRINTF1(_L("TestGSMNewL"));

	CLbsGsmCellInfo* gsmInfo = NULL;
   TRAPD( err, gsmInfo = CLbsGsmCellInfo::NewL( 3, 4, 56, 78 ) );
   if( KErrNone != err )
      return err;
   
   if(	( 3 != gsmInfo->MobileCountryCode() ) || 
	  ( 4 != gsmInfo->MobileNetworkCode() ) ||
	  ( 56 != gsmInfo->LocationAreaCode() ) ||
	  ( 78 != gsmInfo->CellId() ) )
      {
      delete gsmInfo;
      return -1;
      }

   delete gsmInfo;
   return KErrNone;
   }



TInt CTe_LocMonitorStep4::TestGSMSetTA(  )
   {
	INFO_PRINTF1(_L("TestGSMSetTA"));
	CLbsGsmCellInfo* gsmInfo = NULL;
   TRAPD( err, gsmInfo = CLbsGsmCellInfo::NewL() );
   if( KErrNone != err )
      return err;
  
  gsmInfo->SetTimingAdvance( 2 ); 
  if( 2 != gsmInfo->TimingAdvance() )
      {
      delete gsmInfo;
      return -1;
	  }
	
   delete gsmInfo;
   return KErrNone;
   }


TInt CTe_LocMonitorStep4::TestGSMSetRxLev(  )
   {
	INFO_PRINTF1(_L("TestGSMSetRxLev()"));
   CLbsGsmCellInfo* gsmInfo = NULL;
   TRAPD( err, gsmInfo = CLbsGsmCellInfo::NewL() );
   if( KErrNone != err )
      return err;
   
  
  gsmInfo->SetRxLevel( 10 ); 
  if( 10 != gsmInfo->RxLevel() )
      {
      delete gsmInfo;
      return -1;
      }
    
   delete gsmInfo;
   return KErrNone;
   }
   


TInt CTe_LocMonitorStep4::TestGSMAddNCellL(  )
	{
	INFO_PRINTF1(_L("TestGSMAddNCellL()"));
 
   TLbsGsmNeighbourCellInfo gnCellInfo;
   gnCellInfo.SetArfcn(12);
   gnCellInfo.SetBsic(13);
   gnCellInfo.SetRxLevel(14);
   
   CLbsGsmCellInfo* gsmInfo = NULL;
   TRAPD( err, gsmInfo = CLbsGsmCellInfo::NewL() );
   if( KErrNone != err )
      return err;
   CleanupDeletePushL( gsmInfo );
  
  gsmInfo->AddNeighbouringCellInfoL( &gnCellInfo ); 
  
  RLbsGsmNeighbourCellInfoArray nCellArray;
  CleanupClosePushL( nCellArray );
  gsmInfo->GetNeighbouringCellInfoL( nCellArray );
  TLbsGsmNeighbourCellInfo res = nCellArray[0];
  
  if( 12 != res.Arfcn() || 13 != res.Bsic() || 14 != res.RxLevel() )
      {
	  CleanupStack::PopAndDestroy(&nCellArray);
	  CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
  RLbsGsmNeighbourCellInfoArray * ptr = &nCellArray;
	
  CleanupStack::PopAndDestroy(&nCellArray);
  CleanupStack::PopAndDestroy(gsmInfo);

  return KErrNone;
	}



TInt CTe_LocMonitorStep4::TestValidateGSMInfoHierarchyL(  )
   {
	INFO_PRINTF1(_L("TestValidateGSMInfoHierarchyL()"));
 
   CLbsGsmCellInfo* gsmInfo = CLbsGsmCellInfo::NewL();
   CleanupDeletePushL( gsmInfo );
   
   TLbsGsmNeighbourCellInfo nCell;
   nCell.SetArfcn( 100 );
   nCell.SetBsic( 30 );
   nCell.SetRxLevel( 20 );
   gsmInfo->AddNeighbouringCellInfoL( &nCell );
   TRAPD( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   gsmInfo->SetRxLevel( 10 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
       {
       CleanupStack::PopAndDestroy(gsmInfo);
       return -1;
       }
   
   gsmInfo->SetTimingAdvance( 200 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   gsmInfo->SetCellId( 455 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   gsmInfo->SetLocationAreaCode( 350 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   gsmInfo->SetMobileNetworkCode( 300 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   gsmInfo->SetMobileCountryCode( 200 );
   TRAP( err, gsmInfo->ValidateDataL() );
   if( KErrNone != err )
      {
      CleanupStack::PopAndDestroy(gsmInfo);
      return -1;
      }
   
   CleanupStack::PopAndDestroy(gsmInfo);
   return KErrNone;
   }




// *****************************************************************************************************************************************





TInt CTe_LocMonitorStep4::TestWcdmaNewL( )
    {
	INFO_PRINTF1(_L("TestWcdmaNewL()"));
 
    CLbsWcdmaCellInfo* wcdmaCell = NULL;
    TRAPD( err, wcdmaCell = CLbsWcdmaCellInfo::NewL( 4, 56, 78 ) );
    if( KErrNone != err )
        return err;

    TRAP( err, wcdmaCell->ValidateDataL() );
    if( KErrNone != err )
        return err;
    
    if( 4 != wcdmaCell->MobileCountryCode() )
        {
        delete wcdmaCell;
        return -1;
        }

    if( 56 != wcdmaCell->MobileNetworkCode() )
        {
        delete wcdmaCell;
        return -1;
        }

    if( 78 != wcdmaCell->UniqueCellId() )
        {
        delete wcdmaCell;
        return -1;
        }
    
    delete wcdmaCell;
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestWcdmaSetSC( )
    {
	INFO_PRINTF1(_L("TestWcdmaSetSC()"));

    CLbsWcdmaCellInfo* wcdmaCell = NULL;
    TRAPD( err, wcdmaCell = CLbsWcdmaCellInfo::NewL() );
    if( KErrNone != err )
        return err;
    
    wcdmaCell->SetScramblingCode( 90 );
    if( 90 != wcdmaCell->ScramblingCode() )
        {
        delete wcdmaCell;
        return -1;
        }

    delete wcdmaCell;
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestWcdmaAddNCellL( )
    {
	INFO_PRINTF1(_L("TestWcdmaAddNCellL()"));
    TLbsWcdmaNeighbourCellInfo nCell( 78, 90, 47, 100 );
    CLbsWcdmaCellInfo* wcdmaCell = NULL;
    
    TRAPD( err, wcdmaCell = CLbsWcdmaCellInfo::NewL() );
    if( KErrNone != err )
        return err;
  
    CleanupDeletePushL( wcdmaCell );
    wcdmaCell->AddNeighbouringCellInfoL( &nCell );
    
    RLbsWcdmaNeighbourCellInfoArray nCellArr;
    CleanupClosePushL( nCellArr );
    
    wcdmaCell->GetNeighbouringCellInfoL( nCellArr );
    TLbsWcdmaNeighbourCellInfo res = nCellArr[0];
    if( 78 != res.UniqueCellId() )
        {
		CleanupStack::PopAndDestroy(&nCellArr);
		CleanupStack::PopAndDestroy(wcdmaCell);
        return -1;
        }
    
    if( 90 != res.ScramblingCode() )
        {
		CleanupStack::PopAndDestroy(&nCellArr);
		CleanupStack::PopAndDestroy(wcdmaCell);
        return -1;
        }
	CleanupStack::PopAndDestroy(&nCellArr);
	CleanupStack::PopAndDestroy(wcdmaCell);  
    
    return KErrNone;
    }



TInt CTe_LocMonitorStep4::TestValidateWCDMAInfoHierarchyL(  )
   {
	INFO_PRINTF1(_L("TestValidateWCDMAInfoHierarchyL(()"));
   CLbsWcdmaCellInfo* wcdmaInfo = CLbsWcdmaCellInfo::NewL();
   CleanupDeletePushL( wcdmaInfo );
   
   TLbsWcdmaNeighbourCellInfo nCell;
   nCell.SetUniqueCellId( 200 );
   nCell.SetScramblingCode( 400 );
   wcdmaInfo->AddNeighbouringCellInfoL( &nCell );
   TRAPD( err, wcdmaInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wcdmaInfo);
      return -1;
      }
   
   wcdmaInfo->SetScramblingCode( 300 );
   TRAP( err, wcdmaInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wcdmaInfo);
      return -1;
      }
   
   wcdmaInfo->SetUniqueCellId( 455 );
   TRAP( err, wcdmaInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wcdmaInfo);
      return -1;
      }
   
   wcdmaInfo->SetMobileNetworkCode( 300 );
   TRAP( err, wcdmaInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wcdmaInfo);
      return -1;
      }
   
   wcdmaInfo->SetMobileCountryCode( 2000 );
   TRAP( err, wcdmaInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wcdmaInfo);
      return -1;
      }
   
   CleanupStack::PopAndDestroy(wcdmaInfo);
   return KErrNone;
   }



// *************************************************************************************************************************



TInt CTe_LocMonitorStep4::TestCoordNewL( )
    {
	INFO_PRINTF1(_L("TestCoordNewL()"));
    CLbsPositionInfo* pos = NULL;
    TCoordinate coord( 12.9765, 77.1423, 3000.0 );
    TLocality src( coord, 12.0 );
    src.SetVerticalAccuracy( 10.0 );
    TRAPD( err, pos = CLbsPositionInfo::NewL( src ) );
    if ( KErrNone != err )
        return err;
    
    TRAP( err, pos->ValidateDataL() );
    if( KErrNone != err )
        return err;
    
    TLocality loc;
    pos->GetLocality( loc );
    if( 12.9765 !=  loc.Latitude() )
        {
        delete pos;
        return -1;
        }
    
    if( 77.1423 != loc.Longitude() )
        {
        delete pos;
        return -1;
        }
    
    if( 3000.0 != loc.Altitude() )
        {
        delete pos;
        return -1;
        }
    
    if( 12.0 != loc.HorizontalAccuracy() )
        {
        delete pos;
        return -1;
        }
    
    if( 10.0 != loc.VerticalAccuracy() )
        {
        delete pos;
        return -1;
        }
    
    delete pos;
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestValidateCoordInfoL(  )
   {
	INFO_PRINTF1(_L("TestValidateCoordInfoL()"));

   CLbsPositionInfo* pos = CLbsPositionInfo::NewL();
   CleanupDeletePushL( pos );
   TLocality loc;
   loc.SetVerticalAccuracy( 50.0 );
   pos->SetLocality( loc );
   TRAPD( err, pos->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(pos);
      return -1;
      }
   
   loc.SetHorizontalAccuracy( 40.0 );
   pos->SetLocality( loc );
   TRAP( err, pos->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(pos);
      return -1;
      }
   
   TCoordinate coord( 12.9423, 77.1423, 455.0 );
   TLocality loc1( coord, 40.0 );
   pos->SetLocality( loc1 );
   pos->ValidateDataL();
   CleanupStack::PopAndDestroy(pos);
   return KErrNone;
   }

// *************************************************************************************************************************


TInt CTe_LocMonitorStep4::TestWlanNewL( )
    {
	INFO_PRINTF1(_L("TestWlanNewL()"));
    TLbsWlanMacAddress macId;
    macId.Append( _L8("FF:FF:FF:FF:FF:FF") );
    
    CLbsWlanInfo* wlanInfo = NULL;
    TRAPD( err, wlanInfo = CLbsWlanInfo::NewL( macId, 12 ) );
    if( KErrNone != err )
        return err;
    
    TRAP( err, wlanInfo->ValidateDataL() );
    if( KErrNone != err )
        return err;
    
    TLbsWlanMacAddress res;
    wlanInfo->GetMacAddress( res );
    if( macId != res )
        {
        delete wlanInfo;
        return -1;
        }
    
    if( 12 != wlanInfo->SignalStrength() )
        {
        delete wlanInfo;
        return -1;
        }
        
    delete wlanInfo;
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestValidateWlanInfoL(  )
   {
	INFO_PRINTF1(_L("TestValidateWlanInfoL()"));
   CLbsWlanInfo* wlanInfo = CLbsWlanInfo::NewL();
   CleanupDeletePushL( wlanInfo );
   
   wlanInfo->SetSignalStrength( 100 );
   TRAPD( err, wlanInfo->ValidateDataL() );
   if( KErrArgument != err )
      {
      CleanupStack::PopAndDestroy(wlanInfo);
      return -1;
      }

   TLbsWlanMacAddress macId;
   macId.Append( _L8("FF:FF:FF:FF:FF:FF") );
   wlanInfo->SetMacAddress( macId );
   TRAP( err, wlanInfo->ValidateDataL() );
   if( KErrNone != err )
      {
      CleanupStack::PopAndDestroy(wlanInfo);
      return -1;
      }
   
   CleanupStack::PopAndDestroy(wlanInfo);   // wlanInfo
   return KErrNone;
   }

// *************************************************************************************************************************


TInt CTe_LocMonitorStep4::TestLocInfoAddCoordL( )
    {
	INFO_PRINTF1(_L("TestLocInfoAddCoordL()"));
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
    
    CleanupStack::PopAndDestroy(&wStream);  
    CleanupStack::PopAndDestroy(locInfo);      
 
    locInfo = NULL;
    
    // Read from stream
    RBufReadStream rStream;
    rStream.Open( *buffer );
    
    locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    ( (CLbsLocationInfoBase*)locInfo )->InternaliseL( rStream );
    
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
		CleanupStack::PopAndDestroy(locInfo);      
		CleanupStack::PopAndDestroy(buffer); 
        return -1;
        }
    
     // Delete coordinate info
    locInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );  
    CleanupStack::PopAndDestroy(&coords);
	CleanupStack::PopAndDestroy(locInfo);      
	CleanupStack::PopAndDestroy(buffer);
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestLocInfoAddGsmCellL( )
    {
	INFO_PRINTF1(_L("TestLocInfoAddGsmCellL()"));
    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupDeletePushL( buffer );
    CLbsLocationInfo* locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    CLbsGsmCellInfo* gsmCell = CLbsGsmCellInfo::NewL( 23, 34, 45, 56 );
    CleanupStack::PushL( gsmCell );
    gsmCell->SetTimingAdvance( 10 );
    gsmCell->SetRxLevel( 100 );
    TLbsGsmNeighbourCellInfo nCell( 100, 60, 30 );
    gsmCell->AddNeighbouringCellInfoL( &nCell );
    locInfo->AddAreaInfoL( gsmCell );
    CleanupStack::Pop();    // gsmCell; ownership transferred to instance of CLbsLocationInfo
    
    RBufWriteStream wStream;
    wStream.Open( *buffer );
    CleanupClosePushL( wStream );
    ( (CLbsLocationInfoBase*)locInfo )->ExternaliseL( wStream );
    wStream.CommitL();
    
    CleanupStack::PopAndDestroy(&wStream);
    CleanupStack::PopAndDestroy(locInfo);   
    locInfo = NULL;
    
    // Read from stream
    RBufReadStream rStream;
    rStream.Open( *buffer );
    
    locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    ( (CLbsLocationInfoBase*)locInfo )->InternaliseL( rStream );
    
    RLbsAreaInfoBaseArray gsmInfo;
 	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &gsmInfo ));
 
    locInfo->GetAreaInfoL( gsmInfo, CLbsLocationInfo::ELbsGsmInfo );
    CLbsGsmCellInfo* posInfo = static_cast<CLbsGsmCellInfo*>( gsmInfo[0] );
    
    RLbsGsmNeighbourCellInfoArray nCellArr;
    CleanupClosePushL(nCellArr);
    posInfo->GetNeighbouringCellInfoL( nCellArr );
    
    if( 23 != posInfo->MobileCountryCode() || 
            34 != posInfo->MobileNetworkCode() || 
            45 != posInfo->LocationAreaCode() || 
            56 != posInfo->CellId() || 
            10 != posInfo->TimingAdvance() || 
            100 != posInfo->RxLevel() ||
            100 != nCellArr[0].Arfcn() || 
            60 != nCellArr[0].Bsic() || 
            30 != nCellArr[0].RxLevel() )
        {
		CleanupStack::PopAndDestroy(&nCellArr);
        CleanupStack::PopAndDestroy(&gsmInfo);
        CleanupStack::PopAndDestroy(locInfo);
        CleanupStack::PopAndDestroy(buffer);
        return -1;
        }
    posInfo->ResetNeighbouringCellInfo();
    locInfo->ResetAreaInfo( CLbsLocationInfo::ELbsGsmInfo );  // Reset 
	CleanupStack::PopAndDestroy(&nCellArr);
    CleanupStack::PopAndDestroy(&gsmInfo);
    CleanupStack::PopAndDestroy(locInfo);
    CleanupStack::PopAndDestroy(buffer);
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestLocInfoAddWcdmaCellL( )
    {
	INFO_PRINTF1(_L("TestLocInfoAddWcdmaCellL()"));
    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupDeletePushL( buffer );
    CLbsLocationInfo* locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    CLbsWcdmaCellInfo* wcdmaCell = CLbsWcdmaCellInfo::NewL( 23, 34, 45 );
    CleanupStack::PushL( wcdmaCell );
    wcdmaCell->SetScramblingCode( 10 );
    TLbsWcdmaNeighbourCellInfo nCell( 100, 60, 47, 100 );
    wcdmaCell->AddNeighbouringCellInfoL( &nCell );
    locInfo->AddAreaInfoL( wcdmaCell );
    CleanupStack::Pop();    // wcdmaCell; ownership transferred to instance of CLbsLocationInfo
    
    RBufWriteStream wStream;
    wStream.Open( *buffer );
    CleanupClosePushL( wStream );
    ( (CLbsLocationInfoBase*)locInfo )->ExternaliseL( wStream );
    wStream.CommitL();
    
    CleanupStack::PopAndDestroy(&wStream);
    CleanupStack::PopAndDestroy(locInfo);    
    locInfo = NULL;
    
    // Read from stream
    RBufReadStream rStream;
    rStream.Open( *buffer );
    
    locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL(locInfo);
    ( (CLbsLocationInfoBase*)locInfo )->InternaliseL( rStream );
    
    RLbsAreaInfoBaseArray wcdmaInfo;
	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &wcdmaInfo ));

    locInfo->GetAreaInfoL( wcdmaInfo, CLbsLocationInfo::ELbsWcdmaInfo );
    CLbsWcdmaCellInfo* posInfo = static_cast<CLbsWcdmaCellInfo*>( wcdmaInfo[0] );
    
    RLbsWcdmaNeighbourCellInfoArray nCellArr;
    CleanupClosePushL(nCellArr);
    posInfo->GetNeighbouringCellInfoL( nCellArr );
    
    if( 23 != posInfo->MobileCountryCode() || 34 != posInfo->MobileNetworkCode() 
            || 45 != posInfo->UniqueCellId() || 10 != posInfo->ScramblingCode() || 
            100 != nCellArr[0].UniqueCellId() || 60 != nCellArr[0].ScramblingCode() ||
            47 != nCellArr[0].Pathloss() || 100 != nCellArr[0].SignalStrength() )
        {
        CleanupStack::PopAndDestroy(&nCellArr);
		CleanupStack::PopAndDestroy(&wcdmaInfo);
		CleanupStack::PopAndDestroy(locInfo);
	    CleanupStack::PopAndDestroy(buffer);
        return -1;
        }
    
    posInfo->ResetNeighbouringCellInfo();
    locInfo->ResetAreaInfo( CLbsLocationInfo::ELbsWcdmaInfo );
    CleanupStack::PopAndDestroy(&nCellArr);
	CleanupStack::PopAndDestroy(&wcdmaInfo);// Reset
    CleanupStack::PopAndDestroy(locInfo);
    CleanupStack::PopAndDestroy(buffer);
    return KErrNone;
    }


TInt CTe_LocMonitorStep4::TestLocInfoAddWlanL( )
    {
	INFO_PRINTF1(_L("TestLocInfoAddWlanL()"));
    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupDeletePushL( buffer );
    CLbsLocationInfo* locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    TLbsWlanMacAddress macId;
    macId.Append( _L8("FF:FF:FF:FF:FF:FF") );
    CLbsWlanInfo* wlanInfo = CLbsWlanInfo::NewL( macId, 77 );
    CleanupStack::PushL( wlanInfo );
    locInfo->AddAreaInfoL( wlanInfo );
    CleanupStack::Pop();    // wlanInfo; ownership transferred to instance of CLbsLocationInfo
    
    RBufWriteStream wStream;
    wStream.Open( *buffer );
    CleanupClosePushL( wStream );
    ( (CLbsLocationInfoBase*)locInfo )->ExternaliseL( wStream );
    wStream.CommitL();
    
    CleanupStack::PopAndDestroy(&wStream);
    CleanupStack::PopAndDestroy(locInfo);   
    locInfo = NULL;
    
    // Read from stream
    RBufReadStream rStream;
    rStream.Open( *buffer );
    
    locInfo = CLbsLocationInfo::NewL();
    CleanupDeletePushL( locInfo );
    ( (CLbsLocationInfoBase*)locInfo )->InternaliseL( rStream );
    
    RLbsAreaInfoBaseArray wlanInfoArr;
	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &wlanInfoArr ));
    
    locInfo->GetAreaInfoL( wlanInfoArr, CLbsLocationInfo::ELbsWlanInfo );
    
    CLbsWlanInfo* posInfo = static_cast<CLbsWlanInfo*>( wlanInfoArr[0] );
    TLbsWlanMacAddress macIdRes;
    posInfo->GetMacAddress( macIdRes );
    
    if( macId != macIdRes || 77 != posInfo->SignalStrength() )
        {
		CleanupStack::PopAndDestroy(&wlanInfoArr);
        CleanupStack::PopAndDestroy(locInfo);
        CleanupStack::PopAndDestroy(buffer);
        return -1;
        }
    // Delete coordinate info
    locInfo->ResetAreaInfo( CLbsLocationInfo::ELbsWcdmaInfo );  
	CleanupStack::PopAndDestroy(&wlanInfoArr);
    CleanupStack::PopAndDestroy(locInfo);
    CleanupStack::PopAndDestroy(buffer);
    return KErrNone;
    }

