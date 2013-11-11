
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
 @file The TEFUnit test suite for SatControl in the Common TSY.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <et_clsvr.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/ltsy/sat_defs.h>
#include <cstktsy/ttlv.h>

#include "cctsysatfu.h"
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h> //for KLtsyDispatchSmsSendSatSmsApiId
#include <ctsy/ltsy/mltsydispatchphoneinterface.h> //for KLtsyDispatchPhoneGetPhoneIdApiId
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h> //for KLtsyDispatchPhonebookSmsStoreWriteEntryApiId
#include "mockltsyindicatorids.h"


#include "testconstants.h"
#include <test/tmockltsydata.h>


const TDesC8& KGeneralResultAddInfo = KNullDesC8;

const TInt KShiftBitsToGetMSB = 8;
const TInt KShiftBitsToGetLSB = 0;


//const TUint8 KAddrMaxLength 	= 255;
//const TUint8 KFilePathLength	= 8;

CTestSuite* CCTsySatFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUseCase0010L);

	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFU, TestUnit0017L);

	END_SUITE;
	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test CTSY/STK initialisation
@SYMTestPriority High
@SYMTestActions Invokes RSat::Open()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	//OpenSatL contains the test code that drives the initialisation of SAT.
	OpenSatL(KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled); 
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for a representative Proactive Command session involving user interaction
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd, RSat::NotifyDisplayTextPCmd, RSat::NotifySelectItemPCmd,RSat::TerminalRsp, RSat::MenuSelection
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
		
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	const TUint8 KPCmdNumber1 = 123;
	const TUint8 KPCmdNumber2 = 124;
	const TUint8 KPCmdNumber3 = 125;
	
	
	TUint8 pcmdCode = KSetUpMenu;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);

	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber1);//ETLV_CommandNumber
	tlv.AddByte(KSetUpMenu); //ETLV_TypeOfCommand
	tlv.AddByte(KSelectionUsingSoftKey); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAlphaIdentifierTag);
	_LIT8(KSetupMenuTest8,"Menu AlphaIdent");
	_LIT16(KSetupMenuTest16,"Menu AlphaIdent");
	tlv.AddData(KSetupMenuTest8);//ETLV_AlphaIdentifier

	const TUint8 KItemIdentifier1 = 77;
	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(KItemIdentifier1); //item identifier
	tlv.AddData(_L8("Item AlphaIdent1"));
	
	const TUint8 KItemIdentifier2 = 78;
	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(KItemIdentifier2); //item identifier
	tlv.AddData(_L8("Item AlphaIdent2"));
		
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	
	RSat::TSetUpMenuV2 setupMenu;
	RSat::TSetUpMenuV2Pckg setpPck(setupMenu);
	iSat.NotifySetUpMenuPCmd(reqStatus, setpPck);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(KPCmdNumber1, setupMenu.PCmdNumber());
	ASSERT_EQUALS(RSat::EAlphaIdProvided, setupMenu.iAlphaId.iStatus);
	ASSERT_EQUALS(0, setupMenu.iAlphaId.iAlphaId.Compare(KSetupMenuTest16));
	AssertMockLtsyStatusL();
	data.Close();

	//generate the terminal response
	RSat::TSetUpMenuRspV1 setupMenuResp;
	RSat::TSetUpMenuRspV1Pckg setupMenuRespPckg(setupMenuResp);
	
	setupMenuResp.iGeneralResult = RSat::KSuccess;
	setupMenuResp.iInfoType = RSat::KNoAdditionalInfo;
	setupMenuResp.SetPCmdNumber(KPCmdNumber1);

	GenerateAndWaitForTermRspL(KPCmdNumber1,KSetUpMenu,KSelectionUsingSoftKey,RSat::ESetUpMenu, RSat::KSuccess, setupMenuRespPckg, KErrNone);
	
	//simulate the user requesting help on a menu option.
	
	TTlv menuSelectionEnvelope;
	menuSelectionEnvelope.Begin( KBerTlvMenuSelectionTag );
	menuSelectionEnvelope.AddTag( KTlvDeviceIdentityTag );
	menuSelectionEnvelope.AddByte( KKeypad );
	menuSelectionEnvelope.AddByte( KSim );
	menuSelectionEnvelope.AddTag( KTlvItemIdentifierTag );
	menuSelectionEnvelope.AddByte( KItemIdentifier1 );
	menuSelectionEnvelope.AddTag( KTlvHelpRequestTag );

	TDesC8* menuSelectionEnvelopePtr = &const_cast<TDesC8&>(menuSelectionEnvelope.End());
	TMockLtsyData1<TDesC8*> menuSelDescPck(menuSelectionEnvelopePtr);
	data.Close();
	menuSelDescPck.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId, data);

	TRequestStatus reqStatusNotifyTerminated;
	iMockLTSY.NotifyTerminated(reqStatusNotifyTerminated); 
	
	RSat::TMenuSelectionV1 menuSelection;
	RSat::TMenuSelectionV1Pckg menuSelectionPckg(menuSelection);
	menuSelection.iItemId = KItemIdentifier1;
	menuSelection.iHelp = RSat::EHelpRequested;

	iSat.MenuSelection(reqStatus, menuSelectionPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	User::WaitForRequest(reqStatusNotifyTerminated);
	ASSERT_EQUALS(reqStatusNotifyTerminated.Int(),KErrNone);
 

	//response from ucc to the help request
	pcmdCode = KDisplayText;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);


	RSat::TDisplayTextV2 displayText;
	RSat::TDisplayTextV2Pckg displayTextPckg(displayText);
	iSat.NotifyDisplayTextPCmd(reqStatus, displayTextPckg);

	TTlv displayTextTlv;
	displayTextTlv.Begin(KBerTlvProactiveSimCommandTag);
	displayTextTlv.AddTag(KTlvCommandDetailsTag);
	displayTextTlv.AddByte(KPCmdNumber2);//ETLV_CommandNumber
	displayTextTlv.AddByte(KDisplayText); //ETLV_TypeOfCommand
	TUint8 dispTextMask = KMessageClearMask & ~KPriorityMask; //user must acknowledge the message. normal priority.
	displayTextTlv.AddByte(dispTextMask); //ETLV_CommandQualifier
	
	displayTextTlv.AddTag(KTlvDeviceIdentityTag); 
	displayTextTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	displayTextTlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity
	
	displayTextTlv.AddTag(KTlvTextStringTag);
	displayTextTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTstHelpInfo8, "HELP INFORMATION ABOUT MENU COMMAND");//defining this string twice because its returned 
	_LIT(KTstHelpInfo16, "HELP INFORMATION ABOUT MENU COMMAND");//via NotifyDisplayTextPcmd completion as 16 bit desc
	displayTextTlv.AddData(KTstHelpInfo8);
	
	displayTextTlv.AddTag(KTlvImmediateResponseTag);
		
	TDesC8* displayTextDscPtr = &const_cast<TDesC8&>(displayTextTlv.End());
	
	TMockLtsyData1<TDesC8*> displayTextPack(displayTextDscPtr);
	data.Close();
	displayTextPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);


	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(displayText.PCmdNumber(),KPCmdNumber2);
	ASSERT_TRUE(displayText.iText == KTstHelpInfo16);
	AssertMockLtsyStatusL();
	
	
	//generate the terminal response
	RSat::TDisplayTextRspV1 displayTextRsp;
	RSat::TDisplayTextRspV1Pckg displayTextRspPckg(displayTextRsp);
	
	displayTextRsp.iGeneralResult = RSat::KSuccess;
	displayTextRsp.iInfoType = RSat::KNoAdditionalInfo;
	displayTextRsp.SetPCmdNumber(KPCmdNumber2);

	GenerateAndWaitForTermRspL(KPCmdNumber2,KDisplayText,dispTextMask,RSat::EDisplayText, RSat::KSuccess, displayTextRspPckg, KErrNone);

	
	//now let the user select a top level item which will open a submenu.
	
	menuSelectionEnvelope.Begin( KBerTlvMenuSelectionTag );
	menuSelectionEnvelope.AddTag( KTlvDeviceIdentityTag );
	menuSelectionEnvelope.AddByte( KKeypad );
	menuSelectionEnvelope.AddByte( KSim );
	menuSelectionEnvelope.AddTag( KTlvItemIdentifierTag );
	menuSelectionEnvelope.AddByte( KItemIdentifier1 );

	menuSelectionEnvelopePtr = &const_cast<TDesC8&>(menuSelectionEnvelope.End());
	data.Close();
	menuSelDescPck.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId, data);
	
	menuSelection.iItemId = KItemIdentifier1;
	menuSelection.iHelp = RSat::EHelpNotRequested;

	iMockLTSY.NotifyTerminated(reqStatusNotifyTerminated); 
	
	iSat.MenuSelection(reqStatus, menuSelectionPckg);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	User::WaitForRequest(reqStatusNotifyTerminated);
	ASSERT_EQUALS(reqStatusNotifyTerminated.Int(),KErrNone);

	
    
    //now provide the sub menu
	
	pcmdCode = KSelectItem;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);

	
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber3);//ETLV_CommandNumber
	tlv.AddByte(KSelectItem); //ETLV_TypeOfCommand
	tlv.AddByte(KSelectionUsingSoftKey); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAlphaIdentifierTag);
	_LIT8(KSelectItemTestMenu8,"select item Menu AlphaIdent");
	_LIT16(KSelectItemTestMenu16,"select item Menu AlphaIdent");
	tlv.AddData(KSelectItemTestMenu8);//ETLV_AlphaIdentifier
	
	const TUint8 KSubMenuItemIdentifier1 = 90;
	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(KSubMenuItemIdentifier1); //item identifier
	tlv.AddData(_L8("select Item AlphaIdent1"));
	
	const TUint8 KSumMenuItemIdentifier2 = 91;
	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(KSumMenuItemIdentifier2); //item identifier
	tlv.AddData(_L8("select Item AlphaIdent2"));

	tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	
	RSat::TSelectItemV5 selectItem;
	RSat::TSelectItemV5Pckg SelectItemPck(selectItem);
	iSat.NotifySelectItemPCmd(reqStatus, SelectItemPck);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(selectItem.PCmdNumber(),KPCmdNumber3);
	ASSERT_EQUALS(selectItem.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(selectItem.iAlphaId.iAlphaId == KSelectItemTestMenu16);
    
	//generate the terminal response
	RSat::TSelectItemRspV1 selectItemRsp;
	RSat::TSelectItemRspV1Pckg selectItemRspPckg(selectItemRsp);
		
	selectItemRsp.iGeneralResult = RSat::KSuccess;
	selectItemRsp.iInfoType = RSat::KItemIdentifier;
	selectItemRsp.iAdditionalInfo.Append(KSubMenuItemIdentifier1);
	selectItemRsp.SetPCmdNumber(KPCmdNumber3);

	
	TTlv tlvAddInfo;
	tlvAddInfo.AddTag(KTlvItemIdentifierTag);
	tlvAddInfo.AddByte(KSubMenuItemIdentifier1);
	TPtrC8 addInfo = tlvAddInfo.GetDataWithoutTopLevelTag();
	
	GenerateAndWaitForTermRspL(	KPCmdNumber3,
								KSelectItem,
								KSelectionUsingSoftKey,
								RSat::ESelectItem,selectItemRsp.iGeneralResult,
								KGeneralResultAddInfo,
								selectItemRspPckg,
								addInfo,
								KErrNone);
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}	
	

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Getting and Icon Ef(Img) Record, Icon instance and CLUT
@SYMTestPriority High
@SYMTestActions Invokes RSat::GetIcon , RSat::GetImageInstance, Rsat::GetClut
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	RBuf8 getData;
	CleanupClosePushL(getData);
	
	RBuf8 retData;
	CleanupClosePushL(retData);
	
	TRequestStatus reqStatus;

	/******************************************Get Icon*******************************************************/
	
	//Test to get a given Icons EF(Img) record
	
	//re-create the file path
    TBuf8<KFilePathLength> simFilePath(KFilePathLength);
    simFilePath[0] = 0x3F; //MF High byte
    simFilePath[1] = 0x00; //MF Low byte
    simFilePath[2] = 0x7F; //DF(Telecom) High byte
    simFilePath[3] = 0x10; //DF(Telecom) Low byte
    simFilePath[4] = 0x5F; //DF(Graphics) High byte
    simFilePath[5] = 0x50; //DF(Graphics) Low byte
    simFilePath[6] = 0x4F; //EF(Img) High byte
    simFilePath[7] = 0x20; //EF(Img) Low byte
    
    //a EF(Img) record to get
    const TUint8 KRecordNumber = 5;
        
    //create the down expect data
    TUint8 recordNumber = KRecordNumber;
    TDesC8* simFilePathPtr = &simFilePath;
    TMockLtsyData2<TUint8,TDesC8*> getIconData(recordNumber,simFilePathPtr);
    getIconData.SerialiseL(getData);

    //create a EF(Img) record    
    const TUint KNoImagesLength = 1;
    const TUint KEfImgRecordLength = 9;
    
    RBuf8 iconEfRecordInstance;
    CleanupClosePushL(iconEfRecordInstance);
    iconEfRecordInstance.CreateMaxL(KEfImgRecordLength);
    
    const TUint8 KNumberOfImages = 1;
    
    const TUint16 KInstanceNumber = 1;
    const TUint16 KOffset = 1;
    const TUint16 KLength = 1;

    const TUint8 KWidth  = 40;
    const TUint8 KHeight = 40;
    
    iconEfRecordInstance[0] = KWidth; 								//image instance width, expressed in raster image points
    iconEfRecordInstance[1] = KHeight; 								//image instance height, expressed in raster image points
    iconEfRecordInstance[2] = RSat::KBasic;							//image coding scheme that has been used in encoding the image instance 11-basic, 21-colour 
    iconEfRecordInstance[3] = KInstanceNumber >> KShiftBitsToGetMSB; 	//high byte of Image Instance File Identifier
    iconEfRecordInstance[4] = KInstanceNumber & 0xFF; 				//low byte of Image Instance File Identifier.
    iconEfRecordInstance[5] = KOffset >> KShiftBitsToGetMSB;				//high byte of offset into Image Instance File
    iconEfRecordInstance[6] = KOffset & 0xFF; 						//low byte of offset into Image Instance File
    iconEfRecordInstance[7] = KLength >> KShiftBitsToGetMSB;				//high byte of  Image Instance Data length
    iconEfRecordInstance[8] = KLength & 0xFF;						//low byte of Image Instance Data length
                     
    RBuf8 iconEfImgData;
    CleanupClosePushL(iconEfImgData);
    iconEfImgData.CreateL(KNoImagesLength + KEfImgRecordLength);
    iconEfImgData.Append(KNumberOfImages);
    iconEfImgData.Append(iconEfRecordInstance);
    
    
    //create the up complete data
    TDesC8* iconDataPtr = &iconEfImgData;
    TMockLtsyData1<TDesC8*> retIconData(iconDataPtr);
    retIconData.SerialiseL(retData);
    
    //queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId, KErrNone, retData);
	
	// ETel Sat Calls
	RSat::TIcon retrievedIconEfImg;
    RSat::TIconId iconId;
    iconId.iIdentifier = KRecordNumber;
    
	iSat.GetIcon(reqStatus,iconId.iIdentifier,retrievedIconEfImg); 
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(iconEfImgData == retrievedIconEfImg);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();
	
	CleanupStack::PopAndDestroy(&iconEfImgData);
	CleanupStack::PopAndDestroy(&iconEfRecordInstance);
	
	/*************************************************************************************************/
	
	/******************************************Get Image Instance*******************************************************/

	// Test to get a given instance of an icons EF(IIDF)
	
	// The Client should have read the icon EF before reading the image instance.
	// The contents of the icon EF indicate the size of the image instance,
	// the Client should use this information to allocate a buffer size that it requires
	// and pass a reference to is as a TDes8&
	
    //create the down expect data
	TUint8 imageInstanceNumberInRecord = KNumberOfImages - KInstanceNumber + 1;
	TMockLtsyData2<TUint8,TUint8> getImageInstanceData(recordNumber,imageInstanceNumberInRecord);
	getImageInstanceData.SerialiseL(getData);
	
    RBuf8 imageInstanceData;
    CleanupClosePushL(imageInstanceData);
    
    RBuf8 imageInstanceBody;
    CleanupClosePushL(imageInstanceBody);
	
    //A basic image data
    const TUint KIconDataLength = 2;
    imageInstanceData.CreateMaxL(KIconDataLength);
    imageInstanceData[0] = retrievedIconEfImg[KNoImagesLength + 1]; //i.e. KWidth;
    imageInstanceData[1] = retrievedIconEfImg[KNoImagesLength + 2]; //i.e. KHeight;
    
    //The image body
    imageInstanceBody.CreateMaxL(KWidth * KHeight);
    
    //fill with any data (just for testing)
    for(TInt i = 0; i < imageInstanceBody.Length(); ++i)
    	{
    	imageInstanceBody[i] = i;
    	}
    
    TDesC8* imageInstanceDataPtr = &imageInstanceData;
    TDesC8* imageInstanceBodyPtr = &imageInstanceBody;
    
    TMockLtsyData2<TDesC8*,TDesC8*> retImageInstanceData(imageInstanceDataPtr,imageInstanceBodyPtr);
    retImageInstanceData.SerialiseL(retData);
	
    //queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId, KErrNone, retData);
	
	//this is used in the CTSY to check against the retrieved icon EF(Img) record
	RSat::TInstanceInfoV3 instanceInfo;
	instanceInfo.iInstanceNumber	= KInstanceNumber;
	instanceInfo.iLength			= KLength;
	instanceInfo.iOffset			= KOffset;
	instanceInfo.iCoding			= RSat::KBasic;
	instanceInfo.iImgWidth			= KWidth;
	instanceInfo.iImgHeight			= KHeight;
	instanceInfo.iFileId			= 0; //not used in the CTSY
 
	RSat::TInstanceInfoV3Pckg instanceInfoPackage1(instanceInfo);
	
	RBuf8 retrievedImageinstance;
	CleanupClosePushL(retrievedImageinstance);
	retrievedImageinstance.CreateL(KIconDataLength + (KWidth * KHeight)); //the EF(IIDF)
	
	// ETel Sat Calls
	iSat.GetImageInstance(reqStatus, instanceInfoPackage1, retrievedImageinstance);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(retrievedImageinstance.Mid(0,KIconDataLength) == imageInstanceData);
	ASSERT_TRUE(retrievedImageinstance.Mid(KIconDataLength) == imageInstanceBody);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();

	CleanupStack::PopAndDestroy(&retrievedImageinstance);
	CleanupStack::PopAndDestroy(&imageInstanceBody);
	CleanupStack::PopAndDestroy(&imageInstanceData);
	
	/*************************************************************************************************/

	/******************************************Get Image Instance*************************************/

	//Test to get a given Icon instances CLUT
	
	TMockLtsyData2<TUint8,TUint8> getClutData(recordNumber,imageInstanceNumberInRecord);
	getClutData.SerialiseL(getData);
	
	const TUint KClutLength = 24;
	TBuf8<KClutLength> clutData(KClutLength);
	
    //fill with any data (just for testing)
    for(TInt j = 0; j < clutData.Length(); ++j)
    	{
    	clutData[j] = j;
    	}

	TDesC8* clutDataPtr = &clutData;
	TMockLtsyData1<TDesC8*> retClutData(clutDataPtr);
	retClutData.SerialiseL(retData);

	//queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId,KErrNone, retData);
	
	const TUint KGetClutOffset = 0; //Unused in the CTSY

	// ETel Sat Calls
	TBuf8<KClutLength> retrievedClut;
	iSat.GetClut(reqStatus, KInstanceNumber, KGetClutOffset, retrievedClut);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(clutData == retrievedClut);
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();
	
	/*************************************************************************************************/
		
	CleanupStack::PopAndDestroy(&retData);
	CleanupStack::PopAndDestroy(&getData);
	CleanupStack::PopAndDestroy(this);	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test setting up the event list then an event download (User Activity) from ETel
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd, Proactive command, RSat::TerminalRsp, RSat::EventDownload
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	const TUint8 KPCommandNumber = 123;

	SetupEventListL(KPCommandNumber,KEventUserActivity);
	
    // Create envelope
    TTlv envelope;
    envelope.Begin(KBerTlvEventDownloadTag);
    // Event list
    envelope.AddTag(KTlvEventListTag);
    envelope.AddByte(KEventUserActivity);
    // Device identities
    envelope.AddTag(KTlvDeviceIdentityTag);
    envelope.AddByte(KMe);
    envelope.AddByte(KSim);
   
	WaitForEventDownloadL(RSat::KUserActivity,envelope.End());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for control related functionality - a call is requested via a proactive command and then modified via call control
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpCallPCmd, RCall::OpenNewCall, RSat::TerminalRsp
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	iCallContainer = new (ELeave) CCallTypeContainer();
	
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	
	//provide location info so that it can be used during call control events 
	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);
	//************************************************************************************
	//Now we do a setup call proactive command
	
	TUint8 pcmdCode = KSetUpCall;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	RSat::TSetUpCallV6 setupCall;
	RSat::TSetUpCallV6Pckg setupCallPckg(setupCall);
	iSat.NotifySetUpCallPCmd(requestStatus, setupCallPckg);
	const TUint8 KSetUpCallPCmdNo = 201;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KSetUpCallPCmdNo);//ETLV_CommandNumber
	tlv.AddByte(pcmdCode); //ETLV_TypeOfCommand
	tlv.AddByte(KSetUpCallDisconnectingOtherCalls); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KNetwork); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAddressTag);
	const TUint8 KTonNpi = 0x91; 
	tlv.AddByte(KTonNpi); //TON and NPI (national number that conforms to the itu telephony style of numbering)
	
	RBuf8 setUpCallNo;
	CleanupClosePushL(setUpCallNo);
	setUpCallNo.CreateL(KPhoneNumber().Length());
	setUpCallNo.Copy(KPhoneNumber); //converts from 16 to 8 bit.
	
	RBuf8 bcdSetUpCallNo;
	CleanupClosePushL(bcdSetUpCallNo);
	bcdSetUpCallNo.CreateL(KPhoneNumber().Length());

	TSatUtility::AsciiToBCD(setUpCallNo,bcdSetUpCallNo);
	tlv.AddData(bcdSetUpCallNo);
	
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	ASSERT_EQUALS(setupCall.PCmdNumber(),KSetUpCallPCmdNo);
	
	//******************************************************************************************************
	//* At this stage the phone application/system software would attempt to make the call specified in the 
	//* SET UP CALL pcmd payload, first checking whether this is the emergency number (112) per ETSI TS 102 223 
	//* section 6.4.13 and routing the call via the appropriate ETel API, i.e. RMobileCall::DialEmergencyCall()
	//* for emergency calls and RCall::Dial() for standard ones.  
	//*
	//* After checking for emergency numbers and against the FDN list, the LTSY uses the CreateEnvelopeInd IPC to
	//* request that the CTSY constructs TLV data for an ENVELOPE (CALL CONTROL). Once this is done it passes it 
	//* to the LTSY with the SendEnvelopeReq(IPC).
	//*
	//******************************************************************************************************
	
	//Generate the call
	// Call control code cribbed from cctsycallcontrolfu.cpp
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService; 
	//The fourth parameter of DriverDialCallL specifies whether this call originates from the SIM or ETel client
	//In this case the call originates from the SET UP CALL proactive command so the flag is set to ETrue.
	DriverDialCallL(callId, mobileService, EFalse, ETrue);
	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause);
	// Client Side Test
	
	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);
	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	mobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);
	TRequestStatus hangupStatus;
	mobileCall.HangUp(hangupStatus);
	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
	CleanupClosePushL(expectedCallStatuses);
	expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
	expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
	ClientWaitForCallStatusNotifications(mobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);
	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());
	mobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);
	expectedCallStatuses.Close();
	CleanupStack::PopAndDestroy(1, &expectedCallStatuses);
	
	mobileCall.Close();
	mobileLine.Close();
	CleanupStack::PopAndDestroy(&mobileCall);
	CleanupStack::PopAndDestroy(&mobileLine);
	//**********************************************************************************
	//send up the "create envelope" event which causes the ctsy to assemble the ENVELOPE (CALL CONTROL)
	_LIT8(KBcc1,"asdas"); 
	_LIT8(KBcc2,"qwqwqw");
	const TUint8 KDcs = 56;
	
	TDesC8* bcc1Ptr = const_cast<TDesC8*>(&KBcc1);
	TDesC8* bcc2Ptr = const_cast<TDesC8*>(&KBcc2);
	TDesC8* addrPtr = &setUpCallNo;
	TUint8 tag = KTlvAddressTag;  
	TUint8 tonNpi = KTonNpi;
	TUint8 dcs = KDcs;
	TMockLtsyData6<TUint8, TDesC8*, TDesC8*, TDesC8*, TUint8, TUint8> ccInfoPck(tag, bcc1Ptr, bcc2Ptr, addrPtr, tonNpi, dcs);
	data.Close();
	ccInfoPck.SerialiseL(data);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateCallControlEnvelopeIndId,KErrNone,data);
	
	//**********************************************************************************
	//Now send down the created envelope
	tlv.Begin( KBerTlvCallControlTag );
	// Device identities
	tlv.AddTag( KTlvDeviceIdentityTag );
	tlv.AddByte( KMe );
	tlv.AddByte( KSim );
	// Address
	tlv.AddTag( KTlvAddressTag );
	tlv.AddByte( KTonNpi ); 
	tlv.AddData( bcdSetUpCallNo );
	// Optional
	tlv.AddTag( KTlvCapabilityConfigurationParametersTag );
	tlv.AddData( KBcc1 );
	// Add location information data
	tlv.AddTag( KTlvLocationInformationTag );
	// Append Mobile Country  & Network Codes, 3 bytes
	tlv.AddData( KOperatorCode );
	// Append Location Area Code (LAC)
	tlv.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetMSB ) );//msb
	tlv.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetLSB ) );//lsb
	// Append Cell Id
	tlv.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetMSB ) ); // msbthey
	tlv.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetLSB ) ); // lsb
		 
	tlv.AddTag( KTlvCapabilityConfigurationParametersTag );
	tlv.AddData( KBcc2 ); 
	
	// Prepare data
	TDesC8* tlvDscDes = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscDesPck(tlvDscDes);
	data.Close();
	tlvDscDesPck.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId,data);
	WaitForMockLTSYTerminated();
	
	//************************************************************************************
	//Now we send up the response of the call control
	
	tlv.Begin(0x02);//the type of request has changed (the envelope responses have a slightly different format to the rest of the data so we achieve this by passing in call control result as the top level tag)
	tlv.AddTag(KTlvAddressTag); 
	tlv.AddByte(KTonNpi); //ton+npi
	tlv.AddData(bcdSetUpCallNo);
	tlv.AddTag(KTlvAlphaIdentifierTag);
	
	_LIT8(KAlphaId8,"Test alpha id");
	_LIT16(KAlphaId16, "Test alpha id");
	tlv.AddData(KAlphaId8());
	
	tlvDscDes = &const_cast<TDesC8&>(tlv.End());
	
	
	TMockLtsyData1<TDesC8*> controlPck(tlvDscDes);
	data.Close();
	controlPck.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallControlEnvelopeResponseIndId,KErrNone,data); 
	
	
	iSat.NotifyCallControlRequest(requestStatus, iCallContainer->iCallControlPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	RSat::TAlphaIdValidity val;
	iCallContainer->iCallControl.GetAlphaId(val,iCallContainer->iAlphaId);
	ASSERT_TRUE(KAlphaId16() == iCallContainer->iAlphaId.iAlphaId);


	
	//************************************************************************************
	//now we need to provide the terminal response to the SET UP CALL proactive command
	
	//from etsi 102 223
	//6.8.9 Result data object 2
	//When the terminal issues a TERMINAL RESPONSE for a proactive command SET UP CALL which has been
	//modified by call control by UICC in another type of request, it shall supply the Result data object it would have
	//supplied for the proactive command equivalent to the action requested by call control, and given in the Call control
	//request data element.
	
	RSat::TSetUpCallRspV2 setUpCallRsp;
	RSat::TSetUpCallRspV2Pckg setUpCallRspPckg(setUpCallRsp);
	
	
	setUpCallRsp.iGeneralResult = RSat::KSuccess;
	setUpCallRsp.iInfoType = RSat::KNoAdditionalInfo;
	setUpCallRsp.SetPCmdNumber(KSetUpCallPCmdNo);
	
	GenerateAndWaitForTermRspL(KSetUpCallPCmdNo,KSetUpCall,KSetUpCallDisconnectingOtherCalls,RSat::ESetUpCall,RSat::KSuccess,setUpCallRspPckg,KErrNone);		
	
	CleanupStack::PopAndDestroy(&bcdSetUpCallNo); // data, this
	CleanupStack::PopAndDestroy(&setUpCallNo); // data, this
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Timer Managment functionality
@SYMTestPriority High
@SYMTestActions Invokes Proactive command, Terminal response, Timer Expiration Envelope, (start a timer and wait for it to expire)
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
	const TUint8 KPCommandNumber = 11;
	const RSat::TTimerId KTimerId = RSat::KTimer1;
	
	const TUint8 KTimerManagementStart 				= 0x00;
	//others KDeactivateTimer, KGetTimerValue and 0x03 is RFU
	
	//create the  Mock LTSY complete TLV data (a proactive command to start a timer)
	TTlv proactiveCommandTlv;
	
	proactiveCommandTlv.Begin(KBerTlvProactiveSimCommandTag);

	proactiveCommandTlv.AddTag(KTlvCommandDetailsTag);
	proactiveCommandTlv.AddByte(KPCommandNumber);//ETLV_CommandNumber
	proactiveCommandTlv.AddByte(KTimerManagement); //ETLV_TypeOfCommand
	proactiveCommandTlv.AddByte(KTimerManagementStart); //ETLV_CommandQualifier

	proactiveCommandTlv.AddTag(KTlvDeviceIdentityTag);
	proactiveCommandTlv.AddByte(KSim);
	proactiveCommandTlv.AddByte(KMe);
	
	proactiveCommandTlv.AddTag(KTlvTimerIdentifierTag);
	proactiveCommandTlv.AddByte(KTimerId);
	
	proactiveCommandTlv.AddTag(KTlvTimerValueTag);

	const TUint8 KHourLSB = 0;
	const TUint8 KHourMSB = 0;

	const TUint8 KMinLSB = 0;
	const TUint8 KMinMSB = 0;

	const TUint8 KSecLSB = 0;
	const TUint8 KSecMSB = 2; //2 seconds

	const TInt KBitsToShiftMSNibble = 4;
	
	proactiveCommandTlv.AddByte((KHourMSB << KBitsToShiftMSNibble) | KHourLSB); //hour
	proactiveCommandTlv.AddByte((KMinMSB  << KBitsToShiftMSNibble) | KMinLSB); //min
	proactiveCommandTlv.AddByte((KSecMSB  << KBitsToShiftMSNibble) | KSecLSB); //second
	
	proactiveCommandTlv.AddByte((KHourMSB << KBitsToShiftMSNibble) | KHourLSB); //hour
	proactiveCommandTlv.AddByte((KMinMSB  << KBitsToShiftMSNibble) | KMinLSB); //min
	proactiveCommandTlv.AddByte((KSecMSB  << KBitsToShiftMSNibble) | KSecLSB); //second

	TDesC8* proactiveCommandTlvPtr = &const_cast<TDesC8&>(proactiveCommandTlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(proactiveCommandTlvPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);

	//The Terminal response generated by the CTSY (as a result of the proactive command)

	const TDesC8& KGeneralResultAddInfo = KNullDesC8;
	
	TTlv additionalInfoTlv;
	additionalInfoTlv.AddTag(KTlvTimerIdentifierTagCRBitOn);
	additionalInfoTlv.AddByte(KTimerId);

	GenerateAndWaitForTermRspL(	KPCommandNumber,
								KTimerManagement,
								KTimerManagementStart,
								RSat::ETimerMngt,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								KNullDesC8,
								additionalInfoTlv.GetDataWithoutTopLevelTag(),
								KErrNone);
		
	//Queue the expect, this is the Timer Expiration ENVELOPE command when the timer expires.
	TTlv timerExpirationTlv;    
	timerExpirationTlv.Begin( KBerTlvTimerExpirationTag );  
    // Create Device Identity TLV
    // Device identity tag
	timerExpirationTlv.AddTag( KTlvDeviceIdentityTag ); 
    // Source device identity = ME
	timerExpirationTlv.AddByte(KMe); 
    // Destination device identity = SIM
	timerExpirationTlv.AddByte(KSim);
    // Value KTlvTimerIdentifierTagCRBitOn ( 0xA4 ) is added,
    // because used KTlvTimerIdentifierTag ( 0x24 ) doesn't
    // include comprehension bit in tag the value and it is expected
    // to be present in indications and terminal responses.
	timerExpirationTlv.AddTag(KTlvTimerIdentifierTagCRBitOn);
	timerExpirationTlv.AddByte(KTimerId);
    // Timer value in semi-octets
	timerExpirationTlv.AddTag( KTlvTimerValueTag );
	timerExpirationTlv.AddByte((KHourMSB << KBitsToShiftMSNibble) | KHourLSB); //hour
	timerExpirationTlv.AddByte((KMinMSB  << KBitsToShiftMSNibble) | KMinLSB); //min
	timerExpirationTlv.AddByte((KSecMSB  << KBitsToShiftMSNibble) | KSecLSB); //second

	TDesC8* timerExpirationDscPtr = &const_cast<TDesC8&>(timerExpirationTlv.End());

	TMockLtsyData1<TDesC8*> timerExpirationTlvDscPack(timerExpirationDscPtr);
	timerExpirationTlvDscPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId,data);
	data.Close();

	//wait for the timer to expire
	WaitForMockLTSYTerminated();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Setting the polling interval and turning polling off.
@SYMTestPriority High
@SYMTestActions Invokes CTSY Internal Proactive command, Set Polling Interval, Terminal Response
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0007L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
	/*********************************Set Polling Interval*******************************************/
	
	const TUint8 KSetPollingPCmdNumber = 123;
	const TUint8 KCommandQualifier = 0; //RFU (Reserved for future use)
		
	TTlv pollIntervalPCmdTlv;
	pollIntervalPCmdTlv.Begin(KBerTlvProactiveSimCommandTag);
	pollIntervalPCmdTlv.AddTag(KTlvCommandDetailsTag);
	pollIntervalPCmdTlv.AddByte(KSetPollingPCmdNumber);//ETLV_CommandNumber
	pollIntervalPCmdTlv.AddByte(KPollInterval); //ETLV_TypeOfCommand
	pollIntervalPCmdTlv.AddByte(KCommandQualifier); //ETLV_CommandQualifier - RFU
	
	pollIntervalPCmdTlv.AddTag(KTlvDeviceIdentityTag); 
	pollIntervalPCmdTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	pollIntervalPCmdTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
	
	const TUint8 KTimeUint = KSeconds; //seconds
	const TUint8 KTimeInterval = 5; //5 of them
	
	pollIntervalPCmdTlv.AddTag(KTlvDurationTag);
	pollIntervalPCmdTlv.AddByte(KTimeUint);
	pollIntervalPCmdTlv.AddByte(KTimeInterval);
	
	TDesC8* pollIntervalPCmdTlvDscPtr = &const_cast<TDesC8&>(pollIntervalPCmdTlv.End());
	
	//the proactive command (LTSY -> CTSY)
	TMockLtsyData1<TDesC8*> pollIntervalPCmdPack(pollIntervalPCmdTlvDscPtr);
	pollIntervalPCmdPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();

	TUint8 pollInterval = KTimeInterval;

	//set polling request (CTSY -> LTSY)
	TMockLtsyData1<TUint8> setPollIntervalRequestPack(pollInterval);
	setPollIntervalRequestPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId,data);
	data.Close();
	
	//set polling complete (LTSY -> CTSY)
    TMockLtsyData1<TUint8> setPollIntervalCompletePack(pollInterval);
    setPollIntervalCompletePack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId,KErrNone,data);
	data.Close();

	//terminal response for inital proactive command request (CTSY -> LTSY)
	RSat::TPCmdResult pollIntervalGeneralResult = RSat::KSuccess;
	const TDesC8& KGeneralResultAddInfo = KNullDesC8;	
	
	TTlv pollIntervalAdditionalInfoTlv;
	pollIntervalAdditionalInfoTlv.AddTag(KTlvDurationTag);
	pollIntervalAdditionalInfoTlv.AddByte(KSeconds);
	pollIntervalAdditionalInfoTlv.AddByte(pollInterval);
	
	GenerateAndWaitForTermRspL(	KSetPollingPCmdNumber,
								KPollInterval,
								KCommandQualifier,
								RSat::EPollingInterval,
								pollIntervalGeneralResult,
								KGeneralResultAddInfo,
								KNullDesC8,
								pollIntervalAdditionalInfoTlv.GetDataWithoutTopLevelTag(),
								KErrNone);
	
	/*********************************Turn Polling Off*******************************************/
	
	const TUint8 KPollingOffPCmdNumber = 125;
	
	//proactive command to set Polling off (LTSY -> CTSY)
	TTlv pollingOffPCmdTlv;
	pollingOffPCmdTlv.Begin(KBerTlvProactiveSimCommandTag);
	pollingOffPCmdTlv.AddTag(KTlvCommandDetailsTag);
	pollingOffPCmdTlv.AddByte(KPollingOffPCmdNumber);//ETLV_CommandNumber
	pollingOffPCmdTlv.AddByte(KPollingOff); //ETLV_TypeOfCommand
	pollingOffPCmdTlv.AddByte(KCommandQualifier); //ETLV_CommandQualifier - RFU
	
	pollingOffPCmdTlv.AddTag(KTlvDeviceIdentityTag); 
	pollingOffPCmdTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	pollingOffPCmdTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
	
	TDesC8* pollingOffPCmdTlvDscPtr = &const_cast<TDesC8&>(pollingOffPCmdTlv.End());
	
	TMockLtsyData1<TDesC8*> pollingOffPCmdPack(pollingOffPCmdTlvDscPtr);
	pollingOffPCmdPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();
	
	//set polling request (CTSY -> LTSY)
	TUint8 pollingOffInterval = 0;
	TMockLtsyData1<TUint8> setPollingOffRequestPack(pollingOffInterval);
	setPollingOffRequestPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId,data);
	data.Close();
	
	//set polling complete (LTSY -> CTSY)
	TMockLtsyData1<TUint8> setPollingOffCompletePack(pollingOffInterval);
    setPollingOffCompletePack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId,KErrNone,data);
	data.Close();	
	

	//terminal response for inital proactive command request (CTSY -> LTSY)
	RSat::TPCmdResult pollingOffGeneralResult = RSat::KSuccess;

	GenerateAndWaitForTermRspL(KPollingOffPCmdNumber,KPollingOff,KCommandQualifier,RSat::EPollingOff,pollingOffGeneralResult,KNullDesC8,KErrNone);

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY Mo Short Message Control
@SYMTestPriority High
@SYMTestActions Send up a "send sms" proactive command, send the SMS via RSat, then handle the mo sms control envelope 
and response, and complete a terminal response
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL(KSmsMoControlEnabled);

	//setup local info as this is appended to the MO SHORT MESSAGE CONTROL ENVELOPE
	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);
	
	
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	TRequestStatus sendMessageReqStatus;
	TRequestStatus moSmControlRequestReqStatus;

	//send a Sm proactive command
	TUint8 pcmdCode = KSendShortMessage;
	TMockLtsyData1<TUint8> expPCmdNotificationLtsyData(pcmdCode);
	expPCmdNotificationLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	expPCmdNotificationLtsyData.Close();


	const TUint8 KPCmdNumber = 123;
	const TUint8 KCommandQualifier = 0;  //0 = packing not required, 1 = SMS packing by the ME required.
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);

	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSendShortMessage); //ETLV_TypeOfCommand
	tlv.AddByte(KCommandQualifier); // 0 = packing not required, 1 = SMS packing by the ME required.

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);
	tlv.AddByte(KMe);
	
	_LIT8(KAlphaId8,"Text To Display");
	_LIT16(KAlphaId16,"Text To Display");
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddData(KAlphaId8);
	
	tlv.AddTag(KTlvAddressTag);
	tlv.AddByte(0); //Ton NPI
	tlv.AddData(_L8("Number"));
	

	TUint8 tpduProtocolByte = 0;
		
	tpduProtocolByte |= 0x01 << 0;//TP Message Type Indicator, bit 0-1 - SMS SUBMIT
	tpduProtocolByte |= 0x00 << 2;//TP Reject Duplicates, bit 2 - Instruct the SC to accept an SMS SUBMIT for an SM still held in the
						 		//SC which has the same TP MR and the same TP DA as a previously 	submitted SM from 	the same OA.
	tpduProtocolByte |= 0x00 << 3; //TP Validity Period Format, bit 3-4 - TP VP field not present
	tpduProtocolByte |= 0x00 << 5; //TP Status Report Request, bit 5 - A status report is not requested
	tpduProtocolByte |= 0x00 << 6;	//TP User Data Header Indicator, bit 6 - The TP UD field contains only the short message
	tpduProtocolByte |= 0x00 << 7;	//TP Reply Path, bit 7- TP Reply Path parameter is not set in this SMS SUBMIT/DELIVER

	const TUint8 KProtocolIdentifier = 0x7F; //SmsTpduProtcolIdUSimDdl Parameters(1)+TP-OA(min 2)+TP-PID(1)+DCS(1)+TP-SCTS(7)+TP-UDL(2)+TP-UD(?)
	const TUint8 KDataCodingScheme = 0x02; //TP Data Coding Scheme (TP DCS)

	TTpdu tpdu;
	tpdu.Append(tpduProtocolByte);
	
	_LIT8(KOriginatingAddress,"1234567");
	tpdu.Append(KOriginatingAddress().Length() * 2); //Number of octets
	tpdu.Append(0); //Type of address
	tpdu.Append(KOriginatingAddress());
	tpdu.Append(KProtocolIdentifier); //TP Protocol Identifier 
	tpdu.Append(KDataCodingScheme); //TP Data Coding Scheme (TP DCS)
	tpdu.Append(_L8("1234567")); //TP Service Centre Time Stamp

	_LIT8(KUserData,"User Data");
	tpdu.Append(KUserData().Length()); //TP User Data Length
	tpdu.Append(KUserData()); //TP User Data

	tlv.AddTag(KTlvSmsTpduTag);
	tlv.AddData(tpdu);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(sendSm.PCmdNumber(),KPCmdNumber);
	ASSERT_EQUALS(sendSm.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(sendSm.iAlphaId.iAlphaId == KAlphaId16);
	ASSERT_TRUE(sendSm.iSmsTpdu == tpdu);
	
	_LIT8(KServiceCentreNumber,"123456789");
	
	const RMobilePhone::TMobileTON KMobilePhoneTon = RMobilePhone::ENetworkSpecificNumber;
	const RSat::TTypeOfNumber KSatTon = RSat::ENetworkSpecificNumber;
	const RMobilePhone::TMobileNPI KMobilePhoneNpi = RMobilePhone::EPrivateNumberPlan;
	const RSat::TNumberingPlan KSatNpi = RSat::EPrivateNumberPlan;
	
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsSendAttributes;
	smsSendAttributes.iFlags = (RMobileSmsMessaging::KSmsDataFormat | RMobileSmsMessaging::KGsmServiceCentre | RMobileSmsMessaging::KMoreToSend);
	smsSendAttributes.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
	smsSendAttributes.iGsmServiceCentre.iTelNumber.Copy(KServiceCentreNumber);
	smsSendAttributes.iGsmServiceCentre.iTypeOfNumber = KMobilePhoneTon;
	smsSendAttributes.iGsmServiceCentre.iNumberPlan = KMobilePhoneNpi;
	smsSendAttributes.iMore = EFalse; //set False in CSatTsy::SendMessageNoLogging
	
	TDesC8* tpduPtr = &tpdu;
    TMockLtsyData4< TDesC8*,
                        RMobileSmsMessaging::TMobileSmsDataFormat,
                        RMobilePhone::TMobileAddress,
                        TBool
                       > sendMessagePack(tpduPtr, smsSendAttributes.iDataFormat, smsSendAttributes.iGsmServiceCentre, smsSendAttributes.iMore);
	sendMessagePack.SerialiseL(data);	
	iMockLTSY.ExpectL(MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,data);
	data.Close();
	
	RSat::TSatSmsV1 satSms;
	RSat::TSatSmsV1Pckg satSmsPckg(satSms);
	satSms.iBuf = sendSm.iSmsTpdu;
	satSms.iServiceCenter.iTelNumber.Copy(KServiceCentreNumber);
	satSms.iServiceCenter.iTypeOfNumber = KSatTon;
	satSms.iServiceCenter.iNumberPlan = KSatNpi;
		
	RSat::TMoSmControlV1 moSmsControl;
	RSat::TMoSmControlV1Pckg moSmsControlPckg(moSmsControl);
	iSat.NotifyMoSmControlRequest(moSmControlRequestReqStatus,moSmsControlPckg);

	
	TUint16 msgRef;
	iSat.SendMessageNoLogging(sendMessageReqStatus,satSmsPckg,msgRef);


	//indicator up from the CTSY to create a Mo Short Message Envelope 
	//LTSY -> CTSY

	//Type of Number (TON) and numbering plan identification (NPI).
	//MSB - 1, Type of Number - 011 (Network Specific Number), Numbering Plan - 1001 (Private numbering plan) 10111001 = B9 Hex
	const TUint8 KTonAndNpi = TonAndNpi(RMobilePhone::ENetworkSpecificNumber,RMobilePhone::EPrivateNumberPlan); 
	
	RBuf8 rpDestinationAddress;
	CleanupClosePushL(rpDestinationAddress);
	rpDestinationAddress.CreateL(KAddrMaxLength);
	rpDestinationAddress.Append(KTonAndNpi);  //TON & NPI
	rpDestinationAddress.Append(_L8("1234567")); //dummy data not EF(ADN) encoded data

	RBuf8 tpDestinationAddress;
	CleanupClosePushL(tpDestinationAddress);
	tpDestinationAddress.CreateL(KAddrMaxLength);
	tpDestinationAddress.Append(KTonAndNpi); //TON & NPI
	tpDestinationAddress.Append(_L8("7654321")); //dummy data not EF(ADN) encoded data

    TDesC8* rpDestinationAddressPtr = &rpDestinationAddress;
    TDesC8* tpDestinationAddressPtr = &tpDestinationAddress;

    TMockLtsyData2<TDesC8*,TDesC8*> createMoSmsControlEnvelopPack(rpDestinationAddressPtr,tpDestinationAddressPtr);

    createMoSmsControlEnvelopPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId, KErrNone, data);
	data.Close();
	
	//constructed envelope by the CTSY send down (see 3GPP TS 11.14 - Structure of ENVELOPE (MO SHORT MESSAGE CONTROL))
	//CTSY -> LTSY 
	TTlv moSmControlEnvelope;

	moSmControlEnvelope.Begin(KBerTlvMoShortMessageControlTag);
	//device identities
	moSmControlEnvelope.AddTag(KTlvDeviceIdentityTag);
	moSmControlEnvelope.AddByte(KMe);
	moSmControlEnvelope.AddByte(KSim);
	// Address Data object 1
	moSmControlEnvelope.AddTag(KTlvAddressTag);
	moSmControlEnvelope.AddData(rpDestinationAddress);
	// Address Data object 2
	moSmControlEnvelope.AddTag(KTlvAddressTag);
	moSmControlEnvelope.AddData(tpDestinationAddress);

    // Add location information data, start with the tag
	moSmControlEnvelope.AddTag(KTlvLocationInformationTag);
    // Append Mobile Country  & Network Codes, 3 bytes

	moSmControlEnvelope.AddData(KOperatorCode);
	// Append Location Area Code (LAC)
	moSmControlEnvelope.AddByte(KLocationAreaCode >> KShiftBitsToGetMSB);//msb
	moSmControlEnvelope.AddByte(KLocationAreaCode & 0x00FF);//lsb
    // Append Cell Id
	moSmControlEnvelope.AddByte(KCellId >> KShiftBitsToGetMSB); //msb
	moSmControlEnvelope.AddByte(KCellId & 0x00FF); //lsb

	// Prepare data
	TDesC8* moSmControlEnvelopeTlvPtr = &const_cast<TDesC8&>(moSmControlEnvelope.End());

    TMockLtsyData1<TDesC8*> moSmsControlEnvelopePack(moSmControlEnvelopeTlvPtr);

    moSmsControlEnvelopePack.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId,data);
	data.Close();
	
	WaitForMockLTSYTerminated();

	
	//Response from the SIM
	//LTSY -> CTSY
	
	TTlv moSmsControlResponseTlv;

	TUint8 result = KMoSmCtrlResultAllowed;
	_LIT8(KAlphaIdData8,"Sending an SMS");
	_LIT16(KAlphaIdData16,"Sending an SMS");
	
	moSmsControlResponseTlv.Begin(result);
	
	moSmsControlResponseTlv.AddTag(KTlvAddressTag);
	moSmsControlResponseTlv.AddData(rpDestinationAddress);

	moSmsControlResponseTlv.AddTag(KTlvAddressTag);
	moSmsControlResponseTlv.AddData(tpDestinationAddress);

	moSmsControlResponseTlv.AddTag(KTlvAlphaIdentifierTag);
	moSmsControlResponseTlv.AddData(KAlphaIdData8);
	
	TDesC8* moSmsControlResponseTlvDesPtr = &const_cast<TDesC8&>( moSmsControlResponseTlv.End());
		
	TMockLtsyData1<TDesC8*> moSmsControlResponsePack(moSmsControlResponseTlvDesPtr);
	moSmsControlResponsePack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId,KErrNone,data);
	data.Close();
	
	TMockLtsyData3<TUint8,TDesC8*,TDesC8*> moSmControlResponseDataPack(result,rpDestinationAddressPtr,tpDestinationAddressPtr);
	moSmControlResponseDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId,data);
	data.Close();
	
	User::WaitForRequest(moSmControlRequestReqStatus);
	ASSERT_EQUALS(moSmControlRequestReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(moSmsControl.iResult,RSat::EAllowedNoModification);
	ASSERT_EQUALS(moSmsControl.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(moSmsControl.iAlphaId.iAlphaId == KAlphaIdData16);
	
	//LTSY -> CTSY the SMS has been sent
	iMockLTSY.CompleteL(MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,KErrNone);
	User::WaitForRequest(sendMessageReqStatus);
	ASSERT_EQUALS(sendMessageReqStatus.Int(),KErrNone);
		
	//generate the terminal response to the send sms proactive command.
	RSat::TSendSmRspV1 sendSmRsp;
	sendSmRsp.iGeneralResult = RSat::KSuccess;
	sendSmRsp.iInfoType = RSat::KNoAdditionalInfo;
	sendSmRsp.SetPCmdNumber(KPCmdNumber);
	RSat::TSendSmRspV1Pckg sendSmRspPckg(sendSmRsp);

	GenerateAndWaitForTermRspL(KPCmdNumber,KSendShortMessage,KCommandQualifier,RSat::ESendSm,RSat::KSuccess,sendSmRspPckg,KErrNone);
	
	CleanupStack::PopAndDestroy(&tpDestinationAddress);
	CleanupStack::PopAndDestroy(&rpDestinationAddress);

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Sms PP & Cell Broadcast Data Download
@SYMTestPriority High
@SYMTestActions Invokes CTSY Internal, create both SmsPP and Cell Broadcast envelopes
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL(KSmsPpDownloadEnabled);

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	/*********************************SMS PP Download********************************/

	//test the case when SMS PP Data Download is allocated in NAA (KSmsPpDownloadEnabled)
	
	//MSB - 1, Type of Number - 011 (Network Specific Number), Numbering Plan - 1001 (Private numbering plan)
	//10111001 = B9 Hex
		
	RMobilePhone::TMobileTON KTon = RMobilePhone::ENetworkSpecificNumber;
	RMobilePhone::TMobileNPI KNpi = RMobilePhone::EPrivateNumberPlan;
		
	const TUint8 KTonAndNpi = TonAndNpi(KTon,KNpi); //Type of Number (TON) and numbering plan identification (NPI).
	
	RBuf8 rpOriginatingAddress;
	CleanupClosePushL(rpOriginatingAddress);
	rpOriginatingAddress.CreateL(KAddrMaxLength);
	rpOriginatingAddress.Append(KTonAndNpi);  //TON & NPI
	_LIT8(KRpNumber,"1234567");
	rpOriginatingAddress.Append(KRpNumber); //dummy data not EF(ADN) encoded data

	RBuf8 smsDeliveryTpdu;
	CleanupClosePushL(smsDeliveryTpdu);
	smsDeliveryTpdu.CreateL(RMobileSmsMessaging::KGsmTpduSize);
	
	TUint8 tpduProtocolByte = 0;
		
	tpduProtocolByte |= 0x01 << 0;//TP Message Type Indicator, bit 0-1 - SMS SUBMIT
	tpduProtocolByte |= 0x00 << 2;//TP Reject Duplicates, bit 2 - Instruct the SC to accept an SMS SUBMIT for an SM still held in the
						 		//SC which has the same TP MR and the same TP DA as a previously 	submitted SM from 	the same OA.
	tpduProtocolByte |= 0x00 << 3; //TP Validity Period Format, bit 3-4 - TP VP field not present
	tpduProtocolByte |= 0x00 << 5; //TP Status Report Request, bit 5 - A status report is not requested
	tpduProtocolByte |= 0x00 << 6;	//TP User Data Header Indicator, bit 6 - The TP UD field contains only the short message
	tpduProtocolByte |= 0x00 << 7;	//TP Reply Path, bit 7- TP Reply Path parameter is not set in this SMS SUBMIT/DELIVER

	const TUint8 KProtocolIdentifier = 0x7F; //SmsTpduProtcolIdUSimDdl Parameters(1)+TP-OA(min 2)+TP-PID(1)+DCS(1)+TP-SCTS(7)+TP-UDL(2)+TP-UD(?)
	const TUint8 KDataCodingScheme = 0x02; //TP Data Coding Scheme (TP DCS)
	
	smsDeliveryTpdu.Append(tpduProtocolByte);
	
	_LIT8(KOriginatingAddress,"1234567");
	smsDeliveryTpdu.Append(KOriginatingAddress().Length() * 2); //Number of octets
	smsDeliveryTpdu.Append(0); //Type of address
	smsDeliveryTpdu.Append(KOriginatingAddress());
	smsDeliveryTpdu.Append(KProtocolIdentifier); //TP Protocol Identifier 
	smsDeliveryTpdu.Append(KDataCodingScheme); //TP Data Coding Scheme (TP DCS)
	smsDeliveryTpdu.Append(_L8("1234567")); //TP Service Centre Time Stamp

	_LIT8(KUserData,"User Data");
	smsDeliveryTpdu.Append(KUserData().Length()); //TP User Data Length
	smsDeliveryTpdu.Append(KUserData()); //TP User Data
	
	
    TDesC8* rpOriginatingAddressPtr = &rpOriginatingAddress;
    TDesC8* smsDeliveryTpduPtr = &smsDeliveryTpdu;
	
    //Indicator of a Sms-Point to Point-Download - LTSY -> CTSY
	TMockLtsyData2<TDesC8*,TDesC8*> smsPpDatadownloadIndDataPack(rpOriginatingAddressPtr,smsDeliveryTpduPtr);
	smsPpDatadownloadIndDataPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId,KErrNone,data);
	data.Close();
	
    TTlv smsPpDownloadEnvelope;
    smsPpDownloadEnvelope.Begin(KBerTlvSmsPpDownloadTag);
    smsPpDownloadEnvelope.AddTag(KTlvDeviceIdentityTag);
    smsPpDownloadEnvelope.AddByte(KNetwork);
    smsPpDownloadEnvelope.AddByte(KSim);
    smsPpDownloadEnvelope.AddTag(KTlvAddressTag);
    smsPpDownloadEnvelope.AddData(rpOriginatingAddress);
    smsPpDownloadEnvelope.AddTag(KTlvSmsTpduTag );
    smsPpDownloadEnvelope.AddData(smsDeliveryTpdu);

    // Prepare data
    TDesC8* smsPpDownloadEnvelopeTlv = &const_cast<TDesC8&>(smsPpDownloadEnvelope.End());

    //handle the envelope as a result of the Indicator CTSY -> LTSY
	TMockLtsyData1<TDesC8*> smsPpDdlExpectDataPack(smsPpDownloadEnvelopeTlv);
	smsPpDdlExpectDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId,data,KErrNone);
	data.Close();

	WaitForMockLTSYTerminated();
	
	//test the case when SMS PP Data Download is not allocated in NAA

	//re-open RSat with Sms PP Download Disabled
	iSat.Close();
	OpenSatL(); //KSmsPpDownloadEnabled - Disabled

    //Indicator of a Sms-Point to Point-Download - LTSY -> CTSY
	smsPpDatadownloadIndDataPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId,KErrNone,data);
	data.Close();

	
	RMobileSmsStore::TMobileGsmSmsEntryV1 mobileGsmSmsEntry;
	mobileGsmSmsEntry.iIndex = -1; //-1 means 1st free slot. See Etel MM API Specification, 8.1.4 Write Entry. 
	mobileGsmSmsEntry.iServiceCentre.iTypeOfNumber =  KTon;
	mobileGsmSmsEntry.iServiceCentre.iNumberPlan = KNpi;
	
    TBuf8<RSat::KMaxMobileTelNumberSize> address8;
    TPtrC8 rpNumber(KRpNumber);
    TSatUtility::BCDToAscii(rpNumber,address8);

    for(TInt i = 0; i < address8.Length(); ++i)
    	{
    	mobileGsmSmsEntry.iServiceCentre.iTelNumber.Append(address8[i]);
    	}
    
    mobileGsmSmsEntry.iMsgData.Copy(smsDeliveryTpdu);
	

	//the request to do a sms store (CTSY->LTSY)
	TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> mobileGsmSmsEntryDataPack(mobileGsmSmsEntry);
	mobileGsmSmsEntryDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId,data,KErrNone);
	data.Close();
	
	//the complete to the sms stote (LTSY -> CTSY)
	TInt location = 100;
	TBool  receivedClass2ToBeResent = ETrue;
	TMockLtsyData2<TInt, TBool> smsWriteEntryCompPack(location, receivedClass2ToBeResent);
	smsWriteEntryCompPack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId,KErrNone, data);
	data.Close();
	
	WaitForMockLTSYTerminated();
	
	
	//Indicator to create the Sms Delivery Report 
	//LTSY -> CTSY
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateSmsDeliveryReportIndId,KErrNone);
	

	RBuf8 smsDeliveryReportTpdu;
	CleanupClosePushL(smsDeliveryReportTpdu);
	smsDeliveryReportTpdu.CreateL(RMobileSmsMessaging::KGsmTpduSize);
	
	smsDeliveryReportTpdu.Append(tpduProtocolByte); //TP-parameters
	smsDeliveryReportTpdu.Append(0x07); //TP-Parameter-Indicator -KAllOptParamsPresent
	smsDeliveryReportTpdu.Append(KProtocolIdentifier); //TP-Protocol-Identifier
	smsDeliveryReportTpdu.Append(KDataCodingScheme); //TP-Data-Coding-Scheme
	smsDeliveryReportTpdu.Append(KUserData().Length()); //TP User Data Length
	smsDeliveryReportTpdu.Append(KUserData()); //TP User Data
	
	TDesC8* smsDeliveryReportTpduPtr = &smsDeliveryReportTpdu;
	
	//The Sms Delivery report expect Tpdu as a result from the indicator
	//CTSY -> LTSY
	TMockLtsyData1<TDesC8*> smsDeliveryReportExpectDataPack(smsDeliveryReportTpduPtr);
	smsDeliveryReportExpectDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId,data,KErrNone);
	data.Close();

	//no associated complete to MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId
	WaitForMockLTSYTerminated();
	
	/*********************************Cell Broadcast********************************/
	RBuf8 cellBroadcastPage;
	CleanupClosePushL(cellBroadcastPage);
	
	_LIT8(KCellBroadCastPageData,"Page Data");
	cellBroadcastPage.CreateL(92/*KCbsMsgMaxLength*/);
	cellBroadcastPage.Append(KCellBroadCastPageData);
	
	TDesC8* cellBroadcastPagePtr = &cellBroadcastPage;
	TMockLtsyData1<TDesC8*> cellBroadcastCompleteLtsyData(cellBroadcastPagePtr);
	cellBroadcastCompleteLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateCellBroadcastEnvelopeIndId, KErrNone, data);
	data.Close();
	
	
	TTlv cellBroadcastEnvelope;
	// Tag
	cellBroadcastEnvelope.Begin(KBerTlvCellBroadcastTag);
	// Device identities
	cellBroadcastEnvelope.AddTag(KTlvDeviceIdentityTag);
	cellBroadcastEnvelope.AddByte(KNetwork);
	cellBroadcastEnvelope.AddByte(KSim);    
	// Cell broadcast page
	cellBroadcastEnvelope.AddTag(KTlvCellBroadcastPageTag);
	cellBroadcastEnvelope.AddData(KCellBroadCastPageData);  
	// Prepare data
	
	// Prepare data
	TDesC8* cellBroadcastEnvelopePtr = &const_cast<TDesC8&>(cellBroadcastEnvelope.End());

	TMockLtsyData1<TDesC8*> cellBroadcastExpLtsyData(cellBroadcastEnvelopePtr);
	cellBroadcastExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId,data);
	data.Close();

	WaitForMockLTSYTerminated();
	
	CleanupStack::PopAndDestroy(&cellBroadcastPage);
	CleanupStack::PopAndDestroy(&smsDeliveryReportTpdu);
	CleanupStack::PopAndDestroy(&smsDeliveryTpdu);
	CleanupStack::PopAndDestroy(&rpOriginatingAddress);
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/*
@SYMTestCaseID BA-CTSYD-DIS-SAT-UC0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test 
@SYMTestPriority High
@SYMTestActions Testing of Refresh Commands
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUseCase0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	const TUint8 KPCmdNumber = 101;
	const TUint8 KCommandQualifier = KSimInitFullFileChangeNotification; //zero is NAA Initialization and Full File Change Notification
	
	RefreshNoTerminalRspL(KPCmdNumber, KCommandQualifier, KErrNone);
	
	//Do the Client side refresh
	
	
	//once finished generate the Terminal response	

	RSat::TRefreshRspV1 refreshTerminalRsp;
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber);

	RSat::TRefreshRspV1Pckg refreshTerminalRspPckg(refreshTerminalRsp);
	
	GenerateAndWaitForTermRspL(KPCmdNumber,KRefresh,KCommandQualifier,RSat::ERefresh,RSat::KSuccess,refreshTerminalRspPckg,KErrNone);
	
	AssertMockLtsyStatusL();

	//for a coverage test complete MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId with an error
	RefreshNoTerminalRspL(KPCmdNumber, KCommandQualifier, KErrNone);
	GenerateAndWaitForTermRspL(KPCmdNumber,KRefresh,KCommandQualifier,RSat::ERefresh,RSat::KSuccess,refreshTerminalRspPckg,KErrGeneral);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);
	}

//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify local info Proactive Command sequence
@SYMTestPriority High
@SYMTestActions Invokes PROVIDE LOCAL INFO proactive command, RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	

	//
	//First provide local info (langauge) - this one gets sent up to the RSat client to deal with
	
	TUint8 pcmdCode = KProvideLocalInformation;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();

	
	const TUint8 KPCmdNumber = 101;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	tlv.AddByte(RSat::KProvideLocalInfoLanguage);//ETLV_CommandQualifier / KCommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);//ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	
	data.Close();
	
	TUint16 locationAreaCode = KLocationAreaCode;
    TUint16 cellId = KCellId;
    TBuf8<KOperatorCodeLength> operatorCode(KOperatorCode);
  	TDesC8* opCodePtr = &operatorCode;
	TMockLtsyData3<TUint16,TUint16,TDesC8*> localInfoPCmdDataPack(locationAreaCode,cellId,opCodePtr);
	localInfoPCmdDataPack.SerialiseL(data);
	
	RSat::TLocalInfoV3 localInfo;
	RSat::TLocalInfoV3Pckg localInfoPck(localInfo);
	localInfo.iInfoType = RSat::KProvideLocalInfo;
	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPck);
	User::WaitForRequest(stat);
	ASSERT_EQUALS(stat.Int(),KErrNone);
	ASSERT_EQUALS(localInfo.PCmdNumber(),KPCmdNumber);
	ASSERT_EQUALS(localInfo.iDevideId,RSat::KUICC);
	ASSERT_EQUALS(localInfo.iInfoType,RSat::KProvideLocalInfoLanguage);
	
	
	//Now send the terminal response
	 
	_LIT(KTermRespLangCode,"EN"); //language code 
	
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvLanguageTag );
	TBuf8<2> character;
	TSatUtility::Copy16to8LE( KTermRespLangCode, character);
	termRespAddInfo.AddData( character );
	TPtrC8 termRespAddInfoDes= termRespAddInfo.GetDataWithoutTopLevelTag();
	
	RSat::TLocalInfoRspV3 resp;
	RSat::TLocalInfoRspV3Pckg respPck(resp);
	resp.iGeneralResult = RSat::KSuccess;
	//resp.iInfoType = RSat::KLocalInformation;
	resp.iInfoType = RSat::KProvideLocalInfoLanguage;
	resp.iAdditionalInfo = KTermRespLangCode; //language code 
	resp.SetPCmdNumber(KPCmdNumber);
	
	const TDesC8& KGeneralResultAddInfo = KNullDesC8;
	
	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInfoLanguage,
								RSat::ELocalInfo,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								respPck,
								termRespAddInfoDes,
								KErrNone);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify of a Access Technology change
@SYMTestPriority High
@SYMTestActions Invokes RSat::EventDownload, CallbackSatAccessTechnologyChangeInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	const TUint8 KPCmdNumber = 123;
	
	SetupEventListL(KPCmdNumber,KEventAccessTechnologyChange);
		
	TUint8 currentAccessTechnology = 0x06; //i.e. TIA/EIA/IS-2000;

	//MockLTSY complete, an access technology change up from the LTSY to the CTSY
	TMockLtsyData1<TUint8> accTechPack(currentAccessTechnology);
	accTechPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatAccessTechnologyChangeIndId, KErrNone, data);
	data.Close();
	
	//MockLTSY expect, the CTSY sends down a Event Download to the LTSY
	
	// Create envelope
	TTlv envelope;
	envelope.Begin(KBerTlvEventDownloadTag);
	// Event list
	envelope.AddTag(KTlvEventListTag);
	envelope.AddByte(KEventAccessTechnologyChange);
	// Device identities
	envelope.AddTag(KTlvDeviceIdentityTag);
	envelope.AddByte(KMe);
	envelope.AddByte(KSim);
    // channel status
	envelope.AddTag(KTlvAccessTechnologyTag);
	envelope.AddByte(currentAccessTechnology);
	
	// Prepare data
	
	WaitForEventDownloadL(RSat::KAccessTechnologyChange,envelope.End());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc tests EVENT DOWNLOAD (LOCATION STATUS) and (via SET UP EVENT LIST pcmd)
@SYMTestPriority High
@SYMTestActions Invokes RSat::EventDownload, CallbackSatLocationStatusInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this)); 
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	const TUint8 KPCmdNumber = 123;
	
	SetupEventListL(KPCmdNumber,KEventLocationStatus);


	//first scenario is that there is no location service available.	

    TUint8 locationStatus = KLocationStatusNoService;
    _LIT8(KEmptyOpCode,"");
    TDesC8* operatorCode=&const_cast<TDesC8&>(KEmptyOpCode());
    TUint16 locationAreaCode = 0;
    TUint16 cellId = 0;
    TMockLtsyData4<TUint8, TDesC8*, TUint16, TUint16 > lcnStat(locationStatus,operatorCode,locationAreaCode,cellId);
    data.Close();
    lcnStat.SerialiseL(data);
    
	iMockLTSY.CompleteL(KMockLtsyDispatchSatLocationStatusIndId,KErrNone,data);

	
	// Create envelope
	TTlv envelope;
	envelope.Begin(KBerTlvEventDownloadTag);
	// Event list
	envelope.AddTag(KTlvEventListTag);
	envelope.AddByte(KEventLocationStatus);
	// Device identities
	envelope.AddTag(KTlvDeviceIdentityTag);
	envelope.AddByte(KMe);
	envelope.AddByte(KSim);
    // location status
	envelope.AddTag(KTlvLocationStatusTag);
	envelope.AddByte(locationStatus);
	
	WaitForEventDownloadL(RSat::KLocationStatus,envelope.End());
	
	//second scenario is that location status IS available
	
    locationStatus = KLocationStatusNormalService;
    
    _LIT8(KNonEmptyOperator,"gb");
    operatorCode = &const_cast<TDesC8&>(KNonEmptyOperator());
    locationAreaCode = 18;
    cellId = 19;

	
    data.Close();
    lcnStat.SerialiseL(data);
    
	iMockLTSY.CompleteL(KMockLtsyDispatchSatLocationStatusIndId,KErrNone,data);

	
	//MockLTSY expect, the CTSY sends down a Event Download to the LTSY
	
	// Create envelope
	envelope.Begin(KBerTlvEventDownloadTag);
	// Event list
	envelope.AddTag(KTlvEventListTag);
	envelope.AddByte(KEventLocationStatus);
	// Device identities
	envelope.AddTag(KTlvDeviceIdentityTag);
	envelope.AddByte(KMe);
	envelope.AddByte(KSim);
    // location status
	envelope.AddTag(KTlvLocationStatusTag);
	envelope.AddByte(locationStatus);
	// location info
	envelope.AddTag(KTlvLocationInformationTag); //location information varies depending on access technology
	envelope.AddData(*operatorCode);
	
	envelope.AddByte( TUint8( locationAreaCode >> KShiftBitsToGetMSB ) );// Hi byte
	envelope.AddByte( TUint8( locationAreaCode >> KShiftBitsToGetLSB ) );// Lo byte
	envelope.AddByte( TUint8( cellId >> KShiftBitsToGetMSB ) );          // Hi byte
	envelope.AddByte( TUint8( cellId >> KShiftBitsToGetLSB ) );          // Lo byte

	WaitForEventDownloadL(RSat::KLocationStatus,envelope.End());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/*
 * UNSUPPORTED due to being used internally by sat tsy
 * 
        case ESatGetMeSideSatProfile:
        case ESatClientSatProfileIndication:
        case ESatUsatClientReadyIndication:
        case ESatGetProvisioningRefFile:
        case ESatNotifyRefreshRequired:
        // Notifications
        case ESatNotifyPerformCardApduPCmd:
        case ESatNotifyPowerOffCardPCmd:
        case ESatNotifyPowerOnCardPCmd:
        case ESatNotifyGetReaderStatusPCmd:
        case ESatNotifyRunAtCommandPCmd:
		case ESatNotifyPollingOff:
		case ESatNotifyPollingInterval:
        case ESatNotifyServiceSearchPCmd:
        case ESatNotifyGetServiceInfoPCmd:
        case ESatNotifyDeclareServicePCmd:
        case ESatNotifyProactiveSimSessionStart:
        case ESatNotifyTimerExpiration:
        case ESatNotifyTimerMgmtPCmd:
        case ESatNotifyTsyStateUpdated:
*/

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test of Call Connect and Call Disconnect Events from the LTSY
@SYMTestPriority High
@SYMTestActions Invokes RSat::EventDownload, CallbackSatCallConnectedInd, CallbackSatCallDisconnectedInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	//test call connected
	const TUint8 KPCmdNumber = 124;
	const TUint8 KTransactionId = 111;

	SetupEventListL(KPCmdNumber,KEventCallConnected);
	
	TUint8 transactionId = KTransactionId;
	TBool nearEnd = EFalse; //i.e. MO
	TMockLtsyData2<TUint8,TBool> callConnectedTechPack(transactionId,nearEnd);
	callConnectedTechPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallConnectedIndId, KErrNone, data);
	data.Close();
	
    // Create envelope
    TTlv callConnectEnvelope;
    callConnectEnvelope.Begin( KBerTlvEventDownloadTag );

    // Event list
    callConnectEnvelope.AddTag(KTlvEventListTag);
    callConnectEnvelope.AddByte(KEventCallConnected);
    // Device identities
    callConnectEnvelope.AddTag(KTlvDeviceIdentityTag);
    callConnectEnvelope.AddByte(nearEnd ? KMe : KNetwork);
    callConnectEnvelope.AddByte(KSim);
    // Transaction identifier
    callConnectEnvelope.AddTag(KTlvTransactionIdentifierTag);
    callConnectEnvelope.AddByte(KTransactionId);
    // Prepare data
	
    WaitForEventDownloadL(RSat::KCallConnected,callConnectEnvelope.End());
	
	//test call disconnect
	SetupEventListL(KPCmdNumber,KEventCallDisconnected);

	const TUint8  KMaxCauseLength = 248;
	
	RBuf8 cause;
	CleanupClosePushL(cause);
	
	cause.CreateL(KMaxCauseLength);
	cause.Append(_L8("1234567"));
	TDesC8* causePtr = &cause;
	
	TMockLtsyData3<TUint8,TBool,TDesC8*> callDisconnectedLtsyData(transactionId,nearEnd,causePtr);
	callDisconnectedLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallDisconnectedIndId, KErrNone, data);
	data.Close();
	
    // Create envelope
	TTlv callDisconnectedEnvelope;
	callDisconnectedEnvelope.Begin(KBerTlvEventDownloadTag);
	// Event list
	callDisconnectedEnvelope.AddTag(KTlvEventListTag);
	callDisconnectedEnvelope.AddByte(KEventCallDisconnected);
	// Device identities
	callDisconnectedEnvelope.AddTag(KTlvDeviceIdentityTag);
	callDisconnectedEnvelope.AddByte(nearEnd ? KMe : KNetwork);
	callDisconnectedEnvelope.AddByte(KSim);
	// Transaction identifier
	callDisconnectedEnvelope.AddTag(KTlvTransactionIdentifierTag);
	callDisconnectedEnvelope.AddByte(KTransactionId);
    
	// Cause
	if (cause.Length() != 0)
		{
		callDisconnectedEnvelope.AddTag (KTlvCauseTag);
		callDisconnectedEnvelope.AddData(cause);
		}

	// Prepare data
    WaitForEventDownloadL(RSat::KCallDisconnected,callDisconnectedEnvelope.End());
    AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&cause);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test MT Call event IPC
