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


#ifndef  T_SCREENDRIVER_SERVER_H
#define  T_SCREENDRIVER_SERVER_H

//	EPOC Includes
#include <test/testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for screen driver
 */
class CT_ScreenDriverServer : public CTestServer2
	{
private:
	class CT_DisplayBlock : public CTestBlockController
		{
	public:
		inline CT_DisplayBlock();
		inline ~CT_DisplayBlock();
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	
	static CT_ScreenDriverServer* NewL();
	inline CTestBlockController*	CreateTestBlock();

	};

#include "t_screendriverserver.inl"

#endif // T_SCREENDRIVER_SERVER_H
