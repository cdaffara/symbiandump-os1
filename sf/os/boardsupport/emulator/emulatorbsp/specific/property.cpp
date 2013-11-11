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
// wins\specific\property.cpp
// Emulator property management for emulator settings
// 
//

#define _CRTIMP			// we want to use the static runtime library

#define __INCLUDE_CAPABILITY_NAMES__
#include "variant.h"
#include <string.h>
#include <stdlib.h>
#include <emulator.h>

#ifdef _DEBUG
#define _DUMP_PROPERTY
#endif

const char* KDefaultMachineName = "epoc";
const char* KDefaultTestMachineName = "defaulttest";

// name of the environment variable to check for default path
const char* KDefaultEpocRootName = "EPOCROOT";

TInt ScreenId=0;

// At the point this is created there is no kernel heap available
// So all memory allocation is done from a custom allocator

const TInt KMaxTokenLength = 255;

class Allocator
	{
	enum {ETotalMemory=0x10000};	// 64K
public:
	Allocator();
	TAny* Alloc(TInt aSize);
	TAny* Realloc(TAny* aCell, TInt aSize);
	void Free(TAny* aCell);
private:
	TUint iBuf[ETotalMemory];
	TUint* iFree;
	};

Allocator::Allocator()
	:iFree(iBuf)
	{}

TAny* Allocator::Alloc(TInt aSize)
	{
	aSize = (aSize + 7) >> 2;
	if (iFree + aSize > iBuf + ETotalMemory)
		return NULL;
	TUint* p = iFree;
	iFree += aSize;
	*p++ = aSize;
	return p;
	}

TAny* Allocator::Realloc(TAny* aCell, TInt aSize)
	{
	if (!aCell)
		return Alloc(aSize);

	TUint* p = (TUint*)aCell;
	TInt size = *--p;
	if (iFree == p + size)
		{
		aSize = (aSize + 7) >> 2;
		if (p + aSize > iBuf + ETotalMemory)
			return NULL;
		iFree = p + aSize;
		*p = aSize;
		return aCell;
		}

	TAny* newp = Alloc(aSize);
	if (newp)
		{
		memcpy(newp, aCell, size*sizeof(TUint));
		Free(aCell);
		}
	return newp;
	}

void Allocator::Free(TAny* )
	{
	}

Allocator TheAllocator;

///////

char* Duplicate(const char* aString)
	{
	if (aString == NULL)
		aString = "";

	TInt size = strlen(aString) + 1;
	char* p = (char*)TheAllocator.Alloc(size);
	if (p)
		memcpy(p, aString, size);
	return p;
	}

// Properties class implementation

Properties::Properties()
	:iEntries(NULL),iCount(0),iSize(0)
	{}

const char* Properties::Insert(TInt aIndex, const char* aProperty, const char* aValue)
	{
	if (iCount == iSize)
		{
		TInt size = iSize == 0 ? 8 : iSize*2;
		TAny* array = TheAllocator.Realloc(iEntries, size*sizeof(SEntry));
		if (array == NULL)
			return NULL;
		iEntries = (SEntry*)array;
		iSize = size;
		}

	char* prop = Duplicate(aProperty);
	if (prop == NULL)
		return NULL;
	char* value = Duplicate(aValue);
	if (value == NULL)
		TheAllocator.Free(prop);
	else
		{
		SEntry* e = &iEntries[aIndex];
		memmove(e+1, e, (iCount-aIndex)*sizeof(SEntry));
		e->iProperty = prop;
		e->iValue = value;
		++iCount;
		}
	return value;
	}

const char* Properties::Replace(const char* aProperty, const char* aValue)
	{
	TInt ix = Find(aProperty);
	if (ix < 0)
		return Insert(~ix, aProperty, aValue);
	// replacing a property
	SEntry& e = iEntries[ix];
	char* value = Duplicate(aValue);
	if (value != NULL)
		{
		TheAllocator.Free(e.iValue);
		e.iValue = value;
		}
	return value;
	}

const char* Properties::Append(const char* aProperty, const char* aValue)
	{
	TInt ix = Find(aProperty);
	if (ix < 0)
		return Insert(~ix, aProperty, aValue);

	// append a property
	SEntry& e = iEntries[ix];
	TInt size = strlen(e.iValue) + strlen(aValue) + 2;
	char* value = (char*)TheAllocator.Realloc(e.iValue, size);
	if (value != NULL)
		{
		strcat(value, ";");
		strcat(value, aValue);
		e.iValue = value;
		}
	return value;
	}

TInt Properties::GetString(const char* aProperty, const char*& aValue) const
	{
	TInt ix = Find(aProperty);
	if (ix < 0)
		return KErrNotFound;
	aValue = iEntries[ix].iValue;
	return KErrNone;
	}

TInt Properties::GetInt(const char* aProperty, TInt& aValue) const
	{
	TInt ix = Find(aProperty);
	if (ix < 0)
		return KErrNotFound;
	char* value = iEntries[ix].iValue;
	char* end;
	TBool neg = *value=='-';
	value += neg;
	long val = strtoul(value, &end, 0);
	if(neg)
		{
		if(val<0)
			return KErrArgument;
		val = -val;
		}
	if (*end != '\0')
		return KErrArgument;
	aValue = val;
	return KErrNone;
	}

