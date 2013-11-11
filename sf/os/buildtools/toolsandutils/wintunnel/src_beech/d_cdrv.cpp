// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Modified serial pdd that supports IP tunneling data from the emulator to  socket.
// Beech specific version.
// 
//

/**
 @file
*/

#if defined(_UNICODE)
#if !defined(UNICODE)
#define UNICODE
#endif
#endif
#define _WIN32_WINDOWS 0x0410 // Need NT4 / 98 or higher for the timer
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#include <winsock2.h>
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union
#include <k32std.h>
#include <e32wins.h>
#include "uansi.h"
#include "d_comm.h"

//#include "iostream.h"

// Dumps everything written to the serial port to temp files.
//#define _LOG_OUTPUT


//----------------------------------------------------------------
//  logging macros. Just define WINTUN_PDD_LOGGING to enable debug trace
//  require MSVCRTD.LIB
#include <crtdbg.h>

//#define WINTUN_PDD_LOGGING 

#ifdef WINTUN_PDD_LOGGING 
    
    #pragma warning( disable : 4127 ) 

    #define _Log(a)         _RPT0(_CRT_WARN,a)
    #define _Log2(a,b)      _RPT1(_CRT_WARN,a,b)
    #define _Log3(a,b,c)    _RPT2(_CRT_WARN,a,b,c)
    #define _Log4(a,b,c,d)  _RPT3(_CRT_WARN,a,b,c,d)

#else
    #define _Log(a)       
    #define _Log2(a,b)    
    #define _Log3(a,b,c)  
    #define _Log4(a,b,c,d)

#endif

const TInt KReadBufSize = 1024;
const TInt KWriteBufSize = 16;
//const TInt KWinNTReadBufSize = 1024;
const TInt KWinNTReadBufSize = 1500;
const TInt KWinNTWriteBufSize = KWriteBufSize;
static DWORD dummyLen=0;

enum TDCommWinsFault 
	{
	EWindowsUnexpectedError,
	EUnknownCommand,
	EBadIOLen,
	EEofOnSerial,
	EWriteEarlyCompletion,
	ELineErrorNotReported,
	};

class DDriverComm : public DPhysicalDevice
	{
public:
	DDriverComm();
	virtual TInt Install();
	virtual TInt Remove();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual CBase *CreateL(TInt aUnit,const TDesC *anInfo,const TVersion &aVer);
	};

class DCommWins : public DComm
	{
public:
	enum TDriverCommand {ESetBreak,EClearBreak,ETransmit,
						EGetSignals,ESetSignals,EConfigure,
						EStop,EStopNoDrain,EStart,EDie,EInvalidCommand};
public:
	DCommWins();
	~DCommWins();
	virtual TInt Start();
	virtual void Stop(TStopMode aMode);
	virtual void Break(TBool aState);
	virtual void EnableTransmit();
	virtual TUint Signals() const;
	virtual void SetSignals(TUint aSetMask,TUint aClearMask);
	virtual TInt Validate(const TCommConfigV01 &aConfig) const;
	virtual void Configure(TCommConfigV01 &aConfig);
	virtual void DoConfigure();
	virtual void CheckConfig(TCommConfigV01& aConfig);
	virtual void Caps(TDes8 &aCaps) const;
	virtual void EnableInterrupts();
	virtual void DisableInterrupts();
	TInt CompleteSlowOpen(DThread *aThread,TRequestStatus *aReqStat);
	void EnterCritical();
	void LeaveCritical();
	TInt DoCreate(TInt aUnit,const TDesC *anInfo);
	void WaitForEvent();
	void DriverCommand(TDriverCommand aCommand);
	void DoTransmit(TInt aByteCount);
	void DoReceive(TInt aByteCount);
	void RunCommThread(TDriverCommand aCommand);
	inline void SignalDriverThread();
	TInt DoTransmitIsr(TInt anOffset);
	void DoReceiveIsr(TUint aCharAndMask);
	void DoStateIsr();
	
    void SetTimer(const TUint aTimeOut); 
    void CancelTimer(); 
	
	
private:
	TBool iWritePending;
	TBool iReadPending;

    TBool iStopping;
	// TBool iReading;
	TBool iRunning;
	TDriverCommand iCommand;
	TCommConfigV01 *iConfig;
	TUint iSignals;
	TUint8 iInBuf[KReadBufSize];
	DWORD iNumInChars;
	TUint8 iOutBuf[KWriteBufSize];
	DWORD iNumOutChars;
	HANDLE iThread;
	HANDLE iCommThreadSem;
	HANDLE iDriverThreadSem;
	HANDLE iCommPort;
    HANDLE iWaitableTimer; 
    LARGE_INTEGER iTimeOut; 
	DWORD iThreadID;
	DWORD iSignalStatus;
	OVERLAPPED iReadOverLapped;
	OVERLAPPED iWriteOverLapped;
	OVERLAPPED iSignalOverLapped;
#if defined (_DEBUG)
	TInt iUnit;
	HANDLE iLogFile;
#endif
#if defined (_LOG_OUTPUT)
	HANDLE iWritesHandle;
	HANDLE iReadsHandle;
#endif
	CRITICAL_SECTION iCriticalSection;
	TInt iIntRefCount;
	TBool iSkipTransmitIsrNextTime;

	SOCKET iSocket;
	

    /** @return ETrue if we use socket instead of a real COM port */
    TBool inline UseSocket(void) const
        {return  (iSocket != INVALID_SOCKET) && (iSocket != NULL); }
    
    TInt QueSocketRead();
    
    TBool GetComPortMapping(LPCSTR aFileName, TInt aPortNum, TDes8& aPortMapStr);

	};

void Panic(TDCommWinsFault aFault)
//
// Panic the driver 
//
	{

	Plat::Panic(_L("DCommWins"),aFault);
	}


TInt MapWinError(TInt aErrCode)
{
	switch (aErrCode)
	{
	case ERROR_INVALID_USER_BUFFER:
	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_INSUFFICIENT_BUFFER:
		return(KErrNoMemory);
	case ERROR_ACCESS_DENIED:
		return(KErrAccessDenied);
	case ERROR_NOT_SUPPORTED:
		return(KErrNotSupported);
	default:
		return(KErrGeneral);
	} 
}

TInt MapWinError()
//
// Make an E32 error from the dodgy old thing windows tells us
//
{
	DWORD winErr=GetLastError();
    return MapWinError(winErr);
}

TUint commThread(DCommWins *comm)
//
// Comm thread entry point
//
	{

	comm->WaitForEvent();
	return 0;
	}

