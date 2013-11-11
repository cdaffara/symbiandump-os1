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
	XSLT module for merging only two sysdef files according to the 3.0.0 rules. Old syntax not supported and must be converetd before calling.
-->
	
<xsl:variable name="defaultnamespace">http://www.symbian.org/system-definition</xsl:variable>

<xsl:template match="/SystemDefinition[starts-with(@schema,'2.') or starts-with(@schema,'1.')]" priority="2" mode="merge-models">
	<xsl:message terminate="yes">ERROR: Syntax <xsl:value-of select="@schema"/> not supported</xsl:message>
</xsl:template>
<!--<xsl:template match="/SystemDefinition[not(systemModel)]" priority="2" mode="merge-models">
	<xsl:message terminate="yes">ERROR: Can only merge stand-alone system models</xsl:message>
</xsl:template>-->

<!-- stuff for dealing with namespaces -->


<xsl:template match="node()|@*" mode="translate-namespaces"><xsl:copy-of select="."/></xsl:template>
<!-- don't translate meta or unit tags, just copy verbatim -->
<xsl:template match="meta|unit" mode="translate-namespaces" priority="2">
<xsl:element name="{name()}">
<xsl:copy-of select="@*|*|comment()"/>
</xsl:element>
</xsl:template>

<xsl:template match="*" mode="translate-namespaces"><xsl:param name="nsdoc"/>
<xsl:element name="{name()}">
<xsl:apply-templates select="@*|node()" mode="translate-namespaces">
	<xsl:with-param name="nsdoc" select="$nsdoc"/>
</xsl:apply-templates>
</xsl:element>
</xsl:template>


<xsl:template match="@id|@before|@replace" mode="translate-namespaces"><xsl:param name="nsdoc"/>
	<xsl:attribute name="{name()}">
		<xsl:variable name="id">
			<xsl:choose>
				<xsl:when test="contains(.,':')">
					<xsl:value-of select="substring-after(.,':')"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="."/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="ns">
			<xsl:choose>
				<xsl:when test="contains(.,':')">
					<xsl:value-of select="ancestor-or-self::*/namespace::*[name()=substring-before(current()/.,':')]"/>
				</xsl:when>
				<xsl:when test="ancestor::SystemDefinition/@id-namespace">
					<xsl:value-of select="ancestor::SystemDefinition/@id-namespace"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$defaultnamespace"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="not($nsdoc/@id-namespace) and $defaultnamespace=$ns">
				<xsl:value-of select="$id"/>
			</xsl:when>
			<xsl:when test="$nsdoc/@id-namespace=$ns">
				<xsl:value-of select="$id"/>
			</xsl:when>
			<xsl:when test="$nsdoc/namespace::*[.=$ns]">
				<xsl:value-of select="concat(name($nsdoc/namespace::*[.=$ns]),':',$id)"/>
			</xsl:when>
			<xsl:when test="ancestor::SystemDefinition/@id-namespace=$ns">
				<xsl:variable name="myns">
					<xsl:apply-templates mode="ns-prefix" select="$nsdoc">
						<xsl:with-param name="ns" select="$ns"/>
					</xsl:apply-templates>
				</xsl:variable>			
				<xsl:value-of select="concat($myns,':',$id)"/>
			</xsl:when>
			<xsl:otherwise> <!-- some namespace that needed to be defined --> 
			<xsl:message>Warning: need definition for namespace "<xsl:value-of select="$ns"/>" for <xsl:value-of select="$id"/></xsl:message>
				<xsl:value-of select="."/>					
			</xsl:otherwise>
		</xsl:choose>		
	</xsl:attribute>
</xsl:template>

