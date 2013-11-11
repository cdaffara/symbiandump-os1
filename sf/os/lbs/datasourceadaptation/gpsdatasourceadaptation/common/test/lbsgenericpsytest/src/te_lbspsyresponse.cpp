// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The class representing a response from the LBS sub-system to the PSY. 
// 
//

#include <test/testexecutestepbase.h>

#include "te_lbspsyresponse.h"
#include "te_lbspsybasestep.h"
#include "te_lbspsystaticdata.h"

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueSimplePosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode)
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(EPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	TLbsPositionUpdateData data;
	MakeDefaultPosUpdate(data);
	data.iPosInfo.SetPositionMode(aPosMode);
	self->SetAndSavePosUpdateL(data);
	return self;
	}

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueOutdatedPosUpdateLC()
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(EPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	TLbsPositionUpdateData data;
	MakeDefaultPosUpdate(data);	
	TTime now;
	now.UniversalTime();
	now-=TTimeIntervalSeconds(KOldPosUpdateAge);
	data.iActualTime = now;
	self->SetAndSavePosUpdateL(data);
	return self;
	}

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssuePartialPosUpdateLC()
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(EPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	TLbsPositionUpdateData data;
	MakeDefaultPosUpdate(data);
	TPosition pos;
	TRealX nan;
	nan.SetNaN();
	pos.SetCoordinate(nan, 2.0, 3.0);
	data.iPosInfo.SetPosition(pos);
	self->SetAndSavePosUpdateL(data);
	return self;
	}

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueErrPosUpdateLC(TInt aError, TPositionModuleInfo::TTechnologyType aPosMode)
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(EPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	TLbsPositionUpdateData data;
	MakeEmptyPosUpdate(data);
	data.iPosInfo.SetPositionMode(aPosMode);
	data.iError = aError;
	self->SetAndSavePosUpdateL(data);
	return self;
	}

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueRefPosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode)
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(ENetPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	TLbsPositionUpdateData data;
	MakeDefaultPosUpdate(data);
	data.iPosInfo.SetPositionMode(aPosMode);
	self->SetAndSaveRefPosUpdateL(data);
	return self;
	}

EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueFinalNetPosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode)
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(ENetPosUpdate, RLbsNetworkPositionUpdates::ENetworkPositionFinal);
	TLbsPositionUpdateData data;
	MakeDefaultPosUpdate(data);
	data.iPosInfo.SetPositionMode(aPosMode);
	self->SetAndSaveRefPosUpdateL(data);
	return self;
	}


EXPORT_C CTe_PsyResponse* CTe_PsyResponse::IssueModuleStatusLC(const TPositionModuleStatus& aModuleStatus,
			const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet)
	{
	CTe_PsyResponse* self = CTe_PsyResponse::NewLC(EModuleStatus, RLbsNetworkPositionUpdates::ENetworkPositionReference);
	self->SetModuleStatusL(aModuleStatus, aOccurredEventsSinceLastSet);
	return self;
	}

CTe_PsyResponse* CTe_PsyResponse::NewLC(TType aType, 
		RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId aNetPosBusId)
	{
	CTe_PsyResponse* self = new(ELeave) CTe_PsyResponse(aType);
	CleanupStack::PushL(self);
	self->ConstructL(aNetPosBusId);
	return self;
	}

CTe_PsyResponse::~CTe_PsyResponse()
	{
	iNetPosUpdates.Close();
	iModStatus.Close();
	iPosUpdates.Close();
	}
	
CTe_PsyResponse::CTe_PsyResponse(TType aType) : iType(aType)
	{
	}

void CTe_PsyResponse::ConstructL(RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId aNetPosBusId)
	{
	TUid moduleUid = MTe_LbsPsyStaticData::Static().ModuleUid();
	iPosUpdates.OpenL(moduleUid);
	iModStatus.OpenL(moduleUid);
	iNetPosUpdates.OpenL(aNetPosBusId);
	}

