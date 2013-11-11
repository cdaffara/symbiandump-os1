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


#ifndef __T_RCONNECTION_SERVER_H__
#define __T_RCONNECTION_SERVER_H__

//	EPOC Includes
#include <testserver2.h>

//	User Includes
#include "T_RConnectionBlock.h"

class CT_RConnectionServer : public CTestServer2
	{
public:
	CT_RConnectionServer() {}
	~CT_RConnectionServer() {}

	static CT_RConnectionServer* NewL();

	CTestBlockController*	CreateTestBlock();
	};

#endif /* __T_RCONNECTION_SERVER_H__ */