@SYMTestPriority High
@SYMTestActions Invokes RSat::EventDownload, CallbackSatMtCallInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	const TUint8 KPCmdNumber = 124;
	const TUint8 KTransactionId = 201;
	
	SetupEventListL(KPCmdNumber,KEventMTCall);
	
	//Type of Number (TON) and numbering plan identification (NPI).
	//MSB - 1, Type of Number - 011 (Network Specific Number), Numbering Plan - 1001 (Private numbering plan) 10111001 = B9 Hex
	const TUint8 KTonAndNpi = TonAndNpi(RMobilePhone::ENetworkSpecificNumber,RMobilePhone::EPrivateNumberPlan); 
	
	RBuf8 address;
	CleanupClosePushL(address);
	address.CreateL(KAddrMaxLength);
	address.Append(KTonAndNpi);
	address.Append(_L8("1234567")); //dummy data not EF(ADN) encoded data

	const TInt KSubAddressLength = 100;
	RBuf8 subAddress;
	CleanupClosePushL(subAddress);
	subAddress.CreateL(KSubAddressLength);
	subAddress.Append(_L8("12345678910")); //dummy data not EF(ADN) encoded data
	
	TUint8 transactionId = KTransactionId;
	TDesC8* addressPtr = &address;
	TDesC8* subAddressPtr = &subAddress;

	TMockLtsyData3<TUint8,TDesC8*,TDesC8*> mtCallPack(transactionId,addressPtr,subAddressPtr);
	mtCallPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatMtCallIndId, KErrNone, data);
	data.Close();
	
	// Create envelope
	TTlv envelope;
	envelope.Begin(KBerTlvEventDownloadTag);

	// Event list
	envelope.AddTag(KTlvEventListTag);
	envelope.AddByte(KEventMTCall);

	// Device identities
	envelope.AddTag(KTlvDeviceIdentityTag);
	envelope.AddByte(KNetwork);
	envelope.AddByte(KSim);

	// Transaction identifier
	envelope.AddTag(KTlvTransactionIdentifierTag);
	envelope.AddByte(KTransactionId);

	if(KTonAndNpi != 0)
		{
        envelope.AddTag(KTlvAddressTag);
        envelope.AddByte(KTonAndNpi);
        
        if(address.Length() != 0)
        	{
        	envelope.AddData(address.Mid(1)); //chop the Ton and Npi off
        	}
		}
	
	if(subAddress.Length() != 0)
		{
		envelope.AddTag( KTlvSubaddressTag );
		envelope.AddData(subAddress);
		}
	
	// Prepare data
    WaitForEventDownloadL(RSat::KMTCall,envelope.End());
    AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&subAddress);
	CleanupStack::PopAndDestroy(&address);	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Timezone change IPCs
