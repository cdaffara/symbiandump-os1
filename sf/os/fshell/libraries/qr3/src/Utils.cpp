// Utils.cpp
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
#define __INCLUDE_ALL_SUPPORTED_CAPABILITIES__
#define __INCLUDE_CAPABILITY_NAMES__

#include <e32base.h>
#include "Utils.h"
#include <HAL.h>
#include <eikenv.h>

#ifdef INCLUDE_UIQ_DEFINITIONS
#include <DeviceKeys.h>
#endif

#include <fshell/memoryaccess.h>
#include <fshell/ltkutils.h>

/* TomS: These don't seem to be used any more
//#include <featureUIDs.h>
namespace NFeature
	{
	// default present
	const TUid KFax = {0x10279806};
	const TUid KPrint = {0x10279807};
	const TUid KBluetooth = {0x10279808};
	const TUid KInfrared = {0x10279809};
	const TUid KMmc = {0x1027980a};
	const TUid KUsb = {0x1027980b};
	const TUid KObex = {0x1027980c};
	const TUid KRtpRtcp = {0x1027980d};
	const TUid KSip = {0x1027980f};
	const TUid KOmaDataSync	= {0x10279810};
	const TUid KOmaDeviceManagement = {0x10279811};
	const TUid KIPQoS = {0x10279812};
	const TUid KNetworkQoS = {0x10279813};
	const TUid KIPSec = {0x10279814};
	const TUid KDhcp = {0x10279815};
	const TUid KConnectivity = {0x10279816};	// PC Connectivity

	// default not-present
	const TUid KLocation = {0x10281818};
	const TUid KMobileIP = {0x10281819};
	const TUid KOfflineMode = {0x1028181A};
	const TUid KDRM = {0x1028181B};
	const TUid KOmaDsHostServers = {0x10282663};
	}
*/

#ifdef __DLL__
#define gPlugin ((CDefaultProductPlugin*)Dll::Tls())
#else
CDefaultProductPlugin* gPlugin = NULL;
#endif

// GCC has issues with the CASE_LIT definition that ran along the same lines as CASE_LIT2 below - more than 8 CASE_LITs in one switch made the compiler blow up
#define CASE_LIT(name) case name: { const TText16* KName = (const TText16*) L ## #name; return TPtrC(KName); }
#define CASE_LIT2(val, name) case val: { _LIT(KName, name); return TPtrC(KName); }


void HR(TDes& aDes, TInt64 aSize)
	{
	// This functionality has been moved into ltkutils
	LtkUtils::FormatSize(aDes, aSize);
	}

void HR(TDes8& aDes, TInt64 aSize)
	{
	LtkUtils::FormatSize(aDes, aSize);
	}

TPtrC ToString(TProcessPriority aPriority)
	{
	switch(aPriority)
		{
		CASE_LIT(EPriorityLow);
		CASE_LIT(EPriorityBackground);
		CASE_LIT(EPriorityForeground);
		CASE_LIT(EPriorityHigh);
		CASE_LIT(EPriorityWindowServer);
		CASE_LIT(EPriorityFileServer);
		CASE_LIT(EPriorityRealTimeServer);
		CASE_LIT(EPrioritySupervisor);
		default:
			return TPtrC();
		}
	}

TPtrC ToString(TThreadPriority aPriority)
	{
	// Redefine these here - easiest way of making sure this builds and works on all 9.x baselines
	enum TPrioritiesThatArentIn91 
		{
		EPriorityAbsoluteLowNormal=150,
		EPriorityAbsoluteBackgroundNormal=250,
		EPriorityAbsoluteForegroundNormal=350,
		EPriorityAbsoluteHighNormal=450,
		EPriorityAbsoluteHigh=500,
		EPriorityAbsoluteRealTime1=810,
		EPriorityAbsoluteRealTime2=820,
		EPriorityAbsoluteRealTime3=830,
		EPriorityAbsoluteRealTime4=840,
		EPriorityAbsoluteRealTime5=850,
		EPriorityAbsoluteRealTime6=860,
		EPriorityAbsoluteRealTime7=870, 
		EPriorityAbsoluteRealTime8=880
		};

	switch((TInt)aPriority)
		{
		CASE_LIT(EPriorityNull);
		CASE_LIT(EPriorityMuchLess);
		CASE_LIT(EPriorityLess);
		CASE_LIT(EPriorityNormal);
		CASE_LIT(EPriorityMore);
		CASE_LIT(EPriorityMuchMore);
		CASE_LIT(EPriorityAbsoluteVeryLow);
		CASE_LIT(EPriorityAbsoluteLow);
		CASE_LIT(EPriorityAbsoluteBackground);
		CASE_LIT(EPriorityAbsoluteForeground);
		CASE_LIT(EPriorityAbsoluteHigh);
		CASE_LIT(EPriorityRealTime);
		CASE_LIT(EPriorityAbsoluteLowNormal);
		CASE_LIT(EPriorityAbsoluteBackgroundNormal);
		CASE_LIT(EPriorityAbsoluteForegroundNormal);
		CASE_LIT(EPriorityAbsoluteHighNormal);
		CASE_LIT(EPriorityAbsoluteRealTime1);
		CASE_LIT(EPriorityAbsoluteRealTime2);
		CASE_LIT(EPriorityAbsoluteRealTime3);
		CASE_LIT(EPriorityAbsoluteRealTime4);
		CASE_LIT(EPriorityAbsoluteRealTime5);
		CASE_LIT(EPriorityAbsoluteRealTime6);
		CASE_LIT(EPriorityAbsoluteRealTime7);
		CASE_LIT(EPriorityAbsoluteRealTime8);
		default:
			return TPtrC();
		}
	}

