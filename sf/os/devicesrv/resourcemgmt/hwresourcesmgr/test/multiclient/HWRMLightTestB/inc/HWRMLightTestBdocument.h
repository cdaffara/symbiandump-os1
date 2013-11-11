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



#ifndef __HWRMLightTestB_DOCUMENT_H__
#define __HWRMLightTestB_DOCUMENT_H__


#include <eikdoc.h>

class CHWRMLightTestBAppUi;
class CEikApplication;


/** 
  @class CHWRMLightTestBDocument
  
  An instance of class CHWRMLightTestBDocument is the Document part of the AVKON
  application framework for the HWRMLightTestB example application
  * * Nokia Core OS *
  */
class CHWRMLightTestBDocument : public CEikDocument
    {
public:

/**
  NewL
  
  Construct a CHWRMLightTestBDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp The application creating this document
  @return A pointer to the created instance of CHWRMLightTestBDocument
  */
    static CHWRMLightTestBDocument* NewL(CEikApplication& aApp);

/**
  NewLC
  
  Construct a CHWRMLightTestBDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp The application creating this document
  @return A pointer to the created instance of CHWRMLightTestBDocument
  */
    static CHWRMLightTestBDocument* NewLC(CEikApplication& aApp);

/**
  ~CHWRMLightTestBDocument
  
  Destroy the object
  */
    ~CHWRMLightTestBDocument();

public: // from CAknDocument
/**
  CreateAppUiL 
  
  Create a CHWRMLightTestBAppUi object and return a pointer to it
  @return A pointer to the created instance of the AppUi created
  */
    CEikAppUi* CreateAppUiL();

private:

/**
  ConstructL
  
  Perform the second phase construction of a CHWRMLightTestBDocument object
  */
    void ConstructL();

/**
  CHWRMLightTestBDocument
  
  Perform the first phase of two phase construction 
  @param aApp The application creating this document
  */
    CHWRMLightTestBDocument(CEikApplication& aApp);

    };


#endif // __HWRMLightTestB_DOCUMENT_H__
