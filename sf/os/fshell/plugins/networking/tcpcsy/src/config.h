// config.h
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <e32std.h>
#include <e32base.h>
#include <in_sock.h>
#include <centralrepository.h>


class CTcpPortFactory;


class CConfig : public CBase
	{
public:
	enum TMode
		{
		EActive,
		EPassive
		};
public:
	static CConfig* NewL(CTcpPortFactory& aFactory);
	~CConfig();
	TMode ModeL(const TUint aUnit) const;
	TUint ProtocolFamilyL(const TUint aUnit) const;
	TUint ProtocolL(const TUint aUnit) const;
	void GetAddressL(const TUint aUnit, TInetAddr& aAddress) const;
	void UnitRange(TUint& aLowUnit, TUint& aHighUnit) const;
private:
	CConfig(CTcpPortFactory& aFactory);
	void ConstructL();
	TInt ReadIntL(TInt aFieldId, const TUint aUnit) const;
	void ReadStringL(TInt aFieldId, const TUint aUnit, TDes& aString) const;
private:
	CTcpPortFactory& iFactory;
	CRepository* iRepository;
	};


#endif // __CONFIG_H__
