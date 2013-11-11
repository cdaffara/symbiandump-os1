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
#include "ctlbstestpsy18requesthandler.h"
#include "ctlbs_testpsy18constants.h"

// ---------------------------------------------------------
// CT_LbsTestPsy18RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy18RequestHandler* CT_LbsTestPsy18RequestHandler::InstanceL()
    {
    CT_LbsTestPsy18RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy18RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy18RequestHandler;
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
void CT_LbsTestPsy18RequestHandler::Release()
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
CT_LbsTestPsy18RequestHandler::CT_LbsTestPsy18RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsTestPsy18RequestHandler::~CT_LbsTestPsy18RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsTestPsy18RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy18RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache)
	{
	CT_LbsTestPsyBase::SetTPositionInfo(aInfo, aUseCache);
	
    if (++iRequests > 25)
        {
        iRequests=1;
        }
	}

// ---------------------------------------------------------
// CT_LbsTestPsy18RequestHandler::SetAddressGenericInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy18RequestHandler::SetAddressGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	
	if(iRequests != 1 && iRequests != 11)
		{
		if(aGenericInfo.IsRequestedField(EPositionFieldCountry) && iRequests != 2 && iRequests != 14)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountry, KCountry));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldCountry) && iRequests != 3 && iRequests != 15)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountryCode, KCountryCode));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldState) && iRequests != 4 && iRequests != 16)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldState, KState));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldCity) && iRequests != 5 && iRequests != 17)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCity, KCity));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldDistrict) && iRequests != 6 && iRequests != 18)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldDistrict, KDistrict));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldStreet) && iRequests != 7 && iRequests != 19)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreet, KStreet));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldStreetExtension) && iRequests != 8 && iRequests != 20)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreetExtension, KStreetExtension));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldLocationName) && iRequests != 9 && iRequests != 21)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocationName, KLocationName));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldPostalCode) && iRequests != 10 && iRequests != 22)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldPostalCode, KPostalCode));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldLocality) && iRequests != 11 && iRequests != 23)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocality, KLocality));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldCrossing1) && iRequests != 12 && iRequests != 24)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing1, KCrossing1));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldCrossing2) && iRequests != 13 && iRequests != 25)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing2, KCrossing2));
			}
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsy18RequestHandler::SetBuildingGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
void CT_LbsTestPsy18RequestHandler::SetBuildingGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	if(iRequests != 1 && iRequests != 11)
		{
		if(aGenericInfo.IsRequestedField(EPositionFieldBuildingName) &&
			iRequests != 2 && iRequests != 7 && iRequests != 12 && iRequests != 17 && iRequests != 22)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingName, KBuildingName));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldBuildingFloor) &&
			iRequests != 3 && iRequests != 8 && iRequests != 13 && iRequests != 18 && iRequests != 23)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingFloor, KBuildingFloor));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldBuildingRoom) &&
			iRequests != 4 && iRequests != 9 && iRequests != 14 && iRequests != 19 && iRequests != 24)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingRoom, KBuildingRoom));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldBuildingZone) &&
			iRequests != 5 && iRequests != 10 && iRequests != 15 && iRequests != 20 && iRequests != 25)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingZone, KBuildingZone));
			}
		if(aGenericInfo.IsRequestedField(EPositionFieldBuildingTelephone) &&
			iRequests != 6 && iRequests != 11 && iRequests != 16 && iRequests != 21)
			{
			User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingTelephone, KBuildingTelephone));
			}
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsy18RequestHandler::SetMediaGenericInfoL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy18RequestHandler::SetMediaGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
    if(iRequests % 2)
        {
        if (aGenericInfo.IsRequestedField(EPositionFieldMediaLinks))
		    {
		    User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinks, KMediaLinks));
			
		    User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinksStart, KMediaLink1));
				
            TUint16 tmp = (TUint16)EPositionFieldMediaLinksStart;
		    User::LeaveIfError(aGenericInfo.SetValue(++tmp, KMediaLink2));
            
            User::LeaveIfError(aGenericInfo.SetValue(++tmp, KMediaLink3));
            }
        }
    else
        {
        if (aGenericInfo.IsRequestedField(EPositionFieldMediaLinks))
		    {
		    User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinks, KMediaLinks));
			
		    User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinksStart, KMediaLink1));
				
            TUint16 tmp = (TUint16)EPositionFieldMediaLinksStart;
		    User::LeaveIfError(aGenericInfo.SetValue(++tmp, KMediaLink2));
            }
        }
    }
