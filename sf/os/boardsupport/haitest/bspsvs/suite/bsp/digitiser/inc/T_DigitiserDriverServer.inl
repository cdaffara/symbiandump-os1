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


CT_DigitiserDriverServer::CT_DigitiserDriverBlock::CT_DigitiserDriverBlock()
:	CTestBlockController()
	{
	}

CT_DigitiserDriverServer::CT_DigitiserDriverBlock::~CT_DigitiserDriverBlock()
	{
	}

CT_DigitiserDriverServer::CT_DigitiserDriverServer()
	{
	}

CT_DigitiserDriverServer::~CT_DigitiserDriverServer()
	{
	}

void CT_DigitiserDriverServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_DigitiserDriverServer::CreateTestBlock()
	{
	return new CT_DigitiserDriverBlock();
	}
