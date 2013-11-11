// sqlsrv.cpp
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
#include "sqlsrv.h"

#include "sqlcmd_open.h"

_LIT(KNewLine, "\r\n");
_LIT(KPrompt, "ok\r\n");

_LIT(KHistoryFile, "c:\\system\\console\\fshell_sqlsrv\\history");

const TInt KFileBufferSize = 32768;

CCmdSqlSrv*	CCmdSqlSrv::iOnlyInstance = NULL;

CCommandBase* CCmdSqlSrv::NewLC()
	{
	if (CCmdSqlSrv::iOnlyInstance)
		{
		User::LeaveIfError(KErrAlreadyExists);
		}
	
	CCmdSqlSrv* self = new (ELeave)CCmdSqlSrv();
	CCmdSqlSrv::iOnlyInstance = self;
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

//static:
//Check if a string is wrapped by double quote, and removed them
//
void CCmdSqlSrv::StripWrapDoubleQuote(HBufC& aText)
	{
	RBuf text;
	text.Create(aText);
	text.Trim();
	if (text.Left(1)==_L("\"") && text.Right(1)== _L("\""))
		{
		//delete right side quote
		text.Delete(text.Length()-1, 1);
		//delete left side quote
		text.Delete(0,1);
		aText = text;
		}
	text.Close();
	}



//static 
CCmdSqlSrv* CCmdSqlSrv::GetServer()
	{
	return CCmdSqlSrv::iOnlyInstance;	
	}

CCmdSqlSrv::CCmdSqlSrv()
	: CServerBase(0, KPrompt, KHistoryFile), iNextCommandId(1)
	{
	iTmpFileNo = 1;
	iDatabaseOpened = EFalse;
	}

void CCmdSqlSrv::ConstructL()
	{
	BaseConstructL();
	}

CCmdSqlSrv::~CCmdSqlSrv()
	{
	iSqlDb.Close();	
	CCmdSqlSrv::iOnlyInstance = NULL;
	}

//////////////////////////////////////////////////////


//////////////////////////////////////////////////////

const TDesC& CCmdSqlSrv::Name() const
	{
	_LIT(KName, "sqlsrv");
	return KName;
	}

const TDesC& CCmdSqlSrv::Description() const
	{
	_LIT(KDescription, "a command to interact with Symbian SQLite server");
	return KDescription;
	}

void CCmdSqlSrv::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CCmdSqlSrv::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	}

TInt CCmdSqlSrv::NextCommandId()
	{
	return iNextCommandId++;
	}

//when retrieving SQL records, if there is any binary columns and user has specified
//a Temp file template, then we will generate a temp file and dump the binary into such file.
void CCmdSqlSrv::MakeTempFilename(TDes& aTmpFile, TDesC& aTemplate)
	{
	TFileName2 tmpTemplate(aTemplate);
	TPtrC drvPath = tmpTemplate.DriveAndPath();
	TPtrC name = tmpTemplate.Name();
	TPtrC ext = tmpTemplate.Ext();
	
	aTmpFile.Format(_L("%S%S.%02d%S"), &drvPath, &name, iTmpFileNo, &ext);
	iTmpFileNo++;
	}


void CCmdSqlSrv::ReportResult(const TServerCommandId& aId, const TDesC& aName, TRefByValue<const TDesC> aFmt, ...)
	{
	CheckNewConsoleLine();
	DoPrintf(_L("result:%u:%S:"), aId.Value(), &aName);
	VA_LIST list;
	VA_START(list, aFmt);
	DoPrintList(aFmt, list, ENewLine);
	VA_END(list);
	}


void CCmdSqlSrv::InitializeL()
	{
	CServerCommandFactory& factory = Factory();
	
	factory.AddLeafCommandL<CSqlCmdExit>();
	factory.AddLeafCommandL<CSqlCmdOpen>();
	factory.AddLeafCommandL<CSqlCmdCreate>();
	factory.AddLeafCommandL<CSqlCmdExec>();
	factory.AddLeafCommandL<CSqlCmdClose>();	
	factory.AddLeafCommandL<CSqlCmdState>();
	factory.AddLeafCommandL<CSqlCmdAttach>();
	factory.AddLeafCommandL<CSqlCmdDetach>();
	
#ifdef SQL_COMPACT	
	factory.AddLeafCommandL<CSqlCmdCompact>();
#endif
		
	}