/* Compares the received TPositionInfo with that sent */
TBool CTe_PsyResponse::Compare(const TPositionInfo& aSatInfo) const
	{
	const TPositionSatelliteInfo& sentInfo = iPosUpdateData.iPosInfo;
	
	__ASSERT_ALWAYS(iType==EPosUpdate || iType==ENetPosUpdate , User::Invariant());
	
	
	TBool infoEqual = ETrue;
	TPosition recvPos;
	aSatInfo.GetPosition(recvPos);
	TPosition sentPos;
	sentInfo.GetPosition(sentPos);
	if(aSatInfo.UpdateType()!=sentInfo.UpdateType() || aSatInfo.PositionMode()!=sentInfo.PositionMode() ||
	   aSatInfo.PositionModeReason()!=sentInfo.PositionModeReason() || 
	   !DoubleCompare(recvPos.Altitude(),sentPos.Altitude()) ||
	   !DoubleCompare(recvPos.Latitude(),sentPos.Latitude()) || 
	   !DoubleCompare(recvPos.Longitude(),sentPos.Longitude()) || 
	   recvPos.Datum()!=sentPos.Datum() || 
	   !DoubleCompare(recvPos.HorizontalAccuracy(),sentPos.HorizontalAccuracy()) || 
	   !DoubleCompare(recvPos.VerticalAccuracy(),sentPos.VerticalAccuracy())|| 
	   recvPos.Time()!=sentPos.Time())
		{
		infoEqual = EFalse;
		}

	if(iType!=ENetPosUpdate && aSatInfo.PositionClassType() & EPositionCourseInfoClass)
		{
		const TPositionCourseInfo& recvInfo = reinterpret_cast<const TPositionCourseInfo&>(aSatInfo);
		TCourse recvCourse;
		recvInfo.GetCourse(recvCourse);
		TCourse sentCourse;
		sentInfo.GetCourse(sentCourse);
			
		if(!DoubleCompare(recvCourse.Heading(),sentCourse.Heading()) || 
		   !DoubleCompare(recvCourse.HeadingAccuracy(),sentCourse.HeadingAccuracy()) ||
		   !DoubleCompare(recvCourse.SpeedAccuracy(),sentCourse.SpeedAccuracy()) ||
		   !DoubleCompare(recvCourse.Course(),sentCourse.Course()) ||
		   !DoubleCompare(recvCourse.CourseAccuracy(),sentCourse.CourseAccuracy()))
			{
			
			infoEqual = EFalse;
			}
		}
					
	if(iType!=ENetPosUpdate && aSatInfo.PositionClassType() & EPositionSatelliteInfoClass)
		{
		const TPositionSatelliteInfo& recvInfo = reinterpret_cast<const TPositionSatelliteInfo&>(aSatInfo);
		if(recvInfo.NumSatellitesInView()!=sentInfo.NumSatellitesInView() || 
		   recvInfo.NumSatellitesUsed()!=sentInfo.NumSatellitesUsed() ||
		   recvInfo.SatelliteTime()!=sentInfo.SatelliteTime() ||
		   !DoubleCompare(recvInfo.HorizontalDoP(),sentInfo.HorizontalDoP()) ||
		   !DoubleCompare(recvInfo.VerticalDoP(),sentInfo.VerticalDoP()) ||
		   !DoubleCompare(recvInfo.TimeDoP(),sentInfo.TimeDoP()))
			{
			infoEqual = EFalse;
			}
		
		for(TInt i=0; i<recvInfo.NumSatellitesInView(); i++)
			{
			TSatelliteData recvSatData;
			recvInfo.GetSatelliteData(i, recvSatData);
			TSatelliteData sentSatData;
			sentInfo.GetSatelliteData(i, sentSatData);
			if(recvSatData.SatelliteId()!=sentSatData.SatelliteId() || 
			   !DoubleCompare(recvSatData.Azimuth(),sentSatData.Azimuth()) ||	
			   !DoubleCompare(recvSatData.Elevation(),sentSatData.Elevation()) || 
			   recvSatData.IsUsed()!=sentSatData.IsUsed() ||
			   recvSatData.SignalStrength()!=sentSatData.SignalStrength())
				{
				infoEqual = EFalse;
				break;
				}
			}
		}
	
	if(iType!=ENetPosUpdate && aSatInfo.PositionClassType() & EPositionGenericInfoClass)
		{
		const HPositionGenericInfo* posInfo = static_cast<const HPositionGenericInfo*>(&aSatInfo);
		
		TInt8 int8Data;
		TTime timeData;
		TReal32 real32Data;
		
		if(posInfo->IsRequestedField(EPositionFieldSatelliteNumInView))
		   {
		   if(posInfo->GetValue(EPositionFieldSatelliteNumInView,int8Data)!=KErrNone || 
				   int8Data!=sentInfo.NumSatellitesInView())
			   {
			   infoEqual = EFalse;
			   }
		   }
		
		if(posInfo->IsRequestedField(EPositionFieldSatelliteNumUsed))
			{
			if(posInfo->GetValue(EPositionFieldSatelliteNumUsed,int8Data)!=KErrNone || 
							   int8Data!=sentInfo.NumSatellitesUsed())
				{
				infoEqual = EFalse;
				}	            
		    }

		if(posInfo->IsRequestedField(EPositionFieldSatelliteTime))
			{
			if(posInfo->GetValue(EPositionFieldSatelliteTime,timeData)!=KErrNone || 
					timeData!=sentInfo.SatelliteTime())
				{
				infoEqual = EFalse;
				}		            
		    }

		if(posInfo->IsRequestedField(EPositionFieldSatelliteHorizontalDoP))
			{
			if(posInfo->GetValue(EPositionFieldSatelliteHorizontalDoP, real32Data)!=KErrNone || 
					real32Data!=sentInfo.HorizontalDoP())
				{
				infoEqual = EFalse;
				}		    
		    }

		if(posInfo->IsRequestedField(EPositionFieldSatelliteVerticalDoP))
			{
			if(posInfo->GetValue(EPositionFieldSatelliteVerticalDoP, real32Data)!=KErrNone || 
								real32Data!=sentInfo.VerticalDoP())
				{
				infoEqual = EFalse;
				}
			}
			
		if(posInfo->IsRequestedField(EPositionFieldSatellitePositionDoP))
			{
			if(posInfo->GetValue(EPositionFieldSatellitePositionDoP, real32Data)!=KErrNone || 
											real32Data!=sentInfo.TimeDoP())
				{
				infoEqual = EFalse;
				}
			}
			
		TCourse sentCourse;
		sentInfo.GetCourse(sentCourse);
    
		if(posInfo->IsRequestedField(EPositionFieldHorizontalSpeed))
			{
			if(posInfo->GetValue(EPositionFieldHorizontalSpeed, real32Data)!=KErrNone || 
				real32Data!=sentCourse.Speed())
				{
				infoEqual = EFalse;
				}
			}
					            
		if(posInfo->IsRequestedField(EPositionFieldHeading))
			{
			if(posInfo->GetValue(EPositionFieldHeading, real32Data)!=KErrNone || 
					real32Data!=sentCourse.Heading())
				{
				infoEqual = EFalse;
				}
			}
		}
	
	if(!infoEqual)
		{
		INFO_PRINTF1(_L("The sent & received pos updates are not equal."));
		INFO_PRINTF1(_L("The sent pos update:"));
		MTe_LbsPsyStaticData::Static().PrintPosInfo(sentInfo);
		
		INFO_PRINTF1(_L("The recv pos update:"));
		MTe_LbsPsyStaticData::Static().PrintPosInfo(aSatInfo);
		}
		
	return infoEqual;
	}

