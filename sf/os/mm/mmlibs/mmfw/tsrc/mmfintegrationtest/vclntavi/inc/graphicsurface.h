// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference sample client-side "provider" interface CWsGraphic plugin using GCE Surfaces
// Copied from common/generic/graphics/wserv/nga/samplegraphicsurfacedrawer
// if the CRP tests suddenly starts to break or fail to build it might be due to this file has changed in CBR
// 
//

#ifndef __GRAPHICSURFACE_H__
#define __GRAPHICSURFACE_H__

#include <w32std.h>
#include <w32stdgraphic.h>
#include <graphics/surface.h>
#include <graphics/surfaceconfiguration.h>

enum TDrawWsGraphicArgumentFlags
	{
	EDrawWsAllScreens							=0x00000001,
	EDrawWsTSurfaceIdIgnored 					=0x00000002,	//<Indicates that the client should NOT supply
	EDrawWsNotifyProviderTSurfaceIdChanged		=0x00000100,
	EDrawWsNotifyProviderConfigChanged			=0x00000200,
	EDrawWsNotifyProviderPlayControlsChanged	=0x00000400,
	EDrawWsNotifyProviderSurfaceUnreferenced	=0x00000800,
	EDrawWsNotifyProviderConfigRereferenced		=0x00001000,	//<Triggers a one-shot config notification when drawn after an unreference
	EDrawWsNotifyProviderConfigNextReref		=0x00002000,	//<Automatic one-shot set after unreferenced if 
	EDrawWsNotifyError							=0x00010000,
	EDrawWsNotifyProviderAll					=0x000FFF00,
	EDrawWsUpdateTSurfaceId						=0x00100000,
	EDrawWsUpdateConfiguration					=0x00200000,
	EDrawWsUpdatePlayerControls					=0x00400000,
	EDrawWsUpdateAll							=0x0FF00000,
	};

//KUidGraphicMsgSurface=  0x10285C57 	  To    0x10285C5B
class TWsSurfaceConfiguration: public TWsGraphicMsgFixedBase
/** data attached to a CWindowGc::DrawWsGraphic to allow the artwork to understand surface configuration commands.
Also use to notify owner client of config change.

@publishedAll
@released
*/	{
public:
	enum
		{
		ETypeId = 0x10285C57
		};
		TWsSurfaceConfiguration()
		:	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this))
		{	}
		TWsSurfaceConfiguration(const TSurfaceConfiguration& aConfig)
		:	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this)),
			iConfig(aConfig)
			
		{	}
	TSurfaceConfiguration iConfig;
	};

class TWsDebugResultCode: public TWsGraphicMsgFixedBase
/** Data attached to a message to the artwork provider notifying it of a command result code
This code is always followed by the command that caused it (TSurfaceConfig)
*/
	{
public:
	enum
		{
		ETypeId = 0x10285C5A
		};
	TWsDebugResultCode():	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this))
		{	}
	TInt iResult;
	TInt iExtra;
	TInt iLineNum;
	};


class TWsSurfaceUnreferenced: public TWsGraphicMsgFixedBase
/** data attached to a message to the artwork provider to allow it to react if a surface is no longer drawn
play-stop-pause commands.

@publishedAll
@released
*/	{
public:
	enum
		{
		ETypeId = 0x10285C58
		};
	TWsSurfaceUnreferenced():	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this))
		{	}
	TSurfaceId iId;
	};
class CWsGraphicDrawerSurface;
class TWsDebugResultCode;
class CWsGraphicSurface: public CWsGraphic
/** Client representation of a window-server-side CFbsBitmap owned by this Client

@publishedAll
@released
*/
	{
	friend class CWsGraphicDrawerSurface;
public:
	IMPORT_C static CWsGraphicSurface* NewL(const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C static CWsGraphicSurface* NewL(TUid aUid,const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C static CWsGraphicSurface* NewL(const TWsGraphicId& aReplace,const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C  void ConstructL(const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C  void ConstructL(TUid aUid,const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C  void ConstructL(const TWsGraphicId& aReplace,const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C ~CWsGraphicSurface();
protected: 
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
	IMPORT_C TInt SendMessage(const TSurfaceConfiguration&);
	IMPORT_C TInt SendMessage(const TWsGraphicAnimation&);

	virtual void HandleMessage(const TSurfaceConfiguration&)		{}
	virtual void HandleMessage(const TWsGraphicAnimation&)			{}
	virtual void HandleMessage(const TWsSurfaceUnreferenced&)		{}
	virtual void HandleMessage(const TWsDebugResultCode&)		{}
	virtual void HandleOtherMessage(const TWsGraphicMsgFixedBase&)	{}
	IMPORT_C virtual TInt CWsGraphicSurface_Reserved1();
	IMPORT_C virtual TInt CWsGraphicSurface_Reserved2();
	IMPORT_C virtual TInt CWsGraphicSurface_Reserved3();
protected:
	class TConstructParams;
	IMPORT_C TConstructParams* MakeParamsLC(const TSurfaceConfiguration& aSurface, TUint aDrawWsGraphicArgumentFlags, const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C CWsGraphicSurface();		//Derivers should implement NewL to call ConstructL
	IMPORT_C void RegisterScreens(const TSurfaceConfiguration& aSurface,const TArray<TUint>*  aScreensMap=NULL);
	IMPORT_C void UnRegisterScreens();
private:
	void HandleMessage(const TDesC8& aData);
	void OnReplace();

private: //data
	TSurfaceId		iRegisteredId;
	RArray<TUint>	iRegisteredScreens;
	};

/** Used as the parameter payload to construct the drawer
*/
class CWsGraphicSurface::TConstructParams
	{
public:
	TInt Size()
		{	return iParamFlags.Size()+iConfig.Size()+2*sizeof(TWsGraphicMsgFixedBase);	}
	void* operator new(TUint size,TInt aNumExtras)
		{
		return ::operator new(size+((aNumExtras>=0)?aNumExtras*sizeof(TInt):0));
		}
	void operator delete(void* aObj,TInt /*aNumExtras*/)
		{
		::operator delete(aObj);
		}
	void operator delete(void* aObj)
		{
		::operator delete(aObj);
		}
	TWsSurfaceConfiguration iConfig;
	class TParamFlags:public TWsGraphicMsgFixedBase
		{
		public:
		enum
			{
			ETypeId = 0x10285C59
			};

		TParamFlags(): TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this))
			{}
		TParamFlags(TInt aNumExtras): TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this)+aNumExtras*sizeof(TInt))
			{}
		TUint iDrawWsGraphicArgumentFlags;
		TUint iScreensMap[1];	//THIS MUST BE THE LAST FIELD!!!
		} iParamFlags;
	TConstructParams()	
	{	}
	TConstructParams(const TSurfaceConfiguration& aConfig,TInt aNumExtras=0)
	:	iConfig(aConfig),iParamFlags(aNumExtras)
	{	}
	TCleanupItem	CleanupItem()	{ return TCleanupItem(Cleanup,this);	}
	static void Cleanup(void* p)	{	delete(TConstructParams*)p;	}	
	};

#endif //__GRAPHICSURFACE_H__