HBufC* ToStringL(SCapabilitySet& aCaps)
	{
	//TODO
	//_LIT8(KUnknown, "Unknown");
	//return KUnknown().AllocL();

	__ASSERT_COMPILE(ECapability_Limit < 32);
	_LIT(KAll, "All");
	_LIT(KAllMinusTCB, "All -Tcb");
	_LIT(KNone, "None");
	if (aCaps[0] == AllSupportedCapabilities[0])
		{
		return KAll().AllocL();
		}
	else if (aCaps[0] == (AllSupportedCapabilities[0] & ~(1 << ECapabilityTCB)))
		{
		return KAllMinusTCB().AllocL();
		}
	else if (aCaps[0] == 0)
		{
		return KNone().AllocL();
		}
	else
		{
		HBufC8* res = HBufC8::NewLC(512); // Easily enough room for all the capability names
		TPtr8 ptr = res->Des();
		for (TInt bit = 0; bit < ECapability_Limit; bit++)
			{
			if (aCaps[0] & (1 << bit))
				{
				ptr.Append(TPtrC8((TUint8*)CapabilityNames[bit]));
				ptr.Append(' ');
				}
			}
		ptr.SetLength(ptr.Length()-1); // Remove trailing space
		HBufC* res16 = HBufC::NewL(ptr.Length());
		res16->Des().Copy(ptr);
		CleanupStack::PopAndDestroy(res);
		return res16;
		}
	}

TPtrC ToString(TExitType aType)
	{
	switch (aType)
		{
		CASE_LIT2(EExitKill, "Kill");
		CASE_LIT2(EExitTerminate, "Terminate");
		CASE_LIT2(EExitPanic, "Panic");
		CASE_LIT2(EExitPending, "Pending");
		default:
			return TPtrC();
		}
	}

TPtrC ToString(User::TCritical aCritical)
	{
	switch (aCritical)
		{
		CASE_LIT2(User::ENotCritical, "Not critical");
		CASE_LIT2(User::EProcessCritical, "Process critical");
		CASE_LIT2(User::EProcessPermanent, "Process permanent");
		CASE_LIT2(User::ESystemCritical, "System critical");
		CASE_LIT2(User::ESystemPermanent, "System permanent");
		default:
			return TPtrC();
		}
	}

// These lines must all be terminated by a blank string, ""
// First item must be the description of the word, and must be <= 16 characters
static char const *const w0[] = /* kernel debugging */ { "Word 0", "khardware", "kboot", "kserver", "kmmu", "ksemaphore", "ksched", "kproc", "kexec", "kdebugger", "kthread", "kdll", "kipc", "kpbus1", "kpbus", "kpbusdrv", "kpower", "ktiming", "kevent", "kobject", "kdfc", "kextension", "ksched2", "klocdrv", "kfail", "kthread2", "kdevice", "kmemtrace", "kdma", "kmmu2", "knkern", "kscratch", "kpanic", "" };
static char const *const w1[] = /* kernel debugging */ { "Word 1", "kusb", "kusbpsl", "knetwork1", "knetwork2", "ksound1", "kusbhost", "kusbotg", "kusbjournal", "kusbho", "kresmanager", "kiic", "khcr", "" };
static char const *const w2[] = /* kernel behaviour */ { "Word 2", "kallthreadssystem", "ktestfast", "ktestlatency", "kdebugmonitordisable", "kcrashmonitordisable", "" };
static char const *const w3[] = /* kernel behaviour */  { "Word 3", "kuserheaptrace", "" };
static char const *const w4[] = /* file server debug*/ { "" };
static char const *const w5[] = /* file server debug*/ { "" };
static char const *const w6[] = /* licensee specific*/ { "Word 6", "Bit 0", "Bit 1", "Bit 2", "Bit 3", "Bit 4", "Bit 5", "Bit 6", "Bit 7", "Bit 8", "Bit 9", "Bit 10", "Bit 11", "Bit 12", "Bit 13", "Bit 14", "Bit 15", "Bit 16", "Bit 17", "Bit 18", "Bit 19", "Bit 20", "Bit 21", "Bit 22", "Bit 23", "Bit 24", "Bit 25", "Bit 26", "Bit 27", "Bit 28", "Bit 29", "Bit 30", "Bit 31", "" };
static char const *const w7[] = /* licensee specific*/ { "Word 7", "Bit 0", "Bit 1", "Bit 2", "Bit 3", "Bit 4", "Bit 5", "Bit 6", "Bit 7", "Bit 8", "Bit 9", "Bit 10", "Bit 11", "Bit 12", "Bit 13", "Bit 14", "Bit 15", "Bit 16", "Bit 17", "Bit 18", "Bit 19", "Bit 20", "Bit 21", "Bit 22", "Bit 23", "Bit 24", "Bit 25", "Bit 26", "Bit 27", "Bit 28", "Bit 29", "Bit 30", "Bit 31", "" };
static char const *const fs[] = /* file server debug*/ { "File server", "kfsys", "kfserv", "kfldr", "kalloc", "klffs", "kiso9660", "kntfs", "krofs", "kthrd", "kcache", "kcompfs", ""};

static char const*const*const KKTraceFlags[] = { w0, w1, w2, w3, w4, w5, w6, w7, fs };

TPtrC8 TraceFlagName(TInt aWord, TInt aBit)
	{
	return gPlugin->TraceFlagName(aWord, aBit);
	}

TPtrC8 CDefaultProductPlugin::TraceFlagName(TInt aWord, TInt aBit)
	{
	if (iPluginForThisDevice)
		{
		return iPluginForThisDevice->TraceFlagName(aWord, aBit);
		}

	if (aBit == 0 && *KKTraceFlags[aWord][aBit] == 0)
		{
		// Special-case this because the caller expects to be able to test whether the first bit name is empty to decide whether the word is empty, and our data structure doesn't let you do that any more
		return TPtrC8();
		}

	aBit++; // Because we use -1 to mean the word title, and the bits go from 0-32, whereas in the array the title is at index 0 and the bits go from 1 to 33
	return TPtrC8((const TUint8*)KKTraceFlags[aWord][aBit]);
	}

