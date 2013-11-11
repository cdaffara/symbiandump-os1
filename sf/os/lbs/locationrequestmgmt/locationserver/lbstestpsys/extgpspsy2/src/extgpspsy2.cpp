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
//



#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase
#include <lbssatellite.h>        // for TPositionSatelliteInfo

#include <lbs/epos_mpositionerstatus.h>

#include "extgpspsy2.h"
#include "extgpspsy2.hrh"


const TInt KSecondToMicro = 1000000;

const TImplementationProxy KFactoryPtr = {{KExtGpsPsy2ImplUid}, (TProxyNewLPtr)CExtGpsPsy2::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

CExtGpsPsy2* CExtGpsPsy2::NewL(TAny* aConstructionParameters)
    {
    CExtGpsPsy2* self = new(ELeave) CExtGpsPsy2;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CExtGpsPsy2::ConstructL(TAny* aConstructionParameters)
{
    BaseConstructL(aConstructionParameters);
    iUid = ImplementationUid();
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
}


CExtGpsPsy2::~CExtGpsPsy2()
{
    iPsyConfigArray.Close();
    if(iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
}


void CExtGpsPsy2::NotifyPositionUpdate(
                TPositionInfoBase& aPosInfo,
                TRequestStatus& aStatus)
    {
    iRequestStatus = &aStatus;
    iPositionInfoBase = &aPosInfo;
    if(aPosInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        //Check if this is confiuration request
        HPositionGenericInfo* genInfo =
            static_cast<HPositionGenericInfo*>(&aPosInfo);
        if(genInfo->IsRequestedField(KIntGpsPsy1ConfigItemsNumber))
            {
            //Config PSY
            TRAPD(err, ConfigPsyL(*genInfo));
            CompleteRequest(err);
            StartTimerIfNeeded();
            return;
            }
        }

    StartTimerIfNeeded();
    }

void CExtGpsPsy2::ConfigPsyL(const HPositionGenericInfo& aGenInfo)
    {
    TInt32 configUid;
    User::LeaveIfError(aGenInfo.GetValue(KIntGpsPsy1ConfigPsyUid, configUid));
    if(configUid!=iUid.iUid)
        {
        User::Leave(KErrNotSupported);
        }

    iPsyConfigArray.Reset();
    iCurrentIndex = 0;
    TInt32 numOfItem;
    User::LeaveIfError(aGenInfo.GetValue(KIntGpsPsy1ConfigItemsNumber, numOfItem));
    for(TInt i=0; i<numOfItem; i++)
        {
        TPsyConfig psyConfig;
        TPckg<TPsyConfig> configBuf(psyConfig);
        
        User::LeaveIfError(aGenInfo.GetValue(KIntGpsPsy1ConfigItemsNumber+i+1, configBuf));
        User::LeaveIfError(iPsyConfigArray.Append(psyConfig));
        }
    }

TInt CExtGpsPsy2::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CExtGpsPsy2*>(aAny)->TimerCompleted();
    return KErrNone;
    }
        
void CExtGpsPsy2::TimerCompleted()
    {
    TPsyConfig& config(iPsyConfigArray[iCurrentIndex]);
    if(config.iType==TPsyConfig::EConfigLRResponse)
        {
        //If no pending LR, then just return
        if(!iRequestStatus)
            {
            return;
            }
            
        TInt err = config.iData.iLRConfig.iErr;
        //complete location request
        if(iPositionInfoBase->PositionClassType() & EPositionInfoClass)
            {
            //Set TPositionInfo
            TPosition pos;
            pos.SetCoordinate(
                config.iData.iLRConfig.iLat,
                config.iData.iLRConfig.iLon,
                config.iData.iLRConfig.iAlt);
                
            TPositionInfo* posInfo = reinterpret_cast<TPositionInfo*>(iPositionInfoBase);
            posInfo->SetPosition(pos);
            }
        if(iPositionInfoBase->PositionClassType() & EPositionGenericInfoClass)
            {
            //Set HGeneric Info
            HPositionGenericInfo* genInfo =
                static_cast<HPositionGenericInfo*>(iPositionInfoBase);
            if(genInfo->IsRequestedField(EPositionFieldNMEASentences))
                {
                genInfo->SetValue(EPositionFieldNMEASentences, TInt8(1));
                HBufC8* nmea = NULL;
                TRAP(err, nmea = HBufC8::NewL(config.iData.iLRConfig.iNmeaDataSize));
                if(KErrNone == err)
                	{
	                TPtr8 nmeaPtr(nmea->Des());
	                nmeaPtr.Fill('H', config.iData.iLRConfig.iNmeaDataSize);
	                err = genInfo->SetValue(EPositionFieldNMEASentences+1, *nmea);
                	}
                delete nmea;
                }
            }
        CompleteRequest(err);
        if(config.iData.iLRConfig.iNumOfResponse>1)
            {
            config.iData.iLRConfig.iNumOfResponse--;
            }
        else if(config.iData.iLRConfig.iNumOfResponse>0)
            {
            iCurrentIndex++;
            }
        else
            {
            //0 means forever response with this
            }
        }
    else //ECinfigModuleStatus
        {
        //Change module status
        TPositionModuleStatus modStatus;
        modStatus.SetDataQualityStatus(config.iData.iStatusConfig.iDataQuality);
        modStatus.SetDeviceStatus(config.iData.iStatusConfig.iDeviceStatus);
        MPositionerStatus* observer = PositionerStatus();
        observer->ReportStatus(modStatus);
        iCurrentIndex++;
        }
    iTimer->Cancel();
    
    if(iCurrentIndex>=iPsyConfigArray.Count())
        {
        //When all items are used, then clean the config items
        iPsyConfigArray.Reset();
        iCurrentIndex = 0;
        }
        
    StartTimerIfNeeded();
    }

void CExtGpsPsy2::StartTimerIfNeeded()
    {
    //If timer is already runing, then we don't do anything
    if(iTimer->IsActive())
        {
        return;
        }
        
    if(iCurrentIndex>=0 && iCurrentIndex<iPsyConfigArray.Count())
        {
        TPsyConfig& config(iPsyConfigArray[iCurrentIndex]);
        if(config.iType==TPsyConfig::EConfigLRResponse)
            {
            if(iRequestStatus)
                {
                //If there is location request waiting then start timer
                iTimer->Start(
                    config.iData.iLRConfig.iResponseTime*KSecondToMicro,
                    1,
                    TCallBack(TimerCallback, this));
                }
            }
        else //ECinfigModuleStatus
            {
            //Start timer
            iTimer->Start(
                config.iData.iStatusConfig.iResponseTime*KSecondToMicro,
                1,
                TCallBack(TimerCallback, this));
            }
        }
        
    //If we don't start timer but there is LR pending, we shall complete the 
    //request by default.
    if(!iTimer->IsActive() && iRequestStatus)
        {
        CompleteRequestByDefault();
        }
    }

void CExtGpsPsy2::CompleteRequestByDefault()
    {
    if(!iRequestStatus)
        {
        return;
        }
        
    if(iPositionInfoBase->PositionClassType() & EPositionInfoClass)
        {
        TPositionInfo* posInfo = static_cast<TPositionInfo*>(iPositionInfoBase);
        TPosition pos;
        pos.SetCoordinate(1.0, 1.0, 1.0);
        pos.SetAccuracy(1.0, 1.0);
        posInfo->SetPosition(pos);
        }
        
    CompleteRequest(KErrNone);
    }

void CExtGpsPsy2::CompleteRequest(TInt aCompletionCode)
    {
    if(iRequestStatus)
        {
        iPositionInfoBase->SetModuleId(iUid);
        User::RequestComplete(iRequestStatus, aCompletionCode);
        }
    }

void CExtGpsPsy2::CancelNotifyPositionUpdate()
{
    if(iRequestStatus &&
        iCurrentIndex>=0 && iCurrentIndex<iPsyConfigArray.Count())
        {
        TPsyConfig& config(iPsyConfigArray[iCurrentIndex]);

        if(config.iData.iLRConfig.iNumOfResponse>1)
            {
            config.iData.iLRConfig.iNumOfResponse--;
            }
        else if(config.iData.iLRConfig.iNumOfResponse>0)
            {
            iCurrentIndex++;
            if(iCurrentIndex>=iPsyConfigArray.Count())
                {
                //When all items are used, then clean the config items
                iPsyConfigArray.Reset();
                iCurrentIndex = 0;
                }
            }
        else
            {
            //0 means forever response with this
            }
        iTimer->Cancel();
        }
    CompleteRequest(KErrCancel);
    StartTimerIfNeeded();
}


TBool CExtGpsPsy2::TrackingOverridden() const
{
    return ETrue;
}

void CExtGpsPsy2::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval*/)
{
}

void CExtGpsPsy2::StopTracking()
{
}

// End of file

