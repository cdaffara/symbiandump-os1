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
// Part of the MVS Application for TechView
//

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <e32cons.h>
#include <f32file.h>
#include <s32file.h>

class CMVSAppUi;
class CMVSConfigAudioFormatDialog; 
class CMVSConfigVideoFormatDialog;

class CSettingsManager :public CBase
	{
public:
	CSettingsManager();
	~CSettingsManager();
	static CSettingsManager* NewL();
	TBool FileExists();
	void MakeSeedIndexL();
	//gets general settings from the file store
	void ReadGeneralSettingsL(CMVSAppUi* aAppUI);
	void WriteGeneralSettingsL(CMVSAppUi* aAppUI);
	void WriteAudioDataL(CMVSConfigAudioFormatDialog* apAudioFormat,
					  const TUid& aUid);
	void  WriteVideoDataL(CMVSConfigVideoFormatDialog* aVideoFormatDlg,
						 const TUid& aUid);
	TInt ReadAudioDataL(CMVSConfigAudioFormatDialog* apAudioFormat,
							   const TUid& aUid);
	TBool ReadVideoDataL(CMVSConfigVideoFormatDialog* aVideoFormatDlg,
						const TUid& aUid);

private:
	TBool OpenStore2ReadLC();
	void OpenStore2WriteLC();
	TBool HasSettings();
	void ReadControllerTableL(RReadStream& aStream);
	TBool IsControllerAvailableL(const TUid& aUid,
										TStreamId&  aSteamId);										
	void WriteIndexL(RWriteStream& aStream,
					const TUid& aUid,
					TStreamId& aStreamId);
	void ConstructL();

private:
	RFs			iFsSession;
	CFileStore*	iStore;
	TBool iHasSettings;
	TStreamId iGenSettingsId;
	TStreamId iRootId; //for index writing
	//no. of controllers currently available in storage		 
	TInt8 iControllerCnt; 
	RArray<TUid> iArrUid;
	RArray<TStreamId> iArrStreamId;
	}; 

#endif  SETTINGS_MANAGER_H
