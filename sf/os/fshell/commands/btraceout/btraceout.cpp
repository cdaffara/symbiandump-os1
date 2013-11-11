// btraceout.cpp
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
#include <fshell/extrabtrace.h>

using namespace IoUtils;

class CCmdBtraceout : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdBtraceout();
private:
	CCmdBtraceout();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TUint iCategory;
	TUint iSubcategory;
	TUint iArg1;
	TUint iArg2;
	TUint iArg3;
	TBool iStdin;
	TUint iFilterUid;
	TBool iTruncate;
	};

CCommandBase* CCmdBtraceout::NewLC()
	{
	CCmdBtraceout* self = new(ELeave) CCmdBtraceout();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdBtraceout::~CCmdBtraceout()
	{
	}

CCmdBtraceout::CCmdBtraceout()
	{
	}

const TDesC& CCmdBtraceout::Name() const
	{
	_LIT(KName, "btraceout");	
	return KName;
	}

void CCmdBtraceout::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iCategory, _L("category"));
	aArguments.AppendUintL(iSubcategory, _L("subcategory"));
	aArguments.AppendUintL(iArg1, _L("arg_1"));
	aArguments.AppendUintL(iArg2, _L("arg_2"));
	aArguments.AppendUintL(iArg3, _L("arg_3"));
	}

void CCmdBtraceout::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iStdin, _L("stdin"));
	aOptions.AppendBoolL(iTruncate, _L("truncate"));
	aOptions.AppendUintL(iFilterUid, _L("filter"));
	}


EXE_BOILER_PLATE(CCmdBtraceout)

void CCmdBtraceout::DoRunL()
	{
	if (iStdin)
		{
		CTextBuffer* buffer = CTextBuffer::NewLC(512);
		Stdin().SetReadMode(RIoReadHandle::EOneOrMore);
		TBuf<256> buf;
		while (Stdin().Read(buf) == KErrNone)
			{
			buffer->AppendL(buf);
			}
		RBuf8 data;
		data.CreateL(buffer->Length());
		data.Copy(buffer->Descriptor()); // This will collapse us back to real 8-bit data

		if (iTruncate)
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				if (iArguments.IsPresent(3))
					{
					PrintWarning(_L("Ignoring 'arg_2' (%u) and all following arguments"), iArg2);
					}
				BTraceFilteredN(iCategory, iSubcategory, iFilterUid, iArg1, data.Ptr(), data.Size());
				}
			else
				{
				if (iArguments.IsPresent(3))
					{
					PrintWarning(_L("Ignoring 'arg_3' (%u)"), iArg3);
					}
				BTraceN(iCategory, iSubcategory, iArg1, iArg2, data.Ptr(), data.Size());
				}
			}
		else
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				if (iArguments.IsPresent(2))
					{
					PrintWarning(_L("Ignoring 'arg_1' (%u) and all following arguments"), iArg1);
					}
				BTraceFilteredBig(iCategory, iSubcategory, iFilterUid, data.Ptr(), data.Size());
				}
			else
				{
				if (iArguments.IsPresent(3))
					{
					PrintWarning(_L("Ignoring 'arg_2' (%u) and all following arguments"), iArg2);
					}
				BTraceBig(iCategory, iSubcategory, iArg1, data.Ptr(), data.Size());
				}
			}

		data.Close();
		CleanupStack::PopAndDestroy(buffer);
		}
	else
		{
		if (iArguments.IsPresent(4))
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				if (iArguments.IsPresent(3))
					{
					PrintWarning(_L("Ignoring 'arg_3' (%u)"), iArg3);
					}
				BTraceFiltered12(iCategory, iSubcategory, iFilterUid, iArg1, iArg2);
				}
			else
				{
				BTrace12(iCategory, iSubcategory, iArg1, iArg2, iArg3);
				}
			}
		else if (iArguments.IsPresent(3))
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				BTraceFiltered12(iCategory, iSubcategory, iFilterUid, iArg1, iArg2);
				}
			else
				{
				BTrace8(iCategory, iSubcategory, iArg1, iArg2);
				}
			}
		else if (iArguments.IsPresent(2))
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				BTraceFiltered8(iCategory, iSubcategory, iFilterUid, iArg1);
				}
			else
				{
				BTrace4(iCategory, iSubcategory, iArg1);
				}
			}
		else
			{
			if (iOptions.IsPresent(&iFilterUid))
				{
				BTraceFiltered4(iCategory, iSubcategory, iFilterUid);
				}
			else
				{
				BTrace0(iCategory, iSubcategory);
				}
			}
		}
	}
