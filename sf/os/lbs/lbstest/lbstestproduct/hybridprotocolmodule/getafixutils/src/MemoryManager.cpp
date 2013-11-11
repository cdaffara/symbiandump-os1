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
// MemoryManager.cpp
//
// (c) Symbian Software, Ltd 2007
//

#include "MemoryManager.h"
//-------------------------------------------------------------------
CMemoryManager*CMemoryManager::NewL()
	{
	CMemoryManager* self = new (ELeave) CMemoryManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CMemoryManager::~CMemoryManager()
	{
	delete iLbsExternalRequestInfo;
	delete iLbsNetSessionId;
	delete iLbsNetPosRequestPrivacy;
	delete iLbsNetPosRequestQuality;
	delete iLbsNetPosRequestOptionsBase;
	delete iLbsNetPosRequestOptions;
	delete iLbsNetPosRequestOptionsAssistance;
	delete iLbsNetPosMethod;
	delete iLbsNetPosCapabilities;
	delete iLbsNetPosRequestMethod;
    delete iLbsPositionInfo;
    delete iLbsPositionCourseInfo;
    delete iLbsPositionSatelliteInfo;
    delete iLbsPositionExtendedSatelliteInfo;
    delete iLbsPositionGpsMeasurementInfo;
    delete iBuf;
    iLbsAssistanceDataBuilderSet->Close();	
    delete iLbsAssistanceDataBuilderSet;
	}    


CMemoryManager::CMemoryManager()
: iLbsExternalRequestInfo(NULL),iLbsNetSessionId(NULL),
  iLbsNetPosRequestPrivacy(NULL),iLbsNetPosRequestQuality(NULL),
  iLbsNetPosRequestOptionsBase(NULL),iLbsNetPosRequestOptions(NULL),	
  iLbsNetPosRequestOptionsAssistance(NULL),iLbsNetPosMethod(NULL),	
  iLbsNetPosCapabilities(NULL),iLbsNetPosRequestMethod(NULL),
  iLbsPositionInfo(NULL),iLbsPositionCourseInfo(NULL),
  iLbsPositionSatelliteInfo(NULL),iLbsPositionExtendedSatelliteInfo(NULL),iLbsPositionGpsMeasurementInfo(NULL),
  iBuf(NULL),iLbsAssistanceDataBuilderSet(NULL)
  {
  }

void CMemoryManager::ConstructL()
	{
    iLbsExternalRequestInfo = new (ELeave) TLbsExternalRequestInfo();
    iLbsNetSessionId = new (ELeave) TLbsNetSessionId();
    iLbsNetPosRequestPrivacy = new (ELeave) TLbsNetPosRequestPrivacy();
    iLbsNetPosRequestQuality = new (ELeave) TLbsNetPosRequestQuality();
    iLbsNetPosRequestOptionsBase = new (ELeave) TLbsNetPosRequestOptionsBase();
    iLbsNetPosRequestOptions = new (ELeave) TLbsNetPosRequestOptions();
    iLbsNetPosRequestOptionsAssistance= new (ELeave) TLbsNetPosRequestOptionsAssistance();
    iLbsNetPosMethod = new (ELeave) TLbsNetPosMethod();
    iLbsNetPosCapabilities = new (ELeave) TLbsNetPosCapabilities();
    iLbsNetPosRequestMethod = new (ELeave) TLbsNetPosRequestMethod();
    iLbsPositionInfo = new (ELeave) TPositionInfo();
    iLbsPositionCourseInfo = new (ELeave) TPositionCourseInfo();
    iLbsPositionSatelliteInfo = new (ELeave) TPositionSatelliteInfo();
    iLbsPositionExtendedSatelliteInfo = new (ELeave) TPositionExtendedSatelliteInfo();
    iLbsPositionGpsMeasurementInfo = new (ELeave) TPositionGpsMeasurementInfo();
	iBuf = HBufC8::NewMaxLC((TUint32)KMaxLength);
	CleanupStack::Pop(iBuf);
	iLbsAssistanceDataBuilderSet = new (ELeave) RLbsAssistanceDataBuilderSet();
	iLbsAssistanceDataBuilderSet->OpenL();
	}

TLbsExternalRequestInfo* CMemoryManager::LbsExternalRequestInfo()
	{
	return iLbsExternalRequestInfo;
	}	
TLbsNetSessionId* CMemoryManager::LbsNetSessionId()
	{
	return iLbsNetSessionId;
	}		
TLbsNetPosRequestPrivacy* CMemoryManager::LbsNetPosRequestPrivacy()
	{
	return iLbsNetPosRequestPrivacy;
	}	
TLbsNetPosRequestQuality* CMemoryManager::LbsNetPosRequestQuality()
	{
	return iLbsNetPosRequestQuality;
	}	
TLbsNetPosRequestOptionsBase* CMemoryManager::LbsNetPosRequestOptionsBase()
	{
	return iLbsNetPosRequestOptionsBase;
	}	
TLbsNetPosRequestOptions* CMemoryManager::LbsNetPosRequestOptions()
	{
	return iLbsNetPosRequestOptions;
	}	
TLbsNetPosRequestOptionsAssistance* CMemoryManager::LbsNetPosRequestOptionsAssistance()
	{
	return iLbsNetPosRequestOptionsAssistance;
	}	
TLbsNetPosMethod* CMemoryManager::LbsNetPosMethod()
	{
	return iLbsNetPosMethod;
	}	

TLbsNetPosCapabilities* CMemoryManager::LbsNetPosCapabilities()
	{
	return iLbsNetPosCapabilities;
	}	
TLbsNetPosRequestMethod* CMemoryManager::LbsNetPosRequestMethod()
	{
	return iLbsNetPosRequestMethod;
	}
TPositionInfo* CMemoryManager::LbsPositionInfo()
	{
	return iLbsPositionInfo;
	}
TPositionCourseInfo* CMemoryManager::LbsPositionCourseInfo()
	{
	return iLbsPositionCourseInfo;
	}
TPositionSatelliteInfo* CMemoryManager::LbsPositionSatelliteInfo()
	{
	return iLbsPositionSatelliteInfo;
	}
TPositionExtendedSatelliteInfo* CMemoryManager::LbsPositionExtendedSatelliteInfo()
    {
    return iLbsPositionExtendedSatelliteInfo;
    }

TPositionGpsMeasurementInfo* CMemoryManager::LbsPositionGpsMeasurementInfo()
	{
	return iLbsPositionGpsMeasurementInfo;
	}
HBufC8* CMemoryManager::MaxBuf()
	{
	return iBuf;
	}
RLbsAssistanceDataBuilderSet* CMemoryManager::LbsAssistanceDataBuilderSet()
	{
	return iLbsAssistanceDataBuilderSet;
	}


