// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include   <bautils.h>
#include   <lbs/epos_cpostrace.h>

// CONSTANTS
#ifdef _DEBUG
_LIT(KPosTraceLogFile, "epostracelog.txt");
_LIT(KPosTraceLogDir, "epos"); // Relative to C:\Logs
const TInt KMaxTraceLength = 1024;
#endif


NONSHARABLE_CLASS(TDesOverflowHandler) : public TDesOverflow 
	{
private:	
	void Overflow(TDes& aDes);
	};

void TDesOverflowHandler::Overflow(TDes& aDes)
	{
	RProcess process;
	_LIT( KPrefix, "[EPos/0x%LX]: Overflow occured");
	aDes.Format( KPrefix, process.Id().Id() );
	}

// ================= MEMBER FUNCTIONS =======================

/**
* C++ default constructor.
*/
CPosTrace::CPosTrace()
    {
    }

/**
* Two-phased constructor.
*/
EXPORT_C CPosTrace* CPosTrace::NewL()
    {
    CPosTrace* self = new (ELeave) CPosTrace;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
* Two-phased constructor.
* Takes parameters for Log directory and file.
* The Log directory is relative to C:\Logs
*/
EXPORT_C CPosTrace* CPosTrace::NewL(
    const TDesC& aTraceLogDir,
    const TDesC& aTraceLogFile)
    {
    CPosTrace* self = new (ELeave) CPosTrace;
    CleanupStack::PushL(self);
    self->ConstructL(aTraceLogDir, aTraceLogFile);
    CleanupStack::Pop(self);
    return self;
    }

/**
* Destructor.
*/
EXPORT_C CPosTrace::~CPosTrace()
    {
#ifdef _DEBUG
    delete iTraceDir;
    delete iTraceFile;
#endif
    }

/**
* By default EPOC constructor is private.
*/
void CPosTrace::ConstructL()
    {
#ifdef _DEBUG
    iTraceDir = KPosTraceLogDir().AllocL();
    iTraceFile = KPosTraceLogFile().AllocL();
#endif
    }


/**
* By default EPOC constructor is private.
*/
#ifdef _DEBUG
void CPosTrace::ConstructL(
    const TDesC& aTraceLogDir,
    const TDesC& aTraceLogFile)
    {
    iTraceDir = aTraceLogDir.AllocL();
    iTraceFile = aTraceLogFile.AllocL();
    }

#else

void CPosTrace::ConstructL(
    const TDesC& /*aTraceLogDir*/,
    const TDesC& /*aTraceLogFile*/)
    {
    }

#endif



#ifdef _DEBUG
/**
* Specify a trace
* @param[in] aDescription Description of the trace
* @param[in] aFilename may NOT specify __FILE__
* @param[in] aLineNumber may specify __LINE__
*/
EXPORT_C void CPosTrace::TraceL(
    const TDesC& aDescription,
    const TDesC& aFilename,
    const TInt aLineNumber)
    {
    _LIT(KPosTraceLogFormatNormal , "[EPos/0x%LX]: %S (%d): %S");
    const TInt KPosTraceLogFormatNormalExtraChars = 40;

    RProcess process;

    HBufC* buf = HBufC::NewL(
        aDescription.Length() +
        aFilename.Length() +
        KPosTraceLogFormatNormalExtraChars);

    buf->Des().Format(KPosTraceLogFormatNormal,
        process.Id().Id(),
        &aFilename,
        aLineNumber,
        &aDescription);
    
    RFileLogger::Write( *iTraceDir, *iTraceFile, EFileLoggingModeAppend, *buf);
    RDebug::RawPrint(*buf);
    
    delete buf;
    }

/**
* Static trace
* @param[in] aTraceLogDir Trace log folder under c:\logs
* @param[in] aTraceLogFile Trace log file under aTraceLogDir
* @param[in] aFmt Format of the trace string followed by the 
*                 list of parameters.
*/
EXPORT_C void CPosTrace::Log( 
    const TDesC& aTraceLogDir,
    const TDesC& aTraceLogFile,
    TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    RProcess process;
    HBufC* buf = HBufC::New( KMaxTraceLength );
    if ( buf )
    	{
    	TPtr ptr( buf->Des() );
        _LIT( KPrefix, "[EPos/0x%LX]: ");
        ptr.Format( KPrefix, process.Id().Id());
        
        TDesOverflowHandler overflowHandler; 
	    ptr.AppendFormatList( aFmt, list, &overflowHandler );
	
	    RDebug::RawPrint( ptr );
	    RFileLogger::Write( aTraceLogDir, aTraceLogFile, EFileLoggingModeAppend, ptr );

	    delete buf;
    	}
    }

#else

EXPORT_C void CPosTrace::TraceL(
    const TDesC& /*aDescription*/,
    const TDesC& /*aFilename*/,
    const TInt /*aLineNumber*/)
    {
    }

EXPORT_C void CPosTrace::Log( 
    const TDesC& /*aTraceLogDir*/,
    const TDesC& /*aTraceLogFile*/,
    TRefByValue<const TDesC> /*aFmt*/, ... )
    {
    }

#endif

//  End of File
