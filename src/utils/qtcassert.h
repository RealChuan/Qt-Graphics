// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "utils_global.h"

namespace Utils {
UTILS_EXPORT void writeAssertLocation(const char *msg);
UTILS_EXPORT void dumpBacktrace(int maxdepth);
} // namespace Utils

#define QTC_ASSERT_STRINGIFY_HELPER(x) #x
#define QTC_ASSERT_STRINGIFY(x) QTC_ASSERT_STRINGIFY_HELPER(x)
#define QTC_ASSERT_STRING(cond) \
    ::Utils::writeAssertLocation("\"" cond "\" in " __FILE__ ":" QTC_ASSERT_STRINGIFY(__LINE__))

// The 'do {...} while (0)' idiom is not used for the main block here to be
// able to use 'break' and 'continue' as 'actions'.

#define QTC_ASSERT(cond, action) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QTC_ASSERT_STRING(#cond); \
        action; \
    } \
    do { \
    } while (0)
#define QTC_CHECK(cond) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QTC_ASSERT_STRING(#cond); \
    } \
    do { \
    } while (0)
#define QTC_GUARD(cond) ((Q_LIKELY(cond)) ? true : (QTC_ASSERT_STRING(#cond), false))
#define QTC_UNEXPECTED(cond) ((Q_UNLIKELY(cond)) ? (QTC_ASSERT_STRING(#cond), true) : false)
