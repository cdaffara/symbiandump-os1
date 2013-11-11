// iocons.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32cons.h>
#include <fshell/iocli.h>
#include <fshell/heaputils.h>
using LtkUtils::RAllocatorHelper;

#ifdef EKA2
NONSHARABLE_CLASS(CIoConsole)
#else
class CIoConsole
#endif
	 : public CColorConsoleBase
	{
public:
	CIoConsole(); // Only for use by iocons.dll
	CIoConsole(RIoConsoleReadHandle& aReadHandle, RIoConsoleWriteHandle& aWriteHandle); // for use by CCommandBase

public: // From CConsoleBase.
	virtual ~CIoConsole();
	virtual TInt Create(const TDesC& aTitle, TSize aSize);
	virtual void Read(TRequestStatus& aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC& aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint& aPoint);
	virtual void SetCursorPosRel(const TPoint& aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC& aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
public: // From CColorConsoleBase
	virtual void SetTextAttribute(TTextAttribute aAttribute);

private:
	TInt CreateNewConsole(const TDesC& aTitle, TSize aSize);
	TInt FindClientThreadId(TThreadId& aThreadId);
	static TBool HeapWalk(TAny* aSelf, RAllocatorHelper::TCellType aCellType, TLinAddr aCellPtr, TInt aCellLength);
private:
	RIoSession iIoSession;
	RIoConsoleReadHandle iReadHandle;
	RIoConsoleWriteHandle iWriteHandle;
	RIoConsole iConsole;
	TInt iHeapCellCount;
	TAny* iServerAddress;
	};