void CTe_PsyResponse::SetAndSavePosUpdateL(const TLbsPositionUpdateData& aData)
	{
	iPosUpdateData = aData;
	User::LeaveIfError(iPosUpdates.SetPositionInfo(aData.iError, aData.iConflictControl, &aData.iPosInfo, 
				sizeof(TPositionExtendedSatelliteInfo), aData.iTargetTime, aData.iActualTime, aData.iAttributes, 
				aData.iGpsMode));	
	}

void CTe_PsyResponse::SetAndSaveRefPosUpdateL(const TLbsPositionUpdateData& aData)
	{
	iPosUpdateData = aData;
	
	TLbsNetSessionIdInt sessionId; //not used
	TPositionInfo posInfo;
	posInfo = aData.iPosInfo;
	User::LeaveIfError(iNetPosUpdates.SetPositionInfo(sessionId, KErrNone, posInfo, aData.iActualTime));
	}

void CTe_PsyResponse::MakeDefaultPosUpdate(TLbsPositionUpdateData& aUpdateData)
	{
	aUpdateData.iError = KErrNone;
	aUpdateData.iConflictControl = EFalse;
	aUpdateData.iTargetTime = 0;
			
	TTime now;
	now.UniversalTime();
	aUpdateData.iActualTime = now;
	aUpdateData.iAttributes = 0;
	aUpdateData.iGpsMode = RLbsPositionUpdates::EGpsModeAutonomous;
			
	//TPositionSatelliteInfoSymbian2& info = aUpdateData.iPosInfo;
	TPositionSatelliteInfo& info = aUpdateData.iPosInfo;

	TUid moduleUid = MTe_LbsPsyStaticData::Static().ModuleUid();
	info.SetModuleId(moduleUid);
	info.SetUpdateType(EPositionUpdateGeneral);
	info.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal);
	info.SetPositionModeReason(EPositionModeReasonNone);
			
	TPosition pos;
	pos.SetCoordinate(1.0, 2.0, 3.0);
	pos.SetDatum(KPositionDatumWgs84);
	pos.SetAccuracy(0.1, 0.2);
	pos.SetCurrentTime();
	info.SetPosition(pos);
			
	TCourse course;
	course.SetHeading(4.0);
	course.SetHeadingAccuracy(0.3);
	course.SetSpeed(5.0);
	course.SetSpeedAccuracy(0.4);
	info.SetCourse(course);
	
	info.SetHorizontalDoP(0.5);
	info.SetVerticalDoP(0.6);
	info.SetTimeDoP(0.7);
	info.SetSatelliteTime(now);
	
	for(TInt i=0;i<3;i++)
		{
		TSatelliteData satData;
		satData.SetAzimuth(5.0);
		satData.SetElevation(6.0);
		satData.SetIsUsed((i+1)%2);
		satData.SetSatelliteId(i);
		satData.SetSignalStrength(i);
		info.AppendSatelliteData(satData);
		}
	}

