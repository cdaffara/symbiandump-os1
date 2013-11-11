// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32svr.h>
#include <f32file.h>

_LIT(KBlankLine, "\r");

class TDll
	{
public:
	TDll(const TDesC& aName, TUint8* aBase, TInt aSize)
		: iName(aName), iBase((TUint)aBase), iSize((TUint)aSize)
		{}

	TFullName iName;
	TUint iBase;
	TUint iSize;
	};

class TDllList
	{
public:
	static void FindDlls();
	static void ResetAndDestroy();
	static void ListDlls(TFileText& aTextFile);
	static TInt Match(TUint anAddr, const TDll*& aDll);

private:
	static RPointerArray<TDll> iList;
	};

RPointerArray<TDll> TDllList::iList(10);

void TDllList::ResetAndDestroy()
	{
	iList.ResetAndDestroy();
	}

void TDllList::ListDlls(TFileText& aTextFile)
	{
	TBuf<0x100> line;

	_LIT(KDllFormat, "%08x-%08x %S\r");

	aTextFile.Write(KBlankLine);
	for (TInt i=0; i<iList.Count(); i++)
		{
		const TDll& dll=*(iList[i]);
		line.Format(KDllFormat, dll.iBase, dll.iBase+dll.iSize-1, &dll.iName);
		aTextFile.Write(line);
		}
	}

TInt TDllList::Match(TUint anAddr, const TDll*& aDll)
	{
	for (TInt i=0; i<iList.Count(); i++)
		{
		aDll=iList[i];
		if (aDll->iBase<=anAddr && aDll->iBase+aDll->iSize > anAddr)
			return KErrNone;
		}
	return KErrNotFound;
	}


void TDllList::FindDlls()
//
// For each library known to the system, look for a chunk of the same name
//
	{
	TFindLibrary findLib;
	TFullName libName;

	while (findLib.Next(libName)==KErrNone)
		{
		TFindChunk findChunk(libName);
		TFullName chunkName;
		if (findChunk.Next(chunkName)!=KErrNone)
			continue;
		RChunk chunk;
		if (chunk.Open(findChunk)!=KErrNone)
			continue;
		TUint8* base=chunk.Base();
		TInt size=chunk.Size();
		chunk.Close();
		TDll* dllptr=new TDll(libName,base,size);
		if (dllptr!=0)
			iList.Append(dllptr);
		}
	}

_LIT(KFormatStackInfo,"Stack %08x-%08x (? %d?), sp=%08x\r");

HBufC8* GrabStack(const TDesC& /*aLine1*/, TThreadId aId, TUint aSp, TInt& aStackBase, TInt& aStackSize)
	{
	TInt err=KErrNone;
#if 0
	// Defect in E32 162 which means that RThread::GetRamSizes will always reset the machine!
	RThread thread;
	if (thread.Open(aId)!=KErrNone)
		return 0;

	TInt heapsize=0;
	err=thread.GetRamSizes(heapsize,aStackSize);
	thread.Close();
	
	if (err!=KErrNone)
		return 0;

	// Sanity check
	aStackBase = aSp & ~((1024*1024)-1);
	if (aStackSize<0 || aStackSize>=1024*1024 || aSp>(TUint)(aStackBase+aStackSize))
		{
		aStackBase=0;	// indicates a daft stack pointer
		return 0;
		}
#else
	// Sanity check & guess at stack size
	aStackBase = aSp & ~((1024*1024)-1);
	aStackSize = (aSp - aStackBase + 4096) & ~4095;		// round up to a multiple of the page size
	if (aStackBase+aStackSize-aSp < 200)
			aStackSize += 4096;		// seems too small - risk another page!
	if (aStackSize<0 || aStackSize>=1024*1024)
		{
		aStackBase=0;	// indicates a daft stack pointer
		return 0;
		}
	aStackSize-=4;	// a clue to the wise that this is a guess...
#endif

	// Ask the user if we want to risk grabbing the stack...

	TBuf<0x100> line2;
	line2.Format(KFormatStackInfo, aStackBase, aStackBase+aStackSize-1, aStackSize, aSp);
    
	// OK - let stack grabbing commence
	HBufC8* stackbuf = HBufC8::New(aStackSize);
	if (stackbuf==0)
		return 0;

	TPtr8 stackdes(stackbuf->Des());
	err=RDebug::ReadMemory(aId,aStackBase,stackdes,aStackSize);
	if (err!=KErrNone)
		{
		delete stackbuf;
		stackbuf=0;
		}
	return stackbuf;
	}

