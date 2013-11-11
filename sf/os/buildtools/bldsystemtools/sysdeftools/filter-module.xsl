<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common"  exclude-result-prefixes="exslt">
<!--Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of "Eclipse Public License v1.0"
	which accompanies this distribution, and is available
	at the URL "http://www.eclipse.org/legal/epl-v10.html".

	Initial Contributors:
	Nokia Corporation - initial contribution.
	Contributors:
	Description:
	XSLT module which contains the logic to process the filter attribute in the system definition
-->
	<xsl:output method="xml" indent="yes"/>


<!-- filters can only return hide or show -->

<!-- filter-only =  item's @filter can only have items from the list
	anything with no filters passes
-->
<xsl:template match="filter-only" mode="filter"><xsl:param name="item"/>
	<xsl:if test="$item[@filter]">
		<xsl:variable name="this" select="."/>
		<xsl:variable name="my-filters">
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/parent::component/@filter"/>
			</xsl:call-template>
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/@filter"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="match">
			<xsl:for-each select="exslt:node-set($my-filters)/*">
				<xsl:if test="not($this/*[name()=name(current())  and @v=current()/@v])">x</xsl:if>
			</xsl:for-each>
		</xsl:variable>
		<xsl:if test="$match!=''">hide</xsl:if>
	</xsl:if>
</xsl:template>



<!-- filter-has =  item's @filter must have all filters in the list. ie it can have any other filters, but these must all be present	
-->
<xsl:template match="filter-has" mode="filter"><xsl:param name="item"/>
	<xsl:if test="$item[(self::component or self::unit)  and not(unit/@filter or self::unit[not(../unit[@filter])])]">
		<xsl:variable name="my-filters">
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/parent::component/@filter"/>
			</xsl:call-template>
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/@filter"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="match">
			<xsl:for-each select="*">
				<!-- 	if(f in this) {return true}  else if(!f in this) {return false} else {return !(f is positive)} -->
				<xsl:choose>
					<xsl:when test="exslt:node-set($my-filters)/*[name()=name(current()) and @v=current()/@v]"/> <!-- filter in item -->
					<xsl:when test="exslt:node-set($my-filters)/*[name()!=name(current()) and @v=current()/@v]">x</xsl:when> <!-- !filter in item -->
					<xsl:when test="self::filter">x</xsl:when> <!-- !(filter is positive) -->
				</xsl:choose>
			</xsl:for-each>
		</xsl:variable>
		<xsl:if test="$match!=''">hide</xsl:if>
	</xsl:if>
</xsl:template>

<xsl:template match="filter-with" mode="filter"><xsl:param name="item"/>
	<xsl:if test="$item[(self::component or self::unit)  and not(unit/@filter or self::unit[not(../unit[@filter])])]">
		<xsl:variable name="my-filters">
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/parent::component/@filter"/>
			</xsl:call-template>
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$item/@filter"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="match">
			<xsl:for-each select="*">
				<xsl:if test="exslt:node-set($my-filters)/*[name()!=name(current()) and @v=current()/@v]">x</xsl:if> <!-- !filter in item -->
			</xsl:for-each>
		</xsl:variable>
		<xsl:if test="$match!=''">hide</xsl:if>
	</xsl:if>
</xsl:template>



<xsl:template name="filter-item"> <xsl:param name="f"/>
	<!-- create an element for a given filter. If the filter's empty make nothing -->
	<xsl:choose>
		<xsl:when test="$f=''"/>
		<xsl:when test="starts-with($f,'!')">
				<not v="{substring($f,2)}"/>
		</xsl:when>
		<xsl:otherwise>
				<filter v="{$f}"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="filter-list"><xsl:param name="f"/>
	<!-- turn a filter list into a set of elements (<filter> or <not>) with the attribute "v" containing the "absolute value" of the filter
		filter="A,B,!C" becomes  <filter v="A"/><filter v="B"/><not v="C"/> 
	  -->
	<xsl:choose>
		<xsl:when test="contains($f,',')">
			<xsl:call-template name="filter-item"><xsl:with-param name="f" select="normalize-space(substring-before($f,','))"/></xsl:call-template>
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="substring-after($f,',')"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="filter-item"><xsl:with-param name="f" select="normalize-space($f)"/></xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet> 