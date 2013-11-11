// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalAll 
*/

#ifndef __PHONE_H__
#define __PHONE_H__

#include <et_phone.h>
#include "TSYCONFG.H"
#include "ATBASE.H"

class CATIO;
class CATBase;
class CLineHayes;
class CLineMobileFax;
class CLineMobileData;
class CLineMobileVoice;
class CATInit;
class CLineHayes;
class CATQuickInit;
class CATErrorHandler;
class CATWaitForCall;
class CATCallConnectCommands;

class CCallHayes;
class CPhoneHayes : public CPhoneBase
/**
@internalComponent
*/
	{
public:
	static CPhoneHayes* NewL();
	~CPhoneHayes();
//
	static void ClosePhone(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CheckAndSetRegistrationParams(const TInt aIpc,const TDes8* aDes1,const TDes8* aDes2);
	virtual const CArrayFixFlat<TInt>* ArrayOfMemberDataSizes(const TInt aIpc) const;

// MPhoneBaseTSY pure virtuals
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneNotifyInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisation(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle);
    virtual TInt NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* aDetection);
    virtual TInt NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle);
    virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps* aCaps);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aStatus);
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);

// other functions	
	void RemoveLine(CLineHayes* aLineHayes);
	void StartWaitForRing();
	void SetCallRinging(TInt aLineIndex);
	void SetAmbiguousDataFaxCallRinging();
	void StopRinging();
	void StopRingCounter() const;
	void SetHookStatus(RCall::THookStatus aHookStatus);
	TBool CheckForOutstandingAnswer() const;
	void CancelOtherRingingCall(CLineHayes* aLine) const;
	void FreePreAllocCallIfNecessary(CLineHayes* aLine, CCallHayes* aCall);
	TInt MultimodeInitL(TBool aExplicit);
	
protected:
	void ConstructL();		// protected so that derived classes can call this

private:
	void Dummy();

protected:
	CATIO* iIo;
	CATInit* iInit;
	CPhoneGlobals* iPhoneGlobals;
private:
	CLineMobileData* iDataLine;
	CLineMobileFax* iFaxLine;
	CLineMobileVoice* iVoiceLine;
	CATWaitForCall* iWaitForCall;
	CATErrorHandler* iErrorHandler;
	RPhone::TLineInfo iDefaultVoiceLineInfo; 
	RPhone::TLineInfo iDefaultDataLineInfo;
	RPhone::TLineInfo iDefaultFaxLineInfo;
	CArrayFixFlat<TInt>* iSizeOfMemberData;
	};

#endif
