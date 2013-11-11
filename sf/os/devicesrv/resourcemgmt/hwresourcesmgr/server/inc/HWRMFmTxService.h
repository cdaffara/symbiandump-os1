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



#ifndef HWRMFMTXSERVICE_H
#define HWRMFMTXSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMGenericTimer.h"
#include "HWRMFmTxCommands.h"
#include "HWRMFmTxData.h"
#include "HWRMService.h"
#include "HWRMFmTxCommonData.h"

// CONSTANTS
// None
// Frequency ranges as of May 2007:
//   88.1-107.9 MHz for rest of the world (FCC lower limit)
//   88.1 - 90 MHz for Japan (we still cannot go lower than FCC lower limit)
const TInt KHWRMFmTxBackupMinFreq  =  88100; // kHz
const TInt KHWRMFmTxBackupMaxFreq  = 107900; // kHz
const TInt KHWRMFmTxBackupStepSize =     50; // kHz

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMFmTxCommonData;
class CHWRMPluginHandler;
class CHWRMReservationHandler;
class CHWRMFmTxRdsTextConverter;

// CLASS DECLARATION

/**
* Data storage class for FM Tx plug-in requests
*/
class THWRMFmTxPluginRequestData : public THWRMPluginRequestData
    {
    public:
        TInt iArg; // Generic integer param

        /**
        * Convenience constructor
        */
        THWRMFmTxPluginRequestData(TInt aArg,
                                   const RMessage2& aRequestMessage,
                                   TUint8 aTransId, 
                                   TInt aCommandId,
                                   TBool aSplitCommand)
            : THWRMPluginRequestData(aRequestMessage, aTransId, aCommandId, aSplitCommand),
              iArg(aArg)
                {
                };
        /**
        * Virtual destructor.
        */
        virtual ~THWRMFmTxPluginRequestData()
            {
            };
    };