// Format is tag name, followed by the names of the bits, starting from bit 0. Terminate with a "" and remember you only have 32 bits to play with!
//const char* qres[] = { "QResources3", "Bit0", "Bit1", "Bit2", "" };
//const char* sentinel[] = {"Sentinel", "MemoryEvent", "AppSwitch", "QMemoryManager", "" };
static char const *const clogger[] = {"Clogger", "Log SetEnabled", "Log new tags", "Log disks", "Log new rdebuggers", ""};
static char const *const kernel[] = {"Kern::Printf", "<unused>", "Kern::Printf", "Platsec diagnostics", ""};
static char const *const wserv[] = {"Wserv", "Important", "Intermediate", "Everything else", ""};
static char const *const fallback[] = {"", "Bit 0", "Bit 1", "Bit 2", "Bit 3", "Bit 4", "Bit 5", "Bit 6", "Bit 7", "Bit 8", "Bit 9", "Bit 10", "Bit 11", "Bit 12", "Bit 13", "Bit 14", "Bit 15", "Bit 16", "Bit 17", "Bit 18", "Bit 19", "Bit 20", "Bit 21", "Bit 22", "Bit 23", "Bit 24", "Bit 25", "Bit 26", "Bit 27", "Bit 28", "Bit 29", "Bit 30", "Bit 31", "" };

extern char const* const* const KCloggerTags[] = {clogger, kernel, wserv, /*fallback must be the last in the list!*/fallback};

#ifdef __WINS__
#define WCHAR_TYPE TUint16
#else
#define WCHAR_TYPE wchar_t
#endif

WCHAR_TYPE const*const KHalAtrributes[] = {
	L"EManufacturer",
	L"EManufacturerHardwareRev",
	L"EManufacturerSoftwareRev",
	L"EManufacturerSoftwareBuild",
	L"EModel",
	L"EMachineUid",
	L"EDeviceFamily",
	L"EDeviceFamilyRev",
	L"ECPU",
	L"ECPUArch",
	L"ECPUABI",
	L"ECPUSpeed",
	L"ESystemStartupReason",
	L"ESystemException",
	L"ESystemTickPeriod",
	L"EMemoryRAM",
	L"EMemoryRAMFree",
	L"EMemoryROM",
	L"EMemoryPageSize",
	L"EPowerGood",
	L"EPowerBatteryStatus",
	L"EPowerBackup",
	L"EPowerBackupStatus",
	L"EPowerExternal",
	L"EKeyboard",
	L"EKeyboardDeviceKeys",
	L"EKeyboardAppKeys",
	L"EKeyboardClick",
	L"EKeyboardClickState",
	L"EKeyboardClickVolume",
	L"EKeyboardClickVolumeMax",
	L"EDisplayXPixels",
	L"EDisplayYPixels",
	L"EDisplayXTwips",
	L"EDisplayYTwips",
	L"EDisplayColors",
	L"EDisplayState",
	L"EDisplayContrast",
	L"EDisplayContrastMax",
	L"EBacklight",
	L"EBacklightState",
	L"EPen",
	L"EPenX",
	L"EPenY",
	L"EPenDisplayOn",
	L"EPenClick",
	L"EPenClickState",
	L"EPenClickVolume",
	L"EPenClickVolumeMax",
	L"EMouse",
	L"EMouseX",
	L"EMouseY",
	L"EMouseState",
	L"EMouseSpeed",
	L"EMouseAcceleration",
	L"EMouseButtons",
	L"EMouseButtonState",
	L"ECaseState",
	L"ECaseSwitch",
	L"ECaseSwitchDisplayOn",
	L"ECaseSwitchDisplayOff",
	L"ELEDs",
	L"ELEDmask",
	L"EIntegratedPhone",
	L"EDisplayBrightness",
	L"EDisplayBrightnessMax",
	L"EKeyboardBacklightState",
	L"EAccessoryPower",
	L"ELanguageIndex",
	L"EKeyboardIndex",
	L"EMaxRAMDriveSize",
	L"EKeyboardState",
	L"ESystemDrive",
	L"EPenState",
	L"EDisplayIsMono",
	L"EDisplayIsPalettized",
	L"EDisplayBitsPerPixel",
	L"EDisplayNumModes",
	L"EDisplayMemoryAddress",
	L"EDisplayOffsetToFirstPixel",
	L"EDisplayOffsetBetweenLines",
	L"EDisplayPaletteEntry",
	L"EDisplayIsPixelOrderRGB",
	L"EDisplayIsPixelOrderLandscape",
	L"EDisplayMode",
	L"ESwitches",
	L"EDebugPort",
	L"ELocaleLoaded",
	L"EClipboardDrive",
	L"ECustomRestart",
	L"ECustomRestartReason",
	L"EDisplayNumberOfScreens",
	L"ENanoTickPeriod",
	L"EFastCounterFrequency",
	L"EFastCounterCountsUp",
	L"EPointer3D",
	L"EPointer3DZ",
	L"EPointer3DThetaSupported",
	L"EPointer3DPhiSupported",
	L"EPointer3DRotationSupported",
	L"EPointer3DPressureSupported",
	L"EHardwareFloatingPoint",
	L"ECustomResourceDrive",
	};
const TInt KNumHalAttributes = sizeof(KHalAtrributes) / sizeof(TUint16*);

TPtrC ToStringHal(TInt aHalAttribute)
	{
	if (aHalAttribute >= KNumHalAttributes)
		{
		return TPtrC();
		}
	else
		{
		return TPtrC((TUint16*)KHalAtrributes[aHalAttribute]);
		}
	}

