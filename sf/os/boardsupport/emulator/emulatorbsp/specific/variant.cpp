// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\variant.cpp
// 
//

#include "variant.h"
#include "mconf.h"
#include <kernel/kern_priv.h>
#include <stdlib.h>
#include <property.h>
#include <emulator.h>

const TInt KDefaultRam = 63;				// 63MB default internal RAM
const TInt KUnlimitedRam = 0x400;			// 1GB ~= unlimited memory
const TInt KDefaultRamDrive = 0x400000;		// 4MB default RAM drive limit

_LIT(KLitWins,"Wins");

GLDEF_D Wins TheVariant;

GLDEF_D TActualMachineConfig TheConfig;

EXPORT_C Asic* VariantInitialise(TBool aRunExe)
	{
	return TheVariant.Init(aRunExe) == KErrNone ? &TheVariant : NULL;
	}

void AsicInitialise()
	{
	}

class DWinsPowerController : public DPowerController
	{
public: // from DPowerComtroller
	void CpuIdle();
	void EnableWakeupEvents();
	void AbsoluteTimerExpired();
	void DisableWakeupEvents();
	void PowerDown(TTimeK aWakeupTime);
public:
	static DWinsPowerController* New();
	void AssertWakeupSignal();
	void WakeupEvent();
private:
	HANDLE	iStandbySem;
	TUint	iStandby;
	TBool	iWakeupSignal;
	};

Wins::Wins()
	:iUi(0), iRealCpuSpeed(0), iCpuSpeed(0),
	iDebugOutput(INVALID_HANDLE_VALUE), iLogTimeStamp(ETrue),
	iPurgedImages(EFalse), iPowerController(0), iLogToDebugger(EFalse),
	iLogToFile(ETrue)
	{}

