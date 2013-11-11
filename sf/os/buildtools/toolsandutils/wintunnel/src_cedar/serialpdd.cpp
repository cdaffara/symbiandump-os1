// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins/specific/serialpdd.cpp
// Modified serial pdd that supports IP tunneling data from the emulator to  socket.
// 
//

/**
 @file
*/

#include "winscomm.h"
#include <nkern/nk_priv.h>
#include <nkern/win32/nk_plat.h>
#include <emulator.h>

#include <kernel/win32/property.h>

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union

#include <windows.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

//----------------------------------------------------------------
//  logging macros. Just define WINTUN_PDD_LOGGING to enable debug trace
//  note that MSVCRTD.LIB isn't included to the serialpdd.mmp, because this **** CW does not 
//  have it. Use it from MSVC package.

#include <crtdbg.h>

//#define WINTUN_PDD_LOGGING 

#ifdef WINTUN_PDD_LOGGING 
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

//----------------------------------------------------------------


#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union

#ifdef FAULT
#undef FAULT
#endif
#define FAULT() Kern::Fault(__FILE__,__LINE__)

_LIT(KComName,"COM");

// needs ldd version..
const TInt KMinimumLddMajorVersion=1;
const TInt KMinimumLddMinorVersion=1;
const TInt KMinimumLddBuild=1;		

//used for the read and write buffers in the driver.
//large buffer reserved so any transfer to/from the client can fit into a driver buffer
const TInt KSerialBufferMaxSize = 0x800000;
const TInt KSerialBufferInitialSize = 0x10000;
const TInt KSerialBufferIncrementSize = 0x1000;	//granularity.  must be power of 2	


const DWORD KReadOneOrMoreTimeout = MAXDWORD-1;	//milliseconds


//buffer sizes passed to NT for setting its own driver buffer sizes
const TInt KDefaultWinNTReadBufSize = 1024;
const TInt KDefaultWinNTWriteBufSize = 1024;

static DWORD dummyLen = 0;

enum TDCommWinsFault 
	{
	EWindowsUnexpectedError,
	EUnknownCommand,
	EBadIOLen,
	EEofOnSerial,
	EWriteEarlyCompletion,
	ELineErrorNotReported,
	ESerialBufferTooBig,
	EReadLength,
	};


class DDriverComm : public DPhysicalDevice
	{
public:
	DDriverComm();
	virtual TInt Install();
	virtual TInt Remove();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual TInt Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	virtual TInt Validate(TInt aUnit, const TDesC8* aInfo, const TVersion &aVer);
	};


class DCommWins : public DComm
	{
public:
	enum TDriverCommand {ESetBreak=1,EClearBreak,ETransmit,ETransmit0,
						EGetSignals,ESetSignals,EConfigure,
						EStop,EStopNoDrain,EStart,EDie,	ETransmitCancel,
						EReceive, EReceiveOneOrMore, EReceiveCancel,ENotifyDataAvailable,
						ENotifySignals, EInvalidCommand};
public:
	DCommWins();
	~DCommWins();
	virtual TInt Start();
	virtual void Stop(TStopMode aMode);
	virtual void Break(TBool aState);
	virtual void Write(DThread* aThread, TAny *aTxDes, TInt aLength);
	virtual void Read(DThread* aThread, TAny *aTxDes, TInt aLength);
	virtual void NotifySignals(DThread* aThread, TInt aMask);
	virtual void NotifyDataAvailable();
	virtual TUint Signals() const;
	virtual void SetSignals(TUint aSetMask,TUint aClearMask);
	virtual TInt ValidateConfig(const TCommConfigV01 &aConfig) const;
	virtual void Configure(TCommConfigV01 &aConfig);
	virtual void Caps(TDes8 &aCaps) const;
	virtual void CheckConfig(TCommConfigV01& aConfig);
	virtual TDfcQue* DfcQ(TInt aUnit);

	inline void CheckTxBuffer();
	inline TBool Transmitting();
	virtual TInt RxCount();
	virtual void ResetBuffers(TBool);
	virtual TInt SetRxBufferSize(TInt aSize);
	virtual TInt RxBufferSize();
	virtual TDes8* RxBuffer();
	virtual void DoConfigure();
	virtual TBool AreAnyPending();
	virtual void ReadCancel();
	virtual void WriteCancel();
	virtual void SignalChangeCancel();
	TInt DoCreate(TInt aUnit,const TDesC8 *aInfo);
	void WaitForEvent();
	void DriverCommand(TDriverCommand aCommand);
	void DoWriteComplete(TInt aErr);
	void DoReadComplete(TInt aErr, TInt aBytes);
	void DoSignalCompletion(TInt aError, TUint changed, TUint aValues);
	void DoDataAvailableCompletion();
	void RunCommThread(TDriverCommand aCommand);
	inline void SignalDriverThread();

    TInt QueSocketRead();

    /** @return ETrue if we use socket instead of a real COM port */
    TBool inline UseSocket(void) const
        {return  (iSocket != INVALID_SOCKET) && (iSocket != NULL); }

private:
	void ReleaseBuffers();
	void ReSizeBuffer(TUint8*& aBuf, TInt& iBufLen, TPtr8& aDes, const TInt aNewLen);
	TBool IsATerminator(TText8 aChar);
	void CompleteRead(TInt aLength);
	void DataAvailableNotificationCancel();


private:
	TInt iWritePending;
	TInt iReadPending;
	TBool iStopping;
	TBool iRunning;
	TDriverCommand iCommand;
	TCommConfigV01 *iConfig;
	TUint iSignals;
	TUint iFailSignals;
	TUint iSavedSignals;
	TBool iLineFail;
	TBool iRXLineFail;
	TBool iTXLineFail;

	TUint8 * iInBufPtr;		//input buffer used by driver.
	TInt iInBufLength;
	TPtr8 iInDes;

	TInt iReadSoFar;
	TBool iTerminatedRead;	//true if we are reading with 1 or more terminating characters

	TUint8 * iOutBufPtr;
	TInt iOutBufLength;
	TPtr8 iOutDes;

	TInt iRxBufferSize;		//size of receivebuffer passed to windows & set by calls to SetReceiveBufferSize 
							//used to determine xon and xoff levels
	TUint iSignalsRequested;  //mask of signals we are waiting for
	TUint iSignalsWanted;		//mask we are asked to check 
	TBool iDataAvailableNotification;
	HANDLE iThread;
	HANDLE iCommThreadSem;
	HANDLE iDriverThreadSem;
	HANDLE iCommPort;
	DWORD iThreadID;
	DWORD iSignalStatus;
	OVERLAPPED iReadOverLapped;
	OVERLAPPED iWriteOverLapped;
	OVERLAPPED iSignalOverLapped;

	TInt iClientReadLength;		//how much data the client has requested in a read

    //-- win tunnel specific members
	SOCKET  iSocket;        //< socket for serial port simulation
	char    iOptions[151];  //< contains a string with mapping serial port numbers to the IPs
    TBool   iReadCancel;    //< flag, used to indicate that the read was cancelled during socket read

	};

