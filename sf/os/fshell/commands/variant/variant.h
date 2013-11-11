// variant.h
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

#include <fshell/ioutils.h>

using namespace IoUtils;

class CCmdVariant : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdVariant();
private:
	CCmdVariant();
	TInt GetMachineUidL();

	// From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RArray<TUint> iMachineId;
	RPointerArray<HBufC> iVariant;
	TBool iVerbose;
	TBool iList;
	};
