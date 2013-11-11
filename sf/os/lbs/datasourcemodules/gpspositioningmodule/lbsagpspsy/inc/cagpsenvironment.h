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

#ifndef AGPSENV_H
#define AGPSENV_H

#include "cpositionerenvironment.h"

class CAgpsRequestHandler;
class CAgpsResponseHandler;
class CAgpsModStatusHandler;
class CPositionerQ;

class CAgpsEnvironment : public CPositionerEnvironment
    {
    public:
		static CPositionerEnvironment* NewL();
		void CloseInstance();
		void RegisterPSYL(MPositioner* aPSY);
		void UnregisterPSY(MPositioner* aPSY);

    public:
    	CRequestHandler* GetRequestHandler();
    	CResponseHandler* GetResponseHandler();
    	TTimeIntervalMicroSeconds& GetFinalNetPositionLag();

    private:
        CAgpsEnvironment();
        virtual ~CAgpsEnvironment();
		void ConstructL();
		void InitFinalNetPositionLagFromAdminProfile();
	
	private:
		CAgpsRequestHandler* iRequestHandler;
		CAgpsResponseHandler* iResponseHandler;
		CAgpsModStatusHandler* iModStatusHandler;
		TTimeIntervalMicroSeconds iFinalNetPositionLag;	
    };

#endif

// End of File
