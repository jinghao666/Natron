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

#ifndef Engine_RotoStrokeItemSerialization_h
#define Engine_RotoStrokeItemSerialization_h

#include "Serialization/KnobTableItemSerialization.h"
#include "Serialization/SerializationFwd.h"

#define kRotoStrokeItemSerializationBrushTypeSolid "Solid"
#define kRotoStrokeItemSerializationBrushTypeEraser "Eraser"
#define kRotoStrokeItemSerializationBrushTypeClone "Clone"
#define kRotoStrokeItemSerializationBrushTypeReveal "Reveal"
#define kRotoStrokeItemSerializationBrushTypeBlur "Blur"
#define kRotoStrokeItemSerializationBrushTypeSmear "Smear"
#define kRotoStrokeItemSerializationBrushTypeDodge "Dodge"
#define kRotoStrokeItemSerializationBrushTypeBurn "Burn"

#define kSerializationStrokeTag "Stroke"

SERIALIZATION_NAMESPACE_ENTER;


class RotoStrokeItemSerialization
    : public KnobTableItemSerialization
{

public:


    std::string _brushType;
    struct PointCurves
    {
        CurveSerializationPtr x,y,pressure;
    };
    std::list<PointCurves> _subStrokes;


    RotoStrokeItemSerialization()
        : KnobTableItemSerialization()
        , _brushType()
        , _subStrokes()
    {
        verbatimTag = kSerializationStrokeTag;
        _emitMap = false;
    }

    virtual ~RotoStrokeItemSerialization()
    {
    }

    virtual void encode(YAML::Emitter& em) const OVERRIDE;

    virtual void decode(const YAML::Node& node) OVERRIDE;


};

SERIALIZATION_NAMESPACE_EXIT;

#endif // Engine_RotoStrokeItemSerialization_h
