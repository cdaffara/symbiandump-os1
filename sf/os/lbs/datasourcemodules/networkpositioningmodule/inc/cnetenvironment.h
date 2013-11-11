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

#ifndef NETENV_H
#define NETENV_H

#include "cpositionerenvironment.h"

class CNETRequestHandler;
class CNETResponseHandler;
class CNETModStatusHandler;
class CPositionerQ;

class CPositionerQ;

class CNETEnvironment : public CPositionerEnvironment
    {
    public:
		static CPositionerEnvironment* NewL();
		void CloseInstance();
		void RegisterPSYL(MPositioner* aPSY);
		void UnregisterPSY(MPositioner* aPSY);

    public:
    	CRequestHandler* GetRequestHandler();
    	CResponseHandler* GetResponseHandler();

    private:
        CNETEnvironment();
        virtual ~CNETEnvironment();
		void ConstructL();
		void GetEnvCriteria(TPositionCriteria& aCriteria);
		TPositionCriteria& GetEnvCriteria();

	private:
        TPositionCriteria iCriteria;	// fixed criteria for this environment (from quality profile/module cen rep)
		CNETRequestHandler* iRequestHandler;
		CNETResponseHandler* iResponseHandler;
		CNETModStatusHandler* iModStatusHandler;
    };

#endif

// End of File
