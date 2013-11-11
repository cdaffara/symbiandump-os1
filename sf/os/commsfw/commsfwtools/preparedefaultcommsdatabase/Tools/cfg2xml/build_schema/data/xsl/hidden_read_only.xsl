<?xml version="1.0" encoding="UTF-8"?>

<!-- 
    Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
    All rights reserved.
    This component and the accompanying materials are made available
    under the terms of "Eclipse Public License v1.0"
    which accompanies this distribution, and is available
    at the URL "http://www.eclipse.org/legal/epl-v10.html".

    Initial Contributors:
    Nokia Corporation - initial contribution.
    
    Description:
        Stylesheet to remove Hidden and ReadOnly fields from CommDb tables.
-->
 
<xsl:stylesheet 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
        xmlns:xsd="http://www.w3.org/2001/XMLSchema">
	
	<xsl:output encoding="UTF-8"/>
	<!-- Process the entire document -->
	<xsl:template match="@*|node()|/">
  		<xsl:copy>
    		<xsl:apply-templates select="@*|node()"/>
  		</xsl:copy>
	</xsl:template> 
	
	<xsl:template match="xsd:element[@name='ReadOnly']"/>
 	<xsl:template match="xsd:element[@name='Hidden']"/>
       
    
	
</xsl:stylesheet>

