// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\inc\variant.h
// Wins Variant Header
// 
//

#ifndef __VA_STD_H__
#define __VA_STD_H__

#include <assp.h>
#include "vwins.h"
#include "nk_priv.h"
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#include <mmsystem.h>
#pragma warning(default : 4201)
#include <kernel/kpower.h>

#ifdef _DEBUG
const TInt KDefaultDebugMask = 1<<KPANIC; //KPANIC, override this with "DebugMask 0" in EPOC.INI
#else
const TInt KDefaultDebugMask = 0;
#endif

typedef void (*TMediaChangeCallBack)(TAny*);

class WinsTimer
	{
public:
	enum
		{
		EDefaultPeriod = 5,		// milliseconds
		EMaxLag = 100,			// milliseconds
		};
public:
	WinsTimer();
	void Init(TUint aPeriod);
	void Enable();
	inline TUint Period() const
		{return iPeriod;}
	inline void SetIdleThread()
		{if (!iIdleThread) iIdleThread = NKern::CurrentThread();}
	inline void Nobble(TUint aNobbleFactor)
		{iNobbleNanos = aNobbleFactor * iPeriod;}
	TInt SystemTime() const;
	void SetSystemTime(TInt aTime);
	void Standby();
	void Wakeup();
private:
	void EventThread(NTimerQ& aTimerQ);
	static DWORD WINAPI Thread(LPVOID);
	static void CALLBACK Tick(UINT, UINT, DWORD, DWORD, DWORD);
private:
	TUint iPeriod;
	TUint iNobbleNanos;
	TInt iMaxLagTicks;
	HANDLE iSemaphore;
	UINT iTimer;
	TInt iSuspend;
	Int64 iTime;
	NThread* iIdleThread;
	TBool iStandby;
	};

class Properties
	{
	struct SEntry
		{
		char* iProperty;
		char* iValue;
		};
public:
	Properties();
	const char* Append(const char* aProperty, const char* aValue);
	const char* Replace(const char* aProperty, const char* aValue);
	TInt GetString(const char* aProperty, const char*& aValue) const;
	TInt GetInt(const char* aProperty, TInt& aValue) const;
	TInt GetBool(const char* aProperty, TBool& aValue, TBool aDefaultValue=EFalse) const;
	//
	void Dump() const;
private:
	TInt Find(const char* aProperty) const;
	const char* Insert(TInt aIndex, const char* aProperty, const char* aValue);
private:
	SEntry* iEntries;
	TInt iCount;
	TInt iSize;
	};

class Wins : public Asic
	{
public:
	Wins();
	//
	TInt Init(TBool aRunExe);
	inline void InstallUi(DWinsUiBase& aUi);
	TInt EmulatorHal(TInt aFunction, TAny* a1, TAny* a2);
private:
	// initialisation
	void Init1();
	void Init3();
	void AddressInfo(SAddressInfo& anInfo);

	// power management
	void Idle();

	// timing
	TInt MsTickPeriod();
	TInt SystemTimeInSecondsFrom2000(TInt& aTime);
	TInt SetSystemTimeInSecondsFrom2000(TInt aTime);

	// HAL
	TInt VariantHal(TInt aFunction, TAny* a1, TAny* a2);

	// Machine configuration
	TPtr8 MachineConfiguration();
	const char* EmulatorMediaPath();

	// Debug trace
	void DebugPrint(const TDesC8& aDes);
	TBool ErrorDialog(TError aType, const TDesC8& aPanic, TInt aVal);
	void EarlyLogging(const char* aMessage1,const char* aMessage2);
private:
	TInt InitProperties(TBool aRunExe);
	TInt DebugMask();
	TUint32 KernelConfigFlags();
	TInt ParseCapabilitiesArg(SCapabilitySet& aCapabilities, const char *aText);
	void DisabledCapabilities(SCapabilitySet& aCapabilities);
	void CalibrateCpuSpeed();
	TInt SetCpuSpeed(TUint aSpeed);
	TInt AddProperty(char* aProperty, const char* aEol);
	TInt ProcessCommandLine(TBool aRunExe, char* aCDrive);
	TInt LoadProperties();
	TInt ReadProperties(char* aData);
	TInt ReadIniFile(const char* aFileName, char*& aContents);
	TInt SetupPaths();
	TInt SetupMediaPath();
	TInt SetupDrive(int aDrive, const char* aPath);
	TInt MapFilename(const TDesC& aFilename, TDes& aBuffer) const;
	TInt MapDrive(int aDrive, TDes& aBuffer) const;
	HANDLE DebugOutput();
	void PurgeImages();
	TBool ConfigSpecificProperty(const char * aProperty);
	TInt LoadConfigSpecificProperties(const char * pFile);
	TBool ScreenSpecificProperty(const char * aProperty);

private:
	friend class DWinsPowerController;
	DWinsUiBase* iUi;
	WinsTimer iTimer;
	Properties iProperties;
	TUint iRealCpuSpeed;
	TUint iCpuSpeed;
	HANDLE iDebugOutput;
	TBool iLogTimeStamp;
	TBool iPurgedImages;
	TBool iLogToDebugger;
	TBool iLogToFile;
	TBool iConfigPropertySet;
	TInt iConfigId;

	class DWinsPowerController* iPowerController;
	// For MMC emulation
	static TBool MediaDoorOpen;
	static TInt CurrentPBusDevice;
	static TAny* MediaChangeCallbackParam;
	static TMediaChangeCallBack MediaChangeCallBackPtr;
public:
	static Wins* Self()
		{ return (Wins*) Arch::TheAsic();}
	IMPORT_C void AssertWakeupSignal();
	IMPORT_C void WakeupEvent();
	IMPORT_C static TBool* MediaDoorOpenPtr();
	IMPORT_C static TInt* CurrentPBusDevicePtr();
	IMPORT_C static void SetMediaChangeCallBackPtr(TMediaChangeCallBack aPtr, TAny* aParam);
	IMPORT_C static void MediaChangeCallBack();
	};

GLREF_D Wins TheVariant;

#endif
