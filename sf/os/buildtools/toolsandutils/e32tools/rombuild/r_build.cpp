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
//

#define __REFERENCE_CAPABILITY_NAMES__
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <e32std.h>
#include <e32std_private.h>
#include <e32rom.h>
#include "u32std.h"
#include <e32uid.h>
#include <f32file.h>
#include "h_utl.h"

#define USE_IAT_FOR_IMPORTS (iOverrideFlags&KOverrideKeepIAT || (iHdr->iCpuIdentifier & 0x1000))

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <iomanip>
#else //!__MSVCDOTNET__
#include <iomanip.h>
#endif

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <iomanip>
#else //!__MSVCDOTNET__
#include <iomanip.h>
#endif

#include "r_obey.h"
#include "r_global.h"
#include "r_dir.h"

TInt NumRootDirs;

inline TLinAddr ActualToRomAddress(TAny* anAddr)
	{ return TLinAddr(anAddr)-TheRomMem+TheRomLinearAddress; }

TBool THardwareVariant::MutuallyExclusive(THardwareVariant a) const
	{
	if (Layer()<=3 || a.Layer()<=3)
		return EFalse;
	if (Parent()==3 && a.Parent()==3)
		return(Layer()!=a.Layer());
	if (Parent()==3 && a.Parent()!=Layer())
		return ETrue;
	if (a.Parent()==3 && Parent()!=a.Layer())
		return ETrue;
	if (Layer()!=a.Layer())
		return ETrue;
	return((VMask()&a.VMask())==0);
	}

TBool THardwareVariant::IsVariant() const
	{
	if (Layer()<=3 || Parent()==3)
		return EFalse;
	TUint v=VMask();
	TInt i;
	for (i=0; i<16; i++)
		{
		if (v==TUint(1<<i))
			return ETrue;
		}
	return EFalse;
	}

TInt THardwareVariant::Compare(THardwareVariant a) const
	{
	TUint l1=Layer();
	TUint p1=Parent();
	TUint v1=VMask();
	TUint l2=a.Layer();
	TUint p2=a.Parent();
	TUint v2=a.VMask();

	if (l1<=3)
		{
		if (l2<=3)
			{
			return EEqual;
			}
		return EGreater;
		}
	if (l2<=3)
		return ELess;
	if (p1==3)
		{
		if (p2==3)
			{
			if (l1==l2)
				return EEqual;
			return EUnordered;
			}
		if (p2==l1)
			return EGreater;
		return EUnordered;
		}
	if (p2==3)
		{
		if (p1==l2)
			return ELess;
		return EUnordered;
		}
	if (l1!=l2)
		return EUnordered;
	if ((v1&v2)==v1)
		return ELess;
	if ((v1&v2)==v2)
		return EGreater;
	return EUnordered;
	}
//
TInt TRomNode::Count=0;
TRomNode::TRomNode(const TText* aName, TRomBuilderEntry* aEntry)
//
// Constructor from TRomBuilderEntry, i.e. for new file or directory
//
	{
	memset(this, 0, sizeof(TRomNode));
	iAtt = (TUint8)KEntryAttReadOnly;
	iName = (TText*)NormaliseFileName((const char*)aName);
	iIdentifier=TRomNode::Count++;
	iRomFile = new TRomFile;
	if (aEntry)
		{
		iRomFile->iRbEntry = aEntry;
		aEntry->SetRomNode(this);
		iBareName = strdup(aEntry->iBareName);
		iRomFile->iHwvd = aEntry->iHardwareVariant;
		}
	else
		{
		iRomFile->iDir = ETrue;
		iAtt |= (TUint8)KEntryAttDir;
		iBareName = strdup((const char*)iName);
		iRomFile->iHwvd = KVariantIndependent;
		}
	TRACE(TROMNODE,Print(ELog, "TRomNode %d name %s bare %s att %02x romfile %08x\n", iIdentifier,
			iName, iBareName, iAtt, iRomFile));
	}

TRomNode::TRomNode(const TText* aName, TRomNode* aNode)
//
// Constructor from TRomNode, i.e. for aliased file
//
	{
	memset(this, 0, sizeof(TRomNode));
	iAtt = aNode->iAtt;
	iIdentifier=TRomNode::Count++;
	iName = (TText*)NormaliseFileName((const char*)aName);
	iHidden = aNode->iHidden;
	iRomFile = aNode->iRomFile;
	if (iRomFile)
		{
		iRomFile->Open();
		}
	TRACE(TROMNODE,Print(ELog, "TRomNode %d DUP name %s romfile %08x\n", iIdentifier, iName, iRomFile));
	}

TRomNode::TRomNode(const TRomNode& aNode)
//
// Copy constructor - only used in deep copy function
//
	{
	memset(this, 0, sizeof(TRomNode));
	iAtt = aNode.iAtt;
	iIdentifier=TRomNode::Count++;
	iName = (TText*)strdup((const char*)aNode.iName);
	iBareName = strdup(aNode.iBareName);
	iHidden = aNode.iHidden;
	iRomFile = aNode.iRomFile;
	if (iRomFile)
		{
		iRomFile->Open();
		}
	TRACE(TROMNODE,Print(ELog, "TRomNode %d COPY name %s bare %s att %02x romfile %08x\n", iIdentifier,
			iName, iBareName, iAtt, iRomFile));
	}

TRomNode::~TRomNode()
	{
	free(iName);
	free(iBareName);
	if (iRomFile)
		iRomFile->Close();
	}

TRomNode* TRomNode::FindInDirectory(const TText* aName)
//
// Check if the TRomNode for aName exists in aDir, and if so, return it.
//
	{

	TRomNode *entry=iChild; // first subdirectory or file
	while (entry)
		{
		if (!entry->iHidden && (stricmp((const char *)aName, (const char *)entry->iName))==0) 
			return entry;
		else
			entry=entry->iSibling;
		}
	return 0;
	}

TRomNode* TRomNode::FindInDirectory(const TText* aName, THardwareVariant aVariant, TBool aPatchDataFlag)
//
// Check for a file with same name and a compatible hardware variant
//
	{

	TRomNode *entry=iChild; // first subdirectory or file
	while (entry)
		{
		if (((!entry->iHidden)||aPatchDataFlag) && entry->iRomFile && (stricmp((const char *)aName, (const char *)entry->iName))==0) 
			{
			if (!aVariant.MutuallyExclusive(entry->HardwareVariant()))
				return entry;
			}
		entry=entry->iSibling;
		}
	return 0;
	}

void TRomNode::AddFile(TRomNode* aChild)
	{
	if (!(iAtt & KEntryAttDir))
		{
		Print(EError, "Adding subdirectory to a file!!!\n");
		return;
		}
	Add(aChild);
	}

TRomNode* TRomNode::NewSubDir(const TText* aName)
	{
	if (!(iAtt & KEntryAttDir))
		{
		Print(EError, "Adding subdirectory to a file!!!\n");
		return 0;
		}

	TRomNode* node = new TRomNode(aName);
	if (node==0)
		{
		Print(EError, "TRomNode::NewNode: Out of memory\n");
		return 0;
		}
	Add(node);
	return node;
	}

void TRomNode::Add(TRomNode* aChild)
	{
	if (iChild) // this node is a non-empty directory
		{
		TRomNode* dir = iChild; // find where to link in the new node
		while (dir->iSibling)
			dir = dir->iSibling;
		dir->iSibling = aChild;
		}
	else
		iChild = aChild; // else just set it up as the child of the dir
	aChild->iSibling = 0;
	aChild->iParent = this;
	}

void TRomNode::Remove(TRomNode* aChild)
	{
	if (iChild==0)
		{
		Print(EError, "Removing file from a file!!!\n");
		return;
		}
	if (iChild==aChild) // first child in this directory
		{
		iChild = aChild->iSibling;
		aChild->iSibling = 0;
		aChild->iParent = 0;
		return;
		}
	TRomNode* prev = iChild;
	while (prev->iSibling && prev->iSibling != aChild)
		prev = prev->iSibling;
	if (prev==0)
		{
		Print(EError, "Attempting to remove file not in this directory!!!\n");
		return;
		}
	prev->iSibling = aChild->iSibling;
	aChild->iSibling = 0;
	aChild->iParent = 0;
	}

void TRomNode::CountDirectory(TInt& aFileCount, TInt& aDirCount)
	{
	TRomNode *current=iChild;
	while(current)
		{
		if (current->iChild)
			aDirCount++;
		else if (!current->iHidden)
			aFileCount++;
		current=current->iSibling;
		}
	}

/**
 * Walk the contents of the directory, accumulating the
 * files as FileEntry objects in the specified RomFileStructure
 * and recursively handling the sub-directories
 *
 * TRomNode::ProcessDirectory is a pair with
 * RomFileStructure::ProcessDirectory
 */
int TRomNode::ProcessDirectory(RomFileStructure* aRFS)
	{
	TInt r=KErrNone;
	TRomNode *current=iChild;
	while(current)
		{
		if (current->iAtt & (TUint8)KEntryAttDir)
			{
			r=aRFS->ProcessDirectory(current);
			if (r!=KErrNone)
				return r;
			}
		else if (!current->iHidden)
			{
			FileEntry *pE=FileEntry::New(current);
			if (!pE)
				return KErrNoMemory;
			r=aRFS->Add(*pE);
			if (r==KErrOverflow)
				return r;
			}
		current=current->iSibling;
		}
	return r;
	}

