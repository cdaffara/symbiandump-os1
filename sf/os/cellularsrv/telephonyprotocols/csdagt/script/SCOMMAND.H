/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Script Commands Header
* 
*
*/



/**
 @file Scommand.h
 @internalComponent
*/

#ifndef __SCOMMAND_H__
#define __SCOMMAND_H__

#include "SVARLAB.H"
#include "SCHCONV.H"
#include "SSCRREAD.H"

const TText KDollarChar='$';
const TText KUnderscoreChar='_';

// Forward declarations

class CScriptIO;
class CCommChatString;
class CScriptReader;
class CScriptExecutor;


class CScriptCommandBase : public CBase
/**
Base class for script commands.  Uses a CScriptReader and a CScriptVarMan.

@internalComponent
*/
	{
public:
	CScriptCommandBase(TScriptStatus& aStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	~CScriptCommandBase();
	virtual TBool ParseL() =0;
	virtual void Cleanup();
protected:
	virtual void ConstructL();
	TPtrC ParseCompoundExpressionL(TInt& aOffset);
	TPtrC ParseExpressionL(TInt& aOffset);
	TPtrC ParseStringL(TInt& aOffset);
	TPtrC ParseCharacterL(TInt& aOffset);
	TPtrC ParseVariableL(TInt& aOffset);
	TInt32 ParseIntegerL(TInt& aOffset);
	TReal ParseRealL(TInt& aOffset);
	TPtrC ParseCharacterTypeL(TInt& aOffset);
	HBufC8* ConvertLC(const TDesC& aString,TInt& aOffset);
	TInt FindTokenEnd(TInt aOffset);
	TBool IsValidChar(const TText& aChar);
	void EatSpaces(TInt& aOffset);
	void EatSpacesAndLinesL();
	void EatSpacesAndLinesAndCommentsL();
	void SetStatus(TInt& aOffset,TPtrC& aLine,TBool& aSkip,TBool& aSkipModeToggleReq);
	void SetStatus(const TScriptStatus& aStatus);
private:
	TPtrC ParseEnclosedStringL(TInt& aOffset,TText aEndChar,TInt aError);
protected:
	CScriptReader* iScriptReader;
	CScriptVarMan* iVarMan;
	CScriptVarMan* iConvertedVarMan;
	CScriptCharacterConverter* iCharConv;
	TInt iTempVarNum;
	TScriptStatus iStatus;
	};


class CSetCommand : public CScriptCommandBase
/**
Executes SET command. One of these per CScriptExecutor object.

@internalComponent
*/
	{
public:
	static CSetCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	CSetCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	~CSetCommand();
// CScriptCommandBase
	virtual TBool ParseL();
	};


class CSendCommand: public CScriptCommandBase
/**
Executes SEND command. One of these per CScriptExecutor object. Uses CScriptIO object.

@internalComponent
*/
	{
public:
	static CSendCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO);
	CSendCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO);
	~CSendCommand();
// CScriptCommandBase
	virtual TBool ParseL();
private:
	CScriptIO* iScriptIO;
	TPtrC8 iSendString;
	};


class CLabelSearch : public CBase
/**
@internalComponent
*/
	{
public:
	enum TLabelSearchStatus {
		ENotFound =-1,
		EResolved
		};
public:
	static CLabelSearch* NewLC(const TDesC& aLabelName);
	~CLabelSearch();
	inline TPtr LabelName() const;
	inline CCommChatString* ChatString() const;
	void CreateCommChatStringL(const TDesC8& aDes,TBool aIsFolded);
	inline void Set(TLabelSearchStatus aStatus,const TLinePosition& aPosition);
private:
	CLabelSearch();
	void ConstructL(const TDesC& aLabelName);
private:
	CCommChatString* iChatString;
	HBufC* iLabelName;
	TLabelSearchStatus iStatus;
	TLinePosition iPosition;
	};

typedef CArrayFixFlat<CLabelSearch*> CLabelSearchArray;


class CWaitCommand: public CScriptCommandBase
/**
Executes WAIT command. One of these per CScriptExecutor object.  Uses CScriptIO 
object and CScriptLabelMan.

@internalComponent
*/
	{
public:
	static CWaitCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO,CScriptLabelMan* aLabelMan, CScriptExecutor* aScriptExec);
	CWaitCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO,CScriptLabelMan* aLabelMan, CScriptExecutor* aScriptExec);
	~CWaitCommand();
// CScriptCommandBase
	virtual TBool ParseL();
	virtual void Cleanup();
	TPtrC LabelFromIndexL(TInt aIndex);
private:
	void ParseActionL();
	void ParseSkipL();
	CLabelSearch* ParseLabelLC();
	void DeleteLabelArray();
private:
	CScriptIO* iScriptIO;
	CScriptLabelMan* iLabelMan;
	CLabelSearchArray* iLabelArray;
	CScriptExecutor* iScriptExec;
	};


class CLoopCommand: public CScriptCommandBase
/**
Executes LOOP command. One of these per CScriptExecutor object.

@internalComponent
*/
	{
public:
	static CLoopCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	CLoopCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	~CLoopCommand();
// CScriptCommandase
	virtual TBool ParseL();
//
	TBool CheckLoopL();
	inline void Loop(TBool aLoop);
private:
	TBool iLoop;
	TLinePosition iLoopPosition;
	TInt iLoopCounter;
	};


class CGotoCommand: public CScriptCommandBase
/**
Executes GOTO command. One of these per CScriptExecutor object.  Uses a 
CScriptLabelMan object.

@internalComponent
*/
	{
public:
	static CGotoCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader, CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptLabelMan* aLabelMan);
	CGotoCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader, CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptLabelMan* aLabelMan);
	~CGotoCommand();
// CScriptCommandBase
	virtual TBool ParseL();
	void Goto(const TDesC& aLabelName);
	TBool ParseLabelL();
	void ServiceSkipReqs();
private:
	CScriptLabelMan* iLabelMan;
	TBuf<KMaxLabelLength> iSearchName;
	};


class CDTRCommand : public CScriptCommandBase
/**
Executes DTR command. One of these per CScriptExecutor object. Uses a CScriptIO object.

@internalComponent
*/
	{
public:
	static CDTRCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO);
	CDTRCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO);
	~CDTRCommand();
	// CScriptCommandBase
	virtual TBool ParseL();
private:
	CScriptIO* iScriptIO;
	};


class CReadPCTCommand : public CScriptCommandBase
/**
Esecutes READ command. One of these per CScriptExecutor object. Uses a CScriptIO object.

@internalComponent
*/
	{
public:
	static CReadPCTCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptExecutor* aScriptExec);
	CReadPCTCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptExecutor* aScriptExec);
	~CReadPCTCommand();
// CScriptCommandBase
	virtual TBool ParseL();
//
	TBool CheckReadL();
	TPtrC CharSet();
private:
	CScriptExecutor* iScriptExec;
	TBuf<KMaxCharacterTypeLength> iCharSet;
	};


class CCharMapCommand : public CScriptCommandBase
/**
Executes CHARMAP command. One of these per CScriptExecutor object.

@internalComponent
*/
	{
public:
	static CCharMapCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	CCharMapCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	~CCharMapCommand();
// CScriptCommandBase
	virtual TBool ParseL();
	};

class CExitCommand: public CScriptCommandBase
/**
Executes EXIT command. One of these per CScriptExecutor object.

@internalComponent
*/
	{
public:
	static CExitCommand* NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	CExitCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv);
	~CExitCommand();
// CScriptCommandBase
	virtual TBool ParseL();
	};

#include "SCOMMAND.INL"

#endif
