/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file argutils.h
*/

#ifndef __ARG_UTILS_H__
#define __ARG_UTILS_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsnetcommon.h>
//-------------------------------------------------------------------------------------------------

/**
Static utility functions to populate and instantiate arguments for function calls

@internalComponent
@test
*/

const TInt KAlpha2Timeout = 12*1000*1000;
const TInt KTTimeout = 8*1000*1000;
const TInt KMOLRFixTime = 240*1000*1000;
const TInt KMinHorizontalAcc = 50.0;
const TInt KMinVerticalAcc = 1000.0;
const TInt KDefaultMaxFixTime = 130*1000*1000;


class ArgUtils
	{
public:
	inline static void PopulateLC(RLbsAssistanceDataBuilderSet& aData)
		{
		aData.OpenL();
		CleanupClosePushL(aData);
		
		// Access the Gps Reference Time which is the only data we support in this simple provider.
		RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;

		aData.GetDataBuilder(refTimeBuilder);
		if (refTimeBuilder)
			{
			const TUint KAssistanceData_RefTime_Week = 12;
			const TUint KAssistanceData_RefTime_Tow1MSec = 567;
			// Fill out the reference time items we support - note the builder set is already open.
			refTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, KAssistanceData_RefTime_Week);
			refTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, KAssistanceData_RefTime_Tow1MSec);

			// Time stamp the data.
			TTime timeStamp;
			timeStamp.UniversalTime();

			refTimeBuilder->SetTimeStamp(timeStamp);
			}
		}

	inline static TLbsNetPosRequestPrivacy Privacy()
		{
		TLbsNetPosRequestPrivacy privacy;
		privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
		privacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
		return privacy;
		}

	inline static TLbsExternalRequestInfo RequestInfo()
		{
		TLbsExternalRequestInfo requestInfo;
		requestInfo.SetRequesterId(_L8("+441234567891"));
		requestInfo.SetClientName(_L8("THybridNWClient"));
		requestInfo.SetClientExternalId(_L8("+441234567891"));	
		return requestInfo;
		}

	inline static TPositionInfo PositionInfo()
		{
		TCoordinate coordinate(3.14159265359, 2.71828183);
		TLocality locality(coordinate, 99.99, 99.99);
		TTime time(101010101);
		TPosition position(locality, time);
		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		return positionInfo;
		}

	inline static TPositionInfo ReferencePositionInfo()
		{
		// General case: This will be less accurate than the GPS Position and the Hybrid Position.
		TCoordinate coordinate(51.5,-0.096, 2.0);
		TLocality locality(coordinate, 1000.0, 1000.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}


	inline static TPositionInfo MolrReferencePositionInfo()
		{
		// General case: This will be less accurate than the Reference Position and the Hybrid Position.
		TCoordinate coordinate(3.14159265359, 2.71828183, 10.0);
		TLocality locality(coordinate, 1000.0, 1000.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}


	inline static TPositionInfo HybridPositionInfo()
		{
		// General case: This will be more accurate than ReferencePosition
		// but less accurate than the GPS position
		TCoordinate coordinate(51.502,-0.1055, 1.0);
		TLocality locality(coordinate, 200.0, 200.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}


	inline static TPositionInfo MolrHybridPositionInfo()
		{
		// General case: This will be more accurate than ReferencePosition
		// but less accurate than the GPS position
		TCoordinate coordinate(3.14159265359, 2.71828183);
		TLocality locality(coordinate, 200.0, 200.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}


	inline static TPositionInfo NetworkPositionInfo()
		{
		// General case: This will be more accurate than ReferencePosition
		// but less accurate than the GPS position
		TCoordinate coordinate(51.502,-0.1055, 1.0);
		TLocality locality(coordinate, 100.0, 100.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}
		
	inline static TPositionInfo AccurateNetworkPositionInfo()
		{
		// General case: This will be more accurate than ReferencePosition
		// but less accurate than the GPS position
		TCoordinate coordinate(51.502,-0.1055, 1.0);
		TLocality locality(coordinate, 10.0, 10.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}
		
	inline static TPositionInfo MolrNetworkPositionInfo()
		{
		// General case: This will be more accurate than ReferencePosition
		// but less accurate than the GPS position
		TCoordinate coordinate(3.14159265359, 2.71828183);
		TLocality locality(coordinate, 100.0, 100.0);

		// Setting time to current time
		TTime time;
		time.UniversalTime();
		TPosition position(locality, time);

		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);
		positionInfo.SetUpdateType(EPositionUpdateGeneral);
		positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		positionInfo.SetPositionModeReason(EPositionModeReasonNone);
		return positionInfo;
		}


	inline static TLbsNetPosRequestQuality Quality()
		{
		TLbsNetPosRequestQuality quality;
		quality.SetMaxFixTime(KTTimeout);
		quality.SetMaxFixAge(0);
		TRealX nan;
		nan.SetNaN();
		quality.SetMinHorizontalAccuracy(nan);
		quality.SetMinVerticalAccuracy(nan);
		return quality;
		}

	inline static TLbsNetPosRequestQuality QualityAlpha2()
		{
		TLbsNetPosRequestQuality quality;
		quality.SetMaxFixTime(KAlpha2Timeout); 
		quality.SetMaxFixAge(0);
		TRealX nan;
		nan.SetNaN();
		quality.SetMinHorizontalAccuracy(nan);
		quality.SetMinVerticalAccuracy(nan);
		return quality;
		}

	inline static TLbsNetPosRequestMethod RequestMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		TLbsNetPosMethod method2;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted); 
		const TLbsNetPosMethod posMethods [] =
			{
			method1,method2
			};
		method.SetPosMethods(posMethods, 2);
		return method;
		}

	inline static TLbsNetPosRequestMethod RequestTerminalBasedMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1
			};
		method.SetPosMethods(posMethods, 1);
		return method;
		}
	
	inline static TLbsNetPosRequestMethod RequestAutonomousMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1
			};
		method.SetPosMethods(posMethods, 1);
		return method;
		}
			
	inline static TLbsNetPosRequestMethod RequestHybridMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		TLbsNetPosMethod method2;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
		method2.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1,method2
			};
		method.SetPosMethods(posMethods, 2);
		return method;
		}
	
			
	inline static TLbsNetPosRequestMethod RequestTAPMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1
			};
		method.SetPosMethods(posMethods, 1);
		return method;
		}

	inline static TLbsNetPosRequestMethod RequestTerminalAssistedAndTerminalBasedMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		TLbsNetPosMethod method2;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
		method2.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1,method2
			};
		method.SetPosMethods(posMethods, 2);
		return method;
		}
	
	inline static TLbsNetPosRequestMethod RequestUnspecifiedMethod()
		{
		TLbsNetPosRequestMethod method;
		TLbsNetPosMethod method1;
		method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyUnknown);

 		const TLbsNetPosMethod posMethods [] =
			{
			method1
			};
		method.SetPosMethods(posMethods, 1);
		return method;
		}	

	inline static TInt Alpha2()
		{
		return KAlpha2Timeout;
		}

	inline static TBool CompareQuality(TLbsNetPosRequestOptionsAssistance* aOpts, 
										TBool aIsclientConnected, 
										TInt aHorizontalAccuracy, 
										TInt aVerticalAccuracy, 
										TInt aMaxFixTime, 
										TInt aMaxFixAge, 
										TLbsAssistanceDataItem aDataMask, 
										TPositionModuleInfo::TTechnologyType aTechnologyType
										)
		{
		TLbsNetPosRequestQuality quality;
		aOpts->GetRequestQuality(quality);
		
		if(aIsclientConnected != aOpts->NewClientConnected())
			{
			return EFalse;
			}
		
		if(quality.MinHorizontalAccuracy() != aHorizontalAccuracy)	 
			{
			return EFalse;
			}
			
		if(quality.MinVerticalAccuracy()   != aVerticalAccuracy)
			{
			return EFalse;
			}
			
		if(quality.MaxFixTime() != aMaxFixTime)
			{
			return EFalse;
			}
			
		if(quality.MaxFixAge()  != aMaxFixAge)		
			{
			return EFalse;
			}
			
		if(aOpts->DataRequestMask()  != aDataMask) 
			{
			return EFalse;
			}
		
		if(aOpts->PosMode() != aTechnologyType)
			{
			return EFalse;
			}
			
		return ETrue;
		}
		
		
	inline static TBool ComparePositionInfoToReference(const TPositionInfo aPosInfoA)
		{
		TPositionInfo refPosInfo = ReferencePositionInfo();
	
		TPosition posA;
		TPosition posR;
		aPosInfoA.GetPosition(posA);
		refPosInfo.GetPosition(posR);
		
		if(posA.HorizontalAccuracy() != posR.HorizontalAccuracy())
			{
			return EFalse;	
			}
		
		if(posA.VerticalAccuracy() != posR.VerticalAccuracy())
			{
			return EFalse;	
			}

		if(posA.Latitude() != posR.Latitude())
			{
			return EFalse;	
			}

		if(posA.Longitude() != posR.Longitude())
			{
			return EFalse;	
			}

		if(posA.Altitude() != posR.Altitude())
			{
			return EFalse;	
			}

		return ETrue;
		}


	
	inline static TBool ComparePositionInfoToMolrRefPos(const TPositionInfo aPosInfoA)
		{
		TPositionInfo refPosInfo = MolrReferencePositionInfo();
	
		TPosition posA;
		TPosition posR;
		aPosInfoA.GetPosition(posA);
		refPosInfo.GetPosition(posR);
		
		if(posA.HorizontalAccuracy() != posR.HorizontalAccuracy())
			{
			return EFalse;	
			}
		
		if(posA.VerticalAccuracy() != posR.VerticalAccuracy())
			{
			return EFalse;	
			}

		if(posA.Latitude() != posR.Latitude())
			{
			return EFalse;	
			}

		if(posA.Longitude() != posR.Longitude())
			{
			return EFalse;	
			}

		if(posA.Altitude() != posR.Altitude())
			{
			return EFalse;	
			}

		return ETrue;
		}
		
	inline static TBool ComparePositionInfoToNan(const TPositionInfo aPosInfoA)
		{
		TPosition pos;
		aPosInfoA.GetPosition(pos);
		
		if(!Math::IsNaN(pos.HorizontalAccuracy()))
			{
			return EFalse;
			}

		if(!Math::IsNaN(pos.VerticalAccuracy()))
			{
			return EFalse;
			}
			
		if(!Math::IsNaN(pos.Latitude()))
			{
			return EFalse;
			}
			
		if(!Math::IsNaN(pos.Longitude()))
			{
			return EFalse;
			}
			
		if(!Math::IsNaN(pos.Altitude()))	
			{
			return EFalse;
			}
			
		return ETrue;
			
		}
		
	};

//-------------------------------------------------------------------------------------------------

#endif //__ARG_UTILS_H__