<xsl:template match="SystemDefinition" mode="ns-prefix">
	<xsl:param name="ns"/> <!-- the namespace URI -->
	<xsl:param name="pre"/> <!-- the preferred prefix to use if possbile -->
	<xsl:param name="dontuse"/> <!-- space prefixed, separated and terminated list of namespace prefixes to not use -->
	<xsl:param name="chars">ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz</xsl:param> <!-- single letter namespace prefixes to try -->
	<xsl:variable name="name" select="substring(substring-after($ns,'http://www.'),1,1)"/>
	<xsl:choose>
		<xsl:when test="$pre!='' and $pre!='id-namespace' and not(//namespace::*[name()=$pre]) and not(contains($dontuse,concat(' ',$pre,' ')))">
			<xsl:value-of select="$pre"/>
		</xsl:when>
		<xsl:when test="$ns='' and $chars=''">
			<xsl:message terminate="yes">ERROR: Cannot create namespace prefix for downstream default namespace in <xsl:value-of select="*/@id"/></xsl:message>
		</xsl:when>
		<xsl:when test="$name!='' and not(contains($dontuse,concat(' ',$name,' ')))"><xsl:value-of select="$name"/></xsl:when>
		<xsl:when test="namespace::*[name()=substring($chars,1,1)] or contains($dontuse,concat(' ',substring($chars,1,1),' '))">
			<xsl:apply-templates mode="ns-prefix">
				<xsl:with-param name="chars" select="substring($chars,2)"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="substring($chars,1,1)"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<!--  need to make sure this handles <meta> correctly -->

<xsl:template match="SystemDefinition" mode="merge-models">
	<xsl:param name="other"/>	<!-- the downstream SystemDefinition this is merged with -->
	<xsl:param name="up" select="systemModel"/>	<!-- the element containing the origin @name used for any component from "this" model. -->
	<xsl:param name="down" select="$other/systemModel"/> <!-- the element containing origin @name used for any component from $other model. -->
	
	<!-- do some testing -->
 	<xsl:if test="$other[starts-with(@schema,'2.') or starts-with(@schema,'1.')]">
		<xsl:message terminate="yes">ERROR: Syntax <xsl:value-of select="$other/@schema"/> not supported</xsl:message>
	</xsl:if>
	<xsl:if test="name(*) != name($other/*)">
		<xsl:message terminate="yes">ERROR: Can only merge system models of the same rank</xsl:message>
	</xsl:if>
	 
	<xsl:copy>
		<xsl:attribute name="schema">
			<xsl:call-template name="compare-versions">
				<xsl:with-param name="v1" select="@schema"/>
				<xsl:with-param name="v2" select="$other/@schema"/>
			</xsl:call-template>
		</xsl:attribute>
		<xsl:copy-of  select="@*[name()!='schema']"/> <!--  use attributes from origin model -->
		<xsl:variable name="namespaces">
			<xsl:copy> <!-- needs <copy> so the processor doesn't lose the namespaces -->
				<!--copy namespaces as needed -->
				
				<xsl:copy-of select="namespace::*[name()!='xml']"/> <!-- all upstream namespaces -->

				<xsl:variable name="cur" select="."/>
				<xsl:for-each select="$other/namespace::*"> <!-- all namespaces in downstream not already in upstream -->
					<xsl:if test="not((. = $cur/@id-namespace) or (not($cur/@id-namespace) and .= $defaultnamespace) or  $cur/namespace::*[.=current()])">
							<!-- namespace in downstream not in upstream doc -->
							<xsl:variable name="newprefix">
								 <!-- test to see if the ns prefix already exists -->
								<xsl:apply-templates select="$cur" mode="ns-prefix">
									<xsl:with-param name="ns" select="."/>
									<xsl:with-param name="pre" select="name()"/>
								</xsl:apply-templates>
							</xsl:variable>
							<xsl:copy/>
					</xsl:if>   
				</xsl:for-each>
				
					<xsl:if test="not(($other/@id-namespace = @id-namespace) or (not($other/@id-namespace) and not(@id-namespace)) or (not(@id-namespace) and $other/@id-namespace = $defaultnamespace) or namespace::*[.=$other/@id-namespace])">  
						<!-- default namespace in downstream not in upstream doc -->
						<!-- need to make created ns a bit more intelligent -->
						<xsl:attribute name="bar" namespace="{$other/@id-namespace}">
							<xsl:value-of select="$other/@id-namespace"/>
						</xsl:attribute>
				</xsl:if>
			</xsl:copy>
		</xsl:variable>

		
		<!-- copy the namespaces to currently open element (the root one) -->
		<xsl:copy-of select="namespace::*"/>
		<xsl:for-each select="$other/namespace::*[.!=current()/namespace::*]"><xsl:copy/></xsl:for-each>
		<xsl:for-each select="exslt:node-set($namespaces)/*/namespace::*"><xsl:copy/></xsl:for-each>
	<!-- translate all IDs in downstream doc to use namespaces from upstream doc  
		This is so much easier than having to propigate this info around while creating the doc-->
	<xsl:variable name="otherdoc">
		<xsl:apply-templates mode="translate-namespaces" select="$other">
			<xsl:with-param name="nsdoc" select="exslt:node-set($namespaces)/* | ."/>
		</xsl:apply-templates>
	</xsl:variable>
		<xsl:apply-templates mode="merge-models">
			<xsl:with-param name="other" select="exslt:node-set($otherdoc)/*"/>
			<xsl:with-param name="up" select="$up"/>
			<xsl:with-param name="down" select="$down"/>
			<xsl:with-param name="replaces" select="exslt:node-set($otherdoc)//*[self::component or self::collection or self::package or self::layer]/@replace"/>
		</xsl:apply-templates>
	
	</xsl:copy>
