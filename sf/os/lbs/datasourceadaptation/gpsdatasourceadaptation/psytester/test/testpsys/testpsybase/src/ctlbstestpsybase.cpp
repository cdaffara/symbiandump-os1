// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include <f32file.h>
#include <lbs/epos_mpositionerstatus.h>
#include "ctlbstestpsybase.h"
#include "ctlbs_testpsybaseconstants.h"
	
// ---------------------------------------------------------
// CT_LbsTestPsyBase::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CT_LbsTestPsyBase* CT_LbsTestPsyBase::InstanceL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsyBase* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsyBase*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsyBase(aPositionerStatus);
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self
		Dll::SetTls(reinterpret_cast<TAny*>(self));
		}

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// CT_LbsTestPsyBase::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}
        
// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CT_LbsTestPsyBase::CT_LbsTestPsyBase(MPositionerStatus* aPositionerStatus) :
	CActive(EPriorityStandard),
	iMaxAge(0),
	iPositionerStatus(aPositionerStatus),
	iTimerDelay(100000),
	iSupportedClasses(
					EPositionInfoClass |
					EPositionGenericInfoClass |
					EPositionCourseInfoClass |
					EPositionSatelliteInfoClass	),
	iCapabilities(
		TPositionModuleInfo::ECapabilityHorizontal |
		TPositionModuleInfo::ECapabilityVertical |
		TPositionModuleInfo::ECapabilitySpeed |
		TPositionModuleInfo::ECapabilityDirection |
		TPositionModuleInfo::ECapabilitySatellite |
		TPositionModuleInfo::ECapabilityCompass |
		TPositionModuleInfo::ECapabilityNmea |
		TPositionModuleInfo::ECapabilityAddress |
		TPositionModuleInfo::ECapabilityBuilding |
		TPositionModuleInfo::ECapabilityMedia   
					),
    iUpdateType(EPositionUpdateGeneral),
	iRequestsHandling(EQueuing),
	iActiveRequestCompleted(EFalse)
    {
	
    CActiveScheduler::Add(this);
	}

// EPOC default constructor can leave.
EXPORT_C void CT_LbsTestPsyBase::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
    }

