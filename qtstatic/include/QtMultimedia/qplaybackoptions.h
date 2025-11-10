// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLAYBACKOPTIONS_H
#define QPLAYBACKOPTIONS_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qcompare.h>
#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>

#include <chrono>

QT_BEGIN_NAMESPACE

class QPlaybackOptionsPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR(QPlaybackOptionsPrivate)

class QPlaybackOptions
{
    Q_GADGET_EXPORT(Q_MULTIMEDIA_EXPORT)
    Q_PROPERTY(std::chrono::milliseconds networkTimeout READ networkTimeout WRITE setNetworkTimeout RESET
                       resetNetworkTimeout FINAL)
    Q_PROPERTY(PlaybackIntent playbackIntent READ playbackIntent WRITE setPlaybackIntent RESET
                       resetPlaybackIntent)
    Q_PROPERTY(qsizetype probeSize READ probeSize WRITE setProbeSize RESET resetProbeSize)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum class PlaybackIntent {
        Playback,
        LowLatencyStreaming,
    };
    Q_ENUM(PlaybackIntent)

    Q_MULTIMEDIA_EXPORT QPlaybackOptions();
    Q_MULTIMEDIA_EXPORT QPlaybackOptions(const QPlaybackOptions &);
    Q_MULTIMEDIA_EXPORT QPlaybackOptions &operator=(const QPlaybackOptions &);
    QPlaybackOptions(QPlaybackOptions &&) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QPlaybackOptions)
    Q_MULTIMEDIA_EXPORT ~QPlaybackOptions();

    void swap(QPlaybackOptions &other) noexcept { d.swap(other.d); }

    Q_MULTIMEDIA_EXPORT std::chrono::milliseconds networkTimeout() const;
    Q_MULTIMEDIA_EXPORT void setNetworkTimeout(std::chrono::milliseconds timeout);
    Q_MULTIMEDIA_EXPORT void resetNetworkTimeout();

    Q_MULTIMEDIA_EXPORT PlaybackIntent playbackIntent() const;
    Q_MULTIMEDIA_EXPORT void setPlaybackIntent(PlaybackIntent intent);
    Q_MULTIMEDIA_EXPORT void resetPlaybackIntent();

    Q_MULTIMEDIA_EXPORT qsizetype probeSize() const;
    Q_MULTIMEDIA_EXPORT void setProbeSize(qsizetype probeSizeBytes);
    Q_MULTIMEDIA_EXPORT void resetProbeSize();

private:
    friend Q_MULTIMEDIA_EXPORT bool comparesEqual(const QPlaybackOptions &lhs,
                                                  const QPlaybackOptions &rhs) noexcept;
    friend Q_MULTIMEDIA_EXPORT Qt::strong_ordering compareThreeWay(const QPlaybackOptions &lhs,
                                                                   const QPlaybackOptions &rhs) noexcept;
    Q_DECLARE_STRONGLY_ORDERED(QPlaybackOptions)

    friend class QPlaybackOptionsPrivate;
    QExplicitlySharedDataPointer<QPlaybackOptionsPrivate> d;
};

Q_DECLARE_SHARED(QPlaybackOptions)

QT_END_NAMESPACE

#endif
