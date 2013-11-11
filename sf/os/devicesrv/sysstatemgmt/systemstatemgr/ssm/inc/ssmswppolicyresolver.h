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

#ifndef __SSMSWPPOLICYRESOLVER_H__
#define __SSMSWPPOLICYRESOLVER_H__

#include <e32base.h>
#include "ssmswppolicyresolverproxy.h"


class TSsmSwp;
class CSsmSwpPolicyFrame;
class CSsmSwpPolicyMap;

/**
The CSsmSwpPolicyResolver is passed an swp value and uses this to create an swp policy 
frame that encapsulates the corresponding policy DLL
The swp policy is resolved using a map of swp values to policy file names.
The mapping of swp values to names is registered using the appropriate function.

@internalComponent
@released
*/
class CSsmSwpPolicyResolver : public CBase, public MSsmSwpPolicyResolverProxy
	{
public:
	static CSsmSwpPolicyResolver* NewL();
	void ConstructL();
	void RegisterSwpMappingL(TUint aSwpKey, const TDesC& aFile);
#ifdef _DEBUG
	void DeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename);
#endif
	
	//from MSsmSwpPolicyResolverProxy
	CSsmSwpPolicyFrame* GetSwpPolicyL(const TSsmSwp& aSwp);
	
	~CSsmSwpPolicyResolver();
	
private: // methods
	CSsmSwpPolicyResolver();
	void GetFileNameForSwpL(const TSsmSwp& aSwp, TDes& aLibraryFilename) const;
	void LoadLibraryLC(RLibrary& aLibrary, const TDesC& aLibraryFilename) const;
	CSsmSwpPolicyFrame* CreatePolicyLC(const RLibrary& aLibrary) const;
	
private: // members
	CSsmSwpPolicyMap* iSwpPolicyMap;
	};

#endif
