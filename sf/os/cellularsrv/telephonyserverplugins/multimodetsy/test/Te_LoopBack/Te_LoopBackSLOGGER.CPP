// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the GSM Test Harness logging facility.
// The script engine will log significant events during the 
// during the course of a test.
// To turn on logging, created the folder C:\LOGS\ETEL.
// Note that this logging facility uses the Thread Local Storage for a thread.
// 
//

/**
 @file
*/

#include "Te_LoopBackSLOGGER.H"

#ifdef __LOGGER__
#ifdef __EXE__
CETelLogger* ScriptLoggerContext=NULL;
#endif

_LIT(KHayesLogFileName,"C:\\LOGS\\ETEL\\TsyLb.TXT");
_LIT(KHayesLogFolder,"C:\\LOGS\\ETEL\\");

const TUint KLogBufferSize=500;

CETelLogger* CETelLogger::NewL()
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CETelLogger.
 *
 * @leave   Leaves if out-of-memory.
 * @return  pointer to the instance of "CETelLogger".
 */
	{
	CETelLogger* logger=new(ELeave) CETelLogger();
	CleanupStack::PushL(logger);
	logger->ConstructL();
	CleanupStack::Pop();
	return logger;
	}

CETelLogger::CETelLogger() : iValid(EFalse)
/**
 * This method is the constructor for CETelLogger.
 *
 * @param   None.
 * @return	None.
 * @note    Initializes private boolean "iValid" to False.
 */
	{}

void CETelLogger::ConstructL()
/**
 * This method is used to implement the 2 Phase Constructor for CETelLogger.
 * This method sets up the logfile.
 *
 * @leave     Leaves if file can not be created.
 * @note      Logging does not take place if the logging directory has not been created.
 * \note      In debug mode, the logfile is not deleted at start of new session,
 * \note      the logging for each session will be appended to the previous logfile.
 */
	{
	if (FolderExists())
		{
		iFs.Connect();
		TInt ret=KErrNone;
	#if !(defined (_DEBUG))
		ret=iFs.Delete(KHayesLogFileName);
		if(ret==KErrNone || ret==KErrNotFound)
			ret=iFile.Create(iFs,KHayesLogFileName,EFileShareAny|EFileWrite);
	#else
		ret=iFile.Open(iFs,KHayesLogFileName,EFileShareAny|EFileWrite);
		if(ret!=KErrNone)
			ret=iFile.Create(iFs,KHayesLogFileName,EFileShareAny|EFileWrite);
	#endif
		if(ret==KErrNone)
			{
			iValid=ETrue;
			TInt aPos=0;
			iFile.Seek(ESeekEnd,aPos);
			ret=iFile.Write(_L8("----------New Log----------\015\012"));
			}
		}
	}

void CETelLogger::Destruct()
/**
 * This method is used to delete the instantion of CETelLogger.
 *
 * @param   None.
 * @return	None.
 * @note	None.
 */
	{
#ifdef __EXE__
	CETelLogger* context=ScriptLoggerContext;
	delete context;
	ScriptLoggerContext=NULL;
#else
	CETelLogger* context=(CETelLogger*) Dll::Tls();
	delete context;
	Dll::SetTls(NULL);
#endif
	}

CETelLogger::~CETelLogger()
/**
 * This method is the Destructor for the CETelLogger class and as such, closes
 * the logfile.
 *
 * @param   None.
 * @return	None.
 * @note    None.
 */
	{
	if(iValid)
		iFile.Close();
	iFs.Close();
	}

void CETelLogger::WriteL(const TDesC8& aText)
/**
 * This is a static method that is used to write a record of information 
 * into the logfile.  This method is used to place a text string without
 * any variable arguments in the string of input into the logfile.  If an 
 * instance of the CETelLogger class does not exist, this method is used 
 * to create it.  This routine only creates one instance of CETelLogger 
 * for each session. 
 *
 * @param   reference to the text string to be written into the logfile.
 * @leave   Leaves when no memory to create CETelLogger.
 * @note    This is a static method.
 * @note    Logging does not take place if the logging directory does not exist.
 * @note	The logfile is not deleted at start of a new test session, the
 * @note    logging for each test session will be appended in the logfile.
 */
	{
#ifdef __EXE__
	CETelLogger* context=ScriptLoggerContext;
#else
	CETelLogger* context=(CETelLogger*) Dll::Tls();
#endif
	if(context==NULL)
		{
		context=CETelLogger::NewL();
#ifdef __EXE__
		ScriptLoggerContext=context;
#else
		Dll::SetTls(context);
#endif
		}
	if(context->iValid)
		context->WriteRecord(aText);
	}

void CETelLogger::Write(const TText8* aText)
/**
 * Static method used as the entry point to write information into the logfile.
 *
 * @param   pointer to the text string to be written into the logfile.
 * @return	None.
 * @note    This is a static method.
 */
	{
	TPtrC8 text(aText);
	TRAP_IGNORE(WriteL(text));
	}

void CETelLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
/**
 * This is a static method that is used to write a record of information 
 * into the logfile.  This method is used to place a text string with 
 * one or more variable arguments in the string of input into the logfile.
 * If an instance of the CETelLogger class does not exist, this method is 
 * used to create it.  This routine only creates one instance of 
 * CETelLogger for each session. 
 *
 * @param   variable argument list for the text and data in the string
 * @return	None.
 * @note    This is a static method.
 * @note    Logging does not take place if the logging directory does not exist.
 * @note	The logfile is not deleted at start of a new test session, the
 * @note    logging for each test session will be appended in the logfile.
 */
	{
	TBuf8<KLogBufferSize> buf;
    VA_LIST list;
    VA_START(list,aFmt);
    buf.FormatList(aFmt,list);
	TChar tmpchar;
	for(TInt i=0;i<buf.Length();i++)
		{
		tmpchar=buf[i];
		if(!((tmpchar.IsPrint()) || (tmpchar=='\n') || (tmpchar=='\r') || (tmpchar=='\t')))
			buf[i]='.';
		}
#ifdef __EXE__
	CETelLogger* context=ScriptLoggerContext;
#else
	CETelLogger* context=(CETelLogger*) Dll::Tls();
#endif
	if(context==NULL)
		{
		TRAP_IGNORE(context=CETelLogger::NewL());		// trap but ignore leaves
#ifdef __EXE__
		ScriptLoggerContext=context;
#else
		Dll::SetTls(context);
#endif
		}
	if(context->iValid)
		context->WriteRecord(buf);
	}

void CETelLogger::WriteRecord(const TDesC8& aText)
/**
 * This method is used to add date and time information to the text string 
 * and input it as a record into the logfile.
 *
 * @param   reference to the text string to be written into the logfile.
 * @return	None.
 * @note    This is a static method.
 */
	{
	if(iValid)
		{
		TBuf8<KLogBufferSize> buf;
		TTime now;
		now.UniversalTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		buf.Format(_L8 ("%02d.%02d:%02d:%06d "),dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
		buf.AppendFormat(_L8("%S\015\012"),&aText);
		iFile.Write(buf);
		iFile.Flush();
		}
	}

TBool CETelLogger::FolderExists()
/**
 * This method determines if the folder c:\logs\Etel exists.
 *
 * @param   None.
 * @return	boolean value of ETrue if folder and EFalse if folder does not exist.
 * @note    This is a static method.
 */
	{
	TUint n;
	iFs.Connect();
	TInt ret=iFs.Att(KHayesLogFolder,n);
	iFs.Close();

	if (ret==KErrNone)
		return ETrue;
	return EFalse;
	}

#endif
