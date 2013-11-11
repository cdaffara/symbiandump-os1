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

#ifndef __CLESESS_H__
#define __CLESESS_H__


#include <e32base.h>



class CCleServer;
class CSsmCommandUtilProvider;
/**
 The CLE's server session.
 
 @internalComponent
 @released
 */ 
NONSHARABLE_CLASS( CCleSession ) : public CSession2
	{
	
public:
	static CCleSession* NewL( CCleServer& aCleServer );
	~CCleSession();	
	// From CSession2
	void ServiceL( const RMessage2& aMessage );
	
private:
	CCleSession( CCleServer& aCleServer );
	void ConstructL();
	
	void ExecuteCommandListL( const RMessage2& aMessage );
	void ExecuteCommandListCancel();

private:
	CCleServer& iCleSvr;
	TInt iSessionIndex;
	CSsmCommandUtilProvider* iCommandUtilProvider;
	};



#endif // __CLESESS_H__
	
	