</xsl:template>

<xsl:template match="systemModel" mode="merge-models">
	<xsl:param name="other"/>	<!-- the parent of the downstream systemModel this is merged with -->
	<xsl:param name="up"/>
	<xsl:param name="down"/>
	<xsl:param name="replaces"/>
	<xsl:copy><xsl:copy-of  select="@*"/>
		<!--  copy metas and comments in between meta. Do not try to merge metadata between models -->
			<xsl:copy-of select="meta | $other/systemModel/meta | comment()[following-sibling::meta]"/>	
		<xsl:apply-templates mode="merge-models">
			<xsl:with-param name="other" select="$other/systemModel"/>
			<xsl:with-param name="up" select="$up"/>
			<xsl:with-param name="down" select="$down"/>
			<xsl:with-param name="replaces" select="$replaces"/>
		</xsl:apply-templates>
	</xsl:copy>
</xsl:template>

<xsl:template match="@*|*|comment()" mode="merge-models"><xsl:copy-of select="."/></xsl:template>


<xsl:template match="meta|comment()[following-sibling::meta]" mode="merge-models"/>
	<!-- copy elesewhere, not here so that metas always appear first-->



<!-- merge levels attribute via std rules -->
<xsl:template match="layer/@levels|package/@levels" mode="merge-models">
	<xsl:param name="other"/><!-- the element contains the other @levels -->
	<xsl:choose>
		<!--  if they are the same, or not specified in the other,  just copy -->
		<xsl:when test=".=$other/@levels or not($other/@levels)"><xsl:copy-of select="."/></xsl:when>
		<xsl:when test="contains(concat(' ',normalize-space(.),' '),concat(' ',normalize-space($other/@levels),' '))">
			<!--upstream completely contains downstream, just copy --> 
			<xsl:copy-of select="."/>
		</xsl:when>
		<xsl:when test="contains(concat(' ',normalize-space($other/@levels),' '),concat(' ',normalize-space(.),' '))">
			<!--  If this is contained is other, then use other-->
			<xsl:copy-of select="$other/@levels"/>
		</xsl:when>
		<xsl:when test="contains(concat(' ',normalize-space($other/@levels),' '),' - ')">
			<!-- if other uses - syntax, then pre/append -->
			<xsl:variable name="lev">
				<xsl:value-of select="substring-before(concat(' ',normalize-space($other/@levels),' '),' - ')"/>
				<xsl:value-of select="concat(' ',.,' ')"/>
				<xsl:value-of select="substring-after(concat(' ',normalize-space($other/@levels),' '),' - ')"/>
			</xsl:variable>
			<xsl:attribute name="levels"><xsl:value-of select="normalize-space($lev)"/></xsl:attribute>
		</xsl:when>
		<xsl:otherwise> <!--  if they differ, use the origin's levels -->
			<xsl:message>Note: levels differ "<xsl:value-of select="."/>" vs "<xsl:value-of select="$other/@levels"/>"</xsl:message>
			<xsl:copy-of select="."/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="copy-sorted-content">
	<xsl:param name="base"/>
	<xsl:param name="to-sort"/>
	<xsl:param name="start"/>
	<xsl:param name="end"/>
	<xsl:param name="down"/>
	<xsl:param name="remainder" select="/.."/>

	<xsl:choose>
		<xsl:when test="not($to-sort)"/>  <!-- nothing left to copy. stop -->
		<xsl:when test="not($base)"/>  <!-- reached end. stop -->
		<xsl:when test="$base[1]/@id=$end/following-sibling::*[1]/@id"/> <!-- passed $end. Stop -->
		<xsl:when test="$base[1]/@id = $to-sort[1]/@id">  <!-- both lists start with same item -->
			<xsl:if test="$base[1]/@id!=$end/@id"> <!-- not at end, so keep going -->
				<xsl:call-template name="copy-sorted-content">
					<xsl:with-param name="base" select="$base[position() != 1]"/>
					<xsl:with-param name="to-sort" select="$to-sort[position() != 1]"/>
					<xsl:with-param name="remainder" select="$remainder"/>
					<xsl:with-param name="start" select="$start"/>
					<xsl:with-param name="end" select="$end"/>
					<xsl:with-param name="down" select="$down"/>
				</xsl:call-template>		
			</xsl:if>
		</xsl:when>	
		<xsl:when test="$remainder[@id = $base[1]/@id]"> <!-- left over item is in $base -->
			<xsl:call-template name="copy-sorted-content">
				<xsl:with-param name="base" select="$base[position() != 1]"/>
				<xsl:with-param name="to-sort" select="$to-sort"/>
				<xsl:with-param name="remainder" select="$remainder[@id != $base[1]/@id]"/>
				<xsl:with-param name="start" select="$start"/>
				<xsl:with-param name="end" select="$end"/>
				<xsl:with-param name="down" select="$down"/>
			</xsl:call-template>		
		</xsl:when>
		<xsl:when test="not($base[@id = $to-sort[1]/@id])"> <!-- in to-sort, but not base -->		
			<xsl:if test="$base[1]/@id=$end/@id  and not($base[@id=$to-sort[1]/@before])">
			 	<!-- if at end, then this needs to be copied
					don't copy if the before ID is found in $base	-->
				<xsl:apply-templates mode="merge-copy-of" select="$to-sort[1]">
					<xsl:with-param name="origin" select="$down"/>
					<xsl:with-param name="root" select="$end/ancestor::systemModel"/>
				</xsl:apply-templates>
			</xsl:if>			
		<xsl:call-template name="copy-sorted-content">
			<xsl:with-param name="base" select="$base"/>
			<xsl:with-param name="to-sort" select="$to-sort[position() != 1]"/>
			<xsl:with-param name="remainder" select="$remainder"/>
			<xsl:with-param name="start" select="$start"/>
			<xsl:with-param name="end" select="$end"/>
			<xsl:with-param name="down" select="$down"/>
		</xsl:call-template>		
		</xsl:when>	
		<xsl:when test="not($to-sort[@id = $base[1]/@id])"> <!-- in base, but not to-sort -->		
		<xsl:call-template name="copy-sorted-content">
			<xsl:with-param name="base" select="$base[position() != 1]"/>
			<xsl:with-param name="to-sort" select="$to-sort"/>
			<xsl:with-param name="remainder" select="$remainder"/>
			<xsl:with-param name="start" select="$start"/>
			<xsl:with-param name="end" select="$end"/>
			<xsl:with-param name="down" select="$down"/>
		</xsl:call-template>		
		</xsl:when>	
		<xsl:when test="$base[@id = $to-sort[1]/@id]"> <!-- is in base, but not 1st one-->
			<xsl:call-template name="copy-sorted-content">
				<xsl:with-param name="base" select="$base"/>
				<xsl:with-param name="to-sort" select="$to-sort[position() != 1] "/>
				<xsl:with-param name="remainder" select="$remainder | $to-sort[1]"/>
				<xsl:with-param name="start" select="$start"/>
				<xsl:with-param name="end" select="$end"/>
				<xsl:with-param name="down" select="$down"/>
			</xsl:call-template>
		</xsl:when>	
	</xsl:choose>
