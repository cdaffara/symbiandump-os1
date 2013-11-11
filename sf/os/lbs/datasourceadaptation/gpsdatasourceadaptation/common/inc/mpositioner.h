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

#ifndef MPOSITIONER_H
#define MPOSITIONER_H

#include <lbscommon.h>
#include <lbscriteria.h>

class TRequestParameters;
class TPositionInfo;

/**
*  MPositioner is a callback interface for CPositionerQ
*/
class MPositioner
    {
    public: 
        /**
        * Reports that request shall be completed.
        * @param aPositionInfo contains fix information.
        * @param aError error code from retrieved fix
        * @param aActualTime time fix was produced
        */
        virtual void RequestCompleteNotify(const TPositionInfo& aPositionInfo, TInt aError, TTime& aActualTime) = 0;

        /**
        * Reports that the device status or/and the 
        * data quality status has changed.
        *
        * @param aStatus The new status.
        */
        virtual void ReportStatus(const TPositionModuleStatus& aStatus) = 0;
        
		virtual const TRequestParameters* PositionRequestParams() = 0;
        
        virtual TUint InactivityTimeout() = 0;
		virtual TBool IsTracking() = 0;
        virtual TBool IsActive() = 0;
        virtual void UpdateFailed(TInt aError) = 0;
        virtual void ModeUpdate(TUint /*aMode*/){};
        
        /**
         * Whether the positioner has a warmdown timer running
         */
        virtual TBool IsWarmingDown() = 0;
    };

#endif  //MPOSITIONER_H

// End of File
