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


#ifndef T_ACCESSORIES_SERVER_H
#define T_ACCESSORIES_SERVER_H

//EPOC Includes
#include <test/testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for Accessories
 */
class CT_AccessoriesServer : public CTestServer2
	{
private:
	class CT_AccessoriesBlock : public CTestBlockController
		{
	public:
		inline CT_AccessoriesBlock();
		inline ~CT_AccessoriesBlock();

		CDataWrapper* CreateDataL( const TDesC& aData );
		};

public:
	inline ~CT_AccessoriesServer();

	static CT_AccessoriesServer* NewL();

	inline CTestBlockController* CreateTestBlock();
	
	void DoCmdutilStartApp();

protected:
	inline CT_AccessoriesServer();
	};

#include "t_accessoriesserver.inl"

#endif // T_ACCESSORIES_SERVER_H