TInt Wins::Init(TBool aRunExe)
	{
	TInt r = InitProperties(aRunExe);
	if (r == KErrNone)
		{
		iProperties.GetInt("LogTimeStamp",iLogTimeStamp);
		TInt logThreadId=ETrue;
		iProperties.GetInt("LogThreadId",logThreadId);
		TInt cpu=NThread::ECpuSingle;
		iProperties.GetInt("HostCPU",cpu);
		NThread::SetProperties(logThreadId,cpu);
		iProperties.GetInt("LogToDebugger",iLogToDebugger);
		iProperties.GetInt("LogToFile",iLogToFile);

		TInt mask;
		Kern::SuperPage().iDebugMask[0] = DebugMask();  // get int or text mask value
		// check to see if DebugMask0 was used instead of DebugMask
		if ( (iProperties.GetInt("DebugMask", mask) != KErrNone) &&
			 (iProperties.GetInt("DebugMask0", mask) == KErrNone) )
			Kern::SuperPage().iDebugMask[0] = ((iProperties.GetInt("DebugMask0", mask) == KErrNone) ? mask : 0);
			
		// only int entries are supported for DebugMasks 1-7
		Kern::SuperPage().iDebugMask[1] = ((iProperties.GetInt("DebugMask1", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[2] = ((iProperties.GetInt("DebugMask2", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[3] = ((iProperties.GetInt("DebugMask3", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[4] = ((iProperties.GetInt("DebugMask4", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[5] = ((iProperties.GetInt("DebugMask5", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[6] = ((iProperties.GetInt("DebugMask6", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iDebugMask[7] = ((iProperties.GetInt("DebugMask7", mask) == KErrNone) ? mask : 0);

		// initial values for fast trace...
		Kern::SuperPage().iInitialBTraceFilter[0] = ((iProperties.GetInt("BTrace0", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[1] = ((iProperties.GetInt("BTrace1", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[2] = ((iProperties.GetInt("BTrace2", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[3] = ((iProperties.GetInt("BTrace3", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[4] = ((iProperties.GetInt("BTrace4", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[5] = ((iProperties.GetInt("BTrace5", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[6] = ((iProperties.GetInt("BTrace6", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceFilter[7] = ((iProperties.GetInt("BTrace7", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceBuffer = ((iProperties.GetInt("BTraceBuffer", mask) == KErrNone) ? mask : 0);
		Kern::SuperPage().iInitialBTraceMode = ((iProperties.GetInt("BTraceMode", mask) == KErrNone) ? mask : 0);

		Kern::SuperPage().SetKernelConfigFlags(KernelConfigFlags());

		SCapabilitySet caps;
		DisabledCapabilities(caps);
		memcpy(&Kern::SuperPage().iDisabledCapabilities,&caps,sizeof(caps));
		}
	CalibrateCpuSpeed();
	return r;
	}

inline void Wins::InstallUi(DWinsUiBase& aUi)
	{iUi = &aUi;}

void Wins::Init1()
	{
	__KTRACE_OPT(KBOOT,Kern::Printf("Wins::Init1()"));

	TInt tickperiod = WinsTimer::EDefaultPeriod;
	iProperties.GetInt("TimerResolution",tickperiod);
	iTimer.Init(tickperiod);
	TUint speed;
	if (iProperties.GetInt("CPUSpeed", (TInt&)speed) == KErrNone)
		SetCpuSpeed(speed);
	}

static TInt emulatorHal(TAny* aPtr, TInt aFunction, TAny* a1, TAny* a2)
	{
	return ((Wins*)aPtr)->EmulatorHal(aFunction, a1, a2);
	}

void Wins::Init3()
//
// Initialise timer tick and add emulator hal function
//
	{
	__KTRACE_OPT(KBOOT,Kern::Printf("Wins::Init3()"));

	Kern::AddHalEntry(EHalGroupEmulator,&emulatorHal,this);

	iPowerController = DWinsPowerController::New();
	if (iPowerController == 0)
		__PM_PANIC("Can't create 'DWinsPowerController'");

	iTimer.Enable();
	}

void Wins::AddressInfo(SAddressInfo& aInfo)
	{
	TInt megabytes = KDefaultRam;
	iProperties.GetInt("MegabytesOfFreeMemory", megabytes);
	if (megabytes == 0)
		megabytes = KUnlimitedRam;
	aInfo.iTotalRamSize = megabytes << 20;
//
	TInt ramdisk = KDefaultRamDrive;
	iProperties.GetInt("RamDriveMaxSize", ramdisk);
	aInfo.iRamDriveMaxSize = ramdisk;
	}

void Wins::PurgeImages()
//
// Use the idle thread to clean up remnants of the emulator from the image path
//
	{
	char path[KMaxFileName+1];

	const char* imgPath=0;
	iProperties.GetString("EmulatorImagePath", imgPath);
	strcpy(path, imgPath);
	char* name = path +strlen(path);
	strcpy(name,"*");

	Emulator::Lock();

	WIN32_FIND_DATAA fdata;
	HANDLE finder = FindFirstFileA(path, &fdata);
	if (finder != INVALID_HANDLE_VALUE)
		{
		do
			{
			if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
				strcpy(name, fdata.cFileName);
				DeleteFileA(path);
				}
			} while (FindNextFileA(finder, &fdata));
		FindClose(finder);
		}

	Emulator::Unlock();
	}

void Wins::Idle()
//
// Use the win32 NKern idle function
//
	{
	iTimer.SetIdleThread();
	if (!iPurgedImages)
		{
		PurgeImages();
		iPurgedImages = ETrue;
		}
	NThread::Idle();
	}

TInt Wins::MsTickPeriod()
//
// Provide the 'millisecond' timer tick period in microseconds
//
	{
	return 1000 * iTimer.Period();
	}

TInt Wins::SystemTimeInSecondsFrom2000(TInt& aTime)
	{
	aTime = iTimer.SystemTime();
	__KTRACE_OPT(KHARDWARE,Kern::Printf("RTC READ: %d",aTime));
	return KErrNone;
	}

TInt Wins::SetSystemTimeInSecondsFrom2000(TInt aTime)
// 
// Set the emulator time. We must not change the Win32 time so
// we just adjust the offset value to account for the difference
//
	{
	__KTRACE_OPT(KHARDWARE,Kern::Printf("Set RTC: %d",aTime));
	iTimer.SetSystemTime(aTime);
	return KErrNone;
	}

TInt Wins::VariantHal(TInt aFunction, TAny* a1, TAny* /*a2*/)
	{
	TInt r=KErrNone;
	switch(aFunction)
		{
		case EVariantHalVariantInfo:
			{
			TVariantInfoV01Buf infoBuf;
			TVariantInfoV01& info=infoBuf();

//			info.iRomVersion=TVersion(KRomMajorVersionNumber,KRomMinorVersionNumber,KRomBuildVersionNumber);
			info.iMachineUniqueId=0;
			info.iLedCapabilities=0x0;
			info.iProcessorClockInKHz=iCpuSpeed ? iCpuSpeed*1000 : 1;
			info.iSpeedFactor=0;
			if (iUi)
				iUi->Info(info);

			Kern::InfoCopy(*(TDes8*)a1,infoBuf);
			break;
			}

		case EVariantHalCustomRestartReason:
			{
			//This will take value from c:\data\epoc.ini.
			TInt x = Property::GetInt("CustomRestartReason");    
			kumemput32(a1, &x, sizeof(TInt));
			}
			break;

		case EVariantHalCustomRestart:
			{
			if(!Kern::CurrentThreadHasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING("Checked by Hal function EVariantHalCustomRestart")))
				return KErrPermissionDenied;
			//This will only shut down epoc as Custom Restart Reason is not supported on wins.
			Kern::Restart((TInt)a1);
			break;
			}

		default:
			r=KErrNotSupported;
			break;
		}
	return r;
	}

TPtr8 Wins::MachineConfiguration()
	{
	return TPckg<TActualMachineConfig>(TheConfig);
	}

void Wins::CalibrateCpuSpeed()
//
// calculate approx. CPU speed in MHz, 0 if we can't tell
//
	{
    TInt cycleCount =200*1000*20;   //take 20ms at 20MHz
    
    // This loop will double the cyclecount until the difference is non-zero.
    FOREVER
    	{
      	if (cycleCount > (KMaxTUint / 2))
    		{
    		break;
    		}
    
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		LARGE_INTEGER start;
		if (QueryPerformanceCounter(&start))
    		{
	   		__asm mov eax, cycleCount
 	noploop: 
 			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm dec eax
			__asm sub eax, 2
			__asm jnz noploop
			//
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			LARGE_INTEGER f;
			QueryPerformanceFrequency(&f);
			
			TInt64 diff = (end.QuadPart - start.QuadPart);
	    	if(diff!=0)
	    		{
				TInt64 hz = (TInt64(cycleCount) / 1000000) * (f.QuadPart / diff);
				iRealCpuSpeed = (TUint)(hz);
				break;
		     	}
  	  	    }
    	cycleCount *= 2;  // Double the count!!
 		}
    
   	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}

TInt Wins::SetCpuSpeed(TUint aSpeed)
	{
	if (iRealCpuSpeed == 0)
		return KErrNotSupported;	// don't know the real CPUSpeed

	if (IsDebuggerPresent())
		return KErrGeneral;			// nobbling not avaliable when debugging

	if (aSpeed == 0)
		aSpeed = iRealCpuSpeed;		// reset to maximum
	else if (aSpeed > iRealCpuSpeed)
		aSpeed = iRealCpuSpeed;
	else if (aSpeed * 20u  < iRealCpuSpeed)
		aSpeed = (iRealCpuSpeed + 19u) / 20u;

	__KTRACE_OPT(KHARDWARE,Kern::Printf("Set CPUSpeed: %d",aSpeed));
	iCpuSpeed = aSpeed;

	// calculate CPU time to nobble in parts-per-million
	TUint nobble = ((iRealCpuSpeed - aSpeed) * 1000000u) / iRealCpuSpeed;
	iTimer.Nobble(nobble);
	return KErrNone;
	}

HANDLE Wins::DebugOutput()
//
// Return a handle to the trace file, creating the file if required.
//
// The log file name can be specified by a property or environment
// variable called 'EmulatorLog', otherwise it defaults to 
// 'epocwind.out' in the temporary directory.
//
	{
	HANDLE file = iDebugOutput;
	if (file == INVALID_HANDLE_VALUE)
		{
		CHAR debugfile[MAX_PATH];
		const char* logpath;
		if (iProperties.GetString("EmulatorLog",logpath)==KErrNone)
			strcpy(debugfile,logpath);
		else
			{
			DWORD len = GetEnvironmentVariableA("EmulatorLog", debugfile, MAX_PATH);
			debugfile[len]=0;
			if (len == 0)
				{
				len=GetTempPathA(MAX_PATH,debugfile);
				strcpy(debugfile+len,"epocwind.out");	// EPOC WINS DEBUG output file
				}
			}
		file=CreateFileA(debugfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
		if (file!=INVALID_HANDLE_VALUE)
			{
			SetFilePointer(file, NULL, NULL, FILE_END);
			iDebugOutput = file;
			}
		}
	return file;
	}

const TInt MaxOutputMsg = 599;

void Wins::EarlyLogging(const char* aMessage1,const char* aMessage2)
	{
	char message[MaxOutputMsg+3];
	TInt len = min(strlen(aMessage1), MaxOutputMsg);
	memcpy(message,aMessage1,len);
	if(aMessage2)
		{
		TInt len2 = min((TInt)strlen(aMessage2), MaxOutputMsg-len);
		memcpy(message+len,aMessage2,len2);
		len+=len2;
		}
	message[len++] = '\r';
	message[len++] = '\n';
	message[len] = 0;

	if (iLogToFile)
		{
		DWORD bytes;
		WriteFile(DebugOutput(), message, len, &bytes, NULL);
		}
	if (iLogToDebugger)
		{
		OutputDebugStringA(message);
		}
	}

void Wins::DebugPrint(const TDesC8& aDes)
//
// Send the string to the debug output (Win32 debug trace) and trace file
//
	{
	char message[MaxOutputMsg+1];
	TInt len = aDes.Length();
	const char* ptr = (const char*)aDes.Ptr();
	if (iLogTimeStamp)
		{
		strcpy(message,"          ");
		_ultoa(NKern::TickCount() * iTimer.Period(), message + 10, 10);
		int n = strlen(message);
		len = min(len, MaxOutputMsg - n - 2);
		char* msg = message + n;
		msg[0] = msg[-1];
		msg[-1] = n < 10+2 ? '0' : msg[-2];
		msg[-2] = n < 10+3 ? '0' : msg[-3];
		msg[-3] = '.';
		if (n < 10+4)
			msg[-4] = '0';
		++msg;
		*msg++ = '\t';
		strncpy(msg, ptr, len);
		msg[len] = 0;
		ptr = msg - 11;
		len += 11;
		}
	else
		{
		len = min(len, MaxOutputMsg);
		strncpy(message, ptr, len);
		message[len] = 0;
		ptr = message;
		}
	TInt irq = NKern::DisableAllInterrupts();
	if (iLogToFile)
		{
		DWORD bytes;
		WriteFile(DebugOutput(), ptr, len, &bytes, NULL);
		}
	if (iLogToDebugger)
		{
		OutputDebugStringA(message);
		}

	NKern::RestoreInterrupts(irq);
	}

const char* const KErrorTitles[] =
	{
	"Symbian OS Fatal Error",
	"Symbian OS Application Error"
	};

const TText8* const KErrorMsg[] =
	{
	_S8("An error has been detected in the Symbian OS emulator."),
	_S8("A call to User::Panic() has occured, indicating\n"
		"a programming fault in the running application.\n"
		"Please refer to the documentation for more details.")
	};

_LIT8(KProgram, "\n\nProgram\t");
_LIT8(KError, "\nError\t");
_LIT8(KIDFC, "an IDFC");
_LIT8(KEscaped, "an escaped thread");
_LIT8(KInterrupt, "an interrupt thread");
_LIT8(KNThread, "an NThread");
_LIT8(KColon, " : ");
_LIT8(KDebugQuery, "\n\nDo you wish to Debug the error?\0");

TBool Wins::ErrorDialog(TError aType, const TDesC8& aPanic, TInt aVal)
	{
	// Must be called with interrupts enabled to allow thread running windows message loop to run
	
	TBuf8<512> message(KErrorMsg[aType]);
	message.Append(KProgram);
	TInt context = NKern::CurrentContext();
	switch(context)
		{
		case NKern::EIDFC:
			message.Append(KIDFC);
			break;
		case NKern::EEscaped:
			message.Append(KEscaped);
			break;
		case NKern::EInterrupt:
			message.Append(KInterrupt);
			break;
		case NKern::EThread:
			DThread *thread = Kern::NThreadToDThread(NKern::CurrentThread());
			if (thread)
				thread->TraceAppendFullName(message, ETrue);
			else
				message.Append(KNThread);
			break;
		}
	message.Append(KError);
	message.Append(aPanic);
	message.Append(KColon);
	message.AppendNum(aVal);
#ifdef _DEBUG
	message.Append(KDebugQuery);
	UINT type = MB_YESNO | MB_DEFBUTTON2;
#else
	UINT type = MB_OK;
#endif
	type |= MB_SETFOREGROUND  | MB_ICONERROR;
	message.Append('\0');

	TInt r = MessageBoxA(iUi ? iUi->HWnd() : NULL, (LPCSTR)message.Ptr(), KErrorTitles[aType], type);
	return r == IDYES;
	}

// UI installation

EXPORT_C DWinsUiBase::DWinsUiBase()
	{
	TheVariant.InstallUi(*this);
	}


TInt BinaryPowerInit();

DWinsPowerController* DWinsPowerController::New()
	{
	DWinsPowerController* self = new DWinsPowerController();
	if (!self)
		return NULL;
	self->iStandbySem = CreateSemaphore(NULL, 0, 1, NULL);
	if (self->iStandbySem == NULL)
		return NULL;
	TInt r = BinaryPowerInit();
	if (r != KErrNone)
		return NULL;
	self->Register();
	return self; 
	}

void DWinsPowerController::CpuIdle()
	{
	Arch::TheAsic()->Idle();
	}
		
void DWinsPowerController::EnableWakeupEvents()
	{
	iWakeupSignal = EFalse;
	}

void DWinsPowerController::DisableWakeupEvents()
	{
	}

void DWinsPowerController::AbsoluteTimerExpired()
	{
	if (iTargetState == EPwStandby)
		DWinsPowerController::WakeupEvent();	
	}

void DWinsPowerController::WakeupEvent()
	{
	if (iTargetState == EPwStandby)
		{
		iWakeupSignal = ETrue;
		DPowerController::WakeupEvent();
		}
	}

// called in Epoc thread
void DWinsPowerController::PowerDown(TTimeK aWakeupTime)
	{
	if (iTargetState == EPwStandby)
		{
		UINT timeoutMs;
		if (aWakeupTime == 0)
			timeoutMs = INFINITE;
		else 
			{
			TTimeK now = Kern::SystemTime();
			if (now > aWakeupTime)
				timeoutMs = 0;
			else
				timeoutMs = (UINT)((aWakeupTime - now) / 1000);
			}
		TInt l = NKern::DisableAllInterrupts();
		if (!iWakeupSignal && timeoutMs)
			{
			iStandby = ETrue;
			TheVariant.iTimer.Standby();
			NKern::RestoreInterrupts(l);
			DWORD r = WaitForSingleObject(iStandbySem, timeoutMs);
			if (r == WAIT_TIMEOUT)
				{
				l = NKern::DisableAllInterrupts();
				if (!iStandby)
					WaitForSingleObject(iStandbySem, INFINITE);
				else
					iStandby = EFalse;
				NKern::RestoreInterrupts(l);
				}
			TheVariant.iTimer.Wakeup();
			}
		else
			NKern::RestoreInterrupts(l);

		}
	else
		Kern::Restart(0x80000000);
	}

// called in the interrupt context
void DWinsPowerController::AssertWakeupSignal()
	{
	iWakeupSignal = ETrue;
	if (iStandby)
		{
		iStandby = EFalse;
		ReleaseSemaphore(iStandbySem, 1, NULL);
		}
	}


EXPORT_C void Wins::AssertWakeupSignal()
	{
	iPowerController->AssertWakeupSignal();
	}

EXPORT_C void Wins::WakeupEvent()
	{
	iPowerController->DWinsPowerController::WakeupEvent();
	}

// MMC emulation support

TBool Wins::MediaDoorOpen;
TInt Wins::CurrentPBusDevice;
TAny* Wins::MediaChangeCallbackParam;
TMediaChangeCallBack Wins::MediaChangeCallBackPtr;


EXPORT_C TBool* Wins::MediaDoorOpenPtr()
//
// For media change simulation
//
	{

	return(&MediaDoorOpen);
	}

EXPORT_C TInt* Wins::CurrentPBusDevicePtr()
//
// For media change simulation
//
	{

	return(&CurrentPBusDevice);
	}

EXPORT_C void Wins::SetMediaChangeCallBackPtr(TMediaChangeCallBack aPtr, TAny* aMediaChangeCallbackParam)
//
// For media change simulation
//
	{
	MediaChangeCallbackParam=aMediaChangeCallbackParam;
	MediaChangeCallBackPtr=aPtr;
	}

EXPORT_C void Wins::MediaChangeCallBack()
//
// Perform the simulated media change callback
//
	{
	if(MediaChangeCallBackPtr)
		(*MediaChangeCallBackPtr)(MediaChangeCallbackParam);
	}
