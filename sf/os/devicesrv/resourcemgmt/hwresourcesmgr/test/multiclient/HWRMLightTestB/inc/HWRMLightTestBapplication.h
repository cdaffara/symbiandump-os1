/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




#ifndef __HWRMLightTestB_APPLICATION_H__
#define __HWRMLightTestB_APPLICATION_H__

#include <eikapp.h>



/*! 
  @class CHWRMLightTestBApplication
  
  @discussion An instance of CHWRMLightTestBApplication is the application part of the AVKON
  application framework for the HWRMLightTestB example application
  * * Nokia Core OS *
  */
class CHWRMLightTestBApplication : public CEikApplication
    {
public:  // from CAknApplication

/*AppDllUid
  
  discussion Return the application DLL UID value
  @return the UID of this Application/Dll
  */
    TUid AppDllUid() const;

		

protected: // from CAknApplication
/* 
  CreateDocumentL
  
  Create a CApaDocument object and return a pointer to it
  @return a pointer to the created document
  */
    CApaDocument* CreateDocumentL();

private:



    };


#endif // __HWRMLightTestB_APPLICATION_H__
