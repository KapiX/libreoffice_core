/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "XMLIndexUserSourceContext.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XIndexReplace.hpp>
#include <sax/tools/converter.hxx>
#include "XMLIndexTemplateContext.hxx"
#include "XMLIndexTitleTemplateContext.hxx"
#include "XMLIndexTOCStylesContext.hxx"
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimp.hxx>
#include <xmloff/txtimp.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltoken.hxx>
#include <rtl/ustring.hxx>


using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::xmloff::token::IsXMLToken;
using ::xmloff::token::XML_USER_INDEX_ENTRY_TEMPLATE;
using ::xmloff::token::XML_OUTLINE_LEVEL;

XMLIndexUserSourceContext::XMLIndexUserSourceContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrfx,
    const OUString& rLocalName,
    Reference<XPropertySet> & rPropSet) :
        XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName,
                                  rPropSet, true),
        sCreateFromEmbeddedObjects("CreateFromEmbeddedObjects"),
        sCreateFromGraphicObjects("CreateFromGraphicObjects"),
        sCreateFromMarks("CreateFromMarks"),
        sCreateFromTables("CreateFromTables"),
        sCreateFromTextFrames("CreateFromTextFrames"),
        sUseLevelFromSource("UseLevelFromSource"),
        sCreateFromLevelParagraphStyles("CreateFromLevelParagraphStyles"),
        sUserIndexName("UserIndexName"),
        bUseObjects(false),
        bUseGraphic(false),
        bUseMarks(false),
        bUseTables(false),
        bUseFrames(false),
        bUseLevelFromSource(false),
        bUseLevelParagraphStyles(false)
{
}

XMLIndexUserSourceContext::~XMLIndexUserSourceContext()
{
}

void XMLIndexUserSourceContext::ProcessAttribute(
    enum IndexSourceParamEnum eParam,
    const OUString& rValue)
{
    bool bTmp(false);

    switch (eParam)
    {
        case XML_TOK_INDEXSOURCE_USE_INDEX_MARKS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseMarks = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_OBJECTS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseObjects = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_GRAPHICS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseGraphic = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_TABLES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseTables = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_FRAMES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseFrames = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COPY_OUTLINE_LEVELS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseLevelFromSource = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_INDEX_SOURCE_STYLES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseLevelParagraphStyles = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USER_INDEX_NAME:
            sIndexName = rValue;
            break;

        default:
            XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
            break;
    }
}


void XMLIndexUserSourceContext::EndElement()
{
    rIndexPropertySet->setPropertyValue(sCreateFromEmbeddedObjects, css::uno::Any(bUseObjects));
    rIndexPropertySet->setPropertyValue(sCreateFromGraphicObjects, css::uno::Any(bUseGraphic));
    rIndexPropertySet->setPropertyValue(sUseLevelFromSource, css::uno::Any(bUseLevelFromSource));
    rIndexPropertySet->setPropertyValue(sCreateFromMarks, css::uno::Any(bUseMarks));
    rIndexPropertySet->setPropertyValue(sCreateFromTables, css::uno::Any(bUseTables));
    rIndexPropertySet->setPropertyValue(sCreateFromTextFrames, css::uno::Any(bUseFrames));
    rIndexPropertySet->setPropertyValue(sCreateFromLevelParagraphStyles, css::uno::Any(bUseLevelParagraphStyles));

    if( !sIndexName.isEmpty() )
    {
        rIndexPropertySet->setPropertyValue(sUserIndexName, css::uno::Any(sIndexName));
    }

    XMLIndexSourceBaseContext::EndElement();
}


SvXMLImportContext* XMLIndexUserSourceContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList )
{
    if ( (XML_NAMESPACE_TEXT == nPrefix) &&
         (IsXMLToken(rLocalName, XML_USER_INDEX_ENTRY_TEMPLATE)) )
    {
        return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet,
                                           nPrefix, rLocalName,
                                           aSvLevelNameTOCMap,
                                           XML_OUTLINE_LEVEL,
                                           aLevelStylePropNameTOCMap,
                                           aAllowedTokenTypesUser);
    }
    else
    {
        return XMLIndexSourceBaseContext::CreateChildContext(nPrefix,
                                                             rLocalName,
                                                             xAttrList);
    }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