TInt Properties::GetBool(const char* aProperty, TBool& aValue, TBool aDefaultValue) const
	{
	TInt ix = Find(aProperty);
	if (ix < 0)
		{
		aValue = aDefaultValue;
		return KErrNone;
		}
	const char* value=iEntries[ix].iValue;
	if (_stricmp(value, "on")==0 || _stricmp(value, "yes")==0 || _stricmp(value, "1")==0 || strlen(value)==0)
		{
		aValue = ETrue;
		return KErrNone;
		}
	if (_stricmp(value, "off")==0 || _stricmp(value, "no")==0 || _stricmp(value, "0")==0 )
		{
		aValue = EFalse;
		return KErrNone;
		}

	// Bool property has an illegal value!
	return KErrArgument;
	}

TInt Properties::Find(const char* aProperty) const
//
// Lookup a property in the table
// return index (>=0) if found, ~insertion-point (<0) if not
//
	{
	TInt l = 0, r = iCount;
	while (l < r)
		{
		TInt m = (l + r) >> 1;
		const SEntry& e = iEntries[m];
		TInt k = _stricmp(aProperty,e.iProperty);
		if (k < 0)
			r = m;
		else if (k > 0)
			l = m + 1;
		else
			return m;
		}
	return ~l;
	}

#ifdef _DUMP_PROPERTY
void Properties::Dump() const
	{
	for (TInt i = 0; i < iCount; ++i)
		{
		const SEntry& e = iEntries[i];
		char buf[512];
		strcpy(buf, e.iProperty);
		TInt len = strlen(e.iValue);
		if (len)
			{
			strcat(buf, " = ");
			if (len <= 256)
				strcat(buf, e.iValue);
			else
				{
				strncat(buf, e.iValue, 256);
				strcat(buf, "...");
				}
			}
		strcat(buf, "\r\n");
		OutputDebugStringA(buf);
		}
	}
#endif

// Property related variant functions

TInt Wins::EmulatorHal(TInt aFunction, TAny* a1, TAny* a2)
	{
	TInt r=KErrNone;
	switch(aFunction)
		{
		case EEmulatorHalStringProperty:
			return iProperties.GetString((const char*)a1,*(const char**)a2);
		case EEmulatorHalIntProperty:
			return iProperties.GetInt((const char*)a1,*(TInt*)a2);
		case EEmulatorHalBoolProperty:
			return iProperties.GetBool((const char*)a1,*(TBool*)a2);
		case EEmulatorHalMapFilename:
			return MapFilename(*(const TDesC*)a1,*(TDes*)a2);
		case EEmulatorHalSetFlip:
			{
			if (iUi)
				{
				TInt screen = (TInt)a2;
				if((TUint)screen < (TUint)iUi->NumberOfScreens())
					return iUi->SetFlip(TEmulatorFlip(TInt(a1)),screen);
				}
			break;
			}
		case EEmulatorHalColorDepth:
			{
			TUint colorDepth = KDefaultColorDepth;
			if(iUi)
				{
				if((TUint)a2 < (TUint)iUi->NumberOfScreens())
					colorDepth = iUi->ColorDepth((TInt)a2);
				}
			*(TUint*)a1 = colorDepth;
			return KErrNone;
			}
		case EEmulatorHalCPUSpeed:
			if (a1)
				return SetCpuSpeed(TUint(a2)/1000);
			*(TInt*)a2 = iCpuSpeed ? iCpuSpeed * 1000 : 1;
			return KErrNone;
		case EEmulatorHalNumberOfScreens:
			*(TInt*)a2 = iUi ? iUi->NumberOfScreens() : 1;
			return KErrNone;
		case EEmulatorHalSetDisplayChannel:
			if (iUi && (TUint)a1 < (TUint)iUi->NumberOfScreens())
				{
				r = iUi->SetDisplayChannel((TInt)a1, static_cast<DDisplayChannel*>(a2));
				}
			else
				{
				r = KErrNotSupported;
				}
			break;
		default:
			r=KErrNotSupported;
			break;
		}
	return r;
	}

const char* KExtensionListNormal = "btracex.ldd;hcr.dll;winsgui;elocd.ldd;medint.pdd;medlfs.pdd;medmmc.pdd;epbusmmc.dll;epbusv.dll";
const char* KExtensionUsiiNand = "?medusiiw.pdd";
const char* KExtensionUsiiNandLoader = "?medusiiws.pdd";
const char* KExtensionUsiiNandTest = "?medusiiwt.pdd";




const int KMaxEpocRootSize = 120;

