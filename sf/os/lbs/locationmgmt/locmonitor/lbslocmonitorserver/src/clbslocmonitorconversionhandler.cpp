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
* Description: Implementation of CLbsLocMonitorConversionHandler class.
*
*/

#include <s32mem.h>
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <lbs/lbslocinfoconverterpluginbase.hrh>
#include "clbslocmonitorconversionhandler.h"
#include "lbslocmonitorclientconsts.h"
#include "lbslocmonitorserverdata.h"
#include "lbsdevloggermacros.h"



// constants
const TInt KCurrentRequestIndex = 0;
const TInt KMaxBufferSize = 512;

//TODO need to agree which core lbs header to put them in and names
const TReal32 KWlanAccuracy = 100.0;
const TReal32 KCellFullAccuracy = 3000.0;
const TReal32 KCellLocationAccuracy = 30000.0;
const TReal32 KCellCountryAccuracy = 1000000.0;



void CleanUpResetAndDestroy(TAny* aArray)
    {
    static_cast<RLbsAreaInfoBaseArray*>(aArray)->ResetAndDestroy();
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocMonitorConversionHandler* CLbsLocMonitorConversionHandler::NewL(
                                                MLbsConversionCompleteObserver& 
                                                aConversionCompleteObserver,
                                                TUid aConversionPluginUid)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::NewL()");
    CLbsLocMonitorConversionHandler* self = new(ELeave)
                                  CLbsLocMonitorConversionHandler( 
                                  aConversionCompleteObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aConversionPluginUid);
    CleanupStack::Pop(self);
    return self;
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CLbsLocMonitorConversionHandler
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocMonitorConversionHandler::CLbsLocMonitorConversionHandler(
                                                MLbsConversionCompleteObserver& 
                                                aConversionCompleteObserver):
                                                CActive(EPriorityStandard),
                                                iConversionCompleteObserver(
                                                aConversionCompleteObserver)
                                                 
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::CLbsLocMonitorConversionHandler()");
    CActiveScheduler::Add(this);
    iBuffer = NULL;
    iLocInfoConverterPlugIn = NULL;
    iConversionPluginId = TUid::Uid(KNullUidValue);
    iState = EStateIdle;
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::~CLbsLocMonitorConversionHandler
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocMonitorConversionHandler::~CLbsLocMonitorConversionHandler()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::~CLbsLocMonitorConversionHandler()");
    
    iCachePartialResults.Reset();
    iSubsetLocationInfoArray.Reset(); // NOT OWNED!
    iLocationInfoArray.ResetAndDestroy();
    
    iValidationRequestQueue.Close();
    
    delete iLocInfoConverterPlugIn;
    delete iBuffer;
    iCache.Close();
    // NewL of plugin internally uses REComSession to find implementations. 
    // Hence it needs to be closed to avoid memory leak.
    REComSession::FinalClose();
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::ConstructL(TUid aConversionPluginUid)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::ConstructL()");
    LoadConversionPluginL(aConversionPluginUid);
    iCache.OpenL();
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::GetConvertLocationInfoSizeL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::GetConvertLocationInfoSizeL( 
                                             const RMessage2& aMessage) 
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::GetConvertLocationInfoSizeL()");
    iValidationRequestQueue.AppendL(aMessage);
    
    // If the message which is appended is the only message in the queue,
    // send the necessary information of the message to the plug in for
    // validation.
    if(iValidationRequestQueue.Count() == 1)
        {
        iState = EStateValidation;
        // Extract client preferences to get the requested info.
        TLbsClientPrefs clientPrefs; 
        TPckg<TLbsClientPrefs> clientPrefsPckg(clientPrefs);
        aMessage.ReadL(KParamClientPrefs,clientPrefsPckg);
        
        iLocInfoConverterPlugIn->ValidateClient(aMessage.SecureId(),
                                                 aMessage.VendorId(),
                                                 clientPrefs.iRequestedInfo);
        }
    }
        
