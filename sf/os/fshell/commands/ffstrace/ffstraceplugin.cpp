// ffstraceplugin.cpp
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

#include <fshell/extrabtrace.h>
#include <f32plugin.h>
#include <f32pluginutils.h>
#include <fshell/ltkutils.h>

//#define LOG_FUNC() RDebug::Printf(__PRETTY_FUNCTION__)
//#define LOG(args...) RDebug::Printf(args)
//#define LOGW(args...) RDebug::Print(args)
#define LOG_FUNC()
#define LOG(args...)
#define LOGW(args...)


NONSHARABLE_CLASS(CFfsTracerPluginFactory) : public CFsPluginFactory
	{
public:
	static CFfsTracerPluginFactory* NewL();

	// from CFsPluginFactory
	virtual TInt Install();
	virtual CFsPlugin* NewPluginL();
	virtual TInt UniquePosition();

private:
	CFfsTracerPluginFactory();
	};

NONSHARABLE_CLASS(CFfsTracerFsPlugin) : public CFsPlugin
	{
public:
	static CFfsTracerFsPlugin* NewL();
	virtual ~CFfsTracerFsPlugin();
	
	// from CFsPlugin
	virtual void InitialiseL();
	TInt SessionDisconnect(CSessionFs* aSession);
	virtual TInt DoRequestL(TFsPluginRequest& aRequest);
	virtual CFsPluginConn* NewPluginConnL();
private:
	CFfsTracerFsPlugin();
	};
	
class CFfsTracerFsPluginConn : public CFsPluginConn
	{
public:
	CFfsTracerFsPluginConn();

	// from CFsPluginConn
	virtual TInt DoControl(CFsPluginConnRequest& aRequest);
	virtual void DoRequest(CFsPluginConnRequest& aRequest);
	virtual void DoCancel(TInt aReqMask);
	};

extern "C" EXPORT_C CFsPluginFactory* CreateFileSystem()
	{
	return CFfsTracerPluginFactory::NewL();
	}

CFfsTracerPluginFactory* CFfsTracerPluginFactory::NewL()
	{
	return(new(ELeave) CFfsTracerPluginFactory());
	}

CFfsTracerPluginFactory::CFfsTracerPluginFactory()
	{
	}

_LIT(KFfsTracerPluginName, "FfsTracePlugin");

TInt CFfsTracerPluginFactory::Install()
	{
	LOG_FUNC();
	iSupportedDrives = 0x7FFFFFF; // KPluginSupportAllDrives | KPluginVersionTwo. Not specified symbolically to be compatible with fileservers that didn't support v2 plugins
	
	TInt err;
	err = SetName(&KFfsTracerPluginName);
	LOG(" returned %d", err);
	return err;
	}

CFsPlugin* CFfsTracerPluginFactory::NewPluginL()
	{
	return CFfsTracerFsPlugin::NewL();
	}
	
TInt CFfsTracerPluginFactory::UniquePosition()
	{
	return 1; // Isn't used, apparently
	}

//

CFfsTracerFsPluginConn::CFfsTracerFsPluginConn()
	{
	}

TInt CFfsTracerFsPluginConn::DoControl(CFsPluginConnRequest& /*aRequest*/)
	{
	return KErrNone;
	}

void CFfsTracerFsPluginConn::DoRequest(CFsPluginConnRequest& /*aRequest*/)
	{
	}

void CFfsTracerFsPluginConn::DoCancel(TInt /*aReqMask*/)
	{
	}

//

CFfsTracerFsPlugin* CFfsTracerFsPlugin::NewL()
	{
	return new(ELeave) CFfsTracerFsPlugin();
	}
	
CFfsTracerFsPlugin::CFfsTracerFsPlugin()
	{
	LOG_FUNC();
	}

static const TFsMessage KMessages[] =
	{
	EFsDelete,
	EFsRename,
	EFsReplace,
	EFsEntry,
	EFsFileSubClose,
	EFsFileOpen,
	EFsFileCreate,
	EFsFileReplace,
	EFsFileTemp,
	EFsFileRename,
	};
static const TInt KMessageCount = sizeof(KMessages) / sizeof(TFsMessage);

CFfsTracerFsPlugin::~CFfsTracerFsPlugin()
	{
	LOG_FUNC();
	for (TInt i = 0; i < KMessageCount; i++)
		{
		UnregisterIntercept(KMessages[i], CFsPlugin::EPrePostIntercept);
		}
	}

void CFfsTracerFsPlugin::InitialiseL()
	{
	// intercept all calls at start
	for (TInt i = 0; i < KMessageCount; i++)
		{
		RegisterIntercept(KMessages[i], CFsPlugin::EPrePostIntercept);
		}
	}

TInt CFfsTracerFsPlugin::SessionDisconnect(CSessionFs* /*aSession*/)
	{
	// CSessionFs is a private class! It's in the main file server and 
	// has no exported methods!  What's the use of passing it here?
	return KErrNone;
	}

CFsPluginConn* CFfsTracerFsPlugin::NewPluginConnL()
	{
	LOG_FUNC();
	return new(ELeave) CFfsTracerFsPluginConn();
	}

TInt CFfsTracerFsPlugin::DoRequestL(TFsPluginRequest& aRequest)
	{
	// This is where it all happens
	
	TInt subcat = 0;
	TInt fn = aRequest.Function();
	// Map from FS opcodes to the compacted btrace ones
	switch (fn)
		{
		case EFsDelete: subcat = BTrace::EFfsDelete; break;
		case EFsRename: subcat = BTrace::EFfsRename; break;
		case EFsReplace: subcat = BTrace::EFfsReplace; break;
		case EFsEntry: subcat = BTrace::EFfsEntry; break;
		case EFsFileSubClose: subcat = BTrace::EFfsFileSubClose; break;
		case EFsFileOpen: subcat = BTrace::EFfsFileOpen; break;
		case EFsFileCreate: subcat = BTrace::EFfsFileCreate; break;
		case EFsFileReplace: subcat = BTrace::EFfsFileReplace; break;
		case EFsFileTemp: subcat = BTrace::EFfsFileTemp; break;
		case EFsFileRename: subcat = BTrace::EFfsFileRename; break;
		default:
			// An operation we're not interested in
			return KErrNone;
		}
	if (aRequest.IsPostOperation()) subcat |= BTrace::EFfsPost;

	TBuf<513> name; // Twice as big as a max filename because for renames we need it to fit two TFileNames
	GetName(&aRequest, name);

	if (fn == EFsRename || fn == EFsFileRename)
		{
		name.Append(TChar(0));
		TPtr newName((TUint16*)(name.Ptr() + name.Length()), 0, name.MaxLength() - name.Length());
		GetNewName(&aRequest, newName);
		LOGW(_L("new name is %S"), &newName);
		name.SetLength(name.Length() + newName.Length());
		}

	// Only try to use request.Message() if it's a valid handle
	TUint threadId = 0xFFFFFFFFu;
	if(aRequest.Message().Handle() != KNullHandle)
		{
		RThread clientThread;
		aRequest.Message().Client(clientThread, EOwnerThread);
		threadId = clientThread.Id();
		clientThread.Close();
		}

	BTraceBig(BTrace::EFfsTrace, subcat, threadId, name.Ptr(), name.Size());
	
	return KErrNone;
	}