// Destructor
EXPORT_C CT_LbsTestPsyBase::~CT_LbsTestPsyBase()
    {
	Cancel();
    iRequestsPosInfo.Close();
	iRequestsStatus.Close();
    iTimer.Close();
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::GetPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::NotifyPositionUpdate(TPositionInfo* aPosInfo, TRequestStatus* aStatus)
    {
	*aStatus = KRequestPending;
	++iRequests;
    aPosInfo->SetModuleId(iModuleId);

    if (iRequestsStatus.Find(aStatus) != KErrNotFound)
        {
        User::RequestComplete(aStatus, KErrInUse);
        return;
        }
	
	if (iRequests != 1 && CheckMaxAge(aPosInfo))
		{
		User::RequestComplete(aStatus, KErrNone);
		return;
		}

    TInt err = iRequestsStatus.Append(aStatus);
    if (err != KErrNone)
        {
        User::RequestComplete(aStatus, err);
        return;
        }

    err = iRequestsPosInfo.Append(aPosInfo);
    if (err != KErrNone)
        {
        iRequestsStatus.Remove(iRequestsStatus.Count() - 1);
        User::RequestComplete(aStatus, err);
        return;
        }

	if (iRequestsStatus.Count() == 1)
		{
		StartRequest();
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::StartRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyBase::StartRequest()
	{
	iActiveRequestCompleted = EFalse;
	iTimer.After(iStatus,iTimerDelay.Int64());
	SetActive();
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::RunL()
	{
	SetPositionInfoL(iRequestsPosInfo[KActiveRequest]);
	
	if(iRequestsHandling == EBundling)
		{
		for(TInt i=1; i<iRequestsPosInfo.Count(); i++) 
			{
			SetPositionInfoL(iRequestsPosInfo[i]);
			TPosition pos;	
			iRequestsPosInfo[KActiveRequest]->GetPosition(pos);
			iRequestsPosInfo[i]->SetPosition(pos);	
			}	
		CompleteRequests(iStatus.Int());
		}
	else // Queuing
		{
		if(!iActiveRequestCompleted) 
			CompleteRequest(KActiveRequest, iStatus.Int());
		
		if(iRequestsPosInfo.Count() > 0)
			{
			for(TInt i=0; i<iRequestsPosInfo.Count(); i++)
				{		
				if(CheckMaxAge(iRequestsPosInfo[i]))
					{
					CompleteRequest(i, iStatus.Int());
					}
				else
					{
					StartRequest();		
					break;
					}
				}
			}
		}
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::DoCancel()
	{
	iTimer.Cancel();
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::RunError
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CT_LbsTestPsyBase::RunError(TInt aError)
    {
    CompleteRequests(aError);
    return KErrNone;
    }

// ---------------------------------------------------------
// CT_LbsTestPsyBase::CompleteRequests
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyBase::CompleteRequests(TInt aErrorCode)
    {
   	for (TInt i=0; i<iRequestsStatus.Count(); i++)
        {
        User::RequestComplete(iRequestsStatus[i], aErrorCode);
        }
	iRequestsStatus.Reset();   
    iRequestsPosInfo.Reset();
    }

// ---------------------------------------------------------
// CT_LbsTestPsyBase::CompleteRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::CompleteRequest(TInt aIndex, TInt aErrorCode)
    {		
		__ASSERT_DEBUG(iRequestsStatus.Count() > 0, User::Panic(KNullDesC, 0));

		User::RequestComplete(iRequestsStatus[aIndex], aErrorCode);
		iRequestsStatus.Remove(aIndex);	
        iRequestsPosInfo.Remove(aIndex);
		iRequestsStatus.Compress();
		iRequestsPosInfo.Compress();
		
		iActiveRequestCompleted = (aIndex == KActiveRequest);

	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::CancelGetPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::CancelNotifyPositionUpdate(TRequestStatus* aStatus)
    {
    TInt index = iRequestsStatus.Find(aStatus);
    if (index != KErrNotFound)
        {
		CompleteRequest(index, KErrCancel);
        }
    if (iRequestsStatus.Count() == 0)
        {
        Cancel();
        }
    }

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetRequestsHandling
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetRequestsHandling(const TRequestsHandling& aRequestsHandling)
	{
	iRequestsHandling=aRequestsHandling;
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetRequestsHandling
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetTimerDelay(const TTimeIntervalMicroSeconds& aTimerDelay)
	{
	iTimerDelay=aTimerDelay;
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetSupportedClasses
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetSupportedClasses(const TUint32& aSupportedClasses)
	{
	iSupportedClasses=aSupportedClasses;
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetCapabilities
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetCapabilities(const TUint32& aCapabilities)
	{
	iCapabilities=aCapabilities;
	}

//---------------------------------------------------------
// CT_LbsTestPsyBase::SetUpdateType
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetUpdateType(const TUint32& aUpdateType)
	{
	iUpdateType = aUpdateType;
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetMaxAge
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetMaxAge(const TTime& aMaxAge)
	{
	iMaxAge = aMaxAge; 
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetMaxAge
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetModuleId(const TUid& aModuleId)
	{
	iModuleId = aModuleId; 
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::RequestsStatusCount
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CT_LbsTestPsyBase::RequestsStatusCount() const
	{
	return iRequestsStatus.Count();
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::FindStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CT_LbsTestPsyBase::FindStatus(TRequestStatus* aStatus) const
	{
	return iRequestsStatus.Find(aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::CheckMaxAge
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CT_LbsTestPsyBase::CheckMaxAge(TPositionInfo* aPosInfo) 
	{
	TTime tmp(0);

	if (iMaxAge == tmp)
		{
		return EFalse;
		}

	if ( iPosTime > iMaxAge ) 
        {
		SetPositionInfoL(aPosInfo, ETrue);
		return ETrue;
		}
	
	return EFalse;		
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetPositionInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetPositionInfoL(TPositionInfo* aPosInfo, const TBool& aUseCache)
	{
	TUint32 classType = aPosInfo->PositionClassType();
    aPosInfo->SetUpdateType(EPositionUpdateGeneral);
        
	if (classType & EPositionGenericInfoClass)
		{
		HPositionGenericInfo& generic = static_cast <HPositionGenericInfo&> (*aPosInfo);
		generic.ClearPositionData();
        generic.SetModuleId(iModuleId);
		SetTPositionInfo(generic, aUseCache);
		if(iCapabilities & TPositionModuleInfo::ECapabilitySpeed)
			{
			SetSpeedGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityDirection)
			{
			SetDirectionGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilitySatellite)
			{
			SetSatelliteGenericInfoL(generic, aUseCache);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityCompass)
			{
			SetCompassGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityNmea)
			{
			SetNMEAGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityAddress)
			{
			SetAddressGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityBuilding)
			{
			SetBuildingGenericInfoL(generic);
			}
		if(iCapabilities & TPositionModuleInfo::ECapabilityMedia)
			{
			SetMediaGenericInfoL(generic);
			}
		}
	else
		{
        aPosInfo->SetModuleId(iModuleId);
		SetTPositionInfo(*aPosInfo, aUseCache);
		if(classType & EPositionCourseInfoClass & iSupportedClasses)
			{
			TPositionCourseInfo& courseInfo = static_cast <TPositionCourseInfo&> (*aPosInfo);
			SetTPositionCourseInfo(courseInfo);
			}
		if(classType & EPositionSatelliteInfoClass & iSupportedClasses)
			{
			TPositionSatelliteInfo& satelliteInfo = static_cast <TPositionSatelliteInfo&> (*aPosInfo);
			SetTPositionSatelliteInfo(satelliteInfo, aUseCache);
			}
		}
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//                          
EXPORT_C void CT_LbsTestPsyBase::SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache)
	{
	TPosition pos;
	
	pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);
	
    TTime aTime;
    if(aUseCache)
        {
        pos.SetTime(iPosTime);
        }
    else
        {
        aTime.UniversalTime();
        pos.SetTime(aTime);
        iPosTime = aTime;
        }
	
    pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
	aInfo.SetPosition(pos);	
	}


// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo)
	{
	//SetTPositionInfo(aCourseInfo);
	TCourse course;
	course.SetSpeed(KVerticalSpeed);
	course.SetHeading(KHeading);
	course.SetSpeedAccuracy(KVerticalSpeedError);
	course.SetHeadingAccuracy(KHeadingError);
	aCourseInfo.SetCourse(course);
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetTPositionSatelliteInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo, const TBool& aUseCache)
	{
    ClearTPositionSatelliteInfo(aSatelliteInfo);
	
    TTime aTime;
    if (aUseCache)
        {
        aTime = iSatTime;
        }
    else
        {
	    aTime.UniversalTime();
	    iSatTime = aTime;
        }

    aSatelliteInfo.SetSatelliteTime(aTime);
	aSatelliteInfo.SetHorizontalDoP(KSatelliteHorizontalDoP);
	aSatelliteInfo.SetVerticalDoP(KSatelliteVerticalDoP);
	aSatelliteInfo.SetTimeDoP(KSatelliteTimeDoP);

	for (TInt  i = 0; i < KSatelliteNumInView; i++)
		{
		TSatelliteData satellite;
		
		satellite.SetSatelliteId(KSatelliteId+i);
		satellite.SetAzimuth(KAzimuth);
		satellite.SetElevation(KElevation);
		satellite.SetIsUsed(KIsUsed);
		satellite.SetSignalStrength(KSignalStrength);
	
		aSatelliteInfo.AppendSatelliteData(satellite);
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::ClearTPositionSatelliteInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::ClearTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo)
    {
    TRealX nan;
    nan.SetNaN();
    
    aSatelliteInfo.ClearSatellitesInView();
    aSatelliteInfo.SetHorizontalDoP(nan); 
    aSatelliteInfo.SetTimeDoP(nan);
    aSatelliteInfo.SetHorizontalDoP(nan);  
  
    }
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetAddressInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetAddressGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldCountry))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountry, KCountry));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldCountry))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountryCode, KCountryCode));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldState))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldState, KState));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldCity))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCity, KCity));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldDistrict))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldDistrict, KDistrict));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldStreet))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreet, KStreet));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldStreetExtension))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreetExtension, KStreetExtension));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldLocationName))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocationName, KLocationName));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldPostalCode))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldPostalCode, KPostalCode));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldLocality))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocality, KLocality));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldCrossing1))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing1, KCrossing1));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldCrossing2))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing2, KCrossing2));
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetSpeedGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//	
EXPORT_C void CT_LbsTestPsyBase::SetSpeedGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldHorizontalSpeed))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeed, KHorizontalSpeed));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldHorizontalSpeedError))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeedError, KHorizontalSpeedError ));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldVerticalSpeedError))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeedError, KVerticalSpeedError));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldVerticalSpeed))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeed, KVerticalSpeed));
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetDirectionGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetDirectionGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldHeading))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeading, KHeading));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldHeadingError))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeadingError, KHeadingError));
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetCompassGenericInfoL(
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetCompassGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldTrueCourse))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourse, KTrueCourse));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldTrueCourseError))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourseError, KTrueCourseError));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldMagneticCourse))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMagneticCourse, KMagneticCourse));
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetBuildingGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
EXPORT_C void CT_LbsTestPsyBase::SetBuildingGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldBuildingName))
        {                                            
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingName, KBuildingName));
		}                            
	if(aGenericInfo.IsRequestedField(EPositionFieldBuildingFloor))
        {                                            
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingFloor, KBuildingFloor));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldBuildingRoom))
		{                            
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingRoom, KBuildingRoom));
		}                                       
	if(aGenericInfo.IsRequestedField(EPositionFieldBuildingZone))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingZone, KBuildingZone));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldBuildingTelephone))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingTelephone, KBuildingTelephone));
		}
    
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetNMEAGenericInfoL(
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
EXPORT_C void CT_LbsTestPsyBase::SetNMEAGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{	
	if(aGenericInfo.IsRequestedField(EPositionFieldNMEASentences))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentences, KNMEASentences));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldNMEASentencesStart))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentencesStart, KNMEASentencesStart));
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetSatelliteGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetSatelliteGenericInfoL(HPositionGenericInfo& aGenericInfo, const TBool& aUseCache)
	{
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteNumInView))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumInView, KSatelliteNumInView));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteNumUsed))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumUsed, KSatelliteNumUsed));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteTime))
		{
        TTime satTime;
        if(aUseCache)
            {
            satTime =iSatGenericTime;
            }
        else
            {
		    satTime.UniversalTime();
            iSatTime=satTime;
            }
        User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTime, satTime));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteHorizontalDoP))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteHorizontalDoP, KSatelliteHorizontalDoP));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteVerticalDoP))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteVerticalDoP, KSatelliteVerticalDoP));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteTimeDoP))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTimeDoP, KSatelliteTimeDoP));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatellitePositionDoP))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatellitePositionDoP, KSatellitePositionDoP));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteSeaLevelAltitude))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteSeaLevelAltitude, KSatelliteWGS84Altitude));
		}
	if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteGeoidalSeparation))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteGeoidalSeparation, KSatelliteGeoidalSeparation));
		}
	}
