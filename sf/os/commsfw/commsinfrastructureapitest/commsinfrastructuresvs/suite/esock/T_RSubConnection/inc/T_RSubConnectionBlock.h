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


#ifndef __T_RSUBCONNECTION_BLOCK_H__
#define __T_RSUBCONNECTION_BLOCK_H__

//	User Includes
#include <testblockcontroller.h>

#include "T_DataRSubConnection.h"
#include "T_DataRConnection.h"
#include "T_DataRSocket.h"
#include "T_DataRSocketServ.h"
#include "T_DataRSubConParameterBundle.h"
#include "T_DataSubConParameterFamily.h"
#include "T_DataSubConQosGenericParamSet.h"


class CT_RSubConnectionBlock : public CTestBlockController
	{
public:
	CT_RSubConnectionBlock() : CTestBlockController() {}
	~CT_RSubConnectionBlock() {}

	CDataWrapper* CreateDataL( const TDesC& aData );
	};

#endif /* __T_RSUBCONNECTION_BLOCK_H__ */
