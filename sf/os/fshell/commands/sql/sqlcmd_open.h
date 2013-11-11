// sqlcmd_open.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef __FSQLCMD_OPEN_H__
#define __FSQLCMD_OPEN_H__

#include <fshell/ioutils.h>


using namespace IoUtils;

////////////////////////////////////////////////////////////////////////////
class CSqlCmdOpen : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdOpen();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);	
	virtual void DoRunL();
private:
	HBufC* iFileName;
	};

/////////////////////////////////////////////////////////////////

class CSqlCmdExit : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdExit();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	};

///////////////////////////////////////////////////////////////////

class CSqlCmdClose : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdClose();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	};

/////////////////////////////////////////////////////////////////

class CSqlCmdCreate : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdCreate();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void DoRunL();
	
private:
	HBufC* iFileName;	
	};


//////////////////////////////////////////////////////////////////
class CSqlCmdExec : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdExec();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void DoRunL();
	
private:
	HBufC* iExec;	
	};

//////////////////////////////////////////////////////////////////

class CSqlCmdState : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdState();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void DoRunL();
	
private:
	HBufC* iOptFileName;	//TEMP file template, used to dump large binary contents
	RPointerArray<HBufC> iOptParamFile;	//multiple filenames for use with parameter
	
	HBufC* iExec;	//the SQL statement
	//RPointerArray<HBufC> iCmdArgu;	//arguments could be multiple strings	
		
	};

/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////

class CSqlCmdAttach : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdAttach();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	void OptionsL(RCommandOptionList& aOptions);
	virtual void DoRunL();
	
private:
	HBufC* iOptFileName;
	HBufC* iOptDBName;	//database name
	};

///////////////////////////////////////////////////////////////////

class CSqlCmdDetach : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdDetach();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	void OptionsL(RCommandOptionList& aOptions);
	virtual void DoRunL();
	
private:
	HBufC* iOptDBName;	//database name
	};

/////////////////////////////////////////////////////////////////////

#ifdef SQL_COMPACT

//RSqlDatabase::Compact() is documented in Symbian Developer Library, 
//but not exists in header file.
//so just reserved this for future
class CSqlCmdCompact : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CSqlCmdCompact();
	static const TDesC& NameS();
private:
	void ConstructL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	void OptionsL(RCommandOptionList& aOptions);
	virtual void DoRunL();
	
private:
	HBufC* iOptDBName;	//database name
	};
#endif

#endif // __FSQLCMD_OPEN_H__