//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::ProcessRequestL
// (other items were commented in a header).
//----------------------------------------------------------------
TInt CLbsLocMonitorConversionHandler::ProcessRequestL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::ProcessRequestL()");
    // Reset the member variables.
    iLocationInfoArray.ResetAndDestroy();
    delete iBuffer;
    iBuffer = NULL;
    iState = EStateIdle;
    
    // Store secure id and subsession id of the request. This is required 
    // to identify the next IPC that requests for actual data.
    iCurrentRequestSecureId = iConversionRequest.SecureId();
    iCurrentRequestSubsessionId = iConversionRequest.Int3();
    
    // Extract client preferences
    TLbsClientPrefs clientPrefs; 
    TPckg<TLbsClientPrefs> clientPrefsPckg(clientPrefs);
    iConversionRequest.ReadL(KParamClientPrefs,clientPrefsPckg);
    
    // Store the client requested info.This is required to extract the 
    // required information on conversion complete.
    iRequestedInfo = clientPrefs.iRequestedInfo;
    
    // Extract the client specified location information
    TInt dataSize = iConversionRequest.GetDesLength(KParamLocationInfo);
    if (dataSize < 0)
        {
        CompleteRequest(iConversionRequest,KErrBadDescriptor);
        return KErrBadDescriptor;
        }
    HBufC8* buffer = HBufC8::NewLC(dataSize);
    TPtr8 ptrToBuf = buffer->Des();
    iConversionRequest.ReadL(KParamLocationInfo, ptrToBuf);
    
    RDesReadStream stream(*buffer);
    CleanupClosePushL(stream);
    
    TInt count;
    // Read the number of location info object only incase of multiple conversion
    if(iConversionRequest.Function() == EConvertMultipleLocationInfoBufferSize) 
        {
        count = stream.ReadInt8L();
        }
    else
        {
        // Incase of single location info conversion, count value is always one.
        count = 1;
        }
    
    for(TInt i = 0; i<count; i++)
        {
        CLbsLocationInfo* locationInfo = CLbsLocationInfo::NewL();
        CleanupStack::PushL(locationInfo);
        locationInfo->InternaliseL(stream);
        CleanupStack::Pop(locationInfo);
        iLocationInfoArray.AppendL(locationInfo);
        }
    
    CleanupStack::PopAndDestroy(2); // stream,buffer
    
    iState = EStateConversion;
    // Ask Cache,
    // take different action depending on whether it has results for all our requests
    if(CacheLookupL())
        {
        // Cache has supplied all positions. We can self complete now to pass result back to client
        iState = EStateCacheSaveComplete;
        }
    else
        {
        // Cache did not have all the positions we wanted, must go to plugin
        iLocInfoConverterPlugIn->ConvertLocationInfoL(iSubsetLocationInfoArray,
                                                      clientPrefs.iConversionPrefs,
                                                      clientPrefs.iRequestedInfo);
        }
    return KErrNone;
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::LoadConversionPluginL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::LoadConversionPluginL(
                                                    TUid aConversionModuleId)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::LoadConversionPluginL()");
    TLbsLocInfoConverterPluginParams conversionPluginParam(*this);
    
    iLocInfoConverterPlugIn = CLbsLocInfoConverterPluginBase::NewL(
                                aConversionModuleId,conversionPluginParam);
    iConversionPluginId = aConversionModuleId;
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::GetConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::GetConvertedLocationInfoL( 
                                               const RMessage2& aMessage)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::GetConvertedLocationInfoL()");
    TPtr8 ptr = iBuffer->Ptr(0);
    aMessage.WriteL(KParamConvertedInfo,ptr);
    aMessage.Complete(KErrNone);
    
    // This is the last step of a any location conversion. Hence request
    // complete call needs to be called to process next request if any.
    RequestCompleteL();
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::IsMessagePresent
// (other items were commented in a header).
//----------------------------------------------------------------
TBool CLbsLocMonitorConversionHandler::IsMessagePresent( 
                                           const RMessage2& aMessage)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::IsMessagePresent()");
    // Iterate through the validation request array to find the message.
    // First element in the validation queue will the current request 
    // under conversion.
    for(TInt i=0;i<iValidationRequestQueue.Count();i++)
        {
        if(CompareMessage(iValidationRequestQueue[i],aMessage)) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CancelLocationInfoConversionL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::CancelLocationInfoConversionL(
                                          const RMessage2& aMessage)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::CancelLocationInfoConversionL()");
    // Iterate through the queue to find the corresponding message 
    // on which cancelled is called.
    // If the cancel is called on the request under process, that 
    // corresponding message should be cancelled else that message
    // should be removed from the queue.
    for(TInt i=0;i<iValidationRequestQueue.Count();i++)
        {
        if(CompareMessage(iValidationRequestQueue[i],aMessage))
            {
            // If the current request has been sent to validation or conversion, 
            // we need to cancel it 
            if(i == KCurrentRequestIndex)
                {
                // Cancel should be called on plugin based on current request.
                switch(iState)
                    {
                    case EStateValidation:
                        iLocInfoConverterPlugIn->CancelValidateClient();
                        break;
                    
                    case EStateConversion:
                        iLocInfoConverterPlugIn->CancelConvertLocationInfo();
                        break;
                     
                    default:
                        break;
                    }
                CompleteRequest(iValidationRequestQueue[i],KErrCancel);
                Cancel();
                RequestCompleteL();
                }
            else
                {
                RMessage2 message = iValidationRequestQueue[i];
                iValidationRequestQueue.Remove(i);
                message.Complete(KErrCancel);
                }
            aMessage.Complete(KErrNone);
            return;
            }
        }
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::ConversionPluginUid
// (other items were commented in a header).
//----------------------------------------------------------------
TUid CLbsLocMonitorConversionHandler::ConversionPluginUid()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::ConversionPluginUid()");
    return iConversionPluginId;
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::OnConversionComplete
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::OnConversionComplete(TInt aStatusCode)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::OnConversionComplete()");
    // This is done to release the Plug-in's method from which this method 
    // has been called.
    TRequestStatus* statusPtr = &iStatus;
    iStatus = KRequestPending;
    SetActive();
    User::RequestComplete(statusPtr,aStatusCode);
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::OnValidationComplete
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::OnValidationComplete(TInt aStatusCode)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::OnValidationComplete()");
    // This is done to release the Plug-in's method from which this method 
    // has been called.
    TRequestStatus* statusPtr = &iStatus;
    iStatus = KRequestPending;
    SetActive();
    User::RequestComplete(statusPtr,aStatusCode);
    }


