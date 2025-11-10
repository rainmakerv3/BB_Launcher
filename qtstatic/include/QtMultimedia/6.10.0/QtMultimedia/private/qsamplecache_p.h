// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSAMPLECACHE_P_H
#define QSAMPLECACHE_P_H

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

#include <QtCore/qmutex.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qset.h>
#include <QtCore/qthread.h>
#include <QtCore/qfuture.h>
#include <QtCore/qurl.h>
#include <QtCore/private/qglobal_p.h>
#include <QtCore/private/qexpected_p.h>
#include <QtMultimedia/qaudioformat.h>

#include <memory>
#include <optional>

QT_BEGIN_NAMESPACE

class QSampleCache;

class Q_MULTIMEDIA_EXPORT QSample
{
public:
    friend class QSampleCache;
    enum State : uint8_t {
        Creating,
        Error,
        Ready,
    };
    using SharedSamplePromise = QSharedPointer<QPromise<q23::expected<QSample *, QSample::State>>>;
    ~QSample();

    State state() const;
    const QByteArray& data() const { Q_ASSERT(state() == Ready); return m_soundData; }
    const QAudioFormat& format() const { Q_ASSERT(state() == Ready); return m_audioFormat; }

    void setError();
    void setData(QByteArray, QAudioFormat);

    QSample(QUrl url, QSampleCache *parent);

private:
    QSample();

    // clang-format off
    QSampleCache *m_parent;
    QByteArray   m_soundData;
    QAudioFormat m_audioFormat;
    const QUrl   m_url;
    State        m_state = State::Creating;
    // clang-format on

    friend class QSampleCache;
    void clearParent();
};

using SharedSamplePtr = std::shared_ptr<QSample>;
using WeakSamplePtr = std::weak_ptr<QSample>;

class Q_MULTIMEDIA_EXPORT QSampleCache : public QObject
{
public:
    friend class QSample;

    enum class SampleSourceType
    {
        File,
        NetworkManager,
    };

    explicit QSampleCache(QObject *parent = nullptr);
    ~QSampleCache() override;

    QFuture<SharedSamplePtr> requestSampleFuture(const QUrl &);

    bool isCached(const QUrl& url) const;

    // For tests only
    void setSampleSourceType(SampleSourceType sampleSourceType)
    {
        m_sampleSourceType = sampleSourceType;
    }

private:
    std::unique_ptr<QIODevice> createStreamForSample(QSample &sample);

private:
    using SharedSamplePromise = std::shared_ptr<QPromise<SharedSamplePtr>>;

    mutable QRecursiveMutex m_mutex;

    std::map<QUrl, WeakSamplePtr> m_loadedSamples;
    std::map<QUrl, std::pair<SharedSamplePtr, QList<SharedSamplePromise>>> m_pendingSamples;

    void removeUnreferencedSample(const QUrl &url);

    using SampleLoadResult = std::optional<std::pair<QByteArray, QAudioFormat>>;

    static SampleLoadResult loadSample(QByteArray);

#if QT_CONFIG(thread)
    static SampleLoadResult
    loadSample(const QUrl &, std::optional<SampleSourceType> forceSourceType = std::nullopt);
    QThreadPool m_threadPool;
#endif
    QFuture<SampleLoadResult> loadSampleAsync(const QUrl &);

    std::optional<SampleSourceType> m_sampleSourceType;
};

QT_END_NAMESPACE

#endif // QSAMPLECACHE_P_H
