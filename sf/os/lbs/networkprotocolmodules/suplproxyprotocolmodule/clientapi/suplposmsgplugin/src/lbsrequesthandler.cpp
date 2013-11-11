// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Class implementation of CSuplPosRequestHandler
// 
//


// INCLUDE FILES
#include "lbsrequesthandler.h"
#include "netpmutils.h"
#include "lbsdevloggermacros.h"

#include <lbs/epos_comasuplvelocity.h>
#include <lbs/epos_comasuplreqasstdata.h>
#include <lbs/epos_comasuplsetcapabilities.h>
#include <lbs/epos_comasuplpospayload.h>
#include <lbs/epos_eomasuplposerrors.h>
#include <lbs/lbsnetcommon.h>

_LIT(KPosPanicCategory, "SuplPosRequestHandler");

//-----------------------------------------------------------------------------
// CSuplPosRequestHandler
//-----------------------------------------------------------------------------

// Default constructor 
 CSuplPosRequestHandler::CSuplPosRequestHandler(MRequestHandlerObserver& aObserver, RSuplProxyPosSubSession& aSubSession) 
    : CActive(EPriorityStandard), iSubSession(aSubSession), iObserver(aObserver)
    {
	LBSLOG(ELogP1, "CSuplPosRequestHandler::CSuplPosRequestHandler() Begin\n");
    CActiveScheduler::Add(this);
	LBSLOG(ELogP1, "CSuplPosRequestHandler::CSuplPosRequestHandler() End\n");
    }

  // Static constructor
 CSuplPosRequestHandler* CSuplPosRequestHandler::NewL(MRequestHandlerObserver& aObserver, RSuplProxyPosSubSession& aSubSession)
    {
	LBSLOG(ELogP1, "CSuplPosRequestHandler::NewL() Begin\n");
    CSuplPosRequestHandler* self = new (ELeave) CSuplPosRequestHandler(aObserver, aSubSession);
	LBSLOG(ELogP1, "CSuplPosRequestHandler::NewL() End\n");
    return self;
    }

 // Destructor
 CSuplPosRequestHandler::~CSuplPosRequestHandler()
    {
	LBSLOG(ELogP1, "CSuplPosRequestHandler::~CSuplPosRequestHandler() Begin\n");

    delete iPosPayload;
    iPosPayload = NULL;
    
    Cancel();

	LBSLOG(ELogP1, "CSuplPosRequestHandler::~CSuplPosRequestHandler() End\n");
    }

/**
 * Start the position request. 
 * @param aPosition 	holder for the position.
 * @param aStatus		client request status
 */
 void CSuplPosRequestHandler::GetPositionL(COMASuplPosition* aPosition)
    { 
	LBSLOG(ELogP1, "CSuplPosRequestHandler::GetPositionL() Begin\n");
    __ASSERT_ALWAYS(!IsActive(), User::Leave(KErrInUse));
    __ASSERT_ALWAYS(aPosition, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Leave(KErrBadHandle));    
    // store the position holder to fill in on completion:
    iOmaPosition = aPosition;
    // Make request to subsession
    iStatus = KRequestPending;
    iSubSession.GetPositionL(iStatus, iPosition);	
    iRequestOutstanding = EGetPosition;
    SetActive();
	LBSLOG(ELogP1, "CSuplPosRequestHandler::GetPositionL() End\n");
    }

 
 /**
  * Start the suplinfo request. 
  * @param aPosition 	holder for the position.
  * @param aStatus		client request status
  */