void CCmdSqlSrv::DoPrintf(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	DoPrintList(aFmt, list);
	VA_END(list);
	}

void CCmdSqlSrv::DoPrintf(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	DoPrintList(aFmt, list);
	VA_END(list);
	}

void CCmdSqlSrv::DoPrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList, TPrintPostfix aPostfix)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	DoPrint(buf, aPostfix);
	}

void CCmdSqlSrv::DoPrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList, TPrintPostfix aPostfix)
	{
	TOverflowTruncate8 overflow;
	TBuf8<0x200> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	if (buf.Length() > 0x100) buf.SetLength(0x100); // Truncate to half the buffer size so that the call to Expand doesn't panic
	TPtrC wideBuf = buf.Expand();
	DoPrint(wideBuf, aPostfix);
	}

void CCmdSqlSrv::DoPrint(const TDesC& aDes, TPrintPostfix aPostfix)
	{
	Stdout().Write(aDes);
	if ((aPostfix == ENewLine) && ((aDes.Length() < KNewLine().Length()) || (aDes.Right(KNewLine().Length()) != KNewLine)))
		{
		Stdout().Write(KNewLine);
		}
	}


void CCmdSqlSrv::Report(const TServerCommandId& /*aId*/, const TDesC& /*aDes*/)
	{
	//TODO?
//	Log(aId, aDes);
	}

void CCmdSqlSrv::Report(const TServerCommandId& /*aId*/, const TDesC8& /*aDes*/)
	{
	//TODO?
//	Log(aId, aDes);
	}

void CCmdSqlSrv::ReportWarning(const TServerCommandId& aId, const TDesC& aDes)
	{
	CheckNewConsoleLine();
	DoPrintf(_L("warning:%u:%S\r\n"), aId.Value(), &aDes);
	}

void CCmdSqlSrv::ReportError(const TServerCommandId& aId, TInt aError, const TDesC& aDes)
	{
	CheckNewConsoleLine();
	DoPrintf(_L("error:%d:%u:%S\r\n"), aError, aId.Value(), &aDes);
	}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//aCommand: the command which is invoking this function, SHOULD NOT BE NULL
//
void CCmdSqlSrv::SqlCreateL(TDesC& aFilename, CServerCommandBase* aCommand)
	{
	if (iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("A database is already opened"));
	
	iSqlDb.CreateL(aFilename);
	
	//Print size information if no error occurrs
	TInt size = iSqlDb.Size();
	ReportResult(aCommand->Id(), aCommand->Name(), _L("Database created, size: %d bytes.\r\n"), size);
	iDatabaseOpened = ETrue;
	}

void CCmdSqlSrv::SqlOpenL(TDesC& aFilename, CServerCommandBase* aCommand)
	{
	if (iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("A database is already opened"));
		
	iSqlDb.OpenL(aFilename);
	
	//Print size information if no error occurrs
	TInt size = iSqlDb.Size();
	ReportResult(aCommand->Id(), aCommand->Name(), _L("Database opened, size: %d bytes.\r\n"), size);
	iDatabaseOpened = ETrue;

	//more detailed size information seems only available from Symbian OS 9.5 or later.
/*	
	RSqlDatabase::TSize size;
	iSqlDb.Size(size);
	ReportResult(aCommand->Id(), aCommand->Name(), _L("Database opened, size: %Ld bytes. free:%Ld bytes\r\n"), 
			size.iSize, size.iFree);
*/			
	}

//execute a SQL statement without parameter/response
void CCmdSqlSrv::SqlExecL(TDesC& aStatement, CServerCommandBase* aCommand)
	{
	TInt err;
	if (!iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("No database opened"));
		
	err = iSqlDb.Exec(aStatement);
	
	if (err < 0)
		{
		TPtrC errMsg = iSqlDb.LastErrorMessage();
		ReportResult(aCommand->Id(), aCommand->Name(), _L("Exec return code:%d message:%S\r\n"),
				err, &errMsg);				
		}
	
	User::LeaveIfError(err);
	
	//print the return code if it's not error
	ReportResult(aCommand->Id(), aCommand->Name(), _L("Exec return code:%d\r\n"),err);		
	}

