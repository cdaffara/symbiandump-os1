/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "step.h"
#include "te_lbslocationinfoconvertersuitedefs.h"

#include "Te_locationinfoconversionobserver.h"

#include <Lbs.h> 

// the test code sets the timing advance in the GSM vell info to this value
// which instructs the test converter to constuct TLocalitys with
// latitude ser to country code, longitude set to 
// network code and altitiude set to local area code
const TInt KTimingAdvanceSpecialValue= 254;

const TInt KTimingAdvanceSaveToCacheTrue= 253;

const TInt KTimingAdvanceSaveToCacheFalse= 252;

CStep::~CStep()
/**
 * Destructor
 */
	{

	}

CStep::CStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KStep);
	}

TVerdict CStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationInfoConverterSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
    TInt id = GetTestCaseId();
    iApiId = GetTestCaseApi();
    iPluginUid = GetPluginUid();
   
    __UHEAP_MARK;
    CallL(id);
	__UHEAP_MARKEND;
	return TestStepResult();
	}



TVerdict CStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}







// -----------------------------------------------------------------------------
// CStep::CreateLocationInfoConverterL
// Creates and initializes Location info converter object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::CreateLocationInfoConverterL()
    {
    TUid uid = TUid::Uid(iPluginUid);
    iLbsLocationInfo = CLbsLocationInfo::NewL();
    
    if(iApiId == 0)
        {
        iObserver = CTelocationinfoconversionobserver::NewL();
        iObserver->Version();
        if(iPluginUid!=0)
            {
            iLbsLocationInfoConverter = CLbsLocationInfoConverter::NewL(*iObserver,uid);
            }
        else
            {
            iLbsLocationInfoConverter = CLbsLocationInfoConverter::NewL(*iObserver);
            }
        }
    else
        {
        iResolverObserver = CTeLocationResolverObserver::NewL();
        iResolverObserver->Version();
        if(iPluginUid!=0)
            {
            iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid);
            }
        else
            {
            iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver);
            }
        }

    return KErrNone;
    
    }

//// -----------------------------------------------------------------------------
//// CStep::SetCoordinateInfoL
//// Converts one location info to its corresponding position info.
//// (other items were commented in a header).
//// -----------------------------------------------------------------------------
////
//TInt CStep::SetCoordinateInfoL(CStifItemParser& aItem )
//    {
//    TInt fntype;
//    aItem.GetNextInt(fntype);
//    TPtrC Llatidude,Llongitude,Laltidude,Lhoracc,LVerAcc;
//    aItem.GetNextString( KLatitude, Llatidude);    
//    aItem.GetNextString( KLongitude, Llongitude);
//    aItem.GetNextString( KAltitude, Laltidude);
//    aItem.GetNextString( KHorizontalAccuracy, Lhoracc);
//    aItem.GetNextString( KVerticalAccuracy, LVerAcc);
//    
//    TReal64 latitude =0 ;
//    TLex16 lat(Llatidude);
//    lat.Val(latitude);
//    
//    TReal64 longitude =0;
//    TLex16 longi(Llongitude);
//    longi.Val(longitude);
//    
//    TReal32 Altitude =0;
//    TLex16 Alt(Laltidude);
//    Alt.Val(Altitude);
//    
//    TReal32 HorizontalAccuracy =0;
//    TLex16 HA(Lhoracc);
//    HA.Val(HorizontalAccuracy);
//    
//    TReal32 VerticalAccuracy =0;
//    TLex16 VA(LVerAcc);
//    VA.Val(VerticalAccuracy);
//    TLocality Locality;
//    if(latitude!=0 && longitude!=0 && Altitude==0)
//        {
//        Locality.SetCoordinate(latitude,longitude);
//        }
//    else if (latitude!=0 && longitude!=0 && Altitude!=0)
//        {
//        Locality.SetCoordinate(latitude,longitude,Altitude);
//        }
//    
//           
//    if(HorizontalAccuracy!=0 ) 
//        {
//        Locality.SetHorizontalAccuracy(HorizontalAccuracy);
//        }
//    if(VerticalAccuracy!=0 ) 
//            {
//            Locality.SetVerticalAccuracy(VerticalAccuracy);
//            }
//    
//    CLbsPositionInfo* Posinfo; 
//    if(fntype==1)
//        {
//        Posinfo=CLbsPositionInfo::NewL( Locality); 
//        }
//    else if(fntype==2)
//        {
//        Posinfo=CLbsPositionInfo::NewL();
//        Posinfo->SetLocality(Locality);
//        }
//    
//    ilbslocationinfo->AddAreaInfoL(Posinfo);
//    return KErrNone;   
//    }


// -----------------------------------------------------------------------------
// CStep::SetGSMCellInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStep::SetGSMCellInfoL( TInt fntype, TInt countrycode, TInt networkcode, 
        TInt locationcode, TInt cellid, TInt timingadvance, TInt Rxlev, TInt nNeignbourcells, 
        TInt arfcn, TInt bsic, TInt NRxlev)
    {
	INFO_PRINTF5(_L("SetGSMCellInfoL(CC=%d, NC=%d, LC=%d CID=%d)"),countrycode,networkcode,locationcode,cellid);
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
                    //aItem.GetNextInt(KColourcode,bsic);
                    if(bsic!=-2)
                        {
                        neighbourcell.SetBsic(bsic);
                        }
                    //aItem.GetNextInt(KNeighbourRxlev,NRxlev);
                    if(NRxlev!=-2)
                        {
                        neighbourcell.SetRxLevel(NRxlev);
                        }
                    //aItem.GetNextInt(KArfcn,arfcn);
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
               //aItem.GetNextInt(KColourcode,bsic);
               if(bsic!=-2)
                   {
                   bsic = 0;
                   }
               //aItem.GetNextInt(KNeighbourRxlev,Rxlev);
               if(Rxlev!=-2)
                   {
                   Rxlev = 0;
                   }
               //aItem.GetNextInt(KArfcn,arfcn);
               if(arfcn!=-2)
                   {
                   arfcn = 0;
                   }
               TLbsGsmNeighbourCellInfo neighbourcell(bsic,Rxlev,arfcn);
               gsmcellinfo->AddNeighbouringCellInfoL(&neighbourcell);
             
               }   
            }
        }
    
    iLbsLocationInfo->AddAreaInfoL(gsmcellinfo);
   
    }

TInt CStep::SetGSMCellInfo2L( TInt fntype, TInt countrycode, TInt networkcode, 
        TInt locationcode, TInt cellid, TInt timingadvance, TInt Rxlev, TInt nNeignbourcells, 
        TInt arfcn, TInt bsic, TInt NRxlev)
    {
	INFO_PRINTF5(_L("SetGSMCellInfo2L(CC=%d, NC=%d, LC=%d CID=%d)"),countrycode,networkcode,locationcode,cellid);
 
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
                    //aItem.GetNextInt(KColourcode,bsic);
                    if(bsic!=-2)
                        {
                        neighbourcell.SetBsic(bsic);
                        }
                    //aItem.GetNextInt(KNeighbourRxlev,NRxlev);
                    if(NRxlev!=-2)
                        {
                        neighbourcell.SetRxLevel(NRxlev);
                        }
                    //aItem.GetNextInt(KArfcn,arfcn);
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
               //aItem.GetNextInt(KColourcode,bsic);
               if(bsic!=-2)
                   {
                   bsic = 0;
                   }
               //aItem.GetNextInt(KNeighbourRxlev,Rxlev);
               if(Rxlev!=-2)
                   {
                   Rxlev = 0;
                   }
               //aItem.GetNextInt(KArfcn,arfcn);
               if(arfcn!=-2)
                   {
                   arfcn = 0;
                   }
               TLbsGsmNeighbourCellInfo neighbourcell(bsic,Rxlev,arfcn);
               gsmcellinfo->AddNeighbouringCellInfoL(&neighbourcell);
             
               }   
            }
        }
    
    iLbsLocationInfo2->AddAreaInfoL(gsmcellinfo);
    return KErrNone;    
    }