//VOID WINAPI ReadComplete(DWORD anErr,DWORD numBytes,LPOVERLAPPED anOverLapped)
//
// Windows read completion routine
//
//	{
//
//	if (numBytes>KReadBufSize)
//		Panic(EBadIOLen); 
//	if (anErr==ERROR_HANDLE_EOF)
//		Panic(EEofOnSerial);
//	((DCommWins *)(anOverLapped->hEvent))->DoReceive(numBytes);
//	}

VOID WINAPI WriteCompletion(DCommWins *aDrv, DWORD anErr,DWORD numBytes)
//
// Windows read completion routine
//
	{

	if (numBytes>KWriteBufSize+1)	// May have written an Xon
		Panic(EBadIOLen); 
	if (anErr==ERROR_HANDLE_EOF)
		Panic(EEofOnSerial);
	aDrv->DoTransmit(numBytes);
	}

BOOL WINAPI EscapeCommFunctionP(HANDLE hFile,DWORD dwFunc, BOOL bUseSocket)
//
// Protected Set/Clear modem control signals: protects against this operation being cancelled by another thread
//
    {
	if(bUseSocket)
        return TRUE;

    DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=EscapeCommFunction(hFile,dwFunc))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI GetCommModemStatusP(HANDLE hFile,LPDWORD lpModemStat, BOOL bUseSocket)
//
// Protected read modem control signals: protects against this operation being cancelled by another thread
//
	{
	if(bUseSocket)
        return TRUE;

	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=GetCommModemStatus(hFile,lpModemStat))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI GetCommStateP(HANDLE hFile,LPDCB lpDCB, BOOL bUseSocket)
//
// Retrieves the current control settings for a specified communications device: protected against
// this operation being cancelled by another thread
//
	{
	if(bUseSocket)
        return TRUE;

	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=GetCommState(hFile,lpDCB))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI SetCommStateP(HANDLE hFile,LPDCB lpDCB, BOOL bUseSocket)
//
// Configures a communications device according to the specifications in a device-control block: protected against
// this operation being cancelled by another thread
//
	{
	if(bUseSocket)
        return TRUE;

	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=SetCommState(hFile,lpDCB))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI SetCommMaskP(HANDLE hFile,DWORD dwEvtMask, BOOL bUseSocket)
//
// Specifies a set of events to be monitored for a communications device: protected against
// this operation being cancelled by another thread
//
	{
	if(bUseSocket)
        return TRUE;

	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=SetCommMask(hFile,dwEvtMask))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI WriteFileP(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped)
