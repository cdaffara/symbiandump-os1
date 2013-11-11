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


#if (!defined __T_DIGITISER_DRIVER_SERVER_H__)
#define __T_DIGITISER_DRIVER_SERVER_H__

//	EPOC Includes
#include <testserver2.h>



// This wrapper class extends the test server and creates test server for digitiser
class CT_DigitiserDriverServer : public CTestServer2
	{
private:
	class CT_DigitiserDriverBlock : public CTestBlockController
		{
	public:
		inline CT_DigitiserDriverBlock();
		inline ~CT_DigitiserDriverBlock();
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	inline ~CT_DigitiserDriverServer();

	inline void	DeleteActiveSchedulerL();

	static CT_DigitiserDriverServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_DigitiserDriverServer();
	};

#include "T_DigitiserDriverServer.inl"


#endif /* __T_DIGITISER_DRIVER_SERVER_H__ */


