// gobble.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>

using namespace IoUtils;


class CCmdGobble : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdGobble();
private:
	CCmdGobble();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFileName;
	TUint iAmount;
	TUint iBlockSize;
	TBool iVerbose;
	TBool iNoWrite;
	};
