// glinfo.cpp
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

#include <fshell/ioutils.h>
#include <egl/egl.h>
#include <vg/openvg.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdGlInfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdGlInfo();
private:
	CCmdGlInfo();

#ifdef FSHELL_EGL_SUPPORT
	void PrintElgInfoL();
	void PrintElgQueryString(EGLDisplay aDisplay, EGLint aName, const TDesC8& aSymbol);
#endif // FSHELL_EGL_SUPPORT

#ifdef FSHELL_OPENVG_SUPPORT
	void PrintOpenVgInfoL();
	void PrintOpenVgString(VGStringID aName, const TDesC8& aSymbol);
#endif // FSHELL_OPENVG_SUPPORT

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum
		{
		ELibEgl,
		ELibOpenVg
		} iLibrary;
	};


CCommandBase* CCmdGlInfo::NewLC()
	{
	CCmdGlInfo* self = new(ELeave) CCmdGlInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGlInfo::~CCmdGlInfo()
	{
	}

CCmdGlInfo::CCmdGlInfo()
	{
	}

#ifdef FSHELL_EGL_SUPPORT

void CCmdGlInfo::PrintElgInfoL()
	{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	EGLint major;
	EGLint minor;
	if (!eglInitialize(dpy, &major, &minor))
		{
		LeaveIfErr(KErrGeneral, _L("Couldn't initialize EGL display"));
		}
	PrintElgQueryString(dpy, EGL_CLIENT_APIS, _L8("EGL_CLIENT_APIS"));
	PrintElgQueryString(dpy, EGL_EXTENSIONS, _L8("EGL_EXTENSIONS"));
	PrintElgQueryString(dpy, EGL_VENDOR, _L8("EGL_VENDOR"));
	PrintElgQueryString(dpy, EGL_VERSION, _L8("EGL_VERSION"));
	if (!eglTerminate(dpy))
		{
		LeaveIfErr(KErrGeneral, _L("Couldn't terminate EGL display"));
		}
	}

void CCmdGlInfo::PrintElgQueryString(EGLDisplay aDisplay, EGLint aName, const TDesC8& aSymbol)
	{
	const char* string = eglQueryString(aDisplay, aName);
	if (string == NULL)
		{
		string = "Unknown";
		}
	Printf(_L8("%S: %s\r\n"), &aSymbol, string);
	}

#endif // FSHELL_EGL_SUPPORT

#ifdef FSHELL_OPENVG_SUPPORT

void CCmdGlInfo::PrintOpenVgInfoL()
	{
	PrintOpenVgString(VG_VENDOR, _L8("VG_VENDOR"));
	PrintOpenVgString(VG_RENDERER, _L8("VG_RENDERER"));
	PrintOpenVgString(VG_EXTENSIONS, _L8("VG_EXTENSIONS"));
	PrintOpenVgString(VG_VERSION, _L8("VG_VERSION"));
	}

void CCmdGlInfo::PrintOpenVgString(VGStringID aName, const TDesC8& aSymbol)
	{
	const VGubyte* string = vgGetString(aName);
	if (string == NULL)
		{
		string = (const VGubyte*)"Unknown";
		}
	Printf(_L8("%S: %s\r\n"), &aSymbol, string);
	}

#endif // FSHELL_OPENVG_SUPPORT

const TDesC& CCmdGlInfo::Name() const
	{
	_LIT(KName, "glinfo");	
	return KName;
	}

void CCmdGlInfo::DoRunL()
	{
	switch (iLibrary)
		{
#ifdef FSHELL_EGL_SUPPORT
		case ELibEgl:
			{
			PrintElgInfoL();
			break;
			}
#endif // FSHELL_EGL_SUPPORT
#ifdef FSHELL_OPENVG_SUPPORT
		case ELibOpenVg:
			{
			PrintOpenVgInfoL();
			break;
			}
#endif // FSHELL_OPENVG_SUPPORT
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

void CCmdGlInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgLibrary, "library");
	aArguments.AppendEnumL((TInt&)iLibrary, KArgLibrary);
	}

void CCmdGlInfo::OptionsL(RCommandOptionList&)
	{
	}


EXE_BOILER_PLATE(CCmdGlInfo)