TInt Wins::InitProperties(TBool aRunExe)
	{
	if (iProperties.Replace("MachineName", KDefaultMachineName) == NULL)
		return KErrNoMemory;

    char epocRoot[KMaxEpocRootSize];

    TInt total = GetEnvironmentVariableA( KDefaultEpocRootName, epocRoot, KMaxEpocRootSize);

    if (total != 0)
		{
	    if (iProperties.Replace("EpocRoot", epocRoot) == NULL)
    	    return KErrNoMemory;
		}

	if (iProperties.Append("Extension", KExtensionListNormal) == NULL)
		return KErrNoMemory;

	char overrideCDrive[MAX_PATH];
	overrideCDrive[0] = '\0';
	TInt r = ProcessCommandLine(aRunExe, overrideCDrive);
	if (r != KErrNone)
		return r;

	r = SetupPaths();
	if (r != KErrNone)
		return r;

	r = LoadProperties();
	if (r != KErrNone)
		return r;

	//	get Unistore II Media Driver type from epoc.ini
	const char* value = NULL;
	

	iProperties.GetString("NandDriverType", value);
	if (value)
		{
		if (value && _stricmp("XSR",value)==0)
			{
			// epoc.ini "NandDriverType=XSR" for XSR/Unistore-II driver
			if (iProperties.Append("Extension", KExtensionUsiiNand) == NULL)
				return KErrNoMemory;		
			}
		else if (value && _stricmp("XSRNandloader",value)==0)
			{
			// epoc.ini "NandDriverType=XSRNandloader" for XSR/Unistore-II nandloader driver
			if (iProperties.Append("Extension", KExtensionUsiiNandLoader) == NULL)
				return KErrNoMemory;		
			}
		else if (value && _stricmp("XSRTest",value)==0)
			{
			// epoc.ini "NandDriverType=XSRTest" for XSR/Unistore-II test driver
			if (iProperties.Append("Extension", KExtensionUsiiNandTest) == NULL)
				return KErrNoMemory;
			}
		else	
			{
			// If epoc.ini contains "NandDriverType=???" but ??? not equal to any
			// of above XSR driver types then load production/release XSR
			// driver
			if (iProperties.Append("Extension", KExtensionUsiiNand) == NULL)
				return KErrNoMemory;

			}
		}
	else
		{
		// Load the production/release XSR driver, if value is NULL
		if (iProperties.Append("Extension", KExtensionUsiiNand) == NULL)
			return KErrNoMemory;

		}
	

	// Get the name of the extension media drivers from epoc.ini (optional)
	value = NULL;
	iProperties.GetString("MediaExtensionDriver", value);
	if (value)
		{
		if (iProperties.Append("Extension", value) == NULL)
			return KErrNoMemory;		
		}


//	load additional configuration specific properties

//	get the multi property "configuration"
	value = NULL;
	iProperties.GetString("configuration", value);
	
//	load each one of these
//	check for any screen specific properties in the main epoc.ini
//	if configuration property is set
	if (value && !iConfigPropertySet)	//configuration
		{
		iConfigId = 0;
		char configFileName[100];
		do
			{
			//load each set of properties

		   const char * pdest = strchr(value, ';');
		   TInt result = pdest - value;
		   if(pdest)
			   {
			   strncpy(configFileName, value, result);
			   configFileName[result] = '\0';
			   value = value + result + 1;
			   }
		   else
			   {
			   strcpy(configFileName, value);
			   value += strlen(value);
			   }

			r = LoadConfigSpecificProperties(configFileName);
			if (r == KErrNone)
				iConfigId++;
			}
		while(*value);
		}

	char scr[30];
	//if iConfigId is zero, there is only 1 configuration
	wsprintfA(scr, "ConfigCount %d", iConfigId ? iConfigId : 1);
	r = AddProperty(scr, scr+strlen(scr));
	if (r != KErrNone)
		return r;

	r = SetupMediaPath();
	if (r != KErrNone)
		return r;

	if (overrideCDrive[0] != '\0')
		SetupDrive('c', overrideCDrive);

	if (iProperties.Append("Extension", "exstart") == NULL)
		return KErrNoMemory;

#ifdef _DUMP_PROPERTY
	iProperties.Dump();
#endif

	return KErrNone;
	}

char* skipws(char* aPtr)
	{
	while (isspace(*aPtr))
		++aPtr;
	return aPtr;
	}

char* skiptok(char* aPtr)
	{
	if (*aPtr == '\"')
		{
		++aPtr;
		while (*aPtr && *aPtr++ != '\"')
			{}
		}
	else
		{
		while (*aPtr && !isspace(*aPtr))
			++aPtr;
		}
	return aPtr;
	}

#ifdef _DEBUG
struct TDebugTrace
	{
	const char* iName;
	TInt iMask;
	};

