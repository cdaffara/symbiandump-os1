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

#ifndef __SIMPHONESMARTCARDAPP_H_
#define __SIMPHONESMARTCARDAPP_H_

/**
 * @file
 *
 * Contains the class definitions required for the Phone Smart Card applications.
 * @internalAll
 */

#include <etelmm.h>
#include <et_phone.h>
#include <mpbutil.h>
#include "csimtimer.h"
#include "CSimPhone.h"

const TInt KDefaultMaxActiveSmartCardApps = 4;
const TInt KMaxFileSize = 2047; //Max size of an EF file
const TInt KMaxHexFIDLen = 4;
const TInt KMaxBinFIDLen = 2;
const TInt KNoAID = -1;

/**
 * The main Phone Smart Card applications class which simulates Smart Card application/File features.
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
class CSimPhoneSmartCardApp : public CSubSessionExtBase, public MTimerCallBack
	{
public:
	static CSimPhoneSmartCardApp* NewL(CSimPhone* aPhone);
	CSimPhoneSmartCardApp(CSimPhone* aPhone);
	~CSimPhoneSmartCardApp();

	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual void TimerCallBack(TInt aId);
	const CTestConfigSection* CfgFile();
	RMobilePhone::TAID GetActiveUSim();
	
	TBool FoundScAppTags();

protected:
	void ConstructL();
	
private:
	struct TSmartCardAppEvent 
		{
		TUint32 iDuration;	//The amount of time after a notify request, that the event shoudl occur
		RMobilePhone::TAID iAID;	//The AID that should be affected by the event
		RMobilePhone::TSmartCardApplicationAction iAction;	//The event action
		};
	
	struct TSmartCardNotifyData
		{	
		TBool iNotifyPending;  //indicates if a notification is pending
		TTsyReqHandle iNotifyHandle; //the TSY notification handle
		TAny* iNotifyData1;  //Holds the data for the notification pending
		TAny* iNotifyData2;  //Holds the data for the notification pending
		};

	enum TSmartCardAppType
		{
		EUSimApp,		//Indicates that the Smart card App is a USim App
		ESmartCardApp	//Indicates a standard Smart Card App
		};

	enum TSmartCardAppStatus
		{
		EInactive,		//Indicates that the Smart Card App is installed and inactive
		EActive			//Indicates that the Smart Card App is installed and active
		};

	struct TSmartCardApplication
		{
		RMobilePhone::TAID iAID;	//SmartCard Application's AID
		TBuf8<RMobilePhone::KSmartCardApplicationLabelMaxSize> iLabel;	//SmartCard Application's Label
		TSmartCardAppType iAppType;		//The type of smart card application
		TSmartCardAppStatus iAppStatus;	//The status of the smart card application
		TBuf8<RMobilePhone::KEapSupportedTypeListSize> iEapList; //EAP support list
		};
		
	class TSmartCardFile
		{
	public:
		void GetFileInfo(RMobilePhone::TScFileInfoV5& aScFileInfo);
		virtual TInt ReadFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer) =0;
		virtual TInt UpdateFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer) =0;

	public:
		RMobilePhone::TScFileID iFileID;			//Holds the FID ofthe file
		RMobilePhone::TScFileType iType;			//Holds the 'Type' of the file
		TInt iTotalLength;							//Holds the total length (in Bytes) of the file
		TInt iRecordLength;							//Holds the length of each record contained in the file
		TUint iNumberRecords;						//Holds the number of records held in the file
		TInt iAIDIndex;								//Holds an index to the ScApplication the file is under
		RMobilePhone::TScFilePathReference iPath;	//Holds the full path reference of the file
		TBuf8<KMaxFileSize> iFileData;				//Holds the file's data
		};
		
	class TLinearSmartCardFile : public TSmartCardFile
		{
	public:
		virtual TInt ReadFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer);
		virtual TInt UpdateFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer);
		};
		
	class TCyclicSmartCardFile : public TSmartCardFile
		{
	public:
		TCyclicSmartCardFile();
		virtual TInt ReadFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer);
		virtual TInt UpdateFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer);
	private:
		void UpdateOldestRecordPointer();
	private:
		TInt iOldestRecord;		//Points to the beginning of the oldest record in the buffer
		};
	
	class TTransparentSmartCardFile : public TSmartCardFile
		{
	public:
		virtual TInt ReadFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer);
		virtual TInt UpdateFile(const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer);
		};

	TInt GetCurrentActiveUSimApplication(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID);
	TInt GetCurrentActiveUSimApplicationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EnumerateUSimApplications(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt EnumerateUSimApplicationsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetUSimApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aUSimAppIndex, TDes8* aInfo);
	TInt GetUSimApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyUSimApplicationsInfoChange(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt NotifyUSimApplicationsInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetUSimApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aUsimAppAction);
	TInt SetUSimApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle);	
	TInt EnumerateScApplications(const TTsyReqHandle aTsyReqHandle, TUint* aCount);
	TInt EnumerateScApplicationsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetScApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aAppIndex, TDes8* aInfo);
	TInt GetScApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyScApplicationInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationEvent* aEvent);
	TInt NotifyScApplicationInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetScApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationAction* aAction);
	TInt SetScApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt SetUSimAppsSelectionModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt GetUSimAppsSelectionModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyUSIMAppsSelectionModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt NotifyUSIMAppsSelectionModeChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetScFileInfo(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePath& aFilePathId, TDes8* aInfo);
	TInt GetScFileInfoCancel(const TTsyReqHandle aTsyReqHandle);	
	TInt ReadScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer);
	TInt ReadScFileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt UpdateScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer);
	TInt UpdateScFileCancel(const TTsyReqHandle aTsyReqHandle);

	//Helper functions
	void ParseSmartCardApplicationInfoL();
	void ParseSmartCardFileInfoL();
	void MapUSimAppsL();
	TInt FindScFile(const RMobilePhone::TScFilePath& aFilePathInfo, TSmartCardFile*& aSmartCardFile);
	TInt ActivateSmartCardApp(const RMobilePhone::TAID aAID, TBool& aActivatedUSimApp);
	TInt TerminateSmartCardApp(const RMobilePhone::TAID aAID, TBool& aTerminatedUSimApp);
	void TextToBin(const TDesC8& aSrc, TDes8& aDst);
	
private:
	CSimPhone* iPhone;
	CSimTimer* iTimer;
	TSmartCardNotifyData iNotifyUSimInfo;
	TSmartCardNotifyData iNotifyScInfo;
	TNotifyData iNotifyMode;
	TBool iTimerStarted;
	TInt iMaxActiveSmartCardApps;

	CArrayFixFlat<TSmartCardAppEvent>* iSmartCardAppEventList;
	TInt iEventPointer;
	CArrayFixFlat<TSmartCardApplication>* iSmartCardAppList;
	TInt iNumActiveSmartCardApps;
	RMobilePhone::TAID iActiveUSimApp;
	RPointerArray<TSmartCardApplication>* iUSimAppList;
	CArrayPtrFlat<TSmartCardFile>* iSmartCardFileList;
	RMobilePhone::TUSimSelectionMode iCurrentSelection;
	TBool iFoundScAppTags;
	};

#endif
