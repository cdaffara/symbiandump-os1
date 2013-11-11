/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file of the vibra control API implementation that
*                uses HWRM Vibra API instead of DOS.
*
*/


#ifndef VIBRACTRLWRAPPERIMPL_H
#define VIBRACTRLWRAPPERIMPL_H

//  INCLUDES
#include "vibractrl.h"
#include <hwrmvibra.h>

class CVibraControlImpl : public CVibraControl, 
                          public MHWRMVibraObserver
    {
    public: // Constructors and destructor
        CVibraControlImpl(MVibraControlObserver* aCallback);
        void ConstructL();
        virtual ~CVibraControlImpl();

    public:
		// From MHWRMVibraObserver
        void VibraModeChanged(CHWRMVibra::TVibraModeState aStatus);
        void VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus);
                
        //From CVibraControl
        // Note - It is not actually necessary to import these as they are virtual in API, 
        //        but since these are SDK methods, not really feasible to change them either.
        IMPORT_C void StartVibraL(TUint16 aDuration);
        IMPORT_C void StopVibraL();
        IMPORT_C TVibraModeState VibraSettings() const;
        IMPORT_C void StartVibraL(TUint16 aDuration, TInt aIntensity);

    private:
        /**
        * Client of the Vibra Control is notified about the status 
        * of the request, 
        * i.e. MVibraControlObserver::VibraRequestStatus method is called.
        *
        * @param aStatus Indicates vibra request status.
        * @see MVibraControlObserver
        */
        void NotifyRequestStatus(TVibraRequestStatus aStatus);
        void StartVibraL(TUint16 aDuration, TInt aIntensity, TBool aUseDefaultIntensity);
        void HandleErrorL(TInt aError);
        
    private: //Data
        MVibraControlObserver*  iCallback;    // Client listening events. Not owned.
        CHWRMVibra*             iVibra;       // HWRM Vibra client. Owned.
        TBool                   iVibraRouteEvents;  // If ETrue, vibra events are propagated to client
        TBool                   iExpectingStop;  // If ETrue, expecting vibra stopped notification from HWRM
    };
       
#endif      // VIBRACTRLWRAPPERIMPL_H
