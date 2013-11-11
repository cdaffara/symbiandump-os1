// sqlsrv.h
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
#ifndef __SQLSRV_H__
#define __SQLSRV_H__
//  Include Files

#include <fshell/common.mmh>

#include <fshell/ioutils.h>
#include <SqlDb.h>

//class CSqlShared;
//class CLineEditor;

using namespace IoUtils;

class CCmdSqlSrv : public CServerBase
	{
public:
	static const TDesC* ColumnTypeToString(TSqlColumnType aType);
	static void StripWrapDoubleQuote(HBufC& aText);

	void PrintTime(const TTime& aTime, TBool aNewline);	
	static CCommandBase* NewLC();
	static CCmdSqlSrv* GetServer();	
	~CCmdSqlSrv();
	
	TInt NextCommandId();
	
	//SQL commands
	void SqlCreateL(TDesC& aFilename, CServerCommandBase* aCommand);
	void SqlOpenL(TDesC& aFilename, CServerCommandBase* aCommand);
	void SqlExecL(TDesC& aStatement, CServerCommandBase* aCommand);
	void SqlStateL(TDesC& aStatement, RPointerArray<HBufC> &aOptParamFile, CServerCommandBase* aCommand, 
			TDesC* aTempFileTemplate = NULL);
	void SqlClose(CServerCommandBase* aCommand);
	void SqlAttachL(TDesC& aFilename, TDesC& aDateBaseName, CServerCommandBase* aCommand);
	void SqlDetachL(TDesC& aDateBaseName, CServerCommandBase* aCommand);
	
#ifdef SQL_COMPACT	
	void SqlCompactL(TDesC* aDateBaseName, CServerCommandBase* aCommand);
#endif
	///////////////////////////////////
	void ReportResult(const TServerCommandId& aId, const TDesC& aName, TRefByValue<const TDesC> aFmt, ...);

	//////////////////////////////////
	void ParseResultL(RSqlStatement& aState, CServerCommandBase* aCommand, TDesC* aTempFileTemplate);

private:
	CCmdSqlSrv();
	void ConstructL();
	void MakeTempFilename(TDes& aTmpFile, TDesC& aTemplate);

private:
	enum TPrintPostfix
		{
		ENone,
		ENewLine
		};

	void DoPrintf(TRefByValue<const TDesC> aFmt, ...);
	void DoPrintf(TRefByValue<const TDesC8> aFmt, ...);
	void DoPrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList, TPrintPostfix aPostfix = ENone);
	void DoPrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList, TPrintPostfix aPostfix = ENone);
	void DoPrint(const TDesC& aDes, TPrintPostfix aPostfix = ENone);
	
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;

	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From CServerBase.
	virtual void InitializeL();
	virtual void Report(const TServerCommandId& aId, const TDesC& aDes);
	virtual void Report(const TServerCommandId& aId, const TDesC8& aDes);
	virtual void ReportWarning(const TServerCommandId& aId, const TDesC& aDes);
	virtual void ReportError(const TServerCommandId& aId, TInt aError, const TDesC& aDes);
	
private:
	RSqlDatabase	iSqlDb;
	TBool			iDatabaseOpened;	//when database is created or opened, set this to ETrue
	
	TBool iExit;
	TBuf<512> iLine;
	TInt iExitReason;
	
	TBool iLineRead;
	
	TInt iNextCommandId;
	TInt iTmpFileNo;	
		
private:
	//use this static variable to make sure only one instance exists
	static CCmdSqlSrv*	iOnlyInstance;
	};


#endif //__SQLSRV_H__