void CLbsLocMonitorConversionHandler::WriteResultToBufferL()
    {
    iBuffer = CBufFlat::NewL(KMaxBufferSize);
    RBufWriteStream writeStream;
    writeStream.Open(*iBuffer);
    CleanupClosePushL(writeStream);
    
    // Write the count only if multiple conversion was requested by client.
    if(iConversionRequest.Function() == EConvertMultipleLocationInfoBufferSize)
        {
        writeStream.WriteInt8L(iLocationInfoArray.Count());
        }
    
    switch(iRequestedInfo)
        {
        case ELbsConversionOutputPosition:
            WriteAreaInfoL(writeStream,CLbsLocationInfo::ELbsPosInfo);
            break;
            
        case ELbsConversionOutputGsm:
            WriteAreaInfoL(writeStream,CLbsLocationInfo::ELbsGsmInfo);
            break;
            
        case ELbsConversionOutputWcdma:
            WriteAreaInfoL(writeStream,CLbsLocationInfo::ELbsWcdmaInfo);
            break;
            
        case ELbsConversionOutputWlan:
            WriteAreaInfoL(writeStream,CLbsLocationInfo::ELbsWlanInfo);
            break;
        
        default:
            break;
        }
    
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::RunL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::RunL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::RunL()");
    TBool cacheSaveComplete = EFalse;
    switch(iState)
        {
        case EStateValidation:
            {
            // This validation result corresponds to the first request in the validation 
            // queue.
            iConversionRequest = iValidationRequestQueue[KCurrentRequestIndex];
            if(iStatus.Int() == KErrNone)
                {
                // On successful validation, process the request
                ProcessRequestL();
                }
            else
                {
                iConversionRequest.Complete(iStatus.Int());
                RequestCompleteL();
                }
            break;
            }
        case EStateConversion:
            {
            if(iStatus.Int() == KErrNone)
                {
                //First we loop through comparing accuracies to any partial results we may have had
                FillInPartialL();
                
                TLbsConversionPluginInfo pluginPreferences; 
                iLocInfoConverterPlugIn->ConversionPluginPreferences(pluginPreferences);
                
                // Then save all things we are allowed to, to the cache
                if(pluginPreferences.IsPluginPositionToBeCached())
                    {
                    iSavedToCacheCount = 0;
                    cacheSaveComplete = CacheSaveL();
                    if(cacheSaveComplete)
                        {
                        iState = EStateCacheSaveComplete;
                        }
                    }
                else
                    {
                    WriteResultToBufferL();
                    
                    // Send the required buffer size to the client.
                    TInt size = iBuffer->Size();
                    TPckg<TInt> sizePckg(size);
                    iConversionRequest.WriteL(KParamBufferSize,sizePckg);
                    CompleteRequest(iConversionRequest,KErrNone);
                    }
                }
            else 
                {
                // Since there is error in conversion, server dont receive  
                // any call to retrieve converted info.Hence we need to 
                // remove the current request.
                CompleteRequest(iConversionRequest,iStatus.Int());
                RequestCompleteL();
                }
            break;
            }
        case EStateCacheSave:
            {
            cacheSaveComplete = CacheSaveL();
            if(cacheSaveComplete)
                {
                iState = EStateCacheSaveComplete;
                }
            break;
            }
        case EStateCacheSaveComplete:
            {
            // fall through
            break;
            }
        default:
            {
            User::Invariant();
            }
        }
    if(iState == EStateCacheSaveComplete)
        {
        WriteResultToBufferL();
        
        // Send the required buffer size to the client.
        TInt size = iBuffer->Size();
        TPckg<TInt> sizePckg(size);
        iConversionRequest.WriteL(KParamBufferSize,sizePckg);
        CompleteRequest(iConversionRequest,KErrNone);
        }
    }