void ToStringHalVal(TDes& aDes, TInt aHalAttribute, TInt aHalValue)
	{
	gPlugin->FormatValue(aDes, MProductPlugin::EHal, aHalAttribute, aHalValue);
	}

const TPtrC ToStringUid(TInt aVal)
	{
	switch(aVal)
		{
		CASE_LIT2(0x10279806, "KFax");
		CASE_LIT2(0x10279807, "KPrint");
		CASE_LIT2(0x10279808, "KBluetooth");
		CASE_LIT2(0x10279809, "KInfrared");
		CASE_LIT2(0x1027980a, "KMmc");
		CASE_LIT2(0x1027980b, "KUsb");
		CASE_LIT2(0x1027980c, "KObex");
		CASE_LIT2(0x1027980d, "KRtpRtcp");
		CASE_LIT2(0x1027980f, "KSip");
		CASE_LIT2(0x10279810, "KOmaDataSync");
		CASE_LIT2(0x10279811, "KOmaDeviceManagement");
		CASE_LIT2(0x10279812, "KIPQoS");
		CASE_LIT2(0x10279813, "KNetworkQoS");
		CASE_LIT2(0x10279814, "KIPSec");
		CASE_LIT2(0x10279815, "KDhcp");
		CASE_LIT2(0x10279816, "KConnectivity");
		CASE_LIT2(0x10281818, "KLocation");
		CASE_LIT2(0x10281819, "KMobileIP");
		CASE_LIT2(0x1028181A, "KOfflineMode");
		CASE_LIT2(0x1028181B, "KDRM");
		CASE_LIT2(0x10282663, "KOmaDsHostServers");
		default: return TPtrC();
		}
	}

TPtrC KeyName(TInt aKeyCode);

void CDefaultProductPlugin::FormatValue(TDes& aDes, TValueType aType, TInt aAttrib, TInt aVal)
	{
	aDes.SetLength(0);
	// Give product a chance to handle it
	if (iPluginForThisDevice)
		{
		iPluginForThisDevice->FormatValue(aDes, aType, aAttrib, aVal);
		if (aDes.Length()) return;
		}

	if (aType == MProductPlugin::EHal)
		{
		const TInt aHalAttribute = aAttrib;
		const TInt aHalValue = aVal;
		TBool hex = EFalse;
		switch (aHalAttribute)
			{
			case HAL::EManufacturer:
				{
				switch (aHalValue)
					{
				case HAL::EManufacturer_Ericsson:
					aDes = _L("Ericsson"); return;
				case HAL::EManufacturer_Motorola:
					aDes = _L("Motorola"); return;
				case HAL::EManufacturer_Nokia:
					aDes = _L("Nokia"); return;
				case HAL::EManufacturer_Panasonic:
					aDes = _L("Panasonic"); return;
				case HAL::EManufacturer_Psion:
					aDes = _L("Psion"); return;
				case HAL::EManufacturer_Intel:
					aDes = _L("Intel"); return;
				case HAL::EManufacturer_Cogent:
					aDes = _L("Cogent"); return;
				case HAL::EManufacturer_Cirrus:
					aDes = _L("Cirrus"); return;
				case HAL::EManufacturer_Linkup:
					aDes = _L("Linkup"); return;
				case HAL::EManufacturer_TexasInstruments:
					aDes = _L("TI"); return;
				default:
					hex = ETrue;
					break;
					}
				break;
				}
			case HAL::EMemoryRAM:
			case HAL::EMemoryRAMFree:
			case HAL::EMemoryROM:
			case HAL::EMemoryPageSize:
			case HAL::EMaxRAMDriveSize:
				HR(aDes, aHalValue);
				return;
			case HAL::ECPU:
				switch (aHalValue)
					{
					case HAL::ECPU_ARM:
						aDes = _L("ARM"); return;
					case HAL::ECPU_MCORE:
						aDes = _L("MCORE"); return;
					case HAL::ECPU_X86:
						aDes = _L("x86"); return;
					default:
						break;
					}
				break;
			case HAL::EModel:
			case HAL::EMachineUid:
			case HAL::ELEDmask:
			case HAL::EDisplayMemoryAddress:
				hex = ETrue;
				break;
			case HAL::EDeviceFamily:
				hex = ETrue;
				break;
			case HAL::ESystemDrive:
			case HAL::EClipboardDrive:
				if (aHalValue == -1)
					{
					// Emulator returns -1 for system drive...
					aDes = _L("Unknown");
					return;
					}
				aDes.SetLength(1);
				aDes[0] = 'A' + aHalValue;
				return;
			default:
				break;
			}

		// Fall back if nothing recognised it
		if (hex)
			{
			aDes.Format(_L("0x%x"), aHalValue);
			}
		else
			{
			aDes.Format(_L("%i"), aHalValue);
			}
		} 
	else if (aType == MProductPlugin::EKey)
		{
		const TInt aKeyCode = aVal;
		TPtrC name = KeyName(aKeyCode);
		if (name.Length())
			{
			aDes = name;
			}
		else
			{
			if ((aKeyCode >= 'a' && aKeyCode <= 'z') || (aKeyCode >= '0' && aKeyCode <= '9') || aKeyCode == '*' || aKeyCode == '#')
				{
				aDes.Append((TChar)aKeyCode);
				}
			else
				{
				aDes.AppendFormat(_L("Keycode 0x%x"), aKeyCode);
				}
			}
		}
	else if (aType == MProductPlugin::EFeature)
		{
		aDes = ToStringUid(aAttrib);
		}
	}