void TRomNode::AddExecutableFile(TRomNode*& aLast, TRomNode* aNode)
	{
	aLast->iNextExecutable = aNode;
	aLast = aNode;
	aNode->iAtt |= KEntryAttXIP;
	}

char* TRomNode::BareName() const
	{
	return iBareName;
	}

TUint32 TRomNode::Uid3() const
	{
	return iRomFile->Uid3();
	}

TUint32 TRomNode::ModuleVersion() const
	{
	return iRomFile->ModuleVersion();
	}

THardwareVariant TRomNode::HardwareVariant() const
	{
	return iRomFile->HardwareVariant();
	}

TUint32 TRomNode::ABI() const
	{
	return iRomFile->ABI();
	}

TUint32 TRomFile::Uid3() const
	{
	assert(!iDir);
	if (iRbEntry)
	{
		if(iRbEntry->iHdr)
			return iRbEntry->iHdr->iUid3;
	}
	return RomImgHdr()->iUid3;
	}

TUint32 TRomFile::ModuleVersion() const
	{
	assert(!iDir);
	if (iRbEntry)
	{
		if(iRbEntry->iHdr)
			return iRbEntry->iHdr->ModuleVersion();
	}
	return RomImgHdr()->iModuleVersion;
	}

THardwareVariant TRomFile::HardwareVariant() const
	{
	return iHwvd;
	}

TUint32 TRomFile::ABI() const
	{
	assert(!iDir);
	if (iRbEntry)
	{
		if(iRbEntry->iHdr)
			return iRbEntry->iHdr->ABI();
	}
	return RomImgHdr()->iFlags & KRomImageABIMask;
	}

TInt TRomFile::ExportDirCount() const
	{
	assert(!iDir);
	if (iRbEntry)
	{
		if(iRbEntry->iHdr)
			return iRbEntry->iHdr->iExportDirCount;
	}
	return RomImgHdr()->iExportDirCount;
	}

TUint32 TRomFile::RomImageFlags() const
	{
	assert(!iDir);
	if (iRbEntry)
		{
		if(iRbEntry->iHdr)
		{
		const TUint KRomFlagMask = KImageDll | KImageNoCallEntryPoint | KImageFixedAddressExe | KImageNmdExpData;
		TUint romflags = iRbEntry->iHdr->iFlags & KRomFlagMask;
		return iRbEntry->iRomImageFlags | romflags;
		}
		}
	return RomImgHdr()->iFlags;
	}

TLinAddr TRomFile::DataBssLinearBase() const
	{
	assert(!iDir);
	if (iRbEntry)
		return iRbEntry->iDataBssLinearBase;
	return RomImgHdr()->iDataBssLinearBase;
	}

const SSecurityInfo& TRomFile::SecurityInfo() const
	{
	assert(!iDir);
	if (iRbEntry)
		return iRbEntry->iS;
	return RomImgHdr()->iS;
	}

TInt TRomNode::FullNameLength(TBool aIgnoreHiddenAttrib) const
	{
	TInt l = 0;
	// aIgnoreHiddenAttrib is used to find the complete file name length as
	// in ROM of a hidden file.
	if (iParent && ( !iHidden || aIgnoreHiddenAttrib))
		l = iParent->FullNameLength() + 1;
	l += strlen((const char*)iName);
	return l;
	}

TInt TRomNode::GetFullName(char* aBuf, TBool aIgnoreHiddenAttrib) const
	{
	TInt l = 0;
	TInt nl = strlen((const char*)iName);
	// aIgnoreHiddenAttrib is used to find the complete file name as in ROM of a hidden file.
	if (iParent && ( !iHidden || aIgnoreHiddenAttrib))
		l = iParent->GetFullName(aBuf);
	char* b = aBuf + l;
	if (l)
		*b++ = '\\', ++l;
	memcpy(b, iName, nl);
	b += nl;
	*b = 0;
	l += nl;
	return l;
	}

TInt CompareCapabilities(const SCapabilitySet& aSubCaps, const SCapabilitySet& aSuperCaps, const char* aSubName, const char* aSuperName)
//
//	Check that a aSubCaps are a subset of aSuperCaps
//
	{
	if ((!gPlatSecEnforcement)&&(!gPlatSecDiagnostics))
		return KErrNone;
	TInt i;
	TUint32 c = 0;
	for (i=0; i<SCapabilitySet::ENCapW; ++i)
		c |= (aSubCaps[i] &~ aSuperCaps[i]);
	TInt r = c ? KErrPermissionDenied : KErrNone;
	if (r && aSubName && aSuperName)
		{
		TPrintType printType;
		if(gPlatSecEnforcement)
			printType = EError;
		else
			{
			printType = EWarning;
			r = KErrNone;
			}
		char* buf = (char*)malloc(2);
		if(!buf)
			return KErrNoMemory;
		TInt len = 0;
		for(i=0; i<ECapability_Limit; i++)
			{
			if( (aSubCaps[i>>5] &~ aSuperCaps[i>>5]) & (1<<(i&31)) )
				{
				// append capability name to buf
				const char* name = CapabilityNames[i];
				if(!name)
					continue;
				if(len)
					{
					buf[len++] = ' ';
					}
				int nameLen=strlen(name);
				buf = (char*)realloc(buf,len+nameLen+2);
				if(!buf)
					return KErrNoMemory;
				memcpy(buf+len,CapabilityNames[i],nameLen);
				len += nameLen;
				}
			}
		buf[len]=0;
		Print(printType, "*PlatSec* %s - Capability check failed. Can't load %s because it links to %s which has the following capabilities missing: %s\n",gPlatSecEnforcement?"ERROR":"WARNING",aSubName, aSuperName, buf);
		free(buf);
		}
	return r;
	}

TDllFindInfo::TDllFindInfo(const char* aImportName, const TRomBuilderEntry* aEntry)
	{
	TUint32 flags;
	iBareName = SplitFileName(aImportName, iUid3, iModuleVersion, flags);
	assert(iBareName != 0);
	iHwVariant = aEntry->iHardwareVariant.ReturnVariant(); 
	}

TDllFindInfo::~TDllFindInfo()
	{
	free((void*)iBareName);
	}

// Generate name as follows:
// PC filename (UID3:xxxxxxxx HWVD:xxxxxxxx VER:M.m)
TModuleName::TModuleName(const TRomBuilderEntry* a)
	{
	TInt l = strlen(a->iFileName) + strlen(" (UID3:xxxxxxxx HWVD:xxxxxxxx VER:MMMMM.mmmmm)");
	iName = (char*)malloc(l + 1);
	assert(iName != 0);
	sprintf(iName, "%s (UID3:%08lx HWVD:%08lx VER:%ld.%ld)", a->iFileName, a->iHdr->iUid3,
		a->iHardwareVariant.ReturnVariant(), a->iHdr->ModuleVersion()>>16, a->iHdr->ModuleVersion()&0x0000ffffu);
	}

// Generate name as follows:
// Bare name (UID3:xxxxxxxx HWVD:xxxxxxxx VER:M.m)
TModuleName::TModuleName(const TDllFindInfo& a)
	{
	TInt l = strlen(a.iBareName) + strlen(" (UID3:xxxxxxxx HWVD:xxxxxxxx VER:MMMMM.mmmmm)");
	iName = (char*)malloc(l + 1);
	assert(iName != 0);
	sprintf(iName, "%s (UID3:%08lx HWVD:%08lx VER:%ld.%ld)", a.iBareName, a.iUid3,
		a.iHwVariant, a.iModuleVersion>>16, a.iModuleVersion&0x0000ffffu);
	}

// Generate name as follows:
// Name (UID3:xxxxxxxx HWVD:xxxxxxxx VER:M.m)
TModuleName::TModuleName(const TRomNode& a)
	{
	TBool xip = (a.iAtt & KEntryAttXIP);
	TUint32 uid3 = xip ? a.Uid3() : 0;
	TUint32 hwvd = (TUint)a.HardwareVariant();
	TUint32 ver = xip ? a.ModuleVersion() : 0;
	TInt l = a.FullNameLength() + strlen(" (UID3:xxxxxxxx HWVD:xxxxxxxx VER:MMMMM.mmmmm)");
	iName = (char*)malloc(l + 1);
	assert(iName != 0);
	char* b = iName + a.GetFullName(iName);
	sprintf(b, " (UID3:%08lx HWVD:%08lx VER:%ld.%ld)", uid3, hwvd, ver>>16, ver&0x0000ffffu);
	}

TModuleName::TModuleName(const TRomFile& a, const TRomNode* aRootDir)
	{
	iName = 0;
	if (a.iRbEntry)
		{
		new (this) TModuleName(a.iRbEntry);
		return;
		}
	TRomNode* x = aRootDir->iNextExecutable;
	for(; x; x=x->iNextExecutable)
		{
		if (x->iRomFile == &a)
			{
			new (this) TModuleName(*x);
			return;
			}
		}
	}

TModuleName::~TModuleName()
	{
	free(iName);
	}


