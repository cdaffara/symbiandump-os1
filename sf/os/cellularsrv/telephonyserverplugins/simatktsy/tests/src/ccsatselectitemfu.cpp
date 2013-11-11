// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file The TEFUnit test suite for SelectItem in the SAT.
*/

#include "ccsatselectitemfu.h"
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <test/tmockltsydata.h>

#include <cstktsy/tsatutility.h>

const TUint8 KPCmdNumber = 1;
const TUint8 KSelItmQualifierEmpty = 0x00;

_LIT8(KToolkitSelectAlphaId, "Toolkit Select"); 
const TUint8 KNumItemsThree = 3;
const TUint8 KNumItemsFour = 4;
const TUint8 KItemIds[KNumItemsFour] = {1,2,3,4};
_LIT8(KItemName1, "Item 1");
_LIT8(KItemName2, "Item 2");
_LIT8(KItemName3, "Item 3");
_LIT8(KItemName4, "Item 4");
const TPtrC8 KItemNames[KNumItemsFour] = 
	{
		KItemName1(),
		KItemName2(),
		KItemName3(),
		KItemName4()
	};

CTestSuite* CCSatSelectItemFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001cL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001dL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001eL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001fL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001gL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001hL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001iL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001jL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001kL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0001lL);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSelectItemFU, TestNotifySelectItemPCmd0004L);

	END_SUITE;
	}


/**
Resets RSat::SelectItemVx data fields
@param aSelItmData data to be reset
*/
LOCAL_C void ResetSelectItemData(RSat::TMenuItemBaseV1& aSelItmData)
	{
	// reset public members
	switch( aSelItmData.ExtensionId() )
		{
		case RSat::KSatV2:
			(static_cast<RSat::TSelectItemV2*>(&aSelItmData))->iPreference = 
					RSat::ESelectionPreferenceNotSet;
			// there is no need in break
			// do the following for both cases
		case RSat::KSatV1:
		default:
			{
			RSat::TSelectItemV1& selItmDataV1( *static_cast<RSat::TSelectItemV1*>(&aSelItmData) );
			
			selItmDataV1.iAlphaId.iStatus = RSat::EAlphaIdNotSet;
			selItmDataV1.iHelp = RSat::EHelpNotSet;
			selItmDataV1.iDefaultItemId = 0;
			selItmDataV1.iIconId.iQualifier = RSat::EIconQualifierNotSet;
			selItmDataV1.iIconListQualifier = RSat::EIconQualifierNotSet;
			selItmDataV1.iPresentationType = RSat::EPresentationTypeNotSet;
			selItmDataV1.iAlphaId.iAlphaId.SetLength(0);
			}
			break;
		}

	// reset protected members;
	aSelItmData.SetPCmdNumber(0);
	aSelItmData.Reset();
	}


/**
Fills in a Tlv with command details and device identities for DisplayText proactive command
@param aTlv Tlv to fill in
@param aPCmdNumber proactive command number
@param aCommandQualifier DisplayText proactive command qualifier
*/
LOCAL_C void SelectItemProactiveCmdTlvBegin( 
		TTlv& aTlv, 
		TUint8 aPCmdNumber,
		TUint8 aCommandQualifier)
	{
	ProactiveCmdTlvBegin( aTlv, aPCmdNumber, KSelectItem, aCommandQualifier, KMe );
	}


/**
Prepares Select Item proactive command complete data for Mock
@param aPCmdNumber Proactive command number
@param aCmdQual Command qualifier
@param aAlphaIdStatus Alpha identifier status
@param aAlphaId Alpha identifier
@param aNumItems Number of menu items
@param aItemIds Array of menu items identifier
@param aItemNames Array of menu items names
@param aActionArr Array of next actions
@param aIconQual Type of icon
@param aIcon Icon address
@param aIconListQual Type of icons for menu items
@param aIconList Array of icon addresses for menu items
@param aDefaultItem Default item id
@param aResult Proactive command notify result
@param aDelay Delay value for MockLtsy completion
*/
void CCSatSelectItemFU::PrepareSelItemCompleteDataL(
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		RSat::TAlphaIdStatus aAlphaIdStatus,
		const TDesC8& aAlphaId,
		TUint8 aNumItems,
		const TUint8* aItemIds,
		const TPtrC8* aItemNames,
		const RArray<TUint32>* aActionArr,
		RSat::TIconQualifier aIconQual,
		TUint8 aIcon,
		RSat::TIconQualifier aIconListQual,
		const RArray<TUint32>* aIconList,
		TUint8 aDefaultItem,
		TInt aResult,
		TInt aDelay)
	{
	TTlv tlv;
	SelectItemProactiveCmdTlvBegin( tlv, aPCmdNumber, aCmdQual );

	if( (RSat::EAlphaIdProvided == aAlphaIdStatus) ||
			(RSat::EAlphaIdNull == aAlphaIdStatus))
		{			
		tlv.AddTag(KTlvAlphaIdentifierTag);
		tlv.AddData(aAlphaId);//ETLV_AlphaIdentifier
		}

	for( TInt i = 0; i < aNumItems; i++)
		{
		tlv.AddTag(KTlvItemTag);
		tlv.AddByte(aItemIds[i]); //item identifier
		tlv.AddData(aItemNames[i]);
		}

	if(aActionArr)
		{
		tlv.AddTag(KTlvItemsNextActionIndicatorTag);
				
		TInt count = aActionArr->Count();
		for( TInt i = 0; i < count; i++)
			{
			tlv.AddByte((*aActionArr)[i]); 
			}
		}
	
	if(RSat::EIconQualifierNotSet != aIconQual)
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual)?0:1);
		tlv.AddByte(aIcon);
		}

	if(RSat::EIconQualifierNotSet != aIconListQual)
		{
		tlv.AddTag(KTlvItemIconIdentifierListTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconListQual)?0:1);
		
		if ( aIconList )
			{
			TInt iconCount = aIconList->Count();
			for( TInt i = 0; i < iconCount; i++)
				{
				tlv.AddByte((*aIconList)[i]);
				}
			}
		}

	if( 0 < aDefaultItem )
		{
		tlv.AddTag(KTlvItemIdentifierTag);
		tlv.AddByte(aDefaultItem); // ETLV_IdentifierOfItemChosen
		}
	
	PrepareMockWithCompleteDataForPCmdNotifyL(tlv.End(), aResult, aDelay);
	}