void PrettyName(TInt aType, TDes& aName)
	{
	switch (aType)
		{
		case EListProcess:
			{
			// name is too long, lose the uid
			TInt open = aName.LocateReverse('[');
			TInt close = aName.LocateReverse(']');
			if (open != KErrNotFound && close > open)
				{
				aName.Delete(open, close-open+1);
				}
			// Lose the nonce if its 0001
			_LIT(KOne, "0001");
			if (aName.Right(4) == KOne)
				{
				aName.SetLength(aName.Length() - 4);
				}
			else
				{
				_LIT(KSeparator, " ");
				aName.Insert(aName.Length() - 4, KSeparator);
				}
			break;
			}
		case EListChunk:
			{
			// name is too long, as it can be based on thread fullname, ie procname[uid]nonce::threadname::$STK
			// lose the uid and the nonce
			TInt open = aName.Locate('[');
			TInt colon = aName.Locate(':');
			if (open != KErrNotFound && colon > open)
				{
				aName.Delete(open, colon-open);
				}
			break;
			}
		case EListThread:
			{
			// Algorithm is:
			// Given a aName xxx(.exe)?\[uid\]nonce::(!)?yyy
			// if yyy == xxx (case insensitive compare) then just display yyy (with exclamation mark if present)
			// else display xxx::yyy

			// Find the :: that splits thread and process name
			_LIT(KDoubleColon, "::");
			TInt colon = aName.Find(KDoubleColon);
			if (colon == KErrNotFound)
				{
				// It's not a thread name
				return;
				}

			// Remove .exe if present immediately before [uid]
			TInt brak = aName.Left(colon).LocateReverse('[');
			_LIT(KExe, ".exe");
			if (brak >= 5 && aName.Mid(brak-4, 4).CompareF(KExe) == 0)
				{
				aName.Delete(brak-4, 4);
				brak -= 4;
				colon -= 4;
				}

			TPtrC procaName = brak >=0 ? aName.Left(brak) : aName.Left(colon);
			TPtrC threadaName = aName.Mid(colon+2);
			TPtrC realThreadaName = threadaName.Length() && threadaName[0] == '!' ? threadaName.Mid(1) : threadaName;
			if (procaName.CompareF(realThreadaName) == 0)
				{
				aName.Delete(0, colon+2);
				}
			else if (brak >= 0)
				{
				aName.Delete(brak, colon-brak);
				}
			break;
			}
		case EListWindowGroups:
			{
			// Window group names aren't supposed to be proper strings, so make them friendlier
			TInt nullPos = aName.Locate(TChar(0));
			while (nullPos >= 0)
				{
				aName[nullPos] = '.';
				nullPos = aName.Locate(TChar(0));
				}
			break;
			}
		default:
			break;
		}
	}

#ifdef FSHELL_SAMPLINGPROFILER_SUPPORT
#include <profiler.h>

void StartProfilerL()
	{
	_LIT(KProfiler,"Profiler.exe");
	_LIT(KStartnoui,"start -noui");
	TInt err = Profiler::Start();
	if (err == KErrNotFound)
		{
		RProcess p;
		err = p.Create(KProfiler, KStartnoui); 
		User::LeaveIfError(err);
		p.Resume();
		p.Close();
		}
	User::LeaveIfError(err);
	}

TBool IsProfilerRunning()
	{
	_LIT(KProfiler,"Profiler.exe*");

	TBool running = EFalse;
	TFindProcess f(KProfiler);
	TFullName n;
	while (f.Next(n) == KErrNone)
		{
		// Have to iterate all the Profiler RProcesses to see if any of em are still running
		RProcess p;
		if (p.Open(f) == KErrNone)
			{
			TExitType exitType = p.ExitType();
			p.Close();
			if (exitType == EExitPending)
				{
				running = ETrue;
				break;
				}
			}
		}
	return running;
	}

void StopProfiler()
	{
	Profiler::Unload();
	}

#else
void StartProfilerL()
	{
	User::Leave(KErrNotSupported);
	}

TBool IsProfilerRunning()
	{
	return EFalse;
	}

void StopProfiler()
	{
	}
#endif
	
TUint CriticalToFlags(User::TCritical aCrit)
	{
	switch (aCrit)
		{
		case User::ESystemPermanent:
			return KThreadFlagSystemPermanent;
		case User::ESystemCritical:
			return KThreadFlagSystemCritical;
		case User::EProcessPermanent:
			return KThreadFlagProcessPermanent;
		case User::EProcessCritical:
			return KThreadFlagProcessCritical;
		default:
			return 0;
		}
	}

User::TCritical FlagsToCritical(TUint aFlags)
	{
	aFlags = aFlags & (KThreadFlagSystemPermanent | KThreadFlagSystemCritical | KThreadFlagProcessPermanent | KThreadFlagProcessCritical);
	switch (aFlags)
		{
		case KThreadFlagSystemPermanent:
			return User::ESystemPermanent;
		case KThreadFlagSystemCritical:
			return User::ESystemCritical;
		case KThreadFlagProcessPermanent:
			return User::EProcessPermanent;
		case KThreadFlagProcessCritical:
			return User::EProcessCritical;
		default:
			return User::ENotCritical;
		}
	}

TUint32 CDefaultProductPlugin::GetDeviceType()
	{
	TUint32 res = 0; // EUnknownDevice;
	if (iPluginForThisDevice)
		{
		res = iPluginForThisDevice->GetDeviceType();
		}
	return res;
	}

TBool CDefaultProductPlugin::HandleErrorL(MProductPlugin::TError aType, TInt aErr)
	{
	TBool handled = EFalse;
	if (iPluginForThisDevice)
		{
		handled = iPluginForThisDevice->HandleErrorL(aType, aErr);
		}
	return handled;
	}

TKeyResponse CDefaultProductPlugin::OfferKeyEventL(TKeyEvent& aEvent, TEventCode aType)
	{
	if (iPluginForThisDevice)
		{
		return iPluginForThisDevice->OfferKeyEventL(aEvent, aType);
		}
	return EKeyWasNotConsumed;
	}

