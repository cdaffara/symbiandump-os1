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
#include "ExampleLocationConverter.h"

// the test code sets the timing advance in the GSM vell info to this value
// which instructs the test converter to constuct TLocalitys with
// latitude ser to country code, longitude set to 
// network code and altitiude set to local area code
const TInt KTimingAdvanceSpecialValue= 254;

const TInt KTimingAdvanceSaveToCacheTrue= 253;

const TInt KTimingAdvanceSaveToCacheFalse= 252;

const TInt KScramblingCodeWcdmaTests = 254;


// ECOM implementation specifics
static const TImplementationProxy implTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY(0x2002140D, CExampleLocationConverter::NewL)
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
CExampleLocationConverter* CExampleLocationConverter::NewL( TLbsLocInfoConverterPluginParams& aParams)
	{
	CExampleLocationConverter* self = new (ELeave) CExampleLocationConverter(aParams);
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	CleanupStack::Pop();
	return self;
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ConstructL
//------------------------------------------------------------
//
void CExampleLocationConverter::ConstructL( TLbsLocInfoConverterPluginParams& /*aParams*/ )
	{
    CLbsLocInfoConverterPluginBase::Version();
    TInt aFunctionNumber = 1982; 
    TAny* aPtr1 = &aFunctionNumber;
    TAny* aPtr2 = &aFunctionNumber;
    CLbsLocInfoConverterPluginBase::ExtendedInterface(aFunctionNumber, aPtr1, aPtr2);
    
    iObserver.Version();
	}


//-----------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::CLbsLocInfoConversionPluginBase
// C++ Default constructor
//------------------------------------------------------------------
//
CExampleLocationConverter::CExampleLocationConverter(TLbsLocInfoConverterPluginParams& aParams):iObserver(aParams.Observer()),
		iRetainLocalityInCache(ETrue)
	{
	
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::~CLbsLocInfoConversionPluginBase
// Destructor
//------------------------------------------------------------
//
CExampleLocationConverter::~CExampleLocationConverter()
	{
	
	}


//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ConvertLocationInfoL
//------------------------------------------------------------
//	
void CExampleLocationConverter::ConvertLocationInfoL( RLbsLocationInfoArray& aLocationInfoArray,
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
				TBool specialTreatmentOfPos = EFalse;

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

    	        TLocality locality;
				if (areaInfoArray[0]->Type() == ELbsAreaGsmCellInfoClass)
					{
					CLbsGsmCellInfo* posInfo =
							static_cast<CLbsGsmCellInfo*> (areaInfoArray[0]);

					TInt timingAdvance = posInfo->TimingAdvance();

					if (timingAdvance == KTimingAdvanceSaveToCacheFalse)
						{
						iRetainLocalityInCache = EFalse;
						}
					else
						{
						// for all others values ensure that we put
						// localities in cache
						iRetainLocalityInCache = ETrue;
						}

					if ((timingAdvance == KTimingAdvanceSpecialValue)
							|| (timingAdvance
									== KTimingAdvanceSaveToCacheTrue)
							|| (timingAdvance
									== KTimingAdvanceSaveToCacheFalse))
						{
						// For all the special GSM tests
						TReal64 lat = posInfo->MobileCountryCode();
						TReal64 lng = posInfo->MobileNetworkCode();
						TReal32 alt = posInfo->LocationAreaCode();

						TReal32 horAcc = posInfo->CellId();

						locality.SetCoordinate(lat, lng, alt);
						locality.SetAccuracy(horAcc, 11.0);

						specialTreatmentOfPos = ETrue;
						}
					}
				else if (areaInfoArray[0]->Type() == ELbsAreaWcmdaCellInfoClass)
					{
					iRetainLocalityInCache = ETrue;
					
					CLbsWcdmaCellInfo* posInfo =
							static_cast<CLbsWcdmaCellInfo*> (areaInfoArray[0]);

					if (posInfo->ScramblingCode() == KScramblingCodeWcdmaTests )
						{
						// For all the special wcdma tests
						// note we don't set the horiz accuracy to same as cell id (as in GSM tests) as this
						// makes the accuracy too low!
						TReal64 lat = posInfo->MobileCountryCode();
						TReal64 lng = posInfo->MobileNetworkCode();
						TReal32 alt = 25.6;

						locality.SetCoordinate(lat, lng, alt);
						locality.SetAccuracy(24.3, 11.0);

						specialTreatmentOfPos = ETrue;
						}
					}

				// For the original tests we stick with a set position value
				if (!specialTreatmentOfPos)
					{
					locality.SetCoordinate(62.5285, 23.9385, 1.22f);
					locality.SetAccuracy(10.0f, 0.0);
					}

    	        areaInfoArray.ResetAndDestroy();
     
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
void CExampleLocationConverter::CancelConvertLocationInfo( )
    {
     
    }

//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ValidateClient
//------------------------------------------------------------
//  
void CExampleLocationConverter::ValidateClient(  const TSecureId& /*aSecureId*/, 
                                                 const TVendorId& /*aVendorId*/,
                                                 const TLbsConversionOutputInfoMask /*aRequestedInfo*/ )
    {
    iObserver.OnValidationComplete( KErrNone ); 
    }
	
//------------------------------------------------------------
// CLbsLocInfoConversionPluginBase::ValidateClient
//------------------------------------------------------------
//  
void CExampleLocationConverter::CancelValidateClient()
    {

    }

//------------------------------------------------------------
// CExampleLocationConverter::ConversionPluginPreferences
//------------------------------------------------------------
// 
void CExampleLocationConverter::ConversionPluginPreferences(
                 TLbsConversionPluginInfo& aPluginInfo )
	{
	aPluginInfo.SetIsPluginPositionToBeCached(iRetainLocalityInCache);
	
	//Note: The capability mask is currently not used anywhere in the location
	//monitor. 
	TLbsConversionPluginCapabilityMask capabilityMask;
	capabilityMask = aPluginInfo.PluginCapabilitiesMask();
	capabilityMask = ( ELbsPluginCapabilityGsmInfo|ELbsPluginCapabilityPosInfo|ELbsPluginCapabilityWlanInfo);
	aPluginInfo.SetPluginCapabilitiesMask(capabilityMask);
	}
