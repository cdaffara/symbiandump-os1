// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "testpinnotifierplugin.h"
#include <ssm/ssmuiproviderdll.h>
#include <eikdialg.h>
#include <testpinnotifier.rsg>
#include <bautils.h>
#include <eikenv.h>
#include <uikon.hrh>
#include <ecom/implementationproxy.h>
#include <eikseced.h>
#include <e32property.h>

const TUint KCustomcmdServerPropertyKey = 0x0012AC;
const TUid KCustomcmdServerSID={0x2000D75B};				// tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)

_LIT(KPinNotifierResFileNameAndPath,"\\resource\\apps\\testpinnotifier.rsc");
_LIT(KPinNotifierTitle,"Security Check");


CArrayPtr<MEikSrvNotifierBase2>* NotifierArrayL()
	{
	CArrayPtrFlat<MEikSrvNotifierBase2>* subjects=new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>(10);
	CleanupStack::PushL(subjects);
	subjects->AppendL(CSecurityPinNotifier::NewLC());
	CleanupStack::Pop(2, subjects);
	return subjects;
	}

//ECOM SUPPORT
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x2000E667, NotifierArrayL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}


// Notifier methods
void CSecurityPinNotifier::Release()
	{
	delete this;
	}

CSecurityPinNotifier::TNotifierInfo CSecurityPinNotifier::RegisterL()
	{
	const TUid KSecurityPinNotifierUid = {0x2000E667};
	iInfo.iUid = KSecurityPinNotifierUid;

	const TUid KScreenOutputChannel = {0x10009D48};	
	iInfo.iChannel = KScreenOutputChannel;

	iInfo.iPriority = ENotifierPriorityVHigh;
	return iInfo;
	}

CSecurityPinNotifier::TNotifierInfo CSecurityPinNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CSecurityPinNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	RouseSleepingDialog();
	return KNullDesC8();
	}

void CSecurityPinNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	// Set this swp to indicate to stop the active schaduler
	const TInt KSwpCancelRequest = 99;
	TInt err = RProperty::Set(KCustomcmdServerSID, KCustomcmdServerPropertyKey,KSwpCancelRequest);

	iMessage = aMessage;
	iReplySlot = aReplySlot;
	SetTitleL(KPinNotifierTitle());
	StartL(aBuffer);
	}

void CSecurityPinNotifier::Cancel()
	{
	ExitSleepingDialog();
	}

TPtrC8 CSecurityPinNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CSecurityPinNotifier::UpdateL(const TDesC8& aBuffer, TInt /*aReplySlot*/, const RMessagePtr2& /*aMessage*/)
	{
	UpdateL(aBuffer);
	}

CSecurityPinNotifier* CSecurityPinNotifier::NewLC()
	{
	CSecurityPinNotifier* self=new (ELeave) CSecurityPinNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSecurityPinNotifier::CSecurityPinNotifier()
	{
	}

CSecurityPinNotifier::~CSecurityPinNotifier()
	{
	iEikonEnv->RemoveFromStack(this);
	}

void CSecurityPinNotifier::ConstructL()
	{
	 // load the resource file
	const TDriveNumber KStoreDrive = EDriveZ;
	TDriveUnit driveUnit(KStoreDrive);  
	TDriveName drive=driveUnit.Name();
	TFileName   resourceFileName;
	resourceFileName.Insert(0, drive); 
	
	resourceFileName.Append(KPinNotifierResFileNameAndPath);
	CEikonEnv& eikonEnv = *CEikonEnv::Static();
	BaflUtils::NearestLanguageFile(eikonEnv.FsSession(), resourceFileName);
	TInt resourceFileId = eikonEnv.AddResourceFileL(resourceFileName);
	
	TRAPD(err,ConstructSleepingAlertDialogL(R_SECURITY_PIN));
	iEikonEnv->DeleteResourceFile(resourceFileId);
	User::LeaveIfError(err);

	}

void CSecurityPinNotifier::PreLayoutDynInitL()
	{
	}

TBool CSecurityPinNotifier::OkToExitL(TInt /*aButtonId*/)
	{
	TPckgBuf<TInt> securityResultBuffer;
	securityResultBuffer = KErrGeneral; 
	if (iMessage != RMessagePtr2())
		{
   		securityResultBuffer = KErrNone;
		iMessage.Write(iReplySlot,securityResultBuffer);
		iMessage.Complete(KErrNone);	
		// Set this swp to indicate to stop the active schaduler
		const TInt KSwpValForOnOkStopScheduler = 88;
 		TInt err = RProperty::Set(KCustomcmdServerSID, KCustomcmdServerPropertyKey, KSwpValForOnOkStopScheduler);
 		
 		if(err)
 			{
 			return EFalse;
 			}
		}
	return ETrue;
	}
