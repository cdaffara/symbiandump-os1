// btrace_textonscreen.cpp
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

#include <fshell/ltkutils.h>
#include "btrace_parser.h"


EXPORT_C CBtraceTextOnScreen* CBtraceTextOnScreen::NewL(CBtraceReader& aReader)
	{
	CBtraceTextOnScreen* self = new (ELeave) CBtraceTextOnScreen(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtraceTextOnScreen::CBtraceTextOnScreen(CBtraceReader& aReader)
	: iReader(aReader)
	{
	}

EXPORT_C CBtraceTextOnScreen::~CBtraceTextOnScreen()
	{
	iReader.RemoveObserver(KAmTraceCategory, *this);
	iScreenNotifs.Close();
	}

void CBtraceTextOnScreen::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	
	LtkUtils::W32CrackL(); // In case no-one's called it yet
	}

//
// CBtraceTextOnScreen::HandleBtraceFrameL
// process the btrace frame, stripping out the relevant text
//
void CBtraceTextOnScreen::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == KAmTraceCategory && aFrame.iSubCategory == EAmTraceSubCategoryScreenText)
		{
		// First 2 words have packed windowgroup id and coordinates of text - TODO
		TUint16 windowGroupId = *((const TUint16*)aFrame.iData.Mid(2).Ptr());
		TPtrC8 data = aFrame.iData.Mid(8);
		TPtrC text((const TUint16*)data.Ptr(), data.Length() / 2);
		SeenL(aFrame.iTickCount, text, windowGroupId);
		}
	}

//
// CBtraceTextOnScreen::Seen
// inform any observers that text has arrived on-screen, passing a reference to the text
// note the observer may ask for any or a specific pattern of text to be notified on
//
void CBtraceTextOnScreen::SeenL(const TBtraceTickCount& aTickCount, const TDesC& aData, TInt aWindowGroupId)
	{
	iReader.Log(_L("Screen text: '%S' (%d)\r\n"), &aData, aWindowGroupId);
	TInt ii = iScreenNotifs.Count();
	while (--ii >= 0)
		{
		TTextOnScreenNotif& nt = iScreenNotifs[ii];

		if (nt.iWindowGroupId && (nt.iWindowGroupId != aWindowGroupId))
			{
			continue;
			}


		switch (nt.iMatchMode)
			{
			case EWildNonSpanning:
				{
				if (aData.MatchF(nt.iTxtPtr) != KErrNotFound)
					{
					MBtraceTextOnScreenObserver* observer = nt.iObserver;
					if (nt.iPersistence == ENotificationOneShot)
						{
						iScreenNotifs.Remove(ii);
						}
					observer->HandleTextSeenL(aTickCount, aData, aWindowGroupId);
					}
				break;
				}
			case ESpanningNonWild:
				{
				const TInt length = aData.Length();
				for (TInt i = 0; i < length; ++i)
					{
					if (aData[i] == nt.iTxtPtr[nt.iMatchedChars])
						{
						++nt.iMatchedChars;
						}
					else
						{
						nt.iMatchedChars = 0;
						}

					if (nt.iMatchedChars == nt.iTxtPtr.Length())
						{
						MBtraceTextOnScreenObserver* observer = nt.iObserver;
						if (nt.iPersistence == ENotificationOneShot)
							{
							iScreenNotifs.Remove(ii);
							}
						observer->HandleTextSeenL(aTickCount, aData, aWindowGroupId);
						break;
						}
					}
				break;
				}
			default:
				ASSERT(EFalse);
			}
		}	
	}

//
// CBtraceTextOnScreen::NotifyTextOnScreenL
// called by the client to register an interest in any text drawn to screen (via wserv only)
//
EXPORT_C void CBtraceTextOnScreen::NotifyTextOnScreenL(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver)
	{
	NotifyTextOnScreenL(aText, aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceTextOnScreen::NotifyTextOnScreenL(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TTextOnScreenNotif notify(aText, aObserver, aPersistence);
	iScreenNotifs.AppendL(notify);
	}

EXPORT_C void CBtraceTextOnScreen::NotifyTextOnScreenL(const TDesC& aText, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TTextOnScreenNotif notify(aText, aMatchMode, aObserver, aPersistence);
	iScreenNotifs.AppendL(notify);
	}

EXPORT_C void CBtraceTextOnScreen::NotifyTextOnScreenL(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver)
	{
	NotifyTextOnScreenL(aText, aWindowGroupId, aMatchMode, aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceTextOnScreen::NotifyTextOnScreenL(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TTextOnScreenNotif notify(aText, aWindowGroupId, aMatchMode, aObserver, aPersistence);
	iScreenNotifs.AppendL(notify);
	}

EXPORT_C void CBtraceTextOnScreen::CancelNotifyTextOnScreen(MBtraceTextOnScreenObserver& aObserver)
	{
	for (TInt i = iScreenNotifs.Count()-1; i>=0; i--)
		{
		if (iScreenNotifs[i].iObserver == &aObserver)
			{
			iScreenNotifs.Remove(i);
			}
		}
	}

//
// TTextOnScreenNotif
//
CBtraceTextOnScreen::TTextOnScreenNotif::TTextOnScreenNotif(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iTxtPtr(aText), iWindowGroupId(0), iMatchedChars(0), iMatchMode(EWildNonSpanning), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}

CBtraceTextOnScreen::TTextOnScreenNotif::TTextOnScreenNotif(const TDesC& aText, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iTxtPtr(aText), iWindowGroupId(0), iMatchedChars(0), iMatchMode(aMatchMode), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}

CBtraceTextOnScreen::TTextOnScreenNotif::TTextOnScreenNotif(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iTxtPtr(aText), iWindowGroupId(aWindowGroupId), iMatchedChars(0), iMatchMode(aMatchMode), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}