void CSuplPosRequestHandler::GetSuplInfoL(COMASuplInfoRequestList* aSuplInfoRequestList)
	{ 
	LBSLOG(ELogP1, "CSuplPosRequestHandler::GetSuplInfoL() Begin\n");
	__ASSERT_ALWAYS(!IsActive(), User::Leave(KErrInUse));
	__ASSERT_ALWAYS(aSuplInfoRequestList, User::Leave(KErrArgument));
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Leave(KErrBadHandle));  
	// store the position holder to fill in on completion:
	iSuplInfoReqList = aSuplInfoRequestList;
	iRequestOutstanding = EGetSuplInfo;
	__ASSERT_DEBUG(iPosPayload == NULL, User::Leave(KErrGeneral)); 
	SetRequestMaskFromSuplInfoListL();
	if(iSuplInfoRequestData.RequestMask() != 0)
		{
		// Make request to subsession 
		if(iSuplInfoRequestData.RequestMask() & TSuplInfoRequestData::ERequestItemPosPayload)	// contains pospayload request
			{
			// allocate buffer to hold encoded payload
			iPosPayload = HBufC8::NewL(KMaxOutgoingPosPayloadLength);
			}
		iStatus = KRequestPending;
		iSubSession.GetSuplInfoL(iStatus, iSuplInfoRequestData, iPosPayload);
		SetActive();
		}
	else	// we aren't going to ask the SPPM for any data, just self-complete
		{
		iStatus = KRequestPending;
		TRequestStatus* pStat = &iStatus;
		User::RequestComplete(pStat, KErrNone);
		SetActive();		
		}
	LBSLOG(ELogP1, "CSuplPosRequestHandler::GetSuplInfoL() End\n");
	}

 /**
  * From CActive. Called when request is complete
  */
 void CSuplPosRequestHandler::RunL()
    {    
	LBSLOG3(ELogP1, "CSuplPosRequestHandler::RunL(request=%d result=%d) Begin\n", iRequestOutstanding, iStatus.Int());
    TInt result = iStatus.Int();
    
    switch(iRequestOutstanding)		// there can only be a single async request outstanding
    	{
    	case EGetPosition:
    		{   		
    	    // Notify observer that request completed:
    	    if(result == KErrNone)
    	    	{
    	    	FillOMAPositionL(iPosition, iOmaPosition);
    	    	}

    	    // tell the plugin
    	    iObserver.OnGetPositionComplete(result);  
    	    
    	    iOmaPosition = NULL;
    	    break;
    		}
    	case EGetSuplInfo:
    		{
    	    // Notify observer that request completed:
    	    if(result == KErrNone)
    	    	{
    	    	result = FillSuplInfoListL();
    	    	}
    	    else
    	    	{
    	    	delete iPosPayload;
    	    	iPosPayload = NULL;
    	    	}
    	    // tell the plugin
    	    iObserver.OnGetSuplInfoComplete(result);
    	    iSuplInfoReqList = NULL;    		
    		break;
    		}
    	case ENone:	
    	default:
    		{
    		__ASSERT_ALWAYS(EFalse, User::Panic(KPosPanicCategory, KErrUnknown));
    		break;
    		}
    	}
    // no request outstanding
    iRequestOutstanding = ENone;
	LBSLOG(ELogP1, "CSuplPosRequestHandler::RunL() End\n");
    }

/**
* From CActive. Cancels the outstanding request.
*/
 void CSuplPosRequestHandler::DoCancel()
    {    
	LBSLOG(ELogP1, "CSuplPosRequestHandler::DoCancel() Begin\n");
    
    // Cancel this request in the server and complete the
    // outstanding observer request immediately.
    
    switch(iRequestOutstanding)     // there can only be a single async request outstanding
        {
        case EGetPosition:
            {
                iSubSession.CancelGetPosition();
                iObserver.OnGetPositionComplete(KErrCancel);
                break;
            }
        case EGetSuplInfo:
            {
                iSubSession.CancelGetSuplInfo();
                iObserver.OnGetSuplInfoComplete(KErrCancel);
                break;
            }
        default:
            {
            //Behaviour expected by SUPL FW : If there is no outstanding request when a 
            //cancel request is received, no action is taken.
            }
        }
    // no request outstanding
    iRequestOutstanding = ENone;
	LBSLOG(ELogP1, "CSuplPosRequestHandler::DoCancel() End\n");
    }

