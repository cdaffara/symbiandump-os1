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



#ifndef HWRMFMTXFREQUENCYOBSERVER_H
#define HWRMFMTXFREQUENCYOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <centralrepository.h>

// FORWARD DECLARATIONS
class MHWRMFmTxObserver;

// CLASS DECLARATION

/**
*  This class observes the FM Tx frequency
*
*/
NONSHARABLE_CLASS(CHWRMFmTxFrequencyObserver): public CBase,
                                   public MCenRepNotifyHandlerCallback
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        */
        static CHWRMFmTxFrequencyObserver* NewL(MHWRMFmTxObserver* aCallback);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxFrequencyObserver();
        
    public: // New functions
    
        /**
        * This method retrieves the current FM Tx state. 
        *
        * @return TInt indicating the current FM Tx frequency.
        */
        TInt Frequency() const;    
    
        /**
        * This method retrieves the minimum FM Tx frequency
        *
        * @return TInt The lower boundary of the FM range (kHz)
        */
        TInt MinFrequency() const; 

        /**
        * This method retrieves the maximum FM Tx frequency
        *
        * @return TInt The upper boundary of the FM range (kHz)
        */
        TInt MaxFrequency() const; 

        /**
        * This method retrieves the FM Tx frequency step size
        *
        * @return TInt The step size (kHz) 
        */
        TInt StepSize() const; 
        
        /**
        * This method queries whether the frequency range has been updated.
        * Initially CenRep does not contain valid frequency range settings.
        *
        * @return TBool ETrue if frequency range has been updated.
        */
        TBool FrequencyRangeUpdated() const;

    public: // Functions from base classes

        // From MCenRepNotifyHandlerCallback
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);

    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMFmTxFrequencyObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMFmTxObserver*  aCallback);        

    private:
    
        CRepository*          iRepository;          // Cenrep client

        // Cenrep notifications
        CCenRepNotifyHandler* iFreqNotify;
        CCenRepNotifyHandler* iMinFreqNotify;       
        CCenRepNotifyHandler* iMaxFreqNotify;
        CCenRepNotifyHandler* iStepSizeNotify;
        
        MHWRMFmTxObserver*    iCallback;            // Callback object for passing FM Tx frequency changes. Not owned
        TInt                  iFrequency;           // Current FM Tx frequency
        
        // FM frequency range
        TInt iMinFrequency;
        TInt iMaxFrequency;
        TInt iStepSize;
    };

#endif  // HWRMFMTXFREQUENCYOBSERVER_H
            
// End of File
