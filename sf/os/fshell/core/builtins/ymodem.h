// ymodem.h
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


#ifndef __YMODEM_H__
#define __YMODEM_H__

#include <fshell/ioutils.h>
#include "xmodem.h"

using namespace IoUtils;


class CCmdYmodem : public CCmdXmodem
	{
public:
	static CCommandBase* NewLC();
	~CCmdYmodem();
private:
	CCmdYmodem();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RArray<TFileName2> iFileNames;
	};


#endif // __YMODEM_H__