TBool CDefaultProductPlugin::DoDynInitOrDeleteCommandL(CQikCommand*& aCommand, const CCoeControl& aControlAddingCommands)
	{
	if (iPluginForThisDevice)
		{
		return iPluginForThisDevice->DoDynInitOrDeleteCommandL(aCommand, aControlAddingCommands);
		}
	return EFalse;
	}

void CDefaultProductPlugin::RefreshTabL(TInt aTab)
	{
	if (iPluginForThisDevice)
		{
		iPluginForThisDevice->RefreshTabL(aTab);
		}
	}

void CDefaultProductPlugin::LoadProductPluginL()
	{
	ASSERT(!gPlugin);
	
	CDefaultProductPlugin* plugin = new(ELeave) CDefaultProductPlugin;
	plugin->ConstructL();

#ifdef __DLL__
	Dll::SetTls(plugin);
#else
	gPlugin = plugin;
#endif
	}

void CDefaultProductPlugin::EnablePanicChecking(TBool aEnable)
	{
	if (iPluginForThisDeviceVersion > 1)
		{
		static_cast<MProductPluginV2*>(iPluginForThisDevice)->EnablePanicChecking(aEnable);
		}
	}

TBool CDefaultProductPlugin::PanicCheckingIsEnabled()
	{
	if (iPluginForThisDeviceVersion > 1)
		{
		return static_cast<MProductPluginV2*>(iPluginForThisDevice)->PanicCheckingIsEnabled();
		}
	return EFalse;
	}

void CDefaultProductPlugin::DisablePanicCheckPushL()
	{
	if (PanicCheckingIsEnabled())
		{
		CleanupStack::PushL(TCleanupItem(&EnablePanicCheck, this));
		EnablePanicChecking(EFalse);
		}
	else
		{
		// This is a no-op cleanup item, since nothing needs doing if panic checks aren't on in the first place
		CBase* nullItem = NULL;
		CleanupStack::PushL(nullItem);
		}
	}

void CDefaultProductPlugin::EnablePanicCheck(TAny* aSelf)
	{
	static_cast<CDefaultProductPlugin*>(aSelf)->EnablePanicChecking(ETrue);
	}

NONSHARABLE_CLASS(CEcomWatcher) : public CActive
	{
public:
	CEcomWatcher(REComSession& aSession, CDefaultProductPlugin& aSomething)
		: CActive(EPriorityStandard), iSession(aSession), iSomething(aSomething)
		{
		CActiveScheduler::Add(this);
		iSession.NotifyOnChange(iStatus);
		SetActive();
		}

	~CEcomWatcher() { Cancel();	}

private:
	void RunL()
		{
		if (iStatus == KErrNone)
			{
			iSession.NotifyOnChange(iStatus);
			SetActive();
			}
		iSomething.EcomChangedL();
		}

	void DoCancel() { iSession.CancelNotifyOnChange(iStatus); }

private:
	REComSession& iSession;
	CDefaultProductPlugin& iSomething;
	};

void CDefaultProductPlugin::ConstructL()
	{
	//gPlugin->iPluginForThisDevice = CZebraPlugin::NewL();
	//gPlugin->iPlugins.AppendL(gPlugin->iPluginForThisDevice);

#ifndef __DLL__
	// When running as a DLL, we don't want to mess about loading plugins
	iEcom = REComSession::OpenL();
	iEcomChangeWatcher = new(ELeave) CEcomWatcher(iEcom, *this);
	RescanPluginsL();
#endif
	}

void CDefaultProductPlugin::DestroyPlugins()
	{
	for (TInt i = 0; i < iPlugins.Count(); i++)
		{
		iPlugins[i]->Release(iShuttingDown);
		}
	iPlugins.Reset();

	for (TInt i = 0; i < iPluginDestructors.Count(); i++)
		{
		REComSession::DestroyedImplementation(iPluginDestructors[i]);
		}
	iPluginDestructors.Reset();
	iEcomPluginList.ResetAndDestroy();
	iPluginForThisDevice = NULL;
	iPluginForThisDeviceVersion = 0;
	}

void CDefaultProductPlugin::RescanPluginsL()
	{
	DestroyPlugins();
	TUid KUid = {KQr3ProductPluginEcomUid};
	REComSession::ListImplementationsL(KUid, iEcomPluginList);
	for (TInt i = 0; i < iEcomPluginList.Count(); i++)
		{
		TRAP_IGNORE(LoadPluginL(iEcomPluginList[i]->ImplementationUid(), iEcomPluginList[i]->Version()));
		// One dodgy plugin shouldn't prevent everything else loading
		}
	}

void CDefaultProductPlugin::LoadPluginL(TUid aImplUid, TInt aVersion)
	{
	//__DEBUGGER();
	TUid uid;
	MProductPlugin* plugin = reinterpret_cast<MProductPlugin*>(REComSession::CreateImplementationL(aImplUid, uid));
	// OK so yes it would be easier to keep the destructor key with the plugin...
	TInt err = iPlugins.Append(plugin);
	if (!err)
		{
		err = iPluginDestructors.Append(uid);
		if (err)
			{
			// need to remove from iPlugins so that iPlugins and iPluginDestructors are in sync
			iPlugins.Remove(iPlugins.Count()-1);
			}
		}
	if (err)
		{
		plugin->Release(EFalse);
		REComSession::DestroyedImplementation(uid);
		User::Leave(err);
		}
		
	TUint32 devType = plugin->GetDeviceType();
	TUint32 currentDevType = iPluginForThisDevice ? iPluginForThisDevice->GetDeviceType() : 0;
	if (devType > currentDevType)
		{
		iPluginForThisDevice = plugin;
		iPluginForThisDeviceVersion = aVersion;
		if (iParentView)
			{
			plugin->DoViewConstructL(iParentView);
			}
		}
	}

void CDefaultProductPlugin::EcomChangedL()
	{
	// Remove and reread all plugins
	RescanPluginsL();
	//PluginsChangedL(ETrue);
	}

