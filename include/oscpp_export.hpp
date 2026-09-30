// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// OSCPP_API marks the classes and functions that are part of the public interface of the shared library. The library
// is built with hidden visibility, so anything without this macro is not visible outside it. Shared by the oscpp and
// oscpp_exceptionless namespaces.

#ifndef OSCPP_EXPORT_HPP
#define OSCPP_EXPORT_HPP

#if defined(_WIN32) || defined(__CYGWIN__)
// The library's own build defines OSCPP_EXPORTS; everyone else imports. (Windows is not built or tested yet.)
#ifdef OSCPP_EXPORTS
#define OSCPP_API __declspec(dllexport)
#else
#define OSCPP_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define OSCPP_API __attribute__((visibility("default")))
#else
#define OSCPP_API
#endif

#endif // OSCPP_EXPORT_HPP
