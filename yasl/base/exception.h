// Copyright 2022 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <array>
#include <exception>

#include "absl/debugging/stacktrace.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_join.h"
#include "absl/types/span.h"
#include "fmt/format.h"

template <>
struct fmt::formatter<absl::Span<const int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(absl::Span<const int64_t> number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", absl::StrJoin(number, "x"));
  }
};

template <>
struct fmt::formatter<std::vector<int64_t>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::vector<int64_t>& number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", absl::StrJoin(number, "x"));
  }
};

namespace yasl {
namespace internal {

const int kMaxStackTraceDep = 16;

template <typename... Args>
inline std::string Format(Args&&... args) {
  return fmt::format(std::forward<Args>(args)...);
}

// Trick to make Format works with empty arguments.
template <>
inline std::string Format() {
  return "";
}

}  // namespace internal

// NOTE: Currently we are using STL exception tree.
//   |- exception
//       |- logic_error
//       |- runtime_error
//           |- io_error

class Exception : public std::exception {
 public:
  Exception() = default;
  explicit Exception(std::string msg) : msg_(std::move(msg)) {}
  explicit Exception(std::string&& msg) : msg_(std::move(msg)) {}
  explicit Exception(const char* msg) : msg_(msg) {}
  explicit Exception(std::string msg, void** stacks, int dep)
      : msg_(std::move(msg)) {
    for (int i = 0; i < dep; ++i) {
      std::array<char, 1024> tmp;
      const char* symbol = "(unknown)";
      if (absl::Symbolize(stacks[i], tmp.data(), sizeof(tmp))) {
        symbol = tmp.data();
      }
      stack_trace_.append(fmt::format("#{} {}+{}\n", i, symbol, stacks[i]));
    }
  }
  const char* what() const noexcept override { return msg_.c_str(); }

  const std::string& stack_trace() const noexcept { return stack_trace_; }

 private:
  std::string msg_;
  std::string stack_trace_;
};

class RuntimeError : public Exception {
  using Exception::Exception;
};

class LogicError : public Exception {
  using Exception::Exception;
};

class NotImplementedError : public Exception {
  using Exception::Exception;
};

class IoError : public RuntimeError {
  using RuntimeError::RuntimeError;
};

class ArgumentError : public RuntimeError {
  using RuntimeError::RuntimeError;
};

class InvalidFormat : public IoError {
  using IoError::IoError;
};

class NetworkError : public IoError {
  using IoError::IoError;
};

#define YASL_ERROR_MSG(...) \
  fmt::format("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

using stacktrace_t = std::array<void*, ::yasl::internal::kMaxStackTraceDep>;
//
// add absl::InitializeSymbolizer to main function to get
// human-readable names stack trace
//
// Example:
// int main(int argc, char *argv[]) {
//   absl::InitializeSymbolizer(argv[0]);
//   ...
// }
//
#define YASL_THROW(...)                                                    \
  do {                                                                     \
    ::yasl::stacktrace_t stacks;                                           \
    int dep = absl::GetStackTrace(stacks.data(),                           \
                                  ::yasl::internal::kMaxStackTraceDep, 0); \
    throw ::yasl::RuntimeError(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), \
                               dep);                                       \
  } while (false)

#define YASL_THROW_LOGIC_ERROR(...)                                            \
  do {                                                                         \
    ::yasl::stacktrace_t stacks;                                               \
    int dep = absl::GetStackTrace(stacks.data(),                               \
                                  ::yasl::internal::kMaxStackTraceDep, 0);     \
    throw ::yasl::LogicError(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), dep); \
  } while (false)

#define YASL_THROW_IO_ERROR(...)                                            \
  do {                                                                      \
    ::yasl::stacktrace_t stacks;                                            \
    int dep = absl::GetStackTrace(stacks.data(),                            \
                                  ::yasl::internal::kMaxStackTraceDep, 0);  \
    throw ::yasl::IoError(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), dep); \
  } while (false)