/**
* From CActive. Calls this function if RunL() function leaves
*/
 TInt CSuplPosRequestHandler::RunError(TInt aError)
    {
	LBSLOG(ELogP1, "CSuplPosRequestHandler::RunError() Begin and End\n");
    return aError;
    }
 
/*
 * Fills in the stored OMA position structure from the lbs position structure received
 */
 void CSuplPosRequestHandler::FillOMAPositionL(const TPositionInfoBase& aLbsPosition, COMASuplPosition* aSuplPosition)
	{
	LBSLOG(ELogP1, "CSuplPosRequestHandler::FillOMAPositionL() Begin\n");
	__ASSERT_ALWAYS(aSuplPosition, User::Panic(KPosPanicCategory, KErrArgument));
	
	if (aLbsPosition.PositionClassType() & EPositionInfoClass)
		{
		TPosition pos;		
		const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(aLbsPosition);
		posInfo.GetPosition(pos);
	
		// TIMESTAMP (mandatory)
		TTime time = pos.Time();
		TOMASuplUtcTime utcTime;
		TInt zoneCode = 0;	// 0 means COORDINATED_UNIVERSAL_TIME 	1 means LOCAL_TIME_WITH_ZONE 
		TInt zone = 0;	
	
		utcTime.SetUtcTime( time.DateTime(), zoneCode, zone);	
		TOMASuplPositionEstimate posEstimate;
		
		// POSITION ESTIMATE (mandatory)
		// -- latitude/longitude (mandatory)	
		TReal64 latitude  = pos.Latitude();
		TReal64 longitude = pos.Longitude();
		TOMASuplPositionEstimate::TOMASuplLatitudeSign sign = TOMASuplPositionEstimate::ENorth;
		if (latitude < 0)
			{
			sign = TOMASuplPositionEstimate::ESouth;
			latitude *= -1;
			}
		latitude  = latitude  * KLbsLatitudeConst; 
		longitude = longitude * KLbsLongitudeConst;	
		posEstimate.SetPositionEstimate( sign, latitude, longitude);
	
		// -- uncertainty (optional)
		if (pos.HorizontalAccuracy() != 0)
			{
			TOMASuplUncertainty uncertainty;
			TInt uncert = NetPmUtils::Uncertainty(pos.HorizontalAccuracy());
			uncertainty.SetUncertainty(uncert, uncert, 0);
			posEstimate.SetUncertainty(uncertainty);
			}
		
		// -- altitude information (optional)
		if (pos.Altitude() != 0)
			{
			TOMASuplAltitudeInfo::TOMASuplAltitudeDirection direction = TOMASuplAltitudeInfo::EHeight;
			TOMASuplAltitudeInfo altInfo;
			TReal32 altitude = pos.Altitude();
			TInt uncert = NetPmUtils::UncertaintyAltitude(pos.VerticalAccuracy());;
			if (altitude < 0)
				{
				direction = TOMASuplAltitudeInfo::EDepth;
				altitude *= -1;
				}
			
			altitude = NetPmUtils::EncodeAltitude(altitude);
			altInfo.SetAltitudeInfo(direction, altitude, uncert);
			posEstimate.SetAltitudeInfo(altInfo);
			}
		
		// -- confidence (optional)
		// this information is not available, omitted.
		/*
		posEstimate.SetConfidence(TInt);
		*/	
	
		aSuplPosition->SetPosition( utcTime, posEstimate );
		
		// -- velocity
		if ((posInfo.PositionClassType() & EPositionCourseInfoClass) != 0)
			{
			COMASuplVelocity* velocityreq = COMASuplVelocity::NewL();
			COMASuplHorizAndVertVelocity* horizverVel = COMASuplHorizAndVertVelocity::NewL();
	
			TCourse course;
			const TPositionCourseInfo& courseInfo = static_cast<const TPositionCourseInfo&>(posInfo);			
			courseInfo.GetCourse(course);
			
			// -- bearing
			TUint16 bearing = (TUint)course.Heading();
			
			// -- horizontal speed. Convert meters per second -> kilometres per hour
			TReal32 horSpeed = course.Speed() * KLbsMpsKmphConstant;
			// adjust for GAD encoding and lose decimal precision
			horSpeed += 0.5;
			TUint horSpeedInt = (TUint)horSpeed;
			//      limit to 2^16-1
			if (horSpeedInt > 65535)
				{
				horSpeedInt = 65535;
				}
			TUint16 speed = horSpeedInt;
			
			// -- vertical speed. Convert meters per second -> kilometres per hour
			TReal32 lbsVertSpeed = 0;
			
			lbsVertSpeed = course.VerticalSpeed() * KLbsMpsKmphConstant;			
			// adjust for GAD encoding and lose decimal precision
			TInt8 verDirect = (lbsVertSpeed > 0 ? 0 : 1);	// zero value indicates upward speed and 1 indicates downward speed
			lbsVertSpeed += 0.5;
			TUint lbsVertSpeedInt = (TUint)lbsVertSpeed;
			//      limit to 2^8-1
			if (lbsVertSpeedInt > 255)
				{
				lbsVertSpeedInt = 255;
				}
			TInt8 verSpeed = lbsVertSpeedInt;
			
			horizverVel->SetHorizAndVertVel(bearing, speed, verDirect, verSpeed);
			
			velocityreq->SetType(COMASuplInfoRequest::EOMASuplVelocity);
			velocityreq->SetVelocity(horizverVel);
	
			aSuplPosition->SetSuplVelocity(velocityreq);	// ownership of velocity is taken here
	
			}
		 
		aSuplPosition->SetType(COMASuplInfoRequest::EOMASuplPosition);	// necessary or done in c'tor?
		aSuplPosition->SetStatus(KErrNone);
		}
	else
		{
		aSuplPosition->SetStatus(KErrOMASuplPosInfo);			// indicates that requested info not filled in
		}
	 	 
	LBSLOG(ELogP1, "CSuplPosRequestHandler::FillOMAPositionL() End\n");
	}


