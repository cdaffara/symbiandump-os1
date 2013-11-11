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


#if (!defined __T_R_SOCKETSERV_BLOCK_H__)
#define __T_R_SOCKETSERV_BLOCK_H__

//	User Includes
#include <testblockcontroller.h>

#include "T_DataRSocketServ.h"


/**
RSocketServ API test commands
@internalComponent
*/

_LIT(KT_RSocketServWrapper,           "RSocketServ");

class CT_RSocketServBlock : public CTestBlockController
	{
public:
	CT_RSocketServBlock() : CTestBlockController() {}
    ~CT_RSocketServBlock() {}

    CDataWrapper* CreateDataL( const TDesC& aData );
	};

#endif /* __T_R_SOCKETSERV_BLOCK_H__ */
