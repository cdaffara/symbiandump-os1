/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMSIMTSY_H
#define CMMSIMTSY_H

//  INCLUDES
#include "CMmCustomTsy.h"
#include "cmmtsyreqhandlestore.h"
#include "CMmSubTsyBase.h"

// CONSTANTS
const TInt KNoCbIds = -1;

// From Viag HomeZone specification
const TInt KPhEngMaxViagHomeZones = 4;
// From Viag HomeZone specification
const TInt KPhEngMaxCacheId = 21;

// CLASS DECLARATION

/**
*  CMmSIMTsy contains mode-independent SIM functionality.
*  Extension request handles and parameters are stored as attributes.
*/
NONSHARABLE_CLASS( CMmSIMTsy ) : public CMmSubTsyBase
    {
    public:

    // TSIMRequestType enumerates indexes to SIM's request handle
    // table. Request handles are stored there while waiting for
    // completion of the request.
    enum TSIMRequestType
        {
        ESIMTsyReqHandleUnknown,
        ESIMRequestTypeReadParams,
        ESIMRequestTypeReadCache,
        ESIMRequestTypeWriteCache,
        ESIMRequestTypeWriteSettings,
        ESIMRequestTypeStartSimCbTopicBrowsing,
        ESIMRequestTypeGetNextSimCbTopic,
        ESIMRequestTypeDeleteSimCbTopic,

        // Declare constant for those requests that need own request handle 
        // record in iTsyReqHandleStore above the following
        ESIMRequestTypeMaxNumOfRequests,

        // Let the next literal be the last
        ESIMRequestTypeUndefined = 0xff
        };

    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmSIMTsy();

        /**
         * Two-phased constructor.
         */
        static CMmSIMTsy* NewL( CMmCustomTsy* mmCustomTsy );

        /**
         * Destructor.
         */
        ~CMmSIMTsy();

    public:

        /**
         * Initialisation method
         *         
         *
         */
        void Init();

        /**
         * Tells whether the object supports given IPC.
         *         
         *
         * @param aIpc Request's IPC value
         * @return ETrue if object supports given IPC
         */
        TBool SupportingIPC( const TInt aIpc );

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning on
         * memory allocation failure.
         *         
         *
         * @param aTsyReqHandle Request handle
         * @param aIpc Request's IPC value
         * @param aPackage Reference to a data package
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *         
         *
         * @param aIpc An interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *         
         *       
         * @param aIpc An interprocess communication command identifier
         * @return The number of slots reserved for a given IPC
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request of which IPC number and request handle 
         * are given in parameters.
         *         
         *
         * @param aIpc An interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification.
         *         
         *
         * @param aIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         *         
         *
         * @param aIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt DeregisterNotification( const TInt aIpc );

    public:

        /**
         * Sends VIAG homezone parameters read request if requested by client.
         *         
         *
         */
        void CheckViagHomezoneParamsL();

        /**
         * Completes the ReadViagHomeZoneParamsResp response message.
         *         
         *
         * @param aParams Pointer to VIAG Home Zone parameters structure
         * @param aElems Pointer to VIAG Home Zone elements structure
         * @param aError Error value
         */
        void CompleteReadViagHomeZoneParamsResp( 
            RMmCustomAPI::TViagParams* aParams,
            RMmCustomAPI::TViagElements* aElems,
            TInt aError );

        /**
         * Completes the ReadViagHomeZoneCacheResp response message.
         *         
         *
         * @param aViagRecord Pointer to VIAG Home Zone record content
         * @param aError Error value
         */
        void CompleteReadViagHomeZoneCacheRespL(
            RMmCustomAPI::TViagCacheRecordContent* aViagRecord,
            TInt aError );

        /**
         * Completes the WriteViagHomeZoneCacheResp response message.
         *         
         *
         * @param aError Error value
         */
        void CompleteWriteViagHomeZoneCacheResp( TInt aError );

        /**
         * Completes the WriteViagHomeZoneUHZIUESettings response message.
         *         
         * @param aError Error value
         */
        void CompleteWriteViagHomeZoneUHZIUESettingsResp( TInt aError );

        /**
         * Complete StartSimCbTopicBrowsing
         *         
         * @param aTopics CB Topics from SIM
         * @param aError Error value
         */
        void CompleteStartSimCbTopicBrowsing( 
            CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* aTopics,
            TInt aError );

        /**
         * Completes delete SIM CB topic request
         *         
         *
         * @param aError Error value
         */
        void CompleteDeleteSimCbTopic( TInt aError );


#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due the timer expiration
         *         
         *
         * @param aReqHandleType
         * @param aError
         */
        void Complete( TInt aReqHandleType, TInt aError );
#endif //REQHANDLE_TIMER

    private:

        /**
         * By default EPOC constructor is private.
         *         
         *
         * @param aMmCustomTsy
         */
        void ConstructL( CMmCustomTsy* aMmCustomTsy );

        /**
         * Read ViagHomeZoneParams
         *         
         *
         * @param aViagParameters: Pointer to VIAG HomeZone parameters structure
         * @param aViagElements Pointer to VIAG Home Zone elements structure
         * @return Error value
         */
        TInt ReadViagHomeZoneParamsL( RMmCustomAPI::TViagParams* aViagParameters,
            RMmCustomAPI::TViagElement* aViagElements );

        /**
         * Read ViagHomeZoneCache
         *         
         *
         * @param aViagRecordId
         * @param aViagRecordContent
         * @return Error value
         */
        TInt ReadViagHomeZoneCacheL(
            RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
            RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent );

        /**
         * Read Dynamic ViagHomeZoneCache
         *         
         *
         * @param aTsyReqHandle
         * @param aViagRecordId
         * @param aViagRecordContent
         * @return Error value
         */
        TInt ReadDynamicViagHomeZoneCacheL(
            const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
            RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent );

        /**
         * Start dynamic caching
         *         
         *
         */
        void StartDynamicCachingL();

        /**
         * Write ViagHomeZoneCache
         *         
         *
         * @param aViagRecordId Pointer to VIAG Home Zone record id structure
         * @param aViagRecordContent Pointer to VIAG Home Zone record 
         *        content structure
         * @return Error value
         */
        TInt WriteViagHomeZoneCacheL(
            RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
            RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent );

        /**
         * Cancel the ReadViagHomeZoneParams request
         *         
         *
         * @return KErrNone
         */
        TInt ReadViagHomeZoneParamsCancel();

        /**
         * Cancel the ReadViagHomeZoneCache request
         *         
         *
         * @return KErrNone
         */
        TInt ReadViagHomeZoneCacheCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Cancel the WriteViagHomeZoneCache request
         *         
         *
         * @return KErrNone
         */
        TInt WriteViagHomeZoneCacheCancel();

        /**
         * Writes specified VIAG Home Zone UHZIUE settings to SIM.
         *         
         * @param aSettings Pointer to TViagUHZIUESettings
         * @return Error value
         */
        TInt WriteViagHomeZoneUHZIUESettingsL( 
            RMmCustomAPI::TViagUHZIUESettings* aSettings );

        /**
         * Cancels the VIAG Home Zone UHZIUE settings write request.
         *         
         * @param ?arg1 ?description
         * @return Error value
         */
        TInt WriteViagHomeZoneUHZIUESettingsCancel();

        /**
         * Start Sim Cb Topics Browsing
         *         
         *
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt StartSimCbTopicBrowsingL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Get Next Sim Cb Topic
         *         
         *
         * @param aTsyReqHandle Request handle
         * @param aSimCbTopic Pointer to TSimCbTopic structure
         * @return Error value
         */
        TInt GetNextSimCbTopic( TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSimCbTopic* aSimCbTopic );

        /**
         * Delete Sim Cb Topic
         *         
         *
         * @param aTsyReqHandle Request handle
         * @param aSimCbTopicNumber ID number of the sim topic
         * @return Error value
         */
        TInt DeleteSimCbTopicL( const TTsyReqHandle aTsyReqHandle, 
            TUint* aSimCbTopicNumber );

        /**
         * Reset and destroy sim cb topics
         *         
         *
         */
        void ResetAndDestroySimCbTopics();


#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *         
         *
         * @param aReqHandleType
         * @param aTsyReqHandle
         */
        void SetTypeOfResponse( const TInt aReqHandleType,
                                const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

    private:

        /**
         * Pointer to CMmCustomTsy class
         * Not Own.         
         */
        CMmCustomTsy* iMmCustomTsy;
        
        /**
         *Pointer to request handle store
         * Own.         
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         * SIM Custom API request handle table
         * Own.         
         */
        TTsyReqHandle iSIMReqHandles[ESIMRequestTypeMaxNumOfRequests];

        /**
         * Saves temporarily last TSY request type
         * Own.         
         */
        TSIMRequestType iReqHandleType;

        /**
         * Pointer to VIAG Home Zone parameters structure
         * Not Own.         
         */
        RMmCustomAPI::TViagParams* iViagParams;
        
        /**
         * Pointer to VIAG Home Zone elements structure
         * Not Own.         
         */ 
        RMmCustomAPI::TViagElement* iViagElems;

        /**
         * VIAG home zone record content
         * Not Own.         
         */
        RMmCustomAPI::TViagCacheRecordContent* iViagReadCacheRecord;

        /**
         * Is VIAG home zone params requested before SIM is ready
         */
        TBool iViagParamsReadRequested;
        
        /**
         * used for handling SIM CB Topic browsing:
         * a cached table of all entries         
         * Own.         
         */
        CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* iSimCbTopics;

        /**
         * The index of the next topic to retrieve with the GetNext-method         
         */                
        TInt iGetNextSimCbTopicIndex;

        /**
         * The index of the topic to be deleted
         */        
        TInt iSimCbTopicIndexToBeDeleted;

        /**
         * The Dynamic cache for VIAG home zone records
         * Own.         
         */        
        CArrayFixFlat<RMmCustomAPI::TViagCacheRecordContent>* iViagDynamicCache;

        /**
         * Pointer to current cache
         * Not Own.         
         */        
        RMmCustomAPI::TViagCacheRecordId* iCurrentlyRetrievedCache;
        
        /**
         * Is ViagHomeZone cache ready
         */
        TBool iViagHomeZoneCacheReady;

        /**
         * last ViagHomeZone cache error
         */
        TInt iLastViagHomeZoneCacheError;
        
        /** 
         * Used for queuing ReadViagHomeZoneCache requests 
         */
        struct TReadViagHomeZoneCacheRequest
			   {
			   /** ETel request handle */
			   TTsyReqHandle iReqHandle; 
			   /** pointer to client-side	data */
			   RMmCustomAPI::TViagCacheRecordId* iViagRecordId;
               RMmCustomAPI::TViagCacheRecordContent* iViagRecordContent;
			   };
        
        /**
		 * Array for buffering ReadViagHomeZoneCache requests
		 */		     
		    RPointerArray< TReadViagHomeZoneCacheRequest > 
            iReadViagHomeZoneCacheRequests;
        
        
        
        
    };

#endif      // CMMSIMTSY_H

//  End of File
