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


#if (!defined __T_KEYPAD_DRIVER_SERVER_H__)
#define __T_KEYPAD_DRIVER_SERVER_H__

//	EPOC Includes
#include <testserver2.h>

// This wrapper class extends the test server and creates test server for keypad driver
class CT_KeypadDriverServer : public CTestServer2
	{
private:
	class CT_KeypadDriverBlock : public CTestBlockController
		{
	public:
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	static CT_KeypadDriverServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_KeypadDriverServer.inl"

#endif /* __T_KEYPAD_DRIVER_SERVER_H__ */