/**
 * Sets the request mask inside the structure we pass to SPPM 
 */
void CSuplPosRequestHandler::SetRequestMaskFromSuplInfoListL()
 	{
	LBSLOG(ELogP1, "CSuplPosRequestHandler::SetRequestMaskFromSuplInfoListL() Begin\n");
 	iSuplInfoRequestData.InitializeRequestMask();
 	// iterate list and set each requested type in the structure we pass to SPPM 
    TInt count = iSuplInfoReqList->GetCount();
    
    LBSLOG2(ELogP1, "Requested Info Type Count = %d \n", count);

    for ( TInt index = 0; index < count; index ++ )
        {
        // Determine the type of the element
        COMASuplInfoRequest *reqElement = iSuplInfoReqList->GetElement( index );
        __ASSERT_ALWAYS(reqElement,User::Leave(KErrArgument));
       	COMASuplInfoRequest::TOMASuplInfoType infoReqType = reqElement->Type();
 
        // Fill up the element with appropriate values
         switch( infoReqType )
             {
             case COMASuplInfoRequest::EOMASuplReqAsstData:
            	 {
                 LBSLOG(ELogP1, "Requested Info: EOMASuplReqAsstData\n");
            	 iSuplInfoRequestData.AddRequestItemToMask(TSuplInfoRequestData::ERequestItemAssistance);
                 break;
            	 }
             case COMASuplInfoRequest::EOMASuplSETCapabilities:
            	 {
                 LBSLOG(ELogP1, "Requested Info: EOMASuplSETCapabilities\n");
            	 iSuplInfoRequestData.AddRequestItemToMask(TSuplInfoRequestData::ERequestItemCapabilities);
                 break;
            	 }
                 
             case COMASuplInfoRequest::EOMASuplPosPayload:
            	 {
                 LBSLOG(ELogP1, "Requested Info: EOMASuplPosPayload\n");
                 //Uncomment the following after confirming if the SUPL FW is expected to set the payload type
                 //HBufC8* tempPtr;
                 //COMASuplPosPayload* posPayload = static_cast<COMASuplPosPayload*>(reqElement);
                 //COMASuplPosPayload::TOMASuplPosPayloadType type;
                 //posPayload->GetPosPayload(tempPtr, type);
                 //__ASSERT_ALWAYS(type == COMASuplPosPayload::ERRLP, User::Panic(KPosPanicCategory, KErrArgument));
            	 iSuplInfoRequestData.AddRequestItemToMask(TSuplInfoRequestData::ERequestItemPosPayload);
                 break;
            	 }
             case COMASuplInfoRequest::EOMASuplPosition:	
            	 {
                 LBSLOG(ELogP1, "Requested Info: EOMASuplPosition\n");
            	 iSuplInfoRequestData.AddRequestItemToMask(TSuplInfoRequestData::ERequestItemPosition);
                 break;
            	 }
                 
             case COMASuplInfoRequest::EOMASuplVelocity:
            	 {
                 LBSLOG(ELogP1, "Requested Info: EOMASuplVelocity\n");            	 
            	 // do nothing, we will never pass this
            	 break;
            	 }
             default:
                 LBSLOG(ELogP1, "Requested Info: Unknown\n");

            	 __ASSERT_DEBUG(EFalse, User::Leave(KErrArgument));
            	 break;
             }
        }

	// SUPL FW sends 2 requests for pospayload that we don't need to respond to. 
	// The first is when creating a POS_INIT and is requested along with all the other types of info (assistance mask, position, caps, velocity)
	// The second is after the SUPL FW sends the POS_INIT (but before it receives a SUPL_POS) and that is requested (as all subsequent pospayload requests) along with velocity only.
	// In those cases we don't bother to send a buffer to the SPPM, but return the error
    if(iSuplInfoRequestData.RequestMask() & TSuplInfoRequestData::ERequestItemPosPayload)
    	{
		 if((iSuplInfoRequestData.RequestMask() & TSuplInfoRequestData::ERequestItemAssistance))
			 {
			 iSuplInfoRequestData.RemoveRequestItemFromMask(TSuplInfoRequestData::ERequestItemPosPayload);
			 }
		 else if(!iFirstSUPLPOSPosPayloadProcessed)
			 {
			 iSuplInfoRequestData.RemoveRequestItemFromMask(TSuplInfoRequestData::ERequestItemPosPayload);			
			 iFirstSUPLPOSPosPayloadProcessed = ETrue;
			 }  
    	}
	LBSLOG(ELogP1, "CSuplPosRequestHandler::SetRequestMaskFromSuplInfoListL() End\n");
    }