//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::RequestCompleteL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::RequestCompleteL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::RequestCompleteL()");
    // Remove the completed conversion request.
    iValidationRequestQueue.Remove(KCurrentRequestIndex);
    
    // Process the next the request in the queue incase any.
    if(iValidationRequestQueue.Count())
        {
        iState = EStateValidation;
        // Extract client preferences to get the requested info.
        TLbsClientPrefs clientPrefs; 
        TPckg<TLbsClientPrefs> clientPrefsPckg(clientPrefs);
        iValidationRequestQueue[KCurrentRequestIndex].ReadL(KParamClientPrefs,clientPrefsPckg);
        
        iLocInfoConverterPlugIn->ValidateClient( 
                    iValidationRequestQueue[KCurrentRequestIndex].SecureId(),
                    iValidationRequestQueue[KCurrentRequestIndex].VendorId(),
                    clientPrefs.iRequestedInfo);
        }
    else
        {
        iConversionCompleteObserver.HandleConversionComplete(this);
        }
    }


//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::DoCancel
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::DoCancel()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::DoCancel()");
    // Nothing to do here.
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::WriteAreaInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::WriteAreaInfoL(RBufWriteStream& 
                                                      aWriteStream,
                                                      CLbsLocationInfo::TAreaInfoMask aAreaInfoMask)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::WriteAreaInfoL()");
    for(TInt i=0;i<iLocationInfoArray.Count();i++)
        {
        RLbsAreaInfoBaseArray areaInfoArray;    
       
    	CleanupStack::PushL(TCleanupItem(*CleanUpResetAndDestroy, &areaInfoArray));

        iLocationInfoArray[i]->GetAreaInfoL(areaInfoArray,aAreaInfoMask);
        
        // Write the count of converted information for each location info object.
        TInt count = areaInfoArray.Count();
        aWriteStream.WriteInt8L(count);
        
        for(TInt j=0;j<count;j++)
            {
            areaInfoArray[j]->ExternaliseL(aWriteStream);
            }
        CleanupStack::PopAndDestroy(&areaInfoArray);
        }
    }

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CompleteRequest
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocMonitorConversionHandler::CompleteRequest(const RMessage2& aMessage,
                                                       TInt aReason)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::CompleteRequest()");
    TInt pos = iValidationRequestQueue.Find(aMessage);
    if(!aMessage.IsNull())
        {
        aMessage.Complete(aReason);
        iValidationRequestQueue[pos] = aMessage;
        }
    }


