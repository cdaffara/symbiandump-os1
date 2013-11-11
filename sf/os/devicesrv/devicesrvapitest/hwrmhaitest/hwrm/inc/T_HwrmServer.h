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


#ifndef T_HWRM_SERVER_H
#define T_HWRM_SERVER_H

//EPOC Includes
#include <test/testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for Accessories
 */
class CT_HWRMServer : public CTestServer2
	{
private:
	class CT_HWRMBlock : public CTestBlockController
		{
	public:
		inline CT_HWRMBlock();
		inline ~CT_HWRMBlock();

		CDataWrapper* CreateDataL( const TDesC& aData );
		};

public:
	//inline ~CT_HWRMServer();

	static CT_HWRMServer* NewL();

	inline CTestBlockController* CreateTestBlock();

protected:
	//inline CT_HWRMServer();
	};

#include "t_hwrmserver.inl"

#endif // T_HWRM_SERVER_H
