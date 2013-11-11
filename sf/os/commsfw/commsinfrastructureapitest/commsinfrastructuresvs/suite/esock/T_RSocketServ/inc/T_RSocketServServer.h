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


#if (!defined __T_R_SOCKETSERV_SERVER_H__)
#define __T_R_SOCKETSERV_SERVER_H__

//	EPOC Includes
#include <testserver2.h>

//	User Includes
#include "T_RSocketServBlock.h"

class CT_RSocketServServer : public CTestServer2
	{
public:
    CT_RSocketServServer() {}
    ~CT_RSocketServServer() {}
    
    void	DeleteActiveSchedulerL() {};

    static CT_RSocketServServer* NewL();

    CTestBlockController*    CreateTestBlock();
	};

#endif /* __T_R_SOCKETSERV_SERVER_H__ */
