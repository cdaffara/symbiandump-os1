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



#ifndef CMMPRIVATEUTILITY_H
#define CMMPRIVATEUTILITY_H

// INCLUDES 
#include <et_phone.h>
#include <e32base.h>
#include <ctsy/serviceapi/cmmutility.h>
#include "mmgsmwcdmaprivateutils.h"

//  DATA TYPES  

/** Tsy error values*/
enum TMmTsyError
    {
    EIllegalTsyReqHandle,
    EIllegalPanicOccurredValue,
    EIllegalOwnedState,
    ENotificationsRemaining,
    ELastEventsRemaining,
    ECallsRemaining,
    EPtrNull,
    ERelinquishOwnershipWithNoCall,
    EIllegalTsyNameInConnectedModemRecord,
    EIllegalCallDuration,
    EIllegalEvent,
    EPreAllocatedCallDoesNotExist,
    ENewCallDoesNotExist,
    ECallNameAlreadyExists,
    ERingingCallNotFound,
    ELineAlreadyExists,
    EAcquirerNotFound,
    EDataPortPtrNotValid,
    };

/** Diagnostics for supplementary services*/
enum TDiagnosticInfoValues
    {
    KDiagnosticInfoBarredWithCUG = 0x81,
    KDiagnosticInfoBarredNoCUG = 0x82,
    KDiagnosticInfoBarredUnknownCUG = 0x83,
    KDiagnosticInfoBarredIncompatibleCUG = 0x84,
    KDiagnosticInfoBarredFailureCUG = 0x85,
    KDiagnosticInfoBarredClirNotSubscribed = 0x86,
    KDiagnosticInfoBarredCCBSPossible = 0x87,
    KDiagnosticInfoBarredCCBSNotPossible = 0x88    
    };

//  FUNCTION PROTOTYPES
void MmTsyPanic( TMmTsyError aMmTsyError );
void MmTsyLeaveL( TMmTsyError aMmTsyError );

//  FORWARD DECLARATIONS
class CMmSmsMessHandlerGsmExt;
class CMmSmsTsy;
class CMmPhoneTsy;
class CMmCallTsy;
class CAcquireOwnerList;
class CMmCallExtInterface;
class CMmPacketContextTsy;

// CLASS DECLARATION

/**
 *  Available network info 
 */
NONSHARABLE_CLASS( CNetNetworkAvailableInfo ) : public CBase
    {
    public: // Constructors and destructor
    
        /**
         * C++ default constructor.
         */    
        CNetNetworkAvailableInfo();
        
        /**
         * Destructor.
         */
        ~CNetNetworkAvailableInfo();
        
    public: // Data
    
        /**
         * int to hold network status
         */         
        TUint8 iNetworkStatus;
        
        /**
         * Index of this info object
         */         
        TUint8 iIndex;
        
        /**
         * Network's display name
         */         
        RMobilePhone::TMobilePhoneNetworkDisplayTag iAlphaTag;
        
        /**
         * Variable for network's long name
         */         
        RMobilePhone::TMobilePhoneNetworkLongName iLongName;
        
        /**
         * Operator code
         */          
        TBuf8<8> iOperatorCode;
        
        /**
         * GSM bandwidth info variable
         */         
        TUint8 iGsmBandInfo;
        
    };

/**
 *  List reader object
 */ 
NONSHARABLE_CLASS( CListReadAllAttempt ) : public CBase
    {
    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */    
        static CListReadAllAttempt* NewL( RMobilePhone::TClientId* aId );
    
        /**
         * Destructor.
         */    
        ~CListReadAllAttempt();

    protected:
    
        /**
         * C++ default constructor.
         */    
        CListReadAllAttempt( RMobilePhone::TClientId* aId );

    private:
    
        /**
         * By default Symbian 2nd phase constructor is private.
         */    
        void ConstructL();

    public: // Data
    
        /**
         * list
         */                 
        CBufBase* iListBuf;
        
        /**
         * Client ID
         */                 
        RMobilePhone::TClientId iClient;
    };
 
