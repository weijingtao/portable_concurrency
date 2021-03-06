#pragma once

#include <string>
#include <memory>

#include <gtest/gtest.h>

#include "concurrency/future"

#include "test_tools.h"

namespace {

template<typename T>
class PromiseTest: public ::testing::Test {};
TYPED_TEST_CASE_P(PromiseTest);

namespace tests {

template<typename T>
void get_future_twice() {
  experimental::promise<T> p;
  experimental::future<T> f1, f2;
  EXPECT_NO_THROW(f1 = p.get_future());
  EXPECT_FUTURE_ERROR(
    f2 = p.get_future(),
    std::future_errc::future_already_retrieved
  );
}

template<typename T>
void set_val_on_promise_without_future() {
  experimental::promise<T> p;
  EXPECT_NO_THROW(p.set_value(some_value<T>()));
}

template<>
void set_val_on_promise_without_future<void>(){
  experimental::promise<void> p;
  EXPECT_NO_THROW(p.set_value());
}

template<typename T>
void set_err_on_promise_without_future() {
  experimental::promise<T> p;
  EXPECT_NO_THROW(p.set_exception(std::make_exception_ptr(std::runtime_error("error"))));
}

template<typename T>
void set_value_twice_without_future() {
  experimental::promise<T> p;
  EXPECT_NO_THROW(p.set_value(some_value<T>()));
  EXPECT_FUTURE_ERROR(p.set_value(some_value<T>()), std::future_errc::promise_already_satisfied);
}

template<>
void set_value_twice_without_future<void>() {
  experimental::promise<void> p;
  EXPECT_NO_THROW(p.set_value());
  EXPECT_FUTURE_ERROR(p.set_value(), std::future_errc::promise_already_satisfied);
}

template<typename T>
void set_value_twice_with_future() {
  experimental::promise<T> p;
  auto f = p.get_future();

  EXPECT_NO_THROW(p.set_value(some_value<T>()));
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());

  EXPECT_FUTURE_ERROR(p.set_value(some_value<T>()), std::future_errc::promise_already_satisfied);
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());
}

template<>
void set_value_twice_with_future<void>() {
  experimental::promise<void> p;
  auto f = p.get_future();

  EXPECT_NO_THROW(p.set_value());
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());

  EXPECT_FUTURE_ERROR(p.set_value(), std::future_errc::promise_already_satisfied);
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());
}

template<typename T>
void set_value_twice_after_value_taken() {
  experimental::promise<T> p;
  auto f = p.get_future();

  EXPECT_NO_THROW(p.set_value(some_value<T>()));
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());
  f.get();
  EXPECT_FALSE(f.valid());

  EXPECT_FUTURE_ERROR(p.set_value(some_value<T>()), std::future_errc::promise_already_satisfied);
  EXPECT_FALSE(f.valid());
}

template<>
void set_value_twice_after_value_taken<void>() {
  experimental::promise<void> p;
  auto f = p.get_future();

  EXPECT_NO_THROW(p.set_value());
  EXPECT_TRUE(f.valid());
  EXPECT_TRUE(f.is_ready());
  f.get();
  EXPECT_FALSE(f.valid());

  EXPECT_FUTURE_ERROR(p.set_value(), std::future_errc::promise_already_satisfied);
  EXPECT_FALSE(f.valid());
}

} // namespace tests

TYPED_TEST_P(PromiseTest, get_future_twice) {tests::get_future_twice<TypeParam>();}
TYPED_TEST_P(PromiseTest, set_val_on_promise_without_future) {tests::set_val_on_promise_without_future<TypeParam>();}
TYPED_TEST_P(PromiseTest, set_err_on_promise_without_future) {tests::set_err_on_promise_without_future<TypeParam>();}
TYPED_TEST_P(PromiseTest, set_value_twice_without_future) {tests::set_value_twice_without_future<TypeParam>();}
TYPED_TEST_P(PromiseTest, set_value_twice_with_future) {tests::set_value_twice_with_future<TypeParam>();}
TYPED_TEST_P(PromiseTest, set_value_twice_after_value_taken) {tests::set_value_twice_after_value_taken<TypeParam>();}
REGISTER_TYPED_TEST_CASE_P(
  PromiseTest,
  get_future_twice,
  set_val_on_promise_without_future,
  set_err_on_promise_without_future,
  set_value_twice_without_future,
  set_value_twice_with_future,
  set_value_twice_after_value_taken
);

INSTANTIATE_TYPED_TEST_CASE_P(VoidType, PromiseTest, void);
INSTANTIATE_TYPED_TEST_CASE_P(PrimitiveType, PromiseTest, int);
INSTANTIATE_TYPED_TEST_CASE_P(CopyableType, PromiseTest, std::string);
INSTANTIATE_TYPED_TEST_CASE_P(MoveableType, PromiseTest, std::unique_ptr<int>);
INSTANTIATE_TYPED_TEST_CASE_P(ReferenceType, PromiseTest, future_tests_env&);

} // anonymous namespace
