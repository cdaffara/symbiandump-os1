/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CustomInterface Utility
*
*/

#ifndef __CUSTOMINTERFACEUTILITY_H
#define __CUSTOMINTERFACEUTILITY_H

#include <e32base.h>
#include <MCustomInterface.h>
//#include "CustomCommandTypes.h"
#include <CustomCommandUtility.h>
class CProxyCustomInterfaceUtility;
//class CCustomInterfaceProxyBuilder;
class CMdaAudioConvertUtility;
class CMdaAudioPlayerUtility;
class CMdaAudioRecorderUtility;
class MCustomCommand;
class CMMFDevSound;
class CMidiClientUtility;
class CDrmPlayerUtility;
class CVideoPlayerUtility;
class CVideoRecorderUtility;

class CCustomInterfaceUtility : public    CBase,
                                public    MCustomInterface
    {
public:
    IMPORT_C static CCustomInterfaceUtility* NewL(CMdaAudioConvertUtility&  aUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(CMdaAudioPlayerUtility&   aUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(CMdaAudioRecorderUtility& aUtility,
                                                  TBool       aRecordStream);
    IMPORT_C static CCustomInterfaceUtility* NewL(CCustomCommandUtility* aCustomCommandUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(CMMFDevSound& aDevSound);
    IMPORT_C static CCustomInterfaceUtility* NewL(CMidiClientUtility& aUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(MCustomCommand& aUtility);
    IMPORT_C virtual ~CCustomInterfaceUtility();

    /**
    * Factory function for creating CCustomInterfaceUtility object.
    * @since 3.0
    * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
    * @return pointer to CCustomInterfaceUtility object
    */
    IMPORT_C static CCustomInterfaceUtility* NewL(CDrmPlayerUtility& aUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(CVideoPlayerUtility&   aUtility);
    IMPORT_C static CCustomInterfaceUtility* NewL(CVideoRecorderUtility&   aUtility);

    virtual TAny* CustomInterface(TUid aInterfaceId);

    virtual void RemoveCustomInterface(TMMFMessageDestinationPckg aMessageHandler);
protected:
    IMPORT_C CCustomInterfaceUtility();
    virtual void ConstructL(CCustomCommandUtility* aCustomCommandUtility);
    virtual void ConstructL(CMMFDevSound& aDevSound);

private:

    CProxyCustomInterfaceUtility* iProxyFactory;


private:
    CMMFDevSound*       iDevSound;
    };

#endif
