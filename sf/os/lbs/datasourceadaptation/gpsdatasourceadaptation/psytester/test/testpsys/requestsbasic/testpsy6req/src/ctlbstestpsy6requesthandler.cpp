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
#include "ctlbstestpsy6requesthandler.h"

// ---------------------------------------------------------
// CT_LbsTestPsy6RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy6RequestHandler* CT_LbsTestPsy6RequestHandler::InstanceL()
    {
    CT_LbsTestPsy6RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy6RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy6RequestHandler;
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
void CT_LbsTestPsy6RequestHandler::Release()
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
CT_LbsTestPsy6RequestHandler::CT_LbsTestPsy6RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsTestPsy6RequestHandler::~CT_LbsTestPsy6RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsTestPsy6RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy6RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	++iRequests;
	if(!(iRequests % 4) && iRequests != 12 && iRequests  != 24 && iRequests  < 36)
		{
		CompleteRequest(KActiveRequest, KErrArgument);
		}
	else
		{
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsy6RequestHandler::SetTPositionCourseInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy6RequestHandler::SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo)
	{
	if(!(iRequests % 3))
		{
		CompleteRequest(KActiveRequest, KErrArgument);
		}	
	else
		{
		CT_LbsTestPsyBase::SetTPositionCourseInfo(aCourseInfo);
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsy6RequestHandler::SetTPositionSatelliteInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy6RequestHandler::SetTPositionSatelliteInfo(TPositionSatelliteInfo& /*aSatelliteInfo*/,const TBool& /*aCache*/)
	{
		
	}