@SYMTestPriority High
@SYMTestActions Send an indicator to the CTSY with the new time zone - CallbackSatTimeZoneChangeInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
	const TUint8 KTimeZone = 255;
	
	//test and time zone update
	TUint8 timeZone = KTimeZone;
	TMockLtsyData1<TUint8> timeZonePack(timeZone);
	timeZonePack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatTimeZoneChangeIndId, KErrNone, data);
	data.Close();
	
	WaitForMockLTSYTerminated();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc SET UP EVENT LIST proactive command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	const TUint8 KPCmdNumber = 123;
	
	//setup the event list
	SetupEventListL(KPCmdNumber,KEventMTCall);

	//"The list of events provided by the UICC in the last SET UP EVENT LIST
	//command shall be removed if the terminal is powered off or the UICC is removed or a reset is performed."

	//clear the event list
	iMockLTSY.CompleteL(KMockLtsyDispatchSatRemoveEventListIndId,KErrNone);

	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Testing RSat::NotifyProactiveSimSessionEnd support
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyProactiveSimSessionEnd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0008L()
	{
	
	//RSat::NotifyProactiveSimSessionEnd Inroduced in version 6.1, marked as 
	//"no longer needed when using the ETelSat API" from version 8.2

	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
	TUint8 statusWord1 = 0x90;
	TUint8 statusWord2 = 0x00;

	TUint8 proactiveCommandType = KEndOfTheProactiveSession;
	
	//this is as a result of the NotifyProactiveSimSessionEnd call down to the LTSY,
	//this tells the LTSY we are interested in a Sim Session End event
	//CTSY -> LTSY
	TMockLtsyData1<TUint8> proactiveCommandTypeDataPack(proactiveCommandType);
	proactiveCommandTypeDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	

	//the client side call to be notifed when the Proactive Sim Session has ended.
	iSat.NotifyProactiveSimSessionEnd(reqStatus);
	
	//sometime later the Sim Session ends, indicator up
	//LTSY -> CTSY
	TMockLtsyData2<TUint8,TUint8> notifyProactiveSimSessionEndDataPack(statusWord1,statusWord2);
	notifyProactiveSimSessionEndDataPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId,KErrNone,data);
	data.Close();
	
	//wait for the client side to be notified
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}



