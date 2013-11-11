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
* Description: Declaration of CLbsLocMonitorConversionHandler class.
*
*/

#ifndef CLBSLOCMONITORCONVERSIONHANDLER_H_
#define CLBSLOCMONITORCONVERSIONHANDLER_H_

#include <e32base.h>
#include <lbs/lbslocinfoconverterpluginbase.h>
#include <lbs/lbslocationinfo.h>
#include "mlbsconversioncompleteobserver.h"
#include "rlbslocmonitordb.h"

/**
 * This class is responsible for requesting location conversion from 
 * the plug in and later to handle the converted information.
 */
class CLbsLocMonitorConversionHandler : public CActive,
                                        public MLbsLocInfoConverterPluginObserver
    {
public:
    /**
     * Validation info assosciated with each client
     */
    struct TValidationInfo
        {
        TSecureId  iClientSecureId;
        };    /**
     * This represent different state of conversion handler. 
     */
    enum TState
        {
        EStateIdle,
        EStateValidation,
        EStateConversion,
        EStateCacheSave,
        EStateCacheSaveComplete
        };
    
public:
    /**
     * Creates new instance of CLbsLocMonitorConversionHandler object. 
     */ 
    static CLbsLocMonitorConversionHandler* NewL( 
                               MLbsConversionCompleteObserver& 
                               aConversionCompleteObserver,
                               TUid aConversionPluginUid );
    
    /**
     * Destructor
     */
    ~CLbsLocMonitorConversionHandler();
    
    /**
     * Returns the size of converted location information to the client.
     */
    void GetConvertLocationInfoSizeL(const RMessage2& aMessage);
    
    /**
     * Returns the converted location information to the client.
     */
    void GetConvertedLocationInfoL(const RMessage2& aMessage);
    
    /**
     * Cancel location info conversion.
     */
    void CancelLocationInfoConversionL(const RMessage2& aMessage);
    
    /**
     * Returns Uid of conversion plugin that is loaded.
     */
    TUid ConversionPluginUid();
    
    /**
     * Returns ETrue is any message corresponding to subsession id 
     * is present in the handler.
     */
    TBool IsMessagePresent(const RMessage2& aMessage);
    
    /**
     * Returns ETrue if secure id and subsession id matches else EFalse.
     */
    TBool CompareMessage(const RMessage2& aSrc,const RMessage2& aDes);
    
    /**
     * Returns accuracy level from a cache match, or nan if undefined.
     */
    TReal32 AccuracyFromMatchLevel(TPositionAreaExtendedInfo::TArea aMatchlevel);
    
   
    
public: // from MLbsLocInfoConverterPluginObserver
    virtual void OnConversionComplete(TInt aStatusCode);
    
    virtual void OnValidationComplete(TInt aStatusCode);
    
protected: // from CActive
    void RunL();
    void DoCancel();
    
private:
    /**
     * Constructor.
     */
    CLbsLocMonitorConversionHandler(MLbsConversionCompleteObserver& 
                                     aConversionCompleteObserver);
    
    /**
     * Symbian 2 phase constructor.
     */
    void ConstructL(TUid aConversionPluginUid);
    
    /**
     * Processes the request which is recently validated.
     */
    TInt ProcessRequestL();
    
    /**
     * Loads the conversion module specified by client.
     */
    void LoadConversionPluginL(TUid aConversionModuleId);
    
    /**
     * Writes converted position information to the stream.
     */
    void WriteAreaInfoL(RBufWriteStream& aWriteStream,
                         CLbsLocationInfo::TAreaInfoMask aAreaInfoMask);

    /**
     * Completes client's message.
     */
    void CompleteRequest(const RMessage2& aMessage,TInt aReason);
    
    /**
     * This is called after each conversion request complete
     */
    void RequestCompleteL();
    
    
    void WriteResultToBufferL();
    
    
    /**
     * Fills in partial results from cache where they are better than those returned from the plugin
     */
    void FillInPartialL();
    
    /**
     * Sends requests from iRequestedInfo to the cacne and uses the result to
     * fill in iCacheLookupResults, the RunL will decide which responses to 
     * write back into iRequestedInfo and send back to the client
     */
    TBool CacheLookupL();
    
    /*
     * Writes any cellid + location pairs received as part of a query back into 
     * the cache 
     */
    TBool CacheSaveL();
    
private: // Member variables
    /**
     * Handle to conversion complete observer
     */
    MLbsConversionCompleteObserver& iConversionCompleteObserver;
    
    /**
     * Conversion request.
     */
    RMessage2 iConversionRequest;
     
    /**
     * Validation request array.
     */
    RArray<RMessage2> iValidationRequestQueue;
     
    /**
     * Pointer to conversion plugin
     */
    CLbsLocInfoConverterPluginBase* iLocInfoConverterPlugIn;
    
    /**
     * Handle to buffer that contains converted information.
     */
    CBufFlat* iBuffer;
    
    /**
     * Client specified request info
     */
    TLbsConversionOutputInfoMask iRequestedInfo;
    
    /**
     * Location information array passed from client.
     */
    RLbsLocationInfoArray  iLocationInfoArray;
    
    
    /**
     * A subset of location information array passed from client.
     * requests that have been solved by the cache are ommitted
     * this array DOES NOT OWN its contents and should not destroy them.
     * as the ownership remains with iLocationInfoArray
     */
    RLbsLocationInfoArray  iSubsetLocationInfoArray;
    
    /**
     * Uid of currently loaded conversion plug in
     */
    TUid iConversionPluginId;
    
    /**
     * State of conversion handler.
     */
    TState iState;
    
    /**
     * Secure id of client that has outstanding request
     */
    TSecureId iCurrentRequestSecureId;
    
    /**
     * Subsession id of client that has outstanding request
     */
    TInt iCurrentRequestSubsessionId;

    
    /*
     * Cache lookup results
     */
    RArray<TLocality> iCachePartialResults;
    
    
    /**
     * Cache for lookups
     */
    RLbsLocMonitorDb iCache;
    
    
    TInt iSavedToCacheCount;
    
    };

#endif /*CLBSLOCMONITORCONVERSIONHANDLER_H_*/