//
// Writes data to device pointed by hFile: protected against
// this operation being cancelled by another thread
//
	{
	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=WriteFile(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

BOOL WINAPI ReadFileP(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped)
//
// Writes data to device pointed by hFile: protected against
// this operation being cancelled by another thread
//
	{
	DWORD err,res,res1;
	COMSTAT s;
	BOOL result;

	result=FALSE;
	do
		{
		ClearCommError(hFile,&err,&s);
		if((res1=ReadFile(hFile,lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,lpOverlapped))==FALSE)
			res=GetLastError();
		else
			{
			result=TRUE;
			break;
			}
		}
	while((res1==FALSE) && (res==ERROR_OPERATION_ABORTED));
	return(result);
	}

DDriverComm::DDriverComm()
//
// Constructor
//
	{

#if defined (__COM_ONE_ONLY__)
	iUnitsMask=0x1; // Support units 0
#elif defined (__COM_TWO_ONLY__)
	iUnitsMask=0x2; // Support units 1
#else
	iUnitsMask=0x3ff; // Support units 0 to 9
#endif

	iVersion=TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber);
	}

TInt DDriverComm::Install()
//
// Install the device driver.
//
	{
#if defined (__COM_ONE_ONLY__)
	TPtrC buf=_L("Comm.Wins1");
	return(SetName(&buf));
#elif defined (__COM_TWO_ONLY__)
	TPtrC buf=_L("Comm.Wins2");
	return(SetName(&buf));
#else
	TPtrC buf=_L("Comm.Wins");
	return(SetName(&buf));
#endif
	}

TInt DDriverComm::Remove()
//
// Remove the device driver.
//
	{
	return(KErrNone);
	}

void GetWinsCommsCaps(TDes8 &aCaps)
	{

	TCommCaps2 capsBuf;
	TCommCapsV02 &c=capsBuf();
//
// All rates except 50,2000, 3600 and special
//
	c.iRate=KCapsBps75|KCapsBps110|KCapsBps134|KCapsBps150|KCapsBps300|KCapsBps600|KCapsBps1200|KCapsBps1800|KCapsBps2400|KCapsBps4800|KCapsBps7200|KCapsBps9600|KCapsBps19200|KCapsBps38400|KCapsBps57600|KCapsBps115200;

	c.iDataBits=0xf; // All data sizes
	c.iStopBits=0x3; // 1 and 2 stop bits
	c.iParity=0x7; // None, Even and Odd
	c.iHandshake=0x1ff; // All handshakes
	c.iSignals=0x3f; // All signals
	c.iSIR=0;//No Ir
	c.iNotificationCaps=KNotifySignalsChangeSupported|KNotifyDataAvailableSupported;
	c.iRoleCaps=0;
	c.iFlowControlCaps=0;
	aCaps.FillZ(aCaps.MaxLength());
	aCaps=capsBuf.Left(Min(capsBuf.Length(),aCaps.MaxLength()));
	}

void DDriverComm::GetCaps(TDes8 &aDes) const
//
// Return the drivers capabilities.
//
	{

	GetWinsCommsCaps(aDes);
	}

CBase *DDriverComm::CreateL(TInt aUnit,const TDesC *anInfo,const TVersion &aVer)
//
// Create a channel on the device.
//
	{

	if (!User::QueryVersionSupported(iVersion,aVer))
		User::Leave(KErrNotSupported);
	DCommWins *pD=new(ELeave) DCommWins;
	TInt ret=pD->DoCreate(aUnit,anInfo);
	if (ret!=KErrNone)
		{
		delete pD;
		User::Leave(ret);
		}
	return(pD);
	}



void DCommWins::DoTransmit(TInt aByteCount)
//
// Called from write completion routine
//
	{

	if(aByteCount==0)
		{
		DWORD err,res;
		COMSTAT s;
		if (!UseSocket() && ClearCommError(iCommPort,&err,&s)==FALSE)
			res=GetLastError();
		}
	__ASSERT_ALWAYS(iNumOutChars-aByteCount<=KWriteBufSize,Panic(EWriteEarlyCompletion));
#if defined (_LOG_WRITES)
		TBuf<0x40> buf;
		DWORD dummy;
		buf.Format(_L("write complete %d\r\n"),aByteCount);
		WriteFile(iLogFile,buf.PtrZ(),buf.Length(),&dummy,NULL);
#endif
//
	iNumOutChars-=aByteCount;
	if (iCommand==EStop)
		return;
	if (iSkipTransmitIsrNextTime)
		{
		iSkipTransmitIsrNextTime=EFalse;
		}
	else
		{
		while (iNumOutChars<KWriteBufSize)
			{
			iSkipTransmitIsrNextTime=!DoTransmitIsr(iNumOutChars);
			if (iSkipTransmitIsrNextTime)
				break;
			iNumOutChars++;
			}
		}
	if(iNumOutChars>0)
		{
//		WriteFileEx(iCommPort,iOutBuf,iNumOutChars,&iWriteOverLapped,WriteComplete);
		WriteFile(iCommPort,iOutBuf,iNumOutChars, &dummyLen, &iWriteOverLapped);
		iWritePending=ETrue;

#if defined (_LOG_OUTPUT)
		DWORD dummy;
		WriteFile(iWritesHandle,iOutBuf,iNumOutChars,&dummy,NULL);
#endif
#if defined (_LOG_WRITES)
		buf.Format(_L("Writing %d\r\n"),iNumOutChars);
		WriteFile(iLogFile,buf.PtrZ(),buf.Length(),&dummy,NULL);
#endif

		}
	else
		iWritePending=EFalse;
	}

/*
For reference only:
Returned from 2nd param to ClearCommError (err below)
#define CE_RXOVER           0x0001  // Receive Queue overflow
#define CE_OVERRUN          0x0002  // Receive Overrun Error
#define CE_RXPARITY         0x0004  // Receive Parity Error
#define CE_FRAME            0x0008  // Receive Framing error
#define CE_BREAK            0x0010  // Break Detected
#define CE_TXFULL           0x0100  // TX Queue is full
#define CE_PTO              0x0200  // LPTx Timeout
#define CE_IOE              0x0400  // LPTx I/O Error
#define CE_DNS              0x0800  // LPTx Device not selected
#define CE_OOP              0x1000  // LPTx Out-Of-Paper
#define CE_MODE             0x8000  // Requested mode unsupported

#define IE_BADID            (-1)    // Invalid or unsupported id
#define IE_OPEN             (-2)    // Device Already Open
#define IE_NOPEN            (-3)    // Device Not Open
#define IE_MEMORY           (-4)    // Unable to allocate queues
#define IE_DEFAULT          (-5)    // Error in default parameters
#define IE_HARDWARE         (-10)   // Hardware Not Present
#define IE_BYTESIZE         (-11)   // Illegal Byte Size
#define IE_BAUDRATE         (-12)   // Unsupported BaudRate
*/

//void DCommWins::DoReceive(TInt aByteCount)
//
// Called from read completion routine 
//
//	{
//	if(aByteCount==0)
//		{
//		DWORD err=0,res;
//		COMSTAT s;
//		if (iCommPort && ClearCommError(iCommPort,&err,&s)==FALSE)
//			res=GetLastError();
//		if(err)
//			{
//			Panic(ELineErrorNotReported);
//			}
//		}
//	TInt i=0;
//	
//	while(i<aByteCount)
//		{
//		DoReceiveIsr(iInBuf[i]);
//		i++;
//		}
//#if defined (_LOG_OUTPUT)
//		DWORD dummy;
//		WriteFile(iReadsHandle,iInBuf,aByteCount,&dummy,NULL);
//#endif 
//	if(iStopping)
//		{
//		iStopping=EFalse;
//		return;
//		}
//
//	if (!ReadFileEx(iCommPort,iInBuf,KReadBufSize,&iReadOverLapped,ReadComplete))
//		{
//		TInt r=GetLastError();
//		Panic(EWindowsUnexpectedError);
//		}
//	}

void DCommWins::RunCommThread(TDriverCommand aCommand)
//
// Wake up the comms thread
//
	{
    _Log2("# DCommWins::RunCommThread, command=%d\n", aCommand);

	__ASSERT_DEBUG(aCommand!=EInvalidCommand,Panic(EUnknownCommand));
	iCommand=aCommand;
//
// Are we about to go re-entrant?
//
	if(GetCurrentThreadId()==iThreadID)
		{
		DriverCommand(aCommand);
		WaitForSingleObject(iDriverThreadSem,INFINITE);
		}
	else
		{
		Sleep(0); // Possible deadlock solution - see MSDN Knowledge Base Article Q173260
		
		if (ReleaseSemaphore(iCommThreadSem,1,NULL)==FALSE)
			{
			DWORD ret=GetLastError();
			ret=ret;
			Panic(EWindowsUnexpectedError);
			}
		WaitForSingleObject(iDriverThreadSem,INFINITE);
		}
	}

inline void DCommWins::SignalDriverThread()
//
// Wake up the comms thread
//
	{
	_Log("# DCommWins::SignalDriverThread(), release iDriverThreadSem\n");
	Sleep(0); // Possible deadlock solution - see MSDN Knowledge Base Article Q173260
	if (ReleaseSemaphore(iDriverThreadSem,1,NULL)==FALSE)
		{
		DWORD ret=GetLastError();
		ret=ret;
		Panic(EWindowsUnexpectedError);
		}
	}



//
#pragma warning( disable : 4705 )	// statement has no effect
DCommWins::DCommWins()
//
// Constructor
//
	{
//	iRunning=EFalse;
//	iCommand=ESetBreak;
//	iSignals=0;
//	iInBuf=0;
//	iOutBuf=0;
//	iThread=NULL;
//	iCommThreadSem=NULL;
//	iCommPort=NULL;
//	iThreadID=0;
	iWritePending = EFalse;
	iReadPending  = EFalse;

	Mem::FillZ(&iReadOverLapped,sizeof(OVERLAPPED));
	Mem::FillZ(&iWriteOverLapped,sizeof(OVERLAPPED));
	__DECLARE_NAME(_S("DCommWins"));
#ifdef _DEBUG_DEVCOMM
	iTxIntCount = 0;
	iRxIntCount = 0;
	iTxErrCount = 0;
	iRxErrCount = 0;
#endif
	iIntRefCount = 0;
	iSocket = INVALID_SOCKET;

    _Log("# ------------------------------------------------\n");
	_Log("# DCommWins::DCommWins()\n");
    
	}
#pragma warning( default : 4705 )

/**
*   Read serial port mapping from ini file (dComm.ini usually)
*
*   @param  aFileName   ini full file path descriptor
*   @param  aPortNum    serial port number (e.g. 0 for COMM::0)
*   @param  aPortMapStr descriptor for returned port mapping (e.g. "10.35.2.53:110)
*   
*   @return ETrue if the parameter found and everything is OK.
*/
TBool DCommWins::GetComPortMapping(LPCSTR aFileName, TInt aPortNum, TDes8& aPortMapStr)
{
    TBool   bRes = ETrue;
	TInt    fileSize;
    LPVOID	pFileData=NULL;

    //--  open dcomm.ini file
    HANDLE hFile=CreateFileA(aFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
	{//-- can't open file, maybe it is just not present
		bRes = EFalse;
		goto CleanUp;
	}

    //-- get file size
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || !fileSize)
    {//-- something is wrong with file size
		bRes = EFalse;
		goto CleanUp;
    }
    
    //-- allocate data for buffer 
	// fileSize+1 to ensure zero-termination of file, since Windows98 might use
	// a dirty page of memory.  VirtualAlloc initializes memory to zero
    pFileData = VirtualAlloc(NULL, fileSize+1, MEM_COMMIT, PAGE_READWRITE);
    if(! pFileData)
    {//-- something is wrong with memory allocation
		bRes = EFalse;
		goto CleanUp;
    }

    //-- read whole file into buffer
    DWORD bytesRead;    

    if (! ReadFile(hFile, pFileData, fileSize, &bytesRead, NULL))
        bRes = EFalse;
    else
    {//-- parse file line by line looking for the appropriate port 
	    LPSTR linePtr=(LPSTR)pFileData;
		_LIT8(KCommToken, "COMM::");
        bRes = EFalse;


        while(linePtr && *linePtr)
		{
			//-- skip spaces at the beginning of the current line
            while (*linePtr && (*linePtr == '\r' || *linePtr == '\n' || *linePtr == ' ' || *linePtr =='\t'))
			{
				++linePtr;
				--bytesRead;
			}

            //-- find the end of the line
			TPtrC8 line((unsigned char*)linePtr, bytesRead);
            TInt len;
			for (len = 0; len < line.Length() && (line[len] != '\r' && line[len] != '\n'); ++len)
				;

			line.Set(line.Left(len));
			bytesRead -= line.Length();
			linePtr += line.Length();

            //-- parse the line
			TLex8   lex(line);
			TPtrC8  commname (lex.NextToken());
			if(commname.Length() <= KCommToken().Size())
				continue; // not enough chars
			if(commname[0] == '#')
				continue; // comment line

			if (commname.Left(KCommToken().Size()).CompareF(KCommToken) != 0)
				continue; // unreconised option. Should log?!?
    
            //-- extract comm port number
    		TLex8 numLex(commname.Mid(KCommToken().Size()));
			TInt commNum;
			if(numLex.Val(commNum) != KErrNone || commNum != aPortNum)
				continue;

			//-- extract comm port mapping substring
            lex.SkipSpace();
            aPortMapStr.Copy(lex.Remainder());
            aPortMapStr.PtrZ();
            bRes = ETrue;

            break;
        }//while(linePtr && *linePtr)
    }

    
  CleanUp:

    CloseHandle(hFile);
    VirtualFree(pFileData, 0, MEM_RELEASE);

    return bRes;
}


TInt DCommWins::DoCreate(TInt aUnit,const TDesC * /*anInfo*/)
//
// Create the comms driver.
//
	{
    _Log("# DCommWins::DoCreate()\n");

	InitializeCriticalSection(&iCriticalSection);

#if defined (__COM_ONE_ONLY__)
	if (aUnit!=0)
		return KErrNotSupported;
#elif defined (__COM_TWO_ONLY__)
	if (aUnit!=1)
		return KErrNotSupported;
#endif

    //-- get full path for the DComm.ini configuration file
	TBuf8<MAX_PATH> iniFileName;
    iniFileName.Copy(EmulatorDataPath());
	iniFileName.Append(_L8("DComm.ini"));
    LPCSTR  pszIniFle = (LPCSTR)iniFileName.PtrZ();

    //-- get com port mapping string
	TBuf8<MAX_PATH> strPortMapping;
    TBool bFound = GetComPortMapping(pszIniFle, aUnit, strPortMapping);

    if(bFound)
    {//-- comm port mapping string is found in cofiguration file, we are dealing with sockets now
    
        //-- try to locate port number after colon
        TInt nColonPos = strPortMapping.Locate(':');
        if(nColonPos != KErrNotFound)
        {// Found a ':' ... must be a TCP name & port no.
            TPtrC8 hostAddr(strPortMapping.Left(nColonPos));

            ((char*)strPortMapping.Ptr())[nColonPos] = 0; // replace : with a null term

		    TLex8 portLex (strPortMapping.Mid(nColonPos + 1));
		    TInt port = 0;
		    portLex.Val(port);


            //-- initialize socket

		    WORD    wVersionRequested;
		    WSADATA wsaData;
		    int err; 
		    wVersionRequested = MAKEWORD(2, 0); 
		    err = WSAStartup(wVersionRequested, &wsaData);
		    if (err != 0)
                return MapWinError(WSAGetLastError());

		    iSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		    if(iSocket == INVALID_SOCKET)
                return MapWinError(WSAGetLastError());
            
            _Log2("# DCommWins: created socket id: %d\n", iSocket );
		    
            sockaddr_in sin;

		    char* devname = (char*)(strPortMapping.Ptr());
		    u_long nRemoteAddr = inet_addr(devname);
		    
            if (nRemoteAddr != INADDR_NONE)
			{
			    sin.sin_addr.s_addr = nRemoteAddr;
			}
		    else
			{
			    struct hostent FAR * hostaddr = gethostbyname (devname);
			    if(hostaddr == NULL)
				    return MapWinError(WSAGetLastError());
			    
                sin.sin_addr.s_addr = *((u_long*)hostaddr->h_addr_list[0]);
			}

		    sin.sin_family = AF_INET;
		    sin.sin_port = htons(TUint16(port));

		    err = WSAConnect(iSocket, (sockaddr*)&sin, sizeof(sin), 0, 0, 0, 0);
		    if(err)
                return MapWinError(WSAGetLastError());

            _Log2("# DCommWins: socket connected to the port: %d\n", port);

		    iCommPort = (void*)iSocket;
            
        }
    }
   
                        
    //-----------------------------------------

    if(! UseSocket())
    {//-- use PC serial port as usual
        //-- open Windows serial port
	    TBuf8<0x10> n;
	    n.Format(_L8("\\\\.\\COM%d"),aUnit+1);
	    iCommPort=CreateFileA((LPCSTR)n.PtrZ(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	    if (iCommPort==INVALID_HANDLE_VALUE)	
		    {	
	    //	Reused code from MapWinError() rather than adding an extra case 
	    //	to MapWinError() because mapping KErrNotSupported to the returned
	    //	FILE_NOT_FOUND is non-intuitive and special to this case only
		    DWORD winErr=GetLastError();
		    switch (winErr)
			    {
			    case ERROR_INVALID_USER_BUFFER:
			    case ERROR_NOT_ENOUGH_MEMORY:
			    case ERROR_INSUFFICIENT_BUFFER:
				    return(KErrNoMemory);
			    case ERROR_ACCESS_DENIED:
				    return(KErrAccessDenied);
			    case ERROR_FILE_NOT_FOUND:		//	Reflects value returned by
				    return(KErrNotSupported);	//	corresponding MARM Pdd  
			    case ERROR_NOT_SUPPORTED:
				    return(KErrNotSupported);
			    default:
				    return(KErrGeneral);
			    }
		    }
	    
	    DCB dcb;
	    if (!GetCommStateP(iCommPort,&dcb,UseSocket()))
		    return MapWinError();

	    dcb.fDtrControl=DTR_CONTROL_DISABLE;
	    dcb.fRtsControl=RTS_CONTROL_DISABLE;
	    dcb.fAbortOnError=TRUE;	// Tell me about your parity problems luvvy
	    dcb.fParity=TRUE;
	    dcb.fBinary=TRUE;
	    dcb.fOutxCtsFlow=FALSE;
	    dcb.fOutxDsrFlow=FALSE;  
	    dcb.fDsrSensitivity=FALSE;
	    dcb.fTXContinueOnXoff=TRUE;
	    dcb.fOutX=FALSE;
	    dcb.fInX=FALSE;
	    dcb.fErrorChar=FALSE;
	    dcb.fNull=FALSE;
    //
    //	dcb.ByteSize;		// number of bits/byte, 4-8
    //	dcb.Parity;			// 0-4=no,odd,even,mark,space
    //	dcb.StopBits;		// 0,1,2 = 1, 1.5, 2
    //	dcb.XonChar;		// Tx and Rx XON character
    //	dcb.XoffChar;		// Tx and Rx XOFF character
    //	dcb.ErrorChar;		// error replacement character
    //	dcb.EofChar;		// end of input character
    //	dcb.EvtChar;
    //
	    if (!SetCommStateP(iCommPort,&dcb, UseSocket()))
		    return MapWinError();

	    EscapeCommFunctionP(iCommPort,0,UseSocket());

    //
    // Could add EV_RXCHAR|EV_RXFLAG|EV_TXEMPTY to the following
    // mask too and get all chars through the call to WaitForAnyObject
    // rather then the completion routine followed by a manual read
    //
	    if (!SetCommMaskP(iCommPort,EV_CTS|EV_ERR|EV_DSR|EV_RLSD|EV_RXCHAR,UseSocket()))
		    return MapWinError();

	    if(!SetupComm(iCommPort,KWinNTReadBufSize,KWinNTWriteBufSize))
		    return MapWinError();

    //
    // The serial port seems to open with the error condition set
    //

	    DWORD err,res;
	    COMSTAT s;
	    if (ClearCommError(iCommPort,&err,&s)==FALSE)
		    res=GetLastError();
    }//if(! UseSocket())

	if ((iCommThreadSem=CreateSemaphoreA(NULL,0,0x7fffffff,NULL))==NULL)
	    return MapWinError();
    
    if ((iDriverThreadSem=CreateSemaphoreA(NULL,0,0x7fffffff,NULL))==NULL)
	    return MapWinError();

    if ((iWaitableTimer=CreateWaitableTimer(NULL, FALSE, NULL))==NULL)
        return MapWinError();

	if ((iThread=CreateThread(NULL,0x4000,(LPTHREAD_START_ROUTINE)commThread,(void *)this,CREATE_SUSPENDED,&iThreadID))==NULL)
	    return MapWinError();

    

#if defined (_LOG_WRITES)
	TBuf8<0x40> logName;
	logName.Format(_L8("C:\\tmp\\log%d.tmp"),iUnit);
	DWORD fileErr;
	if((iLogFile=CreateFileA((char *)logName.PtrZ(),GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
		fileErr=GetLastError();
#endif
#if defined (_LOG_OUTPUT)
	TBuf8<0x40> fileName;
	DWORD fileErr;
	fileName.Format(_L8("C:\\tmp\\Writes%d.tmp"),iUnit);
	if((iWritesHandle=CreateFileA((char *)fileName.PtrZ(),GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
		fileErr=GetLastError();
	fileName.Format(_L8("C:\\tmp\\Reads%d.tmp"),iUnit);
	if((iReadsHandle=CreateFileA((char *)fileName.PtrZ(),GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
		fileErr=GetLastError();
#endif
	
	SetThreadPriority(iThread,THREAD_PRIORITY_HIGHEST);
	return(KErrNone);
	}

DCommWins::~DCommWins()
//
// Destruct
//
	{
	if (iThread)
		{
		if (! iRunning)
			{
			__ASSERT_ALWAYS(
				ResumeThread(iThread) != 0xffffffff,
				User::Panic(_L("Wins comm error"), 0));
			}

		iRunning=ETrue;
		RunCommThread(EDie);
		}

	if(UseSocket())
	{
	    closesocket(iSocket);
	    WSACleanup();
		iCommPort = (HANDLE) NULL;
		iSocket   = NULL;
	}

	if (iCommPort)
		CloseHandle(iCommPort);

	if (iDriverThreadSem)
		CloseHandle(iDriverThreadSem);

	if (iCommThreadSem)
		CloseHandle(iCommThreadSem);

    if (iWaitableTimer)
        CloseHandle(iWaitableTimer);

	if (iReadOverLapped.hEvent)
		CloseHandle(iReadOverLapped.hEvent);

	if (iWriteOverLapped.hEvent)
		CloseHandle(iWriteOverLapped.hEvent);

	if (iSignalOverLapped.hEvent)
		CloseHandle(iSignalOverLapped.hEvent);

	if (iThread)
		CloseHandle(iThread);

	DeleteCriticalSection(&iCriticalSection);
	}

TInt DCommWins::Start()
//
// Start receiving characters
//
	{

	__ASSERT_ALWAYS(ResumeThread(iThread)!=0xffffffff,User::Panic(_L("Wins comm error"),0));
//	if (ResumeThread(iThread)==0xffffffff)
//		return(MapWinError());
	iRunning=ETrue;
	RunCommThread(EStart);
	return(KErrNone);
	}

void DCommWins::Stop(TStopMode aMode)
//
// Stop receiving characters
//
	{

	iSkipTransmitIsrNextTime=EFalse;
	RunCommThread((aMode==EStopEmergency) ? EStopNoDrain : EStop);
	SuspendThread(iThread);
	iRunning=EFalse;
	iNumInChars=iNumOutChars=0;
	}

void DCommWins::Break(TBool aState)
//
// Assert a break signal
//
	{

	if (aState)
		RunCommThread(ESetBreak);
	else
		RunCommThread(EClearBreak);
	}

void DCommWins::EnableTransmit()
//
// Start transmitting characters
//
	{
	RunCommThread(ETransmit);
	}


TUint DCommWins::Signals() const
//
// Return the current signals state
//
	{
	if(UseSocket()) 
		{
		return MS_CTS_ON|MS_DSR_ON|KSignalCTS|KSignalDSR;
		}
	
    ULONG signals=0;
	GetCommModemStatusP(iCommPort,&signals, UseSocket());
	TUint status=0;
	if (signals&MS_CTS_ON)
		status|=KSignalCTS;
	if (signals&MS_DSR_ON)
		status|=KSignalDSR;
	if (signals&MS_RING_ON)
		status|=KSignalRNG;
	if (signals&MS_RLSD_ON)
		status|=KSignalDCD;
	CONST_CAST(DCommWins*,this)->iSkipTransmitIsrNextTime=EFalse;
	return(status|iSignals);
	}

void DCommWins::SetSignals(TUint aSetMask,TUint aClearMask)
//
// Set the state of the output signals
//
	{

    if (aSetMask&KSignalRTS)
		{
		iSignals|=KSignalRTS;
		EscapeCommFunctionP(iCommPort,SETRTS, UseSocket());
		}
	if (aSetMask&KSignalDTR)
		{
		iSignals|=KSignalDTR;
		EscapeCommFunctionP(iCommPort,SETDTR, UseSocket());
		}
	if (aClearMask&KSignalRTS)
		{
		iSignals&=(~KSignalRTS);
		EscapeCommFunctionP(iCommPort,CLRRTS, UseSocket());
		}
	if (aClearMask&KSignalDTR)
		{
		iSignals&=(~KSignalDTR);
		EscapeCommFunctionP(iCommPort,CLRDTR, UseSocket());
		}
        
        _Log2("#~DCommWins::SetSignals, iSignals has become=0x%x\n", iSignals);
	}

void DCommWins::CheckConfig(TCommConfigV01& /*aConfig*/)
	{
	// Do nothing
	}


TInt DCommWins::CompleteSlowOpen(DThread *aThread,TRequestStatus *aReqStat)
	{
	// Should never be called
	aThread->RequestComplete(aReqStat,KErrNone);
	return(KErrNone);
	}

TInt DCommWins::Validate(const TCommConfigV01 &aConfig) const
//
// Confirm that aConfig is a valid configuration.
//
	{

	if (aConfig.iSIREnable==ESIREnable)
		return KErrNotSupported;

	if (aConfig.iRate&KCapsBpsSpecial)
		return(KErrNotSupported);
	switch (aConfig.iRate)
		{
	case EBps50:
	case EBps2000:
	case EBps3600:
		return(KErrNotSupported);
		};
	return(KErrNone);
	}

void DCommWins::Configure(TCommConfigV01 &aConfig)
//
// Ask comm thread to set up the serial port
//
	{

	iConfig=&aConfig;
	if (iRunning)
		{
		RunCommThread(EConfigure);
		}
	else
		{
//		iCommand=EConfigure;
		DoConfigure();
		}
	}

void DCommWins::DoConfigure()
//
// Set up the serial port
//
	{



	DCB dcb;
	if (!GetCommStateP(iCommPort,&dcb, UseSocket()))
		return;

    if(iRunning)
        {
        SetCommMaskP(iCommPort,EV_CTS|EV_ERR|EV_DSR|EV_RLSD|EV_RXCHAR, UseSocket());
        WaitForSingleObject(iSignalOverLapped.hEvent, INFINITE);
        }

//	
// Need only worry about the framing and speed.
//				   
	switch (iConfig->iRate)
		{
	case EBps75:
		dcb.BaudRate=75;
		break;
	case EBps110:
		dcb.BaudRate=110;
		break;
	case EBps134:
		dcb.BaudRate=134;
		break;
	case EBps150:
		dcb.BaudRate=150;
		break;
	case EBps300:
		dcb.BaudRate=300;
		break;
	case EBps600:
		dcb.BaudRate=600;
		break;
	case EBps1200:
		dcb.BaudRate=1200;
		break;
	case EBps1800:
		dcb.BaudRate=1800;
		break;
	case EBps2400:
		dcb.BaudRate=2400;
		break;
	case EBps4800:
		dcb.BaudRate=4800;
		break;
	case EBps7200:
		dcb.BaudRate=7200;
		break;
	case EBps9600:
		dcb.BaudRate=9600;
		break;
	case EBps19200:
		dcb.BaudRate=19200;
		break;
	case EBps38400:
		dcb.BaudRate=38400;
		break;
	case EBps57600:
		dcb.BaudRate=57600;
		break;
	case EBps115200:
		dcb.BaudRate=115200;
		break;
		}
   
   	switch(iConfig->iDataBits)
		{
	case EData5:
		dcb.ByteSize=5;
		break;
	case EData6:
		dcb.ByteSize=6;
		break;
	case EData7:
		dcb.ByteSize=7;
		break;
	case EData8:
		dcb.ByteSize=8;
		break;
		}

	switch(iConfig->iStopBits)
		{
	case EStop1:
		dcb.StopBits=ONESTOPBIT;
		break;
	case EStop2:
		dcb.StopBits=TWOSTOPBITS;
		break;
		}

	switch (iConfig->iParity)
		{
	case EParityNone:
		dcb.Parity=NOPARITY;
		break;
	case EParityEven:
		dcb.Parity=EVENPARITY;
		break;
	case EParityOdd:
		dcb.Parity=ODDPARITY;
		break;
		}

	// make sure that SetCommState won't change the state of the Modem signals
	TUint prov=iSignals&KSignalRTS;
	if(prov)
		dcb.fRtsControl=RTS_CONTROL_ENABLE;
	else
		dcb.fRtsControl=RTS_CONTROL_DISABLE;

	prov=iSignals&KSignalDTR;
	if(prov)
		dcb.fDtrControl=DTR_CONTROL_ENABLE;
	else
		dcb.fDtrControl=DTR_CONTROL_DISABLE;
		
	TInt error_r=KErrNone;
	if(!SetCommStateP(iCommPort,&dcb, UseSocket()))
		error_r=GetLastError();
//	SetSignals(iSignals, ~iSignals);	// For NT 4 -> redundant (see above comments on preserving Modem control signals)
//
// Clear any error we may have caused
//
	DWORD err,res;
	COMSTAT s;
	if (ClearCommError(iCommPort,&err,&s)==FALSE)
		res=GetLastError();

    if(iRunning)
        {
        if(!WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped))
            {
            TInt i=50;
            while(--i)
                {
                if(WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped))
                    break;
                }
            }
        }
	}

void DCommWins::Caps(TDes8 &aCaps) const
//
// Return the current capabilities
//
	{

	GetWinsCommsCaps(aCaps);
	}

void DCommWins::EnableInterrupts()
//
// Wake the comms thread up
//
	{
	LeaveCritical();
//	if (ResumeThread(iThread)==0xFFFFFFFF)
//		{
//		DWORD err=GetLastError();
//		Panic(EWindowsUnexpectedError);
//		}
	iSkipTransmitIsrNextTime=EFalse;
	}

void DCommWins::DisableInterrupts()
//
// Suspend the thread (assuming we're not it...)
//
	{
	EnterCritical();
//	if(GetCurrentThreadId()!=iThreadID)
//		if (SuspendThread(iThread)==0xFFFFFFFF)
//			{
//			DWORD err=GetLastError();
//			Panic(EWindowsUnexpectedError);
//			}
	}

/**
    Associate event with our socket
*/
TInt DCommWins::QueSocketRead()
	{
	if(WSAEventSelect(iSocket, iReadOverLapped.hEvent, FD_READ))
		{// Some error?!?
		DWORD res = WSAGetLastError();
		return res;
		}
	return KErrNone;

	}


void DCommWins::WaitForEvent()
//
// Comm port thread - wait for serial port and semaphore
//
	{

	HANDLE objects[5];


	iReadOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL); 
	iWriteOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL);
	iSignalOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL);
	objects[0]=iSignalOverLapped.hEvent; // iCommPort;
	objects[1]=iCommThreadSem;
	objects[2]=iWriteOverLapped.hEvent;
	objects[3]=iReadOverLapped.hEvent;
    objects[4]=iWaitableTimer;

	FOREVER
		{
		DWORD waitTimeout = INFINITE; // Default timeout 
		if(iReadPending) // If we have a queued read event, we need to check periodically if it can be carried out.
			{
			waitTimeout = 150; 
			}
		
		// Once an event is signalled and WaitFor.. returns, it is reset by WaitFor.. just before it returns.
		// This means that once a read on the socket is signalled. we absolutely must read from it (eventually).
		// Otherwise, the read event remains reset and no further reads are signalled, even if data arrives. 
		// This means that if we cannot read immediately,we MUST queue the read, and carry it out sometimes in the future.
		DWORD ret=WaitForMultipleObjectsEx(5,objects,FALSE,waitTimeout,TRUE);
				
		// If the wait for an event has timed out, AND we have a queued read, try to carry out the read.
		// If we have not timed out, carry out the operation as usual:
		// it cannot be a read, but it may be something which enables us to read later.
		if(ret == WAIT_TIMEOUT && iReadPending)
			{			
			ret = WAIT_OBJECT_0 + 3; // Fall through to the read. 			
			}
		
		switch (ret)
			{
		case WAIT_OBJECT_0:
			ASSERT(iCommPort);
			EnterCritical();
			DWORD commErrors;
			COMSTAT cstat;
//	
// iCommPort has completed other than read/write - could be either error or line status change
//
			if ((iSignalStatus&EV_ERR) || (iSignalStatus&EV_RXCHAR))
				{ 
				ClearCommError(iCommPort,&commErrors,&cstat);
				if (cstat.cbInQue)
					{
//
// Drain the input buffer
//				
					while (cstat.cbInQue>0)
						{
                        _Log2("#~ WAIT_OBJECT_0 cstat.cbInQue=%d\n",cstat.cbInQue);//

                        TInt min = Min((TInt)cstat.cbInQue, (TInt)KReadBufSize); 
                        TInt i;
						ReadFile(iCommPort,iInBuf,min,(LPDWORD)&i,&iReadOverLapped);
						WaitForSingleObject(iReadOverLapped.hEvent,INFINITE);
						GetOverlappedResult(iCommPort, &iReadOverLapped, (LPDWORD)&i, FALSE);
                        __ASSERT_ALWAYS(i==min,Panic(EBadIOLen)); 

                        cstat.cbInQue-=min;
						i=0;
						while (i<min)
                            {

                            if(i==(min-1) && !cstat.cbInQue)
                                {
                                //
                                // Mark the final byte with the parity error
                                //


                                TInt temp=0;
					            if (commErrors&CE_RXPARITY)
						            temp|=KReceiveIsrParityError;
					            if (commErrors&CE_FRAME)
						            temp|=KReceiveIsrFrameError;
					            if (commErrors&CE_OVERRUN)
						            temp|=KReceiveIsrOverrunError;
                                DoReceiveIsr(iInBuf[i++]|temp);
                                }
                            else
							    DoReceiveIsr(iInBuf[i++]);
                            }
						}
                    

					}
				}
			if (iSignalStatus&(EV_DSR|EV_CTS|EV_RLSD))
				DoStateIsr();
			iSignalStatus=0;
			WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped);
			LeaveCritical();
			break;
		case WAIT_OBJECT_0+1:
//
// iCommThreadSemaphore has been signalled
//
			DriverCommand(iCommand);
			break;
		case WAIT_OBJECT_0+2:
			//
			//	Write completion
			//
			{
			DWORD len=iWriteOverLapped.InternalHigh;
//			GetOverlappedResult(iCommPort, &iWriteOverLapped, &len, FALSE);

			EnterCritical();
			WriteCompletion(this, 0, len);
			LeaveCritical();
			break;
			}

	    case WAIT_OBJECT_0+3:
			//
			// Read completion
			//
			{
            ASSERT(iSocket);
			EnterCritical();
			if(iConfig->iHandshake & KConfigFreeRTS || iSignals & KSignalRTS)
				{// Only read when RTS is freed or asserted

                int num = recv(iSocket, (char*)iInBuf, 16, 0); // Only read 16 bytes at a go, to give LDD plenty of chance to flow off
                
                if(num == 0 ||  (num == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) || num < 0)
					{
					DoReceiveIsr(KReceiveIsrFrameError);
					num = 0;
					}

				for(int i = 0; i<num; ++i)
					{
					DoReceiveIsr(iInBuf[i]);
					}
				// We'll automatically be re-signalled if there's more data waiting

				iSignalStatus=0;
	//			if(QueSocketRead() != KErrNone)
	//				{
	//				DoReceiveIsr(KReceiveIsrFrameError);
	//				iNumInChars = 0;
	//				}
					
				iReadPending = EFalse; // Make sure read is not queued
				}
			else // We cannot read at this time. 
                {
				// We have skipped the read, and the read event is reset at this point.
				iReadPending = ETrue; // Queue the read event
                _Log3("#~ ReadCompletion FC, iSignals=0x%x, iConfig->iHandshake=%d\n", iSignals, iConfig->iHandshake);
                }
			LeaveCritical();
			break;
			}		
      

        case WAIT_OBJECT_0+4:
            //
            //    Timer completion
            //
            {
            Comm().TurnaroundStartDfcImplementation();
            break;
            }
		
		case WAIT_IO_COMPLETION:
//
// A read or write completion routine has been queued - why would we care?
// Do nothing?
            _Log("#~ WAIT_IO_COMPLETION\n");//

		break;
		
		case WAIT_TIMEOUT: // Ignore the timeout.
			// Just in case we timed out and no read is queued. 
			// This is impossible, because we can time out only when a read is queued, 
			// so we'll try to read rather than end up here. 
			// This code is just in case there are further modifications to the event handling code.			
			break;
		
		default:
			MapWinError();
			Panic(EWindowsUnexpectedError);
			}
		}
	}

void DCommWins::DriverCommand(TDriverCommand aCommand)
//
// Do a driver command - executed when the semaphore has been signalled in the comm port thread
//
	{
	switch (aCommand)
		{
	case ESetBreak:
		if(!UseSocket()) SetCommBreak(iCommPort);
		break;
	case EClearBreak:
		if(!UseSocket()) ClearCommBreak(iCommPort);
		break;
	case ETransmit:
		// Only Transmit one character to get the driver transmit cycle going
		// otherwise we can empty the buffer and get double starts and re-write death
		if (!iWritePending)
			{
			if(DoTransmitIsr(iNumOutChars))
				iNumOutChars++;
#if defined (_LOG_OUTPUT)
			DWORD dummy;
			WriteFile(iWritesHandle,iOutBuf,iNumOutChars,&dummy,NULL);
#endif 
			if (iNumOutChars>0)
				{
				iWritePending=ETrue;
//				WriteFileEx(iCommPort,iOutBuf,iNumOutChars,&iWriteOverLapped,WriteComplete);
				WriteFile(iCommPort,iOutBuf,iNumOutChars, NULL, &iWriteOverLapped);

#if defined (_LOG_WRITES)
					TBuf<0x40> buf;
					DWORD dummy;
					buf.Format(_L("writing %d\r\n"),iNumOutChars);
					WriteFile(iLogFile,buf.PtrZ(),buf.Length(),&dummy,NULL);
#endif

				}
			}
		break;
	case EStart:

		if(UseSocket())
		{
			if(QueSocketRead() != KErrNone)
				{// Couldn't que initial read :-(
				__DEBUGGER();
				}
		}
		else
        {
            DWORD err,res;
		    COMSTAT s;
		    if (iCommPort && ClearCommError(iCommPort,&err,&s)==FALSE)
			    res=GetLastError();
		    PurgeComm(iCommPort ,PURGE_RXCLEAR);
		    iSignalStatus=0;
		    WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped);
        }

	//	if (ReadFileEx(iCommPort,iInBuf,1,&iReadOverLapped,ReadComplete)==FALSE)
	//		DWORD res=GetLastError();
		break;
	case EStop:
		if(!UseSocket() && iNumOutChars)
			{
#if defined (_LOG_OUTPUT)
			DWORD dummy;
			WriteFile(iWritesHandle,iOutBuf,iNumOutChars,&dummy,NULL);
#endif 
            // Flush last write
			if(iWritePending)
			    {
			    WaitForSingleObject(iWriteOverLapped.hEvent, INFINITE);
			    FlushFileBuffers(iCommPort);
                iWritePending=EFalse;
				}
			}
		// Fall through
	case EStopNoDrain:
		if(UseSocket())
			{
			CancelIo(iCommPort); // cancels read & write (see below)
			}
		// Cancel any pending writes
        if(iWritePending)
		{
			if(!UseSocket()) 
            {
                PurgeComm(iCommPort, PURGE_TXABORT|PURGE_TXCLEAR);
                WaitForSingleObject(iWriteOverLapped.hEvent, INFINITE);
            }
			iWritePending=EFalse;
		}   

		iStopping=ETrue;
        if(iRunning)
		{
			if(!UseSocket())
			{
				SetCommMask(iCommPort,EV_CTS|EV_ERR|EV_DSR|EV_RLSD|EV_RXCHAR);
				WaitForSingleObject(iSignalOverLapped.hEvent, INFINITE);
			}
        }
		break;
	case EDie:	
		SignalDriverThread();
		ExitThread(1);
		break;
	case EConfigure:
		DoConfigure();
		break;
	default:
		// Panic(EUnknownCommand);
		break;
		}
	iCommand=EInvalidCommand;
	SignalDriverThread();
	}

TInt DCommWins::DoTransmitIsr(TInt anOffset)
//
// Call the transmit Isr in the LDD to get the next character
//
	{

	TInt ret;
	ret=Comm().TransmitIsr();
	iOutBuf[anOffset]=(*(char *)&ret);
#ifdef _DEBUG_DEVCOMM
	iTxIntCount += (ret!=KTransmitIrqEmpty);
#endif
	return(ret!=KTransmitIrqEmpty);
	}

void DCommWins::DoReceiveIsr(TUint anCharAndMask)
//
// Call the Isr in the LDD to process this character
//
	{
#ifdef _DEBUG_DEVCOMM
	iRxIntCount++;
	if (anCharAndMask & ~0xff)
		iRxErrCount++;
#endif
	Comm().ReceiveIsr(anCharAndMask);
	}

void DCommWins::DoStateIsr()
//
// Call the Isr in the device to handle a line status change.
//
	{
	Comm().StateIsr(Signals());
	}

void DCommWins::EnterCritical()
//
// Call the Isr in the device to handle a line status change.
//
	{
	EnterCriticalSection(&iCriticalSection);
	iIntRefCount++;
	}

void DCommWins::LeaveCritical()
//
// Call the Isr in the device to handle a line status change.
//
	{
	--iIntRefCount;
	LeaveCriticalSection(&iCriticalSection);
	}
 
void DCommWins::SetTimer(const TUint aTimeOut) 
    { 
    TInt timeout = aTimeOut; 
    timeout *= 10; // to 100ns units 
    timeout = -timeout; // make relative to current time 
    iTimeOut.QuadPart = timeout; 
    SetWaitableTimer(iWaitableTimer, &iTimeOut, 0, NULL, NULL, 0); 
    } 
 
void DCommWins::CancelTimer() 
    { 
    CancelWaitableTimer(iWaitableTimer); 
    } 

EXPORT_C DPhysicalDevice *CreatePhysicalDevice()
//
// Create a new physical device driver
//
	{

	return(new DDriverComm);
	}

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//
// DLL entry point
//
	{

	return(KErrNone);
	}
