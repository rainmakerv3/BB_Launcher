// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEMORY_RESOURCE_TLSF_P_H
#define QMEMORY_RESOURCE_TLSF_P_H

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

#include <QtCore/qtclasshelpermacros.h>
#include <QtMultimedia/private/q_pmr_emulation_p.h>

#include <tlsf.h>

QT_BEGIN_NAMESPACE

namespace QtMultimediaPrivate {

struct QTlsfMemoryResource final : pmr::memory_resource
{
    explicit QTlsfMemoryResource(std::size_t preallocatedBytes,
                                 pmr::memory_resource *upstream = pmr::get_default_resource());

    Q_DISABLE_COPY_MOVE(QTlsfMemoryResource)

    ~QTlsfMemoryResource() override;

private:
    void *do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void *p, size_t bytes, size_t alignment) override;
    bool do_is_equal(const memory_resource &other) const noexcept override;

    // cache line alignment on most platforms
    static constexpr auto poolAlignment = std::align_val_t{ 128 };

    QtPrivate::tlsf_t m_tlsf;
    const size_t m_allocationSize;
    std::byte *m_buffer = nullptr;
    pmr::memory_resource *m_upstream{};
};

} // namespace QtMultimediaPrivate

QT_END_NAMESPACE

#endif // QMEMORY_RESOURCE_TLSF_P_H