/**
Prepares and calls Select Item proactive command
@param aPCmdNumber Proactive command number
@param aCmdQual Command qualifier
@param aParamVersion Parameter version
@param aAlphaIdStatus Alpha identifier status
@param aAlphaId Alpha identifier
@param aNumItems Number of menu items
@param aItemIds Array of menu items identifier
@param aItemNames Array of menu items names
@param aActionArr Array of next actions
@param aIconQual Type of icon
@param aIcon Icon address
@param aIconListQual Type of icons for menu items
@param aIconList Array of icon addresses for menu items
@param aDefaultItem Default item id
@param aResult Proactive command notify result
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
@param aItemNamesCheck Array of menu items names to ckeck with returned value
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmdL(
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		RSat::TSatVersion aParamVersion,
		RSat::TAlphaIdStatus aAlphaIdStatus,
		const TDesC8& aAlphaId,
		TUint8 aNumItems,
		const TUint8* const aItemIds,
		const TPtrC8* aItemNames,
		const RArray<TUint32>* aActionArr,
		RSat::TIconQualifier aIconQual,
		TUint8 aIcon,
		RSat::TIconQualifier aIconListQual,
		const RArray<TUint32>* aIconList,
		TUint8 aDefaultItem,
		TInt aResult,
		const TDesC16* aAlphaIdCheck,
		const TPtrC16* aItemNamesCheck)
	{
	PrepareMockWithExpDataForPCmdNotifyL(KSelectItem);
	
	PrepareSelItemCompleteDataL(
			aPCmdNumber, aCmdQual, aAlphaIdStatus, aAlphaId, aNumItems, aItemIds,
			aItemNames, aActionArr, aIconQual, aIcon, aIconListQual, aIconList,
			aDefaultItem, aResult);
	
	RSat::TMenuItemBaseV1* selItmData = NULL;
	TInt selItmDataSize(0);
	switch ( aParamVersion )
		{
		case RSat::KSatV1:
			{
			selItmData = new (ELeave) RSat::TSelectItemV1();
			selItmDataSize = sizeof(RSat::TSelectItemV1);
			}
			break;
		case RSat::KSatV2:
		default:
			{
			selItmData = new (ELeave) RSat::TSelectItemV2();
			selItmDataSize = sizeof(RSat::TSelectItemV2);
			aParamVersion = RSat::KSatV2;
			}
			break;
		}
	CleanupStack::PushL(selItmData);
	ResetSelectItemData(*selItmData);

	TRequestStatus requestStatus;
	TPtr8 selItmDataPckg(reinterpret_cast<TUint8*>(selItmData), selItmDataSize, selItmDataSize);
	iSat.NotifySelectItemPCmd(requestStatus, selItmDataPckg);
	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(aResult, requestStatus.Int());

	// check results
	const RSat::TSelectItemV1& selItmDataV1(
			*static_cast<RSat::TSelectItemV1*>(selItmData));
	if ( KErrNone == aResult )
		{
		ASSERT_EQUALS(aPCmdNumber, selItmDataV1.PCmdNumber());
		
		if ( RSat::KSatV1 != aParamVersion )
			{
			if ( KSelectItemSelectUsingSoftKeyMask == 
					(KSelectItemSelectUsingSoftKeyMask & aCmdQual) )
				{
				ASSERT_EQUALS( RSat::ESoftKeyPreferred,
						static_cast<RSat::TSelectItemV2*>(selItmData)->iPreference );
				}
			else
				{
				ASSERT_TRUE( RSat::ESoftKeyPreferred !=
						static_cast<RSat::TSelectItemV2*>(selItmData)->iPreference );
				}
			}
		
		if ( KHelpAvailabilityMask == (KHelpAvailabilityMask & aCmdQual) )
			{
			ASSERT_EQUALS(RSat::EHelpAvailable, selItmDataV1.iHelp);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoHelpAvailable, selItmDataV1.iHelp);
			}

		// Presentation as a choice of navigation options 
		if ( KSelectItemChoiceOfNavigation == 
				(KSelectItemPresentationTypeMask & aCmdQual) )
			{
			ASSERT_EQUALS(RSat::ENavigationOptions, selItmDataV1.iPresentationType);
			}
		// Presentation as a choice of data values 
		else if ( KSelectItemChoiceOfData == 
					(KSelectItemPresentationTypeMask & aCmdQual) )
			{
			ASSERT_EQUALS(RSat::EDataValues, selItmDataV1.iPresentationType);
			}
		else
			{
			ASSERT_TRUE( (RSat::ENotSpecified == selItmDataV1.iPresentationType) );
			}

		if(RSat::EIconQualifierNotSet != aIconQual)
			{
			ASSERT_EQUALS(aIconQual, selItmDataV1.iIconId.iQualifier);
			ASSERT_EQUALS(aIcon, selItmDataV1.iIconId.iIdentifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, selItmDataV1.iIconId.iQualifier);
			}
		
		if(RSat::EIconQualifierNotSet != aIconListQual)
			{
			ASSERT_EQUALS(aIconListQual, selItmDataV1.iIconListQualifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, selItmDataV1.iIconListQualifier);
			}

		ASSERT_EQUALS(static_cast<TUint8>(aDefaultItem), selItmDataV1.iDefaultItemId);

		if( RSat::EAlphaIdProvided == aAlphaIdStatus )
			{
			ASSERT_EQUALS(RSat::EAlphaIdProvided, selItmDataV1.iAlphaId.iStatus);
			if ( aAlphaIdCheck )
				{
				ASSERT_EQUALS( 0, aAlphaIdCheck->Compare(selItmDataV1.iAlphaId.iAlphaId));
				}
			else
				{
				ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(aAlphaId, selItmDataV1.iAlphaId.iAlphaId));
				}
			}
		else if ( RSat::EAlphaIdNull == aAlphaIdStatus )
			{
			ASSERT_TRUE( RSat::EAlphaIdProvided != selItmDataV1.iAlphaId.iStatus );
			}
		else
			{
			ASSERT_TRUE( (RSat::EAlphaIdNotPresent == selItmDataV1.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == selItmDataV1.iAlphaId.iStatus) );
			}

		ASSERT_EQUALS(static_cast<TUint>(aNumItems), selItmDataV1.NumberOfItems());
		
		if(0 < aNumItems)
			{
			ASSERT_TRUE( !selItmDataV1.IsRemoveMenu() );
			
			for(TInt i = 0; i < aNumItems; i++)
				{
				RSat::TItem item;
				TInt action;
				TInt iconId;

				ASSERT_EQUALS( KErrNone,
						selItmDataV1.GetItem( i + 1, item, action, iconId));
				
				ASSERT_EQUALS( aItemIds[i], item.iItemId );

				if ( aItemNamesCheck )
					{
					ASSERT_EQUALS( 0, aItemNamesCheck[i].Compare(item.iItemString) );
					}
				else
					{
					ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(aItemNames[i], item.iItemString));
					}
				
				if( aActionArr && (aNumItems == aActionArr->Count()) )
					{
					ASSERT_EQUALS(static_cast<TInt>((*aActionArr)[i]), action);
					}
				else
					{
					ASSERT_EQUALS(KErrNotFound, action);
					}
				
				if( (RSat::EIconQualifierNotSet != aIconListQual) && aIconList && ( aNumItems == aIconList->Count()) )
					{
					ASSERT_EQUALS(static_cast<TInt>((*aIconList)[i]), iconId);
					}
				else
					{
					ASSERT_EQUALS(KErrNotFound, iconId);
					}
				}
			}
		else
			{
			ASSERT_TRUE( selItmDataV1.IsRemoveMenu() );
			}
		}

	CleanupStack::PopAndDestroy(selItmData);
	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aSelItmCmdQualifier select item proactive command qualifier
@param aResp terminal response data
@param aExpResult expected result of TerminalRsp() execution 
*/
void CCSatSelectItemFU::GenerateSelItemTerminalResponseL(
		TUint8 aSelectItemPCmdQualifier,
		const RSat::TSelectItemRspV1& aResp,
		const TInt aExpResult)
	{

	TPtrC genResultAddInfo(KNullDesC);
	TPtrC8 additionalInfoTlv(KNullDesC8);
	if ( (RSat::KItemIdentifier == aResp.iInfoType) && 
			( (RSat::KHelpRequestedByUser == aResp.iGeneralResult) ||
					(CommandPerformedSuccessfully(aResp.iGeneralResult)) ) )
		{
		TTlv itmChosenTlv;
		itmChosenTlv.AddTag(KTlvItemIdentifierTag);
		// ETLV_IdentifierOfItemChosen
		itmChosenTlv.AddByte( static_cast<TUint8>(aResp.iAdditionalInfo[0]) );

		additionalInfoTlv.Set( itmChosenTlv.GetDataWithoutTopLevelTag() );
		}
	else if ( RSat::KMeProblem == aResp.iInfoType )
		{
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}

	RSat::TSelectItemRspV1Pckg respPckg(aResp);
	GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KSelectItem,
			aSelectItemPCmdQualifier,
			RSat::ESelectItem,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			additionalInfoTlv,
			aExpResult);
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySelectItemPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	PrepareSelItemCompleteDataL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSelectItem, KSelItmQualifierEmpty,
			addInfo, RSat::KMeUnableToProcessCmd);

	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySelectItemPCmd
 	//-------------------------------------------------------------------------
	// test for RSat::NotifySelectItemPCmd with RSat::TSelectItemV1 parameter version
 	//-------------------------------------------------------------------------

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV1, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02);
	resp.SetPCmdNumber(KPCmdNumber);
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// null alpha identifier
 	//-------------------------------------------------------------------------
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdNull, KNullDesC8,
			KNumItemsFour, KItemIds, KItemNames);
	
	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// no alpha identifier, large item, zero length item, screen busy
 	//-------------------------------------------------------------------------
	
	const TPtrC8 largeItemNames[KNumItemsFour] = {_L8("The SIM shall supply a set of item\
s from which the user may choose one. Each item comprises a short identifier (u\
sed to indicat"), KNullDesC8(), _L8("Item 3"), _L8("Item 4")};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdNotSet, KNullDesC8,
			KNumItemsFour, KItemIds, largeItemNames);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KScreenBusy);
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// screen busy, without additional info
 	//-------------------------------------------------------------------------
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames);

	//generate the terminal response
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp, KErrCorrupt );
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// command beyond phone capabilities
 	//-------------------------------------------------------------------------
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames);

	//generate the terminal response
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	resp.iInfoType = RSat::KNoAdditionalInfo;

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CSATTSY
 	//-------------------------------------------------------------------------

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrUnknown);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.1(ETSI TS 102 384), mandatory features
 	//-------------------------------------------------------------------------
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Expected Sequence 1.2(ETSI TS 102 384), large menu, successful
	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText2, "LargeMenu1"); 
	const TUint8 KNumItems2 = 0x1E; // 0x50 - 0x1E + 1 = 0x33
	TUint8 itemIds2[KNumItems2]; // = {0x50, 0x4F, ..., 0x33}
	for (TUint8 i = 0; i < KNumItems2; i++)
		{
		itemIds2[i]= 0x50 - i;
		}
	const TPtrC8 itemNames2[KNumItems2] = 
		{
			_L8("Zero"), _L8("One"), _L8("Two"), _L8("Three"), _L8("Four"),
			_L8("Five"), _L8("Six"), _L8("Seven"), _L8("Eight"), _L8("Nine"),
			_L8("Alpha"), _L8("Bravo"), _L8("Charlie"), _L8("Delta"), _L8("Echo"),
			_L8("Fox-trot"), _L8("Black"), _L8("Brown"), _L8("Red"), _L8("Orange"),
			_L8("Yellow"), _L8("Green"), _L8("Blue"), _L8("Violet"), _L8("Grey"),
			_L8("White"), _L8("milli"), _L8("micro"), _L8("nano"), _L8("pico")
		};
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText2,
			KNumItems2, itemIds2, itemNames2);
	
	//generate the terminal response
	resp.iAdditionalInfo.Zero();
	// Identifier of item chosen(0x3D) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x3D); // Identifier of item chosen
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Expected Sequence 1.3(ETSI TS 102 384), call options, successful
	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText3, "LargeMenu2"); 
	const TUint8 KNumItems3 = 7;
	const TUint8 itemIds3[KNumItems3] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9};
	const TPtrC8 itemNames3[KNumItems3] = 
		{
			_L8("Call Forwarding Unconditional"),
			_L8("Call Forwarding On User Busy"),
			_L8("Call Forwarding On No Reply"),
			_L8("Call Forwarding On User Not Reachable"),
			_L8("Barring Of All Outgoing Calls"),
			_L8("Barring Of All Outgoing International Calls"),
			_L8("CLI Presentation")
		};
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText3,
			KNumItems3, itemIds3, itemNames3);
	
	//generate the terminal response
	resp.iAdditionalInfo.Zero();
	// Identifier of item chosen(0xFB) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0xFB); // Identifier of item chosen
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Expected Sequence 1.4(ETSI TS 102 384), backward move by user, successful
	//-------------------------------------------------------------------------

	// 1.4.1
	_LIT8(KAlphaIdentifierText4, "Select Item"); 
	const TUint8 KNumItems4 = 2;
	const TUint8 itemIds4[KNumItems4] = {0x11, 0x12};
	const TPtrC8 itemNames4[KNumItems4] = {_L8("One"), _L8("Two")};
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText4,
			KNumItems4, itemIds4, itemNames4);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KBackwardModeRequestedByUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );

	// 1.4.2
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText4,
			KNumItems4, itemIds4, itemNames4);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Expected Sequence 1.5(ETSI TS 102 384), "Y", successful
	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText5, "The SIM shall supply a set of items from whic\
h the user may choose one. Each item comprises a short identifier (used to indi\
cate the selection) and a text string.Optionally the SIM may include an alpha i\
dentifier. The alpha identifier i"); 
	const TUint8 KNumItems5 = 1;
	TUint8 itemId5 = 0x01;
	TPtrC8 itemName5(_L8("Y"));
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText5,
			KNumItems5, &itemId5, &itemName5);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x01) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x01); // Identifier of item chosen
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001cL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.1(ETSI TS 102 384), next action support, successful
 	//-------------------------------------------------------------------------
	
	RArray<TUint32> actions(KNumItemsThree);
	CleanupClosePushL(actions);
	actions.Append(KSendShortMessage);
	actions.Append(KSetUpCall);
	actions.Append(KProvideLocalInformation);
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames, &actions);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// number of items in the next action list doesn't match the number of items in the menu
 	//-------------------------------------------------------------------------
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames, &actions);
	
	//generate the terminal response
	TTlv itmChosenTlv;
	itmChosenTlv.AddTag(KTlvItemIdentifierTag);
	itmChosenTlv.AddByte( static_cast<TUint8>(resp.iAdditionalInfo[0]) ); // ETLV_IdentifierOfItemChosen

	RSat::TSelectItemRspV1Pckg respPckg(resp);
	GenerateTerminalResponseL(
			KPCmdNumber, KSelectItem, KSelItmQualifierEmpty, RSat::ESelectItem,
			respPckg, KNullDesC, RSat::KPartialComprehension, itmChosenTlv.GetDataWithoutTopLevelTag());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// action list and icons
 	//-------------------------------------------------------------------------
	
	RSat::TIconQualifier iconQual = RSat::ENotSelfExplanatory;
	TUint8 icon = 1; // record 1 in EFIMG
	RSat::TIconQualifier iconListQual = RSat::ENotSelfExplanatory;

	RArray<TUint32> iconList(KNumItemsThree);
	CleanupClosePushL(iconList);
	 // record 5 in EFIMG for each item
	iconList.Append(5);
	iconList.Append(5);
	iconList.Append(5);

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames, &actions,
			iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // iconList, actions, this

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001dL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 3.1(ETSI TS 102 384), default item support, successful
 	//-------------------------------------------------------------------------
	
	const TUint8 KDefaultItem = 0x02;
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL,
			KDefaultItem);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x03) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x03); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001e
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001eL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 4.1(ETSI TS 102 384), help request support, successful
 	//-------------------------------------------------------------------------
	
	TUint8 selItmQualifier = KHelpAvailabilityMask;
	TestNotifySelectItemPCmdL(
			KPCmdNumber, selItmQualifier, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KHelpRequestedByUser;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x01) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x01); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( selItmQualifier, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001f
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001fL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//-------------------------------------------------------------------------
	// Expected Sequence 5.1(ETSI TS 102 384), icons support, successful
 	//-------------------------------------------------------------------------
	
	// 5.1A BASIC ICON NOT SELF EXPLANATORY
	RSat::TIconQualifier iconQual = RSat::ENotSelfExplanatory;
	TUint8 icon = 1; // record 1 in EFIMG
	RSat::TIconQualifier iconListQual = RSat::ENotSelfExplanatory;

	RArray<TUint32> iconList(KNumItemsThree);
	CleanupClosePushL(iconList);
	 // record 5 in EFIMG for each item
	iconList.Append(5);
	iconList.Append(5);
	iconList.Append(5);
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames,
			NULL, iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x01) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x01); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	// 5.1B BASIC ICON NOT SELF EXPLANATORY, requested icon could not be displayed
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames,
			NULL, iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 5.2(ETSI TS 102 384), icons support, successful
 	//-------------------------------------------------------------------------
	
	// 5.2A BASIC ICON SELF EXPLANATORY
	iconQual = RSat::ESelfExplanatory;
	iconListQual = RSat::ESelfExplanatory;
	
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames,
			NULL, iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	// 5.2B BASIC ICON SELF EXPLANATORY, requested icon could not be displayed
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames,
			NULL, iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// number of items in the icon list doesn't match the number of items in the menu
 	//-------------------------------------------------------------------------

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames, NULL,
			iconQual, icon, iconListQual, &iconList);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iAdditionalInfo.Zero();
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	
	TTlv itmChosenTlv;
	itmChosenTlv.AddTag(KTlvItemIdentifierTag);
	itmChosenTlv.AddByte( static_cast<TUint8>(resp.iAdditionalInfo[0]) ); // ETLV_IdentifierOfItemChosen

	RSat::TSelectItemRspV1Pckg respPckg(resp);
	GenerateTerminalResponseL(
			KPCmdNumber, KSelectItem, KSelItmQualifierEmpty, RSat::ESelectItem,
			respPckg, KNullDesC, RSat::KPartialComprehension, itmChosenTlv.GetDataWithoutTopLevelTag());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // iconList, this

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001g
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001gL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//-------------------------------------------------------------------------
	// Expected Sequence 6.1(ETSI TS 102 384),
	// PRESENTATION AS A CHOICE OF NAVIGATION OPTIONS, successful
 	//-------------------------------------------------------------------------
	
	TUint8 selItmQualifier = KSelectItemChoiceOfNavigation;
	TestNotifySelectItemPCmdL(
			KPCmdNumber, selItmQualifier, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x01) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x01); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSelItemTerminalResponseL( selItmQualifier, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 6.2(ETSI TS 102 384),
	// PRESENTATION AS A CHOICE OF DATA VALUES, successful
 	//-------------------------------------------------------------------------
	
	selItmQualifier = KSelectItemChoiceOfData;

	TestNotifySelectItemPCmdL(
			KPCmdNumber, selItmQualifier, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsThree, KItemIds, KItemNames);
	
	// generate the terminal response
	GenerateSelItemTerminalResponseL( selItmQualifier, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001h
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001hL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 7.1(ETSI TS 102 384), soft keys support, successful
 	//-------------------------------------------------------------------------
	
	const TUint8 KNumItems7 = 2;
	TUint8 selItmQualifier = KSelectItemSelectUsingSoftKeyMask;
	TestNotifySelectItemPCmdL(
			KPCmdNumber, selItmQualifier, RSat::KSatV2, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItems7, KItemIds, KItemNames);
	
	// generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x01) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x01); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( selItmQualifier, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001i
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001iL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 8.1(ETSI TS 102 384), Support of "No response from user"
 	//-------------------------------------------------------------------------
	
	_LIT8(KAlphaIdentifierText81,"<TIME-OUT>");
	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText81,
			KNumItemsThree, KItemIds, KItemNames);
	
	// generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KNoResponseFromUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001j
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001jL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 10.1(ETSI TS 102 384), 
	// UCS2 in Cyrillic characters, 0x80 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText101, // "Hello" in russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15");
	_LIT8(KItemName1011, // "Hello1" in russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x31");
	_LIT8(KItemName1012, // "Hello2" in russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x32");
	_LIT8(KItemName1013, // "Hello3" in russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x33");
	_LIT16(KAlphaIdentifierText10xU, // "Hello" in russian in unicode format for checking results
			"\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\x0423\x0419\x0422\x0415");
	const TPtrC8 itemNames101[KNumItemsThree] = {KItemName1011(), KItemName1012(), KItemName1013()};
	const TPtrC16 itemNames10xU[KNumItemsThree] =
		{ // data for checking results
		// "Hello1" in russian in unicode format
			_L16("\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\x0423\x0419\x0422\x0415\x0031"),
		// "Hello2" in russian in unicode format
			_L16("\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\x0423\x0419\x0422\x0415\x0032"),
		// "Hello3" in russian in unicode format
			_L16("\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\x0423\x0419\x0422\x0415\x0033")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText101,
			KNumItemsThree, KItemIds, itemNames101,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText10xU, itemNames10xU);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 10.2(ETSI TS 102 384), 
	// UCS2 in Cyrillic characters, 0x81 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText102, // "Hello" in russian, first byte "\x81" is 0x81 UCS2 coding type for Tlv data
			"\x81\x0C\x08\x97\x94\xA0\x90\x92\xA1\xA2\x92\xA3\x99\xA2\x95");
	const TPtrC8 itemNames102[KNumItemsThree] =
		{ // first bytes "\x81" is 0x81 UCS2 coding type for Tlv data
		// "Hello1" in russian
			_L8("\x81\x0D\x08\x97\x94\xA0\x90\x92\xA1\xA2\x92\xA3\x99\xA2\x95\x31"),
		// "Hello2" in russian
			_L8("\x81\x0D\x08\x97\x94\xA0\x90\x92\xA1\xA2\x92\xA3\x99\xA2\x95\x32"),
		// "Hello3" in russian
			_L8("\x81\x0D\x08\x97\x94\xA0\x90\x92\xA1\xA2\x92\xA3\x99\xA2\x95\x33")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText102,
			KNumItemsThree, KItemIds, itemNames102,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText10xU, itemNames10xU);

	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 10.3(ETSI TS 102 384), 
	// UCS2 in Cyrillic characters, 0x82 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText103, // "Hello" in russian, first byte "\x82" is 0x82 UCS2 coding type for Tlv data
			"\x82\x0C\x04\x10\x87\x84\x90\x80\x82\x91\x92\x82\x93\x89\x92\x85");
	const TPtrC8 itemNames103[KNumItemsThree] =
		{ // first bytes "\x82" is 0x82 UCS2 coding type for Tlv data
		// "Hello1" in russian
			_L8("\x82\x0D\x04\x10\x87\x84\x90\x80\x82\x91\x92\x82\x93\x89\x92\x85\x31"),
		// "Hello2" in russian
			_L8("\x82\x0D\x04\x10\x87\x84\x90\x80\x82\x91\x92\x82\x93\x89\x92\x85\x32"),
		// "Hello3" in russian
			_L8("\x82\x0D\x04\x10\x87\x84\x90\x80\x82\x91\x92\x82\x93\x89\x92\x85\x33")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText103,
			KNumItemsThree, KItemIds, itemNames103,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText10xU, itemNames10xU);

	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001k
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001kL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 11.1(ETSI TS 102 384), UCS2 in Chinese Characters, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText111, // "Toolkit Select" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x5D\xE5\x51\x77\x7B\xB1\x90\x09\x62\xE9");
	_LIT8(KItemName111, // "Item 1" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x98\x79\x76\xEE\x4E\x00");
	_LIT8(KItemName112, // "Item 2" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x98\x79\x76\xEE\x4E\x8C");
	_LIT8(KItemName113, // "Item 3" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x98\x79\x76\xEE\x4E\x09");
	_LIT8(KItemName114, // "Item 4" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x98\x79\x76\xEE\x56\xDB");
	_LIT16(KAlphaIdentifierText11U, // "Toolkit Select" in Chinese in unicode format for checking results
			"\x5DE5\x5177\x7BB1\x9009\x62E9");
	const TPtrC8 itemNames111[KNumItemsFour] = {KItemName111(), KItemName112(), KItemName113(), KItemName114()};
	const TPtrC16 itemNames11U[KNumItemsFour] =
		{ // data for checking results
		// "Item 1" in Chinese in unicode format
			_L16("\x9879\x76EE\x4E00"),
		// "Item 2" in Chinese in unicode format
			_L16("\x9879\x76EE\x4E8C"),
		// "Item 3" in Chinese in unicode format
			_L16("\x9879\x76EE\x4E09"),
		// "Item 4" in Chinese in unicode format
			_L16("\x9879\x76EE\x56DB")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText111,
			KNumItemsFour, KItemIds, itemNames111,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText11U, itemNames11U);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0001l
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0001lL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 12.1(ETSI TS 102 384), 
	// UCS2 in Katakana characters, 0x80 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText121, // Use characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x00\x38\x00\x30\x30\xEB\x00\x30");
	_LIT8(KItemName1211, // Use characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x00\x38\x00\x30\x30\xEB\x00\x31");
	_LIT8(KItemName1212, // Use characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x00\x38\x00\x30\x30\xEB\x00\x32");
	_LIT8(KItemName1213, // Use characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
			"\x80\x00\x38\x00\x30\x30\xEB\x00\x33");
	const TPtrC8 itemNames121[KNumItemsThree] = {KItemName1211(), KItemName1212(), KItemName1213()};
	_LIT16(KAlphaIdentifierText121U, // Katakana in unicode format for checking results
			"\x0038\x0030\x30EB\x0030");
	const TPtrC16 itemNames121U[KNumItemsThree] =
		{ // Katakana in unicode format for checking results
			_L16("\x0038\x0030\x30EB\x0031"),
			_L16("\x0038\x0030\x30EB\x0032"),
			_L16("\x0038\x0030\x30EB\x0033")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText121,
			KNumItemsThree, KItemIds, itemNames121,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText121U, itemNames121U);
	
	//generate the terminal response
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02); // Identifier of item chosen
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 12.2(ETSI TS 102 384), 
	// UCS2 in Cyrillic characters, 0x81 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText122, // Use characters in Katakana, first byte "\x81" is 0x81 UCS2 coding type for Tlv data
			"\x81\x04\x61\x38\x31\xEB\x30");
	const TPtrC8 itemNames122[KNumItemsThree] =
		{ // Use characters in Katakana, first bytes "\x81" is 0x81 UCS2 coding type for Tlv data
			_L8("\x81\x04\x61\x38\x31\xEB\x31"),
			_L8("\x81\x04\x61\x38\x31\xEB\x32"),
			_L8("\x81\x04\x61\x38\x31\xEB\x33")
		};
	_LIT16(KAlphaIdentifierText122U, // Katakana in unicode format for checking results
			"\x0038\x0031\x30EB\x0030");
	const TPtrC16 itemNames122U[KNumItemsThree] =
		{ // Katakana in unicode format for checking results
			_L16("\x0038\x0031\x30EB\x0031"),
			_L16("\x0038\x0031\x30EB\x0032"),
			_L16("\x0038\x0031\x30EB\x0033")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText122,
			KNumItemsThree, KItemIds, itemNames122,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText122U, itemNames122U);

	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 12.3(ETSI TS 102 384), 
	// UCS2 in Cyrillic characters, 0x82 UCS2 coding, successful
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaIdentifierText123, // Use characters in Katakana, first byte "\x82" is 0x82 UCS2 coding type for Tlv data
			"\x82\x04\x30\xA0\x38\x32\xCB\x30");
	const TPtrC8 itemNames123[KNumItemsThree] =
		{ // Use characters in Katakana, first bytes "\x82" is 0x82 UCS2 coding type for Tlv data
			_L8("\x82\x04\x30\xA0\x38\x32\xCB\x31"),
			_L8("\x82\x04\x30\xA0\x38\x32\xCB\x32"),
			_L8("\x82\x04\x30\xA0\x38\x32\xCB\x33")
		};
	_LIT16(KAlphaIdentifierText123U, // Katakana in unicode format for checking results
			"\x0038\x0032\x30EB\x0030");
	const TPtrC16 itemNames123U[KNumItemsThree] =
		{ // Katakana in unicode format for checking results
			_L16("\x0038\x0032\x30EB\x0031"),
			_L16("\x0038\x0032\x30EB\x0032"),
			_L16("\x0038\x0032\x30EB\x0033")
		};

	TestNotifySelectItemPCmdL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::KSatV2, RSat::EAlphaIdProvided, KAlphaIdentifierText123,
			KNumItemsThree, KItemIds, itemNames123,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, &KAlphaIdentifierText123U, itemNames123U);

	//generate the terminal response
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySelectItemPCmd
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSelectItem);
	
	PrepareSelItemCompleteDataL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::EAlphaIdProvided, KToolkitSelectAlphaId,
			KNumItemsFour, KItemIds, KItemNames,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, 10);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSelectItem, KSelItmQualifierEmpty,
			addInfo, RSat::KMeUnableToProcessCmd);

	RSat::TSelectItemV2 selItmData;
	ResetSelectItemData(selItmData);
	RSat::TSelectItemV2Pckg selItmDataPck(selItmData);
	TRequestStatus requestStatus;
	
	iSat.NotifySelectItemPCmd(requestStatus, selItmDataPck);
	
	iSat.CancelAsyncRequest(ESatNotifySelectItemPCmd);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SI-SNSIPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySelectItemPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySelectItemPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSelectItemFU::TestNotifySelectItemPCmd0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RSat sat2;
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	// prepare proactive command data
	_LIT8(KAlphaIdentifierText, "Toolkit Select 1"); 
	_LIT16(KAlphaIdentifierText16, "Toolkit Select 1"); 

	// prepare Test A data
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RSat::TSelectItemV2 selItmData;
	RSat::TSelectItemV2 selItmData2;
	RSat::TSelectItemV2Pckg selItmDataPckg(selItmData);
	RSat::TSelectItemV2Pckg selItmDataPckg2(selItmData2);

	ResetSelectItemData(selItmData);
	ResetSelectItemData(selItmData2);

	// prepare Test B data
	RSat::TSelectItemV1 selItmDataV1;
	RSat::TSelectItemV1Pckg selItmDataV1Pckg(selItmDataV1);
	ResetSelectItemData(selItmDataV1);

	// prepare terminal response data
	RSat::TSelectItemRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KItemIdentifier;
	// Identifier of item chosen(0x02) is passed as additional info
	// and this value is sufficient because we set information type value as
	// item identifier(RSat::KItemIdentifier)
	resp.iAdditionalInfo.Append(0x02);
	resp.SetPCmdNumber(KPCmdNumber);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySelectItemPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// prepare Mock data
	PrepareMockWithExpDataForPCmdNotifyL(KSelectItem);
	PrepareSelItemCompleteDataL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::EAlphaIdProvided, KAlphaIdentifierText,
			KNumItemsFour, KItemIds, KItemNames,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, 10);

	// send requests
	iSat.NotifySelectItemPCmd(requestStatus, selItmDataPckg);
	sat2.NotifySelectItemPCmd(requestStatus2, selItmDataPckg2);
	
	// wait for completion
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	const TUint8 KNumClients = 2;
	const RSat::TSelectItemV2* selItmDataArr[KNumClients] = { &selItmData, &selItmData2};
	for(TUint8 clientNo = 0; clientNo < KNumClients; clientNo++)
		{
		const RSat::TSelectItemV2& selItmData( *(selItmDataArr[clientNo]) );
		
		ASSERT_EQUALS( KPCmdNumber, selItmData.PCmdNumber() );
		ASSERT_TRUE( RSat::ESoftKeyPreferred != selItmData.iPreference );
		ASSERT_EQUALS( RSat::ENoHelpAvailable, selItmData.iHelp );
		ASSERT_TRUE( ( RSat::ENotSpecified == selItmData.iPresentationType ) ||
				( RSat::EPresentationTypeNotSet == selItmData.iPresentationType ) );
		ASSERT_EQUALS( RSat::ENoIconId, selItmData.iIconId.iQualifier );
		ASSERT_EQUALS( RSat::ENoIconId, selItmData.iIconListQualifier );
		ASSERT_EQUALS( static_cast<TUint8>(0), selItmData.iDefaultItemId );
		ASSERT_EQUALS( 0, KAlphaIdentifierText16().Compare(selItmData.iAlphaId.iAlphaId) );
		ASSERT_EQUALS( RSat::EAlphaIdProvided, selItmData.iAlphaId.iStatus );
		ASSERT_TRUE( !selItmData.IsRemoveMenu() );
		ASSERT_EQUALS( static_cast<TUint>(KNumItemsFour), selItmData.NumberOfItems() );
		for(TInt i = 0; i < KNumItemsFour; i++)
			{
			RSat::TItem item;
			TInt action;
			TInt iconId;
	
			ASSERT_EQUALS( KErrNone, selItmData.GetItem( i + 1, item, action, iconId ) );
			ASSERT_EQUALS( KItemIds[i], item.iItemId );
			ASSERT_EQUALS( 0, Compare16bitDesWith8bitDesL(KItemNames[i], item.iItemString) );
			ASSERT_EQUALS( KErrNotFound, action );
			ASSERT_EQUALS( KErrNotFound, iconId );
			}
		}

	AssertMockLtsyStatusL();
	
	//generate terminal responses
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp);
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifySelectItemPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	// When multiple clients requesting RSat::NotifySelectItemPCmd and they both pass
	// different packaged parameter versions only the last request completes, because
	// etel reposts request with new parameter size and looses previous requests.
	// May be it would be worth to make ESatNotifySelectItemPCmd ipc requests flow controlled
	// by CSATTSY instead of multiple completed by ETEL.
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 51);
	ASSERT_TRUE( EFalse );

	// send requests
	PrepareMockWithExpDataForPCmdNotifyL(KSelectItem);
	PrepareSelItemCompleteDataL(
			KPCmdNumber, KSelItmQualifierEmpty, RSat::EAlphaIdProvided, KAlphaIdentifierText,
			KNumItemsFour, KItemIds, KItemNames,
			NULL, RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 0,
			KErrNone, 10);

	ResetSelectItemData(selItmData);
	iSat.NotifySelectItemPCmd(requestStatus, selItmDataPckg);
	sat2.NotifySelectItemPCmd(requestStatus2, selItmDataV1Pckg);

	// wait for completion
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	// check results general for V2 and V1 parameter versions
	const RSat::TSelectItemV1* selItmDataV1Arr[KNumClients] = { &selItmData, &selItmDataV1};
	for(TInt clientNo = 0; clientNo < KNumClients; clientNo++)
		{
		const RSat::TSelectItemV1& selItmData( *(selItmDataV1Arr[clientNo]) );
		
		ASSERT_EQUALS( KPCmdNumber, selItmData.PCmdNumber() );
		ASSERT_EQUALS( RSat::ENoHelpAvailable, selItmData.iHelp );
		ASSERT_TRUE( ( RSat::ENotSpecified == selItmData.iPresentationType ) ||
				( RSat::EPresentationTypeNotSet == selItmData.iPresentationType ) );
		ASSERT_EQUALS( RSat::ENoIconId, selItmData.iIconId.iQualifier );
		ASSERT_EQUALS( RSat::ENoIconId, selItmData.iIconListQualifier );
		ASSERT_EQUALS( static_cast<TUint8>(0), selItmData.iDefaultItemId );
		ASSERT_EQUALS( 0, KAlphaIdentifierText16().Compare(selItmData.iAlphaId.iAlphaId) );
		ASSERT_EQUALS( RSat::EAlphaIdProvided, selItmData.iAlphaId.iStatus );
		ASSERT_TRUE( !selItmData.IsRemoveMenu() );
		ASSERT_EQUALS( static_cast<TUint>(KNumItemsFour), selItmData.NumberOfItems() );
		for(TInt i = 0; i < KNumItemsFour; i++)
			{
			RSat::TItem item;
			TInt action;
			TInt iconId;
	
			ASSERT_EQUALS( KErrNone, selItmData.GetItem( i + 1, item, action, iconId ) );
			ASSERT_EQUALS( KItemIds[i], item.iItemId );
			ASSERT_EQUALS( 0, Compare16bitDesWith8bitDesL(KItemNames[i], item.iItemString) );
			ASSERT_EQUALS( KErrNotFound, action );
			ASSERT_EQUALS( KErrNotFound, iconId );
			}
		}

	// check results of V2 parameter version
	ASSERT_TRUE( RSat::ESoftKeyPreferred != selItmData.iPreference );

	AssertMockLtsyStatusL();
	
	//generate terminal responses
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	GenerateSelItemTerminalResponseL( KSelItmQualifierEmpty, resp );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


