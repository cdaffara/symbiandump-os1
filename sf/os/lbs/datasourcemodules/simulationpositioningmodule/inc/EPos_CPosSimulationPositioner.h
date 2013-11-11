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



#ifndef CPOSSIMULATIONPOSITIONER_H
#define CPOSSIMULATIONPOSITIONER_H

// INCLUDES
#include <lbs.h>
#include <lbs/epos_cpositioner.h>

// FORWARD DECLARATIONS
class CPosControllerBase;
class CPosControllerManager;
/**
*  Used to get simulated position info.
*/
class CPosSimulationPositioner : public CPositioner
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosSimulationPositioner* NewL(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CPosSimulationPositioner();

    public:  // New functions

        /**
        * Completes an ongoing request.
        *
        * @param aStatus The request status.
        * @param aCompleteCode The code to complete the request with.
        */
        void CompleteRequest(
        /* IN */    TRequestStatus& aStatus,
        /* IN */    TInt aCompleteCode
        );

        /**
        * Reports status back to MLFW.
        *
        * @param aDeviceStatus A device status to report.
        * @param aDataQualityStatus A quality status to report.
        */
        void ReportStatus(
        /* IN */    TInt aDeviceStatus = KErrNotFound,
        /* IN */    TInt aDataQualityStatus = KErrNotFound
        );

        /**
        * Partial updates allowed from requestor.
        *
        * @return ETrue if requester allow partial updates,
        * EFalse otherwise.
        */
        TBool PartialUpdateAllowed() const;

        /**
        * Clears position info and
        * sets correct module ID and correct update type.
        *
        * @param aPosInfo Position info object to clear.
        */
        static void ClearAndSetDefaultPositionData(
        /* IN/OUT */    TPositionInfoBase& aPosInfo
        );

    protected:  // Functions from base classes

        /**
        * From CPositioner. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        */
        void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
        /* OUT */       TRequestStatus& aStatus
        );

        /**
        * From CPositioner. Cancels position info request.
        */
        void CancelNotifyPositionUpdate();

    private:

        /**
        * C++ default constructor.
        */
        CPosSimulationPositioner();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

        /**
        * Set the device status.
        *
        * @param aStatus The device status.
        */
        void SetDeviceStatus(
        /* IN */   TPositionModuleStatus::TDeviceStatus aStatus
        );

        /**
        * Set the data quality status.
        *
        * @param aStatus The data quality status to set.
        */
        void SetDataQualityStatus(
        /* IN */    TPositionModuleStatus::TDataQualityStatus aStatus
        );

        // By default, prohibit copy constructor
        CPosSimulationPositioner( const CPosSimulationPositioner& );
        // Prohibit assigment operator
        CPosSimulationPositioner& operator= ( const CPosSimulationPositioner& );
        
        /**
        * Retrieve max fix age and check if the previously stored fix can be used.
        * 
        * @return ETrue if the stored fix is within max fix age, EFalse otherwise 
        * or if there is no previously stored fix.
        */
        TBool CanUseLastFix() const;        

    private:    // Data
        CPosControllerBase*     iController;
        CPosControllerManager*	iControllerManager;
        TPositionModuleStatus   iModuleStatus;
        TPositionInfoBase*		iPosInfo;
        
    };

#endif      // CPOSSIMULATIONPOSITIONER_H

// End of File

