/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2016 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

#include "KnobTableItemSerialization.h"

#include <iostream>

GCC_DIAG_UNUSED_LOCAL_TYPEDEFS_OFF
#include <yaml-cpp/yaml.h>
GCC_DIAG_UNUSED_LOCAL_TYPEDEFS_ON

#include "Serialization/BezierSerialization.h"
#include "Serialization/RotoStrokeItemSerialization.h"
#include "Serialization/TrackerSerialization.h"

SERIALIZATION_NAMESPACE_ENTER;

static KnobTableItemSerializationPtr createSerializationObjectForItemTag(const std::string& tag)
{
    if (tag == kSerializationRotoLayerTag) {
        KnobTableItemSerializationPtr ret(new KnobTableItemSerialization);
        ret->verbatimTag = kSerializationRotoLayerTag;
        return ret;
    } else if (tag == kSerializationClosedBezierTag) {
        BezierSerializationPtr ret(new BezierSerialization(false));
        return ret;
    } else if (tag == kSerializationOpenedBezierTag) {
        BezierSerializationPtr ret(new BezierSerialization(true));
        return ret;
    } else if (tag == kSerializationStrokeTag) {
        RotoStrokeItemSerializationPtr ret(new RotoStrokeItemSerialization);
        return ret;
    } else if (tag == kSerializationTrackTag) {
        TrackSerializationPtr ret(new TrackSerialization);
        return ret;
    } else {
        std::cerr << "Unknown YAML tag " << tag << std::endl;
        throw YAML::InvalidNode();
        assert(false);
        return KnobTableItemSerializationPtr();
    }
}

void
KnobTableItemSerialization::encode(YAML::Emitter& em) const
{
    if (_emitMap) {
        em << YAML::BeginMap;
    }
    em << YAML::Key << "ScriptName" << YAML::Value << scriptName;
    if (label != scriptName) {
        em << YAML::Key << "Label" << YAML::Value << label;
    }
    if (!children.empty()) {
        em << YAML::Key << "Children" << YAML::Value;
        em << YAML::BeginSeq;
        for (std::list<KnobTableItemSerializationPtr>::const_iterator it = children.begin(); it != children.end(); ++it) {
            assert(!(*it)->verbatimTag.empty());
            em << YAML::VerbatimTag((*it)->verbatimTag);
            (*it)->encode(em);
        }
        em << YAML::EndSeq;
    }
    if (!knobs.empty()) {
        em << YAML::Key << "Params" << YAML::Value << YAML::BeginSeq;
        for (KnobSerializationList::const_iterator it = knobs.begin(); it!=knobs.end(); ++it) {
            (*it)->encode(em);
        }
        em << YAML::EndSeq;
    }
    
    if (_emitMap) {
        em << YAML::EndMap;
    }
}

void
KnobTableItemSerialization::decode(const YAML::Node& node)
{
    if (!node.IsMap()) {
        throw YAML::InvalidNode();
    }

    scriptName = node["ScriptName"].as<std::string>();
    if (node["Label"]) {
        label = node["Label"].as<std::string>();
    } else {
        label = scriptName;
    }
    if (node["Children"]) {
        YAML::Node childrenNode = node["Children"];
        for (std::size_t i = 0; i < childrenNode.size(); ++i) {
            const std::string& nodeTag = childrenNode[i].Tag();
            KnobTableItemSerializationPtr child = createSerializationObjectForItemTag(nodeTag);
            if (!child) {
                continue;
            }
            child->decode(childrenNode[i]);
            children.push_back(child);
        }
    }
    if (node["Params"]) {
        YAML::Node paramsNode = node["Params"];
        for (std::size_t i = 0; i < paramsNode.size(); ++i) {
            KnobSerializationPtr child(new KnobSerialization);
            child->decode(paramsNode[i]);
            knobs.push_back(child);
        }
    }
}

void
KnobItemsTableSerialization::encode(YAML::Emitter& em) const
{
    if (nodeScriptName.empty() && items.empty()) {
        return;
    }
    em << YAML::BeginMap;
    if (!nodeScriptName.empty()) {
        em << YAML::Key << "Node" << YAML::Value << nodeScriptName;
    }
    if (!tableIdentifier.empty()) {
        em << YAML::Key << "ID" << YAML::Value << tableIdentifier;
    }
    if (!items.empty()) {
        em << YAML::Key << "Items" << YAML::Value << YAML::BeginSeq;
        for (std::list<KnobTableItemSerializationPtr>::const_iterator it = items.begin(); it!= items.end(); ++it) {
            assert(!(*it)->verbatimTag.empty());
            em << YAML::VerbatimTag((*it)->verbatimTag);
            (*it)->encode(em);
        }
    }
    em << YAML::EndSeq;
    em << YAML::EndMap;
}

void
KnobItemsTableSerialization::decode(const YAML::Node& node)
{
    if (!node.IsMap()) {
        throw YAML::InvalidNode();
    }
    
    if (node["Node"]) {
        nodeScriptName = node["Node"].as<std::string>();
    }

    if (node["ID"]) {
        tableIdentifier = node["ID"].as<std::string>();
    }
    
    if (node["Items"]) {
        YAML::Node itemsNode = node["Items"];
        for (std::size_t i = 0; i < itemsNode.size(); ++i) {
            const std::string& nodeTag = itemsNode[i].Tag();
            KnobTableItemSerializationPtr s = createSerializationObjectForItemTag(nodeTag);
            if (!s) {
                continue;
            }
            s->decode(itemsNode[i]);
            items.push_back(s);
        }
    }
}



SERIALIZATION_NAMESPACE_EXIT;