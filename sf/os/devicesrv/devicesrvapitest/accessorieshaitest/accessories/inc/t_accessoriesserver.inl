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


CT_AccessoriesServer::CT_AccessoriesBlock::CT_AccessoriesBlock()
:	CTestBlockController()
	{
	}

CT_AccessoriesServer::CT_AccessoriesBlock::~CT_AccessoriesBlock()
	{
	}

CT_AccessoriesServer::CT_AccessoriesServer()
	{
	}

CT_AccessoriesServer::~CT_AccessoriesServer()
	{
	}

CTestBlockController* CT_AccessoriesServer::CreateTestBlock()
	{
	return new CT_AccessoriesBlock();
	}