#define YASL_THROW_NETWORK_ERROR(...)                                      \
  do {                                                                     \
    ::yasl::stacktrace_t stacks;                                           \
    int dep = absl::GetStackTrace(stacks.data(),                           \
                                  ::yasl::internal::kMaxStackTraceDep, 0); \
    throw ::yasl::NetworkError(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), \
                               dep);                                       \
  } while (false)

#define YASL_THROW_INVALID_FORMAT(...)                                      \
  do {                                                                      \
    ::yasl::stacktrace_t stacks;                                            \
    int dep = absl::GetStackTrace(stacks.data(),                            \
                                  ::yasl::internal::kMaxStackTraceDep, 0);  \
    throw ::yasl::InvalidFormat(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), \
                                dep);                                       \
  } while (false)

#define YASL_THROW_ARGUMENT_ERROR(...)                                      \
  do {                                                                      \
    ::yasl::stacktrace_t stacks;                                            \
    int dep = absl::GetStackTrace(stacks.data(),                            \
                                  ::yasl::internal::kMaxStackTraceDep, 0);  \
    throw ::yasl::ArgumentError(YASL_ERROR_MSG(__VA_ARGS__), stacks.data(), \
                                dep);                                       \
  } while (false)

// For Status.
#define CHECK_OR_THROW(statement) \
  do {                            \
    auto __s__ = (statement);     \
    if (!__s__.IsOk()) {          \
      YASL_THROW(__s__.Msg());    \
    }                             \
  } while (false)

// For StatusOr from Asylo.
#define ASSIGN_OR_THROW(lhs, rexpr)      \
  do {                                   \
    auto __s__ = (rexpr);                \
    if (!__s__.IsOk()) {                 \
      YASL_THROW(__s__.status().Msg());  \
    }                                    \
    lhs = std::move(__s__).ValueOrDie(); \
  } while (false)

//------------------------------------------------------
// ENFORCE
// https://github.com/facebookincubator/gloo/blob/master/gloo/common/logging.h

/**
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

class EnforceNotMet : public Exception {
 public:
  EnforceNotMet(const char* file, int line, const char* condition,
                const std::string& msg)
      : full_msg_(fmt::format("[Enforce fail at {}:{}] {}. {}", file, line,
                              condition, msg)) {}
  EnforceNotMet(const char* file, int line, const char* condition,
                const std::string& msg, void** stacks, int dep)
      : Exception(msg, stacks, dep),
        full_msg_(fmt::format("[Enforce fail at {}:{}] {}. {}", file, line,
                              condition, msg)) {}

  const char* what() const noexcept override { return full_msg_.c_str(); }

 private:
  std::string full_msg_;
};

#define YASL_ENFORCE(condition, ...)                                         \
  do {                                                                       \
    if (!(condition)) {                                                      \
      ::yasl::stacktrace_t stacks;                                           \
      int dep = absl::GetStackTrace(stacks.data(),                           \
                                    ::yasl::internal::kMaxStackTraceDep, 0); \
      throw ::yasl::EnforceNotMet(__FILE__, __LINE__, #condition,            \
                                  ::yasl::internal::Format(__VA_ARGS__),     \
                                  stacks.data(), dep);                       \
    }                                                                        \
  } while (false)

/**
 * Rich logging messages
 *
 * YASL_ENFORCE_THAT can be used with one of the "checker functions" that
 * capture input argument values and add it to the exception message. E.g.
 * `YASL_ENFORCE_THAT(Equals(foo(x), bar(y)), "Optional additional message")`
 * would evaluate both foo and bar only once and if the results are not equal -
 * include them in the exception message.
 *
 * Some of the basic checker functions like Equals or Greater are already
 * defined below. Other header might define customized checkers by adding
 * functions to yasl::enforce_detail namespace. For example:
 *
 *   namespace yasl { namespace enforce_detail {
 *   inline EnforceFailMessage IsVector(const vector<TIndex>& shape) {
 *     if (shape.size() == 1) { return EnforceOK(); }
 *     return fmt::format("Shape {} is not a vector", shape);
 *   }
 *   }}
 *
 * With further usages like `YASL_ENFORCE_THAT(IsVector(Input(0).dims()))`
 *
 * Convenient wrappers for binary operations like YASL_ENFORCE_EQ are provided
 * too. Please use them instead of CHECK_EQ and friends for failures in
 * user-provided input.
 */

