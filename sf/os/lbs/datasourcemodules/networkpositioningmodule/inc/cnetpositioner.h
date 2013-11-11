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

/**
 @file
 @InternalComponent
*/

#ifndef NETPOSITIONER_H
#define NETPOSITIONER_H

#include <lbs/epos_cpositioner.h>
#include "mpositioner.h"
#include "cadaptationpositioner.h"
#include "trequestparameters.h"

class CPositionerQ;

class CNETPositioner :public CAdaptationPositioner
    {
    public:
        static CNETPositioner* NewL(TAny* aConstructionParameters);
        ~CNETPositioner();
        
    private:
        CNETPositioner();
        void ConstructL(TAny* aConstructionParameters);

        void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus);
        void SetModuleId(TPositionInfoBase* aPosInfo);
		const TRequestParameters* PositionRequestParams();
		
    private:
		TRequestParameters iRequestParameters;
    };

#endif	// NETPOSITIONER_H

// End of File
