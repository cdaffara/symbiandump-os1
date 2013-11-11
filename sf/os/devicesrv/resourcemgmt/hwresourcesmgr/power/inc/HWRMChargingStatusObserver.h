// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef HWRMCHARGINGSTATUSOBSERVER_H_
#define HWRMCHARGINGSTATUSOBSERVER_H_

#include "pspropertyobserver.h"

class MHWRMBatteryChargingStatusObserver;


/* Observer class for charging status change */

class CHWRMChargingStatusObs: public CBase,
                              public MPsPropertyObserver
    {
    public:
        static CHWRMChargingStatusObs* NewL(MHWRMBatteryChargingStatusObserver* aChargingStatusCallback);
        
        ~CHWRMChargingStatusObs();

        virtual void PsPropertyChanged(TInt aKey, TInt aValue, TInt aError);

        TInt Get(TInt& val);
        
    private:
        CHWRMChargingStatusObs(MHWRMBatteryChargingStatusObserver* aChargingStatusCallback);

        void ConstructL();
        
        CPsPropertyObserver*                iChargingPsObserver;       // Owned. Pub-Sub Observer.
        MHWRMBatteryChargingStatusObserver* iChargingStatusObserver;   // Not owned. Client listening for charging status change. Not owned
        TInt                                iChargingStatus;
    };

#endif //HWRMCHARGINGSTATUSOBSERVER_H_
