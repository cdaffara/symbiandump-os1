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


#include "T_MmcSDSocketDriverData.h"
#include "T_MmcSDStackDriverData.h"
#include "T_MmcSDPsuDriverData.h"
#include "T_MmcSDPsuBaseDriverData.h"
#include "T_MmcSDMediaChangeDriverData.h"
#include "T_MmcSDSessionData.h"

/*@{*/
_LIT(KT_MMCSDSocketDriver,				"MMCSDSocket");
_LIT(KT_MMCSDStackDriver,				"MMCSDStack");
_LIT(KT_MMCSDPsuDriver,					"MMCSDPsu");
_LIT(KT_MMCSDPsuBaseDriver,				"MMCSDPsuBase");
_LIT(KT_MMCSDMediaChangeDriver,			"MMCSDMediaChange");
_LIT(KT_MMCSDSession,					"MMCSDSession");
/*@}*/

CT_MmcSDDriverServer::CT_MmcSDDriverBlock::CT_MmcSDDriverBlock()
:	CTestBlockController()
	{
	}

CT_MmcSDDriverServer::CT_MmcSDDriverBlock::~CT_MmcSDDriverBlock()
	{
	}

CDataWrapper* CT_MmcSDDriverServer::CT_MmcSDDriverBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper=NULL;
	if( KT_MMCSDPsuDriver() == aData )
		{
		wrapper=CT_MMCSDPsuDriverData::NewL();
		}
	else if( KT_MMCSDPsuBaseDriver() == aData )
		{
		wrapper=CT_MMCSDPsuBaseDriverData::NewL();
		}
	else if( KT_MMCSDMediaChangeDriver() == aData )
		{
		wrapper=CT_MMCSDMediaChangeDriverData::NewL();
		}
	else if( KT_MMCSDSocketDriver() == aData )
		{
		wrapper=CT_MMCSDSocketDriverData::NewL();
		}
	else if( KT_MMCSDStackDriver() == aData )
		{
		wrapper=CT_MMCSDStackDriverData::NewL();
		}
	else if( KT_MMCSDSession() == aData )
		{
		wrapper=CT_MmcSDSessionData::NewL();
		}


	return wrapper;
	}

CT_MmcSDDriverServer::CT_MmcSDDriverServer()
	{
	}

CT_MmcSDDriverServer::~CT_MmcSDDriverServer()
	{
	}

void CT_MmcSDDriverServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MmcSDDriverServer::CreateTestBlock()
	{
	return new CT_MmcSDDriverBlock();
	}