//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CompareMessage
// (other items were commented in a header).
//----------------------------------------------------------------
TBool CLbsLocMonitorConversionHandler::CompareMessage(const RMessage2& aSrc,
                                                       const RMessage2& aDes)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorConversionHandler::CompareMessage()");
    // If the src message is NULL,this is the message corresponding to the
    // first request for which it is completed with size of converted info.
    // Hence it needs to checked aganist current request's secure id and the
    // subsession id.
    if(aSrc.IsNull())
        {
        if(aDes.SecureId() == iCurrentRequestSecureId &&
            aDes.Int3() == iCurrentRequestSubsessionId)
            {
            return ETrue;
            }
        return EFalse;
        }
    
    // Subsession id is stored in the 4th IPC slot.
    if(aSrc.SecureId() == aDes.SecureId() && 
        aSrc.Int3() == aDes.Int3())
        {
        return ETrue;
        }
    return EFalse;
    }


//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::AccuracyFromMatchLevel
// (other items were commented in a header).
//----------------------------------------------------------------
TReal32 CLbsLocMonitorConversionHandler::AccuracyFromMatchLevel(TPositionAreaExtendedInfo::TArea aMatchlevel)
    {
    TRealX outputAccuracy;
    outputAccuracy.SetNaN();
    switch(aMatchlevel)
        {
        case TPositionAreaExtendedInfo::EAreaUnknown:
            {
            break;
            }
        case TPositionAreaExtendedInfo::EAreaCountry:
            {
            outputAccuracy.Set(KCellCountryAccuracy);
            break;
            }
        case TPositionAreaExtendedInfo::EAreaRegion:
            {
            outputAccuracy.Set(KCellLocationAccuracy);
            break;
            }
        case TPositionAreaExtendedInfo::EAreaCity:
            {
            outputAccuracy.Set(KCellFullAccuracy);
            break;
            }
        case TPositionAreaExtendedInfo::EAreaDistrict:
            {
            outputAccuracy.Set(KWlanAccuracy);
            break;
            }
        case TPositionAreaExtendedInfo::EAreaStreet:
            {
            outputAccuracy.Set(KWlanAccuracy);
            break;
            }
        default:
            {
            break;
            }
        }
    return outputAccuracy;
    }



void CLbsLocMonitorConversionHandler::FillInPartialL()
    {
    TInt infoCount = iSubsetLocationInfoArray.Count();
    TInt partialCount = iCachePartialResults.Count();
    if((infoCount != partialCount) || infoCount == 0)
        return;
    
    for(TInt i = 0; i < infoCount; i++)
        {
        CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*>(iSubsetLocationInfoArray[i]);
        
        RLbsAreaInfoBaseArray locationArray;
        CleanupStack::PushL(TCleanupItem(*CleanUpResetAndDestroy, &locationArray));
        locationInfo->GetAreaInfoL(locationArray, CLbsLocationInfo::ELbsPosInfo);
        
        if(locationArray.Count() > 0)
            {
            CLbsPositionInfo* location = static_cast<CLbsPositionInfo*>(locationArray[0]);
            TLocality pluginLocality;
            location->GetLocality(pluginLocality);
            if(!Math::IsNaN(iCachePartialResults[i].HorizontalAccuracy()) && 
                    (iCachePartialResults[i].HorizontalAccuracy() < pluginLocality.HorizontalAccuracy()))
                {
                locationInfo->ResetAreaInfo(CLbsLocationInfo::ELbsPosInfo);
                CLbsPositionInfo* newLocation = CLbsPositionInfo::NewL(iCachePartialResults[i]);
                locationInfo->AddAreaInfoL(newLocation);
                }
            }
        CleanupStack::PopAndDestroy(&locationArray);
        }
    }