/**
 * TRomNode::FindImageFileByName is always called on the root TRomNode, so
 * it doesn't consider the current TRomNode, just the linked items in the
 * iNextExecutable list.
 */
TRomNode* TRomNode::FindImageFileByName(const TDllFindInfo& aInfo, TBool aPrintDiag, TBool& aFallBack)
	{
	TUint r_major = aInfo.iModuleVersion >> 16;
	TUint r_minor = aInfo.iModuleVersion & 0x0000ffffu;
	TRomNode* fallback = NULL;
	aFallBack = EFalse;
	for (TRomNode* x=iNextExecutable; x!=0; x=x->iNextExecutable)
		{
		if (stricmp(x->BareName(), aInfo.iBareName))
			continue;	// name doesn't match
		if (aPrintDiag)
			Print(ELog, "Candidate: %s ", (const char*)TModuleName(*x) );
		if ( !(THardwareVariant(aInfo.iHwVariant) <= x->HardwareVariant()) )
			{
			if (aPrintDiag)
				Print(ELog, "HWVD mismatch - requested %08x\n", aInfo.iHwVariant);
			continue;
			}
		if (aInfo.iUid3 && (aInfo.iUid3 != x->Uid3()))
			{
			if (aPrintDiag)
				Print(ELog, "UID3 mismatch - requested %08x\n", aInfo.iUid3);
			continue;
			}
		TUint x_major = x->ModuleVersion() >> 16;
		TUint x_minor = x->ModuleVersion() & 0x0000ffffu;
		if ( x->ModuleVersion() == 0x00010000 && aInfo.iModuleVersion == 0 )
			{
			// allow requested version 0.0 to link to 1.0 with a warning
			fallback = x;
			}
		if ( x_major != r_major )
			{
			if (aPrintDiag)
				Print(ELog, "Major version mismatch - requested %d\n", r_major);
			continue;
			}
		if ( x_minor < r_minor )
			{
			if (aPrintDiag)
				Print(ELog, "??? Minor version mismatch - requested %d\n", r_minor);
			continue;
			}
		if (aPrintDiag)
			Print(ELog, "OK\n");
		return x;
		}
	if (fallback)
		{
		aFallBack = ETrue;
		return fallback;
		}
	return 0;
	}

TInt TRomNode::CheckForVersionConflicts(const TRomBuilderEntry* a)
	{
	TUint r_major = a->iHdr->ModuleVersion() >> 16;
	TUint r_minor = a->iHdr->ModuleVersion() & 0x0000ffffu;
	TInt errors = 0;
	for (TRomNode* x=iNextExecutable; x!=0; x=x->iNextExecutable)
		{
		if (x->iRomFile->iRbEntry == a)
			continue;	// don't compare a with itself
		if (stricmp(x->BareName(), a->iBareName))
			continue;	// name doesn't match
		if ( a->iHardwareVariant.MutuallyExclusive(x->HardwareVariant()) )
			continue;	// HWVDs are mutually exclusive
		if ( a->iHdr->iUid3 && x->Uid3() && (a->iHdr->iUid3 != x->Uid3()) )
			continue;	// UID3's don't match
		TUint x_major = x->ModuleVersion() >> 16;
		TUint x_minor = x->ModuleVersion() & 0x0000ffffu;
		if (x_major == r_major && x_minor != r_minor)	// allow two copies of same file
			{
			Print(EError, "Version Conflict %s with %s\n", (const char*)TModuleName(a), (const char*)TModuleName(*x) );
			++errors;
			}
		}
	return errors;
	}

TInt E32Rom::WriteHeadersToRom(char *anAddr)
//
// Follow the TRomBuilderEntry tree, writing TRomEntry headers to the rom.
//
	{
	TRACE(TTIMING,Print(EAlways,"0\n"));
	TRACE(TDIR,Print(EAlways,"WriteHeadersToRom()\n"));
	char* start=anAddr;
	TRomRootDirectoryList* dirPointers=(TRomRootDirectoryList*)anAddr;
	anAddr+=NumberOfVariants*sizeof(TRootDirInfo)+sizeof(TInt);
	RomFileStructure* pS=RomFileStructure::New(NumberOfVariants);
	if (!pS)
		Print(EError,"Error creating RomFileStructure\n");
	TInt r=pS->ProcessDirectory(iObey->iRootDirectory);
	if (r!=KErrNone)
		Print(EError,"Error %d processing directory tree\n",r);
	TInt c=pS->Count();
	NumRootDirs=c;
	if (c!=NumberOfVariants)
		Print(EError,"Error processing directory tree NR=%d NV=%d\n",c,NumberOfVariants);
	dirPointers->iNumRootDirs=c;
	TInt i;
	TRACE(TDIR,Print(EAlways,"Count=%d\n",c));
	TRACE(TDIR,pS->DebugPrint());
	for(i=0; i<c; i++)
		{
		DirEntry* pD=(DirEntry*)&(*pS)[i];
		TRomDir* pR=pD->CreateRomEntries(anAddr);
		dirPointers->iRootDir[i].iHardwareVariant=TUint(pD->Variants().Lookup());
		dirPointers->iRootDir[i].iAddressLin=ActualToRomAddress(pR);
		}
	TRACE(TDIR,Print(EAlways,"Beginning final cleanup\n"));
	delete pS;
	TRACE(TTIMING,Print(EAlways,"1\n"));
	return anAddr-start;
	}

 void TRomNode::Destroy()
//
// Follow the TRomNode tree, destroying it
//
	{

 	TRomNode *current = this; // root has no siblings
	while (current)
		{
		if (current->iChild)
			current->iChild->Destroy();
		TRomNode* prev=current;
		current=current->iSibling;
		delete prev;
		}
 	}


TInt TRomNode::SetAtt(TText *anAttWord)
//
// Set the file attribute byte from the letters passed
//
	{
	iAtt=0;
	if (anAttWord==0 || anAttWord[0]=='\0')
		return Print(EError, "Missing argument for keyword 'attrib'.\n");
	for (TText *letter=anAttWord;*letter!=0;letter++)
		{
		switch (*letter)
			{
		case 'R':
		case 'w':
			iAtt |= KEntryAttReadOnly;
			break;
		case 'r':
		case 'W':
			iAtt &= ~KEntryAttReadOnly;
			break;
		case 'H':
			iAtt |= KEntryAttHidden;
			break;
		case 'h':
			iAtt &= ~KEntryAttHidden;
			break;
		case 'S':
			iAtt |= KEntryAttSystem;
			break;
		case 's':
			iAtt &= ~KEntryAttSystem;
			break;
		default:
			return Print(EError, "Unrecognised attrib - '%c'.\n", *letter);
			break;
			}
		}
	return KErrNone;
	}

TRomBuilderEntry::TRomBuilderEntry(const char *aFileName,TText *aName)
//
// Constructor
//
	:
	E32ImageFile(),
	iName(0),
	iResource(EFalse), iNonXIP(EFalse), iPreferred(EFalse), iCompression(0), iPatched(EFalse),iArea(0),
	iOverrideFlags(0),iCodeAlignment(0),iDataAlignment(0),iUid1(0), iUid2(0), iUid3(0),iBareName(0), 
	iHardwareVariant(KVariantIndependent),iDataBssOffset(0xffffffff), 
	iStackReserve(0),iIATRefs(0), iNext(0), iNextInArea(0), 
	iRomImageFlags(0),iProcessName(0), iRomNode(NULL)
	{
	if (aFileName)
   		iFileName = NormaliseFileName((const char*)aFileName);
	if (aName)
		iName = (TText*)NormaliseFileName((const char*)aName);
	}

TRomBuilderEntry::~TRomBuilderEntry()
//
// Destructor
//
	{

	free(iFileName);
	iFileName = 0;
	free(iName);
	iName = 0;
	delete[] iProcessName;
	free(iBareName);
	iBareName = 0;
	delete[] iIATRefs;
	}

TInt isNumber(TText *aString)
	{
	if (aString==NULL)
		return 0;
	if (strlen((char *)aString)==0)
		return 0;
	return isdigit(aString[0]);
	}

TInt getNumber(TText *aStr)
	{
	TUint a;
	#ifdef __TOOLS2__
	istringstream val((char *)aStr);
	#else
	istrstream val((char *)aStr,strlen((char *)aStr));
	#endif

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	val >> setbase(0);
#endif //__MSVCDOTNET__

	val >> a;
	return a;
	}

TInt TRomBuilderEntry::SetCodeAlignment(TText* aStr)
	{
    if (!aStr || (aStr && isNumber(aStr)==0))
		return Print(EError, "Number required as argument for keyword 'code-align'.\n");
    iCodeAlignment=getNumber(aStr);
    return KErrNone;
	}

TInt TRomBuilderEntry::SetDataAlignment(TText* aStr)
	{
    if (!isNumber(aStr))
		return Print(EError, "Number required as argument for keyword 'data-align'.\n");
    TInt align=getNumber(aStr);
	if (align<0 || (align&0x0F) != 0)
		return Print(EError, "Positive multiple of 16 required for 'data-align'.\n");
	iDataAlignment=align;
    return KErrNone;
	}