/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify local info (network measurement result) IPC
@SYMTestPriority High
@SYMTestActions Invokes CallbackSatPcmdInd, CallbackSatLocalInformationNmrComp
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0009L()
	{
	/*
	 * 
	 * IPCS that are exercised
	 * 
			ESatTsyLocalInformationNmr:
            ESatTsyNotifyLocalInfo: 
 	*/
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	//provide local info (network measurement)
	const TUint8 KPCmdNumber = 105;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	tlv.AddByte(RSat::KProvideLocalInformationNmr);//ETLV_CommandQualifier / KCommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);//ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
		
	//Now send the terminal response
	iMockLTSY.ExpectL(MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId);	

	
	_LIT8(KNmrResults,"abc");
	TDesC8* nmr = &const_cast<TDesC8&>(KNmrResults());
	_LIT(KBcchList,"def");
	TDesC* bcch = &const_cast<TDesC&>(KBcchList());
	TMockLtsyData2<TDesC8*,TDesC*> nmrPack(nmr,bcch);
	data.Close();
	nmrPack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId, KErrNone, data);
	
	TTlv tlvAddInfo;
	tlvAddInfo.AddTag( KTlvNetworkMeasurementResultsTag );
	tlvAddInfo.AddData(KNmrResults);
	tlvAddInfo.AddTag (KTlvBccdChannelListTag);
	
	//convert the BCCH data to the right format
	TInt i( 0 );
	TInt mask( 3 );
	TInt shiftMove( 2 );
	TUint8 nextChar( 0 );
	  
		// Convert BCCH channel list from 16-bit to 10-bit format
	for ( i = 0; i < bcch->Length(); i++ )
	    {
	    TUint char2 =  ( (*bcch)[i] & 0x03FF );
	    TUint8 char1 = static_cast<TUint8>( char2 >> shiftMove );
	
	    // Append byte
	    tlvAddInfo.AddByte( 
	    static_cast<TUint8>( char1 | nextChar ) );
	
	    nextChar = static_cast<TUint8>( ( char2 & mask )  
	    		<< ( 8 - shiftMove ) );
	
		if ( 8 == shiftMove )
		    {
		    shiftMove = 2;
		    nextChar = 0;
		    tlvAddInfo.AddByte( 
		        static_cast<TUint8> ( char2 & 0x00FF ) );
		    mask = 3;
		    }
		else
		    {
		    shiftMove += 2;
		    mask = ( 1L << shiftMove ) - 1;
		    }
		}
	// Add last byte
	tlvAddInfo.AddByte( nextChar );
		
	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInformationNmr,
								RSat::ELocalInfo,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								KNullDesC8,
								tlvAddInfo.GetDataWithoutTopLevelTag(),
								KErrNone);
	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify local info (timing advance) IPCs