</xsl:template>

<xsl:template match="node()" mode="merge-data">
	<xsl:copy-of select="." />
</xsl:template>

<xsl:template match="meta" mode="merge-data">
	<xsl:param name="metas" />
	<!-- compare this meta against all metas in the  merged doc
		if they are identical, then ignore this one.
		identical is computed by translating to a string, normalising some known parts. This might be slow in some cases-->
	<xsl:variable name="val"><xsl:apply-templates select="." mode="as-xml-text" /></xsl:variable>
	<xsl:variable name="match">
		<xsl:for-each select="$metas">
			<xsl:variable name="cur"><xsl:apply-templates select="." mode="as-xml-text" /></xsl:variable>
			<xsl:if test="$cur=$val">*</xsl:if>
		</xsl:for-each>
	</xsl:variable>
	<xsl:if test="$match='' ">
		<xsl:copy-of select="." />
	</xsl:if>
</xsl:template>

<xsl:template match="text()[normalize-space(.)='']" mode="as-xml-text"/>
<xsl:template match="node()" mode="as-xml-text"><xsl:value-of select="."/></xsl:template>
<xsl:template match="comment()" mode="as-xml-text">&lt;--<xsl:value-of select="."/>--&gt;</xsl:template>
<xsl:template match="@*" mode="as-xml-text">
	<xsl:value-of select="concat(' ',name())"/>="<xsl:value-of select="."/>"</xsl:template>