void CTe_PsyResponse::MakeEmptyPosUpdate(TLbsPositionUpdateData& aUpdateData)
	{
	aUpdateData.iError = KErrNone;
	aUpdateData.iConflictControl = EFalse;
	aUpdateData.iTargetTime = 0;
				
	TTime now;
	now.UniversalTime();
	aUpdateData.iActualTime = now;
	aUpdateData.iAttributes = 0;
	aUpdateData.iGpsMode = RLbsPositionUpdates::EGpsModeAutonomous;
				
//	TPositionSatelliteInfoSymbian2& info = aUpdateData.iPosInfo;
	TPositionSatelliteInfo& info = aUpdateData.iPosInfo;

	TUid moduleUid = MTe_LbsPsyStaticData::Static().ModuleUid();
	info.SetModuleId(moduleUid);
	info.SetUpdateType(EPositionUpdateGeneral);
	info.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal);
	info.SetPositionModeReason(EPositionModeReasonNone);
	
	TRealX nan;
	nan.SetNaN();
	TPosition pos;
	pos.SetCoordinate(nan, nan, nan);
	pos.SetDatum(KPositionDatumWgs84);
	pos.SetAccuracy(nan, nan);
	pos.SetCurrentTime();
	info.SetPosition(pos);
				
	TCourse course;
	course.SetHeading(nan);
	course.SetHeadingAccuracy(nan);
	course.SetSpeed(nan);
	course.SetSpeedAccuracy(nan);
	info.SetCourse(course);
	
	info.SetHorizontalDoP(nan);
	info.SetVerticalDoP(nan);
	info.SetTimeDoP(nan);
	info.SetSatelliteTime(now);
	}


void CTe_PsyResponse::SetModuleStatusL(const TPositionModuleStatus& aModuleStatus,
		const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet)
	{
	iModStatusData = aModuleStatus;
	User::LeaveIfError(
		iModStatus.SetModuleStatus(&aModuleStatus,sizeof(TPositionModuleStatus),
		aOccurredEventsSinceLastSet));
	}

TBool CTe_PsyResponse::DoubleCompare(TReal64 aFirst, TReal64 aSecond) const
	{
	if(Math::IsNaN(aFirst) && Math::IsNaN(aSecond))
		{
		return ETrue;
		}
	if(aFirst==aSecond)
		{
		return ETrue;
		}
	return EFalse;
	}

