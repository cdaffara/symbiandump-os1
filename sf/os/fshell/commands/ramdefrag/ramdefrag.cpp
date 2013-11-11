// ramdefrag.cpp
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
#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

class CCmdRamDefrag : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRamDefrag();
private:
	CCmdRamDefrag();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCmd
		{
		EDefrag,
		EEmptyZone,
		EZoneInfo,
		};
	TCmd iCmd;
	RArray<TUint> iZones;
	TInt iPriority;
	TBool iCsv;
	};

EXE_BOILER_PLATE(CCmdRamDefrag)

CCommandBase* CCmdRamDefrag::NewLC()
	{
	CCmdRamDefrag* self = new(ELeave) CCmdRamDefrag();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRamDefrag::~CCmdRamDefrag()
	{
	iZones.Close();
	}

CCmdRamDefrag::CCmdRamDefrag()
	: iPriority(-1)
	{
	}

const TDesC& CCmdRamDefrag::Name() const
	{
	_LIT(KName, "ramdefrag");
	return KName;
	}

void CCmdRamDefrag::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCmd, _L("command"));
	aArguments.AppendUintL(iZones, _L("zone"));
	}

void CCmdRamDefrag::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iPriority, _L("priority"));
	aOptions.AppendBoolL(iCsv, _L("csv"));
	}

void CCmdRamDefrag::DoRunL()
	{
	LoadMemoryAccessL();

	switch (iCmd)
		{
		case EDefrag:
			LeaveIfErr(iMemAccess.DefragRam(iPriority), _L("Couldn't defrag RAM."));
			break;
		case EEmptyZone:
			{
			if (iZones.Count() == 0) LeaveIfErr(KErrArgument, _L("Must specify at least one zone to empty."));
			for (TInt i = 0; i < iZones.Count(); i++)
				{
				TUint zone = iZones[i];
				TInt err = iMemAccess.EmptyRamZone(zone, iPriority);
				if (err) PrintError(err, _L("Failed to empty RAM zone %u"), zone);
				}
			break;
			}
		case EZoneInfo:
			if (iZones.Count() == 0) LeaveIfErr(KErrArgument, _L("Must specify at least one zone."));
			if (iCsv) Printf(_L("Zone,Discardable pages,Fixed pages,Free pages,Movable pages,Unknown pages\r\n"));

			for (TInt i = 0; i < iZones.Count(); i++)
				{
				TUint zone = iZones[i];
				TRamZoneInfo info;
				TPckg<TRamZoneInfo> infoPkg(info);
				TInt err = iMemAccess.GetRamZoneInfo(zone, infoPkg);
				if (err)
					{
					PrintError(err, _L("Failed to get info for RAM zone %u"), zone);
					}
				else
					{
					if (iCsv)
						{
						Printf(_L("%u,%d,%d,%d,%d,%d\r\n"), zone, info.iDiscardablePages, info.iFixedPages, info.iFreePages, info.iMovablePages, info.iUnknownPages);
						}
					else
						{
						Printf(_L("Zone %u:\r\nDiscardable pages: %d\r\nFixed pages: %d\r\nFree pages: %d\r\nMovable pages: %d\r\nUnknown pages: %d\r\n\r\n"), zone, info.iDiscardablePages, info.iFixedPages, info.iFreePages, info.iMovablePages, info.iUnknownPages);
						}
					}
				}
			break;
		}
	}
