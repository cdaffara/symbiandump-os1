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

#ifndef POSITIONERENV_H
#define POSITIONERENV_H

#include <e32base.h>
#include <lbscriteria.h>

class CRequestHandler;
class CResponseHandler;
class CModuleStatusHandler;
class CPositionerQ;
class MPositioner;
class CLbsAdmin;

class CPositionerEnvironment : public CObject
    {
    public:
        virtual ~CPositionerEnvironment();
		static CPositionerEnvironment* NewL();
    	virtual void CloseInstance() = 0;
		virtual void RegisterPSYL(MPositioner* aPSY) = 0;
		virtual void UnregisterPSY(MPositioner* aPSY) = 0;

    	virtual CRequestHandler* GetRequestHandler() = 0;
    	virtual CResponseHandler* GetResponseHandler() = 0;
		void GetEnvCriteriaL(TPositionCriteria& aCriteria, const TUid aCenRepId);
		TUint GetInactivityTimeoutL(const TUid aCenRepId);
    	TBool ProfileInUse();
    	const CLbsAdmin& Admin();
    	
    private:
    	TInt GetDefaultSelfLocateQualityFromAdminProfile(TPositionQuality& aQuality);
    	
	protected:
		void ConstructL();
		
	protected:
        CPositionerQ* iPositionerQ;
        TBool iProfileInUse;
        const CLbsAdmin* iAdmin;
    };

#endif

// End of File
