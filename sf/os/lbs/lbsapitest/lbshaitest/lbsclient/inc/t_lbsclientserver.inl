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

CT_LbsClientServer::CT_LbsBlock::CT_LbsBlock()
:	CTestBlockController()
	{
	}

CT_LbsClientServer::CT_LbsBlock::~CT_LbsBlock()
	{
	}

CT_LbsClientServer::CT_LbsClientServer()
	{
	}

CT_LbsClientServer::~CT_LbsClientServer()
	{
	}

void CT_LbsClientServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_LbsClientServer::CreateTestBlock()
	{
	return new CT_LbsBlock();
	}
