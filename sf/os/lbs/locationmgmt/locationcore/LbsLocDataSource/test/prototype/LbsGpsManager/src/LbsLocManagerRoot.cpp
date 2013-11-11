// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


//************************************************************************************************************
// System
#include <bacline.h>
#include <e32cons.h>


// Project
#include "LbsLocManagerRoot.h"
#include "LbsLocManagerGps.h"
#include "LbsLasCommon.h"

//************************************************************************************************************
//   CLocationManagerRoot
//************************************************************************************************************
CLocationManagerRoot* CLocationManagerRoot::NewL()
   {
	CLocationManagerRoot* self = new(ELeave) CLocationManagerRoot;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
   };


//------------------------------------------------------------------------------------------------------------
CLocationManagerRoot::CLocationManagerRoot()
	:
	CActive(CActive::EPriorityStandard)
   {
   CActiveScheduler::Add(this);
   }

void CLocationManagerRoot::ConstructL()
   {
   TInt modNum = FindModuleNum();
  
	iLocManager = CLocationManagerGps::NewL(modNum);
	
	iLocManager->NotifyServiceToClose(&iStatus);
	
	SetActive();
   }


//------------------------------------------------------------------------------------------------------------
CLocationManagerRoot::~CLocationManagerRoot()
   {
	delete iLocManager;
	iLocManager = NULL;	
   }


void CLocationManagerRoot::RunL()
	{
	CActiveScheduler::Stop();
	}

void CLocationManagerRoot::DoCancel()
	{
	}


TInt CLocationManagerRoot::FindModuleNum()
   {
   return 0;
   }