/**
 *  ReqHandle entry for call ownership list
 */ 
NONSHARABLE_CLASS( CAcquireEntry ) : public CBase
    {
    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */     
        static CAcquireEntry* NewL( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * C++ default constructor.
         */          
        CAcquireEntry( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Destructor.
         */          
        ~CAcquireEntry();
        
        // New functions
        
        void Deque();
        
    public:
           
        /**
         * TSY request handle
         */                  
        TTsyReqHandle iTsyReqHandle;
        
    private:
    
        /**
         * Double queue link
         */
        TDblQueLink iLink;
        
        friend class CAcquireOwnerList;
    };

/**
 *  Call ownership change request list
 */ 
NONSHARABLE_CLASS( CAcquireOwnerList ) : public CBase
    {
    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */     
        static CAcquireOwnerList* NewL();
        
        /**
         * C++ default constructor.
         */         
        CAcquireOwnerList();
        
        /**
         * Destructor.
         */         
        ~CAcquireOwnerList();
        
        /**
         * Symbian 2nd phase constructor.
         */        
        void ConstructL();
        
        /**
         * Find by TSY request handle
         *          
         * 
         * @return acquire entry
         */ 
        CAcquireEntry* FindByTsyReqHandle(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Remove acquire entry
         *          
         * 
         */             
        void Remove( CAcquireEntry* aEntry );
        
    public: // Data
    
        /**
         * acquire list
         */                 
        TDblQue<CAcquireEntry> iAcquireList;
    };
 
/**
*  Second counter
*/ 
NONSHARABLE_CLASS( CHeartbeatRunner ) : public CBase, public MBeating
    {
    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */
        static CHeartbeatRunner* NewL( CMmCallTsy* aMmCall, CMmPhoneTsy* aMmPhone = NULL );
    
        /**
         * Destructor
         */
        virtual ~CHeartbeatRunner(); 
        
        // New functions
        
        /**
         * Start timer
         * 
         *                   
         */
        void Start();
    
        /**
         * Start timer
         * 
         *                   
         */
        void StartAirTimeDuration();
    
        /**
         * Stop timer
         * 
         *                   
         * @return TBool: True if heart beat runner was started
         */
        TBool Stop();
  
        /**
         * Called when we need to synchronize
         * 
         *                   
         */
        void Synchronize();
    
        /**
         * Returns number of beats from the previous Start of the timer.
         * 
         * 
         * @return number of beats from previous Start of the timer                           
         */
        TInt NumberOfBeats();
            
        /**
         * returns a pointer to iStartTime
         * 
         * 
         * @return a pointer to iStartTime                   
         */
        TTime* GetTimerStartTime();

    private:
    
        /**
         * Default constructor
         */
        CHeartbeatRunner();
        
        /**
         * 2nd phase of the construction
         */
        void ConstructL();
    
        /**
         * Called when beat works ok, once every second
         * 
         *                   
         */
        void Beat();        

    private: // Data
        
        /**
         * Timer
         * Not own.          
         */                 
        CHeartbeat* iHeartbeat; 
        
        /**
         * Pointer to call object
         * No own.         
         */         
        CMmCallTsy* iMmCall;
        
        /**
         * Pointer to phone object
         * No own.         
         */         
        CMmPhoneTsy* iMmPhone;        
        
        /**
         * flag that indicates is this timer active
         */         
        TBool iActive; 
        
        /**
         * Start time of timer
         */         
        TTime iStartTime;
        
        /**
         * Air time durationf flag
         */                  
        TBool iAirTimeDuration;
        
        /**
         * Number of beats of the previous Start->Stop cycle
         */
        TInt iPreviousNumberOfBeats;

    };

#endif // CMMPRIVATEUTILITY_H

// End of File