<xsl:template match="*" mode="as-xml-text">
	<xsl:value-of select="concat('&lt;',name())"/>
	<xsl:apply-templates select="@*" mode="as-xml-text"/>
	<xsl:if test="self::meta and not(@rel)"> rel="Generic"</xsl:if>
	<xsl:if test="self::meta and not(@type)"> type="auto"</xsl:if>
	<xsl:text>&gt;</xsl:text>
	<xsl:apply-templates select="node()" mode="as-xml-text"/>
	<xsl:value-of select="concat('&lt;/',name(),'&gt;')"/>
</xsl:template>


<xsl:template match="layer | package | collection | component" mode="merge-models">
	<xsl:param name="other"/>	<!-- the downstream item of the parent's rank that contains a potential items this is merged with -->
	<xsl:param name="up"/>
	<xsl:param name="down"/>
	<xsl:param name="replaces"/>
	<xsl:variable name="this" select="."/>	<!-- current item -->

	<!-- match = this item in the downstream model -->	
	<xsl:variable name="match" select="$other/*[@id=current()/@id]"/>

	<xsl:choose>
		<xsl:when test="$replaces[.=$this/@id] or (self::component and $match)">  <!-- replace the item instead of merge -->
			<xsl:message>Note: <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" <xsl:choose>
				<xsl:when test="self::component and $match">overridden in downstream sysdef</xsl:when>
				<xsl:otherwise><xsl:for-each select="$replaces[.=$this/@id]/..">replaced by <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>"</xsl:for-each></xsl:otherwise>
			</xsl:choose>
			</xsl:message>
			<!-- if the removed item is in the downstream doc, just copy that and ignore the upstream contents -->
			<xsl:apply-templates mode="merge-copy-of" select="$match">
				<xsl:with-param name="origin" select="$down"/>
				<xsl:with-param name="root" select="$this/ancestor::systemModel"/>			
			</xsl:apply-templates>		
		</xsl:when>
		<xsl:otherwise>
			<!-- remove this if it's in the list of stuff to be replaced-->

	<!-- prev = the previous item before the current one (no metas, only named items)-->
	<xsl:variable name="prev" select="preceding-sibling::*[@id][1]"/> 

	<!-- copy all items between this and prev  that are solely in the downstream model -->	 		

	<xsl:choose>
		<xsl:when test="$match and (not($prev) or $other/*[@id= $prev/@id] )">
			<xsl:call-template name="copy-sorted-content">
				<xsl:with-param name="base" select="../*[@id]"/>
				<xsl:with-param name="to-sort" select="$other/*[@id]"/>
				<xsl:with-param name="start" select="$prev"/>
				<xsl:with-param name="end" select="."/>
				<xsl:with-param name="down" select="$down"/>
			</xsl:call-template>
		</xsl:when>
	<xsl:when test="not($match/preceding-sibling::*[@id=$this/../*/@id]) and $other/*[@id= current()/@id]/preceding-sibling::*[@id and not(@before)]">
		<!-- if this is the first item in other that's also in this, then put all new items from other here -->
		<xsl:apply-templates mode="merge-copy-of" select="$match/preceding-sibling::*[@id and not(@before)]">
			<xsl:with-param name="origin" select="$down"/>
			<xsl:with-param name="root" select="$this/ancestor::systemModel"/>	
		</xsl:apply-templates>
	</xsl:when>
	</xsl:choose>

 	<!-- just copy anything identified as being before this, assume they're all ok -->
	<xsl:apply-templates mode="merge-copy-of" select="$other/*[@before=current()/@id]">
		<xsl:with-param name="remove-before" select="1"/>
		<xsl:with-param name="origin" select="$down"/>
		<xsl:with-param name="root" select="$this/ancestor::systemModel"/>	
	</xsl:apply-templates>

	
	<xsl:copy>
		<xsl:apply-templates select="@*" mode="merge-models"> <!-- copy upstream attributes -->
			<xsl:with-param name="other" select="$match"/>
		</xsl:apply-templates>
		
		<xsl:if test="self::component and not(@origin-model) and $up/@name">
			<!-- insert origin-model and optional root for components only -->
			<xsl:attribute name="origin-model">
				<xsl:value-of select="$up/@name"/>
			</xsl:attribute>
			<xsl:if test="not(@root)">
				<xsl:copy-of select="$up/@root"/>
			</xsl:if>
		</xsl:if>
		
		<xsl:for-each select="$match/@*[name()!='replace']">  <!-- copy downstream attributes, only if not set on upstream -->
			<xsl:if test="not($this/@*[name()=name(current())])"><xsl:copy-of select="."/></xsl:if>
		</xsl:for-each>

		<xsl:if test="$match/@replace"> <!-- check replace separately -->
			<xsl:if test="not($this/ancestor::SystemDefinition//*[(self::component or self::collection or self::package or self::layer) and $match/@replace=@id])">
				<!-- only remove replace if it's been used -->
				<xsl:copy-of select="$match/@replace"/>
			</xsl:if>
		</xsl:if>
		
		<xsl:choose>
			<xsl:when test="self::component">
				<!-- copy all units, metas and comments from this
					copy all metas in the merged component
					copy any new comments in the merged component (not duplicates)
					if there are no units in the this, copy all units in the merged component
					if there are units in this, copy only the versioned units in the merged component (only those versions not already specified) -->
				<xsl:copy-of select="*|comment() | $match/meta |$match/unit[not($this/unit)] | $match/unit[$this/unit and @version[.!=$this/unit/@version]] | $match/comment()[.!=$this/comment()]"/>				
			</xsl:when>
			<xsl:otherwise>

				<!--  copy metas and comments in between meta. Do not try to merge metadata between models -->
				<xsl:apply-templates select="meta | $match/meta | comment()[following-sibling::meta]" mode="merge-data">
					<xsl:with-param name="metas" select="$match/meta"/>
				</xsl:apply-templates>
				<xsl:copy-of select=" $match/meta | $match/comment()[following-sibling::meta]"/>
				
				<xsl:apply-templates mode="merge-models">
					<xsl:with-param name="other" select="$match"/>
					<xsl:with-param name="up" select="$up"/>
					<xsl:with-param name="down" select="$down"/>
					<xsl:with-param name="replaces" select="$replaces"/>
				</xsl:apply-templates>
				<!--  don't copy if explicitly or implicitly placed already-->
				<xsl:for-each select="$match/*[not(@before) and not(following-sibling::*[@id=$this/*/@id])]">
					<xsl:if test="not($this/*[@id=current()/@id])">
						<xsl:apply-templates mode="merge-copy-of" select=".">
							<xsl:with-param name="origin" select="$down"/>
							<xsl:with-param name="root" select="$this/ancestor::systemModel"/>			
						</xsl:apply-templates>
					</xsl:if>
				</xsl:for-each>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>

		</xsl:otherwise>
	</xsl:choose>
	
	<xsl:if test="self::layer and not(following-sibling::layer)">	
		<!-- for the last layer, tack on any remaining layers -->
		<xsl:apply-templates mode="merge-copy-of" select="$other/layer[not(@before) and not(following-sibling::*[@id=$this/../layer/@id]) and not(@id=$this/../layer/@id)]">
			<xsl:with-param name="origin" select="$down"/>
			<xsl:with-param name="root" select="$this/ancestor::systemModel"/>			
		</xsl:apply-templates>		
	</xsl:if>
