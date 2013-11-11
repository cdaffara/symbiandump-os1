// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include "ctlbstestpsy3requesthandler.h"
#include "ctlbs_testPsy3Constants.h"

// ---------------------------------------------------------
// CT_LbsTestPsy2RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy3RequestHandler* CT_LbsTestPsy3RequestHandler::InstanceL()
    {
    CT_LbsTestPsy3RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy3RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy3RequestHandler;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self
		Dll::SetTls(reinterpret_cast<TAny*>(self));
        }

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// ::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy3RequestHandler::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy3RequestHandler::CT_LbsTestPsy3RequestHandler() :
	CT_LbsTestPsyBase(NULL),
	KTimeToFirstFixError(6000000),
	KTimeToFixError(2000000),
	KTimeToFix(100000)
    {
	
	}

// Destructor
CT_LbsTestPsy3RequestHandler::~CT_LbsTestPsy3RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsTestPsy3RequestHandler::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy3RequestHandler::NotifyPositionUpdate(TPositionInfo* aPosInfo, TRequestStatus* aStatus)
	{
	++iRequests;
	if(iRequests == 1)
		{
		SetTimerDelay(KTimeToFirstFixError);
		}	
	else if(iRequests % 2)
		{
		SetTimerDelay(KTimeToFixError);
		}
	else
		{
		SetTimerDelay(KTimeToFix);
		}
	
	CT_LbsTestPsyBase::NotifyPositionUpdate(aPosInfo,aStatus);
	}
// ---------------------------------------------------------
// CT_LbsTestPsy3RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//                         
void CT_LbsTestPsy3RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	TPosition pos;
	
	pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
	pos.SetCurrentTime();
	
	if(iRequests % 2)
		{
		pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);
		}
	else
		{
		pos.SetAccuracy(KHorizontalAcc2, KVerticalAcc2);
		}

	aInfo.SetPosition(pos);	

	}

