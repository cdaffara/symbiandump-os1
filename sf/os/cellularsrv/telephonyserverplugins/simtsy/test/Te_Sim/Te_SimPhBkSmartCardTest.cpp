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

#include "Te_SimPhBkSmartCardTest.h"

CSimPhBkSmartCardTest::CSimPhBkSmartCardTest() 
	{ 
	SetTestStepName(_L("SmartCardTest"));
	}

TVerdict CSimPhBkSmartCardTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkSmartCardTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(10);

	RMobilePhone::TAID smartCardAppID;
	TUint count,smartCardIndex;
	TRequestStatus reqStatus;
	
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("SmartCard test"));
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
		
	// EnumerateSmartCardApplications
	INFO_PRINTF1(_L("EnumerateSmartCardApplications test"));
	enum {KSmartCardApps=10};
	iPhone.EnumerateSmartCardApplications(reqStatus, count);
	User::WaitForRequest(reqStatus);
	TESTL(count==KSmartCardApps);
	CHECKPOINTL(reqStatus, KErrNone, CHP_APPS_CASE("O.1"));

	// GetSmartCardApplicationInfo
	INFO_PRINTF1(_L("GetSmartCardApplicationInfo test"));
	//	Structured data to read in application info
	RMobilePhone::TSmartCardApplicationInfoV5 smartCardAppInfo;
	RMobilePhone::TSmartCardApplicationInfoV5Pckg smartCardAppInfoPckg(smartCardAppInfo);

	//	first application
	smartCardIndex=0;
	iPhone.GetSmartCardApplicationInfo(reqStatus, smartCardIndex, smartCardAppInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus, KErrNone, CHP_APPS_CASE("O.2"));
	//	Test the contents of structured data
	TESTL(smartCardAppInfo.iAID.Compare(_L8("SCApp1")) == KErrNone);
	TESTL(smartCardAppInfo.iLabel.Compare(_L8("SmartCardApp1")) == KErrNone);
	
	//	fifth application - USim Application
	smartCardIndex=4;
	iPhone.GetSmartCardApplicationInfo(reqStatus, smartCardIndex, smartCardAppInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.3"));
	TESTL(smartCardAppInfo.iAID.Compare(_L8("USimApp1")) == KErrNone);
	TESTL(smartCardAppInfo.iLabel.Compare(_L8("USimApplication1")) == KErrNone);

	//	last application in array
	smartCardIndex=9;
	iPhone.GetSmartCardApplicationInfo(reqStatus, smartCardIndex, smartCardAppInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.4"));
	TESTL(smartCardAppInfo.iAID.Compare(_L8("SCAppThatIsTooLo")) == KErrNone);
	TESTL(smartCardAppInfo.iLabel.Compare(_L8("SmartCardAppThatIsMuchMuchMuchTo")) == KErrNone);

	//	Out of range application
	smartCardIndex=10;
	iPhone.GetSmartCardApplicationInfo(reqStatus, smartCardIndex, smartCardAppInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNotFound, CHP_APPS_CASE("O.5"));

	// NotifySmartCardApplicationInfoChange
	/* Activate application in order to terminate it later */
	smartCardAppID=_L8("SCApp2");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	

	INFO_PRINTF1(_L("NotifySmartCardApplicationInfoChange test"));
	RMobilePhone::TSmartCardApplicationEvent event;
	/*	Second application is terminated with an event in config.txt
		This is time dependent */
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, smartCardAppID, event);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.6"));
	TESTL(smartCardAppID==_L8("SCApp2"));
	TESTL(event==RMobilePhone::EScApplicationTerminated);

	//	First application will be activated	
	smartCardAppID=_L8("SCApp1");
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, smartCardAppID, event);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.7"));
	TESTL(smartCardAppID==_L8("SCApp1"));
	TESTL(event==RMobilePhone::EScApplicationActivated);
	
	//	Notification for activation on already activated first application 
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, smartCardAppID, event);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.8"));
	TESTL(smartCardAppID==_L8("SCApp1"));
	TESTL(event==RMobilePhone::EScApplicationActivated);
		
	//	Terminate first application
	smartCardAppID=_L8("SCApp1");
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, smartCardAppID, event);
	User::WaitForRequest(reqStatus);		//	Wait for notification of change
	CHECKPOINTL(reqStatus, KErrNone, CHP_APPS_CASE("O.9"));
	TESTL(smartCardAppID==_L8("SCApp1"));	
	TESTL(event==RMobilePhone::EScApplicationTerminated);
	
	// GetCurrentActiveUSIMApplication
	smartCardAppID=_L8("");
	INFO_PRINTF1(_L("GetCurrentActiveUSIMApplication test"));
	//	There should be no active application at this point
	iPhone.GetCurrentActiveUSimApplication(reqStatus, smartCardAppID);
	User::WaitForRequest(reqStatus);		//	Wait for notification of change
	CHECKPOINTL(reqStatus, KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.10"));
	TESTL(smartCardAppID==_L8(""));
	
	//	Wait for event to activate second application
	smartCardAppID=_L8("USimApp2");
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, smartCardAppID, event);
	User::WaitForRequest(reqStatus);		//	Wait for notification of change
	CHECKPOINTL(reqStatus, KErrNone, CHP_APPS_CASE("O.11"));
	TESTL(smartCardAppID==_L8("USimApp2"));
	TESTL(event==RMobilePhone::EScApplicationActivated);
	
	//	There should be an active application at this point
	iPhone.GetCurrentActiveUSimApplication(reqStatus, smartCardAppID);
	User::WaitForRequest(reqStatus);		//	Wait for notification of change
	CHECKPOINTL(reqStatus, KErrNone, CHP_APPS_CASE("O.12"));
	TESTL(smartCardAppID==_L8("USimApp2"));
	
	//	At this point there should be one active application, the USIM app, USimApp2
	// 	SetSmartCardApplicationStatus
	INFO_PRINTF1(_L("SetSmartCardApplicationStatus test"));
	//	Try and activate a second USIM application
	smartCardAppID=_L8("USimApp1");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrAlreadyExists, CHP_APPS_CASE("O.13"));

	//	Set other smart card apps to active
	smartCardAppID=_L8("SCApp3");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	
	
	//	Set third smart card app to active
	smartCardAppID=_L8("SCApp4");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	

	//	Set fourth smart card app to active
	smartCardAppID=_L8("SCApp5");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	

	/*	Set fifth smart card app to active
		This should return with error code as it surpasses the max set in the config file */
	smartCardAppID=_L8("SCApp6");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScMaxApplicationsActive, CHP_APPS_CASE("O.14"));

	/* Attempt to status of a AID that does not exist */
	smartCardAppID=_L8("NotFound");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNotFound, CHP_APPS_CASE("O.15"));

	smartCardAppID=_L8("NotFound");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNotFound, CHP_APPS_CASE("O.16"));

	//	Set fourth smart card app to active when already active
	smartCardAppID=_L8("SCApp5");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.17"));

	INFO_PRINTF1(_L("GetScFileInfo test"));
	/* Attempt to get info on a file from an application that is not found */
	RMobilePhone::TScFilePath filePath;
	RMobilePhone::TScFileInfoV5 scFileInfo;
	RMobilePhone::TScFileInfoV5Pckg scFileInfoPckg(scFileInfo);
	
	filePath.iAID = _L8("App2");
	filePath.iPath = _L8("7FFF6F3C");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNotFound, CHP_APPS_CASE("O.18"));

	/* Attempt to get info on a file from an application that has not started */	
	filePath.iAID = _L8("SCApp1");
	filePath.iPath = _L8("7FFF6F3C");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.19"));

	/* Attempt to get info on a file from an application that has been terminated */
	smartCardAppID=_L8("SCApp3");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	

	filePath.iAID = _L8("SCApp3");
	filePath.iPath = _L8("7FFF6F3C");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.20"));
	
	smartCardAppID=_L8("SCApp4");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);	
	
	smartCardAppID=_L8("SCApp5");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	/* Attempt to set SCApp5 to inactive when it already is inactive */		
	smartCardAppID=_L8("SCApp5");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.21"));

	smartCardAppID=_L8("USimApp2");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);
	
	filePath.iAID = _L8("SCApp1");
	filePath.iPath = _L8("7FFF6F3C");
	smartCardAppID = _L8("SCApp1");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	/* Attempt to get info on a pre-configured Linear Fixed Record based file */	
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.22"));
	TESTL(scFileInfo.iFileID == (0x6F3C));
	TESTL(scFileInfo.iNumberRecords == 2);
	TESTL(scFileInfo.iRecordLength == 5);
	TESTL(scFileInfo.iType == RMobilePhone::ELinearFixed);
	TESTL(scFileInfo.iTotalLength == 10);
	
	/* Attempt to get info on a pre-configured Cyclical Record based file, testing same filename
	   in different application */
	filePath.iAID = _L8("SCApp2");
	smartCardAppID = _L8("SCApp2");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	filePath.iAID = _L8("SCApp2");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.23"));
	TESTL(scFileInfo.iFileID == (0x6F3C));
	TESTL(scFileInfo.iNumberRecords == 1);
	TESTL(scFileInfo.iRecordLength == 5);
	TESTL(scFileInfo.iType == RMobilePhone::ECyclic);
	TESTL(scFileInfo.iTotalLength == 5);

	/* Attempt to get info on a pre-configured Transparent Record based file, testing same filename
	   in different application */
	smartCardAppID=_L8("SCApp4");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	filePath.iAID = _L8("SCApp4");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.24"));
	TESTL(scFileInfo.iFileID == (0x6F3C));
	TESTL(scFileInfo.iNumberRecords == 0);
	TESTL(scFileInfo.iRecordLength == 0);
	TESTL(scFileInfo.iType == RMobilePhone::ETransparent);
	TESTL(scFileInfo.iTotalLength == 10);

	/* Simple interaction test with smart card not owned by application */
	filePath.iAID = _L8("");
	filePath.iPath = _L8("3FFF6F3C");
	iPhone.GetScFileInfo(reqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.25"));
	TESTL(scFileInfo.iFileID == (0x6F3C));
	TESTL(scFileInfo.iNumberRecords == 1);
	TESTL(scFileInfo.iRecordLength == 5);
	TESTL(scFileInfo.iType == RMobilePhone::ELinearFixed);
	TESTL(scFileInfo.iTotalLength == 5);
	
	INFO_PRINTF1(_L("ReadSCFile test"));
	/* Test ReadSCFile on Linear Fixed file with specified iAID, iPath first record */
	RMobilePhone::TScFilePathWithAccessOffsets offsets;
	TBuf8<25> readBuffer;
	TBuf8<4> testBuffer; // buffer too small for a record
	
	offsets.iAID = _L8("SCApp1");
	offsets.iPath = _L8("7FFF6F3C");
	offsets.iType = RMobilePhone::ELinearFixed;
	offsets.iCount = 1; // number of records to read
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.26"));
	TESTL(readBuffer == _L8("HELLO"));
	
	/* Test ReadSCFile, second record */
	offsets.iStartOffset = 2;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.27"));
	TESTL(readBuffer == _L8("WORLD"));

	/* Test ReadSCFile, out of range record */
	offsets.iStartOffset = 0;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrMMEtelScRecordNotFound);
	
	/* Test ReadSCFile, out of range record */
	offsets.iStartOffset = 10;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScRecordNotFound, CHP_APPS_CASE("O.28"));

	/* Test ReadSCFile, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrOverflow, CHP_APPS_CASE("O.29"));

	/* Simple ReadScFile test with smart card not owned by application */
	offsets.iAID = _L8("");
	offsets.iPath = _L8("3FFF6F3C");
	offsets.iCount = 1; // number of records to read
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.30"));
	TESTL(readBuffer == _L8("HELLO"));
			
	/* Test ReadSCFile on Cyclical file with specified iAID but without application active */	
	offsets.iAID = _L8("SCApp3");
	offsets.iType = RMobilePhone::ECyclic;
	offsets.iCount = 1; // number of records to read
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.31"));
	
	smartCardAppID=_L8("SCApp3");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	/* Test ReadSCFile, with incorrect path */
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScFileNotFound, CHP_APPS_CASE("O.32"));

	/* Test ReadSCFile on Cyclical file with specified iAID, iPath last record written */
	offsets.iPath = _L8("7FFF6F3C"); // set correct path

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.33"));
	TESTL(readBuffer == _L8("WORLD"));

	/* Test ReadSCFile, second record, actually oldest record */
	offsets.iStartOffset = 2;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.34"));
	TESTL(readBuffer == _L8("HELLO"));

	/* Test ReadSCFile, out of range record */
	offsets.iStartOffset = 0;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrMMEtelScRecordNotFound);
	
	/* Test ReadSCFile, out of range record */
	offsets.iStartOffset = 10;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScRecordNotFound, CHP_APPS_CASE("O.35"));
	
	/* Test ReadSCFile, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrOverflow, CHP_APPS_CASE("O.36"));

	/* Test ReadSCFile on Transparent file with specified iAID, iPath first 4 bytes */
	offsets.iAID = _L8("SCApp4");
	offsets.iType = RMobilePhone::ETransparent;
	offsets.iCount = 4; // number of bytes to read
	offsets.iStartOffset = 0;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.37"));
	TESTL(readBuffer == _L8("HELL"));

	/* Test ReadSCFile, 5 bytes in for 5 bytes */
	offsets.iCount = 5; // number of bytes to read
	offsets.iStartOffset = 5;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.38"));
	TESTL(readBuffer == _L8("WORLD"));

	/* Test ReadSCFile, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.ReadScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrOverflow, CHP_APPS_CASE("O.39"));
	
	/* Test ReadSCFile, out of range record */
	offsets.iStartOffset = 11;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScReferencedDataNotFound, CHP_APPS_CASE("O.40"));

	/* Test ReadSCFile, 5 bytes in for 10 bytes, beyond end of file */
	offsets.iCount = 10; // number of bytes to read
	offsets.iStartOffset = 5;
	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScEofReached, CHP_APPS_CASE("O.41"));

	INFO_PRINTF1(_L("UpdateSCFile test"));
	/* Test UpdateSCFile on Linear Fixed file with specified iAID, write first record */
	TBuf8<25> writeBuffer;

	/* Simple UpdateSCFile test with smart card not owned by application */
	writeBuffer = _L8("EARTH");
	offsets.iAID = _L8("");
	offsets.iPath = _L8("3FFF6F3C");
	offsets.iType = RMobilePhone::ELinearFixed;
	offsets.iCount = 1; // number of records to read
	offsets.iStartOffset = 1;
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.42"));
	TESTL(readBuffer == _L8("EARTH"));

	offsets.iAID = _L8("SCApp1");
	offsets.iPath = _L8("7FFF6F3C");
	offsets.iCount = 1; // number of records to write
	offsets.iStartOffset = 1; // record number to write
	writeBuffer = _L8("GOODB");
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.43"));
	TESTL(readBuffer == _L8("GOODB"));

	/* Test UpdateSCFile on Linear Fixed file with specified iAID, write 2nd record */
	offsets.iStartOffset = 2; // record number to write
	writeBuffer = _L8("EARTH");
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.44"));
	TESTL(readBuffer == _L8("EARTH"));

	/* Test UpdateSCFile, out of range record */
	offsets.iStartOffset = 0;
	iPhone.UpdateScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrMMEtelScRecordNotFound);
	
	/* Test UpdateSCFile, out of range record */
	offsets.iStartOffset = 10;
	iPhone.UpdateScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScRecordNotFound, CHP_APPS_CASE("O.45"));

	/* Test UpdateSCFile on Linear Fixed file, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.UpdateScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScWrongLength, CHP_APPS_CASE("O.46"));

	/* Test UpdateSCFile on Cyclic file with specified iAID, write oldest record at 1st position */
	offsets.iAID = _L8("SCApp3");
	offsets.iType = RMobilePhone::ECyclic;
	writeBuffer = _L8("GOODB");
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	offsets.iStartOffset = 1; // record number to read (lastest written record)

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.47"));
	TESTL(readBuffer == _L8("GOODB"));

	/* Test UpdateSCFile on Cyclic file with specified iAID, write oldest record, now at 2nd position */
	writeBuffer = _L8("EARTH");
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.48"));
	TESTL(readBuffer == _L8("EARTH"));

	/* Test UpdateSCFile on Cyclic file with specified iAID, write oldest record, now at back at 1st position */
	writeBuffer = _L8("HELLO");
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	offsets.iStartOffset = 2; // record number to read (oldest record)

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.49"));
	TESTL(readBuffer == _L8("EARTH"));
	
	/* Test UpdateSCFile on Cyclic file, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.UpdateScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScWrongLength, CHP_APPS_CASE("O.50"));

	/* Test UpdateSCFile on Transparent file with specified iAID, write from beginning */
	offsets.iAID = _L8("SCApp4");
	offsets.iType = RMobilePhone::ETransparent;
	writeBuffer = _L8("GOODB");
	offsets.iCount = 5; // number of bytes to write
	offsets.iStartOffset = 0; // From position 0
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.51"));
	TESTL(readBuffer == _L8("GOODB"));

	/* Test UpdateSCFile on Transparent file with specified iAID, write from half way through */
	writeBuffer = _L8("EARTH");
	offsets.iCount = 5; // number of bytes to write
	offsets.iStartOffset = 5; // From position 0
	
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	offsets.iCount = 10; // number of bytes to write
	offsets.iStartOffset = 0; // From position 0

	iPhone.ReadScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrNone, CHP_APPS_CASE("O.52"));	
	TESTL(readBuffer == _L8("GOODBEARTH"));

	/* Test UpdateSCFile, buffer too small */
	offsets.iStartOffset = 1;
	iPhone.UpdateScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrArgument, CHP_APPS_CASE("O.53"));	
	
	/* Test UpdateSCFile, out of range record */
	offsets.iStartOffset = 11;
	iPhone.UpdateScFile(reqStatus, offsets, readBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScReferencedDataNotFound, CHP_APPS_CASE("O.54"));	

	/* Test UpdateSCFile, writing beyond range of file */
	offsets.iStartOffset = 1;
	offsets.iCount = 11; // number of bytes to write
	iPhone.UpdateScFile(reqStatus, offsets, testBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrArgument, CHP_APPS_CASE("O.55"));	

	/* Test UpdateSCFile on Linear file with specified iAID but without application active */	
	smartCardAppID=_L8("SCApp1");
	offsets.iAID = _L8("SCApp1");
	offsets.iType = RMobilePhone::ELinearFixed;
	offsets.iCount = 1; // number of records to write
	offsets.iStartOffset = 1; // record number to write
	writeBuffer = _L8("GOODB");
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScTerminate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScApplicationNotActive, CHP_APPS_CASE("O.56"));	
	
	iPhone.SetSmartCardApplicationStatus(reqStatus,smartCardAppID,RMobilePhone::EScApplicationActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	/* Test UpdateSCFile, with incorrect path */
	offsets.iPath = _L8("3FFF6F3C");
	iPhone.UpdateScFile(reqStatus, offsets, writeBuffer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_EXL(reqStatus==KErrMMEtelScFileNotFound, CHP_APPS_CASE("O.57"));	

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

