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
#include "ctlbstestPsy3RequestHandler.h"
#include "ctlbstestPsy3Constants.h"

// ---------------------------------------------------------
// CT_LbsPsy2RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsPsy3RequestHandler* CT_LbsPsy3RequestHandler::InstanceL()
    {
    CT_LbsPsy3RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsPsy3RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsPsy3RequestHandler;
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
void CT_LbsPsy3RequestHandler::Release()
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
CT_LbsPsy3RequestHandler::CT_LbsPsy3RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsPsy3RequestHandler::~CT_LbsPsy3RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsPsy3RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	if (++iRequests > 25)
        {
        iRequests=1;
        }
	CT_LbsTestPsyBase::SetTPositionInfo(aInfo);
	}

// ---------------------------------------------------------
// CT_LbsPsy3RequestHandler::SetNMEAGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3RequestHandler::SetNMEAGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{	
	switch(iRequests)
		{
		case 1:
		case 3:
		case 6:	// No Capabilities set
			break;
		case 2:  // Sentences is 5 but only 2 are set
			if(aGenericInfo.IsRequestedField(EPositionFieldNMEASentences))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentences, KNMEASentences5));
			
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentencesStart, KNMEASentencesStart));
				
                //TInt buffSize = aGenericInfo.BufferSize();

				TUint16 tmp = (TUint16)EPositionFieldNMEASentencesStart;
				User::LeaveIfError(aGenericInfo.SetValue(++tmp, KNMEASentencesSecond));
                
                //TBuf16<100> buf;
                //TInt err = aGenericInfo.GetValue(603, buf); 
			    }
            break;
		default: // All set correct
			if(aGenericInfo.IsRequestedField(EPositionFieldNMEASentences))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentences, KNMEASentences2));
			
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentencesStart, KNMEASentencesStart));
				TUint16 tmp = (TUint16)EPositionFieldNMEASentencesStart;
				User::LeaveIfError(aGenericInfo.SetValue(++tmp, KNMEASentencesSecond));
				}
			break;
		}
	}

// ---------------------------------------------------------
// CT_LbsPsy3RequestHandler::SetSatelliteGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3RequestHandler::SetSatelliteGenericInfoL(HPositionGenericInfo& aGenericInfo, const TBool& /*aCache*/)
	{
	if(iRequests != 3)
		{
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteNumInView) && iRequests != 4)		
			{
			if(iRequests == 6) // 127 satellites in view
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumInView, KSatelliteNumInViewError));
			else
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumInView, KSatelliteNumInView2));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteNumUsed) && iRequests != 5)
			{
			if(iRequests == 7) // Sat. used > sat. in view 
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumUsed, KSatelliteNumUsed3));
			else if(iRequests == 8) // Sat. used -1
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumUsed, KSatelliteNumUsedError));
			else
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumUsed, KSatelliteNumUsed2));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteTime) && iRequests != 6)
			{
			TTime satTime;
			satTime.UniversalTime();
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTime, satTime));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteHorizontalDoP) && iRequests != 7)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteHorizontalDoP, KSatelliteHorizontalDoP));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteVerticalDoP) && iRequests != 8)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteVerticalDoP, KSatelliteVerticalDoP));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteTimeDoP) && iRequests != 9)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTimeDoP, KSatelliteTimeDoP));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatellitePositionDoP) && iRequests != 10)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatellitePositionDoP, KSatellitePositionDoP));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteSeaLevelAltitude) && iRequests != 11)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteSeaLevelAltitude, KSatelliteWGS84Altitude));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldSatelliteGeoidalSeparation) && iRequests != 12)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteGeoidalSeparation, KSatelliteGeoidalSeparation));
			}
		}
	}

// ---------------------------------------------------------
// CT_LbsPsy3RequestHandler::SetDirectionGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3RequestHandler::SetDirectionGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	switch(iRequests)
		{
		case 3:	// No Capabilities set
			break;
		case 4: // Heading error not set  
			if(aGenericInfo.IsRequestedField(EPositionFieldHeading))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeading, KHeading));
				}
			break;
		case 5:	// Heading not set
			if(aGenericInfo.IsRequestedField(EPositionFieldHeadingError))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeadingError, KHeadingError));
				}
			break;
		default: // All set correct
			if(aGenericInfo.IsRequestedField(EPositionFieldHeading))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeading, KHeading));
				}
			if(aGenericInfo.IsRequestedField(EPositionFieldHeadingError))
				{
				User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeadingError, KHeadingError));
				}
			break;
		}
	}

