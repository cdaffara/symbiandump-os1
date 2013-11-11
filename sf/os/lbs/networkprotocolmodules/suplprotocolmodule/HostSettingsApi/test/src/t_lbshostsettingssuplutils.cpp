// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SUPL test utilities
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#include <test/tefunit.h>
#include <lbs/lbshostsettings.h>
#include <lbs/lbshostsettingsclasstypes.h>

#include "t_lbshostsettingssuplutils.h"

/**
Read LBS Host Settings from .ini file

@param aConfig		Test configuration
@param aSetting		LBS host settings output
@param aSectionName .ini file section name
*/
TInt CTestSuplUtils::GetHostSettingFromConfig(CTestConfig& aConfig, TLbsHostSettingsSupl& aSetting, const TPtrC& aSectionName)
	{
	TInt ap = 0;
	TInt ct = 0;	
	TBool readOnly;
	
	TBuf16<KLbsMaxHostNameAddressSize> addrBuffer;
	TPtrC16 addrPtr = addrBuffer.Ptr();
	
	TBuf16<KLbsMaxSettingName> nameBuffer;		
	TPtrC16 namePtr = nameBuffer.Ptr();
	
	TBuf16<KLbsMaxSettingName> providerIdBuffer;		
	TPtrC16 providerIdPtr = providerIdBuffer.Ptr();
	
	if(aConfig.GetString(aSectionName, _L("HostNameAddress"), addrPtr) &&
	   aConfig.GetInt(aSectionName, _L("HostNameAccessPoint"), ap) &&
	   aConfig.GetInt(aSectionName, _L("ReadOnly"), readOnly) &&
	   aConfig.GetInt(aSectionName, _L("HostNameAccessPointType"), ct))
		{
		
		// set account settings	
		TLbsHostNameAddress addr;		
		addr.Copy(addrPtr);
		aSetting.SetHostNameAddress(addr);
		
		aSetting.SetConnectionPoint(ap, static_cast<TLbsHostSettingsSupl::TLbsConnectionType>(ct));
		aSetting.SetReadOnly(readOnly);
		
		// get port id if supplied
		TInt portId;
		if(aConfig.GetInt(aSectionName, _L("HostPort"), portId))
			{			
			aSetting.SetPortId(portId);
			}
		else
			{
			aSetting.SetPortId(KLbsSuplUseDefaultPortId);
			}
		
		// get name parameter if supplied	
		if(aConfig.GetString(aSectionName, _L("HostName"), namePtr))
			{			
			TLbsHostName hostname;
			hostname.Copy(namePtr);
			aSetting.SetName(hostname);
			}	
		
		// get providerid parameter if supplied	
		if(aConfig.GetString(aSectionName, _L("ProviderId"), providerIdPtr))
			{			
			TLbsProviderId providerId;
			providerId.Copy(providerIdPtr);
			aSetting.SetProviderId(providerId);
			}	
						
		return KErrNone;
		}
	return KErrNotFound;
	}

/**
Compare LBS Host Settings from config file with those retrieved from CenRep

@param aSetting1		LBS host settings from config file
@param aSetting2		LBS host settings from CenRep
@return TRUE if settings are equal, otherwise FALSE
*/
TBool CTestSuplUtils::AreSettingsEqual(const TLbsHostSettingsSupl& aSetting1, const TLbsHostSettingsSupl& aSetting2)
	{
	TLbsHostNameAddress addr1, addr2;
	TLbsHostName 		name1, name2;
	TLbsProviderId		providerId1, providerId2;
	TInt 		 		ap1, ap2; // NAP or proxy id
	TLbsHostSettingsSupl::TLbsConnectionType  ct1, ct2;
	TLbsHostCreatorId	c2;
	TUint               cs1, cs2; // class size
	TBool               ro1, ro2;  // read only
	TInt                portId1, portId2;	
	
	aSetting1.GetHostNameAddress(addr1);
	aSetting2.GetHostNameAddress(addr2);

	aSetting1.GetConnectionPoint(ap1, ct1);
	aSetting2.GetConnectionPoint(ap2, ct2);
	
	c2 = aSetting2.CreatorId();	
		
	cs1 = aSetting1.ClassSize();                               
	cs2 = aSetting2.ClassSize();
		
	aSetting1.GetReadOnly(ro1);
	aSetting2.GetReadOnly(ro2);
	
	aSetting1.GetPortId(portId1);
	aSetting2.GetPortId(portId2);
		
	aSetting1.GetName(name1);
	aSetting2.GetName(name2);
	
	aSetting1.GetProviderId(providerId1);
	aSetting2.GetProviderId(providerId2);
	
//  SetCreatorId is not exported so we can't do a direct comparison of creator ids. However, we will check 
//	that the creator id of the setting retrieved from the CenRep is equal to KUidLbsCreatorId. For simplicity
//  sake we will make it convention that this is the second host setting passed in.
	TBool match = ((0 == addr1.Compare(addr2)) &&
				  (0 == name1.Compare(name2)) &&
				  (0 == providerId1.Compare(providerId2)) &&
				  (ap1 == ap2) &&
			      (ct1 == ct2) &&
			      (KLbsHostSettingsTestCreatorId == c2) &&
			      (cs1 == cs2) &&
			      (portId1 == portId1) &&
				  (ro1 == ro2));
			      
	return match;
	}
