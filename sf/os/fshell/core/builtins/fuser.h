// fuser.h
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

#include <fshell/ioutils.h>

using namespace IoUtils;

class CCmdFileUser : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFileUser();
private:
	CCmdFileUser();
	void PrintAllUsersL();
	void PrintUsersL(const TDesC& aFileName);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	RArray<TFileName2> iFiles;
	};

