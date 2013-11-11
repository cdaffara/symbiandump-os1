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

#ifndef GPSPOSITIONER_H
#define GPSPOSITIONER_H

#include <lbs/epos_cpositioner.h>
#include "mpositioner.h"
#include "cadaptationpositioner.h"
#include "lbsnetinternalapi.h"
#include "cagpspositioner.h"

class CPositionerQ;
class TAGPSRequestParameters;

class CGpsPositioner : public CAgpsPositioner
    {
    public:
        static CGpsPositioner* NewL(TAny* aConstructionParameters);
        ~CGpsPositioner();

    private:
        CGpsPositioner();
        void ConstructL(TAny* aConstructionParameters);
        
	private:
		void SetModuleId(TPositionInfoBase* aPosInfo);
		void SetAGPSMode();
    };

#endif

// End of File
