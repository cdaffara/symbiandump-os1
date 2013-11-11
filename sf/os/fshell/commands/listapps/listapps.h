// listapps.h
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

class TApaTask;
class RWsSession;

class CCmdListApps : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdListApps();
private:
	CCmdListApps();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	void PrintDetailsL(RWsSession& aWsSession, TInt aWindowGroupId);
private:
	CTextFormatter* iFormatter;
	TUint iWgId;
	TBool iVerbose;
	TBool iShowAll;
	};