// Only the first 32 trace bits can be defined using these values
// "ALWAYS" in this context means all of the first 32 bits not all 256 bits
const TDebugTrace KTraceValues[] =
	{
	{"ALWAYS", KALWAYS},
	{"BOOT", 1<<KBOOT},
	{"DEVICE", 1<<KDEVICE},
	{"DFC", 1<<KDFC},
	{"DLL", 1<<KDLL},
	{"EVENT", 1<<KEVENT},      
	{"EXEC", 1<<KEXEC},
	{"DEBUGGER", 1<<KDEBUGGER},
	{"EXTENSION", 1<<KEXTENSION},
	{"FAIL", 1<<KFAIL},
	{"HARDWARE", 1<<KHARDWARE},
	{"IPC", 1<<KIPC},
	{"LOCDRV", 1<<KLOCDRV},
	{"MEMTRACE", 1<<KMEMTRACE},
	{"MMU", 1<<KMMU},
	{"NKERN", 1<<KNKERN},
	{"OBJECT", 1<<KOBJECT},
	{"PANIC", 1<<KPANIC},
	{"PBUS1", 1<<KPBUS1},
	{"PBUS2", 1<<KPBUS2},
	{"PBUSDRV", 1<<KPBUSDRV},
	{"POWER", 1<<KPOWER},      
	{"PROC", 1<<KPROC},
	{"SCHED", 1<<KSCHED},
	{"SCHED2", 1<<KSCHED2},
	{"SCRATCH", 1<<KSCRATCH},
	{"SEMAPHORE", 1<<KSEMAPHORE},
	{"SERVER", 1<<KSERVER},
	{"THREAD", 1<<KTHREAD},
	{"THREAD2", 1<<KTHREAD2},
	{"TIMING", 1<<KTIMING},
	{"DMA", 1<<KDMA},
	{"MMU2", 1<<KMMU2}
	};
const TInt KMaxTraceName = 9;
const TInt KCountTraceValues = sizeof(KTraceValues)/sizeof(TDebugTrace);

static const TDebugTrace* TraceType(const char* aTrace, TInt aLen)
	{
	if (aLen > KMaxTraceName)
		return 0;

	char name[KMaxTraceName + 1];
	strncpy(name, aTrace, aLen);
	name[aLen] = '\0';

	for (TInt i=KCountTraceValues; --i>=0;)
		{
		if (_stricmp(name, KTraceValues[i].iName)==0)
			return &KTraceValues[i];
		}
	return 0;
	}
#endif

TInt Wins::DebugMask()
	{
	TInt mask = KDefaultDebugMask;
	if (iProperties.GetInt("DebugMask", mask) != KErrArgument)
		return mask;
#ifdef _DEBUG
	// allow text ones
	const char* e;
	if (iProperties.GetString("DebugMask", e) != KErrNone)
		return mask;

	for (;;)
		{
		char* p = skipws((char*)e);
		if (*p == 0)
			break;
		e = skiptok(p);
		TBool add = ETrue;
		if (*p == '+')
			++p;
		else if (*p == '-')
			{
			add = EFalse;
			++p;
			}
		const TDebugTrace* type = TraceType(p, e - p);
		if (type)
			{
			if (add)
				mask |= type->iMask;
			else
				mask &= ~type->iMask;
			}
		}
#endif
	return mask;
	}

TUint32 Wins::KernelConfigFlags()
	{
	TUint32 flags = 0;
	TBool b;

	b=0;
	iProperties.GetBool("PlatSecEnforcement",b,EFalse);
	if(b) flags |= EKernelConfigPlatSecEnforcement;
	Wins::EarlyLogging("PlatSecEnforcement ",b?"ON":"OFF");

	b=0;
	iProperties.GetBool("PlatSecDiagnostics",b,EFalse);
	if(b) flags |= EKernelConfigPlatSecDiagnostics;
	Wins::EarlyLogging("PlatSecDiagnostics ",b?"ON":"OFF");

	b=0;
	iProperties.GetBool("PlatSecProcessIsolation",b,EFalse);
	if(b) flags |= EKernelConfigPlatSecProcessIsolation;
	Wins::EarlyLogging("PlatSecProcessIsolation ",b?"ON":"OFF");

	b=0;
	iProperties.GetBool("PlatSecEnforceSysBin",b,EFalse);
	if(b) flags |= EKernelConfigPlatSecEnforceSysBin;
	Wins::EarlyLogging("PlatSecEnforceSysBin ",b?"ON":"OFF");

	b=0;
	iProperties.GetBool("CrazyScheduling",b,EFalse);
	if(b) flags |= EKernelConfigCrazyScheduling;
	Wins::EarlyLogging("CrazyScheduling ",b?"ON":"OFF");

	return flags;
	}

void Wins::DisabledCapabilities(SCapabilitySet& aCapabilities)
	{
	const char* text;
	if(iProperties.GetString("PlatSecDisabledCaps", text)!=KErrNone)
		text = "NONE";
	Wins::EarlyLogging("PlatSecDisabledCaps ",text);
	ParseCapabilitiesArg(aCapabilities,text);
	}

#define PARSE_CAPABILITIES_ERROR(aMessage) Wins::EarlyLogging(aMessage,0)
#define PARSE_CAPABILITIES_ERROR2(aMessage,aArg) Wins::EarlyLogging(aMessage,aArg)
#define strnicmp _strnicmp

