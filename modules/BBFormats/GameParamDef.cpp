// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GameParam.h"

namespace fs = std::filesystem;

namespace FileHelper {

// witchy gets paramdefs from premade xmls, so I'm assuming this is a valid approach
GameParam::ParamDef GameParam::GetParamDef(const std::string& filename) {
    ParamDef def;

    if (filename.contains("ActionButtonParam")) {
        Field regionType;
        regionType.fieldName = "regionType";
        regionType.defType = DefType::u8;
        regionType.sortId = 100;
        regionType.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(regionType);

        Field padding1;
        padding1.fieldName = "padding1";
        padding1.defType = DefType::dummy8;
        padding1.arrayLength = 3;
        padding1.sortId = 100010;
        padding1.defaultValue.push_back(static_cast<int>(0));
        padding1.defaultValue.push_back(static_cast<int>(0));
        padding1.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(padding1);

        Field dummyPoly1;
        dummyPoly1.fieldName = "dummyPoly1";
        dummyPoly1.defType = DefType::s32;
        dummyPoly1.sortId = 200;
        dummyPoly1.defaultValue.push_back(static_cast<int>(-1));
        def.paramFields.push_back(dummyPoly1);

        Field dummyPoly2;
        dummyPoly2.fieldName = "dummyPoly2";
        dummyPoly2.defType = DefType::s32;
        dummyPoly2.sortId = 210;
        dummyPoly2.defaultValue.push_back(static_cast<int>(-1));
        def.paramFields.push_back(dummyPoly2);

        Field radius;
        radius.fieldName = "radius";
        radius.defType = DefType::f32;
        radius.sortId = 300;
        radius.defaultValue.push_back(static_cast<float>(1.2));
        def.paramFields.push_back(radius);

        Field angle;
        angle.fieldName = "angle";
        angle.defType = DefType::s32;
        angle.sortId = 400;
        angle.defaultValue.push_back(static_cast<int>(180));
        def.paramFields.push_back(angle);

        Field depth;
        depth.fieldName = "depth";
        depth.defType = DefType::f32;
        depth.sortId = 500;
        depth.defaultValue.push_back(static_cast<float>(0));
        def.paramFields.push_back(depth);

        Field width;
        width.fieldName = "width";
        width.defType = DefType::f32;
        width.sortId = 510;
        width.defaultValue.push_back(static_cast<float>(0));
        def.paramFields.push_back(width);

        Field height;
        height.fieldName = "height";
        height.defType = DefType::f32;
        height.sortId = 520;
        height.defaultValue.push_back(static_cast<float>(2));
        def.paramFields.push_back(height);

        Field baseHeightOffset;
        baseHeightOffset.fieldName = "baseHeightOffset";
        baseHeightOffset.defType = DefType::f32;
        baseHeightOffset.sortId = 600;
        baseHeightOffset.defaultValue.push_back(static_cast<float>(-1));
        def.paramFields.push_back(baseHeightOffset);

        Field angleCheckType;
        angleCheckType.fieldName = "angleCheckType";
        angleCheckType.defType = DefType::u8;
        angleCheckType.sortId = 700;
        angleCheckType.defaultValue.push_back(static_cast<float>(0));
        def.paramFields.push_back(angleCheckType);

        Field padding2;
        padding2.fieldName = "padding2";
        padding2.defType = DefType::dummy8;
        padding2.arrayLength = 3;
        padding2.sortId = 100020;
        padding2.defaultValue.push_back(static_cast<int>(0));
        padding2.defaultValue.push_back(static_cast<int>(0));
        padding2.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(padding2);

        Field allowAngle;
        allowAngle.fieldName = "allowAngle";
        allowAngle.defType = DefType::s32;
        allowAngle.sortId = 800;
        allowAngle.defaultValue.push_back(static_cast<int>(90));
        def.paramFields.push_back(allowAngle);

        Field textBoxType;
        textBoxType.fieldName = "textBoxType";
        textBoxType.defType = DefType::u8;
        textBoxType.sortId = 900;
        textBoxType.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(textBoxType);

        Field padding3;
        padding3.fieldName = "padding3";
        padding3.defType = DefType::dummy8;
        padding3.arrayLength = 3;
        padding3.sortId = 100030;
        padding3.defaultValue.push_back(static_cast<int>(0));
        padding3.defaultValue.push_back(static_cast<int>(0));
        padding3.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(padding3);

        Field textId;
        textId.fieldName = "textId";
        textId.defType = DefType::s32;
        textId.sortId = 1000;
        textId.defaultValue.push_back(static_cast<int>(10010100));
        def.paramFields.push_back(textId);

        Field invalidFlag;
        invalidFlag.fieldName = "invalidFlag";
        invalidFlag.defType = DefType::s32;
        invalidFlag.sortId = 1100;
        invalidFlag.defaultValue.push_back(static_cast<int>(-1));
        def.paramFields.push_back(invalidFlag);

        Field grayoutFlag;
        grayoutFlag.fieldName = "grayoutFlag";
        grayoutFlag.defType = DefType::s32;
        grayoutFlag.sortId = 1200;
        grayoutFlag.defaultValue.push_back(static_cast<int>(-1));
        def.paramFields.push_back(grayoutFlag);

        Field priority;
        priority.fieldName = "priority";
        priority.defType = DefType::s32;
        priority.sortId = 1300;
        priority.defaultValue.push_back(static_cast<int>(5));
        def.paramFields.push_back(priority);

        Field execInvalidTime;
        execInvalidTime.fieldName = "execInvalidTime";
        execInvalidTime.defType = DefType::f32;
        execInvalidTime.sortId = 1400;
        execInvalidTime.defaultValue.push_back(static_cast<float>(3));
        def.paramFields.push_back(execInvalidTime);

        Field execButtonCircle;
        execButtonCircle.fieldName = "execButtonCircle";
        execButtonCircle.defType = DefType::u8;
        execButtonCircle.sortId = 1500;
        execButtonCircle.defaultValue.push_back(static_cast<int>(1));
        def.paramFields.push_back(execButtonCircle);

        Field padding4;
        padding4.fieldName = "padding4";
        padding4.defType = DefType::dummy8;
        padding4.arrayLength = 3;
        padding4.sortId = 100040;
        padding4.defaultValue.push_back(static_cast<int>(0));
        padding4.defaultValue.push_back(static_cast<int>(0));
        padding4.defaultValue.push_back(static_cast<int>(0));
        def.paramFields.push_back(padding4);
    } else {
        def.loaded = false;
    }

    return def;
}

} // namespace FileHelper
