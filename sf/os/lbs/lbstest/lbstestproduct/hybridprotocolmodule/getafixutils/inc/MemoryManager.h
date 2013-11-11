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
* Description:
*
*/
//
// PropPipe.h
//
//
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include <e32base.h>
#include <e32std.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>

const TInt KMaxLength= 1024;
//------------------------------------------------------------------------------------------------

class CMemoryManager : public CBase
	{
public:
	static CMemoryManager* NewL();
	~CMemoryManager();
	TLbsExternalRequestInfo* LbsExternalRequestInfo();
	TLbsNetSessionId * LbsNetSessionId();                           
	TLbsNetPosRequestPrivacy* LbsNetPosRequestPrivacy();
	TLbsNetPosRequestQuality* LbsNetPosRequestQuality(); 
	TLbsNetPosRequestOptionsBase* LbsNetPosRequestOptionsBase();
	TLbsNetPosRequestOptions* LbsNetPosRequestOptions();
	TLbsNetPosRequestOptionsAssistance* LbsNetPosRequestOptionsAssistance();
	TLbsNetPosMethod* LbsNetPosMethod();
	TLbsNetPosCapabilities* LbsNetPosCapabilities();
	TLbsNetPosRequestMethod* LbsNetPosRequestMethod();
	TPositionInfo* LbsPositionInfo();
	TPositionCourseInfo* LbsPositionCourseInfo();
	TPositionSatelliteInfo* LbsPositionSatelliteInfo();
	TPositionExtendedSatelliteInfo* LbsPositionExtendedSatelliteInfo();
	TPositionGpsMeasurementInfo* LbsPositionGpsMeasurementInfo();
	HBufC8* MaxBuf();
	RLbsAssistanceDataBuilderSet* LbsAssistanceDataBuilderSet();

private:
	CMemoryManager();
	void ConstructL();

private:
	TLbsExternalRequestInfo* iLbsExternalRequestInfo;
	TLbsNetSessionId* iLbsNetSessionId;
	TLbsNetPosRequestPrivacy* iLbsNetPosRequestPrivacy;
	TLbsNetPosRequestQuality* iLbsNetPosRequestQuality;
	TLbsNetPosRequestOptionsBase* iLbsNetPosRequestOptionsBase;
	TLbsNetPosRequestOptions* iLbsNetPosRequestOptions;
	TLbsNetPosRequestOptionsAssistance* iLbsNetPosRequestOptionsAssistance;
	TLbsNetPosMethod* iLbsNetPosMethod;
	TLbsNetPosCapabilities* iLbsNetPosCapabilities;
	TLbsNetPosRequestMethod* iLbsNetPosRequestMethod;
	TPositionInfo* iLbsPositionInfo;
	TPositionCourseInfo* iLbsPositionCourseInfo;
	TPositionSatelliteInfo* iLbsPositionSatelliteInfo;
	TPositionExtendedSatelliteInfo* iLbsPositionExtendedSatelliteInfo;
	TPositionGpsMeasurementInfo* iLbsPositionGpsMeasurementInfo;
	HBufC8*iBuf;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};


#endif // __MEMORY_MANAGER_H__	