TInt Wins::ParseCapabilitiesArg(SCapabilitySet& aCapabilities, const char *aText)
//
// This is a cun'n'paste copy of the function in TOOLS\E32TOOLS\HOST\H_UTIL.CPP
// Keep both of these versions up to date with each other
//
	{
	memset(&aCapabilities,0,sizeof(aCapabilities));
	char c;
	while((c=*aText)!=0)
		{
		if(c<=' ')
			{
			++aText;
			continue;
			}
		int invert=0;
		if(c=='+')
			{
			++aText;
			c=*aText;
			}
		if(c=='-')
			{
			invert=1;
			++aText;
			}
		const char* name = aText;
		while((c=*aText)>' ')
			{
			if(c=='-' || c=='+')
				break;
			++aText;
			}
		TInt n = aText-name;
		TInt i;

		if(n==3 && strnicmp("all",name,n)==0)
			{
			if(invert)
				{
				PARSE_CAPABILITIES_ERROR("Capability '-ALL' not allowed");
				return KErrArgument;
				}
			for(i=0; i<ECapability_Limit; i++)
				{
				if(CapabilityNames[i])
					aCapabilities[i>>5] |= (1<<(i&31));
				}
			continue;
			}

		if(n==4 && strnicmp("none",name,n)==0)
			{
			if(invert)
				{
				PARSE_CAPABILITIES_ERROR("Capability '-NONE' not allowed");
				return KErrArgument;
				}
			memset(&aCapabilities,0,sizeof(aCapabilities));
			continue;
			}

		for(i=0; i<ECapability_Limit; i++)
			{
			const char* cap = CapabilityNames[i];
			if(!cap)
				continue;
			if((int)strlen(cap)!=n)
				continue;
			if(strnicmp(cap,name,n)!=0)
				continue;
			break;
			}
		if(i>=ECapability_Limit)
			{
			char badName[32];
			if(n>=sizeof(badName)) n=sizeof(badName)-1;
			memcpy(badName,name,n);
			badName[n]=0;
			PARSE_CAPABILITIES_ERROR2("Unrecognised capability name: ",badName);
			return KErrArgument;
			}
		if(invert)
			aCapabilities[i>>5] &= ~(1<<(i&31));
		else
			aCapabilities[i>>5] |= (1<<(i&31));
		}
	return KErrNone;
	}

TInt Wins::AddProperty(char* aProperty, const char* aEol)
	{
	const char* tok = aProperty;
	int c;
	do
		{
		if (aProperty == aEol)
			{
			// boolean property
			if (_stricmp(tok, "_NewScreen_") == 0)
 				{
 				++ScreenId;
 				return KErrNone;
 				}
			else
				{
				char newtok[KMaxTokenLength];
				if (ConfigSpecificProperty(tok))
					{
					wsprintfA(newtok, "Configuration[%d]", iConfigId);
					strcat(newtok, tok);
					tok = newtok;
					}
				}
			return iProperties.Replace(tok, NULL) == NULL ? KErrNoMemory : KErrNone;
			}
		c=*aProperty++;
		} while (isalnum(c) || c=='_');
	aProperty[-1]='\0';	// terminate property name
	while (isspace(c))
		c=*aProperty++;
	TBool append=ETrue;
	if (c=='=')
		{
		append=EFalse;
		c=*aProperty++;
		}
	else if (c=='+' && *aProperty=='=')
		{
		++aProperty;
		c=*aProperty++;
		}
	while (isspace(c))
		c=*aProperty++;
	--aProperty;	// point back to value

	if (_strnicmp(tok, "_epoc_drive_", 12) == 0)
		return SetupDrive(tok[12], aProperty);
	else
		{
		char newtok[KMaxTokenLength];
		if (ConfigSpecificProperty(tok))
			{
			if (ScreenSpecificProperty(tok))
				{
				wsprintfA(newtok, "Configuration[%d][%d]", iConfigId, ScreenId);
				}
			else
				{
				wsprintfA(newtok, "Configuration[%d]", iConfigId);
				}
			strcat(newtok, tok);
			tok = newtok;
			}
		if (append)
			return iProperties.Append(tok, aProperty) == NULL ? KErrNoMemory : KErrNone;
		else
			return iProperties.Replace(tok, aProperty) == NULL ? KErrNoMemory : KErrNone;
		}
	}

TInt Wins::ProcessCommandLine(TBool aRunExe, char* aCDrive)
	{
	if (aRunExe)
		{
		char exe[MAX_PATH];
		DWORD len=GetModuleFileNameA(NULL, exe, MAX_PATH);
		if (len == 0)
			return KErrGeneral;
		exe[len] = '\0';
		const char* base = strrchr(exe, '\\') + 1;
		if (iProperties.Replace("AutoRun", base) == NULL)
			return KErrNoMemory;
		}

	char* cmd = skipws(skiptok(GetCommandLineA()));
	if (strstr(cmd, "--") != NULL)
		{
		for (;;)
			{
			cmd = strchr(cmd, '-') + 1;
			TInt opt = *cmd++;
			if (opt == '-')
				break;
			char* end = skiptok(cmd);
			*end = '\0';
			switch (tolower(opt))
				{
			    case 'd':
				    {
				    TInt r = AddProperty(cmd, end);
				    if (r != KErrNone)
				    	return r;
				    }
				    break;
			    case 'm':
			    	// specify base name for .INI file
				    if (iProperties.Replace("MachineName", cmd) == NULL)
				    	return KErrNoMemory;
				    break;
			    case 'l':
			    	// specify language
			    	if (iProperties.Replace("TheMachineLanguageIndex", cmd) == NULL)
			    		return KErrNoMemory;
			    	break;
		    	case 'c':
		    		// specify path for emulated C drive
			    	{
			    	DWORD len=GetFullPathNameA(cmd, MAX_PATH, aCDrive, NULL);
			    	if (len==0 || len >= MAX_PATH)
			    		aCDrive[0] = '\0';
			    	}
			        break;
			    case 't':
			    	// specify the temp path as the emulated C drive
			    	{
			    	DWORD len=GetTempPathA(MAX_PATH, aCDrive);
			    	if (len==0 || len >= MAX_PATH)
			    		aCDrive[0] = '\0';
			    	}
			    	break;
				}
			cmd = end+1;
			}
		cmd = skipws(cmd);
		}

	if (aRunExe && iProperties.Replace("CommandLine", cmd) == NULL)
		return KErrNoMemory;

	return KErrNone;
	}