/**
*  Hardware Resource Manager server side service object for FmTx sessions.
*/
class CHWRMFmTxService : public CHWRMService
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler The plugin handler to use with this service.
        * @param aWatcherPluginHandler The watcher plugin handler to use with this service.
        * @param aReservationHandler The reservation handler to use with this service.
        * @param aFmTxCommonData Commmon data instance to use with this service.
        * @param aRdsTextConverter Text converter instance to use with this service.
        */
        static CHWRMFmTxService* NewL(CHWRMPluginHandler* aPluginHandler,
        							  CHWRMPluginHandler& aWatcherPluginHandler,
                                      CHWRMReservationHandler* aReservationHandler,
                                      CHWRMFmTxCommonData& aFmTxCommonData,
                                      CHWRMFmTxRdsTextConverter& aRdsTextConverter);

        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxService();

    public: // Base class methods
        
        // From CHWRMService
        virtual TBool ExecuteMessageL( const RMessage2& aMessage );
        virtual void ProcessResponseL( TInt aCommandId, TUint8 aTransId, TDesC8& aData, TBool aTimeout );
        virtual void SuspendSubResource(TInt aSubResource);
        virtual void ResumeSubResource(TInt aSubResource);
        virtual void ActivateSubResource(TInt aSubResource, TBool aActivate);           
    
    public: // Other methods

        /** 
        * Handles plug-in requests that are not client initiated
        * Called by Common Data service instance
        *
        * @param aCommand the plug-in command to execute
        */
        void ExecutePluginCommand(HWRMFmTxCommand::TFmTxCmd aCommand);

        /** 
        * Handles plug-in requests that are not client initiated
        * Called by Common Data service instance
        *
        * @param aCommand the plug-in command to execute
        * @param aSplitCommand ETrue if this command is split into multiple transactions
        * @param aPckg Command package
        */
        void ExecutePluginCommand(HWRMFmTxCommand::TFmTxCmd aCommand,
                                  TBool aSplitCommand,
                                  TDesC8& aPckg );

    private: 
    
        /**
        * C++ default constructor.
        */
        CHWRMFmTxService(CHWRMPluginHandler& aWatcherPluginHandler,
        				 CHWRMFmTxCommonData& aFmTxCommonData,
                         CHWRMFmTxRdsTextConverter& aRdsTextConverter);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);

        // Helpers for ProcessResponseL

        /**
        * Checks the response data size
        *
        * @return ETrue if aData.Size() is correct
        */
        TBool CheckResponseDataSize(TInt aCommandId, TDesC8& aData);

        // Helpers for ExecuteMessageL

        /**
        * Handles EHWRMFmTxOn request
        *
        * @param aFrequency The requested initial Tx frequency
        */
        void EnableL(const RMessage2& aMessage);
        
        /**
        * Handles EHWRMFmTxOff request
        * @return ETrue if client request can be completed immediately
        */
        TBool DisableL(const RMessage2& aMessage);

        /**
        * Handles EGetTxFrequencyRangeCmdId request
        */
        void GetFrequencyRangeL(const RMessage2& aMessage);

        /**
        * Handles EHWRMFmTxSetFreq request
        *
        * @return TBool ETrue if message can be completed immediately, on return
        */
        TBool SetFrequencyL(const RMessage2& aMessage);

        /**
        * Handles EHWRMFmTxGetNextClearFreq requests.
        *
        * @param aClearFreqsRequired The number of clear frequencies requested
        */      
        void GetNextClearFrequencyL(const RMessage2& aMessage,
                                    TUint  aClearFreqsRequired);

        /**
        * Handles EHWRMFmTxSetNextClearFreq requests
        *
        * @param aClearFreqsRequired The number of clear frequencies requested      
        */  
        void SetNextClearFrequencyL(const RMessage2& aMessage,
                                    TUint  aClearFreqsRequired);

        /**
        * Handles EHWRMFmTxSetRdsPty requests
        */  
        void SetRdsPtyL(const RMessage2& aMessage);
        
        /**
        * Handles EHWRMFmTxSetRdsPtyn requests
        */          
        void SetRdsPtynL(const RMessage2& aMessage);
        
        /**
        * Handles EHWRMFmTxSetRdsMs requests
        */          
        void SetRdsMsL(const RMessage2& aMessage);

        /**
        * Handles EHWRMFmTxSetRdsLanguageId requests
        */          
        void SetRdsLanguageIdL(const RMessage2& aMessage);

        /**
        * Handles EHWRMFmTxSetRadiotext requests
        */          
        void SetRadiotextL(const RMessage2& aMessage);
        void SetRadiotextL(const RMessage2& aMessage, TRtPlusTag aTag);
        void SetRadiotextL(const RMessage2& aMessage, TRtPlusTag aTag1, TRtPlusTag aTag2);

        /**
        * Handles EHWRMFmTxClearRdsData requests
        */          
        void ClearRdsDataL(const RMessage2& aMessage);  
        
        /**
        * Handles EHWRMFmTxClearRtData requests
        */          
        void ClearRadiotextDataL(const RMessage2& aMessage);    
        
        /**
        * Handles EHWRMFmTxRelease and EHWRMFmTxCleanup requests
        */
        void DoRelease();

        /**
        * Leaves if another client has reserved FM Tx
        */
        void LeaveIfOtherReservationL();

        /**
        * Leaves if FM Tx is not enabled
        */
        void LeaveIfTransmitterOffL();
        
        /**
        * Leaves if RT plus tag is not valid
        */
        void LeaveIfTagInvalidL( TRtPlusTag aTag );

        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                   HWRMFmTxCommand::TFmTxCmd aCommandId,
                                   TBool aSplitCommand );

        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions        
        * @param aPckg Command package
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                   HWRMFmTxCommand::TFmTxCmd aCommandId,
                                   TBool aSplitCommand,
                                   TDesC8& aPckg );


        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions        
        * @param aPckg Command package  
        * @param aArg Generic integer parameter for THWRMFmTxPluginRequestData       
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                   HWRMFmTxCommand::TFmTxCmd aCommandId,
                                   TBool aSplitCommand,
                                   TDesC8& aPckg,
                                   TInt aArg );

        /**
        * Cancels a plugin command and handles the related transaction(s).
        *
        * @param aCommandId Command ID of the command.  
        */
        void CancelPluginCommandL(HWRMFmTxCommand::TFmTxCmd aCommandId);

    private:  // data

        CHWRMPluginHandler& iWatcherPluginHandler;  // Reference to watcher plugin handler.
        CHWRMPluginTransactionList*	iWatcherTransactionList;  // List of open watcher transactions. Owned.
        CHWRMFmTxCommonData& iFmTxCommonData; // Ref to FM Tx data object common to all sessions. Not owned.
        CHWRMFmTxRdsTextConverter& iRdsTextConverter; // Ref to Unicode->RDS text converter. Not owned.

        TBool iSuspended; // Flag to indicate if this session is suspended.
        TBool iReserved;  // Flag to indicate if this session is reserved.
    };

#endif  // HWRMFMTXSERVICE_H
            
// End of File