</xsl:template>



<xsl:template match="*" mode="merge-copy-of">
	<xsl:param name="remove-before" select="0"/> <!-- set to true if any before attribute is to be removed -->
	<xsl:param name="origin"/>	<!--the element containing the @name to use the origin-model attribute  -->
	<xsl:param name="root"/> 	<!--the systemModel element in the upstream doc  -->

	<xsl:choose>
		<!-- this might slow things down, consider making optional -->
		<xsl:when test="not(self::layer) and count($root/descendant::*[name()=name(current()/..) and @id!=current()/../@id]/*[@id=current()/@id])">
			<xsl:message>Warning: <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" moved in downstream model. Ignoring moved <xsl:value-of select="name()"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:message>
		</xsl:when>
		<xsl:otherwise>
			<!-- save all content in a variable to test to see if it's got any problems (ie been removed due to errors)-->
			<xsl:variable name="content">
				<xsl:apply-templates select="*|comment()" mode="merge-copy-of">
					<xsl:with-param name="origin" select="$origin"/>
					<xsl:with-param name="root" select="$root"/>
				</xsl:apply-templates>
			</xsl:variable>
			<xsl:choose>
				<!-- if all elements in this have been deleted, throw out this element -->
				<xsl:when test="not(exslt:node-set($content)/*) and *">
					<xsl:message>Warning: All content in downstream <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" is invalid. Ignoring <xsl:value-of select="name()"/>
						<xsl:text>&#xa;</xsl:text>
					</xsl:message>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy>
						<xsl:call-template name="merge-copy-of-atts">
							<xsl:with-param name="remove-before" select="$remove-before"/>
							<xsl:with-param name="root" select="$root"/>
						</xsl:call-template>
						<xsl:copy-of select="exslt:node-set($content)"/>
					</xsl:copy>
				</xsl:otherwise>
			</xsl:choose>					
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template match="comment()|@*" mode="merge-copy-of">
	<xsl:copy-of select="."/>