TInt Wins::LoadConfigSpecificProperties(const char * aFile)
	{
	const char* path;
	TInt r = iProperties.GetString("EmulatorDataPath", path);
	if (r != KErrNone)
		return r;

	char file[KMaxFileName + 1];
	strcpy(file, path);
	strcat(file, aFile);

	char* iniData;
	r = ReadIniFile(file, iniData);
	if (r == KErrNone)
		{
		r = ReadProperties(iniData);
		VirtualFree(iniData, 0, MEM_RELEASE);
		}
	else if (r == KErrNotFound)
		r = KErrNotFound;

	return r;

	}



TInt Wins::LoadProperties()
	{
	const char* path;
	TInt r = iProperties.GetString("EmulatorDataPath", path);
	if (r != KErrNone)
		return r;
	const char* name;
	r = iProperties.GetString("MachineName", name);
	if (r != KErrNone)
		return r;
retry:
	char file[KMaxFileName + 1];
	strcpy(file, path);
	strcat(file, name);
	strcat(file, ".ini");

	char* iniData;
	r = ReadIniFile(file, iniData);
	if (r == KErrNone)
		{
		r = ReadProperties(iniData);
		VirtualFree(iniData, 0, MEM_RELEASE);
		}
	else if (r == KErrNotFound)
		{
		if(_stricmp(name,KDefaultMachineName)==0)
			{
			// try test ini file
			name = KDefaultTestMachineName;
			goto retry;
			}
		r = KErrNone;		// no ini file - oh well
		}

	return r;
	}

TInt Wins::ReadProperties(char* aData)
	{
	ScreenId = 0;
	while (*aData)
		{
		char* beg = aData;
		char* eol = strchr(beg, '\n');
		aData = eol+1;
		if (eol == beg)
			continue;
		if (eol[-1] == '\r' && --eol == beg)
			continue;
		*eol = '\0';		// terminate line

		while (isspace(*beg))
			++beg;
		char* comment = strchr(beg, '#');
		if (comment)
			eol = comment;
		while (eol > beg && isspace(eol[-1]))
			--eol;
		if (beg == eol)
			continue;
		*eol = '\0';		// terminate line

		TInt r = AddProperty(beg, eol);
		if (r != KErrNone)
			return r;
		}
	char sc[5];
 	wsprintfA(sc, "%d", ScreenId+1);
	TInt screens;
	if(iProperties.GetInt("[screens]", screens) == KErrNone && screens > ScreenId)
		return KErrNone;
 	else
		return iProperties.Replace("[screens]", sc)  == NULL ? KErrNoMemory : KErrNone;
	}

TInt Wins::ReadIniFile(const char* aFileName, char*& aContents)
	{
	TInt r = KErrNone;
	HANDLE file=CreateFileA(aFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!file || file==INVALID_HANDLE_VALUE)
		r = KErrNotFound;	// More than likely !
	else
		{
		TInt size=GetFileSize(file, NULL);
		if (size==INVALID_FILE_SIZE)
			r = KErrGeneral;
		else
			{
			// fileSize+3 to ensure zero-termination of file and trailing CRLF
			// VirtualAlloc initializes memory to zero
			TAny* data = VirtualAlloc(NULL, size+3, MEM_COMMIT, PAGE_READWRITE);
			if (!data)
				r = KErrNoMemory;
			else
				{
				DWORD bytesRead;
				if (!ReadFile(file, data, size, &bytesRead, NULL))
					{
					VirtualFree(data, 0, MEM_RELEASE);
					r = KErrGeneral;
					}
				else
					{
					aContents = (LPSTR)data;
					strcpy(aContents + size,"\r\n");
					}
				}
			}
		CloseHandle(file);
		}
	return r;
	}