@SYMTestPriority High
@SYMTestActions Invokes CallbackSatPcmdInd, CallbackSatTimingAdvanceComp
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	//provide local info (network measurement)
	
	
	const TUint8 KPCmdNumber = 109;
	const TUint8 KTimingAdvance = 47;
	const TUint8 KMeStatus = 0x01;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
											//ETLV_CommandQualifier / KCommandQualifier
	tlv.AddByte(RSat::KProvideLocalInfoTimingAdv);
	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);//ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);	
	
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId);
	

	TUint8 timingAdvance = KTimingAdvance;
	TUint8 meStatus = KMeStatus;
	TMockLtsyData2<TUint8,TUint8> nmrPack(timingAdvance,meStatus);
	data.Close();
	nmrPack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId, KErrNone, data);
	
	
	const TDesC8& KGeneralResultAddInfo = KNullDesC8;
	
	TTlv tlvAddInfo;
	tlvAddInfo.AddTag( KTlvTimingAdvanceTag );
	tlvAddInfo.AddByte(KMeStatus); //me status
	tlvAddInfo.AddByte(KTimingAdvance); //timing advance
	
	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInfoTimingAdv,
								RSat::ELocalInfo,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								KNullDesC8,
								tlvAddInfo.GetDataWithoutTopLevelTag(),
								KErrNone);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify local info access technology ipcs