//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CacheLookupL
// (other items were commented in a header).
//----------------------------------------------------------------
TBool CLbsLocMonitorConversionHandler::CacheLookupL()
    {
    iCachePartialResults.Reset();
    // Does NOT own contents
    iSubsetLocationInfoArray.Reset();
    TInt infoCount = iLocationInfoArray.Count();
    // We only do lookups if the client wants a position
    if(iRequestedInfo != ELbsConversionOutputPosition)
        {
        // copy all into subset array, the cache has solved nothing
        for(TInt i = 0; i < infoCount; i++)
            {
            iSubsetLocationInfoArray.AppendL(iLocationInfoArray[i]);
            }
        return EFalse;
        }
    
    TInt cacheLoads = 0;
    TBool resolved = EFalse;
    

    for(TInt i=0; i < infoCount; i++)
        {
        CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*>(iLocationInfoArray[i]);
        RLbsAreaInfoBaseArray areaArray;
        CleanupStack::PushL(TCleanupItem(*CleanUpResetAndDestroy, &areaArray));
        // NOTE! When locmonitor is expanded to cache wlan etc. this call must be changed! 
		// Currently we know about GSM and 3g (WCDMA).
        locationInfo->GetAreaInfoL(areaArray, CLbsLocationInfo::ELbsGsmInfo|CLbsLocationInfo::ELbsWcdmaInfo);
        
        TInt areaCount = areaArray.Count();
        TBool saved = EFalse;
        
        TRealX nan;
        nan.SetNaN();
        TLocality partialLocality;
        partialLocality.SetCoordinate(nan, nan, nan);
        partialLocality.SetAccuracy(nan, nan);
        
        for (TInt j = 0; j < areaCount; j++)
            {   
            resolved = EFalse;
            TLbsAreaInfoClassType type = areaArray[j]->Type();
            TLbsLocMonitorAreaInfoGci areaInfoGci;
            switch(type)
                {
                case ELbsAreaGsmCellInfoClass:
                    {
                    CLbsGsmCellInfo* area = static_cast<CLbsGsmCellInfo*>(areaArray[j]);
                    areaInfoGci.iMcc = area->MobileCountryCode();
                    areaInfoGci.iMnc = area->MobileNetworkCode();
                    areaInfoGci.iLac = area->LocationAreaCode();
                    areaInfoGci.iCid = area->CellId();
                    areaInfoGci.iValidity = ETrue;
                    areaInfoGci.iIs3gNetworkMode = EFalse;
                    
                    resolved = ETrue;
                    break;
                    }

                case ELbsAreaWcmdaCellInfoClass :
                    {
                    CLbsWcdmaCellInfo* area = static_cast<CLbsWcdmaCellInfo*>(areaArray[j]);
                    areaInfoGci.iMcc = area->MobileCountryCode();
                    areaInfoGci.iMnc = area->MobileNetworkCode();
                    areaInfoGci.iLac = area->LocalAreaCode();
                    areaInfoGci.iCid = area->UniqueCellId();
                    areaInfoGci.iValidity = ETrue;
                    areaInfoGci.iIs3gNetworkMode = ETrue;
                    
                    resolved = ETrue;
                    break;
                    }

                default:
                    {
                    resolved = EFalse;
                    break;
                    }
                }
            
            if(resolved && !saved)
                {
                TPosition position;
                RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
                outArray.Append(&areaInfoGci);
                TPositionAreaExtendedInfo matchLevel; 
                matchLevel.SetArea(TPositionAreaInfo::EAreaUnknown);
                TRequestStatus status; 
                // Cache gets are fast and not really asynch, so no harm in blocking here
                TInt ret = iCache.GetPosition(position,outArray,matchLevel,status);
                if((ret == KErrNone) || (ret == KErrNotFound))
                    {
                    User::WaitForRequest(status);
                    }
                outArray.Reset();
                
                if((ret == KErrNone) && (matchLevel.Area()  >= TPositionAreaExtendedInfo::EAreaCity))
                    {
                    TLocality locality;
                    locality.SetCoordinate(position.Latitude(), position.Longitude(),
								position.Altitude());
                	TRealX nan;
                	nan.SetNaN();
                    locality.SetAccuracy(AccuracyFromMatchLevel(matchLevel.Area()), nan);
                    
                    CLbsPositionInfo* positionInfo = CLbsPositionInfo::NewL(locality);
                    CleanupStack::PushL(positionInfo);
                    iLocationInfoArray[i]->AddAreaInfoL(positionInfo);
                    CleanupStack::Pop(positionInfo);
                    saved = ETrue;
                    cacheLoads++;
                    }
                else if ((ret == KErrNone) && (matchLevel.Area()  > TPositionAreaExtendedInfo::EAreaUnknown))
                    {
                    // we also store partial matches in case they are better than the plugin response
                    partialLocality.SetCoordinate(position.Latitude(), position.Longitude(),
                                position.Altitude());
                    TRealX nan;
                    nan.SetNaN();
                    partialLocality.SetAccuracy(AccuracyFromMatchLevel(matchLevel.Area()), nan);
                    }

                }
            }
        if(!saved)
            {
            //copy into subset array, we couldn't solve this from cache
            iSubsetLocationInfoArray.AppendL(iLocationInfoArray[i]);
            
            iCachePartialResults.AppendL(partialLocality);
            }
        CleanupStack::PopAndDestroy(&areaArray);
        }
    // If we've loaded as many results from the cache as there are requests 
    // then we have fully satisfied the query
    return cacheLoads == infoCount;
    }
    

