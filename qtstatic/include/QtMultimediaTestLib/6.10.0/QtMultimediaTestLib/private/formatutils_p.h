// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef FORMATUTILS_P_H
#define FORMATUTILS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtMultimedia/qmediaformat.h>
#include <set>
#include <vector>

QT_BEGIN_NAMESPACE

std::set<QMediaFormat::VideoCodec> allVideoCodecs(bool includeUnspecified = false);
std::set<QMediaFormat::AudioCodec> allAudioCodecs(bool includeUnspecified = false);
std::set<QMediaFormat::FileFormat> allFileFormats(bool includeUnspecified = false);
std::vector<QMediaFormat> allMediaFormats(bool includeUnspecified = false);

QT_END_NAMESPACE

#endif
