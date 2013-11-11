/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CdummyMvmtDet class.
*
*/



#include <ecom/ecom.h>
#include <e32math.h>
#include <ecom/implementationproxy.h>
#include "TestLocationConverter3.h"

// the test code sets the timing advance in the GSM vell info to this value
// which instructs the test converter to constuct TLocalitys with
// latitude ser to country code, longitude set to 
// network code and altitiude set to local area code
const TInt KTimingAdvanceSpecialValue= 254;

const TInt KTimingAdvanceSaveToCacheTrue= 253;

const TInt KTimingAdvanceSaveToCacheFalse= 252;

// ECOM implementation specifics
static const TImplementationProxy implTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY(0x2002170D, CTestLocationConverter3::NewL)
	}; 


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(implTable) / sizeof(TImplementationProxy);

    return implTable;
    }
 

// Class methods

//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::NewL
// Symbian Two - phase constructor
//------------------------------------------------------------
//
CTestLocationConverter3* CTestLocationConverter3::NewL( TLbsLocInfoConverterPluginParams& aParams)
	{
	CTestLocationConverter3* self = new (ELeave) CTestLocationConverter3(aParams);
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	CleanupStack::Pop();
	return self;
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ConstructL
//------------------------------------------------------------
//
void CTestLocationConverter3::ConstructL( TLbsLocInfoConverterPluginParams& /*aParams*/ )
	{
	

	}


//-----------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::CLbsLocInfoConversionPluginBase
// C++ Default constructor
//------------------------------------------------------------------
//
CTestLocationConverter3::CTestLocationConverter3(TLbsLocInfoConverterPluginParams& aParams):iObserver(aParams.Observer()),
		iRetainLocalityInCache(ETrue)
	{
	
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::~CLbsLocInfoConversionPluginBase
// Destructor
//------------------------------------------------------------
//
CTestLocationConverter3::~CTestLocationConverter3()
	{
	
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ConvertLocationInfoL
//------------------------------------------------------------
//	
void CTestLocationConverter3::ConvertLocationInfoL( RLbsLocationInfoArray& aLocationInfoArray,
                                                       const TLbsConversionPrefs /*aConversionPrefs*/, 
                                                        const TLbsConversionOutputInfoMask aRequestedInfo)
	{
	if(aRequestedInfo == ELbsConversionOutputNotDefined)
	   {
	   iObserver.OnConversionComplete(KErrArgument);
	   return;
	   }
	TInt count = aLocationInfoArray.Count();	
	for(TInt i=0 ; i<count;i++)
	    {
	    switch(aRequestedInfo)
    	    {
    	    case ELbsConversionOutputPosition:
    	        {
    	        // Extract the area info provided by the client.
    	        // If client has specified only coordinate info,
    	        // return KErrNotSupported.
    	        RLbsAreaInfoBaseArray areaInfoArray; 
    	        CLbsLocationInfo::TAreaInfoMask areaInfoMask = 0;
    	        areaInfoMask |= CLbsLocationInfo::ELbsGsmInfo | 
                                CLbsLocationInfo::ELbsWcdmaInfo |
                                CLbsLocationInfo::ELbsWlanInfo;
    	        aLocationInfoArray[i]->GetAreaInfoL( areaInfoArray,areaInfoMask );
    	        TInt count = areaInfoArray.Count();
    	        
    	
    	        
    	        if( count == 0 )
    	            {
    	            iObserver.OnConversionComplete(KErrNotSupported);
    	            return;
    	            }
    	        //
    	        CLbsGsmCellInfo* posInfo = static_cast<CLbsGsmCellInfo*>( areaInfoArray[0] );
    	                   
    	        TInt timingAdvance = posInfo->TimingAdvance();
    	        
    	    	TLocality locality;
    	        if (timingAdvance == KTimingAdvanceSpecialValue)
    	        	{
					TReal64 lat = posInfo->MobileCountryCode() ;
					TReal64 lng = posInfo->MobileNetworkCode();
					TReal32 alt = posInfo->LocationAreaCode();
					 locality.SetCoordinate( lat,lng, alt);
    	        	}
    	        else 
    	        	{
				    locality.SetCoordinate( 62.5285,23.9385, 1.22f);
    	        	}
    	           
//     	        if (timingAdvance == KTimingAdvanceSaveToCacheTrue)
//        	        {
//					TReal64 lat = posInfo->MobileCountryCode() ;
//					TReal64 lng = posInfo->MobileNetworkCode();
//					TReal32 alt = posInfo->LocationAreaCode();
//					locality.SetCoordinate( lat,lng, alt);
//    				iRetainLocalityInCache = ETrue;
//        	        }
//     	        else if (timingAdvance == KTimingAdvanceSaveToCacheFalse)
//     	        	{
//					TReal64 lat = posInfo->MobileCountryCode() ;
//					TReal64 lng = posInfo->MobileNetworkCode();
//					TReal32 alt = posInfo->LocationAreaCode();
//					locality.SetCoordinate( lat,lng, alt);
//					iRetainLocalityInCache = EFalse;
//     	        	}
     	        
     	       if (timingAdvance == KTimingAdvanceSaveToCacheFalse)
					{
					TReal64 lat = posInfo->MobileCountryCode();
					TReal64 lng = posInfo->MobileNetworkCode();
					TReal32 alt = posInfo->LocationAreaCode();
					locality.SetCoordinate(lat, lng, alt);
					iRetainLocalityInCache = EFalse;
					}
				else
					{
					iRetainLocalityInCache = ETrue;
					}
     	        
     	        
    	        areaInfoArray.ResetAndDestroy();
     
    	        locality.SetAccuracy( 100,0.0 );
    	        CLbsPositionInfo* positionInfo = CLbsPositionInfo::NewL( locality );
    	        CleanupStack::PushL( positionInfo );
    	        aLocationInfoArray[i]->AddAreaInfoL( positionInfo );
    	        CleanupStack::Pop( positionInfo );
    	        break;
    	        }
    	    case ELbsConversionOutputGsm:
    	        {
                // Extract the area info provided by the client.
    	        // If client has specified only gsm info,
    	        // return KErrNotSupported.
    	        RLbsAreaInfoBaseArray areaInfoArray; 
                CLbsLocationInfo::TAreaInfoMask areaInfoMask = 0;
                areaInfoMask |= CLbsLocationInfo::ELbsPosInfo | 
                                CLbsLocationInfo::ELbsWcdmaInfo |
                                CLbsLocationInfo::ELbsWlanInfo;
                aLocationInfoArray[i]->GetAreaInfoL( areaInfoArray,areaInfoMask );
                TInt count = areaInfoArray.Count();
                areaInfoArray.ResetAndDestroy();
                
                if( count == 0 )
                    {
                    iObserver.OnConversionComplete(KErrNotSupported);
                    return;
                    }
                    
    	        CLbsGsmCellInfo* gsmCellInfo = CLbsGsmCellInfo::NewL( 100,101,102,103);
    	        CleanupStack::PushL( gsmCellInfo );
    	        aLocationInfoArray[i]->AddAreaInfoL( gsmCellInfo );
                CleanupStack::Pop( gsmCellInfo );
                break;
    	        }
    	    case ELbsConversionOutputWcdma:
    	        {
                // Extract the area info provided by the client.
                // If client has specified only wcdma info,
                // return KErrNotSupported.
                RLbsAreaInfoBaseArray areaInfoArray; 
                CLbsLocationInfo::TAreaInfoMask areaInfoMask = 0;
                areaInfoMask |= CLbsLocationInfo::ELbsPosInfo | 
                                CLbsLocationInfo::ELbsGsmInfo |
                                CLbsLocationInfo::ELbsWlanInfo;
                aLocationInfoArray[i]->GetAreaInfoL( areaInfoArray,areaInfoMask );
                TInt count = areaInfoArray.Count();
                areaInfoArray.ResetAndDestroy();
                
                if( count == 0 )
                    {
                    iObserver.OnConversionComplete(KErrNotSupported);
                    return;
                    }
    	        
    	        CLbsWcdmaCellInfo* wcdmaCellInfo = CLbsWcdmaCellInfo::NewL( 100,101,102 );
    	        CleanupStack::PushL( wcdmaCellInfo );
                aLocationInfoArray[i]->AddAreaInfoL( wcdmaCellInfo );
                CleanupStack::Pop( wcdmaCellInfo );
                break;
    	        }
    	    case ELbsConversionOutputWlan:
    	        {
                // Extract the area info provided by the client.
                // If client has specified only wlan info,
                // return KErrNotSupported.
                RLbsAreaInfoBaseArray areaInfoArray; 
                CLbsLocationInfo::TAreaInfoMask areaInfoMask = 0;
                areaInfoMask |= CLbsLocationInfo::ELbsPosInfo | 
                                CLbsLocationInfo::ELbsGsmInfo |
                                CLbsLocationInfo::ELbsWcdmaInfo;
                aLocationInfoArray[i]->GetAreaInfoL( areaInfoArray,areaInfoMask );
                TInt count = areaInfoArray.Count();
                areaInfoArray.ResetAndDestroy();
                
                if( count == 0 )
                    {
                    iObserver.OnConversionComplete(KErrNotSupported);
                    return;
                    }
                
    	        TLbsWlanMacAddress macAddress(_L8("10.0.1"));
    	        CLbsWlanInfo* WlanInfo = CLbsWlanInfo::NewL( macAddress,10.1);
    	        CleanupStack::PushL( WlanInfo );
                aLocationInfoArray[i]->AddAreaInfoL( WlanInfo );
                CleanupStack::Pop( WlanInfo );
    	        }
    	    default:
    	        break;
    	    }
	    
	    }
	iObserver.OnConversionComplete(KErrNone);
	}

//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ConvertLocationInfoL
//------------------------------------------------------------
//  
void CTestLocationConverter3::CancelConvertLocationInfo( )
    {
     
    }

//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ValidateClient
//------------------------------------------------------------
//  
void CTestLocationConverter3::ValidateClient(  const TSecureId& /*aSecureId*/, 
                                                 const TVendorId& /*aVendorId*/,
                                                 const TLbsConversionOutputInfoMask /*aRequestedInfo*/ )
    {
    iObserver.OnValidationComplete( KErrNone ); 
    }
	
//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ValidateClient
//------------------------------------------------------------
//  
void CTestLocationConverter3::CancelValidateClient()
    {

    }

//------------------------------------------------------------
// CExampleLocationConverter::ConversionPluginPreferences
//------------------------------------------------------------
// 
void CTestLocationConverter3::ConversionPluginPreferences(
                 TLbsConversionPluginInfo& aPluginInfo )
	{
	aPluginInfo.SetIsPluginPositionToBeCached(iRetainLocalityInCache);
	//aPluginInfo.SetIsPluginPositionToBeCached(ETrue);
	}
