// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __SSMSTATEPOLICYRESOLVER_H__
#define __SSMSTATEPOLICYRESOLVER_H__

#include <e32base.h>
#include "ssmstatepolicyresolverproxy.h"


class TSsmState;
class TParsePtrC;
class CSsmStatePolicyFrame;

/**
The CSsmStatePolicyResolver is passed a TSsmState and uses this to create a
CSsmStatePolicyFrame that encapsulates the corresponding policy DLL
The policy DLL is resolved using a naming standard described in @c MSsmStatePolicy.

@internalComponent
@released
*/
class CSsmStatePolicyResolver : public CBase, public MSsmStatePolicyResolverProxy
	{
public:
	static CSsmStatePolicyResolver* NewL();
	static CSsmStatePolicyResolver* NewLC();
	
		//from MPolicyResolverProxy
	void GetStatePolicyL(TSsmState aState);
	CSsmStatePolicyFrame* Policy();
	void ReleasePolicyResolver();

	
private:
	CSsmStatePolicyResolver();
	~CSsmStatePolicyResolver();
	void GetFileNameForState(const TSsmState& aState, TDes& aLibraryFilename) const;
	TBool NeedsLoading(const TParsePtrC& aLibraryFilename) const;
	void LoadLibraryLC(RLibrary& aLibrary, const TDesC& aLibraryFilename) const;
	CSsmStatePolicyFrame* CreatePolicyLC(const RLibrary& aLibrary) const;
	
private:
	RLibrary iLibrary;
	CSsmStatePolicyFrame* iPolicy;
	};

#endif
