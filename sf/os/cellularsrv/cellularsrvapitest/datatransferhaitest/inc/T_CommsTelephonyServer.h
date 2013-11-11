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


#ifndef T_COMMS_TELEPHONY_SERVER_H
#define T_COMMS_TELEPHONY_SERVER_H

//	EPOC Includes
#include <testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for DataTransfer driver
 */
class CT_CommsTelephonyServer : public CTestServer2
	{
private:
	class CT_CommsTelephonyBlock : public CTestBlockController
		{
	public:
		inline CT_CommsTelephonyBlock();
		inline ~CT_CommsTelephonyBlock();
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	
	static CT_CommsTelephonyServer* NewL();
	inline CTestBlockController*	CreateTestBlock();

	};

#include "T_CommsTelephonyServer.inl"

#endif // T_COMMS_TELEPHONY_SERVER_H

	