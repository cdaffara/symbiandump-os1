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
#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase
#include <lbssatellite.h>        // for TPositionSatelliteInfo

#include <lbs/epos_mpositionerstatus.h>

#include "intgpspsy1.h"
#include "intgpspsy1.hrh"


const TInt KSecondToMicro = 1000000;

const TImplementationProxy KFactoryPtr = {{KIntGpsPsy1ImplUid}, (TProxyNewLPtr)CIntGpsPsy1::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

CIntGpsPsy1* CIntGpsPsy1::NewL(TAny* aConstructionParameters)
    {
    CIntGpsPsy1* self = new(ELeave) CIntGpsPsy1;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CIntGpsPsy1::ConstructL(TAny* aConstructionParameters)
{
    BaseConstructL(aConstructionParameters);
    iUid = ImplementationUid();
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
}


CIntGpsPsy1::~CIntGpsPsy1()
{
    iPsyConfigArray.Close();
    if(iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
}


void CIntGpsPsy1::NotifyPositionUpdate(
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

void CIntGpsPsy1::ConfigPsyL(const HPositionGenericInfo& aGenInfo)
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

TInt CIntGpsPsy1::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CIntGpsPsy1*>(aAny)->TimerCompleted();
    return KErrNone;
    }
        
void CIntGpsPsy1::TimerCompleted()
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
        if(iPositionInfoBase->PositionClassType() & EPositionSatelliteInfoClass)
            {
            TPositionSatelliteInfo* satInfo = 
                static_cast<TPositionSatelliteInfo*>(iPositionInfoBase);
            SetSatelliteInfo(*satInfo);
            }
        if(iPositionInfoBase->PositionClassType() & EPositionGenericInfoClass)
            {
            //Set HGeneric Info
            HPositionGenericInfo* genInfo =
                static_cast<HPositionGenericInfo*>(iPositionInfoBase);
            SetHGenericInfoData(*genInfo);
            /*
            if(genInfo->IsRequestedField(EPositionFieldNMEASentences))
                {
                genInfo->SetValue(EPositionFieldNMEASentences, TInt8(1));
                HBufC8* nmea = HBufC8::NewLC(config.iData.iLRConfig.iNmeaDataSize);
                TPtr8 nmeaPtr(nmea->Des());
                nmeaPtr.Fill('H', config.iData.iLRConfig.iNmeaDataSize);
                err = genInfo->SetValue(EPositionFieldNMEASentences+1, *nmea);
                delete nmea;
                }*/
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

void CIntGpsPsy1::SetHGenericInfoData(HPositionGenericInfo& aGenInfo)
    {
    //We set only these fields
    //TDes16
    _LIT(KDes16, "This is comment line");
    aGenInfo.SetRequestedField(EPositionFieldComment);
    aGenInfo.SetValue(EPositionFieldComment, KDes16);
    
    //TReal32
    TReal32 real32=1234.56;
    aGenInfo.SetRequestedField(EPositionFieldHorizontalSpeed);
    aGenInfo.SetValue(EPositionFieldHorizontalSpeed, real32); 
    
    //TUint8
    TUint8 uint8 = 1;
    aGenInfo.SetRequestedField(EPositionFieldMediaLinks);
    aGenInfo.SetValue(EPositionFieldMediaLinks, uint8);
    
    //TDes8
    _LIT8(KDes8, "This is is des8");
    aGenInfo.SetValue(EPositionFieldMediaLinksStart, KDes8);
    
    //TInt8
    TInt8 int8 = 123;
    aGenInfo.SetRequestedField(EPositionFieldSatelliteNumInView);
    aGenInfo.SetValue(EPositionFieldSatelliteNumInView, int8);
    
    //TTime
    TTime time;
    time.UniversalTime();
    aGenInfo.SetRequestedField(EPositionFieldSatelliteTime);
    aGenInfo.SetValue(EPositionFieldSatelliteTime, time);
    }

void CIntGpsPsy1::SetSatelliteInfo(TPositionSatelliteInfo& aSatInfo)
    {
    TSatelliteData data;
    data.SetSatelliteId(2);
    data.SetAzimuth(30);
    data.SetIsUsed(ETrue);
    for( TInt i=0; i<8; i++)
        {
        aSatInfo.AppendSatelliteData(data);
        }
    }


void CIntGpsPsy1::StartTimerIfNeeded()
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
            if(iRequestStatus!=NULL)
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

void CIntGpsPsy1::CompleteRequestByDefault()
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

void CIntGpsPsy1::CompleteRequest(TInt aCompletionCode)
    {
    if(iRequestStatus)
        {
        iPositionInfoBase->SetModuleId(iUid);
        User::RequestComplete(iRequestStatus, aCompletionCode);
        }
    }

void CIntGpsPsy1::CancelNotifyPositionUpdate()
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


TBool CIntGpsPsy1::TrackingOverridden() const
{
    return ETrue;
}

void CIntGpsPsy1::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval*/)
{
}

void CIntGpsPsy1::StopTracking()
{
}

// End of file