TInt Wins::SetupPaths()
//
// set up the Emulator paths
//
	{
	// the Emulator path
	CHAR path[KMaxFileName + 1];
	DWORD len=GetModuleFileNameA(NULL, path, KMaxFileName);
	if (len == 0)
		return(KErrGeneral);
	path[len] = '\0';
	_strlwr(path);
	*(strrchr(path, '\\') + 1) = '\0';
	const char* emulatorPath = iProperties.Replace("EmulatorPath", path);
	if (!emulatorPath)
		return KErrNoMemory;

	CHAR drive[KMaxFileName + 1];

	// the Emulator data path
	strcat(path, "data\\");
	DWORD att = GetFileAttributesA(path);
	if (att != -1 && (att&FILE_ATTRIBUTE_DIRECTORY))
		{
		// if Data directory exists in the emulator path, do things the new way
		strcpy(drive, emulatorPath);
		strcat(drive,"c\\");
		}
	else
		{
		// the old way
#if defined(__VC32__)
		char* p = strstr(path, "\\epoc32\\release\\wins\\");
#elif defined(__CW32__)
		char* p = strstr(path, "\\epoc32\\release\\winscw\\");
#endif
		if (p == NULL)
			return KErrNotFound;
		strcpy(p, "\\epoc32\\");
		strcpy(drive, path);
		strcat(path, "data\\");
#if defined(__VC32__)
		strcat(drive,"wins\\c\\");
#elif defined(__CW32__)
		strcat(drive,"winscw\\c\\");
#endif
		}
	if (!iProperties.Replace("EmulatorDataPath", path))
		return KErrNoMemory;

	// The Emulator Image path (for temporary EXE files)
	const char* eip;
	TInt r = iProperties.GetString("EmulatorImagePath", eip);
	if (r!=KErrNone)
		{
		len=GetTempPathA(KMaxFileName, path);
		strcat(path, "epoc\\");
		char* p = path + strlen(path);
		*p++ = emulatorPath[0];
		strcpy(p, emulatorPath+2);
		if (!iProperties.Replace("EmulatorImagePath", path))
			return KErrNoMemory;
		}
	else
		strcpy(path, eip);
	if (!Emulator::CreateAllDirectories(path))
		return Emulator::LastError();

	// Win32 filesystem paths mapped to local WINS drives
	r = SetupDrive('c',drive);  // set up C here, can be overridden by system.ini settings
	if (r)
		return(r);

	strcpy(drive, emulatorPath);
	strcat(drive,"z\\");

	r=SetupDrive('z',drive);  // set up Z here, can be overridden by system.ini settings
	if (r)
		return(r);

	return(KErrNone);
	}

TInt Wins::SetupMediaPath()
//
// Set up the path for emulated media devices 'EmulatedMediaPath'
// The default is <datapath>media/
// The system temporary path can be set by value '%temp%'
//
	{
	CHAR path[KMaxFileName + 1];
	const char* mpath;
	if (iProperties.GetString("EmulatorMediaPath", mpath) == KErrNotFound)
		{
		const char* dpath;
		TInt r = iProperties.GetString("EmulatorDataPath", dpath);
		if (r != KErrNone)
			return r;
		strcpy(path, dpath);
		strcat(path, "media\\");
		return iProperties.Replace("EmulatorMediaPath", path) ? KErrNone : KErrNoMemory;
		}

	if (_stricmp(mpath, "%temp%") == 0)
		{
		DWORD len=GetTempPathA(KMaxFileName, path);
		if (len > 0 && len < KMaxFileName)
			return iProperties.Replace("EmulatorMediaPath", path) ? KErrNone : KErrNoMemory;
		}

	return KErrNone;
	}

const char* Wins::EmulatorMediaPath()
	{
	const char* mpath = NULL;
	iProperties.GetString("EmulatorMediaPath", mpath);
	return mpath;
	}

TInt Wins::SetupDrive(int aDrive, const char* aPath)
//
// set up emulated drives
//
	{

	// Z drive can't end in anything but "Z\\", since we chop this off and use
	// the resulting directory to find filenames with no drive specified in
	// MapFileName() below.
	aDrive = tolower(aDrive);
	if (aDrive=='z')
		{
		const char* end = aPath + strlen(aPath);
		if (_stricmp(end-2,"\\z") != 0 && _stricmp(end-3,"\\z\\") != 0)
			return KErrArgument;
		}

	char prop[] = "_epoc_drive_?";
	*strchr(prop, '?') = char(aDrive);


    // If the path begins with the keyword %epocroot%, replace this with EPOCROOT
    if (_strnicmp(aPath, "%epocroot%", 10) == 0)
        {
		aPath += 10; // skip "%epocroot%"

        const char* eRoot;
        TInt r = iProperties.GetString("EpocRoot", eRoot);
        if (r != KErrNone)
            return r;

		int rootSize = strlen(eRoot);
		int pathSize = strlen(aPath);
		if(rootSize+pathSize>MAX_PATH)
			return KErrArgument;

        char fullPath[MAX_PATH+1];
		memcpy(fullPath,eRoot,rootSize);
		memcpy(fullPath+rootSize,aPath,pathSize+1); // +1 to get the terminating NULL char

        return iProperties.Replace(prop, fullPath) ? KErrNone : KErrNoMemory;

        }
    else
        // otherwise, aPath is fully qualified path name. Use that.
        return iProperties.Replace(prop, aPath) ? KErrNone : KErrNoMemory;
  
	}

