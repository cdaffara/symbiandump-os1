// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SYSMONDEMO_H__
#define __SYSMONDEMO_H__

#include "e32std.h" 
#include "e32base.h"


_LIT(KDemoServerName, "SysMonDemoServ") ;
_LIT(KDemoServerImage, "Z:\\sys\\bin\\sysmondemoserv.exe") ;


class CDemoSSess;
class CDemoServ;

class RDemoCli: public RSessionBase
{
	public:
		TInt Open() ;
		void Close() ;	
		void Poke(TTime &time) ;
		void Kill(TTime &time) ;
		void Done(TTime &time) ;		
};

class CDemoSSess: public CSession2
{
	public:
		static CDemoSSess* NewL() ;
		~CDemoSSess() ;
		virtual void ServiceL(const RMessage2 &aMessage) ;		
		void CmdPokeL() ;		
		void CmdKillL() ;		
		void CmdDoneL() ;		
		CDemoServ* DemoServer() ;	
	private:
		RMessage2 iMessage ;	
};

class CDemoServ: public CServer2
{
	public:
		static CDemoServ* NewLC() ;
		CDemoServ() ;
		~CDemoServ() ;
		TTime GetLoadTime() ;
		
	protected:
	//	virtual void RunL() ;
	//	virtual void DoCancel() ;
		CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	
	private:
		TTime iLoadTime ;	
};

enum TSysMonDemoCom
	{
	EServerPoke = 0,
	EServerKill,
	EServerDone
	};


#endif