// -----------------------------------------------------------------------------
// CStep::SetWCDMACellInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::SetWCDMACellInfoL(TInt fntype, TInt countrycode,
        TInt networkcode, TInt cellid, TInt nNeignbourcells, TInt scramblingcode,  
        TInt neighbourucid, TInt neighbourscode, TInt neighbourpathloss, 
        TInt neighboursignalstrength)
    {
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
                //aItem.GetNextInt(KNeighbourCellid,neighbourucid);
                if(neighbourucid!=-2)
                    {
                    neighbourcell.SetUniqueCellId(neighbourucid);
                    }
                //aItem.GetNextInt(KNeighbourScramblingcode,neighbourscode);
                if(neighbourscode!=-2)
                    {
                    neighbourcell.SetScramblingCode(neighbourscode);
                    }
                //aItem.GetNextInt(KNeighbourPathloss,neighbourpathloss);
                if(neighbourpathloss!=-2)
                    {
                    neighbourcell.SetPathloss(neighbourpathloss);
                    }
                //aItem.GetNextInt(KNeighbourSignalstrength,neighboursignalstrength);
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
                //aItem.GetNextInt(KNeighbourCellid,neighbourucid);
                if(neighbourucid==-2)
                    {
                    neighbourucid = 0;
                    }
                //aItem.GetNextInt(KNeighbourScramblingcode,neighbourscode);
                if(neighbourscode==-2)
                    {
                    neighbourscode = 0;
                    }
                //aItem.GetNextInt(KNeighbourPathloss,neighbourpathloss);
                if(neighbourpathloss==-2)
                    {
                    neighbourpathloss = 0;
                    }
                //aItem.GetNextInt(KNeighbourSignalstrength,neighboursignalstrength);
                if(neighboursignalstrength==-2)
                    {
                    neighboursignalstrength = 0;
                    }
                
                TLbsWcdmaNeighbourCellInfo neighbourcell(neighbourucid,neighbourscode,neighbourpathloss,neighboursignalstrength);
                wcdmacellinfo->AddNeighbouringCellInfoL(&neighbourcell);
                }
            }
        }
   
    iLbsLocationInfo->AddAreaInfoL(wcdmacellinfo);
    return KErrNone;   
    }

// -----------------------------------------------------------------------------
// CStep::SetWLanInfoL
// .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::SetWLanInfoL(TInt fntype, TInt macidint,  TInt signalstrength)
    {
    TBuf8<6> macidbuf;
    macidbuf.AppendNum(macidint);
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
    iLbsLocationInfo->AddAreaInfoL(wlaninfo);
    return KErrNone;    
    }
// -----------------------------------------------------------------------------
// CStep::AddLocationInfoL
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStep::AddLocationInfoL()
    {
    TInt nTimes =1;
    iLbsLocationInfoArrayTemp.Append(iLbsLocationInfo);
    for (TInt i=0 ; i<nTimes;i++ )
        {
        iLbsLocationInfoArray.Append(iLbsLocationInfo);
        }
    iLbsLocationInfo = CLbsLocationInfo::NewL();
    }

void CStep::AddLocationInfo2L()
    {
    TInt nTimes =1;
    iLbsLocationInfoArrayTemp2.Append(iLbsLocationInfo2);
    for (TInt i=0 ; i<nTimes;i++ )
        {
        iLbsLocationInfoArray2.Append(iLbsLocationInfo2);
        }
    iLbsLocationInfo2 = CLbsLocationInfo::NewL();
    }

// -----------------------------------------------------------------------------
// CStep::ConvertLocationInfoL
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStep::ConvertLocationInfoL(TInt conversionprefs, TInt requestedinfo, TInt fntype)
    {
    if(iApiId == 0)
        {
        if(fntype==0)
            {
            iLbsLocationInfoConverter->ConvertLocationInfoL(*iLbsLocationInfoArray[0],conversionprefs,requestedinfo);
            }
        else if (fntype==2)
            {
            iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,conversionprefs,requestedinfo);
            }
        }
    else
        {
        CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
        iLbsLocationResolver->ResolveLocationL(*input);
        }

    CActiveScheduler::Start();

    }

void CStep::ConvertLocationInfo2L(TInt conversionprefs, TInt requestedinfo, TInt fntype)
    {

    if(iApiId == 0)
        {
        if(fntype==0)
            {
            iLbsLocationInfoConverter2->ConvertLocationInfoL(*iLbsLocationInfoArray2[0],conversionprefs,requestedinfo);
            }
        else if (fntype==2)
            {
            iLbsLocationInfoConverter2->ConvertLocationInfoL(iLbsLocationInfoArray2,conversionprefs,requestedinfo);
            }
        }
    else
        {
        CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray2[0]);
        iLbsLocationResolver2->ResolveLocationL(*input);
        }

    CActiveScheduler::Start();

    }
// -----------------------------------------------------------------------------
// CStep::CancelConvertLocationInfo
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::MiscL(TInt testcase)
    {
    switch(testcase)
        {
        case 1:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(*iLbsLocationInfo,0,1);
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                iObserver->After(1500000);
                CActiveScheduler::Start();
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                iLbsLocationResolver->CancelResolveLocation();
                iResolverObserver->After(1500000);
                CActiveScheduler::Start();
                }
            break;
        case 2:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(*iLbsLocationInfo,0,1);
                CActiveScheduler::Start();
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                CActiveScheduler::Start();
                iLbsLocationResolver->CancelResolveLocation();
                }  
            break;
        case 3:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                return KErrNone;
                }
            else
                {
                iLbsLocationResolver->CancelResolveLocation();
                return KErrNone;
                }  
            break;
        case 4:
//            TODO
//            TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);      
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(*iLbsLocationInfo,0,1);
                iLbsLocationInfoConverter->ConvertLocationInfoL(*iLbsLocationInfo,0,1);
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                iLbsLocationResolver->ResolveLocationL(*input);
                }                 
            break;
        case 5:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,0,1);
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                iObserver->After(1500000);
                CActiveScheduler::Start();
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                iLbsLocationResolver->CancelResolveLocation();
                iResolverObserver->After(1500000);
                CActiveScheduler::Start();
                }
            break;
        case 6:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,0,1);
                CActiveScheduler::Start();
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                CActiveScheduler::Start();
                iLbsLocationResolver->CancelResolveLocation();
                }  
            break;
        case 7:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->CancelConvertLocationInfo();
                return KErrNone;
                }
            else
                {
                iLbsLocationResolver->CancelResolveLocation();
                return KErrNone;
                }       
            break;
        case 8:
            if(iApiId == 0)
                {
                iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,0,1);
                iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,0,1);
                }
            else
                {
                CLbsLocationInfo* input = static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0]);
                iLbsLocationResolver->ResolveLocationL(*input);
                iLbsLocationResolver->ResolveLocationL(*input);
                }              
            break;
        }
    
    if(iApiId == 0)
        {
        return iObserver->ConversionResult();
        }
    else
        {
        return iResolverObserver->ConversionResult();
        }
    }



