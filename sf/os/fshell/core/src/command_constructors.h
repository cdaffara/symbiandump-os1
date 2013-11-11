// command_constructors.h
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


#ifndef __COMMAND_CONSTRUCTORS_H__
#define __COMMAND_CONSTRUCTORS_H__

#include <e32base.h>
#include <fshell/ioutils.h>

class MCommand;
class MTaskRunner;

namespace LtkUtils { class RLtkBuf16; }
using LtkUtils::RLtkBuf16;

class CCommandConstructorBase : public CBase
	{
public:
	enum TAttributes
		{
		EAttExternal   = 0x00000001,
		EAttNotInHelp  = 0x00000002,
		EAttAlias      = 0x00000004
		};
	enum TType
		{
		ETypeUnknown,
		ETypeThread,
		ETypeAlias,
		ETypeExe,
		ETypePips
		};
public:
	const TDesC& CommandName() const;
	TUint Attributes() const;
	TType Type() const;
	void SetAttributes(TUint aAttributes);
	virtual MCommand* ConstructCommandL() = 0;
	virtual void AppendDescriptionL(RLtkBuf16& aBuf) const = 0;
	virtual ~CCommandConstructorBase();
protected:
	CCommandConstructorBase();
	CCommandConstructorBase(TType aType);
	void BaseConstructL(const TDesC& aCommandName);
private:
	TType iType;
	TUint iAttributes;
	HBufC* iCommandName;
	};

typedef IoUtils::CCommandBase* (*TCommandConstructor)();

class CThreadCommandConstructor : public CCommandConstructorBase
	{
public:
	static CThreadCommandConstructor* NewLC(TCommandConstructor aConstructor, TUint aFlags, MTaskRunner* aTaskRunner);
	static CThreadCommandConstructor* NewLC(const TDesC& aCommandName, TCommandConstructor aConstructor, TUint aFlags, MTaskRunner* aTaskRunner);
private:
	CThreadCommandConstructor(TUint aFlags, TCommandConstructor aConstructor, MTaskRunner* aTaskRunner);
private: // From CCommandConstructorBase.
	virtual MCommand* ConstructCommandL();
	virtual void AppendDescriptionL(RLtkBuf16& aBuf) const;
private:
	TUint iFlags;
	TCommandConstructor iConstructor;
	MTaskRunner* iTaskRunner;
	};

class CExeCommandConstructor : public CCommandConstructorBase
	{
public:
	static CExeCommandConstructor* NewLC(const TDesC& aCommandName, const TDesC& aExeName);
	const TDesC& ExeName() const;
private:
	CExeCommandConstructor();
	~CExeCommandConstructor();
	void ConstructL(const TDesC& aCommandName, const TDesC& aExeName);
private: // From CCommandConstructorBase.
	virtual MCommand* ConstructCommandL();
	virtual void AppendDescriptionL(RLtkBuf16& aBuf) const;
private:
	HBufC* iExeName;
	};

class CPipsCommandConstructor : public CCommandConstructorBase
	{
public:
	static CPipsCommandConstructor* NewLC(const TDesC& aCommandName);
private:
	CPipsCommandConstructor();
private: // From CCommandConstructorBase.
	virtual MCommand* ConstructCommandL();
	virtual void AppendDescriptionL(RLtkBuf16& aBuf) const;
	};

class CAliasCommandConstructor : public CCommandConstructorBase
	{
public:
	static CAliasCommandConstructor* NewLC(const TDesC& aCommandName, CCommandConstructorBase* aAliasedConstructor, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments);
	
private:
	~CAliasCommandConstructor();
	CAliasCommandConstructor(CCommandConstructorBase* aAliasedConstructor);
	void ConstructL(const TDesC& aCommandName, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments);
private: // From CCommandConstructorBase.
	virtual MCommand* ConstructCommandL();
	virtual void AppendDescriptionL(RLtkBuf16& aBuf) const;
private:
	CCommandConstructorBase* iAliasedConstructor;
	HBufC* iAdditionalArguments;
	HBufC* iReplacementArguments;
	};

#endif // __COMMAND_CONSTRUCTORS_H__