CDefaultProductPlugin::~CDefaultProductPlugin()
	{
	//__DEBUGGER();
	DestroyPlugins();
	delete iEcomChangeWatcher;
	iEcom.Close();
	}

void CDefaultProductPlugin::DoViewConstructL(CQikMultiPageViewBase* aParentView)
	{
	iParentView = aParentView;
	if (iPluginForThisDevice)
		{
		iPluginForThisDevice->DoViewConstructL(aParentView);
		}
	}

void CDefaultProductPlugin::DoHandleControlEventL(CCoeControl *aControl, MCoeControlObserver::TCoeEvent aEventType)
	{
	if (iPluginForThisDevice)
		{
		iPluginForThisDevice->DoHandleControlEventL(aControl, aEventType);
		}
	}

TBool CDefaultProductPlugin::ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aDownKeyCode, TInt& aLogKeyCode)
	{
	TInt upscan, downscan, logscan;
	return ConsumeMemoryKeys(aDisk, aCheckboxName, aUpKeyCode, upscan, aDownKeyCode, downscan, aLogKeyCode, logscan);
	}

TBool CDefaultProductPlugin::ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aUpScanCode, TInt& aDownKeyCode, TInt& aDownScanCode, TInt& aLogKeyCode, TInt& aLogScanCode)
	{
	// Set up defaults here. Device plugins are welcome to modify them
	TBool display = ETrue;
	if (aDisk)
		{
		aCheckboxName = _L("Use 2-way nav to consume disk");
		}
	else
		{
		aCheckboxName = _L("Use 2-way nav to consume RAM");
		}
#ifdef INCLUDE_UIQ_DEFINITIONS
	aUpKeyCode = EDeviceKeyTwoWayUp;
	aUpScanCode = EStdDeviceKeyTwoWayUp;
	aDownKeyCode = EDeviceKeyTwoWayDown;
	aDownScanCode = EStdDeviceKeyTwoWayDown;
	aLogKeyCode = EDeviceKeyAction;
	aLogScanCode = EStdDeviceKeyAction;
#else
	aUpKeyCode = EKeyUpArrow;
	aUpScanCode = EStdKeyUpArrow;
	aDownKeyCode = EKeyDownArrow;
	aDownScanCode = EStdKeyDownArrow;
	aLogKeyCode = EKeyEnter;
	aLogScanCode = EStdKeyEnter;
#endif
	
	if (iPluginForThisDevice && iPluginForThisDeviceVersion > 3)
		{
		// Supports the scancode-aware interface
		display = static_cast<MProductPluginV4*>(iPluginForThisDevice)->ConsumeMemoryKeys(aDisk, aCheckboxName, aUpKeyCode, aUpScanCode, aDownKeyCode, aDownScanCode, aLogKeyCode, aLogScanCode);
		}
	else if (iPluginForThisDevice)
		{
		aUpScanCode = 0;
		aDownScanCode = 0;
		aLogScanCode = 0;
		display = iPluginForThisDevice->ConsumeMemoryKeys(aDisk, aCheckboxName, aUpKeyCode, aDownKeyCode, aLogKeyCode);
		}
	return display;
	}

void CDefaultProductPlugin::Release(TBool aAppIsShuttingDown)
	{
	iShuttingDown = aAppIsShuttingDown;
	delete this;
	}

void CDefaultProductPlugin::GetFeatureUidsL(RArray<TUid>& aUids)
	{
	const TUid KDefaultFeatures[] = 
		{
		/* KFax */ {0x10279806},
		/* KPrint */ {0x10279807},
		/* KBluetooth */ {0x10279808},
		/* KInfrared */ {0x10279809},
		/* KMmc */ {0x1027980a},
		/* KUsb */ {0x1027980b},
		/* KObex */ {0x1027980c},
		/* KRtpRtcp */ {0x1027980d},
		/* KSip */ {0x1027980f},
		/* KOmaDataSync */ {0x10279810},
		/* KOmaDeviceManagement */ {0x10279811},
		/* KIPQoS */ {0x10279812},
		/* KNetworkQoS */ {0x10279813},
		/* KIPSec */ {0x10279814},
		/* KDhcp */ {0x10279815},
		/* KConnectivity */ {0x10279816},
		/* KLocation */ {0x10281818},
		/* KMobileIP */ {0x10281819},
		/* KOfflineMode */ {0x1028181A},
		/* KDRM */ {0x1028181B},
		/* KOmaDsHostServers */ {0x10282663},
		};
	const TInt KNumFeatures = sizeof(KDefaultFeatures) / sizeof(TUid);
	for (TInt i = 0; i < KNumFeatures; i++)
		{
		aUids.AppendL(KDefaultFeatures[i]);
		}

	if (iPluginForThisDeviceVersion > 2)
		{
		static_cast<MProductPluginV3*>(iPluginForThisDevice)->GetFeatureUidsL(aUids);
		}
	}