TInt CStep::ValidateLocationInfo(TInt requestedinfo)
    {
    if(iApiId == 0)
        {
        if( iObserver->ConversionResult() == KErrNone )
            {
            if(requestedinfo ==1)
                {
                TInt count = iLbsLocationInfoArray.Count();
                for(TInt i=0; i<count ; i++)
                    {
                    RPointerArray<CLbsAreaInfoBase> PosInfoList;
                    TRAPD(error, static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[i])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo));
                    if (KErrNone != error)
                    	{
						PosInfoList.ResetAndDestroy();
                        PosInfoList.Close();
						return error;
                    	}
                    TInt count2 = PosInfoList.Count();
                    PosInfoList.ResetAndDestroy();
                    PosInfoList.Close();
                    if(count2>=1)
                        {
                        return KErrNone;
                        }
                    }
                }
            }
        
        return iObserver->ConversionResult();
        }
    else
        {
        if( iResolverObserver->ConversionResult() == KErrNone )
            {
            if(requestedinfo ==1)
                {
                TInt count = iLbsLocationInfoArray.Count();
                for(TInt i=0; i<count ; i++)
                    {
                    RPointerArray<CLbsAreaInfoBase> PosInfoList;
                    TRAPD(error, static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[i])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo));
                    if (KErrNone != error)
                    	{
                    	PosInfoList.ResetAndDestroy();
                        PosInfoList.Close();
                        return error;
                    	}
                    TInt count2 = PosInfoList.Count();
                    PosInfoList.ResetAndDestroy();
                    PosInfoList.Close();
                    if(count2>=1)
                        {
                        return KErrNone;
                        }
                    }
                }
            }
        return iResolverObserver->ConversionResult();        
        }
    }


TInt CStep::ValidateLocationInfo2(TInt requestedinfo)
    {
    if(iApiId == 0)
        {
        if( iObserver2->ConversionResult() == KErrNotSupported ||
             iObserver2->ConversionResult() == KErrNotFound )
            return KErrNone;
    
        if( iObserver2->ConversionResult() == KErrNone )
            {
            if(requestedinfo ==1)
                {
                TInt count = iLbsLocationInfoArray2.Count();
                for(TInt i=0; i<count ; i++)
                    {
                    RPointerArray<CLbsAreaInfoBase> PosInfoList;
                    static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray2[i])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
                    TInt count2 = PosInfoList.Count();
                    PosInfoList.ResetAndDestroy();
                    PosInfoList.Close();
                    if(count2>=1)
                        {
                        return KErrNone;
                        }
                    }
                return KErrGeneral;
                }
            }
        
        return iObserver2->ConversionResult();
        }
    else
        {
        if( iResolverObserver2->ConversionResult() == KErrNotSupported ||
                iResolverObserver2->ConversionResult() == KErrNotFound )
            return KErrNone;
    
        if( iResolverObserver2->ConversionResult() == KErrNone )
            {
            if(requestedinfo ==1)
                {
                TInt count = iLbsLocationInfoArray2.Count();
                for(TInt i=0; i<count ; i++)
                    {
                    RPointerArray<CLbsAreaInfoBase> PosInfoList;
                    static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray2[i])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
                    TInt count2 = PosInfoList.Count();
                    PosInfoList.ResetAndDestroy();
                    PosInfoList.Close();
                    if(count2>=1)
                        {
                        return KErrNone;
                        }
                    }
                }
            }
        return iResolverObserver->ConversionResult();        
        }
    }


// -----------------------------------------------------------------------------
// CStep::SetConversionPrefs
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::SetConversionPrefs(TLbsConversionPrefs aSetConversionPrefs)
    {
    TRAPD( err,
            if(iApiId != 0)
                {
                err = iLbsLocationResolver->SetConversionPreferences(aSetConversionPrefs);
                }
    )

    if (err)
        return err;
    else
        return KErrNone;
    }


// -----------------------------------------------------------------------------
// CStep::GetAndCheckConversionPrefs
// Converts one location info to its corresponding position info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CStep::GetAndCheckConversionPrefs(TLbsConversionPrefs aSetConversionPrefs)
    {
    TRAPD( err,
            if(iApiId != 0)
                {
                TLbsConversionPrefs readConversionPrefs = iLbsLocationResolver->ConversionPreferences();
                if (readConversionPrefs != aSetConversionPrefs)
                    {
                    err = KErrGeneral;
                    }
                }
    )
    if (err)
        return err;
    else
        return KErrNone;
    }



TInt CStep::Cleanup()
    {
    if(iLbsLocationInfo)
        {
        delete iLbsLocationInfo;
        iLbsLocationInfo = NULL;
        }
    if(iLbsLocationInfoConverter)
        {
        delete iLbsLocationInfoConverter;
        iLbsLocationInfoConverter = NULL;
        }
    if(iLbsLocationResolver)
        {
        delete iLbsLocationResolver;
        iLbsLocationResolver = NULL;
        }
    if(iObserver)
        {
        delete iObserver;
        iObserver = NULL;
        }
    if(iResolverObserver)
        {
        delete iResolverObserver;
        iResolverObserver = NULL;
        }
    iLbsLocationInfoArrayTemp.ResetAndDestroy();
    iLbsLocationInfoArray.Reset();
      
    return KErrNone;
    }

void CStep::test_2698_4_1_1_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 404, 49, 16, 7141);
    
    AddLocationInfoL();

    TRAP(err,ConvertLocationInfoL(1, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 0, 0, 0, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
  
    TRAP(err,ConvertLocationInfoL(0, 1, 0));

    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 999, 999, 65535, 65535);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));

    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, -1, 0, 0, 0);

    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));

    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, -1, 0, 0);

    AddLocationInfoL();
  
    TRAP (err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, -1, 0);
    
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP (err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, -1);
    
    AddLocationInfoL();
    
    TRAP (err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 1000, 0, 0, 0);
   
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 1000, 0, 0);
    
    AddLocationInfoL();
    
    TRAP (err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_11L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 65536, 0);
    
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_12L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 65536);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_1_13L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 30);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 999);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 0);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 1000);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, -1);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200);
    
    AddLocationInfoL();
   
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 999);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 0);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 1000);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_11L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, -1);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_12L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_13L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 65535);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_14L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 0);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_15L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 65536);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_16L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, -1);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_17L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 65535);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_18L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 0);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_19L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 65536);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_20L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, -1);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_21L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 30, 255);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_22L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 30, 0);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_23L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 30, -1);

    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_24L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 200, 300, 30, 256);
    
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_25L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    
    AddLocationInfoL();
    
    ConvertLocationInfoL(1, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_26L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    ConvertLocationInfoL(2, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_27L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    ConvertLocationInfoL(4, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 0, 0, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 999, 999, 65535);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, -1, 200, 300);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, -1, 300);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 1000, 200, 300);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 1000, 300);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_1_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 268435456);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_28_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
 
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_29L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
   
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_30L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 999);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
   
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_31L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
   
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_32L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 1000);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_33L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_34L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_35L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 999);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_36L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_37L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 1000);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_38L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_39L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 65535);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_40L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_41L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 268435456);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_42L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_43L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30, 0, 511);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_44L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_45L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30, 512);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_46L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_47_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 200);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_47_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_47_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(2, 100, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_49_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_49_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_51L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(1, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_52L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(2, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_1_53L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(2, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(4, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, 1, 0, 1, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 3, 63, 1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 2, 3, 10, 1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, -1, 1, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 64, 1, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 0, 63, 1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 63, 1, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 63, 1, 1024);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 63, 0, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_11L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_12L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 63, -1, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_13L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 1, 63, 64, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_14L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 0, 15, 46, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_15L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 0, 268435455, 15, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_16L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 2, 15, 44, 15, 46, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_17L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -1, 15);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_18L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 268435456, 15);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_19L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 0, 0, 0, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_20L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_21L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 0, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_2_22L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 268435455, 512);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_1_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_2_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_2_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(1, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(2, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(2, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 255);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, -1);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_3_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 256);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_4_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    err = MiscL(1);
    if(err != KErrCancel) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_4_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 255);
    if(err != KErrNone) User::Leave(err);
    err = MiscL(1);
    if(err != KErrCancel) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_4_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    err = MiscL(2);
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_4_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    err = MiscL(3);
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_4_5_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    if(iApiId == 0)
    	{
		err = MiscL(4);
        if(err != KErrNone) User::Leave(err);
    	}
    else
    	{
		TRAPD(leaveErr, MiscL(4));
		if(leaveErr != KErrInUse) User::Leave(leaveErr);
		CActiveScheduler::Start();
    	}
    
    Cleanup();
 
};


