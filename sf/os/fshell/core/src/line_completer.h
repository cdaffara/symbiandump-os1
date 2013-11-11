// line_completer.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __LINE_COMPLETER_H__
#define __LINE_COMPLETER_H__

#include <e32base.h>
#include <fshell/line_editor.h>

class RFs;
class TToken;
class TConsoleLine;
class CLexer;
class CTextBuffer;
class CCommandFactory;
namespace IoUtils
	{
	class CCommandInfoFile;
	}

class CLineCompleter : public CBase, public MLineCompleter
	{
public:
	static CLineCompleter* NewL(RFs& aFs, CCommandFactory& aCommandFactory, IoUtils::CEnvironment& aEnv);
	~CLineCompleter();
private: // From MLineCompleter.
	virtual void LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar);
private:
	CLineCompleter(RFs& aFs, CCommandFactory& aCommandFactory, IoUtils::CEnvironment& aEnv);
	void ConstructL();
	void CompleteCommandNameL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const;
	void CompleteFileNameL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const;
	void CompleteL(TConsoleLine& aLine, const TToken& aToken, const RArray<TPtrC> aPossibilities, const TDesC* aPrefix, const TDesC* aSuffix, TBool aPrefixIsPartOfToken=EFalse) const;
	void CompleteEnvVarL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const;
	void CompleteOptionL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const;

private:
	RFs& iFs;
	CCommandFactory& iCommandFactory;
	IoUtils::CEnvironment& iEnv;
	CLexer* iLexer;
	IoUtils::CCommandInfoFile* iLastUsedCif;
	};


#endif // __LINE_COMPLETER_H__
