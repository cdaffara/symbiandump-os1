/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __CT_LBS_TEST_PROXY_PSY_BASE_H__
#define __CT_LBS_TEST_PROXY_PSY_BASE_H__

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MPositionerStatus;
class TPositionInfo;

// CLASS DECLARATION

// CONSTANTS


class CT_LbsTestProxyPsyBase : public CActive
    {

    public:  // Constructors and destructor
        
        CT_LbsTestProxyPsyBase(MPositionerStatus* aPositionerStatus);
        
        
        /**
        * Destructor.
        */
        virtual CT_LbsTestProxyPsyBase::~CT_LbsTestProxyPsyBase();      

    public:

        /**
		* Sets module. uid
		*/
		void SetModuleId(const TUid& aModuleId);

        /**
        * Requests position info asynchronously.
        * @param aStatus a request status to update during the request
        * @param aPosition will point to the requested position when
        *                  the request is done.
        */
        virtual void NotifyPositionUpdate(TPositionInfo* aPosInfo,TRequestStatus* aStatus);

        /**
        * Cancels a position info request.
        */
        virtual void CancelNotifyPositionUpdate();

        void ReportStatus(const TInt aDeviceStatus, const TInt aDataQuality);

         /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request
        */
        void DoCancel();

        void SetErrorCode(const TInt& aErrorCode);

        void SetTimerDelay(const TTimeIntervalMicroSeconds& aTimerDelay);

        void StartTracking();

        void StopTracking();

    protected:

        void SetTPositionInfo(TPositionInfo& aInfo);

		

    private:
        
        void StartRequest();

    private:

         // By default, prohibit copy constructor
        CT_LbsTestProxyPsyBase(const CT_LbsTestProxyPsyBase&);
        // Prohibit assigment operator
        CT_LbsTestProxyPsyBase& operator=(const CT_LbsTestProxyPsyBase&);
      
    protected: // Data
        
        

    private: // Data

        MPositionerStatus*			iPositionerStatus;
        TUid                        iModuleId;
        TTimeIntervalMicroSeconds	iTimerDelay;
        RTimer						iTimer;
        TRequestStatus*		        iRequestStatus;
        TPositionInfo*      		iRequestPosInfo;
        TInt                        iErrorCode;
    };

#endif      // __CT_LBS_TEST_PROXY_PSY_BASE_H__
            
// End of File
