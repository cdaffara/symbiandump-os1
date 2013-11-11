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
// This contains CT_e32Server 
#ifndef T_E32_SERVER_H
#define T_E32_SERVER_H

//	EPOC Includes
#include <testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for USB
 */
class CT_e32Server : public CTestServer2
	{
private:
	class CT_e32Block : public CTestBlockController
		{
	public:
		inline CT_e32Block();
		inline ~CT_e32Block();
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	static CT_e32Server* NewL();
	inline CTestBlockController*	CreateTestBlock();

	};

#include "t_e32server.inl"

#endif // T_E32_SERVER_H
