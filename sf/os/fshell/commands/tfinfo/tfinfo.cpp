// tfinfo.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <w32std.h>
#include <gdi.h>
#include <fbs.h>

using namespace IoUtils;

class CCmdTfinfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTfinfo();
private:
	CCmdTfinfo();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RWsSession iWsSession;
	CWsScreenDevice* iScreenDevice;
	CFbsTypefaceStore* iTypefaceStore;
	TBool iVerbose;
	};


CCommandBase* CCmdTfinfo::NewLC()
	{
	CCmdTfinfo* self = new(ELeave) CCmdTfinfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTfinfo::~CCmdTfinfo()
	{
	delete iTypefaceStore;
	delete iScreenDevice;
	iWsSession.Close();
	}

CCmdTfinfo::CCmdTfinfo()
	{
	}

const TDesC& CCmdTfinfo::Name() const
	{
	_LIT(KName, "tfinfo");	
	return KName;
	}

void CCmdTfinfo::DoRunL()
	{
	LeaveIfErr(iWsSession.Connect(), _L("Couldn't connect to the Window Server"));
	iScreenDevice = new(ELeave) CWsScreenDevice(iWsSession);
	iScreenDevice->Construct();
	iTypefaceStore = CFbsTypefaceStore::NewL(iScreenDevice);

	TInt numTypeFaces = iTypefaceStore->NumTypefaces();
	if (iVerbose)
		{
		Printf(_L("%d type faces found\r\n"), numTypeFaces);
		}
	for (TInt i = 0; i < numTypeFaces; ++i)
		{
		TTypefaceSupport typefaceSupport;
		iTypefaceStore->TypefaceSupport(typefaceSupport, i);
		Printf(_L("%S\r\n"), &typefaceSupport.iTypeface.iName);
		if (iVerbose)
			{
			Printf(_L("\tProportional: %d\r\n"), typefaceSupport.iTypeface.IsProportional());
			Printf(_L("\tSerif: %d\r\n"), typefaceSupport.iTypeface.IsSerif());
			Printf(_L("\tSymbol: %d\r\n"), typefaceSupport.iTypeface.IsSymbol());
			Printf(_L("\tScalable: %d\r\n"), typefaceSupport.iIsScalable);
			if (!typefaceSupport.iIsScalable)
				{
				Printf(_L("\tNumber of heights: %d\r\n"), typefaceSupport.iNumHeights);
				Printf(_L("\tMin height in twips: %d\r\n"), typefaceSupport.iMinHeightInTwips);
				Printf(_L("\tMax height in twips: %d\r\n"), typefaceSupport.iMaxHeightInTwips);
				}
			}
		}
	}

void CCmdTfinfo::ArgumentsL(RCommandArgumentList&)
	{
	}

void CCmdTfinfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}


EXE_BOILER_PLATE(CCmdTfinfo)