// ---------------------------------------------------------
// CT_LbsTestPsyBase::SetMediaGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::SetMediaGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
  	
    if (aGenericInfo.IsRequestedField(EPositionFieldMediaLinks))
		{
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinks, KMediaLinks));
			
		User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinksStart, KMediaLink1));
				
        TUint16 tmp = (TUint16)EPositionFieldMediaLinksStart;
		User::LeaveIfError(aGenericInfo.SetValue(++tmp, KMediaLink2));
            
        User::LeaveIfError(aGenericInfo.SetValue(++tmp, KMediaLink3));
        }

    }

// ---------------------------------------------------------
// CT_LbsTestPsyBase::ReportStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::ReportStatus(const TInt aDeviceStatus, const TInt aDataQuality)
	{
	TPositionModuleStatus moduleStatus;
	moduleStatus.SetDeviceStatus(aDeviceStatus);
	moduleStatus.SetDataQualityStatus(aDataQuality);
	iPositionerStatus->ReportStatus(moduleStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestPsyBase::CT_LbsTestPsyBase
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CT_LbsTestPsyBase::Trace(TBuf8 <KBufSize> msg) 
    {
     _LIT(KFileTrace, "c:\\Trace.txt");
 
    _LIT(KEnd,"\r\n");
    RFs fileserver;
    RFile file;
    msg.Append(KEnd);
    User::LeaveIfError(fileserver.Connect());
    TInt err = file.Open(fileserver, KFileTrace, EFileWrite);
    if (err == KErrNotFound) 
		{
        file.Create(fileserver, KFileTrace, EFileWrite);
		}

    file.Write(100000000,msg);

    file.Close();
    fileserver.Close();
	}


//  End of File  
