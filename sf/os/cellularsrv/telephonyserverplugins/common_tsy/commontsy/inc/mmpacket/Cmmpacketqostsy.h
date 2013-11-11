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



#ifndef CMMPACKETQOSTSY_H
#define CMMPACKETQOSTSY_H

// INCLUDES
#include <e32base.h>
#include <et_phone.h>
#include <etelpckt.h>
#include <etelqos.h>
#include "cmmphonetsy.h"
#include "CMmPrivateUtility.h"
#include "cmmtsyreqhandlestore.h"

// FORWARD DECLARATIONS
class CMmTsyReqHandleStore;
class CMmPacketContextTsy;
class CMmPacketQoSGsmWcdmaExt;


// CLASS DECLARATION

/**
*  CMmPacketQoSTsy contains GPRS QoS related functionality. 
*/
NONSHARABLE_CLASS( CMmPacketQoSTsy ) : public CSubSessionExtBase
    {
    
  	public: // Constructor and destructor 

        /**
         * NewL method is used to create a new instance of class
         * @param aMmPacketService pointer to PacketService
         * @param aMmPacketContext pointer to PacketContext
         */
        static CMmPacketQoSTsy* NewL(CMmPacketServiceTsy* aMmPacketService, 
                                    CMmPacketContextTsy* aMmPacketContext ); 
    
        /**
         * Destructor
         */
        ~CMmPacketQoSTsy();

        /**
         * Return pointer to the PacketQoSGsmWcdmaExt
         *          
         *
         */
        virtual CMmPacketQoSGsmWcdmaExt* PacketQoSGsmWcdmaExt();

        /**
         * Complete Notify Profile Changed
         *          
         *
         */
        virtual void CompleteNotifyProfileChanged();

        /**
         * Complete Set Profile Parameters
         *          
         *
         * @param aError Completion value
         */
        virtual void CompleteSetProfileParameters
            (
            const TInt aError 
            );

        /**
         * Return a pointer to CMmPacketContextTsy
         *          
         *
         */
        virtual CMmPacketContextTsy* PacketContextTsy();

        /** 
         * Handles extended client requests 
         *          
         *
         * @param TTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage  Data Package contains request related data
         * @return error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
                    const TInt aIpc, const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number
         *          
         *
         * @param aIpc IPC number of request
         * @return CTelObject::TReqMode reguest mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC
         *          
         *
         * @param aIpc IPC number of request
         * @return number of slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in         
         * parameters
         *          
         *
         * @param aIpc IPC number of request
         * @param aTsyReqHandle Request handle of given request 
         * @return error value
         */
        virtual TInt CancelService( const TInt aIpc, 
                            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification
         *          
         *
         * @param aIpc IPC number of request
         * @return error value
         */ 
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         *
         * @param aIpc IPC number of request
         * @return error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );
    
        /**
         * Creates new object and returns a pointer to it
         *          
         *
         * @param aNewName a name of the object to be created
         * @return CTelObject* allways NULL
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Returns a pointer to object if it exists
         *          
         *
         * @param aName A name of the object to be created,
         *                not used 
         * @return CTelObject* allways NULL
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Initialisation method that is called from ETel Server.
         *          
         *
         */
        virtual void Init();


#ifdef REQHANDLE_TIMER

        /**
         * Calls the needed complete method due timer expiration
         *          
         *
         */
        void Complete
            (
            const TInt aReqHandleType,
            const TInt aError
            );

#endif //REQHANDLE_TIMER

#ifdef TF_LOGGING_ENABLED

        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aError error value
         */
        virtual void ReqCompleted
            (
            const TTsyReqHandle aTsyReqHandle,
            const TInt aError
            );

#endif //TF_LOGGING_ENABLED

      
    private:

        /** 
         * C++ default constructor
         */
        CMmPacketQoSTsy();
    
        /**
         * Class attributes are created in ConstructL.
         */
        void ConstructL();

        /**
         * Handles extended client requests.
         * 
         *                  
         * @param aTsyReqHandle Request handle of given request 
         * @param aIpc IPC number of request
         * @param aPackage datapackage contains request related data
         * @return error value
         */
        TInt DoExtFuncL(const TTsyReqHandle aTsyReqHandle, const TInt aIpc, 
                            const TDataPackage& aPackage);

        /**
         * Initialises extension modules
         *          
         *
         */
        void InitExtensionModulesL();

        /**
         * Get Profile Capabilities
         *          
         *
         * @param aTsyReqHandle Request handle of given request
         * @param aProfileCaps Profile capabilities
         * @return error value
         */
        TInt GetProfileCapabilities( const TTsyReqHandle aTsyReqHandle,
                            TDes8* aProfileCaps );
    
        /**
         * Get Profile Parameters
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aProfile Pointer to data
         * @return error value 
         */
        TInt GetProfileParameters( const TTsyReqHandle aTsyReqHandle,
                                    TDes8* aProfile );

        /**
         * Set Profile Parameters
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aProfile Pointer to data
         * @return error value
         */
        TInt SetProfileParametersL( const TTsyReqHandle aTsyReqHandle,
                            TDes8* aProfile );

        /**
         * Notify Profile Changed
         *          
         *
         * @param aProfile Pointer to data
         * @return error value
         */
        TInt NotifyProfileChanged( TDes8* aProfile );

#ifdef REQHANDLE_TIMER

        /**
         * Chooses the type of response, automatic or common
         *          
         *
         */
        void SetTypeOfResponse( const TInt aReqHandleType,
                                const TTsyReqHandle aTsyReqHandle );

#endif //REQHANDLE_TIMER

    public: //Data
    
        enum TPacketQoSRequestTypes
            {
            EMultimodePacketQoSReqHandleUnknown,
            EMultimodePacketQoSNotifyParameterChanged,
            EMultimodePacketQoSSetProfileParameters,

            // Max number of requests
            EMaxNumOfPacketQoSRequests
            };
           
    private: //Data

        /**
         * QoS Request Handle Type
         */
        TPacketQoSRequestTypes                      iReqHandleType;

        /**
         * A pointer to the tsy req handle store
         * Own.         
         */
        CMmTsyReqHandleStore*                       iTsyReqHandleStore;  

        /**
         *  A table for packet context request handles
         */
        TTsyReqHandle       iPacketQoSReqHandles[EMaxNumOfPacketQoSRequests];

        /**
         *  A pointer to the Packet Service TSY
         *  Not own.         
         */
        //
        CMmPacketServiceTsy*                        iMmPacketService;

        /**
         * A pointer to the Packet Context
         * Not own.         
         */
        CMmPacketContextTsy*                        iMmPacketContext;

        /**
         * A pointer to the PacketQoSGsmWcdmaExt
         * Own.         
         */        
        CMmPacketQoSGsmWcdmaExt*                    iMmPacketQoSGsmWcdmaExt;

        };

#endif // CMMPACKETQOSTSY_H

            
// End of File
