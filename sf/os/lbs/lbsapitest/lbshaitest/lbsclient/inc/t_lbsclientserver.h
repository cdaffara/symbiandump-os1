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


#ifndef __T_LBS_CLIENT_SERVER_H__
#define __T_LBS_CLIENT_SERVER_H__

#include <testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for Lbs.
 */
class CT_LbsClientServer : public CTestServer2
	{
private:
	class CT_LbsBlock : public CTestBlockController
		{
	public:
		inline CT_LbsBlock();
		inline ~CT_LbsBlock();
		CDataWrapper* CreateDataL( const TDesC& aData );
		};

public:
	inline ~CT_LbsClientServer();
	inline void	DeleteActiveSchedulerL();
	static CT_LbsClientServer* NewL();
	inline CTestBlockController* CreateTestBlock();

protected:
	inline CT_LbsClientServer();
	};

#include "t_lbsclientserver.inl"

#endif // __T_LBS_CLIENT_SERVER_H__
