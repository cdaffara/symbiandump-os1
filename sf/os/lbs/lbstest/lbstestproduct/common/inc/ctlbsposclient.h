/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_POS_CLIENT_H__
#define __CT_LBS_POS_CLIENT_H__

//  INCLUDES
#include <lbspositioninfo.h>
#include <e32base.h>
#include <lbs.h>

// CONSTANTS

// FUNCTION TYPES

// FORWARD DECLARATIONS
class CPositioner;
class CT_LbsPosClientHolder;

// CLASS DECLARATION

/**
*  Active object used to make position requests
*
*/
class CT_LbsPosClient : public CActive
    {
    public:  // Constructors and destructor
        
        static CT_LbsPosClient* NewL(CT_LbsPosClientHolder* aParent, TUid aPsy);

        static CT_LbsPosClient* NewL(CT_LbsPosClientHolder* aParent, TUid aPsy, RPositionServer& aPositionServer);

        /**
        * Destructor.
        */
        ~CT_LbsPosClient();

    private:

        /**
        * C++ default constructor.
        */
    	CT_LbsPosClient(CT_LbsPosClientHolder* aParent);

        void ConstructL(TUid aPsy);    

        void ConstructL(TUid aPsy, RPositionServer& aPositionServer);

    public: // Functions from base classes
    	
        /**
        * Starts a position request
        */
        void MakeRequest();

        /**
        * Get result
        */
        void GetResult(
            TInt& aStatus, 
            TPositionInfo& aModuleInfo,
            TTimeIntervalMicroSeconds& aRequestTime
            );

    private:

        void RunL();

        void DoCancel();

    private:
        RPositioner						iPositioner;
        RPositionServer					iPosServer;
        TBool                       	iSameClient;
        TPositionInfo					iModuleInfo;
        TTime							iStartTime;
        TTimeIntervalMicroSeconds		iRequestTime;
        CT_LbsPosClientHolder*	iParent;
    };

#endif      // __CT_LBS_POS_CLIENT_H__
            
// End of File
