// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef QT_QWINDOWSRESAMPLER_H
#define QT_QWINDOWSRESAMPLER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbytearray.h>
#include <QtCore/qbytearrayview.h>
#include <QtCore/private/qcomptr_p.h>
#include <QtCore/private/qexpected_p.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/private/qcominitializer_p.h>
#include <QtMultimedia/private/qplatformaudioresampler_p.h>
#include <QtMultimedia/private/qwindowsmediafoundation_p.h>

#include <mftransform.h>

#include <memory_resource>

struct IMFSample;
struct IMFTransform;

QT_BEGIN_NAMESPACE

class QWindowsMediaFoundation;

class Q_MULTIMEDIA_EXPORT QWindowsResampler : public QPlatformAudioResampler
{
public:
    static bool isAvailable();

    QWindowsResampler();
    ~QWindowsResampler();

    bool setup(const QAudioFormat &in, const QAudioFormat &out);
    void setStartTimeOffset(std::chrono::microseconds); // for QPlatformAudioResampler

    QByteArray resample(QByteArray);
    QByteArray resample(const QByteArrayView &);
    QByteArray resample(const ComPtr<IMFSample> &);

    QAudioBuffer resample(const char *data, size_t size) override;

    // Caveat: the memory resource needs outlive the QWindowsResampler instance (the IMFTransform
    // API does not rule out that the input buffers are kept alive)
    std::pmr::vector<std::byte> resample(QSpan<const std::byte>, std::pmr::memory_resource *);

    QAudioFormat inputFormat() const { return m_inputFormat; }
    QAudioFormat outputFormat() const { return m_outputFormat; }

    quint64 outputBufferSize(quint64 inputBufferSize) const;
    quint64 inputBufferSize(quint64 outputBufferSize) const;

    quint64 totalInputBytes() const { return m_totalInputBytes; }
    quint64 totalOutputBytes() const { return m_totalOutputBytes; }

private:
    qsizetype overAllocatedOutputBufferSize();
    template <typename Functor>
    auto processOutput(ComPtr<IMFMediaBuffer> buffer, Functor &&f)
            -> std::invoke_result_t<Functor, const ComPtr<IMFMediaBuffer> &>;

    q23::expected<QByteArray, HRESULT> processOutput();

    QComInitializer m_comInitializer;
    QWindowsMediaFoundation *m_wmf{ QWindowsMediaFoundation::instance() };
    QMFRuntimeInit m_wmfRuntime{ m_wmf };
    ComPtr<IMFTransform> m_resampler;
    ComPtr<IMFSample> m_inputSample;
    ComPtr<IMFSample> m_outputSample;

    quint64 m_totalInputBytes = 0;
    quint64 m_totalOutputBytes = 0;
    QAudioFormat m_inputFormat;
    QAudioFormat m_outputFormat;

    DWORD m_inputStreamID = 0;

    std::chrono::microseconds m_startTimeOffset;
};

QT_END_NAMESPACE

#endif // QT_QWINDOWSRESAMPLER_H
