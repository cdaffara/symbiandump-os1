// activeview.cpp
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
#include <fshell/qr3dll.h>

using namespace IoUtils;

class CCmdActiveView : public CCommandBase,  public MVwsSessionWrapperObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdActiveView();
private:
	CCmdActiveView();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From MVwsSessionWrapperObserver.
	virtual void HandleViewEventL(const TVwsViewEvent& aEvent);
private:
	CVwsSessionWrapper* iVws;
	TBool iVerbose;
	};


CCommandBase* CCmdActiveView::NewLC()
	{
	CCmdActiveView* self = new(ELeave) CCmdActiveView();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdActiveView::~CCmdActiveView()
	{
	delete iVws;
	}

CCmdActiveView::CCmdActiveView() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdActiveView::Name() const
	{
	_LIT(KName, "activeview");	
	return KName;
	}

void CCmdActiveView::DoRunL()
	{
	iVws = CVwsSessionWrapper::NewL(*this);
	iVws->NotifyNextActivation();
	}

void CCmdActiveView::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	}

void CCmdActiveView::HandleViewEventL(const TVwsViewEvent& aEvent)
	{
	if (aEvent.iEventType == TVwsViewEvent::EVwsActivationNotification)
		{
		if (iVerbose)
			{
			RBuf a, b, c, d;
			ExeNameForSid(aEvent.iViewTwoId.iAppUid.iUid, a);
			ExeNameForSid(aEvent.iViewTwoId.iViewUid.iUid, b);
			ExeNameForSid(aEvent.iViewOneId.iAppUid.iUid, c);
			ExeNameForSid(aEvent.iViewOneId.iViewUid.iUid, d);
			Printf(_L("View switch: 0x%08x %S 0x%08x %S -> 0x%08x %S 0x%08x %S\r\n"), aEvent.iViewTwoId.iAppUid, &a, aEvent.iViewTwoId.iViewUid, &b, aEvent.iViewOneId.iAppUid, &c, aEvent.iViewOneId.iViewUid, &d);
			a.Close();
			b.Close();
			c.Close();
			d.Close();
			}
		else
			{
			Printf(_L("View switch: 0x%08x 0x%08x -> 0x%08x 0x%08x\r\n"), aEvent.iViewTwoId.iAppUid, aEvent.iViewTwoId.iViewUid, aEvent.iViewOneId.iAppUid, aEvent.iViewOneId.iViewUid);
			}
		iVws->NotifyNextActivation();
		}
	}


EXE_BOILER_PLATE(CCmdActiveView)

