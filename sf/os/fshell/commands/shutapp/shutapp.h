// shutapp.h
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


class CCmdShutApp : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdShutApp();
private:
	CCmdShutApp();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	void ShutL(RWsSession& aWsSession, TInt aWindowGroupId);
private:
	RArray<TUint> iWgIds;
	RPointerArray<HBufC> iWgNames;
	TInt iTimeout;
	TBool iForce;
	TBool iVerbose;
	TBool iAll;
	};
