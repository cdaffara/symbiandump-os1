// vtc_base.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __VT100CONS_BASE_H__
#define __VT100CONS_BASE_H__

#include <e32std.h>
#include <e32cons.h>
#include <fshell/settings.h>
#include <fshell/vtc_controller.h>
#include <fshell/consoleextensions.h>

class CVtcConsoleBase : public CConsoleBase, public MConsoleOutput, public MConsoleInput, public MIosrvConsoleHelper
	{
public:
	IMPORT_C virtual ~CVtcConsoleBase();
	IMPORT_C virtual TInt Create(const TDesC& aTitle, TSize aSize);
	IMPORT_C virtual void Read(TRequestStatus& aStatus);
	IMPORT_C virtual void ReadCancel();
	IMPORT_C virtual void Write(const TDesC& aDes);
	IMPORT_C virtual TPoint CursorPos() const;
	IMPORT_C virtual void SetCursorPosAbs(const TPoint& aPoint);
	IMPORT_C virtual void SetCursorPosRel(const TPoint& aPoint);
	IMPORT_C virtual void SetCursorHeight(TInt aPercentage);
	IMPORT_C virtual void SetTitle(const TDesC& aTitle);
	IMPORT_C virtual void ClearScreen();
	IMPORT_C virtual void ClearToEndOfLine();
	IMPORT_C virtual TSize ScreenSize() const;
	IMPORT_C virtual TKeyCode KeyCode() const;
	IMPORT_C virtual TUint KeyModifiers() const;
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
protected:
	IMPORT_C CVtcConsoleBase();
	IMPORT_C virtual void ConstructL(const TDesC& aTitle);
	IMPORT_C void ConstructL(const TDesC& aTitle, TBool aConsoleSupportsSizeDetect); // Note this is NOT virtual, only used for baseclasses to call up to from their overload of ConstructL(const TDesC&) if they explicitly don't support console size detect
	IMPORT_C static TInt ReadKeywordValuePair(TLex& aLex, TPtrC& aKeyword, TPtrC& aValue);

private:
	void DetectScreenSizeL(TSize& aSize);
	TInt DoDetectScreenSize(TSize& aSize);
	TInt ReadCursorPos(TPoint& aResult);
private:
	TKeyPress iKeyPress;
	CVtConsoleOutputController* iOutputController;
	CVtConsoleInputController* iInputController;
	LtkUtils::CIniFile* iIniFile;
	};


#endif // __VT100CONS_BASE_H__