//execute a SQL statement with parameter/response
//aStatement: SQL statement
//aOptParamFile: contains multiple string (HBufC*):should be parameters for that statement
//aTempFileTemplate: template to generate temp file, used to dump binary contents
void CCmdSqlSrv::SqlStateL(TDesC& aStatement, RPointerArray<HBufC> &aOptParamFile, CServerCommandBase* aCommand, TDesC* aTempFileTemplate)
	{
	TInt err;
	if (!iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("No database opened"));
	
	RSqlStatement sqlState;
	CleanupClosePushL(sqlState);
	
	err = sqlState.Prepare(iSqlDb, aStatement);
	if (err < 0)
		{
		TPtrC errMsg = iSqlDb.LastErrorMessage();
		ReportResult(aCommand->Id(), aCommand->Name(), _L("RSqlStatement::Prepare() return code:%d message:%S\r\n"),
				err, &errMsg);				
		}
	User::LeaveIfError(err);

	//depend on the statement type, for something like SELECT, which is expected to have returned results,
	//we should call Next(),
	//otherwise we should call Exec();
	//Panic will occur if calling wrong function
	//so check if this statement is SELECT
	TBool bIsSelect;
	TBuf<64> strBegin;
	strBegin.Copy(aStatement.Left(64));
	strBegin.TrimLeft();
	strBegin.UpperCase();
	
	if (strBegin.Left(6)==_L("SELECT"))
		bIsSelect = ETrue;
	else
		bIsSelect = EFalse;
	
	if (bIsSelect) 
		{
		//once successfully execute the statement, check if there is any response.
		//only quit the loop when there is no more results found
		TInt totalRecordsReturned = 0;	
		while(ETrue)
			{	
			err =  sqlState.Next();
			if (err==KSqlAtRow)	//ready to retrieve a result (usual response for SELECT command)
				{
				totalRecordsReturned++;
				ReportResult(aCommand->Id(), aCommand->Name(), _L("Record #%d =========================\r\n"), totalRecordsReturned);
				
				ParseResultL(sqlState, aCommand, aTempFileTemplate);
					
				}
			else if (err==KSqlAtEnd) //no records found
				{
				ReportResult(aCommand->Id(), aCommand->Name(), _L("===================================\r\n"));
				ReportResult(aCommand->Id(), aCommand->Name(), _L("%d record(s) returned\r\n"), totalRecordsReturned);
				break;
				}
			else 
				{
				User::LeaveIfError(err);			
				}
			}
		}
	else
		{		
		RSqlParamWriteStream strm;	//only will have effect when parameters are used
	    CleanupClosePushL(strm);
		//check if there is any parameters that need to be bound with the statement
	    TInt paramCnt = aOptParamFile.Count();
		if (paramCnt)
			{
			for (TInt paramId = 0; paramId<paramCnt; paramId++)
				{
				HBufC* paramFile = aOptParamFile[paramId];
				RFile file;
				CleanupClosePushL(file);
				err = file.Open(FsL(), *paramFile, EFileRead);
				LeaveIfErr(err, _L("Error openning file %S"), paramFile);
				
				err = strm.BindBinary(sqlState, paramId);
				User::LeaveIfError(err);
				
				//dump the file content the the parameter
				//file could be very large, so it is wise to separate it into several times. each time 32K or so.
				{
				TInt fileSize; 
				file.Size(fileSize);
				RBuf8 buf;
				buf.CreateL(KFileBufferSize);	//each time 32KB

				for (TInt curPos = 0; curPos<fileSize ;)
					{
					TInt remaingBytes = (fileSize-curPos);
					TInt bytesToProc = (remaingBytes>KFileBufferSize)? KFileBufferSize : remaingBytes;
					err = file.Read(curPos, buf, bytesToProc);
					User::LeaveIfError(err);
					
					strm.WriteL(buf);
					curPos += bytesToProc;
					}
				
				buf.Close();
				strm.CommitL();
				strm.Close();
				}
				
				CleanupStack::PopAndDestroy();	//file
				}
		
			}				
		err = sqlState.Exec();
		ReportResult(aCommand->Id(), aCommand->Name(), _L("Exec return code:%d\r\n"),err);
		
		CleanupStack::PopAndDestroy();	//strm
		}
	
	CleanupStack::PopAndDestroy();
	}