@SYMTestPriority High
@SYMTestActions Invokes CallbackSatPcmdInd, CallbackSatAccessTechnologyChangeInd
@SYMTestExpectedResults Pass
@SYMTestType UT
 */

void CCTsySatFU::TestUnit0011L()
	{
	/*
	 * 
	 * IPCS that are exercised in this unit test
	 * 
	 * 		ESatTsyProvideLocalInfo:
	 *		ESatTsyAccTech:
	 */
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	//provide local info (network measurement)
	
	
	const TUint8 KPCmdNumber = 129;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	tlv.AddByte(RSat::KProvideLocalInfoAccTech); //ETLV_CommandQualifier / KCommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);//ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe);

	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);	
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId);
	

	const TUint8 KAccessTechnology = 47;
	
	TUint8 accessTechnology = KAccessTechnology;
	TMockLtsyData1<TUint8> accessTechnologyPack(accessTechnology);
	data.Close();
	accessTechnologyPack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId, KErrNone, data);
	
	const TDesC8& KGeneralResultAddInfo = KNullDesC8;
	    
	TTlv tlvAddInfo;
    tlvAddInfo.AddTag( KTlvAccessTechnologyTag  );
    tlvAddInfo.AddByte( KAccessTechnology ); //access tech
	
	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInfoAccTech,
								RSat::ELocalInfo,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								KNullDesC8,
								tlvAddInfo.GetDataWithoutTopLevelTag(),
								KErrNone);
	
	AssertMockLtsyStatusL();
	
	//test with completing with an Error
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);	

	iMockLTSY.ExpectL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId);
	data.Close();
	accessTechnologyPack.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId, KErrGeneral, data);

	TBuf8<1> generalResultAddInfo;
	generalResultAddInfo.Append(RSat::KNoSpecificMeProblem);
	
	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInfoAccTech,
								RSat::ELocalInfo,
								RSat::KMeUnableToProcessCmd,
								generalResultAddInfo,
								KNullDesC8,
								KNullDesC8, //no additional info
								KErrNone);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ESatTsyEnvelopeError api
@SYMTestPriority High
@SYMTestActions Invokes CallbackSatLocationStatusInd, CallbackSatCreateCallControlEnvelopeInd, HandleUssdControlEnvelopeErrorReqL
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
	TRequestStatus stat;
	
	
	//first we need to provide local info so that it can be used during cc events

	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);
		
	//************************************************************************************
	//Now we do a SEND USSD proactive command
	
	TUint8 pcmdCode = KSendUssd;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	
	RSat::TSendUssdV6 ussd;
	RSat::TSendUssdV6Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(stat, ussdPck);
	
	const TInt KSendUssdPCmdNo = 205;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KSendUssdPCmdNo);//ETLV_CommandNumber
	tlv.AddByte(pcmdCode); //ETLV_TypeOfCommand
	tlv.AddByte(KSetUpCallDisconnectingOtherCalls); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KNetwork); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvUssdStringTag);
	const TUint8 KTonNpi = 0x91; //0b10010001;
	tlv.AddByte(KTonNpi); //TON and NPI (national number that conforms to the itu telephony style of numbering)

	_LIT8(KSetUpCallNo8 ,"012345678hjhj");
	//_LIT16(KSetUpCallNo16,"012345678hjhj"); 
	TBuf8<KMaxAddressLength> bcdSetUpCallNo;
	TBuf16<KMaxAddressLength> bcdSetUpCallNo16;
	TSatUtility::AsciiToBCD(KSetUpCallNo8,bcdSetUpCallNo);
	bcdSetUpCallNo16.Copy(bcdSetUpCallNo);
	tlv.AddData(bcdSetUpCallNo);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	
	User::WaitForRequest(stat);
	ASSERT_EQUALS(stat.Int(),KErrNone);
	
	
	//******************************************************************************************************
	//* At this stage the phone application/system software would attempt to make the call specified in the 
	//* SEND USSD pcmd payload, first checking whether this is the emergency number (112) per ETSI TS 102 223 
	//* section 6.4.13 and routing the call via the appropriate ETel API, i.e. RMobileCall::DialEmergencyCall()
	//* for emergency calls and RCall::Dial() for standard ones.  
	//*
	//* After checking for emergency numbers and against the FDN list, the LTSY uses the CreateEnvelopeInd IPC to
	//* request that the CTSY constructs TLV data for an ENVELOPE (CALL CONTROL). Once this is done it passes it 
	//* to the LTSY with the SendEnvelopeReq(IPC).
	//*
	//******************************************************************************************************
	
	
	
	//**********************************************************************************
	//send up the "create envelope" event which we know the CTSY won't be able to create,
	//due to USSD strings not being supported.
	_LIT8(KBcc1,"asdas");
	_LIT8(KBcc2,"qwqwqw");
	TDesC8* bcc1Ptr = const_cast<TDesC8*>(&KBcc1);
	TDesC8* bcc2Ptr = const_cast<TDesC8*>(&KBcc2);
	TDesC8* addrPtr = const_cast<TDesC8*>(&KSetUpCallNo8);
	TUint8 tag = KTlvUssdStringTag;  
	const TUint8 KDcs = 56;
	TUint8 tonNpi = KTonNpi;
	TUint8 dcs = KDcs;
	TMockLtsyData6<TUint8, TDesC8*, TDesC8*, TDesC8*, TUint8, TUint8> ccInfoPck(tag, bcc1Ptr, bcc2Ptr, addrPtr, tonNpi, dcs);
	
	data.Close();
	ccInfoPck.SerialiseL(data);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateCallControlEnvelopeIndId,KErrNone,data);

	
	//**********************************************************************************
	//Now report the error creating the USSD envelope.
	
	iMockLTSY.ExpectL(MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId,KErrNone);
	
	
	WaitForMockLTSYTerminated();
	
	
	CleanupStack::PopAndDestroy(&completeData);
	CleanupStack::PopAndDestroy(&expectData);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}




