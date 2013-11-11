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
#include "ctlbstestpsy4requesthandler.h"
#include "ctlbstestpsy4constants.h"

// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsPsy4RequestHandler* CT_LbsPsy4RequestHandler::InstanceL()
    {
    CT_LbsPsy4RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsPsy4RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsPsy4RequestHandler;
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
void CT_LbsPsy4RequestHandler::Release()
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
CT_LbsPsy4RequestHandler::CT_LbsPsy4RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsPsy4RequestHandler::~CT_LbsPsy4RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy4RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	if (++iRequests > 25)
        {
        iRequests=1;
        }
	CT_LbsTestPsyBase::SetTPositionInfo(aInfo);
	}

// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::SetCompassGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy4RequestHandler::SetCompassGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	
	if(iRequests != 1) 
		{
		if(aGenericInfo.IsRequestedField(EPositionFieldTrueCourse) && iRequests != 2)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourse, KTrueCourse));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldTrueCourseError) && iRequests != 3)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourseError, KTrueCourseError));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldMagneticCourse) && iRequests != 4)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMagneticCourse, KMagneticCourse));
			}
		}
	}
// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::SetSpeedGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy4RequestHandler::SetSpeedGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(iRequests != 2)
		{
		if(aGenericInfo.IsRequestedField(EPositionFieldHorizontalSpeed) && iRequests != 3)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeed, KHorizontalSpeed));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldHorizontalSpeedError) && iRequests != 4)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeedError, KHorizontalSpeedError ));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldVerticalSpeedError) && iRequests != 5)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeedError, KVerticalSpeedError));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldVerticalSpeed) && iRequests != 6)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeed, KVerticalSpeed));
			}
		}
	}
