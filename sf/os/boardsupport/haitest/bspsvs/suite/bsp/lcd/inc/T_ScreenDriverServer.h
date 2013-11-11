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


#if (!defined __T_SCREEN_DRIVER_SERVER_H__)
#define __T_SCREEN_DRIVER_SERVER_H__

//	EPOC Includes
#include <testserver2.h>

// This wrapper class extends the test server and creates test server for screen driver
class CT_ScreenDriverServer : public CTestServer2
	{
private:
	class CT_ScreenDriverBlock : public CTestBlockController
		{
	public:
		inline CT_ScreenDriverBlock();
		inline ~CT_ScreenDriverBlock();

		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	inline ~CT_ScreenDriverServer();

	inline void	DeleteActiveSchedulerL();

	static CT_ScreenDriverServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_ScreenDriverServer();
	};

#include "T_ScreenDriverServer.inl"

#endif /* __T_SCREEN_DRIVER_SERVER_H__ */