//close SQL database file
void CCmdSqlSrv::SqlClose(CServerCommandBase* aCommand)
	{
	if (!iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("No database opened"));
	
	iSqlDb.Close();
	//print the return code if it's not error
	ReportResult(aCommand->Id(), aCommand->Name(), _L("SQL database file closed\r\n"));
	iDatabaseOpened = EFalse;
	}

//attach additional database onto main database
void CCmdSqlSrv::SqlAttachL(TDesC& aFilename, TDesC& aDateBaseName, CServerCommandBase* aCommand)
	{
	TInt err;
	if (!iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("No database opened"));
	
	err = iSqlDb.Attach(aFilename, aDateBaseName);
	ReportResult(aCommand->Id(), aCommand->Name(), _L("RSqlDatabase::Attach return code:%d\r\n"), err);		
	User::LeaveIfError(err);	
	}

//
void CCmdSqlSrv::SqlDetachL(TDesC& aDateBaseName, CServerCommandBase* aCommand)
	{
	TInt err;
	if (!iDatabaseOpened)
		LeaveIfErr(KErrGeneral, _L("No database opened"));
	
	err = iSqlDb.Detach(aDateBaseName);
	ReportResult(aCommand->Id(), aCommand->Name(), _L("RSqlDatabase::Detach return code:%d\r\n"), err);		
	User::LeaveIfError(err);	
	}

#ifdef SQL_COMPACT
//RSqlDatabase::Compact() is documented but not implemented, so Do not use for now
//aDateBaseName: can be NULL (compacting main database)
void CCmdSqlSrv::SqlCompactL(TDesC* aDateBaseName, CServerCommandBase* aCommand)
	{
	TInt err;
	err = iSqlDb.Compact(0, aDateBaseName);
	ReportResult(aCommand->Id(), aCommand->Name(), _L("RSqlDatabase::Compact return code:%d\r\n"), err);		
	User::LeaveIfError(err);	
	}
#endif

////////////////////////////////////////////////////////////////////////////////////////////