void CStep::test_2698_4_5_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, 60, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 60, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_7L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_8L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_9L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 1000, -1, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_10L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 1, 64, 64, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_11L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -1, 1, 63, 63, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_12L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 1, 63, 63, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_13L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 1000, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_14L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 268435456, 511);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_15L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 512, 1, 268435455, 511);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_16L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_17L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_18L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(1, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_19L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(2, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_20L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, -2, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(4, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_21L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(1, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_22L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(2, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_23L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, 511, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(4, 1, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_24L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(0, 0, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_25L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(0, 2, 2);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNotSupported) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_1_26L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, -2, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 4, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(iApiId == 0)
        {
        if(err != KErrNotSupported) User::Leave(err);
        }
    Cleanup();
 
};


void CStep::test_2698_5_1_27L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30, 1, -2, 268435455, 511, 47, 0);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWCDMACellInfoL(1, 100, 200, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 40, 63, 1, 1023, 63, 63);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 8, 2));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 200000, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 200000, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_5L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 256);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 2));
    if(err != KErrArgument) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_2_6L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(1, 100000, 3);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30, 63, 1, 63, 1023);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    TRAP(err,ConvertLocationInfoL(0, 1, 0));
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_3_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = MiscL(5);
    if(err != KErrCancel) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_3_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = SetWLanInfoL(2, 100000, 255);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = MiscL(5);
    if(err != KErrCancel) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_3_3L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = MiscL(6);
    // Unsure of the correct result here, should be an error after cancellation?
    if(err != KErrNotFound) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_3_4L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = MiscL(7);
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};


void CStep::test_2698_5_4_1L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 100, 200, 300, 30);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    err = MiscL(8);
    if(err != KErrNone) User::Leave(err);
    Cleanup();
 
};



void CStep::test_MultipleSessionsOneAfterAnotherL()
{
	TInt err = KErrNone;
	TUint Uidinteger = 537007117;      // Example
	TUid uid = TUid::Uid( Uidinteger );
	iLbsLocationInfo = CLbsLocationInfo::NewLC();
	CleanupStack::Pop(iLbsLocationInfo);
     
	iObserver = CTelocationinfoconversionobserver::NewL();
	iLbsLocationInfoConverter = CLbsLocationInfoConverter::NewL(*iObserver,uid);
	TBuf8<6> macidbuf;
	TInt macidint = 1;
	macidbuf.AppendNum(macidint);
	CLbsWlanInfo* wlaninfo;    
	TInt signalstrength = 20;
	wlaninfo=CLbsWlanInfo::NewL( macidbuf,signalstrength); 
	iLbsLocationInfo->AddAreaInfoL(wlaninfo);
    
	iLbsLocationInfoArrayTemp.Append(iLbsLocationInfo);

	iLbsLocationInfoArray.Append(iLbsLocationInfo);

	iLbsLocationInfo = CLbsLocationInfo::NewL();

	iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,ELbsConversionSilent,ELbsConversionOutputPosition);
 
      
	CActiveScheduler::Start();

	if( iObserver->ConversionResult() == KErrNone )
		{
		RPointerArray<CLbsAreaInfoBase> PosInfoList;
		static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
		TInt count2 = PosInfoList.Count();
		PosInfoList.ResetAndDestroy();
		}

	// Cleanup();
    
	err = KErrNone;
	TUint Uidinteger2 = 537007117;      // Example
	TUid uid2 = TUid::Uid( Uidinteger2 );
	iLbsLocationInfo2 = CLbsLocationInfo::NewL();
      
	iObserver2 = CTelocationinfoconversionobserver::NewL();
	iLbsLocationInfoConverter2 = CLbsLocationInfoConverter::NewL(*iObserver2,uid2);
	TBuf8<6> macidbuf2;
	TInt macidint2 = 1;
	macidbuf2.AppendNum(macidint2);
	CLbsWlanInfo* wlaninfo2;    
	TInt signalstrength2 = 20;
	wlaninfo2=CLbsWlanInfo::NewL( macidbuf2,signalstrength2); 
	iLbsLocationInfo2->AddAreaInfoL(wlaninfo2);
     
	iLbsLocationInfoArrayTemp.Append(iLbsLocationInfo2);

	iLbsLocationInfoArray.Append(iLbsLocationInfo2);

	iLbsLocationInfo2 = CLbsLocationInfo::NewL();

	iLbsLocationInfoConverter2->ConvertLocationInfoL(iLbsLocationInfoArray,ELbsConversionSilent,ELbsConversionOutputPosition);
  
	CActiveScheduler::Start();

	if( iObserver2->ConversionResult() == KErrNone )
		{
		RPointerArray<CLbsAreaInfoBase> PosInfoList;
		static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
		TInt count2 = PosInfoList.Count();
		PosInfoList.ResetAndDestroy();
		}

	Cleanup();
   
	if(iLbsLocationInfo2)
		{
		delete iLbsLocationInfo2;
		iLbsLocationInfo2 = NULL;
		}
	if(iLbsLocationInfoConverter2)
		{
		delete iLbsLocationInfoConverter2;
		iLbsLocationInfoConverter2 = NULL;
		}
     
	if(iObserver2)
		{
		delete iObserver2;
		iObserver2 = NULL;
		}

};

void CStep::test_MultipleCoversionsAtSameTimeL()
{	// two conversion at same time!
	TInt err = KErrNone;
	TUint Uidinteger = 537007117;      // Example
	TUid uid = TUid::Uid( Uidinteger );
	iLbsLocationInfo = CLbsLocationInfo::NewL();
     
	iObserver = CTelocationinfoconversionobserver::NewL();
	iLbsLocationInfoConverter = CLbsLocationInfoConverter::NewL(*iObserver,uid);
	TBuf8<6> macidbuf;
	TInt macidint = 1;
	macidbuf.AppendNum(macidint);
	CLbsWlanInfo* wlaninfo;    
	TInt signalstrength = 20;
	wlaninfo=CLbsWlanInfo::NewL( macidbuf,signalstrength); 
	iLbsLocationInfo->AddAreaInfoL(wlaninfo);
    
	iLbsLocationInfoArrayTemp.Append(iLbsLocationInfo);

	iLbsLocationInfoArray.Append(iLbsLocationInfo);

	iLbsLocationInfo = CLbsLocationInfo::NewL();

    err = KErrNone;
	TUint Uidinteger2 = 537007117;      // Example
	TUid uid2 = TUid::Uid( Uidinteger2 );
	iLbsLocationInfo2 = CLbsLocationInfo::NewL();
      
	iObserver2 = CTelocationinfoconversionobserver::NewL();
	iLbsLocationInfoConverter2 = CLbsLocationInfoConverter::NewL(*iObserver2,uid2);
	TBuf8<6> macidbuf2;
	TInt macidint2 = 1;
	macidbuf2.AppendNum(macidint2);
	CLbsWlanInfo* wlaninfo2;    
	TInt signalstrength2 = 20;
	wlaninfo2=CLbsWlanInfo::NewL( macidbuf2,signalstrength2); 
	iLbsLocationInfo2->AddAreaInfoL(wlaninfo2);
     
	iLbsLocationInfoArrayTemp2.Append(iLbsLocationInfo2);

	iLbsLocationInfoArray2.Append(iLbsLocationInfo2);

	iLbsLocationInfo2 = CLbsLocationInfo::NewL();

	iLbsLocationInfoConverter->ConvertLocationInfoL(iLbsLocationInfoArray,ELbsConversionSilent,ELbsConversionOutputPosition);
	iLbsLocationInfoConverter2->ConvertLocationInfoL(iLbsLocationInfoArray2,ELbsConversionSilent,ELbsConversionOutputPosition);
  
	CActiveScheduler::Start();

	TInt error = iObserver->ConversionResult(); 
	if( error == KErrNone )
		{
		RPointerArray<CLbsAreaInfoBase> PosInfoList;
		static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray[0])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
		TInt count2 = PosInfoList.Count();
		PosInfoList.ResetAndDestroy();
		}
	else
		{
		INFO_PRINTF2(_L("iObserver->ConversionResulterror = %d"), error);
			
		}
	
	CActiveScheduler::Start();

	error = iObserver2->ConversionResult();
	if( error == KErrNone )
		{
		RPointerArray<CLbsAreaInfoBase> PosInfoList;
		static_cast<CLbsLocationInfo*>(iLbsLocationInfoArray2[0])->GetAreaInfoL(PosInfoList,CLbsLocationInfo::ELbsPosInfo);
		TInt count2 = PosInfoList.Count();
		PosInfoList.ResetAndDestroy();
		}
	else
		{
		INFO_PRINTF2(_L("iObserver->ConversionResult error = %d"), error);
		User::Leave(error);
		}

	Cleanup();
   
	if(iLbsLocationInfo2)
		{
		delete iLbsLocationInfo2;
		iLbsLocationInfo2 = NULL;
		}
	if(iLbsLocationInfoConverter2)
		{
		delete iLbsLocationInfoConverter2;
		iLbsLocationInfoConverter2 = NULL;
		}
     
	if(iObserver2)
		{
		delete iObserver2;
		iObserver2 = NULL;
		}
    iLbsLocationInfoArrayTemp2.ResetAndDestroy();
    iLbsLocationInfoArray2.Reset();
};

