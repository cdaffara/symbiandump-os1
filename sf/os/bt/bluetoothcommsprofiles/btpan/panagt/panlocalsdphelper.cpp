// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// panhelperlocalsdp.cpp
// 
//

#include "panlocalsdphelper.h"
#include "panagtutils.h"
#include "pancommon.h"			// For KBnepPsm
#include "panprog.h"			// For EOther

using namespace PanAgent;

const TSdpServRecordHandle KIllegalSdpRecordHandle = 0;

CPanHelperLocalSdpRegistrar* CPanHelperLocalSdpRegistrar::NewL()
/**
Return a newly constructed local sdp registrar
*/
	{
	CPanHelperLocalSdpRegistrar* self = new(ELeave) CPanHelperLocalSdpRegistrar();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPanHelperLocalSdpRegistrar::CPanHelperLocalSdpRegistrar()
/**
do nothing
*/
	{	}
	
void CPanHelperLocalSdpRegistrar::ConstructL()
/**
Initialisation
*/
	{
	User::LeaveIfError(iSdpServer.Connect());
	User::LeaveIfError(iSdpDatabase.Open(iSdpServer));
	for(TUint i=0;i<KMaxPanRoles;i++)
		{
		iLocalSdpRecordHandles[i] = KIllegalSdpRecordHandle;
		}
	}


	
CPanHelperLocalSdpRegistrar::~CPanHelperLocalSdpRegistrar()
/**
Clean up
*/
	{
	// Remove any SDP records created by this instance.
	for(TUint i=0;i<KMaxPanRoles;i++)
		{
		if(iLocalSdpRecordHandles[i]!=KIllegalSdpRecordHandle)
			{
			iSdpDatabase.DeleteRecord(iLocalSdpRecordHandles[i]);
			iLocalSdpRecordHandles[i] = KIllegalSdpRecordHandle;
			}
		}

	iSdpDatabase.Close();
	iSdpServer.Close();
	}
	


void CPanHelperLocalSdpRegistrar::RegisterLocalSdpRecordL(TBluetoothPanRole aRole, TInt /*aIapId*/)
/**
Register the specified role in the local SDP database
@param aRole The role to register
@param aIapId The IAP that we're using - for finding out which network layer protocols 
we're supporting (not implemented at present), and filling in NAP-related stuff when 
we support NAP (also not supported at present!)
*/
	{
	_LIT8(KProviderName, "Symbian OS");
	
	// Make sure any array access is going to fall within the array bounds
	__ASSERT_DEBUG(static_cast<TUint>(aRole) >= static_cast<TUint>(EPanRoleU), PanAgentPanic(EPanRoleOutOfBounds));
	__ASSERT_DEBUG((static_cast<TUint>(aRole) - static_cast<TUint>(EPanRoleU)) <= KMaxPanRoles, PanAgentPanic(EPanRoleOutOfBounds));
	__ASSERT_DEBUG(((aRole==EPanRoleU)||(aRole==EPanRoleGn)||(aRole==EPanRoleNap)), PanAgentPanic(EAttemptToRegisterIllegalPanRoleInLocalSdpDatabase)); //<@note embeds knowledge of valid roles

	TUUID uuid(static_cast<TUint32>(aRole));
	
	// Rebase the role to use role as index into array
	TUint panRoleRecordHandleIndex = static_cast<TUint>(aRole) - static_cast<TUint>(EPanRoleU);

	// Check if the record is already registered.  If so, no action is required.
	if(iLocalSdpRecordHandles[panRoleRecordHandleIndex] == KIllegalSdpRecordHandle)
		{	
		// Create the SDP record
		iSdpDatabase.CreateServiceRecordL(uuid, iLocalSdpRecordHandles[panRoleRecordHandleIndex]);
	
		
		TSdpServRecordHandle recordHandle = iLocalSdpRecordHandles[panRoleRecordHandleIndex];
		iSdpDatabase.UpdateAttributeL(recordHandle,
			KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetProviderName,
			KProviderName
			);
		//
		// Add attributes
		//
		CSdpAttrValue* attrVal = NULL;
		CSdpAttrValueDES* attrValDES = NULL;	
		
		// Service Class ID list is handled above when creating the record
		// protocol descriptor list
		attrValDES = CSdpAttrValueDES::NewDESL(0);
		CleanupStack::PushL(attrValDES);
		attrValDES->StartListL()
			->BuildDESL()->StartListL()
				->BuildUUIDL(TUUID(TUint16(KL2CAPUUID)))					// UUID for protocol: L2CAP
				->BuildUintL(TSdpIntBuf<TUint16>(KBnepPsm))					// PSM: BNEP
			->EndListL()
			->BuildDESL()->StartListL()
				->BuildUUIDL(TUUID(TUint16(KBnepUUID)))						// UUID for protocol: BNEP
				->BuildUintL(TSdpIntBuf<TUint16>(KBnepVersion))				// version: 0x0100
				->BuildDESL()->StartListL()
					->BuildUintL(TSdpIntBuf<TUint16>(0x0800))				// ethernet protocol: IPv4
					->BuildUintL(TSdpIntBuf<TUint16>(0x0806))				// ethernet protocol: ARP
					->BuildUintL(TSdpIntBuf<TUint16>(0x86dd))				// ethernet protocol: IPv6
				->EndListL()
			->EndListL()
		->EndListL();
		iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);

		// language base attribute ID list
		attrValDES = CSdpAttrValueDES::NewDESL(0);
		CleanupStack::PushL(attrValDES);
		attrValDES->StartListL()
			->BuildUintL(TSdpIntBuf<TUint16>(KLanguageEnglish))
			->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdCharacterEncodingUTF8))
			->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdBasePrimaryLanguage))
		->EndListL();
		iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);

		// bluetooth profile descriptor list
		attrValDES = CSdpAttrValueDES::NewDESL(0);
		CleanupStack::PushL(attrValDES);
		attrValDES->StartListL()
			->BuildDESL()->StartListL()
				->BuildUUIDL(uuid)											// role: uuid of PAN role
				->BuildUintL(TSdpIntBuf<TUint16>(KPanRoleVersion))			// version: 0x0100
			->EndListL()
		->EndListL();
		iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBluetoothProfileDescriptorList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);
		
		// bluetooth browse list
		attrValDES = CSdpAttrValueDES::NewDESL(0);
		CleanupStack::PushL(attrValDES);
		attrValDES->StartListL()
					->BuildUUIDL(TUUID(TUint32(KPublicBrowseGroupUUID)))	 // Public browse group										
				  ->EndListL();
		iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);
	
		// service name
		switch(aRole)
			{
			case EPanRoleU:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Personal Ad-hoc User Service"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;
				}
			case EPanRoleGn:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Group Ad-hoc Network Service"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;
				}
			case EPanRoleNap:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Network Access Point Service"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;			
				}
			default:
				{
				PanAgentPanic(EAttemptToRegisterIllegalPanRoleInLocalSdpDatabase);
				}
			}
			
		// service description
		switch(aRole)
			{
			case EPanRoleU:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Personal Ad-hoc User Service"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;			
				}
			case EPanRoleGn:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Personal Group Ad-hoc Network Service"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;
				}
			case EPanRoleNap:
				{
				attrVal = CSdpAttrValueString::NewStringL(_L8("Personal Ad-hoc Network Service which provides access to a network"));
				CleanupStack::PushL(attrVal);
				iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription, *attrVal);
				CleanupStack::PopAndDestroy(attrVal);
				break;			
				}
			default:
				{
				PanAgentPanic(EAttemptToRegisterIllegalPanRoleInLocalSdpDatabase);
				}
			}

		// Add NAP specific fields.
		if(aRole == EPanRoleNap)
			{
			// Set the uplink type
			attrVal = CSdpAttrValueInt::NewIntL(TSdpIntBuf<TUint16>(EOther));	// Access type set to 'other'
			CleanupStack::PushL(attrVal);
			iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdNetAccessType, *attrVal);
			CleanupStack::PopAndDestroy(attrVal);
			
			// Set the uplink maximum throughput
			attrVal = CSdpAttrValueInt::NewIntL(TSdpIntBuf<TUint32>(KPanNapDefaultUplinkThroughput));
			CleanupStack::PushL(attrVal);
			iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdMaxNetAccessRate, *attrVal);
			CleanupStack::PopAndDestroy(attrVal);
			}
			
		// security description
		attrVal = CSdpAttrValueUint::NewUintL(TSdpIntBuf<TUint16>(EServiceLevelSecurity)); 		// security type: service level
		CleanupStack::PushL(attrVal);
		iSdpDatabase.UpdateAttributeL(recordHandle, KSdpAttrIdSecurityDescription, *attrVal);
		CleanupStack::PopAndDestroy(attrVal);
		}
	}
	