TPtrC KeyName(TInt aKeyCode)
	{
	enum TStuffNotDefinedInAllReleases
		{
		EKeyDevice20 = EKeyApplication1F + 1,
		EKeyDevice21,
		EKeyDevice22,
		EKeyDevice23,
		EKeyDevice24,
		EKeyDevice25,
		EKeyDevice26,
		EKeyDevice27,
		EKeyApplication20,
		EKeyApplication21,
		EKeyApplication22,
		EKeyApplication23,
		EKeyApplication24,
		EKeyApplication25,
		EKeyApplication26,
		EKeyApplication27,
		};

	switch(aKeyCode)
		{
#ifdef INCLUDE_UIQ_DEFINITIONS
		CASE_LIT2(EDeviceKeyTwoWayUp, "2-way up");
		CASE_LIT2(EDeviceKeyTwoWayDown, "2-way down");
		CASE_LIT2(EDeviceKeyFourWayUp, "4-way up");
		CASE_LIT2(EDeviceKeyFourWayDown, "4-way down");
		CASE_LIT2(EDeviceKeyFourWayLeft, "4-way left");
		CASE_LIT2(EDeviceKeyFourWayRight, "4-way right");
		CASE_LIT2(EDeviceKeyAction, "Confirm");
		CASE_LIT2(EDeviceKeyHomeScreen, "Home key");
		CASE_LIT2(EDeviceKeyCameraFocusLock, "Camera focus");
		CASE_LIT2(EDeviceKeyCameraShutter, "Camer shutter");
		CASE_LIT2(EDeviceKeyLock, "Key lock");
		CASE_LIT2(EDeviceKeyMultimedia, "Multimedia key");
		CASE_LIT2(EDeviceKeyIncVolume, "Volume up");
		CASE_LIT2(EDeviceKeyDecVolume, "Volume down");
		CASE_LIT2(EDeviceKeyPower, "Power");
		CASE_LIT2(EDeviceKeyVideoCall, "Video Call");
#else
		// These are things that have the same value as UIQ definitions above, thus we can't include them twice in the switch
		CASE_LIT(EKeyDevice1);
		CASE_LIT(EKeyDevice2);
		CASE_LIT(EKeyDevice4);
		CASE_LIT(EKeyDevice5);
		CASE_LIT(EKeyDevice6);
		CASE_LIT(EKeyDevice7);
		CASE_LIT(EKeyDevice8);
		CASE_LIT(EKeyDeviceD);
		CASE_LIT(EKeyApplication1);
		CASE_LIT(EKeyApplication3);
		CASE_LIT(EKeyApplication5);
#endif
		CASE_LIT2(EKeyBackspace, "Backspace");
		CASE_LIT2(EKeyEnter, "Return");
		CASE_LIT2(EKeySpace, "Space");
		CASE_LIT2(EKeyLeftArrow, "Left arrow");
		CASE_LIT2(EKeyRightArrow, "Right arrow");
		CASE_LIT2(EKeyUpArrow, "Up arrow");
		CASE_LIT2(EKeyDownArrow, "Down arrow");
		CASE_LIT2(EKeyYes, "GREEN key");
		CASE_LIT2(EKeyNo, "RED key");
		CASE_LIT(EKeyDevice0);
		CASE_LIT(EKeyDevice3);
		CASE_LIT(EKeyDevice9);
		CASE_LIT(EKeyDeviceA);
		CASE_LIT(EKeyDeviceB);
		CASE_LIT(EKeyDeviceC);
		CASE_LIT(EKeyDeviceE);
		CASE_LIT(EKeyDeviceF);
#ifdef FSHELL_PLATFORM_S60
		CASE_LIT2(EKeyApplication0, "Menu");
#else
		CASE_LIT(EKeyApplication0);
#endif
		CASE_LIT(EKeyApplication2);
		CASE_LIT(EKeyApplication4);
		CASE_LIT(EKeyApplication6);
		CASE_LIT(EKeyApplication7);
		CASE_LIT(EKeyApplication8);
		CASE_LIT(EKeyApplication9);
		CASE_LIT(EKeyApplicationA);
		CASE_LIT(EKeyApplicationB);
		CASE_LIT(EKeyApplicationC);
		CASE_LIT(EKeyApplicationD);
		CASE_LIT(EKeyApplicationE);
		CASE_LIT(EKeyApplicationF);
		CASE_LIT(EKeyDevice10);
		CASE_LIT(EKeyDevice11);
		CASE_LIT(EKeyDevice12);
		CASE_LIT(EKeyDevice13);
		CASE_LIT(EKeyDevice14);
		CASE_LIT(EKeyDevice15);
		CASE_LIT(EKeyDevice16);
		CASE_LIT(EKeyDevice17);
		CASE_LIT(EKeyDevice18);
		CASE_LIT(EKeyDevice19);
		CASE_LIT(EKeyDevice1A);
		CASE_LIT(EKeyDevice1B);
		CASE_LIT(EKeyDevice1C);
		CASE_LIT(EKeyDevice1D);
		CASE_LIT(EKeyDevice1E);
		CASE_LIT(EKeyDevice1F);
		CASE_LIT(EKeyApplication10);
		CASE_LIT(EKeyApplication11);
		CASE_LIT(EKeyApplication12);
		CASE_LIT(EKeyApplication13);
		CASE_LIT(EKeyApplication14);
		CASE_LIT(EKeyApplication15);
		CASE_LIT(EKeyApplication16);
		CASE_LIT(EKeyApplication17);
		CASE_LIT(EKeyApplication18);
		CASE_LIT(EKeyApplication19);
		CASE_LIT(EKeyApplication1A);
		CASE_LIT(EKeyApplication1B);
		CASE_LIT(EKeyApplication1C);
		CASE_LIT(EKeyApplication1D);
		CASE_LIT(EKeyApplication1E);
		CASE_LIT(EKeyApplication1F);
		CASE_LIT(EKeyDevice20);
		CASE_LIT(EKeyDevice21);
		CASE_LIT(EKeyDevice22);
		CASE_LIT(EKeyDevice23);
		CASE_LIT(EKeyDevice24);
		CASE_LIT(EKeyDevice25);
		CASE_LIT(EKeyDevice26);
		CASE_LIT(EKeyDevice27);
		CASE_LIT(EKeyApplication20);
		CASE_LIT(EKeyApplication21);
		CASE_LIT(EKeyApplication22);
		CASE_LIT(EKeyApplication23);
		CASE_LIT(EKeyApplication24);
		CASE_LIT(EKeyApplication25);
		CASE_LIT(EKeyApplication26);
		CASE_LIT(EKeyApplication27);
		default:
			return TPtrC();
		}
	}