//once we got a result from a SELECT command, then we go through the returned result
//only call this function after calling Next().
//we only analyse ONE returned record in this function
void CCmdSqlSrv::ParseResultL(RSqlStatement& aState, CServerCommandBase* aCommand, TDesC* aTempFileTemplate)
	{
	TInt err;
	TInt colCnt = aState.ColumnCount();
	TBuf<64> colHdr;	//column information fixed header
	
	for (TInt i=0; i<colCnt; i++)
		{
		TPtrC columnName;
		err = aState.ColumnName(i, columnName);
		if (err != KErrNone)
			{
			_LIT(KNotAvailable, "N/A");
			columnName.Set(KNotAvailable);
			}
				
		TSqlColumnType colType = aState.ColumnType(i);
		const TDesC* pTypeStr = ColumnTypeToString(colType);
		TInt colSize = aState.ColumnSize(i);
		
		colHdr.Format(_L("Column %d \"%S\"(%S)"), i, &columnName, pTypeStr);
		
		switch(colType)
			{
			case ESqlNull:
				{
				ReportResult(aCommand->Id(), aCommand->Name(), _L("%S \r\n"), &colHdr);	
				break;
				}
			case ESqlInt:
				{
				TInt colValue = aState.ColumnInt(i);
				ReportResult(aCommand->Id(), aCommand->Name(), _L("%S %d\r\n"), &colHdr, colValue);	
				break;
				}
			case ESqlInt64:
				{
				TInt64 colValue = aState.ColumnInt64(i);
				ReportResult(aCommand->Id(), aCommand->Name(),_L("%S %Ld\r\n"), &colHdr , colValue);
				break;
				}			
			case ESqlReal:
				{
				TInt64 colValue = aState.ColumnReal(i);
				ReportResult(aCommand->Id(), aCommand->Name(),_L("%S %f\r\n"), &colHdr, colValue);				
				break;
				}						
			case ESqlText:
				{
				TPtrC  colValue = aState.ColumnTextL(i);
				ReportResult(aCommand->Id(), aCommand->Name(),_L("%S %S\r\n"), &colHdr, &colValue);				
				break;
				}						
			case ESqlBinary:
				{
				TBool dumpToFile = EFalse;
				TBuf<128> printableHex;
				TFileName tmpFileName;
				
				RSqlColumnReadStream strm;
				CleanupClosePushL(strm);
				err = strm.ColumnBinary(aState, i);
				
				//generate a temp file name for dumping binary content
				RFile tmpFile;
				CleanupClosePushL(tmpFile);
				if (aTempFileTemplate && aTempFileTemplate->Length()>0 )
					{
					MakeTempFilename(tmpFileName, *aTempFileTemplate);
					//open the file
					err = tmpFile.Replace(FsL(), tmpFileName, EFileWrite);
					if (err==KErrNone)
						dumpToFile = ETrue;
					}
				
				//go through the binary content, and dump it if tmp file is available
				RBuf8 readBuf;
				CleanupClosePushL(readBuf);
				readBuf.CreateL(KFileBufferSize);
				for(TInt cursor=0; cursor < colSize; )
					{
					TInt bytesRemain = (colSize-cursor);
					TInt bytesToProc = (bytesRemain>KFileBufferSize) ? KFileBufferSize : bytesRemain;
					strm.ReadL(readBuf, bytesToProc);
					
					if (cursor==0)	//it is beginning the binary data, always print first few bytes on screen
						{
						TInt byteCnt = bytesToProc;
						TInt byteToPrint = (byteCnt > 32)? 32 : byteCnt;
						printableHex.AppendFormat(_L("DataLength:%d Hex: "), colSize);
						
						for (TInt a=0; a<byteToPrint; a++)
							{
							TInt byteVal = readBuf[a]; 
							printableHex.AppendFormat(_L("%02x "), byteVal);
							}
						if (byteToPrint < byteCnt)
							printableHex.AppendFormat(_L("..."));
						}
					
					if (dumpToFile)
						{
						err = tmpFile.Write(cursor, readBuf, bytesToProc);
						if (err != KErrNone) 
							{
							dumpToFile = EFalse;
							break;
							}
						}
					else
						break;
					
					cursor += bytesToProc; 
					}								
				CleanupStack::PopAndDestroy(); //readBuf			
				CleanupStack::PopAndDestroy(); //tmpFile
				CleanupStack::PopAndDestroy();//strm;				
				ReportResult(aCommand->Id(), aCommand->Name(),_L("%S %S\r\n"), &colHdr, &printableHex);
				if (dumpToFile)
					{
					ReportResult(aCommand->Id(), aCommand->Name(),_L("Binary content dumped to file %S\r\n"), 
							&tmpFileName);
					}
				}
				break;
			}
		
		}
	}



EXE_BOILER_PLATE(CCmdSqlSrv)

#define CASE_LIT(x) case x: { _LIT(KName, #x); pString = &KName; break; }

//static
const TDesC* CCmdSqlSrv::ColumnTypeToString(TSqlColumnType aType)
	{
	const TDesC* pString = NULL;
	switch(aType)
		{
		CASE_LIT(ESqlNull);
		CASE_LIT(ESqlInt);
		CASE_LIT(ESqlInt64);
		CASE_LIT(ESqlReal);
		CASE_LIT(ESqlText);
		CASE_LIT(ESqlBinary);
		default:
			_LIT(KUnknowStr, "Unknown");
			pString = &KUnknowStr;		
		}
	return pString;	
	}

void CCmdSqlSrv::PrintTime(const TTime& aTime, TBool aNewline)
	{	
	TTime NullTime = Time::NullTTime();
	if (aTime == NullTime) 
		{
		Printf(_L("(NullTime)"));
		}
	else
		{
		_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
		TDateTime dt = aTime.DateTime();
		Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
		}
	
	if (aNewline) Printf(_L("\r\n"));
	}