void CStep::test_MultipleResolversAtSameTimeL()
{
    TInt err = KErrNone;
 
    iApiId =1; // use resolver
    	

    TUint Uidinteger = 537007117;      // Example
    TUid uid = TUid::Uid( Uidinteger );
    iLbsLocationInfo = CLbsLocationInfo::NewL();
      
    iResolverObserver = CTeLocationResolverObserver::NewL();
    iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid);


    iLbsLocationInfo2 = CLbsLocationInfo::NewL();
      
    iResolverObserver2 = CTeLocationResolverObserver::NewL();
    iLbsLocationResolver2 = CLbsLocationResolver::NewL(*iResolverObserver2,uid);

    SetGSMCellInfoL(1, 404, 49, 16, 7141);
    if(err != KErrNone) User::Leave(err);
    
    err = SetGSMCellInfo2L(1, 404, 49, 16, 7141);
    if(err != KErrNone) User::Leave(err);
    
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    
    AddLocationInfo2L();
    if(err != KErrNone) User::Leave(err);
    
    ConvertLocationInfoL(1, 1, 0);
    
    ConvertLocationInfo2L(1, 1, 0);
    
    err = ValidateLocationInfo();
    if(err != KErrNone) User::Leave(err);
    
    err = ValidateLocationInfo2();
    if(err != KErrNone) User::Leave(err);
    Cleanup();

//
    
    if(iLbsLocationInfo2)
        {
        delete iLbsLocationInfo2;
        iLbsLocationInfo2 = NULL;
        }
    if(iLbsLocationInfoConverter2)
        {
        delete iLbsLocationInfoConverter2;
        iLbsLocationInfoConverter2 = NULL;
        }
    if(iLbsLocationResolver2)
        {
        delete iLbsLocationResolver2;
        iLbsLocationResolver2 = NULL;
        }
    if(iObserver2)
        {
        delete iObserver2;
        iObserver2 = NULL;
        }
    if(iResolverObserver2)
        {
        delete iResolverObserver2;
        iResolverObserver2 = NULL;
        }
    iLbsLocationInfoArrayTemp2.ResetAndDestroy();
    iLbsLocationInfoArray2.Reset();
 
    //
};



void CStep::test_2698_5_4_2L()
{
    TInt err = KErrNone;
 
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    SetGSMCellInfoL(1, 404, 49, 16, 7141);
    if(err != KErrNone) User::Leave(err);
    AddLocationInfoL();
    if(err != KErrNone) User::Leave(err);
    ConvertLocationInfoL(1, 1, 0);
    if(err != KErrNone) User::Leave(err);
    err = ValidateLocationInfo();
    if(err != KErrNotFound) User::Leave(err);
    Cleanup();
};


void CStep::test_SetAndGetConversionPrefs()
{
    TInt err = KErrNone;
    
    err = CreateLocationInfoConverterL();
    if(err != KErrNone) User::Leave(err);
    err = GetAndCheckConversionPrefs(0);//default value ELbsConversionNotDefined (0x0000)
    if(err != KErrNone) User::Leave(err);
    err = SetConversionPrefs(4);    //ELbsConversionPrompt(0x0004)
    if(err != KErrNone) User::Leave(err);
    err = GetAndCheckConversionPrefs(4);
    if(err != KErrNone) User::Leave(err);
    err = SetConversionPrefs(15);
    if(err != KErrArgument) User::Leave(err);
    err = SetConversionPrefs(999999999);
    if(err != KErrArgument) User::Leave(err);
    err = GetAndCheckConversionPrefs(4);    //Check if previously set value has been retained
    if(err != KErrNone) User::Leave(err);
    err = SetConversionPrefs(64);    //Is a valid value though the current max in the enum list is only 4
    if(err != KErrNone) User::Leave(err);
    err = GetAndCheckConversionPrefs(64);    //Check if previously set value has been retained
    if(err != KErrNone) User::Leave(err);
    
    Cleanup();
};

typedef struct 
	{
	TInt 	countryCode;
	TInt 	networkCode;
	TInt 	locationCode;
	TInt 	cellId;
	TBool 	fromPlugin;
	} 
sGsmInfo;

/*
 * Here, we test the mode where localities coming from the converter are cache.
 * Thus the third and forth localities  should come from the cache and NOT the
 * plugin 
 */
const TInt KTestCacheArraySize= 4;
sGsmInfo testArray[KTestCacheArraySize] = {
		{44, 49, 16, 714, 	ETrue },
		{45, 50, 17, 714, 	ETrue},
		{45, 50, 17, 714, 	EFalse},
		{45, 50, 17, 714, 	EFalse}
};


