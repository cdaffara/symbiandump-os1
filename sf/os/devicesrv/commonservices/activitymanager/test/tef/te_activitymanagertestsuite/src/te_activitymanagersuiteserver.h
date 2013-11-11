/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file 
 @internalTechnology
*/

#ifndef TE_ACTIVITYMANAGERSUITE_SERVER_H
#define TE_ACTIVITYMANAGERSUITE_SERVER_H

#include <test/testblockcontroller.h>
#include <test/testserver2.h>
#include "te_activitymanager.h"

class CTeActivityManagerSuite : public CTestServer2
	{
private:
	class CtActivityManagerBlock : public CTestBlockController
		{
	public:
		inline CtActivityManagerBlock();
		inline ~CtActivityManagerBlock();

		inline CDataWrapper* CreateDataL( const TDesC& aData );
		};

public:
	inline CTeActivityManagerSuite();
	inline ~CTeActivityManagerSuite();

	static CTeActivityManagerSuite* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "te_activitymanagersuiteserver.inl"

#endif // TE_ACTIVITYMANAGERSUITE_SERVER_H