//----------------------------------------------------------------
// CLbsLocMonitorConversionHandler::CacheSaveL
// (other items were commented in a header).
//----------------------------------------------------------------
TBool CLbsLocMonitorConversionHandler::CacheSaveL()
    {
    iState = EStateCacheSave;
    TBool saveRequested = EFalse;
    TInt infoCount = iSubsetLocationInfoArray.Count();
    while((iSavedToCacheCount < infoCount) && !saveRequested)
        {
        CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*>(iSubsetLocationInfoArray[iSavedToCacheCount]);
        
        RLbsAreaInfoBaseArray cellArray;
        CleanupStack::PushL(TCleanupItem(*CleanUpResetAndDestroy, &cellArray));
        locationInfo->GetAreaInfoL(cellArray, CLbsLocationInfo::ELbsGsmInfo | CLbsLocationInfo::ELbsWcdmaInfo);
        
        RLbsAreaInfoBaseArray locationArray;
        CleanupStack::PushL(TCleanupItem(*CleanUpResetAndDestroy, &locationArray));
        locationInfo->GetAreaInfoL(locationArray, CLbsLocationInfo::ELbsPosInfo);
        
        
        // We should get only 1 pos back but may have passed in multiple cells
        // We want to save that pos against all cells
        if((locationArray.Count() > 0) && (cellArray.Count() > 0))
            {
            CLbsPositionInfo* location = static_cast<CLbsPositionInfo*>(locationArray[0]);
            TLocality locality;
            location->GetLocality(locality);
            // Only locations that have an accuracy that is better than a cell
            // location code will be saved in the cache
            if(locality.HorizontalAccuracy() < KCellLocationAccuracy)
                {
                for(TInt i = 0; i < cellArray.Count(); i++)
                    {
					TLbsLocMonitorAreaInfoGci areaInfoGci;
					if (cellArray[i]->Type() == ELbsAreaGsmCellInfoClass)
						{
						CLbsGsmCellInfo* cell = static_cast<CLbsGsmCellInfo*>(cellArray[i]);
						areaInfoGci.iMcc = cell->MobileCountryCode();
						areaInfoGci.iMnc = cell->MobileNetworkCode();
						areaInfoGci.iLac = cell->LocationAreaCode();
						areaInfoGci.iCid = cell->CellId();
                        areaInfoGci.iValidity = ETrue;
						areaInfoGci.iIs3gNetworkMode = EFalse;
						}
					else 
						{ 
						// by elimination type must be ELbsAreaWcmdaCellInfoClass!
						CLbsWcdmaCellInfo* cell = static_cast<CLbsWcdmaCellInfo*>(cellArray[i]);

						// If we don't have a valid LAC (which may happen with WCDMA) then we cannot
						// save to the cache and we should go to the next cell immediately.
						if (cell->LocalAreaCode() == -1)
							continue;
						areaInfoGci.iMcc = cell->MobileCountryCode();
						areaInfoGci.iMnc = cell->MobileNetworkCode();
						areaInfoGci.iLac = cell->LocalAreaCode();
						areaInfoGci.iCid = cell->UniqueCellId();
						areaInfoGci.iValidity = ETrue;
						areaInfoGci.iIs3gNetworkMode = ETrue;
						}

                    RPointerArray<TLbsLocMonitorAreaInfoBase> areaArray;
                    areaArray.Append(&areaInfoGci);
                    TTime now;
                    now.HomeTime();
                    TPosition pos(locality, now);
                    TInt ret = iCache.SavePosition(pos, areaArray, EFalse, iStatus);
                    if(ret != KErrNone)
                        {
                        saveRequested = EFalse;
                        }
                    else
                        {
                        saveRequested = ETrue;
                        SetActive();
                        }
                    areaArray.Reset();
                    }            
                }
            }
        CleanupStack::PopAndDestroy(&locationArray);
        CleanupStack::PopAndDestroy(&cellArray);
        iSavedToCacheCount++;
        }
    if((iSavedToCacheCount >= infoCount) && !saveRequested)
        {
        return ETrue;
        }
    return EFalse;
    }


// End of file.
