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


#ifndef T_ETELMM_SERVER_H
#define T_ETELMM_SERVER_H

//	EPOC Includes
#include <testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for Telephony-eTelmm
 */
class CeTelmmServer : public CTestServer2
	{
private:
	class CeTelmmBlock : public CTestBlockController
		{
	public:
		inline CeTelmmBlock();
		inline ~CeTelmmBlock();

		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	static CeTelmmServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	
	};

#include "t_etelmmserver.inl"

#endif // T_ETELMM_SERVER_H

	