// switchview.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#undef SYMBIAN_ENABLE_SPLIT_HEADERS // Stopgap to handle CVwsSessionWrapper being compiled out of viewcli.h in latest TB92
#include <fshell/ioutils.h>
#include <viewcli.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdSwitchview : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSwitchview();
private:
	CCmdSwitchview();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TUid iAppId;
	TUid iViewId;
	TUid iCustomMessageId;
	HBufC* iCustomMessage;
	};


CCommandBase* CCmdSwitchview::NewLC()
	{
	CCmdSwitchview* self = new(ELeave) CCmdSwitchview();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSwitchview::~CCmdSwitchview()
	{
	delete iCustomMessage;
	}

CCmdSwitchview::CCmdSwitchview()
	{
	}

const TDesC& CCmdSwitchview::Name() const
	{
	_LIT(KName, "switchview");	
	return KName;
	}

void CCmdSwitchview::DoRunL()
	{
	CVwsSessionWrapper* vws = CVwsSessionWrapper::NewLC();
	TVwsViewId viewId(iAppId, iViewId);

	TPtrC8 customMessagePtr(KNullDesC8);
	if (iCustomMessage)
		{
		customMessagePtr.Set((TUint8*)iCustomMessage->Ptr(), iCustomMessage->Size());
		}

#ifdef FSHELL_9_1_SUPPORT
	User::LeaveIfError(vws->ActivateView(viewId, iCustomMessageId, customMessagePtr));
#else
	TRequestStatus status;
	vws->ActivateView(viewId, iCustomMessageId, customMessagePtr, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
#endif

	CleanupStack::PopAndDestroy(vws);
	}

void CCmdSwitchview::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgAppId, "app_id");
	aArguments.AppendUintL((TUint&)iAppId.iUid, KArgAppId);
	_LIT(KArgViewId, "view_id");
	aArguments.AppendUintL((TUint&)iViewId.iUid, KArgViewId);
	_LIT(KArgCustomMessageId, "custom_message_id");
	aArguments.AppendUintL((TUint&)iCustomMessageId.iUid, KArgCustomMessageId);
	_LIT(KArgCustomMessage, "custom_message");
	aArguments.AppendStringL(iCustomMessage, KArgCustomMessage);
	}

void CCmdSwitchview::OptionsL(RCommandOptionList&)
	{
	}


EXE_BOILER_PLATE(CCmdSwitchview)

