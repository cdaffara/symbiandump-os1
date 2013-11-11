/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the macros for logging mechanism in LBT
*
*/


#ifndef LBT_LOGGER_H
#define LBT_LOGGER_H

/**
 * To use this logger mechanism you also need to add flogger.lib to the mmp
 * using,
 * DEBUGLIBRARY flogger.lib
 */


/**
 * The trace level is defined as follows,
 * 0x[moduleid - 4digits][logginglevel-4digits],
 * The currently define logginglevels are
 * 0x0001 - Verbose
 * things like entered method, leaving method etc,
 * logs of the parameters to methods, files 
 * that are being used, creating a key, creating a file,
 * opening a resource, 
 *
 * 0x0002 - Warning
 * things like events, error cases that were not processed 
 * but ignored.
 *
 * 0x0003 - Operations 
 * things like what operation started (create, delete), 
 * manipulations to the data structures, important events like
 * MMC card , app uninstall events which we process,
 * current location, triggers being monitored, psy conked off,
 * any other such important event.
 *
 * 0x0004 - Errors
 * log all _ASSERT_DEBUG kind of cases, basically things which 
 * you don't expect to happen but happened - Sounds like paradox ;-)
 * Also log all error cases with this trace level.
 *
 * 0x0005 - Fatal
 * Use this if the server cannot recover from this error and you 
 * have to crash !! ;-)
 *
 * So for a create trigger operation in server the log should be like
 * LBT_TRACE(KLbtLogOperation|KLbtLogServerLogic,"Creating Trigger");
 */
#define KLbtLogLevelMask    0x0000FFFF
 
#define KLbtLogVerbose      0x00000001
#define KLbtLogWarning      0x00000002
#define KLbtLogOperation    0x00000003
#define KLbtLogError        0x00000004
#define KLbtLogFatal        0x00000005

/**
 *
 * Then for the module id lets use
 * 0x0001 - Lbt Client Library
 * 0x0002 - Lbt Server Core
 * 0x0003 - Lbt Server Logic Base
 * 0x0004 - Lbt Server Logic ( ecom plugin )
 * 0x0005 - Lbt Container
 * 0x0006 - Lbt Strategy Plugin api
 * 0x0007 - Lbt Strategy Implementation ( Ecom plugin )
 * 0x0008 - Lbt Movement detector plugin api
 * 0x0009 - Lbt Cell id Movement Detector
 * Did I miss any component ?
 */
#define KLbtLogComponentMask            0xFFFF0000
 
#define KLbtLogClientLib                0x00010000
#define KLbtLogServerCore               0x00020000
#define KLbtLogServerLogicBase          0x00030000
#define KLbtLogServerLogic              0x00040000
#define KLbtLogContainer                0x00050000
#define KLbtLogStrategyPluginApi        0x00060000
#define KLbtLogStrategyEngine           0x00070000
#define KLbtLogMovementDetectorApi      0x00080000
#define KLbtLogCellIdMovementDetector   0x00090000

#ifdef _DEBUG
#define _LBT_DEBUG
#endif // #ifdef _DEBUG

// define _LBT_DEBUG to enable LBT logging
#define _LBT_DEBUG

#ifdef _LBT_DEBUG

#include <flogger.h>
#include <f32file.h>
#include "lbtloggerimpl.h"

_LIT8( KEndOfLineChars, "\r\n");

// CLASS DECLARATION
class TLogFunc
	{
	public:
		TLogFunc(const char aFunctionName[])
			{
			const unsigned char* string = (unsigned char*)(aFunctionName);
			iBuffer.Zero();
			iBuffer.Copy(string);			
			TBuf8<150> buf;
			buf.Zero();
			buf.Append(_L("=>"));
			buf.Append(iBuffer);
			CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger();
			if( loggerImpl )
			    {
			    RFileLogger& logger = loggerImpl->GetFileLogger();
			    logger.Write( buf );
			    }
			}

		~TLogFunc()
			{
			RThread thread;
			TInt exitReason = thread.ExitReason();
			if(exitReason != KErrNone)
				{
				TExitCategoryName name = thread.ExitCategory();
				TBuf8<400> buf;
				buf.Zero();
				buf.Append(_L("LBT PANICKED : "));
				buf.Append(name);
				
				CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger();
				if(loggerImpl)
					{
					RFileLogger& logger = loggerImpl->GetFileLogger();				
					logger.Write( buf);
					}
				// The thread has panicked and hence dont print the exit placeholder
				thread.Close();
				return;
				}
			iBuffer.Append(_L("=>"));
			CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger();
			if(loggerImpl)
				{
				RFileLogger& logger = loggerImpl->GetFileLogger();				
				logger.Write( iBuffer);
				}
			thread.Close();
			}
		
		static void FileName( TDes& aFileNameBuffer )
		    {
		    TTime currTime;
		    currTime.HomeTime();
		    TDateTime dt = currTime.DateTime();
		    
		    _LIT16( filefmt, "%S_%d%d%d_%d%d%d.log" );
		    
		    TBuf<150> logFileName;
		    logFileName.Append( _L("C:\\Logs\\Epos\\lbt_func") );
		    aFileNameBuffer.AppendFormat( filefmt(), &logFileName, dt.Day(), 
		                                dt.Month(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second() );
		    }

		static void FormatTextToBuffer( TDes8& aTrg, const TDesC8& aText )
		    {
		    TTime currTime;
		    currTime.HomeTime();
		    TDateTime dt = currTime.DateTime();
		    _LIT8( dtFmt, "%d/%d/%d %d:%d:%d ");
		    aTrg.AppendFormat( dtFmt(), dt.Day(), dt.Month(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second() );
		    aTrg.Append( aText );
		    aTrg.Append( KEndOfLineChars );
		    }
		
		static void RenameFileIfRequired()
			{
			RFs fs;
		    RFile file;
		    fs.Connect();
		    
			TInt err = file.Open( fs, _L("C:\\Logs\\Epos\\lbt_func.log"), EFileWrite|EFileShareAny );
	        if( KErrNone == err )
	            {
	            // Check Size of file
	            TInt size;
	            file.Size( size );
	            
	            if( size > 1024 )
	                {
	                file.Close();
	                TBuf<200> fileName;
	                FileName( fileName );
	                //BaflUtils::RenameFile( fs, _L("C:\\Logs\\Epos\\lbt.log"), fileName );
	                fs.Rename(_L("C:\\Logs\\Epos\\lbt_func.log"), fileName );
	                }	            
	            }
	        file.Close();
	        fs.Close();
			}

	private:
		TBuf8<150> iBuffer;
	};

#define FUNC_ENTER(x) //TLogFunc lbtlog(x);

#define STOPLOGGING CLbtLoggerImpl::Destroy();

#define ERROR(x,y)	{ TBuf8<150> buffer; \
					  const unsigned char* string = (unsigned char*)(x); \
					  buffer.Zero(); \
					  buffer.Copy(string); \
					  buffer.Append(_L(": LBT ERROR")); \
					  CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger(); \
					  if( loggerImpl ) \
					    { \
					    RFileLogger& logger = loggerImpl->GetFileLogger(); \
					    logger.WriteFormat( buffer, y); \
					    } \
					}

#define LOG(x)	{ TBuf8<150> buffer; \
				  const unsigned char* string = (unsigned char*)(x); \
				  buffer.Zero(); \
				  buffer.Copy(string); \
				  CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger(); \
				  if( loggerImpl ) \
				    { \
				    RFileLogger& logger = loggerImpl->GetFileLogger(); \
				    logger.Write( buffer); \
				    } \
				}
				  