namespace enforce_detail {

struct EnforceOK {};

class EnforceFailMessage {
 public:
  constexpr /* implicit */ EnforceFailMessage(EnforceOK) : msg_(nullptr) {}

  EnforceFailMessage(EnforceFailMessage&&) = default;
  EnforceFailMessage(const EnforceFailMessage&) = delete;
  EnforceFailMessage& operator=(EnforceFailMessage&&) = delete;
  EnforceFailMessage& operator=(const EnforceFailMessage&) = delete;

  /* implicit */ EnforceFailMessage(std::string&& msg) {
    msg_ = new std::string(std::move(msg));
  }

  ~EnforceFailMessage() { Free(); }

  inline bool Bad() const { return msg_ != nullptr; }

  std::string GetMessageAndFree(std::string&& extra) {
    std::string r;
    if (extra.empty()) {
      r = std::move(*msg_);
    } else {
      r = fmt::format("{}.{}", *msg_, extra);
    }
    Free();
    return r;
  }

 private:
  void Free() {
    delete msg_;
    msg_ = nullptr;
  }

  std::string* msg_;
};

#define BINARY_COMP_HELPER(name, op)                         \
  template <typename T1, typename T2>                        \
  inline EnforceFailMessage name(const T1& x, const T2& y) { \
    if (x op y) {                                            \
      return EnforceOK();                                    \
    }                                                        \
    return fmt::format("{} vs {}", x, y);                    \
  }
BINARY_COMP_HELPER(Equals, ==)
BINARY_COMP_HELPER(NotEquals, !=)
BINARY_COMP_HELPER(Greater, >)
BINARY_COMP_HELPER(GreaterEquals, >=)
BINARY_COMP_HELPER(Less, <)
BINARY_COMP_HELPER(LessEquals, <=)
#undef BINARY_COMP_HELPER

#define YASL_ENFORCE_THAT_IMPL(condition, expr, ...)                   \
  do {                                                                 \
    ::yasl::enforce_detail::EnforceFailMessage r(condition);           \
    if (r.Bad()) {                                                     \
      throw ::yasl::EnforceNotMet(                                     \
          __FILE__, __LINE__, expr,                                    \
          r.GetMessageAndFree(::yasl::internal::Format(__VA_ARGS__))); \
    }                                                                  \
  } while (false)
}  // namespace enforce_detail

#define YASL_ENFORCE_THAT(condition, ...) \
  YASL_ENFORCE_THAT_IMPL((condition), #condition, __VA_ARGS__)

#define YASL_ENFORCE_EQ(x, y, ...)                                 \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::Equals((x), (y)), \
                         #x " == " #y, __VA_ARGS__)
#define YASL_ENFORCE_NE(x, y, ...)                                    \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::NotEquals((x), (y)), \
                         #x " != " #y, __VA_ARGS__)
#define YASL_ENFORCE_LE(x, y, ...)                                     \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::LessEquals((x), (y)), \
                         #x " <= " #y, __VA_ARGS__)
#define YASL_ENFORCE_LT(x, y, ...)                                            \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::Less((x), (y)), #x " < " #y, \
                         __VA_ARGS__)
#define YASL_ENFORCE_GE(x, y, ...)                                        \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::GreaterEquals((x), (y)), \
                         #x " >= " #y, __VA_ARGS__)
#define YASL_ENFORCE_GT(x, y, ...)                                  \
  YASL_ENFORCE_THAT_IMPL(::yasl::enforce_detail::Greater((x), (y)), \
                         #x " > " #y, __VA_ARGS__)

template <typename T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
T CheckNotNull(T t) {
  YASL_ENFORCE(t != nullptr);
  return t;
}

}  // namespace yasl