/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ESatNotifySetupCall (interaction between setup call proactive command and the call connected event)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpCallPCmd, RSat::NotifyCallControlRequest
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	iCallContainer = new (ELeave) CCallTypeContainer();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	TRequestStatus reqStatus;
	
	//**************************************************************************
	//first we need to provide local info so that it can be used during cc events 

	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);

	//*****************************************************
	//now we set up an event list containing the call connected event
	const TInt KPCommandNumber = 135;
	SetupEventListL(KPCommandNumber,KEventCallConnected);
	//************************************************************************************
	//Now we do a setup call proactive command
	
	TUint8 pcmdCode = KSetUpCall;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	//iMockLTSY.ExpectL(MLtsyDispatchSatNotifySetUpCallPcmd::KLtsyDispatchSatNotifySetUpCallPcmdApiId);
	
	iSat.NotifySetUpCallPCmd(reqStatus, iCallContainer->iSetUpCallPckg);
	AssertMockLtsyStatusL(); //LTSY should be empty KLtsyDispatchSatPCmdNotificationApiId expect consumed by the NotifySetUpCallPCmd
	
	const TUint8 KSetUpCallPCmdNo = 201;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KSetUpCallPCmdNo);//ETLV_CommandNumber
	tlv.AddByte(pcmdCode); //ETLV_TypeOfCommand
	tlv.AddByte(KSetUpCallDisconnectingOtherCalls); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KNetwork); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAddressTag);
	const TUint8 KTonNpi = 0x91; //0b10010001;
	tlv.AddByte(KTonNpi); //TON and NPI (national number that conforms to the itu telephony style of numbering)
	
	_LIT8(KSetUpCallNo,"012345678"); 
	TBuf8<KMaxAddressLength> bcdSetUpCallNo;
	TSatUtility::AsciiToBCD(KSetUpCallNo,bcdSetUpCallNo);
	tlv.AddData(bcdSetUpCallNo);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	data.Close();
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	User::WaitForRequest(reqStatus); //NotifySetUpCallProactiveCommand
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(KSetUpCallPCmdNo, iCallContainer->iSetUpCall.PCmdNumber());
	AssertMockLtsyStatusL();
	//set up to listen for call control
	

	iSat.NotifyCallControlRequest(reqStatus, iCallContainer->iCallControlPckg);
	
	//**********************************************************************************
	//send up the "create envelope" event which causes the ctsy to assemble the ENVELOPE (CALL CONTROL)
	_LIT8(KBcc1,"asdas");
	_LIT8(KBcc2,"qwqwqw");
	TDesC8* bcc1Ptr = const_cast<TDesC8*>(&KBcc1);
	TDesC8* bcc2Ptr = const_cast<TDesC8*>(&KBcc2);
	TDesC8* addrPtr = const_cast<TDesC8*>(&KSetUpCallNo);
	TUint8 tag = KTlvAddressTag;  
	const TUint8 KDcs = 56;
	TUint8 tonNpi = KTonNpi;
	TUint8 dcs = KDcs;

	TMockLtsyData6<TUint8, TDesC8*, TDesC8*, TDesC8*, TUint8, TUint8> ccInfoPck(tag, bcc1Ptr, bcc2Ptr, addrPtr, tonNpi, dcs);
	data.Close();
	ccInfoPck.SerialiseL(data);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateCallControlEnvelopeIndId,KErrNone,data);
	//**********************************************************************************
	//Now send down the created envelope 
	
	tlv.Begin( KBerTlvCallControlTag );
	// Device identities
	tlv.AddTag( KTlvDeviceIdentityTag );
	tlv.AddByte( KMe );
	tlv.AddByte( KSim );
	// Address
	tlv.AddTag( KTlvAddressTag );
	tlv.AddByte( KTonNpi ); 
	tlv.AddData( bcdSetUpCallNo );
	// Optional
	tlv.AddTag( KTlvCapabilityConfigurationParametersTag );
	tlv.AddData( KBcc1 );
	// Add location information data
	tlv.AddTag( KTlvLocationInformationTag );
	// Append Mobile Country  & Network Codes, 3 bytes
	tlv.AddData(KOperatorCode );
	// Append Location Area Code (LAC)
	tlv.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetMSB ) );//msb
	tlv.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetLSB ) );//lsb
	// Append Cell Id
	tlv.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetMSB ) ); // msbthey
	tlv.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetLSB ) ); // lsb
		 
	tlv.AddTag( KTlvCapabilityConfigurationParametersTag );
	tlv.AddData( KBcc2 ); 
	
	// Prepare data
	TDesC8* tlvDscDes = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscDesPck(tlvDscDes);
	tlvDscDesPck.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId,expectData);
	
	//let the envelope IPCs go up and down
	WaitForMockLTSYTerminated();
	data.Close();
	expectData.Close();
	//************************************************************************************
	//Now we send up the response of the call control
	
	tlv.Begin(0x02);//the type of request has changed (the envelope responses have a slightly different format to the rest of the data so we achieve this by passing in call control result as the top level tag)
	tlv.AddTag(KTlvAddressTag); 
	tlv.AddByte(KTonNpi); //ton+npi
	tlv.AddData(bcdSetUpCallNo);
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddData(_L8("Test alpha id"));
	tlvDscDes = &const_cast<TDesC8&>(tlv.End());
	//RSat::TControlResult controlResult=RSat::EAllowedWithModifications;
	
	TMockLtsyData1<TDesC8*> controlPck(tlvDscDes);
	controlPck.SerialiseL(completeData);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallControlEnvelopeResponseIndId,KErrNone,completeData); 
	//**********************************************************************
	// we now get a call connected event after the potentially modified call has been dialled
	TUint8 transactionId = 111;
	TBool nearEnd = EFalse;  //i.e. this is a MO call
	TMockLtsyData2<TUint8,TBool> callConnectedTechPack(transactionId,nearEnd);
	callConnectedTechPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallConnectedIndId, KErrNone, data);
	
	User::WaitForRequest(reqStatus); //NotifyCallControlRequest
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();
	data.Close();
	completeData.Close();
	//************************************************************************************
	//now we need to provide the terminal response to the SET UP CALL proactive command;
	//when the terminal response is sent, it will trigger the sending of the saved 
	//call connected event envelope

	//now fire off terminal response
	
	RSat::TSetUpCallRspV2 setUpCallRsp;
	RSat::TSetUpCallRspV2Pckg setUpCallRspPckg(setUpCallRsp);
	
	const RSat::TPCmdResult KPCmdResult = RSat::KSuccess;
	
	const TDesC8& KGeneralResultAddInfo =  KNullDesC8();
	 
	setUpCallRsp.iGeneralResult = KPCmdResult;
	setUpCallRsp.iInfoType = RSat::KNoAdditionalInfo;
	setUpCallRsp.SetPCmdNumber(KSetUpCallPCmdNo);
	
	
	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
	//void GenerateTermRspTlv(RBuf8& buf, TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier, RSat::TPCmd aRSatTypeOfCommand, RSat::TPCmdResult aResult, const TDesC8& aRsp, const TDesC8& aAdditionalInfo);
	GenerateTermRspTlv(terminalRsp, KSetUpCallPCmdNo, KSetUpCall, KSetUpCallDisconnectingOtherCalls, KPCmdResult, KGeneralResultAddInfo, KNullDesC8());
	
	TDesC8* terminalRspPtr = &terminalRsp;
	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);
	
	//following the dispatch terminal response ipc, we get the event envelope in response to call connected
	TTlv callConnectEnvelope;
	callConnectEnvelope.Begin( KBerTlvEventDownloadTag );
	
	// Event list
	callConnectEnvelope.AddTag(KTlvEventListTag);
	callConnectEnvelope.AddByte(KEventCallConnected);
	// Device identities
	callConnectEnvelope.AddTag(KTlvDeviceIdentityTag);
	callConnectEnvelope.AddByte(nearEnd ? KMe : KNetwork);
	callConnectEnvelope.AddByte(KSim);
	// Transaction identifier
	callConnectEnvelope.AddTag(KTlvTransactionIdentifierTag);
	callConnectEnvelope.AddByte(transactionId);
	// Prepare data
	//WaitForEventDownloadL(RSat::KCallConnected,);
	TPtrC8 ccEnv(callConnectEnvelope.End());
	TDesC8* ccEnvPtr = &ccEnv;
	    
	TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(ccEnvPtr);
	eventDownloadExpLtsyData.SerialiseL(expectData);
	iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,expectData);
	//after that we can complete the terminal response ipc
	
	iMockLTSY.CompleteL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, KErrNone);

	TRequestStatus reqStatusNotifyTerminated;
	iMockLTSY.NotifyTerminated(reqStatusNotifyTerminated); 
	
	iSat.TerminalRsp(reqStatus, RSat::ESetUpCall, setUpCallRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	User::WaitForRequest(reqStatusNotifyTerminated);
	ASSERT_EQUALS(reqStatusNotifyTerminated.Int(),KErrNone);
	
	
	AssertMockLtsyStatusL();
	
	//****************
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&completeData);
	CleanupStack::PopAndDestroy(&expectData);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}




/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for refresh
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyRefreshRequired, RSat::NotifyRefreshPCmd, RSat::NotifyRefreshAllowed, RSat::TerminalRsp
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySatFU::TestUnit0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	const TUint8 KPCmdNumber1 = 101;
	const TUint8 KCommandQualifier1 = KSimInitFullFileChangeNotification; //zero is NAA Initialization and Full File Change Notification
	
	RefreshNoTerminalRspL(KPCmdNumber1,KCommandQualifier1,KErrNotReady);
	
	//ESatTsyRefresh
	//indicator to create the terminal response.
	const TUint8 KGeneralResultAddInfo = 0x3D;

	TBuf8<RSat::KAdditionalInfoMaxSize> additionalInfo8;
	additionalInfo8.Append(KGeneralResultAddInfo);
	
    RSat::TAdditionalInfo additionalInfo;
    additionalInfo.Append(KGeneralResultAddInfo);
    
    RSat::TPCmdResult generalResult = RSat::KMeUnableToProcessCmd;
    TDesC* additionalInfoPtr = &additionalInfo;
    TMockLtsyData2<RSat::TPCmdResult,TDesC*> refreshDataPck(generalResult,additionalInfoPtr);
    refreshDataPck.SerialiseL(data);	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateRefreshTerminalRspIndId,KErrNone,data);
	data.Close();	
			
	const TUint8 KRefreshCommandQualifier = 0x00; // 00 = NAA Initialization and Full File Change Notification
	GenerateAndWaitForTermRspL(KPCmdNumber1,KRefresh,KRefreshCommandQualifier,RSat::ERefresh,generalResult,additionalInfo8,KNullDesC8,KNullDesC8,KErrNone);

	/********************************************************************/
	
	const TUint8 KPCmdNumber2 = 102;
	const TUint8 KCommandQualifier2 = KSimInit; //zero is NAA Initialization and Full File Change Notification

	RefreshNoTerminalRspL(KPCmdNumber2,KCommandQualifier2,KErrNone);

	//ESatTsyImsiChanged
	iMockLTSY.CompleteL(KMockLtsyDispatchSatImsiChangedIndId,KErrNone);
	WaitForMockLTSYTerminated();
	
	//once finished generate the Terminal response	
	RSat::TRefreshRspV1 refreshTerminalRsp;
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber1);

	RSat::TRefreshRspV1Pckg refreshTerminalRspPckg(refreshTerminalRsp);
	
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::ERefresh, refreshTerminalRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify local info - location information request (i.e. cell id etc)
@SYMTestPriority High
@SYMTestActions Invokes CallbackSatProvideLocationInfoComp
@SYMTestExpectedResults Pass
@SYMTestType UT
 */

void CCTsySatFU::TestUnit0015L()
	{
	/*
	 * 
	 * IPCS that are exercised in this unit test
	 * 
	 * 		ESatTsyProvideLocalInfo:
	 *		ESatTsyAccTech:
     */
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	//provide local info (location info)
	
	
	const TUint8 KPCmdNumber = 142;
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	tlv.AddByte(RSat::KProvideLocalInfo); //ETLV_CommandQualifier / KCommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);//ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe);

	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId);
	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	TDesC8* opCodePtr = const_cast<TDesC8*>(&KOperatorCode);
	TUint16 lac = KLocationAreaCode;
	TUint16 cellId = KCellId;
	TMockLtsyData3<TUint16,TUint16,TDesC8* > locationInfoPCmdDataPack(	lac,
			cellId, opCodePtr);
	data.Close();
	locationInfoPCmdDataPack.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId, KErrNone, data);
 
    
    

    const TDesC8& KGeneralResultAddInfo = KNullDesC8;
    
	TTlv tlvAddInfo;
    tlvAddInfo.AddTag( KTlvLocationInformationTag  );
    tlvAddInfo.AddData( KOperatorCode ); 
    
    tlvAddInfo.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetMSB ) );//msb
    tlvAddInfo.AddByte( static_cast<TUint8>( KLocationAreaCode >> KShiftBitsToGetLSB ) );//lsb
    tlvAddInfo.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetMSB ) );//msb
    tlvAddInfo.AddByte( static_cast<TUint8>( KCellId >> KShiftBitsToGetLSB ) );//lsb

	GenerateAndWaitForTermRspL(	KPCmdNumber,
								KProvideLocalInformation,
								RSat::KProvideLocalInfo,
								RSat::ELocalInfo,
								RSat::KSuccess,
								KGeneralResultAddInfo,
								KNullDesC8,
								tlvAddInfo.GetDataWithoutTopLevelTag(),
								KErrNone);
								
    
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for the ESatTsySsStatus api
@SYMTestPriority High
@SYMTestActions Invokes 
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUnit0016L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	DispatcherSat::TSsStatus status = DispatcherSat::ENotBusy;
	TMockLtsyData1<DispatcherSat::TSsStatus> ltsyData(status);
	ltsyData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(reqStatus); 
	iMockLTSY.CompleteL(KMockLtsyDispatchSatSsChangeIndId, KErrNone, data);
	data.Close();
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	status = DispatcherSat::ESsBusy;
	ltsyData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(reqStatus); 
	iMockLTSY.CompleteL(KMockLtsyDispatchSatSsChangeIndId, KErrNone, data);
	data.Close();
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);

	status = DispatcherSat::EUssdBusy;
	ltsyData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(reqStatus); 
	iMockLTSY.CompleteL(KMockLtsyDispatchSatSsChangeIndId, KErrNone, data);
	data.Close();
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SAT-UN0017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SatRefreshCompleteNotification()
@SYMTestPriority High
@SYMTestActions Invokes  RMmCustomAPI::SatRefreshCompleteNotification()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySatFU::TestUnit0017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RBuf8 data;
	CleanupClosePushL(data);
		
	TRequestStatus reqStatus;
	
	customApi.SatRefreshCompleteNotification(reqStatus);
	
	const TUint8 KPCmdNumber = 101;
	const TUint8 KCommandQualifier = KSimInitFullFileChangeNotification; //zero is NAA Initialization and Full File Change Notification
	
	RefreshNoTerminalRspL(KPCmdNumber, KCommandQualifier, KErrNone);
	
	//can't wait here known defect 403701, see CCTsyCustomIPCFU::TestSatRefreshCompleteNotification0001L
	//User::WaitForRequest(reqStatus);
	//ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	//Do the Client side refresh
	
	
	//once finished generate the Terminal response	

	RSat::TRefreshRspV1 refreshTerminalRsp;
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber);

	RSat::TRefreshRspV1Pckg refreshTerminalRspPckg(refreshTerminalRsp);
	
	GenerateAndWaitForTermRspL(KPCmdNumber,KRefresh,KCommandQualifier,RSat::ERefresh,RSat::KSuccess,refreshTerminalRspPckg,KErrNone);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}


