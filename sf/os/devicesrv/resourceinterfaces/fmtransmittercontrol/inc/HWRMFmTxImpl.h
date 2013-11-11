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



#ifndef HWRMFMTXIMPL_H
#define HWRMFMTXIMPL_H

// INCLUDES
#include <hwrm/hwrmfmtx.h>
#include "HWRMClient.h"
#include "hwrmuipluginservice.h"

// FORWARD DECLARATIONS
class CHWRMFmTxStatusObserver;
class CHWRMFmTxFrequencyObserver;
class CHwrmUiPluginHandler;

// CLASS DECLARATION

/**
*  Class for the FM Tx client-side session.
*
*/
NONSHARABLE_CLASS(CHWRMFmTxImpl) : public CHWRMFmTx, 
								   public MHwrmForegroundObserver
    {
    public:  // Constructors and destructor
        
        // Implementations are always constructed by CHWRMFmTx::NewL or NewLC.

        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxImpl();

    public: // New functions

    public: // Functions from base classes
       
        // From CHWRMFmTx        
        
        void ReserveL(TBool aForceNoCCoeEnv);       
        void Release();
        
        TFmTxState Status() const;
        
        void EnableL(TInt aFrequency=0);
        void DisableL();

        TInt Frequency() const;
        void GetFrequencyRangeL(TFmTxFrequencyRange& aFrequencyRange) const;  
        void SetFrequencyL(TInt aFrequency);
        
        void GetNextClearFrequencyL(TInt& aClearFrequency);
        void GetNextClearFrequencyL(TClearFrequencyArray& aClearFrequencyArray, TUint& aCount);
        void GetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency);
        void GetNextClearFrequencyL(TRequestStatus& aStatus, TClearFrequencyArray& aClearFrequencyArray, TUint& aCount);
        void CancelGetNextClearFrequency();        
        
        void SetNextClearFrequencyL(TInt& aClearFrequency);        
        void SetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency);
        void CancelSetNextClearFrequency();
        
        TBool IsAutoFrequencyEnabled();
        void  EnableAutoFrequencyL();        
        void  DisableAutoFrequencyL();

        void GetRdsData(TFmTxRdsData& aData);
        void SetRdsPtyL(TRdsProgrammeType aPty);
        void SetRdsPtynL(TRdsProgrammeTypeName& aPtyn);
        void SetRdsMsL(TBool aMusic);
        void SetRdsLanguageIdL(TRdsLanguageIdType aLanguageId);
        
        void SetRadiotextL(TDes& aText);
        void SetRadiotextPlusL(TDes& aText, TRtPlusContentType aContentType);
        void SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag);      
        void SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag1, TRtPlusTag aTag2);
        
        void ClearRdsData();
        void ClearRadiotextData();
        
        // From MHWRMForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();

    private:

        /**
        * C++ default constructor.
        */
        CHWRMFmTxImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMFmTxObserver* aCallback);

        /**
        * Closes the session. 
        */
        void Close();
    
        /**
        * Range check on aFrequency parameter
        */
        TBool FrequencyRangeCheck(TInt aFrequency);
                
        /**
        * Do actual reservation. For parameters, see ReserveL
        */        
        void DoReserveL(TBool aForceNoCCoeEnv);
        
        /**
        * Do actual release
        */
        TInt DoRelease();
        
        /**
         * Round up the frequency to the nearest valid frequency  
         * based on the frequency range and the step size
         */
        TInt RoundFreqToNearestStep(TInt aFrequency);

    public:     // Data
    
    protected:  // Data

    private:    // Data

        RHWRMClient                 iClient;           // HWRM server client
        MHWRMFmTxObserver*          iCallback;         // Client listening events. Not owned
        CHWRMFmTxStatusObserver*    iStatusObserver;   // FM Tx status observer pointer. Owned.
        CHWRMFmTxFrequencyObserver* iFrequencyObserver;   // FM Tx frequency observer pointer. Owned.
        CHwrmUiPluginHandler*    	iUiPluginHandler;
        
		TPtr8 iClearFrequencyPckg;
		TPtr8 iClearFrequencyCountPckg;

    public:     // Friend classes

        friend class CHWRMFmTx;
    };
    
#endif      // HWRMFMTXIMPL_H   
            
// End of File
