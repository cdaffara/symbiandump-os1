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
// The class for retrieving the test static data.
// 
//

#include <lbssatellite.h>
#include <test/testexecutestepbase.h>

#include "te_lbspsystaticdata.h"

EXPORT_C void MTe_LbsPsyStaticData::SetStaticL(MTe_LbsPsyStaticData* aData)
	{
	__ASSERT_ALWAYS(!Dll::Tls(), User::Invariant());
	User::LeaveIfError(Dll::SetTls(aData));
	}

MTe_LbsPsyStaticData& MTe_LbsPsyStaticData::Static()
	{
	__ASSERT_ALWAYS(Dll::Tls(), User::Invariant());
	return *(reinterpret_cast<MTe_LbsPsyStaticData*>(Dll::Tls()));
	}

EXPORT_C void MTe_LbsPsyStaticData::PrintPosInfo(const TPositionInfo& aPosInfo) const
	{	
	_LIT(KTimeFormat, "%H:%T:%S.%C");
	TBuf<100> cTimeStr;
	
	INFO_PRINTF2(_L("classSize=%d"), aPosInfo.PositionClassSize());
	INFO_PRINTF2(_L("classType=0x%x"), aPosInfo.PositionClassType());
	INFO_PRINTF2(_L("moduleId=0x%x"), aPosInfo.ModuleId());
	INFO_PRINTF2(_L("updateType=%d"), aPosInfo.UpdateType());
	INFO_PRINTF2(_L("positionMode=%d"), aPosInfo.PositionMode());
	INFO_PRINTF2(_L("positionModeReason=%d"), aPosInfo.PositionModeReason());
	
	TPosition pos;
	aPosInfo.GetPosition(pos);
	INFO_PRINTF2(_L("pos altitude=%f"), pos.Altitude());
	INFO_PRINTF2(_L("pos latitude=%f"), pos.Latitude());
	INFO_PRINTF2(_L("pos longitude=%f"), pos.Longitude());
	INFO_PRINTF2(_L("pos datum=0x%x"), pos.Datum());
	INFO_PRINTF2(_L("pos horAccuracy=%f"), pos.HorizontalAccuracy());
	INFO_PRINTF2(_L("pos verAccuracy=%f"), pos.VerticalAccuracy());
						
	TRAP_IGNORE(pos.Time().FormatL(cTimeStr, KTimeFormat);)
	INFO_PRINTF2(_L("pos time=%S"), &cTimeStr);
	
	if(aPosInfo.PositionClassType() & EPositionCourseInfoClass)
		{
		const TPositionCourseInfo& info = reinterpret_cast<const TPositionCourseInfo&>(aPosInfo);
		TCourse course;
		info.GetCourse(course);
		
		INFO_PRINTF2(_L("course heading=%f"), course.Heading());
		INFO_PRINTF2(_L("course headingAccuracy=%f"), course.HeadingAccuracy());
		INFO_PRINTF2(_L("course speedAccuracy=%f"), course.SpeedAccuracy());		
		INFO_PRINTF2(_L("course course=%f"), course.Course());
		INFO_PRINTF2(_L("course courseAccuracy=%f"), course.CourseAccuracy());
		}
	
	if(aPosInfo.PositionClassType() & EPositionSatelliteInfoClass)
		{
		const TPositionSatelliteInfo& info = reinterpret_cast<const TPositionSatelliteInfo&>(aPosInfo);
		INFO_PRINTF2(_L("satInfo satellitesInView=%d"), info.NumSatellitesInView());
		INFO_PRINTF2(_L("satInfo satellitesUsed=%d"), info.NumSatellitesUsed());
		TRAP_IGNORE(info.SatelliteTime().FormatL(cTimeStr, KTimeFormat);)
		INFO_PRINTF2(_L("satInfo satTime=%S"), &cTimeStr);
		INFO_PRINTF2(_L("satInfo horDoP=%f"), info.HorizontalDoP());
		INFO_PRINTF2(_L("satInfo verDoP=%f"), info.VerticalDoP());
		INFO_PRINTF2(_L("satInfo timeDoP=%f"), info.TimeDoP());	
					
		for(TInt i=0; i<info.NumSatellitesInView(); i++)
			{
			TSatelliteData satData;
			info.GetSatelliteData(i, satData);
			
			INFO_PRINTF3(_L("sat%d id=%d"), i, satData.SatelliteId());
			INFO_PRINTF3(_L("sat%d azimuth=%f"), i, satData.Azimuth());
			INFO_PRINTF3(_L("sat%d elevation=%f"), i, satData.Elevation());
			INFO_PRINTF3(_L("sat%d isUsed=%d"), i, satData.IsUsed());
			INFO_PRINTF3(_L("sat%d signalStrength=%d"), i, satData.SignalStrength());
			}	
		}
	}

