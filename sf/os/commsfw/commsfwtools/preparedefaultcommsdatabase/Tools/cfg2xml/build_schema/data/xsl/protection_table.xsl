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
	Stylesheet to remove reference to AccessTypeTable and its elements from CommDB XML schema
-->

<xsl:stylesheet 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
        xmlns:xsd="http://www.w3.org/2001/XMLSchema"
        xmlns:CommDB="SymbianOS.Generic.Comms-Infras.CommDB">
        
    <xsl:output encoding="UTF-8"/>     	
	
	<!-- Process the entire document -->
	<xsl:template match="@*|node()|/">
  		<xsl:copy>
    		<xsl:apply-templates select="@*|node()"/>
  		</xsl:copy>
	</xsl:template> 
	
	<!-- CommDb.xsd -->
	<xsl:template match="xsd:include[@schemaLocation='AccessTypeTableTable.xsd']"/>
 	<xsl:template match="xsd:element[@ref='AccessTypeTableTable']"/>
 	
 	<!-- CommTypes.xsd MUST REMOVE COMMENTS TOO -->
 	<xsl:template match="xsd:simpleType[@name='ProtectableTableType']"/>
 	<xsl:template match="xsd:simpleType[@name='TableAccessType']"/>
 
 	<!-- Contents.xml --> 		
	<xsl:template match="CommDB:table[@file='AccessTypeTableTable.xsd']"/>

 	
 
 </xsl:stylesheet>
	