</xsl:template>


<xsl:template name="merge-copy-of-atts">
	<xsl:param name="remove-before" select="0"/> <!-- set to true if any before attribute is to be removed -->
	<xsl:param name="root"/> 	<!--the systemModel element in the upstream doc  -->
	<xsl:choose>
		<xsl:when test="$remove-before">
			<xsl:copy-of select="@*[name()!='before' and name()!='replace']"/>
		</xsl:when>
		<xsl:otherwise><xsl:copy-of select="@*[name()!='replace']"/></xsl:otherwise>
	</xsl:choose>
	<xsl:if test="@replace and not($root/descendant::*[(self::component or self::collection or self::package or self::layer) and @id=current()/@replace])">
		<!-- only include replace if it's not been used -->
		<xsl:copy-of select="@replace"/>
	</xsl:if>
</xsl:template>

<xsl:template match="component" mode="merge-copy-of">
	<xsl:param name="remove-before" select="0"/> <!-- set to true if any before attribute is to be removed -->
	<xsl:param name="origin"/>	<!--the element containing the @name to use the origin-model attribute  -->
	<xsl:param name="root"/> 	<!--the systemModel element in the upstream doc  -->

	<xsl:choose>
		<!-- this might slow things down, consider making optional -->
		<xsl:when test="$root/descendant::collection[@id!=current()/../@id]/component[@id=current()/@id]">
			<xsl:message>Warning: <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" moved in downstream model. Ignoring moved <xsl:value-of select="name()"/>
				<xsl:text>&#xa;</xsl:text>
			</xsl:message>
		</xsl:when>
		<xsl:otherwise>
			<xsl:copy>
				<xsl:call-template name="merge-copy-of-atts">
					<xsl:with-param name="remove-before" select="$remove-before"/>
					<xsl:with-param name="root" select="$root"/>
				</xsl:call-template>
				<xsl:if test="not(@origin-model) and $origin/@name">
					<xsl:attribute name="origin-model">
						<xsl:value-of select="$origin/@name"/>
					</xsl:attribute>
					<xsl:if test="not(@root)">
						<xsl:copy-of select="$origin/@root"/>
					</xsl:if>
				</xsl:if>
				<xsl:copy-of select="*|comment()"/>
			</xsl:copy>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>