void CPanHelperLocalSdpRegistrar::UnregisterLocalSdpRecord(TBluetoothPanRole aRole)
/**
Can be called to unregister an SDP record even if it is not registered.
Simplifies logic in PAN agent, so it doesn't need to keep track of which roles it has unregistered
*/
	{
	// Make sure any array access is going to fall within the array bounds
	__ASSERT_DEBUG(static_cast<TUint>(aRole) >= static_cast<TUint>(EPanRoleU), PanAgentPanic(EPanRoleOutOfBounds));
	__ASSERT_DEBUG((static_cast<TUint>(aRole) - static_cast<TUint>(EPanRoleU)) <= KMaxPanRoles, PanAgentPanic(EPanRoleOutOfBounds));
	__ASSERT_ALWAYS(((aRole==EPanRoleU)||(aRole==EPanRoleGn)||(aRole==EPanRoleNap)), PanAgentPanic(EAttemptToRegisterIllegalPanRoleInLocalSdpDatabase)); //<@note embeds knowledge of valid roles

	TInt arrayIndex = (static_cast<TUint>(aRole) - static_cast<TUint>(EPanRoleU));
	if(iLocalSdpRecordHandles[arrayIndex]!=KIllegalSdpRecordHandle)
		{
		iSdpDatabase.DeleteRecord(iLocalSdpRecordHandles[arrayIndex]);
		iLocalSdpRecordHandles[arrayIndex] = KIllegalSdpRecordHandle;
		}
	}