void Panic(TDCommWinsFault aFault)
//
// Panic the driver 
//
	{
	Kern::PanicCurrentThread(_L("DCommWins"), aFault);
	}


TUint commThread(DCommWins *comm)
//
// Comm thread entry point
//
	{

	comm->WaitForEvent();
	return 0;
	}

VOID WINAPI WriteCompletion(DCommWins *aDrv, DWORD aErr,DWORD /*numBytes*/)
	{

	aDrv->DoWriteComplete(aErr);
	}


VOID WINAPI ReadCompletion(DCommWins *aDrv, DWORD aErr,DWORD numBytes)
	{

	aDrv->DoReadComplete(aErr, numBytes);
	}

VOID WINAPI SignalCompletion(DCommWins *aDrv, TInt aError, TUint aChanged, TUint aValues)
	{
	aDrv->DoSignalCompletion(aError, aChanged, aValues);
	}

VOID WINAPI DataAvailableCompletion(DCommWins *aDrv)
	{
	aDrv->DoDataAvailableCompletion();
	}



BOOL WINAPI EscapeCommFunctionP(HANDLE hFile,DWORD dwFunc, TBool bUseSocket)
	{
	DWORD   err;
	DWORD   lastError = 0;
	BOOL    res;
	COMSTAT s;
	
	if(bUseSocket)
	{
	    res = FALSE;//, not supported... EscapeCommFunction(hFile,dwFunc);
	}
	else
	do
	{
		ClearCommError(hFile, &err, &s);
		res = EscapeCommFunction(hFile,dwFunc);
		if(!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return res;
	}


BOOL WINAPI GetCommModemStatusP(HANDLE hFile,LPDWORD lpModemStat, TBool bUseSocket)
{
	DWORD err;
	DWORD lastError = 0;
	BOOL res;
	COMSTAT s;

	if(bUseSocket)
	{
	    res = FALSE;//, not supported... GetCommModemStatus(hFile,lpModemStat);
	}
	else
	do
	{
		ClearCommError(hFile, &err, &s);
		res = GetCommModemStatus(hFile,lpModemStat);
		if(!res)
			lastError  = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return(res);
}


BOOL WINAPI GetCommStateP(HANDLE hFile,LPDCB lpDCB, TBool bUseSocket)
{
	DWORD err;
	DWORD lastError = 0;
	BOOL res;
	COMSTAT s;
	
	if(bUseSocket)
	{
	    res = FALSE;//, not supported... GetCommState(hFile,lpDCB);
	}
	else
	do
	{
		ClearCommError(hFile,&err,&s);
		res = GetCommState(hFile,lpDCB);
		if (!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return(res);
}

BOOL WINAPI SetCommStateP(HANDLE hFile,LPDCB lpDCB, TBool bUseSocket)
	{
	DWORD   err;
	DWORD   lastError = 0;
	BOOL    res;
	COMSTAT s;
	
	if(bUseSocket)
	{
        res = FALSE;//, not supported... SetCommState(hFile, lpDCB);
	}
	else
	do
	{
		ClearCommError(hFile, &err, &s);
		res = SetCommState(hFile, lpDCB);
		if (!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return(res);
	}

BOOL WINAPI SetCommMaskP(HANDLE hFile,DWORD dwEvtMask, TBool bUseSocket)
{
	DWORD   err;
	DWORD   lastError = 0;
	BOOL    res;
	COMSTAT s;
	
	if(bUseSocket)
	{
	    res = FALSE;//, not supported... SetCommMask(hFile, dwEvtMask);
	}
	else
	do
	{
		ClearCommError(hFile, &err, &s);
		res = SetCommMask(hFile, dwEvtMask);
		if (!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return(res);
}

BOOL WINAPI WriteFileP(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped, TBool bUseSocket)
	{
	DWORD err;
	DWORD lastError = 0;
	BOOL res;
	COMSTAT s;
	
	if(bUseSocket)
	{//-- tunnel data to the socket
		WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}
	else
	do
	{//-- normal operation with serial port
		ClearCommError(hFile, &err, &s);
		res = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
		if (!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

	return(res);
	}

BOOL WINAPI ReadFileP(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped, TBool bUseSocket)
	{
	DWORD err;
	DWORD lastError = 0;
	BOOL res;
	COMSTAT s;

    _Log2("#ReadFileP()#1 to read: %d bytes \n", nNumberOfBytesToRead);

	if(bUseSocket)
	{//-- tunnel data to the socket
		ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
	
	else
	do
	{//-- normal operation with serial port
		ClearCommError(hFile, &err, &s);
		res = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		if (!res)
			lastError = GetLastError();
	}
	while((!res) && (lastError == ERROR_OPERATION_ABORTED));

    _Log2("#ReadFileP()#2 read: %d bytes \n", *lpNumberOfBytesRead);

	return(res);
	}




DDriverComm::DDriverComm()
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
	{
	return(KErrNone);
	}


TInt DDriverComm::Validate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer)
//
//	Validate the requested configuration
//
	{
	if ((!Kern::QueryVersionSupported(iVersion,aVer)) || (!Kern::QueryVersionSupported(aVer,TVersion(KMinimumLddMajorVersion,KMinimumLddMinorVersion,KMinimumLddBuild))))
		return KErrNotSupported;
#if defined (__COM_ONE_ONLY__)
	if (aUnit!=0)
		return KErrNotSupported;
#elif defined (__COM_TWO_ONLY__)
	if (aUnit!=1)
		return KErrNotSupported;
#endif
	// leave Unit check to CreateFile
	return KErrNone;
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
	c.iHandshake = 0x3BF;	//all except KConfigObeyDCD
	c.iSignals=0x3f; // All signals
	c.iSIR=0;//No Ir
	c.iNotificationCaps=KNotifySignalsChangeSupported|KNotifyDataAvailableSupported;
	c.iRoleCaps=0;
	c.iFlowControlCaps=0;
	aCaps.FillZ(aCaps.MaxLength());
	aCaps=capsBuf.Left(Min(capsBuf.Length(),aCaps.MaxLength()));
	}

void DDriverComm::GetCaps(TDes8 &aDes) const
	{

	GetWinsCommsCaps(aDes);
	}
TInt DDriverComm::Create(DBase*& aChannel, TInt aUnit,const TDesC8* aInfo,const TVersion& aVer)
	{

	if (!Kern::QueryVersionSupported(iVersion,aVer))
		return KErrNotSupported;

	DCommWins *pD= new DCommWins;
	if (!pD) return KErrNoMemory;

	TInt ret=pD->DoCreate(aUnit,aInfo);

	if (ret!=KErrNone)
	{
		delete pD;
		return ret;
	}
	
	aChannel = pD;			    

	return KErrNone;
	}

/**
    Associate event with our socket
*/
TInt DCommWins::QueSocketRead()
{
	_Log("# DCommWins::QueSocketRead()\n");
	if(WSAEventSelect(iSocket, iReadOverLapped.hEvent, FD_READ | FD_CLOSE))
		{// Some error?!?
		DWORD res = WSAGetLastError();
		return res;
		}
		
	return KErrNone;

}

void DCommWins::DoWriteComplete(TInt aErr)
	{
	iWritePending = 0;
	StartOfInterrupt();
	iLdd->iTxError = aErr;
	iLdd->iTxCompleteDfc.Add();
	EndOfInterrupt();
	}



void DCommWins::DoSignalCompletion(TInt aError, TUint aChanged, TUint aValues)
	{

	//aValues contains the signal values.  EPOC constants
	//aChanged contains the signals which have changed
	//we return the signal values
	TUint res = aValues & iSignalsWanted;
	res |= (aChanged << 12);
	
	iLdd->iSignalResult = res;
	iLdd->iSignalError = aError;
	StartOfInterrupt();
	iLdd->iSigNotifyDfc.Add();
	EndOfInterrupt();
	}

void DCommWins::DoDataAvailableCompletion()
	{
	StartOfInterrupt();
	iLdd->iRxDataAvailableDfc.Add();
	EndOfInterrupt();
	}



void DCommWins::CompleteRead(TInt aLength)
	{
	
	_Log2("# DCommWins::CompleteRead(), len=%d\n", aLength);
	
	iReadPending = 0;
	iInDes.SetLength(aLength);
	StartOfInterrupt();
	iLdd->iRxCompleteDfc.Add();
	EndOfInterrupt();
	}



void DCommWins::DoReadComplete(TInt aErr, TInt aBytes)
	{
	_Log3("# DCommWins::DoReadComplete(), err=%d, bytes=%d\n", aErr, aBytes);
	
	iLdd->iRxError = aErr;
	//write back the length and the data
	//process for any terminating characters.
	//win32 only does binary reads and ignores the eofchar, so terminated reads
	//require reading one char at a time
	if (iTerminatedRead && !aErr)
		{
		__NK_ASSERT_ALWAYS(aBytes <= 1);
		if (aBytes == 0)
			{
			// not sure why we get this somtimes, but handle it anyway : read another character
			ReadFileP(iCommPort,(void*)(iInBufPtr+iReadSoFar), 1, &dummyLen, &iReadOverLapped, UseSocket());
			}
		else if (++iReadSoFar == iClientReadLength)	//see if we have read enough characters into the buffer
			{
			//got them all so complete it
			CompleteRead(iReadSoFar);
			}
		else if (IsATerminator(iInBufPtr[iReadSoFar-1]))	//see if the char just read was the terminator
			{
			//it's a terminated read and we've found one of the terminbators
			CompleteRead(iReadSoFar);
			}
		else if (iReadPending == EReceive)
			{
			//read another character
			ReadFileP(iCommPort,(void*)(iInBufPtr+iReadSoFar), 1, &dummyLen, &iReadOverLapped, UseSocket());
			}
		else
			{
			//it's a receive 1 or more with terminators, we've got 1 so that'll do
			CompleteRead(iReadSoFar);
			}
		}
	else
		{
		CompleteRead(aBytes);
		}
	}




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
		_Log("# DCommWins::RunCommThread() DriverCommand #1\n");
		DriverCommand(aCommand);

		_Log("#~ DCommWins::RunCommThread() WaitForSingleObject #1\n");
		WaitForSingleObject(iDriverThreadSem,INFINITE);
		}
	else
		{
		Sleep(0); // Possible deadlock solution - see MSDN Knowledge Base Article Q173260
										   
		_Log("# DCommWins::RunCommThread() Release iCommThreadSem\n");
		if (ReleaseSemaphore(iCommThreadSem,1,NULL)==FALSE)
			{
			DWORD ret=GetLastError();
			ret=ret;
			Panic(EWindowsUnexpectedError);
			}
				  
        _Log("#~ DCommWins::RunCommThread() WaitForSingleObject #2\n");
		WaitForSingleObject(iDriverThreadSem,INFINITE);
		_Log("# DCommWins::RunCommThread() checkpoint #2\n");
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
DCommWins::DCommWins() :  iOutDes(0,0), iInDes(0,0), iRxBufferSize(KDefaultWinNTReadBufSize), iSignalsRequested(0)
	{
	__DECLARE_NAME(_S("DCommWins"));
	
    _Log("# ------------------------------------------------\n");
	_Log("# DCommWins::DCommWins()\n");
		
	iSocket = INVALID_SOCKET;
	}
#pragma warning( default : 4705 )


//-----------------------------------------------------------------------------------------------

/**
*   Create the comms driver.
*   @param  aUnit serial porn number to work with (starts with 0)
*/


//-----------------------------------------------------------------------------------------------
TInt DCommWins::DoCreate(TInt aUnit,const TDesC8 * /*aInfo*/)
	{

    _Log("# DCommWins::DoCreate()\n");

#if defined (__COM_ONE_ONLY__)
	if (aUnit!=0)
		return KErrNotSupported;
#elif defined (__COM_TWO_ONLY__)
	if (aUnit!=1)
		return KErrNotSupported;
#endif
    
    //-- obtain and parse a string with mapping serial ports to IP addresses
	//-- the string shall look like: WIN_TUNNEL=0,10.35.2.53,110;4,10.35.2.59,110;5,10.35.2.50,110;6,10.35.2.50,443;7,10.35.2.53,443
	TInt    foundPort   = 0;
	TInt    supposedPort= -1;
	char*   hostname    = iOptions;
	TInt16  portNumber  = 0;

	const char * property = Property::GetString("WIN_TUNNEL");    
	
	if(property)
	{
		if((strlen(property) < 5) || (strlen(property) > 150))
		    FAULT(); //-- illegal parameter string length
			
		strcpy(iOptions, property);
	    char* pchBeg = iOptions;
	    
	    while(!foundPort && pchBeg)
		{
			char* pchEnd;
		
			supposedPort = strtol(pchBeg, &pchEnd, 10);
		
			if(aUnit == supposedPort)
			{//-- found a com port number, extract IP address and port for the socket
				if(*pchEnd++ != ',')
					FAULT();
		    
				hostname = pchBeg = pchEnd;
				pchEnd = strchr(pchBeg, ',');
				pchBeg = pchEnd + 1;
				*pchEnd = '\0';
		    
				portNumber = (TInt16)strtol(pchBeg, &pchEnd, 10);
		    
				foundPort = 1;
			}
			else
			{
				pchBeg = strchr(pchBeg, ';');
				
				if(pchBeg)
					pchBeg ++;
			}
		}
	}//if(property)
    

    //-----------------------------------------

	//create the buffers.
	//the buffers need to be as big as the client will ever use.  8mb reserved, but commit less
	iInBufPtr = (TUint8*)VirtualAlloc(NULL, KSerialBufferMaxSize,MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE);
	if (!iInBufPtr)
		return(Emulator::LastError());

	//commit the first bit of the buffer
	if (!VirtualAlloc(iInBufPtr, KSerialBufferInitialSize, MEM_COMMIT,PAGE_READWRITE))
		return(Emulator::LastError());

	iInBufLength = KSerialBufferInitialSize;
	iInDes.Set(iInBufPtr, 0, iInBufLength);

	//reserve address space for the output buffer
	iOutBufPtr = (TUint8*)VirtualAlloc(NULL, KSerialBufferMaxSize,MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE);
	if (!iOutBufPtr)
		return(Emulator::LastError());

	//commit a smaller region of it
	if (!VirtualAlloc(iOutBufPtr, KSerialBufferInitialSize, MEM_COMMIT,PAGE_READWRITE))
		return(Emulator::LastError());

	iOutBufLength = KSerialBufferInitialSize;
	iOutDes.Set(iOutBufPtr, 0, iOutBufLength);

	//-----------------------------------------


	if(aUnit == supposedPort)
	{//-- a serial port number is found in the parameter string.
	 //-- create and connect socket.
	    
	    WORD    wVersionRequested;
		WSADATA wsaData;
		int     err; 
		wVersionRequested = MAKEWORD(2, 0);
		
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		    return(Emulator::LastError());


		//-- create a socket
		iSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

		_Log3("# DCommWins: created socket id: %d, port emulation: %d\n", iSocket, supposedPort);
		
		if(iSocket == INVALID_SOCKET)
			{
			err = WSAGetLastError();
			return(Emulator::LastError());
			}

		sockaddr_in sin;
		u_long      nRemoteAddr = inet_addr(hostname);

		if (nRemoteAddr != INADDR_NONE)
		{
			sin.sin_addr.s_addr = nRemoteAddr;
		}
		else
		{
			struct hostent FAR * hostaddr = gethostbyname (hostname);
			if(hostaddr == NULL)
			{
				err = WSAGetLastError();
				return(Emulator::LastError());
			}
			sin.sin_addr.s_addr = *((u_long*)hostaddr->h_addr_list[0]);
		}

		sin.sin_family  = AF_INET;
		sin.sin_port    = htons(portNumber);

        //-- connect to the server
		err = WSAConnect(iSocket, (sockaddr*)&sin, sizeof(sin), 0, 0, 0, 0);
		if(err)
		{
			err = WSAGetLastError();
			return(Emulator::LastError());
		}

        _Log("# DCommWins: socket connected. \n");

		iCommPort = (void*)iSocket;
		
    }

    //-----------------------------------------    
    
    if( !UseSocket() )
    {
        _Log2("# DCommWins: Using serial port# %d \n",aUnit);
        
        //-- Open a true serial port
        TBuf8<0x10> n;
    	n.Append(KComName);
    	n.AppendNum(aUnit+1);
    	n.Append('\0');

    	iCommPort=CreateFileA((LPCSTR)n.Ptr(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
    	if (iCommPort==INVALID_HANDLE_VALUE)	
    		{	
    	//	Reused code from Emulator::LastError() rather than adding an extra case 
    	//	to Emulator::LastError() because mapping KErrNotSupported to the returned
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
    	//set the dcb size
    	dcb.DCBlength = sizeof(dcb);
    	if (!GetCommStateP(iCommPort,&dcb, UseSocket()))
    		return(Emulator::LastError());

    	EscapeCommFunctionP(iCommPort,0, UseSocket());

    	if (!SetCommMaskP(iCommPort,EV_CTS|EV_ERR|EV_DSR|EV_RLSD|EV_RXCHAR|EV_RING, UseSocket()))
    		return(Emulator::LastError());

    	if(!SetupComm(iCommPort,KDefaultWinNTReadBufSize,KDefaultWinNTWriteBufSize))
    		return(Emulator::LastError());

    }//if( !UseSocket() )
	
    _Log("# DCommWins: creating semaphores. \n");
    
    //--- create thread semaphores 
	if ((iCommThreadSem=CreateSemaphoreA(NULL,0,0x7fffffff,NULL))==NULL)
		return(Emulator::LastError());

	if ((iDriverThreadSem=CreateSemaphoreA(NULL,0,0x7fffffff,NULL))==NULL)
		return(Emulator::LastError());

    //-----------------------------------------    

    if( !UseSocket() )
    {// The serial port seems to open with the error condition set
    	DWORD err,res;
    	COMSTAT s;
    	if (ClearCommError(iCommPort,&err,&s)==FALSE)
    		res=GetLastError();
    }

    _Log("# DCommWins: creating comm thread. \n");

    iReadCancel = EFalse;

	if ((iThread=CreateWin32Thread(EThreadEvent,(LPTHREAD_START_ROUTINE)commThread,(void *)this, FALSE))==NULL)
		return(Emulator::LastError());

	SetThreadPriority(iThread,THREAD_PRIORITY_HIGHEST);
	
	_Log("# DCommWins::DoCreate() exit\n");
	return(KErrNone);
	}


	
void DCommWins::ReleaseBuffers()
	{
	if (iInBufPtr)
		{
		//decommit the buffer
		VirtualFree(iInBufPtr,KSerialBufferMaxSize, MEM_DECOMMIT);
		//and release the region
		VirtualFree(iInBufPtr, NULL, MEM_RELEASE);
		iInBufPtr = NULL;
		}
	if (iOutBufPtr)
		{
		VirtualFree(iOutBufPtr,KSerialBufferMaxSize, MEM_DECOMMIT);
		VirtualFree(iOutBufPtr, NULL, MEM_RELEASE);
		iOutBufPtr = NULL;
		}
	}
	

DCommWins::~DCommWins()
	{
	_Log("# DCommWins::~DCommWins()\n");
	if (iThread)
		{
		if (! iRunning)
			{
			__ASSERT_ALWAYS(
				ResumeThread(iThread) != 0xffffffff,
				Panic(EWindowsUnexpectedError)); 
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
	{
		CloseHandle(iCommPort);
	}

	if (iDriverThreadSem)
		CloseHandle(iDriverThreadSem);

	if (iCommThreadSem)
		CloseHandle(iCommThreadSem);

	if (iReadOverLapped.hEvent)
		CloseHandle(iReadOverLapped.hEvent);

	if (iWriteOverLapped.hEvent)
		CloseHandle(iWriteOverLapped.hEvent);

	if (iSignalOverLapped.hEvent)
		CloseHandle(iSignalOverLapped.hEvent);

	if (iThread)
		CloseHandle(iThread);

	//free up the memory
	ReleaseBuffers();

	_Log("# DCommWins::~DCommWins() exit\n");
	
	}

TInt DCommWins::Start()
	{

	__ASSERT_ALWAYS(ResumeThread(iThread)!=0xffffffff,Panic(EWindowsUnexpectedError));
	iRunning=ETrue;
	RunCommThread(EStart);
	return(KErrNone);
	}

void DCommWins::Stop(TStopMode aMode)
	{

	RunCommThread((aMode==EStopEmergency) ? EStopNoDrain : EStop);
	SuspendThread(iThread);
	iRunning=EFalse;
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



TInt DCommWins::RxCount()
	{
	DWORD err =0;
	COMSTAT stat;
	if (ClearCommError(iCommPort,&err, &stat))
		return stat.cbInQue;
	else
		return Emulator::LastError();
	}


void DCommWins::ResetBuffers(TBool aTx)
	{
	PurgeComm(iCommPort, PURGE_RXCLEAR | (aTx ? PURGE_TXCLEAR : 0));
	}


TBool DCommWins::AreAnyPending()
	{
	return (iReadPending != 0) || (iWritePending != 0);
	}


void DCommWins::WriteCancel()
	{
	DriverCommand(ETransmitCancel);
	}


void DCommWins::ReadCancel()
	{
	DriverCommand(EReceiveCancel);
	}

void DCommWins::SignalChangeCancel()
	{
	if (iSignalsRequested)
		{
		iSignalsRequested = 0;
		}

	}

void DCommWins::DataAvailableNotificationCancel()
	{
	if (iDataAvailableNotification)
		{
		iDataAvailableNotification = EFalse;
		iLdd->iRxDAError = KErrCancel;
		iLdd->iRxDataAvailableDfc.Enque();
		}
	}

TDes8* DCommWins::RxBuffer()
	{
	return &iInDes;
	}

TInt DCommWins::SetRxBufferSize(TInt aSize)
	{
	aSize += aSize&1;	//round up to multiple of 2 bytes as windows complains if odd
	TInt ret = SetupComm(iCommPort, aSize, KDefaultWinNTWriteBufSize);
	if (ret)
		{
		iRxBufferSize = aSize;
		
		DCB dcb = {0};
		dcb.DCBlength = sizeof(dcb);
		
		if (!GetCommStateP(iCommPort,&dcb, UseSocket()))
			return Emulator::LastError();

		//use rx buffer size to configure xon/xoff limits
		dcb.XoffLim = (WORD)(iRxBufferSize / 4);		//25%
		if (iConfig->iParityError & KConfigXonXoffDebug)
			dcb.XonLim = (WORD)((iRxBufferSize / 2) -5);	//50%-5
		else
			dcb.XonLim = (WORD)((iRxBufferSize / 4) * 3);	//75%

		if (!SetCommStateP(iCommPort,&dcb, UseSocket()))
			return Emulator::LastError();

		return KErrNone;
		}
	return Emulator::LastError();
	}


TInt DCommWins::RxBufferSize()
	{
	return iRxBufferSize;
	}


TBool DCommWins::IsATerminator(TText8 aChar)
	{
	TInt x;
	for (x=0; x < iConfig->iTerminatorCount; x++)
		if (aChar == iConfig->iTerminator[x])
			return ETrue;
	return EFalse;
	}


void DCommWins::ReSizeBuffer(TUint8*& aBuf, TInt& aBufLen, TPtr8& aDes, const TInt aNewLen)
	{

	if (aNewLen > KSerialBufferMaxSize)
		Panic(ESerialBufferTooBig);

	aBufLen = ((aNewLen + KSerialBufferIncrementSize-1) / KSerialBufferIncrementSize) * KSerialBufferIncrementSize;

	if (aBufLen > KSerialBufferMaxSize)
		aBufLen = KSerialBufferMaxSize;

	if (!VirtualAlloc(aBuf, aBufLen, MEM_COMMIT,PAGE_READWRITE))
		{
		ReleaseBuffers();
		Panic(ESerialBufferTooBig);
		}
	aDes.Set(aBuf, 0, aBufLen);
	}


void DCommWins::Write(DThread* aThread, TAny* aSrc, TInt aLength)

	{
	if (aLength==0)
		{
		RunCommThread(ETransmit0);
		}
	else
		{
		if (aLength > iOutBufLength)
			ReSizeBuffer(iOutBufPtr, iOutBufLength, iOutDes, aLength);

		//copy the data from the client
		Kern::ThreadDesRead(aThread, aSrc, iOutDes, 0,0);
		iOutDes.SetLength(aLength);
		//tell the comms thread to write the data
		RunCommThread(ETransmit);
		}
	}

void DCommWins::NotifySignals(DThread* /*aThread*/, TInt aMask)
	{
	iSignalsWanted = aMask;
	RunCommThread(ENotifySignals);
	}


void DCommWins::NotifyDataAvailable()
	{
	RunCommThread(ENotifyDataAvailable);
	}


void DCommWins::Read(DThread* /*aThread*/, TAny* /*aDest*/, TInt aLength)

	{
	_Log2("# DCommWins::Read, len=%d\n", aLength);
	
	TDriverCommand command;

	if (aLength < 0)
		{
		iClientReadLength = Abs(aLength);
		command = EReceiveOneOrMore;
		}
	else
		{
		iClientReadLength = aLength;
		command = EReceive;
		}

	if (iClientReadLength > iInBufLength)
		ReSizeBuffer(iInBufPtr, iInBufLength, iInDes, iClientReadLength);
	
	//tell the comms thread to read the data
	RunCommThread(command);
	}





TUint DCommWins::Signals() const
{
    if(UseSocket())
	    return MS_CTS_ON|MS_DSR_ON|KSignalCTS|KSignalDSR;

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
	return(status|iSignals);
}


void DCommWins::SetSignals(TUint aSetMask,TUint aClearMask)
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
	}



void DCommWins::CheckConfig(TCommConfigV01& /*aConfig*/)
	{
	// Do nothing
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

    if(UseSocket())
        return; // nothing to configure

	DCB dcb = {0};
	//set the dcb size
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommStateP(iCommPort,&dcb, UseSocket()))
		return;

			
	//stop if an error happens
	dcb.fAbortOnError = TRUE;
	
	//baud rate
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
   
	switch (iConfig->iParity)
		{
	case EParityNone:
		dcb.Parity=NOPARITY;
		dcb.fParity = FALSE;
		break;
	case EParityEven:
		dcb.Parity=EVENPARITY;
		dcb.fParity = TRUE;
		break;
	case EParityOdd:
		dcb.Parity=ODDPARITY;
		dcb.fParity = TRUE;
		break;
	case EParityMark:
		dcb.Parity = MARKPARITY;
		dcb.fParity = TRUE;
		break;
	case EParitySpace:
		dcb.Parity = SPACEPARITY;
		dcb.fParity = TRUE;
		break;
		}

	switch (iConfig->iParityError)
		{
	case KConfigParityErrorFail:
		dcb.fErrorChar = FALSE;
		break;

	case KConfigParityErrorIgnore:
		dcb.fErrorChar = FALSE;
		dcb.fAbortOnError = FALSE;
		break;

	case KConfigParityErrorReplaceChar:
		dcb.fErrorChar = TRUE;
		dcb.ErrorChar = iConfig->iParityErrorChar;
		break;
		}


	TUint& hs = iConfig->iHandshake;


	//SOFTWARE FLOW CONTROL
	dcb.fInX  = (hs & KConfigObeyXoff) ? TRUE : FALSE;
	dcb.fOutX = (hs & KConfigSendXoff) ? TRUE : FALSE;

	dcb.XonChar = iConfig->iXonChar;
	dcb.XoffChar = iConfig->iXoffChar;
	dcb.ErrorChar = iConfig->iParityErrorChar;

	//use rx buffer size to configure xon/xoff limits
	dcb.XoffLim = (WORD)(iRxBufferSize / 4);		//25%
	if (iConfig->iParityError & KConfigXonXoffDebug)
		dcb.XonLim = (WORD)((iRxBufferSize / 2) -5);	//50%-5
	else
		dcb.XonLim = (WORD)((iRxBufferSize / 4) * 3);	//75%



	//OUTPUT HARDWARE FLOW CONTROL
	//set out DSR control to be off
	dcb.fOutxDsrFlow = FALSE;
	dcb.fOutxCtsFlow = (hs & KConfigObeyCTS) ? TRUE : FALSE;
	dcb.fDsrSensitivity =  (hs & KConfigObeyDSR) ? TRUE : FALSE;


	if (hs & KConfigObeyDCD)
		{
		}

	
	//INPUT HARDWARE FLOW CONTROL
	dcb.fRtsControl = (hs & KConfigFreeRTS) ? RTS_CONTROL_DISABLE : RTS_CONTROL_HANDSHAKE;
	dcb.fDtrControl = (hs & KConfigFreeDTR) ? DTR_CONTROL_DISABLE : DTR_CONTROL_ENABLE;


	//complete with KErrCommsLineFail if these are set and the line goes low
	iFailSignals = 0;
	if (hs & KConfigFailDSR)
		iFailSignals |= KSignalDSR;
	
	if (hs & KConfigFailCTS)
		iFailSignals |= KSignalCTS;

	if (hs & KConfigFailDCD)
		iFailSignals |= KSignalDCD;
	
	
	iTerminatedRead = iConfig->iTerminatorCount > 0;

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



	TInt error_r=KErrNone;
	if(!SetCommStateP(iCommPort,&dcb, UseSocket()))
		error_r=GetLastError();

// Clear any error we may have caused
//
	DWORD err,res;
	COMSTAT s;
	if (ClearCommError(iCommPort,&err,&s)==FALSE)
		res=GetLastError();

	}

void DCommWins::Caps(TDes8 &aCaps) const
//
// Return the current capabilities
//
	{

	GetWinsCommsCaps(aCaps);
	}


void DCommWins::WaitForEvent()
	{

	HANDLE objects[4];


	iReadOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL); 
	iWriteOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL);
	iSignalOverLapped.hEvent=CreateEventA(NULL,FALSE,FALSE,NULL);
	objects[0]=iSignalOverLapped.hEvent; // iCommPort;
	objects[1]=iCommThreadSem;
	objects[2]=iWriteOverLapped.hEvent;
	objects[3]=iReadOverLapped.hEvent;

	FOREVER
		{
		
		_Log("#~ DCommWins::WaitForEvent() WaitForMultipleObjectsEx #1\n");
		DWORD ret=WaitForMultipleObjectsEx(4,objects,FALSE,INFINITE,TRUE);
		_Log2("#--  DCommWins::WaitForEvent(), event=%d\n", ret);
		
		switch (ret)
			{
		case WAIT_OBJECT_0:
			{
			//			EnterCritical();

            _Log("#--     DCommWins::WaitForEvent(), event0\n");
            
			if (iDataAvailableNotification)
				{
				DataAvailableCompletion(this);
				iDataAvailableNotification = EFalse;	//stop us repeatedly reporting it
				}

			TUint currentSignals = Signals();

			//mask out all the signals but the ones we are interested in
			iLineFail = (iFailSignals & currentSignals) != iFailSignals;
			if (iLineFail)
				{
				//if we have the handshake options of
				//KConfigFailDSR, KConfigFailDCD KFailConfigCTS set
				//we need to do something if any of them are low so
				//complete any outstanding ops with failure
				if (iReadPending)
					{
					//we have a read to complete
					iRXLineFail = ETrue;
					PurgeComm(iCommPort, PURGE_RXABORT);
					}

				if (iWritePending)
					{
					//we have a write to complete
					iTXLineFail = ETrue;
					PurgeComm(iCommPort, PURGE_TXABORT);
					}
				}


			//iSignalsRequested will only have bits set if outstanding request
			TUint changed = (currentSignals ^ iSavedSignals) & iSignalsRequested;
			if (changed) 
				{
				SignalCompletion(this, KErrNone, changed, currentSignals);
				iSavedSignals = currentSignals;
				iSignalsRequested = 0;				//stop us repeatedly reporting it.
												//iSignalsRequested is setup in the call to notify
				}
			
			if (iWritePending == ETransmit0 && (currentSignals & KSignalCTS) != 0)
				WriteCompletion(this, KErrNone, 0);

			//request another notification event.  All events are requested.
			iSignalStatus=0;
			DWORD commErrors;
			BOOL res;
			DWORD lastError = 0;
			COMSTAT cstat;

			do
				{
				ClearCommError(iCommPort,&commErrors,&cstat);
				res = WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped);
				if (!res)
					lastError = GetLastError();
				}
			while((!res) && (lastError != ERROR_IO_PENDING));

			break;
			}

		case WAIT_OBJECT_0+1:
//
// iCommThreadSemaphore has been signalled
//			
			_Log2("#--     DCommWins::WaitForEvent(), event1, driver command %d \n", iCommand);
			DriverCommand(iCommand);
			break;
			
		case WAIT_OBJECT_0+2:
			//
			//	Write completion
			//
			{
			_Log("#--     DCommWins::WaitForEvent(), event2, write completion \n");
			 
			DWORD len = 0;
			TInt error = KErrNone;
			if (!GetOverlappedResult(iCommPort, &iWriteOverLapped, &len, FALSE))
				error = Emulator::LastError();
			
			COMSTAT s;
			DWORD err = 0;
			ClearCommError(iCommPort,&err,&s);

			//if we are failing if one or more of CTS, DSR, DCD go low
			if (iTXLineFail)
				{
				error = KErrCommsLineFail;
				iTXLineFail = EFalse;
				}
			else if (err)
				{
				if (err & CE_FRAME)
					error = KErrCommsFrame;
				else if (err & CE_OVERRUN)
					error = KErrCommsOverrun;
				else if (err & CE_RXPARITY)
					error = KErrCommsParity;
				}

			WriteCompletion(this, error, len);
			break;
			}

		case WAIT_OBJECT_0+3:
			//
			//	Read completion
			//
		{
			//if(!iCommandEvent) break;
			
			_Log("#--     DCommWins::WaitForEvent(), event3, read completion \n");
			
			DWORD len = 0;
			TInt error = KErrNone;

    	    if(UseSocket() && iReadCancel)	
    	    {//-- socket read cancel occured													 
    	        error = KErrCancel;
    	        iReadCancel = EFalse;
    	    }	
    	    else
    	    {		
    			if (!GetOverlappedResult(iCommPort, &iReadOverLapped, &len, FALSE))
    				error = Emulator::LastError();

    			COMSTAT s;
    			DWORD err = 0;
    			ClearCommError(iCommPort,&err,&s);

    			//if we are failing if one or more of CTS, DSR, DCD go low
    			if (iRXLineFail)
    				{
    				error = KErrCommsLineFail;
    				iRXLineFail = EFalse;
    				}
    			else if (err)
    				{
    				if (err & CE_FRAME)
    					error = KErrCommsFrame;
    				else if (err & CE_OVERRUN)
    					error = KErrCommsOverrun;
    				else if (err & CE_RXPARITY)
    					error = KErrCommsParity;
    				}
            }
		
		ReadCompletion(this, error, len);
		break;
		}

		case WAIT_IO_COMPLETION:
			_Log("#--     DCommWins::WaitForEvent(), Wait IO completion \n");
			
			break;

		default:
			Emulator::LastError();
			FAULT();
			}
		}
	}

void DCommWins::DriverCommand(TDriverCommand aCommand)
//
// Do a driver command - executed when the semaphore has been signalled in the comm port thread
//
	{
	
	_Log2("#<- DCommWins::DriverCommand() :%d \n", aCommand);
	
	switch (aCommand)
		{
	case ESetBreak:
		SetCommBreak(iCommPort);
		break;

	case EClearBreak:
		ClearCommBreak(iCommPort);
		break;

	case ETransmit0:
		
		if (!iWritePending)
			{
			if ((iConfig->iHandshake & KConfigObeyCTS) != 0 && (Signals() & KSignalCTS) == 0)
				iWritePending = ETransmit0;
			else
				DoWriteComplete(KErrNone);
			}
		break;

	case ETransmit:
		
		if (!iWritePending)
			{
			COMMTIMEOUTS ct;
			int r = GetCommTimeouts(iCommPort, &ct);
			ct.WriteTotalTimeoutConstant = 0;
			ct.WriteTotalTimeoutMultiplier = 0;
			r = SetCommTimeouts(iCommPort, &ct);

			WriteFileP(iCommPort,iOutDes.Ptr(), iOutDes.Length(), &dummyLen, &iWriteOverLapped, UseSocket());
			iWritePending = ETransmit;
			}
		break;

	case EStart:
	{
		iSignalStatus=0;
		iSignalStatus=0;
		
		if(! UseSocket())
		{
		
    		DWORD commErrors;
    		BOOL res;
    		DWORD lastError = 0;
    		COMSTAT cstat;

    		do
    			{
    			ClearCommError(iCommPort,&commErrors,&cstat);
    			res = WaitCommEvent(iCommPort,&iSignalStatus,&iSignalOverLapped);
    			if (!res)
    				lastError = GetLastError();
    			}
    		while((!res) && (lastError != ERROR_IO_PENDING));
		 }
		 else
		if(QueSocketRead() != KErrNone)
		{	// Inital Read Queue Failed :-(
			// __DEBUGGER();
		}
		 
	}
	break;
	
	case EStop:
        // Flush last write
		if(iWritePending == ETransmit)
			{

			_Log("#~ DCommWins::DriverCommand() WaitForSingleObject (case EStop)\n");

			if(!UseSocket()) //-- can wait forewer on socket
			    WaitForSingleObject(iWriteOverLapped.hEvent, INFINITE);
			    
			FlushFileBuffers(iCommPort);
			}
        iWritePending=0;
		// Fall through
	case EStopNoDrain:
		// Cancel any pending writes
        if(iWritePending == ETransmit)
		    {
			PurgeComm(iCommPort, PURGE_TXABORT|PURGE_TXCLEAR);
            _Log("#~ DCommWins::DriverCommand() WaitForSingleObject (case EStopNoDrain #1)\n");

            if(!UseSocket()) //-- can wait forewer on socket
                WaitForSingleObject(iWriteOverLapped.hEvent, INFINITE);
		    }   
        iWritePending=0;

		iStopping=ETrue;
        
        if(iRunning)
            {
            SetCommMaskP(iCommPort,EV_CTS|EV_ERR|EV_DSR|EV_RLSD|EV_RXCHAR, UseSocket());
            _Log("#~ DCommWins::DriverCommand() WaitForSingleObject (case EStopNoDrain #2)\n");

            if(!UseSocket()) //-- can wait forewer on socket
                WaitForSingleObject(iSignalOverLapped.hEvent, INFINITE);
            }
		break;

	case EDie:	
		SignalDriverThread();
		ExitThread(1);
		break;
	
	case EConfigure:
		DoConfigure();
		break;
	
	case ETransmitCancel:
		if (iWritePending == ETransmit)
			PurgeComm(iCommPort, PURGE_TXABORT);
//		else if (iWritePending == ETransmit0)
//			{
			// careful - this runs in the context of the kernel thread, not the event thread
		iLdd->iTxError = KErrCancel;
		iLdd->iTxCompleteDfc.Enque();
//			}
		break;

	case EReceive:
		if (!iReadPending)
			{
			COMMTIMEOUTS ct;
			int r = GetCommTimeouts(iCommPort, &ct);
			ct.ReadIntervalTimeout = 0;
			ct.ReadTotalTimeoutMultiplier = 0;
			ct.ReadTotalTimeoutConstant = 0;
			r = SetCommTimeouts(iCommPort, &ct);

			//if we are doing a terminated read.... we need to do it a byte at a time!
			if (iTerminatedRead)
				{
				iReadSoFar = 0;
				ReadFileP(iCommPort,(void*)iInDes.Ptr(), 1, &dummyLen, &iReadOverLapped, UseSocket());
				}
			else
				{
				ReadFileP(iCommPort,(void*)iInDes.Ptr(), iClientReadLength, &dummyLen, &iReadOverLapped, UseSocket());
				}

			iReadPending = EReceive;
			}
		break;

	case EReceiveOneOrMore:
		if (!iReadPending)
			{
			COMMTIMEOUTS ct;
			int r = GetCommTimeouts(iCommPort, &ct);
			ct.ReadIntervalTimeout = MAXDWORD;
			ct.ReadTotalTimeoutMultiplier = MAXDWORD;
			ct.ReadTotalTimeoutConstant = KReadOneOrMoreTimeout;
			r = SetCommTimeouts(iCommPort, &ct);

			//if we are doing a terminated read....
			if (iTerminatedRead)
				{
				iReadSoFar = 0;
				ReadFileP(iCommPort,(void*)iInDes.Ptr(), 1, &dummyLen, &iReadOverLapped, UseSocket());
				}
			else
				{
				ReadFileP(iCommPort,(void*)iInDes.Ptr(), iClientReadLength, &dummyLen, &iReadOverLapped, UseSocket());
				}

			iReadPending = EReceiveOneOrMore;
			}
		break;

	case EReceiveCancel:
	
		if (iReadPending)
		    if(!UseSocket()) 
		        PurgeComm(iCommPort, PURGE_RXABORT);
		    else
		    {//-- force to resume comms event watcher thread and simulate read cancellation on the socket
		    iReadCancel=ETrue;
            SetEvent(iReadOverLapped.hEvent);
		    }
		else 
		if (iDataAvailableNotification)
			DataAvailableNotificationCancel();


	break;

	case ENotifyDataAvailable:
		{
		iDataAvailableNotification = ETrue;
		//setup the comms notifications for data available
		break;
		}

	case ENotifySignals:
		{
		TUint currentSignals = Signals();
		TUint changed = (currentSignals ^ iSavedSignals) & iSignalsWanted;
		if (changed) 
			{
			SignalCompletion(this, KErrNone, changed, currentSignals);
			iSavedSignals = currentSignals;
			iSignalsWanted = 0;				
			}
		else
			iSignalsRequested = iSignalsWanted;	//checked when signals change
		}
		break;


	default:
		// Panic(EUnknownCommand);
		break;
		}
	iCommand=EInvalidCommand;
	SignalDriverThread();
	}


TDfcQue* DCommWins::DfcQ(TInt /*aUnit*/)
	{
	return Kern::DfcQue0();
	}


TInt DCommWins::ValidateConfig(const TCommConfigV01 &aConfig) const
//
// Check a config structure.
//
	{
	if(aConfig.iRate & EBpsSpecial)
		return KErrNotSupported;

	switch (aConfig.iParity)
		{
		case EParityNone:
		case EParityOdd:
		case EParityEven:
			break;
		default:
			return KErrNotSupported;
		}
	switch (aConfig.iRate)
		{
		case EBps50:
		case EBps75:
		case EBps134:
		case EBps1800:
		case EBps2000:
		case EBps3600:
		case EBps7200:
			return KErrNotSupported;
		default:
			break;
		};
	return KErrNone;
	}




DECLARE_STANDARD_PDD()
	{
	return new DDriverComm;
	}