TInt Wins::MapDrive(int aDrive, TDes& aBuffer) const
//
// Map aDrive to a path given by environment variables or defaults
// Use this function only in WINS builds
//
	{
	char drive[KMaxFileName + 1];
	char prop[] = "_epoc_drive_?";
	*strchr(prop, '?') = char(tolower(aDrive));

	TInt len;
	const char* val;
	if (iProperties.GetString(prop, val) == KErrNone)
		{
		len = strlen(val);
		if (len > KMaxFileName)
			return KErrArgument;
		strcpy(drive, val);
		}
	else
		{
		// not in properties, so check environment
		len = GetEnvironmentVariableA(prop, drive, KMaxFileName + 1);
		if (len > KMaxFileName)
			return KErrArgument;
		}
	while (len > 0 && isspace(drive[len-1]))
		--len;
	if (len == 0)
		return KErrNotFound;
	if (drive[len-1] != '\\') // add trailing backslash
		drive[len++] = '\\';
	if (drive[0] == '\\')
		{
		// put in the emulator drive
		TInt r = iProperties.GetString("EmulatorPath", val);
		if (r != KErrNone)
			return r;

		memmove(drive + 2, drive, len);
		drive[0] = val[0];
		drive[1] = ':';
		len += 2;
		}
	else if (len < 3 || drive[1] != ':' || drive[2] != '\\')
		return KErrArgument;
#ifdef _UNICODE
	TUint16* aBufPtr = (TUint16*)aBuffer.Ptr();
	const TText* drv = (const TText*)drive;
	for(int index=0;index<len;index++)
		*aBufPtr++ = (TUint16)*drv++;
	aBuffer.SetLength(len<<1);
#else
	aBuffer.Copy(TPtrC8((const TText8*)drive,len));
#endif
	return KErrNone;
	}

TInt Wins::MapFilename(const TDesC& aFilename, TDes& aBuffer) const
//
// Map aFileName to real windows directory - aFileName must be a full filepath
//
	{

	// if the filename does not have a drive specified then don't imagine
	// it describes an Epoc filepath
	// Assume it's a subdirectory/file of the file containing the emulated Z drive
	TInt offset;
	if (aFilename.Length() < 4 || aFilename[2] != ':')
		{
		TInt r = MapDrive('z', aBuffer);
		if (r)
			return(r);
		aBuffer.SetLength(aBuffer.Length()-4);	// chop "Z\\"
		offset = aFilename[0] == '\\' ? 1 : 0; // remove the guaranteed backslash
		}
	else
		{
		TInt r = MapDrive(aFilename[0], aBuffer);
		if (r)
			return(r);
		if (aFilename.Length() >= 6 && aFilename[4] == '\\')
			offset = 3;
		else
			offset = 2;
		}
#ifdef _UNICODE
	offset = offset<<1;
	TUint8* ptrFilename = (TUint8*)aFilename.Ptr() + offset;
	TUint8* ptrBuffer = (TUint8*)aBuffer.Ptr()+aBuffer.Length();
	if (aBuffer.MaxLength()<aBuffer.Length()+aFilename.Length()-offset+1)
		return KErrBadName;

	memcpy(ptrBuffer, ptrFilename, aFilename.Length()-offset);
	aBuffer.SetLength(aBuffer.Length()+aFilename.Length()-offset);
#else
	TPtrC name(aFilename.Mid(offset));
	if (aBuffer.MaxLength()<aBuffer.Length()+name.Length()+1)
		return KErrBadName;
	aBuffer.Append(name);
#endif
	return KErrNone;
	}


//table of the property names which can be used in multiple configurations
const char * KConfigSpecificProperties[] =
	{
	"ScreenWidth",
	"ScreenHeight",
	"PhysicalScreenWidth",
	"PhysicalScreenHeight",
	"ScreenOffsetX",
	"ScreenOffsetY",
	"LedSize",
	"LedArrangeVertically",
	"LedArrangeHorizontally",
	"LedOffsetX",
	"LedOffsetY",
	"LedGap",
	"PointerType",
	"ColorDepth",
	"KeyMap",
	"DrawVirtualKeys",
	"VirtualKeyColor",
	"VirtualKey",
	"MouseTarget",
	"FasciaBitmap",
	"DigitizerOffsetX",
	"DigitizerOffsetY",
	"DigitizerWidth",
	"DigitizerHeight",
	"DisableDigitizer",
	"DefineKeyName",
	"WindowTitle",
	"NoVersionInfo",
	"OnActivation",
	"EmulatorControl",
	"EmulatorControlHotKey",
	"CompositionBuffers",
	"RefreshRateHz",
	};

const char * KScreenSpecificProperties[] =
 	{
 	"ScreenWidth",
 	"ScreenHeight",
 	"PhysicalScreenWidth",
 	"PhysicalScreenHeight",
 	"ScreenOffsetX",
 	"ScreenOffsetY",
 	"ColorDepth",
	"FasciaBitmap",
	"CompositionBuffers",
	"RefreshRateHz",
 	};


TBool Wins::ConfigSpecificProperty(const char * aProperty)
	{
	TInt x;
	TInt count = sizeof(KConfigSpecificProperties) / sizeof(char *);
	for (x = 0; x < count; ++x)
		if (_stricmp(aProperty, KConfigSpecificProperties[x]) == 0)	return ETrue;
	return EFalse;
	}

TBool Wins::ScreenSpecificProperty(const char * aProperty)
	{
	TInt x;
	TInt count = sizeof(KScreenSpecificProperties) / sizeof(char *);
	for (x = 0; x < count; ++x)
		if (_stricmp(aProperty, KScreenSpecificProperties[x]) == 0)	return ETrue;
	return EFalse;
	}
