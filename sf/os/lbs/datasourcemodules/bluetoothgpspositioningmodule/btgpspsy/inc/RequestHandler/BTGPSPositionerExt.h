/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSPOSITIONEREXT_H
#define BTGPSPOSITIONEREXT_H

//  INCLUDES

#include "BTGPSFix.h"
#include <lbscommon.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  MBTGPSPositionerExt is a callback 
*  interface for the request complete event.
*  
*/
class MBTGPSPositionerExt
    {
    public: 
        
        /**
        * Reports that request shall be completed.
        * @param aFix contains fix information.
        * @param aErr System error code when tries to retrieve a fix. If this
		*        value is not KErrNone then aFix shall not be checked.
        * @return ETrue if the request has been completed. Otherwise EFalse
        */
        virtual TBool RequestCompleteNotify(
            const CBTGPSFix* aFix, 
            TInt aErr) = 0;

        /**
        * Reports that the device status or/and the 
        * data quality status has changed.
        *
        * @param aStatus The new status.
        */
        virtual void ReportStatus(const TPositionModuleStatus& aStatus) = 0;
        
        /**
        * Set initial NMEA buffer bottom
        * @param aIndex The index of previous NMEA buffer bottom.
        */
        virtual void SetInitialNmeaBufferBottom(TInt aIndex) = 0;
        
    };

#endif  //BTGPSPositionerExt_H

// End of File
