/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TE_GEOLOCMONITORSUITE_H
#define TE_GEOLOCMONITORSUITE_H

#include <test/testexecuteserverbase.h>
#include "lbsnetinternalapi.h"

class CTe_GeoLocMonitorSuite : public CTestServer
	{
public:
	static CTe_GeoLocMonitorSuite* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	virtual void ConstructL(const TDesC& aName);
	~CTe_GeoLocMonitorSuite();
	};

#endif // TE_GEOLOCMONITORSUITE_H
