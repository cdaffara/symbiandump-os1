// sysinfo.h
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
#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include <hal.h>
#include <hal_data.h>

#include <fshell/ioutils.h>
using namespace IoUtils;

class CCmdSysInfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSysInfo();
private:
	CCmdSysInfo();
	void UpdateEnvironmentL();
	void PrintHalL(HALData::TAttribute aHalAttribute);
	void PrintPhoneIdL();
	void PrintWlanL();
	void PrintKernelHalStuff();
	void PagingFormat(TUint aFlags);
	void PrintRomVersion();
	static void IdentCapToString(TUint32& aCap, TDes &aDes);
	
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iMachineUIDOnly;
	};


#endif // __SYSINFO_H__