void CStep::test_cacheL()
{
	   TInt err = KErrNone;
	   iApiId =1; // use resolver
   
	   TUint Uidinteger = 537007117;
	   TUid uid = TUid::Uid( Uidinteger );
	   
	   for (TUint i = 0; i < KTestCacheArraySize; i++)
		   {
			TRAPD(leaveErr, iLbsLocationInfo = CLbsLocationInfo::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationInfo = CLbsLocationInfo::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iResolverObserver = CTeLocationResolverObserver::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("CTeLocationResolverObserver::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid));
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,SetGSMCellInfoL(1, 
					testArray[i].countryCode, 
					testArray[i].networkCode,
					testArray[i].locationCode, 
					testArray[i].cellId , KTimingAdvanceSpecialValue)); // timing advance 254 is special!
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("SetGSMCellInfoL LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	
			TRAP(leaveErr, AddLocationInfoL());
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("AddLocationInfoL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	      
			TRAP(leaveErr, ConvertLocationInfoL(1, 1, 0));
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("ConvertLocationInfoL(1, 1, 0)) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	       
			TInt err = ValidateLocationInfo();
			if( KErrNone !=err)
				{
				INFO_PRINTF2(_L("ValidateLocationInfo() returns error =%d"), err);
				User::Leave(err);
				}
	    
			TLocality locality;
			TBool fromCache = iResolverObserver->ConversionLocality(locality);
	    
			TReal64 lat = locality.Latitude();
			TReal64 lng = locality.Longitude();
			TReal32 alt = locality.Altitude();
	
			TInt cCode = testArray[i].countryCode; 
			if (cCode != (TInt)lat)
				{
				INFO_PRINTF2(_L("VcCode != (TInt)lat error =%d"), KErrGeneral);
			
				User::Leave(KErrGeneral);
				}
	
			TInt nCode = testArray[i].networkCode; 
			if (nCode != (TInt)lng)
				{
				INFO_PRINTF2(_L("nCode != (TInt)lng error =%d"), KErrGeneral);
			
				User::Leave(KErrGeneral);
				}
					
			TInt lCode = testArray[i].locationCode; 
			if (lCode != (TInt)alt)
				{
				INFO_PRINTF2(_L("lCode != (TInt)alt error =%d"), KErrGeneral);

				User::Leave(KErrGeneral);
				}
	    

			if (fromCache)
				{
				INFO_PRINTF4(_L("From Cache(lat=%Lf, lng=%Lf, alt=%f"),lat,lng,alt);
				if (testArray[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from Cache bit should have come from plugin"));
					User::Leave(KErrGeneral);
					}
				}
			else
				{ // from plugin!
				INFO_PRINTF4(_L("From plugin(lat=%Lf, lng=%Lf, alt=%f"),lat,lng,alt);
				if (!testArray[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from plugin but should have come from Cache"));
					User::Leave(KErrGeneral);
					}
				}
			Cleanup();
		   } // end of for loop
};

const TInt KTest2ArraySize= 4;

/*
 * Here all four converts should from the cache as the
 * loc monitor has been instructed NOT to put things into the cache.
 * The timing advance is used by the test (in an adhoc way!)
 * to instruct the test plugin to tell the loc monitor NOT to put stuff
 * in the cache
 */
sGsmInfo testArray2[KTest2ArraySize] = {
		{44, 49, 16, 7141, 	ETrue },
		{45, 50, 17, 714, 	ETrue},
		{45, 50, 17, 714, 	ETrue},
		{45, 50, 17, 714, 	ETrue}
	
};
void CStep::test_cache2L()
{
		TInt err = KErrNone;
	 
		iApiId =1; // use resolver
   
		TUint Uidinteger = 537007117;      // Example
		TUid uid = TUid::Uid( Uidinteger );
		
		for (TUint i = 0; i < KTest2ArraySize; i++)
		   {
			TRAPD(leaveErr, iLbsLocationInfo = CLbsLocationInfo::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationInfo = CLbsLocationInfo::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iResolverObserver = CTeLocationResolverObserver::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("CTeLocationResolverObserver::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid));
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,SetGSMCellInfoL(1, 
					testArray2[i].countryCode, 
					testArray2[i].networkCode,
					testArray2[i].locationCode, 
					testArray2[i].cellId , KTimingAdvanceSaveToCacheFalse)); // timing advance is special!
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("SetGSMCellInfoL LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	
			TRAP(leaveErr, AddLocationInfoL());
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("AddLocationInfoL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	      
			TRAP(leaveErr, ConvertLocationInfoL(1, 1, 0));
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("ConvertLocationInfoL(1, 1, 0)) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	       
			TInt err = ValidateLocationInfo();
			if( KErrNone !=err)
				{
				INFO_PRINTF2(_L("ValidateLocationInfo() returns error =%d"), err);
				User::Leave(err);
				}
	    
			TLocality locality;
			TBool fromCache = iResolverObserver->ConversionLocality(locality);
	    
			TReal64 lat = locality.Latitude();
			TReal64 lng = locality.Longitude();
			TReal32 alt = locality.Altitude();
	
			TInt cCode = testArray2[i].countryCode; 
			if (cCode != (TInt)lat)
				{
				INFO_PRINTF2(_L("VcCode != (TInt)lat error =%d"), KErrGeneral);
			
				User::Leave(KErrGeneral);
				}
	
			TInt nCode = testArray2[i].networkCode; 
			if (nCode != (TInt)lng)
				{
				INFO_PRINTF2(_L("nCode != (TInt)lng error =%d"), KErrGeneral);
			
				User::Leave(KErrGeneral);
				}
					
			TInt lCode = testArray2[i].locationCode; 
			if (lCode != (TInt)alt)
				{
				INFO_PRINTF2(_L("lCode != (TInt)alt error =%d"), KErrGeneral);

				User::Leave(KErrGeneral);
				}
	    

			if (fromCache)
				{
				INFO_PRINTF4(_L("From Cache(lat=%Lf, lng=%Lf, alt=%f"),lat,lng,alt);
				if (testArray2[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from Cache bit should have come from plugin"));
					User::Leave(KErrGeneral);
					}
				}
			else
				{ // from plugin!
				INFO_PRINTF4(_L("From plugin(lat=%Lf, lng=%Lf, alt=%f"),lat,lng,alt);
				if (!testArray2[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from plugin but should have come from Cache"));
					User::Leave(KErrGeneral);
					}
				}
			Cleanup();
		   } // end of for loop
};



/* this tests the cache behaviour when a non-exact match occurs.
 * For example, when cache contains
 * CC=A, NC=B, LC=C, CID=D Lat=20.0, Long=30.0 Acc=5.0m
 * and we look up 
 * CC=A, NC=B, LC=X, CID=Y
 * then  CC=A, NC=B results in a partial match occurs and the accuracy
 * of Lat=20.0, Long=30.0 is the accuracy associated with a
 * partial CC=A, NC=B match (1000000.0)
 * Notes:
 * CC=A, NC=B, LC=C, CID=D Lat=20.0, Long=30.0 Acc=5.0m
 * followed by 
 * CC=A, NC=B, LC=C, CID=D Lat=20.0, Long=30.1 Acc=5.1m
 * results inn a single entry of 
 * CC=A, NC=B, LC=C, CID=D Lat=20.0, Long=30.1 Acc=5.1m in cache
 * KWlanAccuracy 			= 100.0; - not used currently
 * KCellFullAccuracy 		= 3000.0; CC and NC and LC and CID
 * KCellLocationAccuracy 	= 30000.0; CC and NC and LC match
 * KCellCountryAccuracy 	= 1000000.0; CC only or CC + NC
 * 
 * If in cache we have:
 * CC=A, NC=B, LC=C, CID=D Lat=20.0, Long=30.0 Acc=5.0m
 * CC=A, NC=B, LC=C, CID=E Lat=20.1, Long=30.0 Acc=6.0m
 * 
 * and we try to match
 * CC=A, NC=B, LC=C
 * then we get from the cache the most recent and not the most accurate
 * Lat=20.1, Long=30.0 Acc=3000.0
 * 
 */


typedef struct 
	{
	TInt 	countryCode; 	// -> lat of position composed by plugin
	TInt 	networkCode; 	// -> long of position composed by plugin
	TInt 	locationCode; 	// -> alt of position composed by plugin
	TInt 	cellId;			// -> horizontal accuracy of position composed by plugin
	TBool 	fromPlugin; 	// when false then it is expected that locality comes out of cache 
	TInt	expectedHorAccuracyWhenFromCache; // and these values below are the expected values 
	TInt	expectedLatitudeWhenFromCache;
	TInt	expectedLongitudeWhenFromCache; // note absence of Vertical accuracy
											// here - as it is set to Nan by the lc monitor
	} 
sGsmInfoExtended;

const TInt KTestArraySize= 18;

sGsmInfoExtended testArray3[KTestArraySize] = {
// test the case where there a non-exact match
// and the position returned by the plugin is less accurate
// than the one deduced from the cache
//   cc	nc	 lc  cid

	{44, 49, 16, 714, 		ETrue, 		-1, 	-1,	-1}, //stored in cash
	{45, 50, 17, 714, 		ETrue, 		-1, 	-1,	-1},
	{45, 50, 17, 30001, 	EFalse,		30000,	45,	50},// not saved in cache - cc and nc and lc match with plugins less accurate than cached
	{45, 50, 18, 1000001, 	EFalse,		1000000,45,	50},// not saved in cache - cc and nc  match with plugins less accurate than cached 
	{45, 51, 18, 1000001,	EFalse,		1000000,45,	50},// not saved in cache -cc only match with plugins less accurate than cached
	{45, 50, 17, 714, 		EFalse,		3000,	45,	50},// exact match - to cellid level 
														// regardless, should use cache
	// test the case where there a non-exact match
	// and the position returned by the plugin is more accurate
	// than the one deduced from the cache
	{54, 49, 16, 714, 		ETrue,		-1, 	-1,	-1},
	{55, 50, 17, 714, 		ETrue,		-1, 	-1,	-1},
	{55, 50, 17, 2999, 		ETrue,		-1, 	-1,	-1},		// cc and nc and lc match with plugins less accurate than cached
	{55, 50, 18, 29999, 	ETrue,		-1, 	-1,	-1},		// cc and nc  match with plugins less accurate than cached 
	{55, 51, 18, 999999,	ETrue,		-1, 	-1,	-1},		// cc only match with plugins less accurate than cached
	{55, 50, 17, 714, 		EFalse,		3000, 	55,	50},		// exact match - to cellid level 
																// regardless, should use cache
	// now check that cache contains all the exact matches it should
	// note that several of the entries above were NOT saved in cache
	// because the value returned by the plugin was not accurate enough
	{44, 49, 16, 714,		EFalse,		3000, 	44,	49},
	{45, 50, 17, 714, 		EFalse,		3000, 	45,	50},
	{45, 50, 17, 714, 		EFalse,		3000, 	45,	50},
	{54, 49, 16, 714, 		EFalse,		3000, 	54,	49},
	{45, 50, 17, 714, 		EFalse,		3000, 	45,	50},
	{45, 50, 17, 714,		EFalse,		3000, 	45,	50}
};

void CStep::test_cache3L()
{
	   TInt err = KErrNone;
	 
		iApiId =1; // use resolver
   
		TUint Uidinteger = 537007117;      // Example
		TUid uid = TUid::Uid( Uidinteger );
		
		for (TUint i = 0; i < KTestArraySize; i++)
		   {
		
			TRAPD(leaveErr, iLbsLocationInfo = CLbsLocationInfo::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationInfo = CLbsLocationInfo::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iResolverObserver = CTeLocationResolverObserver::NewL());
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("CTeLocationResolverObserver::NewL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid));
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("iLbsLocationResolver = CLbsLocationResolver::NewL(*iResolverObserver,uid) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
			
			TRAP(leaveErr,SetGSMCellInfoL(1, 
					testArray3[i].countryCode, 
					testArray3[i].networkCode,
					testArray3[i].locationCode, 
					testArray3[i].cellId , KTimingAdvanceSaveToCacheTrue)); // timing advance is special!
			if (KErrNone != leaveErr )
				{
				INFO_PRINTF2(_L("SetGSMCellInfoL LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	
			TRAP(leaveErr, AddLocationInfoL());
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("AddLocationInfoL() LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	      
			TRAP(leaveErr, ConvertLocationInfoL(1, 1, 0));
			if( KErrNone !=leaveErr) 
				{
				INFO_PRINTF2(_L("ConvertLocationInfoL(1, 1, 0)) LEAVES, error =%d"), leaveErr);
				User::Leave(leaveErr);
				}
	       
			TInt err = ValidateLocationInfo();
			if( KErrNone !=err)
				{
				INFO_PRINTF2(_L("ValidateLocationInfo() returns error =%d"), err);
				User::Leave(err);
				}
	    
			TLocality locality;
			TBool fromCache = iResolverObserver->ConversionLocality(locality);
	    
			TReal64 lat = locality.Latitude();
			TReal64 lng = locality.Longitude();
			TReal32 alt = locality.Altitude();
	
			TReal32 horAcc = locality.HorizontalAccuracy();
			


			if (fromCache)
				{
				INFO_PRINTF5(_L("From Cache(lat=%Lf, lng=%Lf, alt=%f horacc=%f"),lat,lng,alt,horAcc);
				if (testArray3[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from Cache bit should have come from plugin"));
					User::Leave(KErrGeneral);
					}
				// now check horizontal accuracy
				TInt expectedAccuracy = testArray3[i].expectedHorAccuracyWhenFromCache;
				if ( expectedAccuracy != (TInt)horAcc)
					{
					INFO_PRINTF1(_L("Test failed - incorrect horizontal accuracy read from plugin"));
					INFO_PRINTF3(_L("Expected %d and got %d"),expectedAccuracy,(TInt)horAcc);
								
					User::Leave(KErrGeneral);
					}
				// now check latitude
				TInt expectedLatitude = testArray3[i].expectedLatitudeWhenFromCache;
				if ( expectedLatitude != (TInt)lat)
					{
					INFO_PRINTF1(_L("Test failed - incorrect latitude read from plugin"));
					INFO_PRINTF3(_L("Expected %d and got %d"),expectedLatitude,(TInt)lat);
								
					User::Leave(KErrGeneral);
					}

				// now check longitude
				TInt expectedLongitude = testArray3[i].expectedLongitudeWhenFromCache;
				if ( expectedLongitude != (TInt)lng)
					{
					INFO_PRINTF1(_L("Test failed - incorrect Longitude read from plugin"));
					INFO_PRINTF3(_L("Expected %d and got %d"),expectedLongitude,(TInt)lng);
									
					User::Leave(KErrGeneral);
					}

				}
			else
				{ // from plugin!
				INFO_PRINTF5(_L("From plugin(lat=%Lf, lng=%Lf, alt=%f horacc=%f"),lat,lng,alt,horAcc);
				if (!testArray3[i].fromPlugin)
					{
					INFO_PRINTF1(_L("Test failed - Position got from plugin but should have come from Cache"));
					User::Leave(KErrGeneral);
					}
				// now check horizontal accuracy
				TInt cellId = testArray3[i].cellId; 
				if ( cellId != (TInt)horAcc)
					{
					INFO_PRINTF1(_L("Test failed - incorrect horizontal accuracy read from plugin"));
					INFO_PRINTF3(_L("Expected %d and got %d"),cellId,(TInt)horAcc);

					User::Leave(KErrGeneral);
					}
				TInt cCode = testArray3[i].countryCode; 
				if (cCode != (TInt)lat)
					{
					INFO_PRINTF2(_L("VcCode != (TInt)lat error =%d"), KErrGeneral);
				
					User::Leave(KErrGeneral);
					}
		
				TInt nCode = testArray3[i].networkCode; 
				if (nCode != (TInt)lng)
					{
					INFO_PRINTF2(_L("nCode != (TInt)lng error =%d"), KErrGeneral);
				
					User::Leave(KErrGeneral);
					}
						
				TInt lCode = testArray3[i].locationCode; 
				if (lCode != (TInt)alt)
					{
					INFO_PRINTF2(_L("lCode != (TInt)alt error =%d"), KErrGeneral);

					User::Leave(KErrGeneral);
					}
		    
				}
			Cleanup();
		   } // end of for loop
};
void CStep::CallL(TInt aId)
{
    switch(aId)
    {
//        case 0: GetAllLocationInfoL(); break;
    	case KErrNotFound:
    		INFO_PRINTF1(_L("Test case not found"));
    		User::Leave(KErrNotFound);
    		break;
    		
        case 1: test_2698_4_1_1_1L(); break;
        case 2: test_2698_4_1_1_3L(); break;
        case 3: test_2698_4_1_1_4L(); break;
        case 4: test_2698_4_1_1_5L(); break;
        case 5: test_2698_4_1_1_6L(); break;
        case 6: test_2698_4_1_1_7L(); break;
        case 7: test_2698_4_1_1_8L(); break;
        case 8: test_2698_4_1_1_9L(); break;
        case 9: test_2698_4_1_1_10L(); break;
        case 10: test_2698_4_1_1_11L(); break;
        case 11: test_2698_4_1_1_12L(); break;
        case 12: test_2698_4_1_1_13L(); break;
        case 13: test_2698_4_1_2L(); break;
        case 14: test_2698_4_1_3L(); break;
        case 15: test_2698_4_1_4L(); break;
        case 16: test_2698_4_1_5L(); break;
        case 17: test_2698_4_1_6L(); break;
        case 18: test_2698_4_1_7L(); break;
        case 19: test_2698_4_1_8L(); break;
        case 20: test_2698_4_1_9L(); break;
        case 21: test_2698_4_1_10L(); break;
        case 22: test_2698_4_1_11L(); break;
        case 23: test_2698_4_1_12L(); break;
        case 24: test_2698_4_1_13L(); break;
        case 25: test_2698_4_1_14L(); break;
        case 26: test_2698_4_1_15L(); break;
        case 27: test_2698_4_1_16L(); break;
        case 28: test_2698_4_1_17L(); break;
        case 29: test_2698_4_1_18L(); break;
        case 30: test_2698_4_1_19L(); break;
        case 31: test_2698_4_1_20L(); break;
        case 32: test_2698_4_1_21L(); break;
        case 33: test_2698_4_1_22L(); break;
        case 34: test_2698_4_1_23L(); break;
        case 35: test_2698_4_1_24L(); break;
        case 36: test_2698_4_1_25L(); break;
        case 37: test_2698_4_1_26L(); break;
        case 38: test_2698_4_1_27L(); break;
        case 39: test_2698_4_1_28_1L(); break;
        case 40: test_2698_4_1_28_1_3L(); break;
        case 41: test_2698_4_1_28_1_4L(); break;
        case 42: test_2698_4_1_28_1_5L(); break;
        case 43: test_2698_4_1_28_1_6L(); break;
        case 44: test_2698_4_1_28_1_7L(); break;
        case 45: test_2698_4_1_28_1_8L(); break;
        case 46: test_2698_4_1_28_1_9L(); break;
        case 47: test_2698_4_1_28_1_10L(); break;
        case 48: test_2698_4_1_28_2L(); break;
        case 49: test_2698_4_1_29L(); break;
        case 50: test_2698_4_1_30L(); break;
        case 51: test_2698_4_1_31L(); break;
        case 52: test_2698_4_1_32L(); break;
        case 53: test_2698_4_1_33L(); break;
        case 54: test_2698_4_1_34L(); break;
        case 55: test_2698_4_1_35L(); break;
        case 56: test_2698_4_1_36L(); break;
        case 57: test_2698_4_1_37L(); break;
        case 58: test_2698_4_1_38L(); break;
        case 59: test_2698_4_1_39L(); break;
        case 60: test_2698_4_1_40L(); break;
        case 61: test_2698_4_1_41L(); break;
        case 62: test_2698_4_1_42L(); break;
        case 63: test_2698_4_1_43L(); break;
        case 64: test_2698_4_1_44L(); break;
        case 65: test_2698_4_1_45L(); break;
        case 66: test_2698_4_1_46L(); break;
        case 67: test_2698_4_1_47_1L(); break;
        case 68: test_2698_4_1_47_2L(); break;
        case 69: test_2698_4_1_47_3L(); break;
        case 70: test_2698_4_1_49_1L(); break;
        case 71: test_2698_4_1_49_2L(); break;
        case 72: test_2698_4_1_51L(); break;
        case 73: test_2698_4_1_52L(); break;
        case 74: test_2698_4_1_53L(); break;
        case 75: test_2698_4_2_1L(); break;
        case 76: test_2698_4_2_2L(); break;
        case 77: test_2698_4_2_3L(); break;
        case 78: test_2698_4_2_4L(); break;
        case 79: test_2698_4_2_5L(); break;
        case 80: test_2698_4_2_6L(); break;
        case 81: test_2698_4_2_7L(); break;
        case 82: test_2698_4_2_8L(); break;
        case 83: test_2698_4_2_9L(); break;
        case 84: test_2698_4_2_10L(); break;
        case 85: test_2698_4_2_11L(); break;
        case 86: test_2698_4_2_12L(); break;
        case 87: test_2698_4_2_13L(); break;
        case 88: test_2698_4_2_14L(); break;
        case 89: test_2698_4_2_15L(); break;
        case 90: test_2698_4_2_16L(); break;
        case 91: test_2698_4_2_17L(); break;
        case 92: test_2698_4_2_18L(); break;
        case 93: test_2698_4_2_19L(); break;
        case 94: test_2698_4_2_20L(); break;
        case 95: test_2698_4_2_21L(); break;
        case 96: test_2698_4_2_22L(); break;
        case 97: test_2698_4_3_1_1L(); break;
        case 98: test_2698_4_3_2_1L(); break;
        case 99: test_2698_4_3_2_2L(); break;
        case 100: test_2698_4_3_4L(); break;
        case 101: test_2698_4_3_5L(); break;
        case 102: test_2698_4_3_6L(); break;
        case 103: test_2698_4_3_7L(); break;
        case 104: test_2698_4_3_8L(); break;
        case 105: test_2698_4_3_9L(); break;
        case 106: test_2698_4_3_10L(); break;
        case 107: test_2698_4_4_1L(); break;
        case 108: test_2698_4_4_2L(); break;
        case 109: test_2698_4_4_3L(); break;
        case 110: test_2698_4_4_4L(); break;
        case 111: test_2698_4_5_1L(); break;
        case 112: test_2698_4_5_2L(); break;
        case 113: test_2698_5_1_1L(); break;
        case 114: test_2698_5_1_2L(); break;
        case 115: test_2698_5_1_3L(); break;
        case 116: test_2698_5_1_4L(); break;
        case 117: test_2698_5_1_5L(); break;
        case 118: test_2698_5_1_6L(); break;
        case 119: test_2698_5_1_7L(); break;
        case 120: test_2698_5_1_8L(); break;
        case 121: test_2698_5_1_9L(); break;
        case 122: test_2698_5_1_10L(); break;
        case 123: test_2698_5_1_11L(); break;
        case 124: test_2698_5_1_12L(); break;
        case 125: test_2698_5_1_13L(); break;
        case 126: test_2698_5_1_14L(); break;
        case 127: test_2698_5_1_15L(); break;
        case 128: test_2698_5_1_16L(); break;
        case 129: test_2698_5_1_17L(); break;
        case 130: test_2698_5_1_18L(); break;
        case 131: test_2698_5_1_19L(); break;
        case 132: test_2698_5_1_20L(); break;
        case 133: test_2698_5_1_21L(); break;
        case 134: test_2698_5_1_22L(); break;
        case 135: test_2698_5_1_23L(); break;
        case 136: test_2698_5_1_24L(); break;
        case 137: test_2698_5_1_25L(); break;
        case 138: test_2698_5_1_26L(); break;
        case 139: test_2698_5_1_27L(); break;
        case 140: test_2698_5_2_1L(); break;
        case 141: test_2698_5_2_2L(); break;
        case 142: test_2698_5_2_3L(); break;
        case 143: test_2698_5_2_4L(); break;
        case 144: test_2698_5_2_5L(); break;
        case 145: test_2698_5_2_6L(); break;
        case 146: test_2698_5_3_1L(); break;
        case 147: test_2698_5_3_2L(); break;
        case 148: test_2698_5_3_3L(); break;
        case 149: test_2698_5_3_4L(); break;
        case 150: test_2698_5_4_1L(); break;
        case 151: test_MultipleCoversionsAtSameTimeL(); break;
        case 152: test_MultipleSessionsOneAfterAnotherL(); break;
        case 153: test_MultipleResolversAtSameTimeL(); break;
        case 154: test_2698_5_4_2L(); break;
        case 155: test_SetAndGetConversionPrefs(); break;
        case 156: test_cacheL(); break;
        case 157: test_cache2L(); break;
        case 158: test_cache3L(); break;
    };
};
