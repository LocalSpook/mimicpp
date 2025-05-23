//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef MIMICPP_ADAPTERS_CATCH2_HPP
#define MIMICPP_ADAPTERS_CATCH2_HPP

#pragma once

#if __has_include("mimic++/Reporting.hpp")
    #include "mimic++/Reporting.hpp"
    #include "mimic++/utilities/C++23Backports.hpp"
#elif not defined(MIMICPP_VERSION)
    #error "It appears that the test-adapter is not included in the mimic++ project or package." \
        "If you plan to use it alongside the mimic++-amalgamated header, please ensure to include the adapter-header afterwards."
#endif

#if __has_include(<catch2/catch_test_macros.hpp>)
    #include <catch2/catch_test_macros.hpp>
#else
    #error "Unable to find catch2 includes."
#endif

namespace mimicpp::reporting::detail::catch2
{
    [[noreturn]]
    inline void send_fail(StringViewT const msg)
    {
#ifdef CATCH_CONFIG_PREFIX_ALL
        CATCH_FAIL(msg);
#else
        FAIL(msg);
#endif

        util::unreachable();
    }

    inline void send_success(StringViewT const msg)
    {
#ifdef CATCH_CONFIG_PREFIX_ALL
        CATCH_SUCCEED(msg);
#else
        SUCCEED(msg);
#endif
    }

    inline void send_warning(StringViewT const msg)
    {
#ifdef CATCH_CONFIG_PREFIX_MESSAGES
        CATCH_WARN(msg);
#else
        WARN(msg);
#endif
    }
}

namespace mimicpp::reporting
{
    // GCOVR_EXCL_START

    /**
     * \brief Reporter for the integration into Catch2.
     * \ingroup REPORTING_ADAPTERS
     * \details This reporter enables the integration of ``mimic++`` into ``Catch2`` and prefixes the headers
     * of ``Catch2`` with ``catch2/``.
     *
     * This reporter installs itself by simply including this header file into any source file of the test executable.
     *
     * ## Experimental Matcher integration
     *
     * ``mimic++`` has an opt-in config, which lets users directly use ``catch2``-matchers everywhere, where a ``mimic++``-matcher
     * would be suitable.
     * \see \ref MIMICPP_CONFIG_EXPERIMENTAL_CATCH2_MATCHER_INTEGRATION
     */
    using Catch2ReporterT = BasicReporter<
        &detail::catch2::send_success,
        &detail::catch2::send_warning,
        &detail::catch2::send_fail>;

    // GCOVR_EXCL_STOP
}

namespace mimicpp::reporting::detail::catch2
{
    [[maybe_unused]]
    inline const ReporterInstaller<Catch2ReporterT> installer{};
}

#ifdef MIMICPP_CONFIG_EXPERIMENTAL_CATCH2_MATCHER_INTEGRATION

#include <catch2/matchers/catch_matchers_templated.hpp>

template <typename Matcher>
    requires std::derived_from<Matcher, Catch::Matchers::MatcherGenericBase> // new style
          || std::derived_from<Matcher, Catch::Matchers::MatcherUntypedBase> // old style
struct mimicpp::custom::matcher_traits<Matcher>
{
    template <typename T>
    [[nodiscard]]
    static constexpr bool matches(const Matcher& matcher, T& value)
        requires requires { { matcher.match(value) } -> std::convertible_to<bool>; }
    {
        return matcher.match(value);
    }
};

#endif

#endif