#define LOG1(x,y)	{ TBuf8<150> buffer; \
					  const unsigned char* string = (unsigned char*)(x); \
					  buffer.Zero(); \
					  buffer.Copy(string); \
					  CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger(); \
					  if( loggerImpl ) \
					    { \
					    RFileLogger& logger = loggerImpl->GetFileLogger(); \
					    logger.WriteFormat( buffer, y); \
					    } \
					}
					  
#define LOGSTRING(x, y)  {  \
						 _LIT(KTmpStr, x); \
						 CLbtLoggerImpl* loggerImpl = CLbtLoggerImpl::CreateLogger(); \
						 if( loggerImpl ) \
						    { \
						    RFileLogger& logger = loggerImpl->GetFileLogger(); \
						    logger.WriteFormat( TRefByValue<const TDesC>(KTmpStr()), y); \
						    } \
					  	 } \


#define LBT_FILE_LOGGER 1 // Always log to file

const char* const LbtLogLevelStr[] = 
    {
    "",
    "Verbose",
    "Warning",
    "Operation",
    "Error",
    "Fatal"
    };

const char* const LbtLogComponentStr[] = 
    {
    "",
    "client lib",
    "server core",
    "server logic api",
    "server logic",
    "container",
    "strategy api",
    "strategy engine",
    "movement detector api",
    "cell id movement detector"
    };


#define LENGTH(str,len) for(len = 0; str[len]; len++) {}


NONSHARABLE_CLASS( TLbtDesOverflow ) : public TDes8Overflow
    {
    void Overflow(TDes8 &/*aDes*/)
        {
        // Dummy Implementation to prevent User panic 23
        }
    };
    
#define LBT_TRACE(level, file, lineno, desc...) 

/*
LOCAL_C void LBT_TRACE(TInt level, const char* file, TInt lineno, const char* desc,...)
    {
    _LIT8(fmt, "!%s %s %s:%d ");
    TInt desclength, filelength, fmtlength;
    fmtlength = fmt().Length();
    LENGTH(desc,desclength);
    LENGTH(file,filelength);

    HBufC8* str = NULL;
    TRAPD(error, str = HBufC8::NewL(desclength+fmtlength+filelength+10+25+200));
    if ( error != KErrNone )
        {
        return;
        }
    
    TPtr8 ptr(str->Des());
    ptr.Zero();

    TLbtDesOverflow ovrflowHandler;
    ptr.AppendFormat(fmt(),
                     &ovrflowHandler,
                     LbtLogLevelStr[(level&KLbtLogLevelMask)],
                     LbtLogComponentStr[(level&KLbtLogComponentMask) >> 16],
                     file,
                     lineno);

    HBufC8* descbuf = HBufC8::NewL(desclength);
    if ( error == KErrNone )
        {
        descbuf->Des().Append(reinterpret_cast<const TUint8 *>(desc), desclength);
        VA_LIST list;
        VA_START(list, desc);
        ptr.AppendFormat(*descbuf,
                         &ovrflowHandler,
                         list );
        delete descbuf;
        }

#ifdef LBT_FILE_LOGGER
    _LIT(KLbtTraceDir,"epos");
    _LIT(KLbtTraceFile,"lbt.log");
    RFileLogger::Write(KLbtTraceDir(), KLbtTraceFile(), EFileLoggingModeAppend, *str);
#else
    RDebug::RawPrint(*str);
#endif
    delete str;
    }
*/

#else // if _DEBUG flag is not defined

#define LBT_TRACE(level, file, lineno, desc...) 
#define FUNC_ENTER(x)
#define STOPLOGGING
#define LOG(x)
#define LOG1(x,y)
#define ERROR(x,y)
#define LOGSTRING(x, y)

#endif // end of #ifdef _DEBUG 


#endif // LBT_LOGGER_H