GLDEF_C TInt E32Main()
//
// Reporting more detail of KERN-EXEC 3 errors
//
// Warning: This code uses fragile assumptions which may not be true
// in future releases of EPOC
//
// 1) EXEs are located at 0x20000000
// 2) the map file for an EXE puts the start of text at 400010
// 3) the map file for a DLL puts the start of text at 10000010
// 4) the EPOC ROM lives at address 0x50000000
// 5) EPOC stacks start at a megabyte boundary
//
	{
	TBuf<0x100> line1;
	TBuf<0x100> line2;
	SDebugInfo info;
	struct SRegisterInfo reginfo;
	TUint pc = 0;
	TUint regs[16];
	const TDll* faultDll = NULL;

	_LIT(KInfo1, "D_EXC started");
	User::InfoPrint(KInfo1);

	TRequestStatus stat(0);
//	FOREVER
	for (TInt rep=0; rep<2; rep++)	// die after two exceptions
		{
		TInt err = KErrNone;
		// wait for any thread to panic...


		err=RDebug::GetException(info,stat);
		if (err!=KErrNone)
			{
			_LIT(KInfo2, "RDebug failure");
			User::Panic(KInfo2, err);
			}

		User::WaitForRequest(stat);

		_LIT(KFormatPanic, "%S panic %S %d\r");
		_LIT(KFormatException, "%S exception type %d\r");
		_LIT(KFormatBreakpoint, "%S has breakpoint %d\r");


		switch (info.iDebugType)
			{
		case EPanic:
			line1.Format(KFormatPanic,&info.iName,&info.iCategory, info.iPanicType);
			break;
		case EException:
			line1.Format(KFormatException,&info.iName,info.iPanicType);
			break;
		case EBreakPoint:
			line1.Format(KFormatBreakpoint,&info.iName,info.iPanicType);
			break;
		default:
			continue;
			}

		// assume that it's KERN-EXEC 3 and try to use the 
		// full RDebug support to locate the faulting instruction

		HBufC8* stack = NULL;
		TInt stackbase=0;
		TInt stacksize=0;

		const TInt KStackPointerReg = 13;

		pc = 0x00000001;	// illegal value


		err = RDebug::Open();
		if (err==KErrNone)
			{
			err = RDebug::RegisterInfo(reginfo);
			if (!err)
				{
				RDebug::GetRegister(info.iId,reginfo.iNumberOfPcRegister, pc);
				for (int i=0; i<16; i++)
				    {
					RDebug::GetRegister(info.iId, i, regs[i]);
				    }
				}

			TDllList::FindDlls();

			stack=GrabStack(line1, info.iId, regs[KStackPointerReg], stackbase, stacksize);

			RDebug::Close();
			}


		_LIT(KFormatEXE,	"pc=%08x, .map equivalent %08x\r");
		_LIT(KFormatROM,	"pc=%08x, in ROM\r");
		_LIT(KFormatDll,	"pc=%08x, in DLL %S, .map equivalent %08x\r");
		_LIT(KFormatOther,	"pc=%08x, iCodeAddr=%08x\r");
		_LIT(KFormatError,	"(Unable to determine pc)\r");


		if ((pc&3) == 0)
			{
			if (pc >= 0x20000000 && pc < 0x30000000)
			    {
				line2.Format(KFormatEXE, pc, pc-0x20000000+0x400010);
			    }
			else if (pc >= 0x50000000 && pc < 0x60000000)
				{
                line2.Format(KFormatROM, pc);
				}
			else if (TDllList::Match(pc, faultDll)==KErrNone)
			    {
				line2.Format(KFormatDll, pc, &faultDll->iName, pc-(faultDll->iBase)+0x10000010);
			    }
			else
			    {
				line2.Format(KFormatOther, pc, info.iCodeAddr);
			    }
			}
		else
		    {
			line2.Copy(KFormatError);
		    }
		
		RFs fs;
		err = fs.Connect();
		if (err!=KErrNone)
		    {
			break;
		    }

		_LIT(KFormatFilename,"d:\\d_exc_%d.txt");
		_LIT(KFormatStackname,"d:\\d_exc_%d.stk");

		// Report the basic information about registers, DLLs etc

		TFileName name;
		name.Format(KFormatFilename, *(TUint*)&info.iId);

		RFile file;
		err=file.Replace(fs, name, EFileWrite+EFileShareAny+EFileStreamText);
		if (err!=KErrNone)
		    {
            fs.Close();
            break;
		    }

		TFileText textfile;
		textfile.Set(file);

		textfile.Write(line1);
		textfile.Write(line2);
		textfile.Write(KBlankLine);

		_LIT(KFormatRegisters,"r%02d=%08x %08x %08x %08x\r");

		line2.Format(KFormatRegisters, 0, regs[0], regs[1], regs[2], regs[3]);
		textfile.Write(line2);
		line2.Format(KFormatRegisters, 4, regs[4], regs[5], regs[6], regs[7]);
		textfile.Write(line2);
		line2.Format(KFormatRegisters, 8, regs[8], regs[9], regs[10], regs[11]);
		textfile.Write(line2);
		line2.Format(KFormatRegisters, 12,regs[12], regs[13], regs[14], regs[15]);
		textfile.Write(line2);

		if (stackbase!=0)
			{
			line2.Format(KFormatStackInfo, stackbase, stackbase+stacksize-1, stacksize, regs[KStackPointerReg]);
			textfile.Write(line2);
			}

		TDllList::ListDlls(textfile);
		TDllList::ResetAndDestroy();

		file.Close();

		// Dump the stack as binary data

		if (stack)
			{
			name.Format(KFormatStackname, *(TUint*)&info.iId);
			err=file.Replace(fs, name, EFileWrite+EFileShareAny+EFileStreamText);
			if (err==KErrNone)
				{
				file.Write(*stack);
				file.Close();
				}
			}
		delete stack;

		fs.Close();
		}
	return stat.Int();
	}