/**
 * Fills in the stored OMA suplinforequest list structure from the lbs structures received
 */
TInt CSuplPosRequestHandler::FillSuplInfoListL()
	{
	LBSLOG2(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() Begin with count=%d\n", iSuplInfoReqList->GetCount());

	TInt result = KErrOMASuplPosInfo;	// no data being returned
	// fill in the iSuplInfoReqList from iSuplInfoRequestData and iPosPayload
	// set errors as approriate for each request 
	
	// iterate list and set each requested type in the structure we pass to SPPM 
    TInt count = iSuplInfoReqList->GetCount();
    for ( TInt index = 0; index < count; index ++ )
        {
        // Determine the type of the element
        COMASuplInfoRequest *reqElement = iSuplInfoReqList->GetElement( index );
       	COMASuplInfoRequest::TOMASuplInfoType infoReqType = reqElement->Type();
       	reqElement->SetStatus(KErrNone);
       	
        // Fill up the element with appropriate values
         switch(infoReqType)
			{
			case COMASuplInfoRequest::EOMASuplReqAsstData:
				{
				LBSLOG2(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() EOMASuplReqAsstData, err=%d\n", iSuplInfoRequestData.AssistanceMaskError());

				if(iSuplInfoRequestData.AssistanceMaskError() == KErrNone)
					{
					COMASuplReqAsstData* asstData = static_cast<COMASuplReqAsstData*>(reqElement);
					TLbsAssistanceDataGroup asstMask = iSuplInfoRequestData.AssistanceDataMask();
					asstData->SetReqAsstData(((asstMask & EAssistanceDataAlmanac)>0),
											((asstMask & EAssistanceDataPositioningGpsUtcModel)>0),
											((asstMask & EAssistanceDataIonosphericModel)>0),
											((asstMask & EAssistanceDataDgpsCorrections)>0),
											((asstMask & EAssistanceDataReferenceLocation)>0),
											((asstMask & EAssistanceDataReferenceTime)>0),
											((asstMask & EAssistanceDataAquisitionAssistance)>0),
											((asstMask & EAssistanceDataBadSatList)>0) // 'real-time integrity' == 'bad sat list'
											);	
					//Set the Navigation Model parameters if it has been requested.
					if ((asstMask & EAssistanceDataNavigationModel)>0)
					    {
					    // Note that setting nSAT (number of satellites for which the SET has
					    // ephemeris data available) to zero means that the SLP should ignore
					    // values for gpsWeek and gpsToe. The TOE-Limit is set to 2 [range 0-10]. 
					    // Also means there is no need to include any additional satellite 
					    // information. See OMA-TS-V1_0-20070615A p22-23 for further detail.
					    TOMASuplNavigationModel naviModel;
					    naviModel.SetNavigationModel(0, 0, 0, 2);
					    asstData->SetNavigationData(naviModel);	// this sets 'navigationmodel requested' flag to true
					    }
                    result = KErrNone;
					}
				else
					{
					reqElement->SetStatus(KErrOMASuplPosInfo);
					}
				break;
				}
			case COMASuplInfoRequest::EOMASuplSETCapabilities:
				{
				LBSLOG2(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() EOMASuplSETCapabilities, err=%d\n", iSuplInfoRequestData.CapabilitiesError());

				if(iSuplInfoRequestData.CapabilitiesError() == KErrNone)
					{
					COMASuplSETCapabilities* setCaps = static_cast<COMASuplSETCapabilities*>(reqElement);
					TLbsNetPosCapabilities posCaps = iSuplInfoRequestData.Capabilities();
					
					// get the lbs preferred method
					TLbsNetPosMethod prefMethod;
					posCaps.GetPosMethod(0, prefMethod);
					COMASuplSETCapabilities::TOMASuplPrefMethod omaPrefMethod = COMASuplSETCapabilities::EOMANoPreferred;
					if(prefMethod.PosMode() & TPositionModuleInfo::ETechnologyTerminal)
						{
						omaPrefMethod = COMASuplSETCapabilities::EOMAAGpsSETBasedPreferred;
						}
					else
						{
						omaPrefMethod = COMASuplSETCapabilities::EOMAAGpsSETAssitedPreferred;
						}
					
					// which methods does lbs support?
					TBool setAssisted = EFalse, setBased = EFalse, autonomous = EFalse;
					TLbsNetPosMethod posMethod;
					for (TInt index = 0; index < posCaps.NumPosMethods(); index++)
						{				
						posCaps.GetPosMethod(index, posMethod);
						if(posMethod.PosMode() == TPositionModuleInfo::ETechnologyTerminal)
							{
							autonomous = ETrue;
							}
						else if(posMethod.PosMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted))
							{
							setBased = ETrue;
							}
						else if(posMethod.PosMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
							{
							setAssisted = ETrue;
							}
						}
					
					TUid posProtocol;
					TBool aFLT = EFalse, eOTD = EFalse, oTDOA = EFalse;
					for (TInt index = 0; index < posCaps.NumPosProtocols(); index++)
						{
						posCaps.GetPosProtocol(index, posProtocol);
						if(posProtocol == KLbsPositioningMeansAflt)
							{
							aFLT = ETrue;
							}
						else if(posProtocol == KLbsPositioningMeansEotd)
							{
							eOTD = ETrue;
							}
						else if(posProtocol == KLbsPositioningMeansOtdoa)
							{
							oTDOA = ETrue;
							}
						}
					TOMASuplPosTechnology posTech;
					posTech.SetPosTechnology(setAssisted, setBased, autonomous, aFLT, eOTD, oTDOA);
					TOMASuplPosProtocol suplProtocol;
					suplProtocol.SetPosProtocol(EFalse, ETrue, EFalse ); 	// RRLP protocol 
					
					TOMASuplPosProtocolVersion protocolVersion;
					TUint8 RrlpMajorVersion = 5, RrlpTechVersion = 12, RrlpminorVersion = 0;
					
					protocolVersion.SetPosProtocolVersion(RrlpMajorVersion, RrlpTechVersion, RrlpminorVersion);
					
					setCaps->SetSETCapabilities(posTech, omaPrefMethod, suplProtocol, protocolVersion);
					
					result = KErrNone;
					}
				else	// something went wrong when trying to retrieve pos capabilities
					{
					reqElement->SetStatus(KErrOMASuplPosInfo);
					}
				break;
				}
			case COMASuplInfoRequest::EOMASuplPosPayload:
				{
				LBSLOG2(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() EOMASuplPosPayload, err=%d\n", iSuplInfoRequestData.PosPayloadError());

				TInt err = iSuplInfoRequestData.PosPayloadError();
				TBool posPayloadReqSentToSPPM = (iSuplInfoRequestData.RequestMask() & TSuplInfoRequestData::ERequestItemPosPayload);
				if(posPayloadReqSentToSPPM && (err == KErrNone))
					{
					COMASuplPosPayload* posPayload = static_cast<COMASuplPosPayload*>(reqElement);
					// set the pospayload with data provided by SPPM (posPayload takes ownership)
					posPayload->SetPosPayload(iPosPayload, COMASuplPosPayload::ERRLP);
					iPosPayload = NULL;
					if (iSuplInfoRequestData.IsLastPosMsg())
					    {
					    //inform the SUPL FW that this will be the last pospayload for this rrlp session
					    reqElement->SetStatus(KErrOMASuplLastPosMsg);
					    }
					result = KErrNone;
					}
				else	// request for pospayload not sent to SPPM, or error returned from SPPM for pospayload
					{
					reqElement->SetStatus(KErrOMASuplPosInfo);
					// iPosPayload has not been passed to the SUPL FW.
					delete iPosPayload;
					iPosPayload = NULL;
					}
				break;
				}
			case COMASuplInfoRequest::EOMASuplPosition:	
				{
				LBSLOG2(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() EOMASuplPosition, err=%d\n", iSuplInfoRequestData.PositionError());

				if(iSuplInfoRequestData.PositionError() == KErrNone)
					{
					COMASuplPosition* position = static_cast<COMASuplPosition*>(reqElement);
					FillOMAPositionL(iSuplInfoRequestData.Position(), position);	
					result = KErrNone;
					}
				else
					{
					reqElement->SetStatus(KErrOMASuplPosInfo);	
					}
				break;
				}
			case COMASuplInfoRequest::EOMASuplVelocity:
				{
				LBSLOG(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() EOMASuplVelocity, err=KErrOMASuplPosInfo\n");
				// we will never pass this, just fill in error code
				reqElement->SetStatus(KErrOMASuplPosInfo);	
				break;
				}
			default:
				__ASSERT_DEBUG(EFalse, User::Leave(KErrArgument));
				break;
			}
        }

	LBSLOG(ELogP1, "CSuplPosRequestHandler::FillSuplInfoListL() End\n");	
	return result;
	}

/* Called when SUPL FW reinitializes a POS session object inorder to reuse it with a
 * different SUPL session.
 */
void CSuplPosRequestHandler::ResetState()
    {
    iFirstSUPLPOSPosPayloadProcessed = EFalse;
    iRequestOutstanding = ENone;
    }
