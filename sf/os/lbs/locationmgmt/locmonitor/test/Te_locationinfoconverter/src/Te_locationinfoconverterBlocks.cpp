/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifItemParser.h>
#include <Stiftestinterface.h>
#include "Te_locationinfoconverter.h"
#include <e32math.h> 
#include <e32const.h>
#include <BAUTILS.h>
#include <e32std.h>
#include <e32base.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define KTestLatitude 10
#define KTestLongitude 20
#define KTestAltitude 30
#define KTestHorizontalaccuracy 30
#define KTestVerticalaccuracy 40

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def
//Tags used in cfg file
_LIT(KLatitude, "LATITUDE= ");
_LIT(KLongitude, "LONGITUDE= ");
_LIT(KAltitude, "ALTITUDE= ");
_LIT(KHorizontalAccuracy, "HA= ");
_LIT(KVerticalAccuracy, "VA= ");
_LIT(KCountrycode, "COUNTRYCODE= ");
_LIT(KNetworkcode, "NETWORKCODE= ");
_LIT(KLocationcode, "LOCATIONCODE= ");
_LIT(KCellid, "CELLID= ");
_LIT(KRxlev, "RXLEV= ");
_LIT(KTimingAdvance, "TIMINGADVANCE= ");
_LIT(KNeighbouringcells, "NEIGHBOURCELLS= ");
_LIT(KArfcn, "ARFCN= ");
_LIT(KColourcode, "CCODE= ");
_LIT(KNeighbourRxlev, "NRXLEV= ");
_LIT(KScramblingcode, "SCODE= ");
_LIT(KNeighbourCellid, "NCELLID= ");
_LIT(KNeighbourScramblingcode, "NSCODE= ");
_LIT(KMacId, "MACID= ");
_LIT(KSignalstrength, "SIGNAL= ");
_LIT(KNeighbourPathloss, "NPATHLOSS= ");
_LIT(KNeighbourSignalstrength, "NSIGNAL= ");

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTe_locationinfoconverter::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "ConvertLocationInfo", CTe_locationinfoconverter::ConvertLocationInfoL ),
        ENTRY( "Misc", CTe_locationinfoconverter::MiscL ),
        ENTRY( "CreateLocationInfoConverter", CTe_locationinfoconverter::CreateLocationInfoConverterL ),
        ENTRY( "SetCoordinateInfo", CTe_locationinfoconverter::SetCoordinateInfoL ),
        ENTRY( "SetGSMCellInfo", CTe_locationinfoconverter::SetGSMCellInfoL ),
        ENTRY( "SetWCDMACellInfo", CTe_locationinfoconverter::SetWCDMACellInfoL ),
        ENTRY( "SetWLanInfo", CTe_locationinfoconverter::SetWLanInfoL ),
        ENTRY( "AddLocationInfo", CTe_locationinfoconverter::AddLocationInfoL ),
        ENTRY( "ValidateLocationInfo", CTe_locationinfoconverter::ValidateLocationInfoL ),
        ENTRY( "Cleanup", CTe_locationinfoconverter::Cleanup ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::CreateLocationInfoConverterL
// Creates and initializes Location info converter object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::CreateLocationInfoConverterL( CStifItemParser& aItem )
    {
	iLog->Log(_L("CovertLocationInfoL Function"));
//    TUint Uidinteger = 537028467;       // SUPL
	TUint Uidinteger = 537007117;      // Example
    TUid uid = TUid::Uid( Uidinteger );
   
    aItem.GetNextInt(Uidinteger);
    
    iObserver = CTelocationinfoconversionobserver::NewL();
    ilbslocationinfo = CLbsLocationInfo::NewL();
    if(Uidinteger!=0)
        {
        ilbslocationinfoconverter = CLbsLocationInfoConverter::NewL(*iObserver,uid);
        }
    else
        {
		iLog->Log(_L("CovertLocationInfoL Function : Creating"));
        ilbslocationinfoconverter = CLbsLocationInfoConverter::NewL(*iObserver);
        }
		iLog->Log(_L("CovertLocationInfoL Function end"));
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::SetCoordinateInfoL
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::SetCoordinateInfoL( CStifItemParser& aItem )
    {
    TInt fntype;
    aItem.GetNextInt(fntype);
    TPtrC Llatidude,Llongitude,Laltidude,Lhoracc,LVerAcc;
    aItem.GetNextString( KLatitude, Llatidude);    
    aItem.GetNextString( KLongitude, Llongitude);
    aItem.GetNextString( KAltitude, Laltidude);
    aItem.GetNextString( KHorizontalAccuracy, Lhoracc);
    aItem.GetNextString( KVerticalAccuracy, LVerAcc);
    
    TReal64 latitude =0 ;
    TLex16 lat(Llatidude);
    lat.Val(latitude);
    
    TReal64 longitude =0;
    TLex16 longi(Llongitude);
    longi.Val(longitude);
    
    TReal32 Altitude =0;
    TLex16 Alt(Laltidude);
    Alt.Val(Altitude);
    
    TReal32 HorizontalAccuracy =0;
    TLex16 HA(Lhoracc);
    HA.Val(HorizontalAccuracy);
    
    TReal32 VerticalAccuracy =0;
    TLex16 VA(LVerAcc);
    VA.Val(VerticalAccuracy);
    TLocality Locality;
    if(latitude!=0 && longitude!=0 && Altitude==0)
        {
        Locality.SetCoordinate(latitude,longitude);
        }
    else if (latitude!=0 && longitude!=0 && Altitude!=0)
        {
        Locality.SetCoordinate(latitude,longitude,Altitude);
        }
    
           
    if(HorizontalAccuracy!=0 ) 
        {
        Locality.SetHorizontalAccuracy(HorizontalAccuracy);
        }
    if(VerticalAccuracy!=0 ) 
            {
            Locality.SetVerticalAccuracy(VerticalAccuracy);
            }
    
    CLbsPositionInfo* Posinfo; 
    if(fntype==1)
        {
        Posinfo=CLbsPositionInfo::NewL( Locality); 
        }
    else if(fntype==2)
        {
        Posinfo=CLbsPositionInfo::NewL();
        Posinfo->SetLocality(Locality);
        }
    
    ilbslocationinfo->AddAreaInfoL(Posinfo);
    return KErrNone;   
    }


// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::SetGSMCellInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::SetGSMCellInfoL( CStifItemParser& aItem )
    {
    TInt fntype;
    aItem.GetNextInt(fntype);
    TInt countrycode=-2,networkcode=-2,locationcode=-2,cellid=-2,timingadvance=-2,Rxlev=-2,nNeignbourcells=0,arfcn=-2,bsic=-2,NRxlev=-2;
    aItem.GetNextInt( KCountrycode, countrycode);    
    aItem.GetNextInt( KNetworkcode, networkcode);
    aItem.GetNextInt( KLocationcode, locationcode);
    aItem.GetNextInt( KCellid, cellid);
    aItem.GetNextInt( KTimingAdvance, timingadvance);
    aItem.GetNextInt( KRxlev, Rxlev);
    aItem.GetNextInt( KNeighbouringcells, nNeignbourcells);
      
    CLbsGsmCellInfo* gsmcellinfo;    
    if(fntype==1)
        {
        gsmcellinfo=CLbsGsmCellInfo::NewL( countrycode,networkcode,locationcode,cellid); 
        if(timingadvance!=-2)
            {
            gsmcellinfo->SetTimingAdvance(timingadvance);
            }
        if(Rxlev!=-2)
            {
            gsmcellinfo->SetRxLevel(Rxlev);
            }
        if(nNeignbourcells!=0)
            {
            TLbsGsmNeighbourCellInfo neighbourcell;
            for(TInt i =0 ; i<nNeignbourcells ; i++)
                    {
                    aItem.GetNextInt(KColourcode,bsic);
                    if(bsic!=-2)
                        {
                        neighbourcell.SetBsic(bsic);
                        }
                    aItem.GetNextInt(KNeighbourRxlev,NRxlev);
                    if(NRxlev!=-2)
                        {
                        neighbourcell.SetRxLevel(NRxlev);
                        }
                    aItem.GetNextInt(KArfcn,arfcn);
                    if(arfcn!=-2)
                        {
                        neighbourcell.SetArfcn(arfcn);
                        }
                    gsmcellinfo->AddNeighbouringCellInfoL(&neighbourcell);
                  
                    }
            }
        }
    else if(fntype==2)
        {
        gsmcellinfo=CLbsGsmCellInfo::NewL();
        if(countrycode!=-2)
            {
            gsmcellinfo->SetMobileCountryCode(countrycode);
            }
        if(networkcode!=-2)
            {
            gsmcellinfo->SetMobileNetworkCode(networkcode);
            }
        if(locationcode!=-2)
            {
            gsmcellinfo->SetLocationAreaCode(locationcode);
            }
        if(cellid!=-2)
            {
            gsmcellinfo->SetCellId(cellid);
            }
        if(timingadvance!=-2)
            {
            gsmcellinfo->SetTimingAdvance(timingadvance);
            }
        if(Rxlev!=-2)
            {
            gsmcellinfo->SetRxLevel(Rxlev);
            }
        if(nNeignbourcells!=0)
            {
            
            
            for(TInt i =0 ; i<nNeignbourcells ; i++)
               {
               aItem.GetNextInt(KColourcode,bsic);
               if(bsic!=-2)
                   {
                   bsic = 0;
                   }
               aItem.GetNextInt(KNeighbourRxlev,Rxlev);
               if(Rxlev!=-2)
                   {
                   Rxlev = 0;
                   }
               aItem.GetNextInt(KArfcn,arfcn);
               if(arfcn!=-2)
                   {
                   arfcn = 0;
                   }
               TLbsGsmNeighbourCellInfo neighbourcell(bsic,Rxlev,arfcn);
               gsmcellinfo->AddNeighbouringCellInfoL(&neighbourcell);
             
               }   
            }
        }
    
    ilbslocationinfo->AddAreaInfoL(gsmcellinfo);
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::SetWCDMACellInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::SetWCDMACellInfoL( CStifItemParser& aItem )
    {
    TInt fntype;
    aItem.GetNextInt(fntype);
    TInt countrycode=-2,networkcode=-2,cellid=-2,nNeignbourcells=0,scramblingcode=-2,neighbourucid=-2,neighbourscode=-2,neighbourpathloss=-2,neighboursignalstrength=-2;
    aItem.GetNextInt( KCountrycode, countrycode);    
    aItem.GetNextInt( KNetworkcode, networkcode);
    
    aItem.GetNextInt( KCellid, cellid);
    aItem.GetNextInt( KScramblingcode, scramblingcode);
  
    aItem.GetNextInt( KNeighbouringcells, nNeignbourcells);
         
    CLbsWcdmaCellInfo* wcdmacellinfo;    
    if(fntype==1)
        {
        wcdmacellinfo=CLbsWcdmaCellInfo::NewL( countrycode,networkcode,cellid); 
        if(scramblingcode!=-2)
            {
            wcdmacellinfo->SetScramblingCode(scramblingcode);
            }
        if(nNeignbourcells!=0)
            {
            TLbsWcdmaNeighbourCellInfo neighbourcell;
            for(TInt i =0 ; i<nNeignbourcells ; i++)
                {
                aItem.GetNextInt(KNeighbourCellid,neighbourucid);
                if(neighbourucid!=-2)
                    {
                    neighbourcell.SetUniqueCellId(neighbourucid);
                    }
                aItem.GetNextInt(KNeighbourScramblingcode,neighbourscode);
                if(neighbourscode!=-2)
                    {
                    neighbourcell.SetScramblingCode(neighbourscode);
                    }
                aItem.GetNextInt(KNeighbourPathloss,neighbourpathloss);
                if(neighbourpathloss!=-2)
                    {
                    neighbourcell.SetPathloss(neighbourpathloss);
                    }
                aItem.GetNextInt(KNeighbourSignalstrength,neighboursignalstrength);
                if(neighboursignalstrength!=-2)
                    {
                    neighbourcell.SetSignalStrength(neighboursignalstrength);
                    }
                wcdmacellinfo->AddNeighbouringCellInfoL(&neighbourcell);
                }
            }
        }
    else if(fntype==2)
        {
        wcdmacellinfo=CLbsWcdmaCellInfo::NewL();
        if(countrycode!=-2)
            {
            wcdmacellinfo->SetMobileCountryCode(countrycode);
            }
        if(networkcode!=-2)
            {
            wcdmacellinfo->SetMobileNetworkCode(networkcode);
            }
        if(cellid!=-2)
            {
            wcdmacellinfo->SetUniqueCellId(cellid);
            }
        if(scramblingcode!=-2)
            {
            wcdmacellinfo->SetScramblingCode(scramblingcode);
            }
        if(nNeignbourcells!=0)
            {
            for(TInt i =0 ; i<nNeignbourcells ; i++)
                {
                aItem.GetNextInt(KNeighbourCellid,neighbourucid);
                if(neighbourucid==-2)
                    {
                    neighbourucid = 0;
                    }
                aItem.GetNextInt(KNeighbourScramblingcode,neighbourscode);
                if(neighbourscode==-2)
                    {
                    neighbourscode = 0;
                    }
                aItem.GetNextInt(KNeighbourPathloss,neighbourpathloss);
                if(neighbourpathloss==-2)
                    {
                    neighbourpathloss = 0;
                    }
                aItem.GetNextInt(KNeighbourSignalstrength,neighboursignalstrength);
                if(neighboursignalstrength==-2)
                    {
                    neighboursignalstrength = 0;
                    }
                
                TLbsWcdmaNeighbourCellInfo neighbourcell(neighbourucid,neighbourscode,neighbourpathloss,neighboursignalstrength);
                wcdmacellinfo->AddNeighbouringCellInfoL(&neighbourcell);
                }
            }
        }
   
    ilbslocationinfo->AddAreaInfoL(wcdmacellinfo);
    return KErrNone;   
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::SetWLanInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::SetWLanInfoL( CStifItemParser& aItem )
    {
    TInt fntype;
    aItem.GetNextInt(fntype);
    TPtrC macid;
    TInt signalstrength=-2;
    aItem.GetNextString( KMacId, macid);    
    aItem.GetNextInt( KSignalstrength, signalstrength);
    TBuf8<6> macidbuf;
    macidbuf.Copy(macid);
    CLbsWlanInfo* wlaninfo;    
    if(fntype==1)
        {
        wlaninfo=CLbsWlanInfo::NewL( macidbuf,signalstrength); 
        }
    else if(fntype==2)
        {
        wlaninfo=CLbsWlanInfo::NewL();
        if (macidbuf.Length()!=0)
            {
            wlaninfo->SetMacAddress(macidbuf);
            }
        if(signalstrength!=-2)
            {
            wlaninfo->SetSignalStrength(signalstrength);
            }
        }
    ilbslocationinfo->AddAreaInfoL(wlaninfo);
    return KErrNone;    
    }
// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::AddLocationInfoL
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::AddLocationInfoL( CStifItemParser& aItem )
    {
    TInt nTimes =1;
    aItem.GetNextInt(nTimes);
    ilbslocationinfoarrayTemp.Append(ilbslocationinfo);
    for (TInt i=0 ; i<nTimes;i++ )
        {
        ilbslocationinfoarray.Append(ilbslocationinfo);
        }
    ilbslocationinfo = CLbsLocationInfo::NewL();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::ConvertLocationInfoL
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::ConvertLocationInfoL( CStifItemParser& aItem )
    {
    
    TInt conversionprefs ;
    aItem.GetNextInt(conversionprefs);
    TInt requestedinfo;
    aItem.GetNextInt(requestedinfo);
    TInt fntype=0;
    aItem.GetNextInt(fntype);
    TRAPD( err, 
    if(fntype==0)
        {
        ilbslocationinfoconverter->ConvertLocationInfoL(*ilbslocationinfoarray[0],conversionprefs,requestedinfo);
        }
    else if (fntype==2)
        {
        ilbslocationinfoconverter->ConvertLocationInfoL(ilbslocationinfoarray,conversionprefs,requestedinfo);
        }
    )
    
    if(err)
        return err;
    
    CActiveScheduler::Start();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::CancelConvertLocationInfo
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTe_locationinfoconverter::MiscL( CStifItemParser& aItem)
    {
    TInt testcase ;
    aItem.GetNextInt(testcase);
    switch(testcase)
        {
        case 1:
            ilbslocationinfoconverter->ConvertLocationInfoL(*ilbslocationinfo,0,1);
            ilbslocationinfoconverter->CancelConvertLocationInfo();
            iObserver->After(1500000);
            CActiveScheduler::Start();
            break;
        case 2:
            ilbslocationinfoconverter->ConvertLocationInfoL(*ilbslocationinfo,0,1);
            CActiveScheduler::Start();
            ilbslocationinfoconverter->CancelConvertLocationInfo();
                        
            break;
        case 3:
            
            ilbslocationinfoconverter->CancelConvertLocationInfo();
                        
            break;
        case 4:
            TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);          
            ilbslocationinfoconverter->ConvertLocationInfoL(*ilbslocationinfo,0,1);
            ilbslocationinfoconverter->ConvertLocationInfoL(*ilbslocationinfo,0,1);
                                
            break;
        case 5:
            ilbslocationinfoconverter->ConvertLocationInfoL(ilbslocationinfoarray,0,1);
            ilbslocationinfoconverter->CancelConvertLocationInfo();
            iObserver->After(1500000);
            CActiveScheduler::Start();
            break;
        case 6:
            ilbslocationinfoconverter->ConvertLocationInfoL(ilbslocationinfoarray,0,1);
            CActiveScheduler::Start();
            ilbslocationinfoconverter->CancelConvertLocationInfo();
                        
            break;
        case 7:
            
            ilbslocationinfoconverter->CancelConvertLocationInfo();
                        
            break;
        case 8:
            TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);          
            ilbslocationinfoconverter->ConvertLocationInfoL(ilbslocationinfoarray,0,1);
            ilbslocationinfoconverter->ConvertLocationInfoL(ilbslocationinfoarray,0,1);
                                
            break;
            
        }
    
    
    return iObserver->ConversionResult();;
    }



TInt CTe_locationinfoconverter::ValidateLocationInfoL( CStifItemParser& aItem)
    {

	if( iObserver->ConversionResult() == KErrNotSupported ||
		 iObserver->ConversionResult() == KErrNotFound )
		return KErrNone;

	if( iObserver->ConversionResult() == KErrNone )
		{
	    TInt requestedinfo =1;
	    aItem.GetNextInt(requestedinfo);
	    if(requestedinfo ==1)
	        {
	        TInt count = ilbslocationinfoarray.Count();
	        for(TInt i=0; i<count ; i++)
	            {
	    		RPointerArray<CLbsAreaInfoBase> PosInfoList;
	            static_cast<CLbsLocationInfo*>(ilbslocationinfoarray[i])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
	            TInt count2 = PosInfoList.Count();
	            PosInfoList.Reset();
	            PosInfoList.Close();
	            if(count2>=1)
	                {
	                return KErrNone;
	                }
	            }
	        return KErrGeneral;
	        }
    	}
    
    return iObserver->ConversionResult();
    }

TInt CTe_locationinfoconverter::Cleanup( CStifItemParser& aItem )
    {
    if(iObserver)
        {
        delete iObserver;
        }
    if(ilbslocationinfo)
        {
        delete ilbslocationinfo;
        }
    if(ilbslocationinfoconverter)
        {
        delete ilbslocationinfoconverter;
        }
    ilbslocationinfoarrayTemp.ResetAndDestroy();
    ilbslocationinfoarray.Reset();
      
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTe_locationinfoconverter::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CTe_locationinfoconverter::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