TInt TRomBuilderEntry::SetRelocationAddress(TText *aStr)
	{
	if (aStr && isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'reloc'.\n");
	iOverrideFlags |= KOverrideAddress;
	iRelocationAddress=aStr ? getNumber(aStr) : 0xFFFFFFFF;
	return KErrNone;
	}

TInt TRomBuilderEntry::SetStackReserve(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'stackreserve'.\n");
	iOverrideFlags |= KOverrideStackReserve;
	iStackReserve=getNumber(aStr);
	return KErrNone;
	}

TInt TRomBuilderEntry::SetStackSize(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'stack'.\n");
	iOverrideFlags |= KOverrideStack;
	iStackSize=getNumber(aStr);
	return KErrNone;
	}

TInt TRomBuilderEntry::SetHeapSizeMin(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'heapmin'.\n");
	iOverrideFlags |= KOverrideHeapMin;
	iHeapSizeMin=getNumber(aStr);
	return KErrNone;
	}

TInt TRomBuilderEntry::SetHeapSizeMax(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'heapmax'.\n");
	iOverrideFlags |= KOverrideHeapMax;
	iHeapSizeMax=getNumber(aStr);
	return KErrNone;
	}

TInt TRomBuilderEntry::SetCapability(TText *aStr)
	{
	iOverrideFlags |= KOverrideCapability;
	if (isNumber(aStr))
		{
		Print(EDiagnostic,"Old style numeric CAPABILTY specification ignored.\n");
		return KErrNone;
		}
	return ParseCapabilitiesArg(iS.iCaps, (char*)aStr);
	}

TInt TRomBuilderEntry::SetPriority(TText *aStr)
	{
	if (isNumber(aStr))
		iPriority=(TProcessPriority)getNumber(aStr);
	else
		{
		char *str=(char *)aStr;
		if (stricmp(str, "low")==0)
			iPriority=EPriorityLow;
		else if (strnicmp(str, "background", 4)==0)
			iPriority=EPriorityBackground;
		else if (strnicmp(str, "foreground", 4)==0)
			iPriority=EPriorityForeground;
		else if (stricmp(str, "high")==0)
			iPriority=EPriorityHigh;
		else if (strnicmp(str, "windowserver",3)==0)
			iPriority=EPriorityWindowServer;
		else if (strnicmp(str, "fileserver",4)==0)
			iPriority=EPriorityFileServer;
		else if (strnicmp(str, "realtimeserver",4)==0)
			iPriority=EPriorityRealTimeServer;
		else if (strnicmp(str, "supervisor",3)==0)
			iPriority=EPrioritySupervisor;
		else
			return Print(EError, "Unrecognised priority keyword.\n");
		}
	if (iPriority<EPriorityLow || iPriority>EPrioritySupervisor)
		return Print(EError, "Priority out of range.\n");
	iOverrideFlags |= KOverridePriority;
	return KErrNone;
	}

TInt TRomBuilderEntry::SetUid1(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'uid1'.\n");
	iOverrideFlags |= KOverrideUid1;
	iUid1=getNumber(aStr);
	return KErrNone;
	}
TInt TRomBuilderEntry::SetUid2(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'uid2'.\n");
	iOverrideFlags |= KOverrideUid2;
	iUid2=getNumber(aStr);
	return KErrNone;
	}
TInt TRomBuilderEntry::SetUid3(TText *aStr)
	{
	if (isNumber(aStr)==0)
		return Print(EError, "Number required as argument for keyword 'uid3'.\n");
	iOverrideFlags |= KOverrideUid3;
	iUid3=getNumber(aStr);
	return KErrNone;
	}
TInt TRomBuilderEntry::SetCallEntryPoint(TBool aState)
	{
	if (aState)
		iOverrideFlags|=KOverrideCallEntryPoint;
	else
		iOverrideFlags|=KOverrideNoCallEntryPoint;
	return KErrNone;
	}

TInt TRomBuilderEntry::SetAttachProcess(TText *aStr)
	{
	const char* s=(const char*)aStr;
	TInt nd=0;
	if (*s=='\\')
		{
		++s;
		++nd;
		}
	TInt l=strlen(s);
	if (l==0)
		return KErrGeneral;
	const char* ss=s;
	while(*ss!=0 && *ss!='.') ++ss;
	int ext=*ss;	// 0 if no extension
	iProcessName=new TText[l+2+(ext?0:4)];
	char* d=(char *)iProcessName;
	strcpy(d+1, s);
	if (!ext)
		{
		char* t=d+1+l;
		*t++='.';
		*t++='e';
		*t++='x';
		*t++='e';
		*t++=0;
		}
	char* dd=d;
	int ind=nd;
	while(*dd)
		{
		while(*dd && *dd!='\\') ++dd;
		if (*dd=='\\')
			{
			*dd++=0;	// change \ to NUL
			++nd;		// count path elements
			}
		}
	if (!ind && nd)
		++nd;			// add initial \ if not present
	*d=(char)nd;
	return 0;
	}

TInt TRomBuilderEntry::OpenImageFile()
	{
	Print(ELog,"Loading E32Image file %s \n", iFileName);
	TInt err = Open(iFileName);
	if (err != KErrNone)
		{
		Print(EError,"File %s is not a valid E32Image file (error %d)\n", iFileName, err);
		return err;
		}
	TUint hdrfmt = iHdr->HeaderFormat();
	if (hdrfmt != KImageHdrFmt_V)
		{
		Print(EError,"%s: Can't load old format binary\n", iFileName);
		return KErrNotSupported;
		}
	E32ImageHeaderV* h = iHdr;
	
	// Overide any settings in the image file with those in the obey file
	if (iOverrideFlags & (KOverrideUid1|KOverrideUid2|KOverrideUid3))
		{
		TUint uid1 = h->iUid1;
		TUint uid2 = h->iUid2;
		TUint uid3 = h->iUid3;
		if (iOverrideFlags & KOverrideUid1)
			uid1 = iUid1;
		if (iOverrideFlags & KOverrideUid2)
			uid2 = iUid2;
		if (iOverrideFlags & KOverrideUid3)
			uid3 = iUid3;
		SetUids(TUid::Uid(uid1), TUid::Uid(uid2), TUid::Uid(uid3));
		}
	if (iOverrideFlags & KOverrideStack)
		h->iStackSize = iStackSize;
	if (iOverrideFlags & KOverrideHeapMax)
		h->iHeapSizeMax = iHeapSizeMax;
	if (iOverrideFlags & KOverrideHeapMin)
		h->iHeapSizeMin = iHeapSizeMin;
	if (iOverrideFlags & KOverridePriority)
		h->iProcessPriority = (TUint16)iPriority;
	if (iOverrideFlags & KOverrideCapability)
		h->iS.iCaps = iS.iCaps;
	for (TInt i=0; i<SCapabilitySet::ENCapW; ++i)
		{
		h->iS.iCaps[i] |= gPlatSecDisabledCaps[i];
		h->iS.iCaps[i] &= gPlatSecAllCaps[i];
		}

	if (iOverrideFlags & KOverrideCodePaged)
		{
		h->iFlags &= ~KImageCodeUnpaged;
		h->iFlags |= KImageCodePaged;
		}
	if (iOverrideFlags & KOverrideCodeUnpaged)
		{
		h->iFlags |= KImageCodeUnpaged;
		h->iFlags &= ~KImageCodePaged;
		}

	if ((TInt)h->iUid1 == KExecutableImageUidValue)
		{
		if (iOverrideFlags & KOverrideDataPaged)
			{
			h->iFlags &= ~KImageDataUnpaged;
			h->iFlags |= KImageDataPaged;
			}
		if (iOverrideFlags & KOverrideDataUnpaged)
			{
			h->iFlags |= KImageDataUnpaged;
			h->iFlags &= ~KImageDataPaged;
			}
		}

	switch(gCodePagingOverride)
		{
	case EKernelConfigPagingPolicyNoPaging:
		h->iFlags |= KImageCodeUnpaged;
		h->iFlags &= ~KImageCodePaged;
		break;
	case EKernelConfigPagingPolicyAlwaysPage:
		h->iFlags |= KImageCodePaged;
		h->iFlags &= ~KImageCodeUnpaged;
		break;
	case EKernelConfigPagingPolicyDefaultUnpaged:
		if(!(h->iFlags&(KImageCodeUnpaged|KImageCodePaged)))
			h->iFlags |= KImageCodeUnpaged;
		break;
	case EKernelConfigPagingPolicyDefaultPaged:
		if(!(h->iFlags&(KImageCodeUnpaged|KImageCodePaged)))
			h->iFlags |= KImageCodePaged;
		break;
		}
	switch(gDataPagingOverride)
		{
	case EKernelConfigPagingPolicyNoPaging:
		h->iFlags |= KImageDataUnpaged;
		h->iFlags &= ~KImageDataPaged;
		break;
	case EKernelConfigPagingPolicyAlwaysPage:
		h->iFlags |= KImageDataPaged;
		h->iFlags &= ~KImageDataUnpaged;
		break;
	case EKernelConfigPagingPolicyDefaultUnpaged:
		if(!(h->iFlags&(KImageDataUnpaged|KImageDataPaged)))
			h->iFlags |= KImageDataUnpaged;
		break;
	case EKernelConfigPagingPolicyDefaultPaged:
		if(!(h->iFlags&(KImageDataUnpaged|KImageDataPaged)))
			h->iFlags |= KImageDataPaged;
		break;
		}
	
	h->iCompressionType=KUidCompressionDeflate; // XIP images are always uncompressed
	
	Print(ELog,"\t\tcompression format:0x%08x \n", h->iCompressionType);
	if (gLogLevel & LOG_LEVEL_COMPRESSION_INFO)
		Print(ELog,"\t\tgCompress:%d, gCompressionMethod: 0x%08x \n", gCompress , gCompressionMethod);


	// Check the uids

	if ((TInt)h->iUid1 != KExecutableImageUidValue && (TInt)h->iUid1 != KDynamicLibraryUidValue) 
		return Print(EError, "First Uid for %s is not KExecutableImageUid or KDynamicLibraryUid\n", iFileName);

	// Set up the sizes and location of the distinct areas

	iHeaderRange.iSize = sizeof(TRomImageHeader);

	iCodeSection.iSize    = h->iTextSize;
	iCodeSection.iFilePtr = iData + iOrigHdr->iCodeOffset;

	TUint impfmt = h->ImportFormat();

	if (impfmt==KImageImpFmt_PE || impfmt==KImageImpFmt_PE2)
		{
		TInt nimports = NumberOfImports();
		if (nimports)
			{
			iImportAddressTableSection.iSize    = (nimports+1)*4;
			iImportAddressTableSection.iFilePtr = iData + iOrigHdr->iCodeOffset + iOrigHdr->iTextSize;
			iIATRefs = new TLinAddr*[nimports+1];
			memcpy(iIATRefs, iImportAddressTableSection.iFilePtr, (nimports+1)*sizeof(TLinAddr*));
			}

		if (h->iExportDirCount)
			{
			iExportDirSection.iSize    = h->iExportDirCount*4;
			iExportDirSection.iFilePtr = iData + iOrigHdr->iExportDirOffset;
			}

		// assertion - there's no rdata between IAT and Export Directory
		TInt rdatasize = h->iCodeSize;		// overall "readonly" size
		rdatasize -= iCodeSection.iSize;				// text
		rdatasize -= iImportAddressTableSection.iSize;	// IAT plus trailing 0
		rdatasize -= iExportDirSection.iSize;			// export data

		if (rdatasize != 0)
			{
			Print(EWarning, "Unexpected code in %s: %d bytes unexplained\n", iFileName, rdatasize);
			// expand the code to cover text+IAT+rdata
			iCodeSection.iSize = h->iCodeSize - iExportDirSection.iSize;
			}
		else
			{
			if (USE_IAT_FOR_IMPORTS)
				iCodeSection.iSize += iImportAddressTableSection.iSize;	// include IAT
			}
		}
	else
		{
		// ELF-derived images have no IAT and the export directory is included in the code section
		iImportAddressTableSection.iSize    = 0;
		iImportAddressTableSection.iFilePtr = NULL;
		iExportDirSection.iSize    = 0;
		iExportDirSection.iFilePtr = NULL;
		}

	if (h->iDataSize)
		{
		iDataSection.iSize    = h->iDataSize;
		iDataSection.iFilePtr = iData + iOrigHdr->iDataOffset;
		}

	iRomNode->iRomFile->iTotalDataBss = h->iDataSize + h->iBssSize;
	iS = h->iS;
	if (iVersionPresentInName && iVersionInName != h->ModuleVersion())
		{
		Print(EError,"%s: Version in name (%d.%d) does not match version in header (%d.%d)\n", iFileName,
			iVersionInName>>16, iVersionInName&0x0000ffffu, h->ModuleVersion()>>16, h->ModuleVersion()&0x0000ffffu);
		return KErrGeneral;
		}
	return KErrNone;
	}

TInt TRomNode::NameCpy(char* aDest)
//
// Safely copy a file name in the rom entry
//
	{

	if ((aDest==NULL) || (iName==NULL))
		return 0;
	if (Unicode)
		{
		const unsigned char* pSourceByte=iName;
		unsigned char* pTargetByte=(unsigned char*)aDest;
		for (;;)
			{
			const TUint sourceByte=*pSourceByte;
			if (sourceByte==0)
				{
				*pTargetByte=0;
				*(pTargetByte+1)=0;
				break;
				}
			if ((sourceByte&0x80)==0)
				{
				*pTargetByte=(unsigned char)sourceByte;
				++pTargetByte;
				*pTargetByte=0;
				++pTargetByte;
				++pSourceByte;
				}
			else if ((sourceByte&0xe0)==0xc0)
				{
				++pSourceByte;
				const TUint secondSourceByte=*pSourceByte;
				if ((secondSourceByte&0xc0)!=0x80)
					{
					Print(EError, "Bad UTF-8 '%s'", iName);
					exit(671);
					}
				*pTargetByte=(unsigned char)((secondSourceByte&0x3f)|((sourceByte&0x03)<<6));
				++pTargetByte;
				*pTargetByte=(unsigned char)((sourceByte>>2)&0x07);
				++pTargetByte;
				++pSourceByte;
				}
			else if ((sourceByte&0xf0)==0xe0)
				{
				++pSourceByte;
				const TUint secondSourceByte=*pSourceByte;
				if ((secondSourceByte&0xc0)!=0x80)
					{
					Print(EError, "Bad UTF-8 '%s'", iName);
					exit(672);
					}
				++pSourceByte;
				const TUint thirdSourceByte=*pSourceByte;
				if ((thirdSourceByte&0xc0)!=0x80)
					{
					Print(EError, "Bad UTF-8 '%s'", iName);
					exit(673);
					}
				*pTargetByte=(unsigned char)((thirdSourceByte&0x3f)|((secondSourceByte&0x03)<<6));
				++pTargetByte;
				*pTargetByte=(unsigned char)(((secondSourceByte>>2)&0x0f)|((sourceByte&0x0f)<<4));
				++pTargetByte;
				++pSourceByte;
				}
			else
				{
				Print(EError, "Bad UTF-8 '%s'", iName);
				exit(674);
				}
			}
		const TInt numberOfBytesInTarget=(pTargetByte-(unsigned char*)aDest); // this number excludes the trailing null-terminator
		if (numberOfBytesInTarget%2!=0)
			{
			Print(EError, "Internal error");
			exit(675);
			}
		return numberOfBytesInTarget/2; // returns the length of aDest (in UTF-16 characters for Unicode, not bytes)
		}
	strcpy(aDest,(const char*)iName);
	return strlen((const char*)iName);
	}

TInt TRomBuilderEntry::SizeInRom()
//
// Approximate the required size of the file when rommed
//
	{

	TInt size1, size2;
	SizeInSections(size1,size2);
	return size1+size2;
	}

void TRomBuilderEntry::LoadToRom()
//
//
//
	{
	// Copy fixed stuff into iRomImageHeader
	E32ImageHeaderV* h = iHdr;
	const TUint KRomFlagMask = KImageDll | KImageNoCallEntryPoint | KImageFixedAddressExe | KImageNmdExpData | KImageDataPagingMask;
	TUint romflags = h->iFlags & KRomFlagMask;
	TUint abi = h->ABI();
	TUint ept = h->EntryPointFormat();
	TUint impfmt = h->ImportFormat();
	romflags |= (abi | ept);

	iRomImageHeader = (TRomImageHeader*)iHeaderRange.iImagePtr;

	iRomImageHeader->iUid1              = h->iUid1;
	iRomImageHeader->iUid2              = h->iUid2;
	iRomImageHeader->iUid3              = h->iUid3;
	iRomImageHeader->iUidChecksum       = h->iUidChecksum;
	iRomImageHeader->iEntryPoint        = iCodeSection.iRunAddr + h->iEntryPoint;
	iRomImageHeader->iCodeAddress       = iCodeSection.iRunAddr;
	iRomImageHeader->iDataAddress       = iDataSection.iRunAddr;
	iRomImageHeader->iCodeSize          = iCodeSection.iSize+iExportDirSection.iSize;
	iRomImageHeader->iTextSize          = iCodeSection.iSize;
	iRomImageHeader->iDataSize          = iDataSection.iSize;
	iRomImageHeader->iBssSize           = h->iBssSize;
	iRomImageHeader->iTotalDataSize		= iRomNode->iRomFile->iTotalDataBss;
	iRomImageHeader->iHeapSizeMin       = h->iHeapSizeMin;
	iRomImageHeader->iHeapSizeMax       = h->iHeapSizeMax;
	iRomImageHeader->iStackSize         = h->iStackSize;
	iRomImageHeader->iDllRefTable       = (TDllRefTable*)(iDllRefTableRange.iImageAddr);
	iRomImageHeader->iExportDirCount    = h->iExportDirCount;
	iRomImageHeader->iExportDir         = (impfmt==KImageImpFmt_ELF) ?
											iCodeSection.iRunAddr + (h->iExportDirOffset - h->iCodeOffset)
											: iExportDirSection.iRunAddr;
	iRomImageHeader->iS					= h->iS;
	iRomImageHeader->iToolsVersion		= h->iToolsVersion;
	iRomImageHeader->iModuleVersion		= h->ModuleVersion();
	iRomImageHeader->iFlags             = romflags | iRomImageFlags;
	iRomImageHeader->iPriority          = h->ProcessPriority();
	iRomImageHeader->iDataBssLinearBase = iDataBssLinearBase;
	iRomImageHeader->iNextExtension     = 0;
	iRomImageHeader->iHardwareVariant   = iHardwareVariant;
	iRomImageHeader->iExceptionDescriptor = 0;
	TUint32 xd = h->iExceptionDescriptor;
	if ((xd & 1) && (xd != 0xffffffffu))
		iRomImageHeader->iExceptionDescriptor = (xd & ~1) + iRomImageHeader->iCodeAddress;

	if (iPreferred)
		{
		iRomImageHeader->iModuleVersion	&= ~0xffffu;
		iRomImageHeader->iModuleVersion	|= 0x8000u;
		}

	// Relocate the file to reflect the new addresses
	Relocate();

	// Copy the sections
	iCodeSection.Load();
	iExportDirSection.Load();
	iDataSection.Load();
	}

void TRomBuilderEntry::Relocate()
//
// Relocates the iData to new Code and Data addresses
//	
	{
	TUint codeDelta=iRomImageHeader->iCodeAddress       - iHdr->iCodeBase;
	TUint dataDelta=iRomImageHeader->iDataBssLinearBase - iHdr->iDataBase;

	// code section (text, IAT, export directory)

	if (iOrigHdr->iCodeRelocOffset)
		RelocateSection(iData + iOrigHdr->iCodeOffset, iData + iOrigHdr->iCodeRelocOffset,
			codeDelta, dataDelta, (char*)iCodeSection.iImagePtr, iIATRefs);

	// data section 

	if (iOrigHdr->iDataRelocOffset)
		RelocateSection(iData + iOrigHdr->iDataOffset, iData + iOrigHdr->iDataRelocOffset, 
			codeDelta, dataDelta, (char*)iDataSection.iImagePtr, iIATRefs);

	// export directory (only for PE-derived files)
	if (iExportDirSection.iSize)
		{
		TLinAddr* ptr=(TLinAddr*)(iData + iOrigHdr->iExportDirOffset);

		TLinAddr textStart = iHdr->iCodeBase;
		TLinAddr textFinish = textStart + iHdr->iTextSize;
		TLinAddr dataStart = textStart + iHdr->iCodeSize;
		TLinAddr dataFinish = dataStart + iHdr->iDataSize + iHdr->iBssSize;
	 
		TInt i;
		for (i=0; i<iHdr->iExportDirCount; i++, ptr++)
			{
			TLinAddr data=*ptr+textStart;
			if ((data>=textStart) && (data<textFinish))
				*ptr=data+codeDelta; // export something from the text/rdata section
			else if ((data>=dataStart) && (data<dataFinish))
				*ptr=data+dataDelta; // export some data or bss item
			else
				{
				Print(EWarning, "Export directory in %s: item %d -> %08x, which is not text or data!\n", iFileName, i, data);
				*ptr=0x13;	// unlucky for some
				}
			}
		}

	// Replace absent exports with 0
	TLinAddr* ptr = (TLinAddr*)(iData + iOrigHdr->iExportDirOffset);
	TInt i;
	for (i=0; i<iHdr->iExportDirCount; i++, ptr++)
		{
		if ( !( iExportBitMap[i>>3] & (1u << (i&7)) ) )
			*ptr = 0;
		}

	// Update E32ImageHeader, in case we want to do this process again later

	iHdr->iCodeBase += codeDelta;
	iHdr->iDataBase += dataDelta;
	}




TInt TRomBuilderEntry::FixupImports(E32Rom& aRom)
//
// Modify the import stubs to point directly into the export directory of the corresponding DLLs
// using the back pointers captured by detecting relocations referring to the Import Address Table
// The old-style Import Address Table behaviour can be retained by specifying the "keepIAT" attribute.
//
	{
	if (iHdr->iImportOffset == 0)
		return KErrNone;	// nothing to do

	

	TUint impfmt = iHdr->ImportFormat();
	TUint my_abi = iHdr->ABI();
	TRACE(TIMPORT,Print(ELog,"%40s[%08x] flags %08x\n",iFileName,(TUint)iHardwareVariant,iHdr->iFlags));
	const E32ImportSection* importsection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
	TLinAddr **iatRef=iIATRefs;
	TAddressRange iatRange = iCodeSection;
	iatRange.Move(iHdr->iTextSize);
	if (USE_IAT_FOR_IMPORTS)
		{
		if (impfmt == KImageImpFmt_ELF)
			return Print(EError, "Can't retain IAT for %s since it never existed\n", iFileName);
		if (iRomSectionNumber==0 && aRom.iObey->iSectionPosition!=-1)
			return Print(EError, "Can't retain IAT for %s in first section - not yet implemented\n", iFileName);
		Print(ELog, "%s has IAT at %08x\n", iFileName, iatRange.iRunAddr);
		}
	const E32ImportBlock* b = (const E32ImportBlock*)(importsection + 1);
	TInt i = iHdr->iDllRefTableCount;
	TInt numberOfImports=0;
	TUint *impOrdinalP = (TUint*)iImportAddressTableSection.iFilePtr;	// points to original IAT in file
	while (i-->0)
		{
		char* dllname = (char*)importsection + b->iOffsetOfDllName;
		TDllFindInfo find_info(dllname, this);
		TBool fallback;
		TRomNode* romnode = aRom.FindImageFileByName(find_info, EFalse, fallback);
		if (!romnode)
			{
			Print(EError, "Can't fixup imports for\n\t%s\nbecause\n\t%s\nis not in rom.\n",
						 (const char*)TModuleName(this), (const char*)TModuleName(find_info));
			aRom.FindImageFileByName(find_info, ETrue, fallback);
			return KErrGeneral;
			}
		TRomFile* dll=romnode->iRomFile;
		TRACE(TIMPORT,Print(ELog,"%s importing from %s\n", (const char*)TModuleName(this), (const char*)TModuleName(find_info)));
		if (romnode->ABI() != my_abi)
			{
			Print(EWarning, "File %s links to %s with different ABI\n", (const char*)TModuleName(this), (const char*)TModuleName(find_info));
			}
		TInt j;
		numberOfImports += b->iNumberOfImports;
		if (impfmt==KImageImpFmt_ELF)
			impOrdinalP = (TUint*)(b->Imports());	// for ELF must look in import block
		char* codeBase = (char*)iCodeSection.iImagePtr;
		for (j=0; j<b->iNumberOfImports; j++)
			{
			TLinAddr exportAddr = 0xdeadbeef;
			TLinAddr exporter = 0xdeadbeef;
			TUint impOrdinal = *impOrdinalP;
			TUint impOffset = 0;
			if (impfmt==KImageImpFmt_ELF)
				{
				TUint impd = *(TUint*)(codeBase + impOrdinal);
				impOrdinal = impd & 0xffff;
				impOffset = impd >> 16;
				}
			TRACE(TIMPORT,Print(ELog,"Ordinal %d\n", impOrdinal));
			TInt ret=dll->AddressFromOrdinal(exporter, exportAddr, impOrdinal);
			TRACE(TIMPORT,Print(ELog,"export %08x exporter %08x\n",exportAddr,exporter));
			if (ret!=KErrNone)
				{
				Print(EError, "%s wants ordinal %d from %s which only exports %d functions\n",
					iFileName, impOrdinal, 	(const char*)TModuleName(find_info), dll->ExportDirCount());
				exporter=0x13;	// unlucky for some...
				exportAddr=0x13;
				}
			else if (exportAddr == 0 && impOrdinal != 0)
				{
				Print(EError, "%s wants ordinal %d from %s which is absent\n",
					iFileName, impOrdinal, 	(const char*)TModuleName(find_info));
				exporter=0x13;	// unlucky for some...
				exportAddr=0x13;
				}
			if (USE_IAT_FOR_IMPORTS)
				{
				// must be PE-derived
				*iatRef=(unsigned long*)exportAddr; //iatRange.iRunAddr;					// point into IAT ...
				*(TLinAddr*)(iatRange.iImagePtr)=exportAddr;	// ... which has a copy of the export
				iatRange.Move(sizeof(TLinAddr));
				iatRef++;
				}
			else if (impfmt==KImageImpFmt_PE || impfmt==KImageImpFmt_PE2)
				{
				**iatRef=exporter;	// point directly into export directory
				iatRef++;
				}
			else
				{
				// ELF-derived
				*(TUint*)(codeBase + *impOrdinalP) = exportAddr + impOffset;
				}
			impOrdinalP++;
			}
		b = b->NextBlock(impfmt);
		}
	iImportCount=numberOfImports;
	return KErrNone;
	}

const char* KF32ProcessName="efile.exe";
const char* KWservProcessName="ewsrv.exe";
const char* KFbservProcessName="fbserv.exe";
const char* KMdaSvrProcessName="mediaserverstub.exe";
const char* KC32ProcessName="c32exe.exe";

const char* TRomBuilderEntry::GetDefaultAttachProcess()
//
// Work out the attach process from the file extension
//
// Only need to handle DLLs which run in F32, WSERV, FBSERV, MEDIASVR, C32
// F32:		FSY FXT
// WSERV:	ANI
// FBSERV:
// MDASVR:	MDA
// C32:		CSY, PRT, TSY, AGT, AGX
//
	{
	const char* s=(const char*)iName;
	TInt l=strlen(s);
	if (l<4 || s[l-4]!='.')
		return NULL;
	s+=(l-3);
	if (stricmp(s,"fsy")==0)
		return KF32ProcessName;
	if (stricmp(s,"fxt")==0)
		return KF32ProcessName;
	if (stricmp(s,"ani")==0)
		return KWservProcessName;
	if (stricmp(s,"mda")==0)
		return KMdaSvrProcessName;
	if (stricmp(s,"csy")==0)
		return KC32ProcessName;
	if (stricmp(s,"prt")==0)
		return KC32ProcessName;
	if (stricmp(s,"tsy")==0)
		return KC32ProcessName;
	if (stricmp(s,"agt")==0)
		return KC32ProcessName;
	if (stricmp(s,"agx")==0)
		return KC32ProcessName;
	return NULL;
	}

TInt TRomBuilderEntry::FindAttachProcess(E32Rom& aRom)
	{
	if (iRomImageFlags & (KRomImageFlagVariant|KRomImageFlagExtension|KRomImageFlagDevice))
		return KErrNone;
	const char* attp_name=(const char*)iProcessName;
	int nd=0;
	if (attp_name)
		nd=*attp_name++;
	else
		attp_name=GetDefaultAttachProcess();
	if (!attp_name)
		return KErrNone;
	TInt i;
	TUint my_abi = iHdr->ABI();
	TUint abi = 0;
	if (nd)
		{
		// path search
		TRomNode* rn=aRom.iObey->iRootDirectory;
		for (; nd; --nd)
			{
			rn=rn->FindInDirectory((TText*)attp_name);
			if (!rn)
				{
				Print(EError, "Invalid attach process name element %s\n", attp_name);
				return KErrGeneral;
				}
			attp_name+=(strlen(attp_name)+1);
			}
		iRomNode->iRomFile->iAttachProcess=rn->iRomFile;
		abi = iRomNode->iRomFile->iAttachProcess->ABI();
		}
	else
		{
		// filename only search
		for (i=0; i<aRom.iObey->iNumberOfPeFiles; i++)
			{
			TRomBuilderEntry* e=aRom.iPeFiles[i];
			abi = e->iHdr->ABI();
			if (stricmp((const char*)e->iName, attp_name)==0)
				{
				if (iRomNode->iRomFile->iAttachProcess)
					{
					Print(EError, "Ambiguous attach process name %s\n", attp_name);
					return KErrGeneral;
					}
				iRomNode->iRomFile->iAttachProcess=e->iRomNode->iRomFile;
				}
			}
		}
	if (abi != my_abi)
		{
		Print(EWarning, "File %s: Attach process has different ABI\n", (const char*)TModuleName(this));
		}
	return KErrNone;
	}

TInt TRomBuilderEntry::BuildDependenceGraph(E32Rom& aRom)
//
// Fill in the iDeps
//
	{
	TBool is_kernel = ((iRomImageFlags & KRomImageFlagsKernelMask) != 0);
	TRomNode* rn = iRomNode;
	TRomFile* rf = rn->iRomFile;
	TUint my_abi = iHdr->ABI();
	TUint impfmt = iHdr->ImportFormat();
	rf->iNumDeps = iHdr->iDllRefTableCount;
	if (IsDll() && aRom.iObey->iMemModel!=E_MM_Flexible && aRom.iObey->iMemModel!=E_MM_Multiple && (iHdr->iDataSize!=0 || iHdr->iBssSize!=0))
		{
		TInt r=FindAttachProcess(aRom);
		if (r!=KErrNone)
			return r;
		if (aRom.iObey->iMemModel==E_MM_Moving)
			{
			if (rf->iAttachProcess && !(rf->iAttachProcess->RomImageFlags() & KRomImageFlagFixedAddressExe))
				rf->iAttachProcess=NULL;	// ignore attach process if not fixed
			}
		}
	TRomFile* attp=rf->iAttachProcess;
	if (attp)
		++rf->iNumDeps;		// extra implicit dependence on process
	if (rf->iNumDeps)
		{
		rf->iDeps=new TRomFile* [rf->iNumDeps];
		memset(rf->iDeps, 0, rf->iNumDeps*sizeof(TRomFile*));
		}

	TInt err = KErrNone;
	const E32ImportSection* importSection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
	const E32ImportBlock* block = (const E32ImportBlock*)(importSection + 1);
	TInt i;
	for (i=0; i<iHdr->iDllRefTableCount; i++, block = block->NextBlock(impfmt), TRACE(TIMPORT,Print(ELog,"DllRef/dll done\n")) )
		{
		char* dllname = (char*)importSection + block->iOffsetOfDllName;
		TDllFindInfo find_info(dllname, this);
		TBool fallback;
		TRomNode* romnode = aRom.FindImageFileByName(find_info, EFalse, fallback);
		if (!romnode)
			{
			Print(EError, "Can't build dependence graph for\n\t%s\nbecause\n\t%s\nis not in rom.\n",
						 (const char*)TModuleName(this), (const char*)TModuleName(find_info));
			aRom.FindImageFileByName(find_info, ETrue, fallback);
			err = KErrNotFound;
			continue;
			}
		if (fallback)
			{
			Print(EWarning, "File %s links to %s\n\twhich is not in ROM. Version 1.0 of latter used instead.\n",
					 (const char*)TModuleName(this), (const char*)TModuleName(find_info));
			}
		TRACE(TIMPORT,Print(ELog,"%s links to %s\n", (const char*)TModuleName(this), (const char*)TModuleName(find_info)));
		TRACE(TIMPORT,Print(ELog,"Resolves to %s\n", (const char*)TModuleName(*romnode)));
		TBool dep_is_kernel = ((romnode->iRomFile->RomImageFlags() & KRomImageFlagsKernelMask) != 0);
		if (dep_is_kernel != is_kernel)
			{
			if (is_kernel)
				{
				Print(EError, "Kernel side executable\n\t%s\nlinks to user side executable\n\t%s\n",
									 (const char*)TModuleName(this), (const char*)TModuleName(find_info));
				}
			else
				{
				Print(EError, "User side executable\n\t%s\nlinks to kernel side executable\n\t%s\n",
									 (const char*)TModuleName(this), (const char*)TModuleName(find_info));
				}
			err = KErrGeneral;
			continue;
			} 
		// prevent the situiation which importer is primary, variant or extension, exporter is device
		if (is_kernel && !Device() && romnode->iRomFile->iRbEntry->Device())	
			{
			Print(EError, "Kernel/variant/extension\n\t%s\nlinks to non-extension LDD/PDD\n\t%s\n",
							(const char*)TModuleName(this), (const char*)TModuleName(find_info));
			err = KErrGeneral;
			continue;
			}
		if (romnode->ABI() != my_abi)
			{
			Print(EWarning, "File %s links to %s with different ABI\n", (const char*)TModuleName(this), (const char*)TModuleName(find_info));
			}
		rf->iDeps[i]=romnode->iRomFile;
		const SSecurityInfo& s1 = iHdr->iS;
		const SSecurityInfo& s2 = romnode->iRomFile->SecurityInfo();
		TInt r = CompareCapabilities(s1.iCaps, s2.iCaps, iFileName, dllname);
		if (r != KErrNone)
			err = r;
		if (romnode->iRomFile==attp)
			attp=NULL;
		}

	if (attp)
		rf->iDeps[rf->iNumDeps-1]=attp;
	TRACE(TIMPORT,Print(ELog,"BuildDep done all\n"));
	return err;
	}

TInt TRomBuilderEntry::ResolveDllRefTable(E32Rom& aRom)
//
// Fill in the DLLRefTable
//
	{
	TRomNode* rn = iRomNode;
	TRomFile* rf = rn->iRomFile;
	(void)aRom;
	if (rf->iNumPDeps==0)
		return KErrNone;	// nothing to do

	TDllRefTable* dllRefTable=(TDllRefTable*)(iDllRefTableRange.iImagePtr);

	TUint16 flags=0;

	dllRefTable->iFlags=flags;
	dllRefTable->iNumberOfEntries=(TUint16)rf->iNumPDeps;

	TInt err = KErrNone;
	TInt i;
	for (i=0; i<rf->iNumPDeps; i++)
		{
		dllRefTable->iEntry[i]=(TRomImageHeader*)rf->iPDeps[i]->iAddresses.iRunAddr;
		}
	TRACE(TIMPORT,Print(ELog,"DllRef done all\n"));
	return err;
	}

void TRomBuilderEntry::DisplaySize(TPrintType aWhere)
	{
	if(gLogLevel > DEFAULT_LOG_LEVEL){

		if(gLogLevel & LOG_LEVEL_FILE_DETAILS)
		{
			// More detailed information about file name in .
			TBool aIgnoreHiddenAttrib = ETrue;
			TInt aLen = iRomNode->FullNameLength(aIgnoreHiddenAttrib);
			char * aBuf = new char[aLen+1];
			iRomNode->GetFullName(aBuf, aIgnoreHiddenAttrib);
			if (iPatched|iRomNode->iHidden)
				Print(aWhere, "%s\t%d\t%s\t%s\n", iFileName, SizeInRom(), iPatched?"patched":"hidden", aBuf);
			else
				Print(aWhere, "%s\t%d\t%s\n", iFileName, SizeInRom(), aBuf);
			delete[] aBuf;
		}
	}
	else{
		if (iPatched|iRomNode->iHidden)
			Print(aWhere, "%s\t%d\t%s\n", iFileName, SizeInRom(), iPatched?"patched":"hidden");
		else
			Print(aWhere, "%s\t%d\n", iFileName, SizeInRom());
	}
	}

/**
 * TRomFile iRomEntry is a linked list through the various
 * distinct TRomEntry objects which may exist for the associated file
 * due to variant processing / aliasing
 */

void TRomFile::SetRomEntry(TRomEntry* aEntry)
	{
	// Need to add to the tail of the list, for backwards compatibility
	// Adding to the front of the list changes the iPrimary and iSecondary
	// values in the TRomHeader when multiple variants are present

	if (iFinal)
		return;			// address already established so no fixup required
	if (iRomEntry==0)
		{
		iRomEntry = aEntry;
		return;
		}
	TRomEntry* entry = iRomEntry;
	while (entry->iAddressLin != 0)
		entry = (TRomEntry*)entry->iAddressLin;
	entry->iAddressLin = (TLinAddr)aEntry;
	}

void TRomBuilderEntry::FixupRomEntries(TInt aSize)
	{
	if (iPatched)
		return;		// patched files don't appear in the ROM file system

	iRomNode->Finalise(aSize);
	}

/**
 * TRomNode::Finalise updates the associated TRomEntry objects with the final size and
 * linear address information supplied by the TRomBuilderEntry. It also stores the
 * summary information for resolving static linkages to this executable (if appropriate)
 * and adjusts the TRomNodes so that they are identical to the ones which would be 
 * obtained by walking the directory structure in an existing ROM.
 */
void TRomNode::Finalise(TInt aSize)
	{
	TRACE(TIMPORT,Print(ELog,"TRomNode %s Finalise %08x %d\n", (const char*)TModuleName(*this), iRomFile->iFinal));
	iRomFile->Finalise(aSize);
	}

/**
 * TRomFile::Finalise updates the associated TRomEntry objects with the final size and
 * linear address information supplied by the TRomBuilderEntry. It also stores the
 * summary information for resolving static linkages to this executable (if appropriate)
 * and adjusts the TRomFiles so that they are identical to the ones which would be 
 * obtained by walking the directory structure in an existing ROM.
 */
void TRomFile::Finalise(TInt aSize)
	{
	if (iFinal)
		return;
	TLinAddr ra = iRbEntry->iHeaderRange.iImageAddr;
	TRomEntry* entry = iRomEntry;
	while (entry)
		{
		TRomEntry* next = (TRomEntry*)entry->iAddressLin;
		entry->iSize = aSize;
		entry->iAddressLin = ra;
		entry = next;
		}
	iAddresses = iRbEntry->iHeaderRange;
	iAddresses.iSize = aSize;
	if ((!iRbEntry->iResource) && (!iRbEntry->HCRDataFile()))
		{
		iExportDir = iAddresses;
		iExportDir.iSize = iRbEntry->iHdr->iExportDirCount * sizeof(TLinAddr);
		iExportDir.Move(RomImgHdr()->iExportDir - iAddresses.iRunAddr);
		}
	iRbEntry = 0;
	iFinal = ETrue;
	}

void TRomBuilderEntry::SetRomNode(TRomNode* aNode)
	{
	iRomNode = aNode;
	}


void TImageSection::Load() const
	{ 
	if (iSize && iImagePtr && iFilePtr) 
		memcpy(iImagePtr,iFilePtr,iSize); 
	}

/**
 * TDllExportInfo is the information about a DLL which is necessary to
 * resolve a static link to that DLL. It all comes from the TRomImageHeader,
 * as it would with a static linkage resolved at runtime.
 */
TRomFile::TRomFile()
	{
	memset(this, 0, sizeof(TRomFile));
	iRefCount = 1;
	iHwvd = KVariantIndependent;
	iDataBssOffsetInExe = -1;
	}

TRomFile::~TRomFile()
	{
	delete[] iDeps;
	delete[] iPDeps;
	}

TInt TRomFile::AddressFromOrdinal(TLinAddr& aEDataAddr, TLinAddr& aExport, TUint aOrdinal)
//
// Get the export address of symbol aOrdinal
//
	{
	if(aOrdinal == 0)
	{
		aEDataAddr = iExportDir.iRunAddr -1 ;
		aExport = *(TLinAddr*)((TLinAddr*)iExportDir.iImagePtr - 1);
		if((TInt)aExport == ExportDirCount()) {
			aEDataAddr = 0;
			aExport = 0;
		}
		return KErrNone;
	}
	
	TUint index = aOrdinal - KOrdinalBase;
	if (index >= (TUint)ExportDirCount())
		return KErrNotFound;
	aEDataAddr = iExportDir.iRunAddr + index * sizeof(TLinAddr);
	aExport = ((TLinAddr*)iExportDir.iImagePtr)[index];
	return KErrNone;
	}


bool TRomFile::ComputeSmpSafe(const TRomBuilderEntry* aRbEntry)
	{
	// A component is SMP safe if:
	//
	// 1. It's E32 image file is marked as SMP safe (MMP keyword SMPSAFE).
	// 2. All components it links to are SMP safe.
	//
	// This implies a recursive dependency structure.

	if (iSmpInfo.isInit)
		{
		// We have already visited this node.
		return iSmpInfo.isSafe;
		}

	// Mark this node as "active," meaning that we are currently evaluating it. We
	// use this to detect cycles in the dependency graph.
	iSmpInfo.isActive = 1;

	iSmpInfo.isSafe = 1;

	// Have we found any cycle in the graph?
	bool is_cycle = 0;

	if ( aRbEntry->iOrigHdr->iFlags & KImageSMPSafe )
		{
		// OK, the E32 file for this node is marked as SMPSAFE. Now we need to check
		// that all nodes we depend on are SMP safe.

		for (int i = 0; i < iNumDeps; i++)
			{
			TRomFile* e = iDeps[i];

			assert(this != e);

			if (e->iSmpInfo.isActive)
				{
				is_cycle = 1;
				}
			else if ( ! e->ComputeSmpSafe(e->iRbEntry) )
				{
				if (gLogLevel & LOG_LEVEL_SMP_INFO)
					{
					Print(ELog,"SMP-unsafe: %s: links to unsafe component %s.\n",
							aRbEntry->iBareName , e->iRbEntry->iBareName);
					}

				iSmpInfo.isSafe = 0;
				break;
				}
			}
		}
	else
		{
		if (gLogLevel & LOG_LEVEL_SMP_INFO)
			{
			Print(ELog,"SMP-unsafe: %s: MMP keyword SMPSAFE not used.\n", aRbEntry->iBareName);
			}

		iSmpInfo.isSafe = 0;
		}

	iSmpInfo.isActive = 0;

	if (!iSmpInfo.isSafe || !is_cycle)
		{
		iSmpInfo.isInit = 1;
		}

	return iSmpInfo.isSafe;
	}

/**
 * TRomNode::CopyDirectory performs a deep copy of the TRomNode structure
 */
TRomNode* TRomNode::CopyDirectory(TRomNode*& aLastExecutable, TRomNode* aParent)
	{
	if (iHidden && iChild==0)
		{
		// Hidden file - do not copy (as it wouldn't be visible in the ROM filestructure)
		if (iSibling)
			return iSibling->CopyDirectory(aLastExecutable, aParent);
		else
			return 0;
		}

	TRomNode* copy = new TRomNode(*this);
	copy->iParent = aParent;
	if(aLastExecutable==0)
		aLastExecutable = copy;		// this must be the root of the structure

	// recursively copy the sub-structures
	if (iChild)
		copy->iChild = iChild->CopyDirectory(aLastExecutable, copy);
	if (iSibling)
		copy->iSibling = iSibling->CopyDirectory(aLastExecutable, aParent);

	if (copy->iAtt & KEntryAttXIP)
		AddExecutableFile(aLastExecutable,copy);
	return copy;
	}

TInt TRomNode::Alias(TRomNode* aNode, TRomNode*& aLastExecutable)
	{
	if (aNode->iAtt & KEntryAttXIP)
		AddExecutableFile(aLastExecutable,this);
	return SetBareName();
	}

TInt TRomNode::Rename(TRomNode *aOldParent, TRomNode* aNewParent, TText* aNewName)
	{
	aOldParent->Remove(this);
	aNewParent->Add(this);
	free(iName);
	iName = (TText*)NormaliseFileName((const char*)aNewName);
	return SetBareName();
	}

TInt TRomNode::SetBareName()
	{
	free(iBareName);
	TUint32 uid;
	TUint32 vin;
	TUint32 flg;
	iBareName = SplitFileName((const char*)iName, uid, vin, flg);
	if (uid || (flg & EUidPresent))
		return KErrBadName;
	if (strchr(iBareName, '{') || strchr(iBareName, '}'))
		return KErrBadName;
	if ((iAtt & KEntryAttXIP) && (flg & EVerPresent))
		{
		TUint32 ver = iRomFile->ModuleVersion();
		if (ver != vin)
			return KErrArgument;
		}
	return KErrNone;
	}
