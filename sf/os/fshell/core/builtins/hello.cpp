// hello.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "hello.h"


CCommandBase* CCmdHello::NewLC()
	{
	CCmdHello* self = new(ELeave) CCmdHello();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdHello::~CCmdHello()
	{
	}

CCmdHello::CCmdHello()
	{
	}

const TDesC& CCmdHello::Name() const
	{
	_LIT(KName, "hello");
	return KName;
	}

void CCmdHello::DoRunL()
	{
	_LIT(KHelloWorld, "Hello World!");

	if (iColor)
		{
		CTextBuffer* buf = CTextBuffer::NewLC(16);
		TInt color = ConsoleAttributes::ERed;
		for (TInt i = 0; i < KHelloWorld().Length(); ++i)
			{
			buf->SetAttributesL(ConsoleAttributes::ENone, (ConsoleAttributes::TColor)color);
			buf->AppendL(KHelloWorld()[i]);
			++color;
			if (color == ConsoleAttributes::EWhite)
				{
				color = ConsoleAttributes::ERed;
				}
			}
		buf->SetAttributesL(ConsoleAttributes::ENone); // Restore the colours to normal
		buf->Write(Stdout());
		CleanupStack::PopAndDestroy(buf);
		}
	else
		{
		Write(KHelloWorld);
		}
	}

void CCmdHello::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iColor, _L("color"));
	}

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdHello)
#endif