/**
 * Utility function which handles all the Expects/Completes generated by Opening an RSat object.
 */
void CCTsySatFU::OpenSatL(TUint8 aEnabled)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
	
	TBool smsMoControlActivated = aEnabled & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = aEnabled & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = aEnabled & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	
	ASSERT_EQUALS(KErrNone, iSat.Open(iPhone));

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	CleanupStack::PopAndDestroy(&data); 
	}

void CCTsySatFU::SetupEventListL(TUint8 aPCommandNumber, TUint8 aEvent)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
		
	//setup the expect for Mock LTSY from the RSat::NotifySetUpEventListPCmd call
	//ETel -> CTSY -> LTSY
	TUint8 pcmdCode = KSetUpEventList;
	TMockLtsyData1<TUint8> setUpEventListPCmdExpLtsyData(pcmdCode);
	setUpEventListPCmdExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	
	//create the Mock LTSY complete TLV data (a proactive command)
	//LTSY -> CTSY
	TTlv proactiveCmdTlv;
	
	proactiveCmdTlv.Begin(KBerTlvProactiveSimCommandTag);

	proactiveCmdTlv.AddTag(KTlvCommandDetailsTag);
	proactiveCmdTlv.AddByte(aPCommandNumber);//ETLV_CommandNumber
	proactiveCmdTlv.AddByte(KSetUpEventList); //ETLV_TypeOfCommand
	proactiveCmdTlv.AddByte(0); //ETLV_CommandQualifier - RFU for Set up event list
	
	proactiveCmdTlv.AddTag(KTlvEventListTag);
	proactiveCmdTlv.AddByte(aEvent); //ETLV_TypeOfCommand

	proactiveCmdTlv.AddTag(KTlvDeviceIdentityTag);
	proactiveCmdTlv.AddByte(KSim);
	proactiveCmdTlv.AddByte(KMe);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(proactiveCmdTlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();
	
	//ETel Sat calls
	RSat::TSetUpEventListV1 setUpEventList;
	RSat::TSetUpEventListV1Pckg setUpEventListPckg(setUpEventList);
	
	iSat.NotifySetUpEventListPCmd(reqStatus,setUpEventListPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(setUpEventList.iType,RSat::EUpdateEventList);
	//Can't be asserted (easily) as some events the TSY is completly responsible for and iEvent is not set in the CTSY
	//ASSERT_TRUE((setUpEventList.iEvents & RSat::KUserActivity) == RSat::KUserActivity)
	ASSERT_EQUALS(setUpEventList.PCmdNumber(),aPCommandNumber);
	
	AssertMockLtsyStatusL();
	
	//The Terminal response
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(aPCommandNumber);

	GenerateAndWaitForTermRspL(aPCommandNumber,KSetUpEventList,0,RSat::ESetUpEventList,RSat::KSuccess,respPck,KErrNone);

	CleanupStack::PopAndDestroy(&data);
	}

void CCTsySatFU::SetupLocalInfoL(TUint16 aAreaCode, TUint16 aCellId, const TDesC8& aOperatorCode, TUint8 aLocationStatus)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	ASSERT_TRUE(aOperatorCode.Length() <= KOperatorCodeLength);
	
	TDesC8* operatorCode = &const_cast<TDesC8&>(aOperatorCode); 
	
	TMockLtsyData4<TUint8, TDesC8*, TUint16, TUint16 >localInfoPack(aLocationStatus,operatorCode,aAreaCode,aCellId);
	localInfoPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatLocationStatusIndId, KErrNone, data);
	
	WaitForMockLTSYTerminated();
	
	CleanupStack::PopAndDestroy(&data);
	}

void CCTsySatFU::WaitForEventDownloadL(RSat::TEventList aEventList, const TDesC8& aEventDownloadEnvelope)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	TDesC8* envelopePtr = &const_cast<TDesC8&>(aEventDownloadEnvelope);
        
    TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
    eventDownloadExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
	data.Close();
	
	switch(aEventList)
		{
		case RSat::KUserActivity:
			{
			//ETel calls event download
			RSat::TEventDownloadBaseV2      eventDownload;
			RSat::TEventDownloadBaseV2Pckg  eventDownloadPck(eventDownload);
			RSat::TEventList eventList = aEventList;

			iSat.EventDownload(reqStatus, eventList, eventDownloadPck); // No associated data

			AssertMockLtsyStatusL(); //RSat::EventDownload should consume the queue expect of KLtsyDispatchSatEventDownloadEnvelopeApiId IPC
			
			User::WaitForRequest(reqStatus);
			ASSERT_EQUALS(reqStatus.Int(),KErrNone);
			ASSERT_FALSE(eventDownload.iDataAvailable);
			break;
			}
		case RSat::KAccessTechnologyChange:
		case RSat::KLocationStatus:
		case RSat::KCallConnected:
		case RSat::KCallDisconnected:
		case RSat::KMTCall:
			{
			//ETel notifiers not currently supported
			WaitForMockLTSYTerminated();
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * Handles sending a terminal response, generating a "command completed successfully" additional info tlv
 */
void CCTsySatFU::GenerateAndWaitForTermRspL(	TUint8 aPCmdNumber,
											TUint8 aTypeOfCommand, 
											TUint8 aCommandQualifier, 
											RSat::TPCmd aRSatTypeOfCommand,
											RSat::TPCmdResult aGeneralResult,
											const TDesC8& aRsp,
											TInt aCompError)
	{
	GenerateAndWaitForTermRspL(aPCmdNumber, aTypeOfCommand, aCommandQualifier, aRSatTypeOfCommand, aGeneralResult, KNullDesC8, aRsp, KNullDesC8,aCompError);
	}

/**
 * Handles sending a terminal response, and takes as an argument a descriptor containing a tlv. This is to be used when a "command completed successfully" tlv
 * is insufficient.
 */
void CCTsySatFU::GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,
											TUint8 aTypeOfCommand, 
											TUint8 aCommandQualifier,
											RSat::TPCmd aRSatTypeOfCommand,
											RSat::TPCmdResult aGeneralResult,
											const TDesC8& aGeneralResultAddInfo,
											const TDesC8& aRsp,
											const TDesC8& aAdditionalInfo,
											TInt aCompError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
	
	GenerateTermRspTlv(terminalRsp, aPCmdNumber, aTypeOfCommand, aCommandQualifier, aGeneralResult, aGeneralResultAddInfo, aAdditionalInfo);
	
	TDesC8* terminalRspPtr = &terminalRsp;
	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);
	
	if(aRsp != KNullDesC8)
		{
		//ETel generated Terminal response
		iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRsp);
		}
	else
		{
		//CTSY generate Terminal response
		iMockLTSY.NotifyTerminated(reqStatus); 
		}
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	iMockLTSY.NotifyTerminated(reqStatus); 
	
	iMockLTSY.CompleteL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, aCompError);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}

void CCTsySatFU::GenerateTermRspTlv(	RBuf8& aBuf, 
										TUint8 aPCmdNumber,
										TUint8 aTypeOfCommand, 
										TUint8 aCommandQualifier, 
										RSat::TPCmdResult aGeneralResult, 
										const TDesC8& aGeneralResultAddInfo, 
										const TDesC8& aAdditionalInfo)
	{
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(aTypeOfCommand); //ETLV_TypeOfCommand
	tlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
	
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	tlv.AddData(aGeneralResultAddInfo);
	
	aBuf.Zero();
	aBuf.Append(tlv.GetDataWithoutTopLevelTag());
	aBuf.Append(aAdditionalInfo);
	}

void CCTsySatFU::RefreshNoTerminalRspL(TUint8 aPCmdNumber, TUint8 aCommandQualifier, TInt aRefreshAllowedCompleteResult)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//this is an example of how a other interested parties can register for NotifyRefreshPCmd,
	//this would be any other but the main SAT engine.
	RSat satUser;
	CleanupClosePushL(satUser);
	satUser.Open(iPhone);
	
	//Expect a MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId when both
	//notifiers are set (thus needs to be queued before we setup the two notifiers below)
	TUint8 pcmdCode = KRefresh;
	TMockLtsyData1<TUint8> proactiveCmdNotificationsLtsyData(pcmdCode);
	proactiveCmdNotificationsLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	
	//Refresh Required client side notifier
	//ETel -> CTSY
	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPckg(refreshRequired);
	TRequestStatus refreshRequiredReqStatus;
	iSat.NotifyRefreshRequired(refreshRequiredReqStatus,refreshRequiredPckg);
	
	//Refresh Proactive Command client side notifier 
	//ETel -> CTSY
	//Not both have been set, this will trigger a HandlePCmdNotificationReqL() down to the LTSY
	RSat::TRefreshV2 refreshPCmd;
	RSat::TRefreshV2Pckg refreshPCmdPckg(refreshPCmd);
	TRequestStatus refreshPCmdReqStatus;
	iSat.NotifyRefreshPCmd(refreshPCmdReqStatus,refreshPCmdPckg);
	
	RSat::TRefreshV2 refreshPCmd2;
	RSat::TRefreshV2Pckg refreshPCmdPckg2(refreshPCmd2);
	TRequestStatus refreshPCmdReqStatus2;
	satUser.NotifyRefreshPCmd(refreshPCmdReqStatus2,refreshPCmdPckg2);
	
	//the MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId expect should be gone
	//so assert the LTSY status
	AssertMockLtsyStatusL();
	
	
	TTlv proactiveCmdTlv;
	
	proactiveCmdTlv.Begin(KBerTlvProactiveSimCommandTag);

	proactiveCmdTlv.AddTag(KTlvCommandDetailsTag);
	proactiveCmdTlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	proactiveCmdTlv.AddByte(KRefresh); //ETLV_TypeOfCommand
	proactiveCmdTlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier
	
	proactiveCmdTlv.AddTag(KTlvDeviceIdentityTag);
	proactiveCmdTlv.AddByte(KSim);
	proactiveCmdTlv.AddByte(KMe);

	TUint numberOfFiles = 1;
	proactiveCmdTlv.AddTag(KTlvFileListTag);
	proactiveCmdTlv.AddByte(numberOfFiles);
	//full paths to files (first byte identifies the type of file)
	//master file
	
	const TInt KFilePathLength = 4;
	TBuf8<KFilePathLength> filePath(KFilePathLength);
	filePath[0] = 0x3F;
   	filePath[1] = 0x00;
   	filePath[2] = 0x2F;
   	filePath[3] = 0xE2;
	
   	proactiveCmdTlv.AddData(filePath);

   	_LIT8(KAid,"AID INFO"); //not correct format, just for test check "Application identifier as defined in TS 101 220 [31]"
	proactiveCmdTlv.AddTag(KTlvAIDTag);
   	proactiveCmdTlv.AddData(KAid);
   	
   	
	const TUint8 KTextAttributeTag = 0xC8;
	proactiveCmdTlv.AddTag(KTextAttributeTag);
	//text formatting
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);

	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(proactiveCmdTlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();
	
	User::WaitForRequest(refreshRequiredReqStatus);
	ASSERT_EQUALS(refreshRequiredReqStatus.Int(),KErrNone);
	//ASSERT_EQUALS(refreshRequired.PCmdNumber(),aPCmdNumber); not set in the CTSY
	ASSERT_TRUE(refreshRequired.iAid == KAid);
	ASSERT_EQUALS(refreshRequired.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshRequired.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshRequired.iFileList[0] & 0xFF));
	
	//no IPCs down to the LTSY should have occured, so we can AssertMockLtsyStatusL here.
	AssertMockLtsyStatusL();

	
	//ESatTsyServiceRequest
    TDesC8* aidPtr = const_cast<TDesC8*>(&KAid);
	TDesC8* refreshFileListPtr = &filePath;
	TUint16 cache = 0x01 | 0x02;//ECacheEFSST | ECacheEFCBMID;
	TMockLtsyData3<TDesC8*, TDesC8*, TUint16> refreshAllowedPack(refreshFileListPtr,aidPtr,cache);
	refreshAllowedPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,data,KErrNone);
	data.Close();

	RSat::TRefreshRspV1 refreshAllowedRsp;
	refreshAllowedRsp.iGeneralResult = RSat::KSuccess;
	refreshAllowedRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshAllowedRsp.iAdditionalInfo = KNullDesC;
	RSat::TRefreshRspV1Pckg refreshAllowedRspPckg(refreshAllowedRsp);
	TRequestStatus refreshAllowedReqStatus;
	iSat.RefreshAllowed(refreshAllowedReqStatus,refreshAllowedRspPckg);
	User::WaitForRequest(refreshAllowedReqStatus);
	ASSERT_EQUALS(refreshAllowedReqStatus.Int(),KErrNone);
	
	//the MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId expect should be gone
	//so assert the LTSY status
	AssertMockLtsyStatusL();
	
	
	
	//ESatTsyCompleteRefresh
	//indicator to create complete the notifier
	const TInt KResult = aRefreshAllowedCompleteResult;
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KResult);
	
	User::WaitForRequest(refreshPCmdReqStatus);
	ASSERT_EQUALS(refreshPCmdReqStatus.Int(),KResult);
	//ASSERT_EQUALS(refreshPCmd.PCmdNumber(),aPCmdNumber);  not set in the CTSY
	ASSERT_TRUE(refreshPCmd.iAid == KAid);
	ASSERT_EQUALS(refreshPCmd.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshPCmd.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshPCmd.iFileList[0] & 0xFF));
	
	User::WaitForRequest(refreshPCmdReqStatus2);
	ASSERT_EQUALS(refreshPCmdReqStatus2.Int(),KResult);
	//ASSERT_EQUALS(refreshPCmd2.PCmdNumber(),aPCmdNumber);  not set in the CTSY
	ASSERT_TRUE(refreshPCmd2.iAid == KAid);
	ASSERT_EQUALS(refreshPCmd2.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshPCmd2.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshPCmd2.iFileList[0] & 0xFF));

	CleanupStack::PopAndDestroy(&satUser);
	CleanupStack::PopAndDestroy(&data);
	}

TUint8 CCTsySatFU::TonAndNpi(RMobilePhone::TMobileTON aTon, RMobilePhone::TMobileNPI aNpi)
	{
	return (0x80) | (aTon << 4) | (aNpi);
	}

void CCTsySatFU::DoCleanup()
	{
	
	delete iCallContainer;
	iCallContainer = NULL;
	
	iSat.Close();
	CCtsyComponentTestBase::DoCleanup();
	}

CCTsySatFU::CCallTypeContainer::CCallTypeContainer() :
	iCallControlPckg(iCallControl),
	iSetUpCallPckg(iSetUpCall)
	{
	
	}



