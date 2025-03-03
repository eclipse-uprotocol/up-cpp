// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <up-cpp/utils/CallbackConnection.h>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>

namespace {

class CallbackTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	CallbackTest() = default;
	~CallbackTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

/// It should be possible to establish a connection without an exception
/// being thrown. Exceptions that can be thrown at this stage would be a result
/// of a system-level failure, such as running out of memory.
TEST_F(CallbackTest, EstablishDoesNotThrow) {
	using namespace uprotocol::utils;

	EXPECT_NO_THROW(callbacks::Connection<void>::establish([]() {}));
}

/// It should be possible to establish a connection and call the callback
/// function via the CallerHandle (aka the callable).
TEST_F(CallbackTest, EstablishLinkedPair) {
	using namespace uprotocol::utils;

	int call_count{0};

	auto [handle, callable] = callbacks::Connection<void>::establish(
	    [&call_count]() { ++call_count; });

	EXPECT_TRUE(handle);
	EXPECT_TRUE(callable);
	EXPECT_EQ(call_count, 0);

	// It's the first time we're calling the callback - check that it doesn't
	// throw to stop the test here if something is wrong.
	EXPECT_NO_THROW(callable());

	EXPECT_TRUE(handle);
	EXPECT_TRUE(callable);
	EXPECT_EQ(call_count, 1);

	callable();
	callable();

	EXPECT_TRUE(handle);
	EXPECT_TRUE(callable);
	EXPECT_EQ(call_count, 3);
}

// When dropping a CalleeHandle or the last CallerHandle, the connection should
// be broken.
TEST_F(CallbackTest, DroppedHandlesBreakConnection) {
	int call_count{0};

	// Utility to produce connected pairs that have been validated as connected
	auto getPair = ([&call_count]() {
		using namespace uprotocol::utils;

		auto connected_pair = callbacks::Connection<void>::establish(
		    [&call_count]() { ++call_count; });

		// Handle
		EXPECT_TRUE(std::get<0>(connected_pair));
		// Callable
		EXPECT_TRUE(std::get<1>(connected_pair));

		return connected_pair;
	});

	// Drop the handle end via reset()
	{
		auto [handle, callable] = getPair();
		handle.reset();
		EXPECT_FALSE(handle);
		EXPECT_FALSE(callable);
		callable();
		EXPECT_EQ(call_count, 0);
	}

	// Drop the callable end via reset()
	{
		auto [handle, callable] = getPair();
		callable.reset();
		EXPECT_FALSE(handle);
		EXPECT_FALSE(callable);
	}

	// Drop the handle end by letting it go out of scope
	{
		using namespace uprotocol::utils;
		callbacks::Connection<void>::Callable callable_outside;
		static_assert(std::is_move_assignable_v<decltype(callable_outside)>);
		EXPECT_FALSE(callable_outside);
		{
			auto [handle, callable] = getPair();
			callable_outside = std::move(callable);
			EXPECT_FALSE(callable);
			EXPECT_TRUE(callable_outside);
			// Handle dropped here
		}
		EXPECT_FALSE(callable_outside);
		// Calling the callback after disconnect from the callee end should not
		// result in an exception.
		EXPECT_NO_THROW(callable_outside());
		EXPECT_EQ(call_count, 0);
	}

	// Drop the callable end by letting it go out of scope
	{
		using namespace uprotocol::utils;
		callbacks::Connection<void>::Handle handle_outside;
		static_assert(std::is_move_assignable_v<decltype(handle_outside)>);
		EXPECT_FALSE(handle_outside);
		{
			auto [handle, callable] = getPair();
			handle_outside = std::move(handle);
			EXPECT_FALSE(handle);
			EXPECT_TRUE(handle_outside);
			// Callable dropped here
		}
		EXPECT_FALSE(handle_outside);
	}

	// If multiple copies of the callable end are held, dropping one doesn't
	// break the connection.
	{
		auto [handle, callable] = getPair();
		auto other_callable = callable;
		EXPECT_TRUE(other_callable);
		callable();
		callable.reset();
		EXPECT_FALSE(callable);
		EXPECT_TRUE(handle);
		EXPECT_TRUE(other_callable);
		other_callable();
		EXPECT_EQ(call_count, 2);
	}
}

// CallerHandles cannot be used when default constructed or after reset is
// called. Doing so will result in an exception being thrown.
TEST_F(CallbackTest, CallerHandleThrowsBadCall) {
	using namespace uprotocol::utils;

	// Default constructed CallerHandle cannot be called
	{
		callbacks::CallerHandle<void> callable;
		EXPECT_THROW(callable(), callbacks::BadCallerAccess);
	}

	// Freshly reset CallerHandle cannot be called
	{
		auto [handle, callable] =
		    callbacks::Connection<void>::establish([]() {});
		callable.reset();
		EXPECT_THROW(callable(), callbacks::BadCallerAccess);
	}
}

// This connection system will be used for multiple connected callbacks
// simultaneously. As such, it should be possible to have multiple handle pairs
// in use and not have any unexpected interactions between them.
TEST_F(CallbackTest, MultipleConnectionsCanCoexist) {
	using namespace uprotocol::utils;

	std::array<int, 3> call_count{0};

	auto [handle_a, callable_a] = callbacks::Connection<void>::establish(
	    [&call_count]() { ++call_count[0]; });
	auto [handle_b, callable_b] = callbacks::Connection<void>::establish(
	    [&call_count]() { ++call_count[1]; });
	auto [handle_c, callable_c] = callbacks::Connection<void>::establish(
	    [&call_count]() { ++call_count[2]; });

	///////////////////////////////////////////////////////////////////////
	// Check that the connections are independent for calls to the callback
	// function
	EXPECT_EQ(call_count[0], 0);
	EXPECT_EQ(call_count[1], 0);
	EXPECT_EQ(call_count[2], 0);

	callable_a();
	callable_b();
	callable_a();
	callable_b();
	callable_c();
	callable_b();
	callable_b();

	EXPECT_EQ(call_count[0], 2);
	EXPECT_EQ(call_count[1], 4);
	EXPECT_EQ(call_count[2], 1);

	/////////////////////////////////////////////////////////////////////////
	// Check that disconnecting from one connection doesn't affect the others
	EXPECT_TRUE(handle_a);
	EXPECT_TRUE(handle_b);
	EXPECT_TRUE(handle_c);

	handle_a.reset();

	EXPECT_FALSE(handle_a);
	EXPECT_TRUE(handle_b);
	EXPECT_TRUE(handle_c);

	handle_b.reset();

	EXPECT_FALSE(handle_a);
	EXPECT_FALSE(handle_b);
	EXPECT_TRUE(handle_c);

	handle_c.reset();

	EXPECT_FALSE(handle_a);
	EXPECT_FALSE(handle_b);
	EXPECT_FALSE(handle_c);
}

// Cleanup functions should be called when the connection is broken from the
// callee end of the connection.
TEST_F(CallbackTest, CleanupCalledWhenCalleeHandleDropped) {
	using namespace uprotocol::utils;

	int cleanup_count{0};

	auto [handle, callable] = callbacks::Connection<void>::establish(
	    []() {}, [&cleanup_count](auto c) { ++cleanup_count; });

	EXPECT_EQ(cleanup_count, 0);
	handle.reset();
	EXPECT_EQ(cleanup_count, 1);
}

// Cleanup functions should not be called when the connection is broken from
// the caller end of the connection.
TEST_F(CallbackTest, CleanupNotCalledWhenCallerHandleDropped) {
	using namespace uprotocol::utils;

	{
		int cleanup_count{0};

		auto [handle, callable] = callbacks::Connection<void>::establish(
		    []() {}, [&cleanup_count](auto c) { ++cleanup_count; });

		EXPECT_EQ(cleanup_count, 0);
		callable.reset();
		EXPECT_EQ(cleanup_count, 0);
	}

	{
		int cleanup_count{0};

		auto [handle, callable] = callbacks::Connection<void>::establish(
		    []() {}, [&cleanup_count](auto c) { ++cleanup_count; });

		auto callable_copy = callable;

		EXPECT_EQ(cleanup_count, 0);
		callable.reset();
		EXPECT_EQ(cleanup_count, 0);
		callable_copy.reset();
		EXPECT_EQ(cleanup_count, 0);
	}
}

// It is very likely that connections will be held in some sort of map. In
// order to effectively make use of the cleanup function, it should be possible
// to use the CallerHandle as a reverse-lookup key.
TEST_F(CallbackTest, CleanupParameterCanLookUpCallable) {
	using namespace uprotocol::utils;

	std::map<callbacks::Connection<void>::Callable, int> cleanup_count;

	auto [handle_a, callable_a] = callbacks::Connection<void>::establish(
	    []() {}, [&cleanup_count](auto c) { ++cleanup_count[c]; });
	auto [handle_b, callable_b] = callbacks::Connection<void>::establish(
	    []() {}, [&cleanup_count](auto c) { ++cleanup_count[c]; });
	auto [handle_c, callable_c] = callbacks::Connection<void>::establish(
	    []() {}, [&cleanup_count](auto c) { ++cleanup_count[c]; });

	EXPECT_EQ(cleanup_count[callable_a], 0);
	EXPECT_EQ(cleanup_count[callable_b], 0);
	EXPECT_EQ(cleanup_count[callable_c], 0);

	handle_c.reset();
	EXPECT_EQ(cleanup_count[callable_a], 0);
	EXPECT_EQ(cleanup_count[callable_b], 0);
	EXPECT_EQ(cleanup_count[callable_c], 1);

	handle_a.reset();
	EXPECT_EQ(cleanup_count[callable_a], 1);
	EXPECT_EQ(cleanup_count[callable_b], 0);
	EXPECT_EQ(cleanup_count[callable_c], 1);

	handle_b.reset();
	EXPECT_EQ(cleanup_count[callable_a], 1);
	EXPECT_EQ(cleanup_count[callable_b], 1);
	EXPECT_EQ(cleanup_count[callable_c], 1);
}

// Until this point, none of the tests have examined parameters to the callback
// functions. However, this should be supported by the connection system. We
// can verify the parameters are passed through by checking for the result of
// known operations.
TEST_F(CallbackTest, CallablesCanTakeArguments) {
	using namespace uprotocol::utils;

	// Add a couple of numbers together, check the result
	{
		int sum{0};

		auto [handle, callable] =
		    callbacks::Connection<void, int, int>::establish(
		        [&sum](int x, int y) { sum = x + y; });

		callable(5, 9);
		EXPECT_EQ(sum, 14);
		callable(-80, 79);
		EXPECT_EQ(sum, -1);
	}

	// Pass a string in, report the length of that string
	{
		size_t len{0};

		auto [handle, callable] =
		    callbacks::Connection<void, const std::string&>::establish(
		        [&len](const std::string& s) { len = s.size(); });

		callable("Hello, world");
		EXPECT_EQ(len, 12);
		callable("123456789012345678901234567");
		EXPECT_EQ(len, 27);
	}
}

// Until this point, the callback has not returned a value. This should be
// supported by the connection system. Building on passing parameters, we can
// return the result of an operation performed by the callback function.
TEST_F(CallbackTest, CallablesCanReturnValues) {
	using namespace uprotocol::utils;

	// Multiply two numbers together, check the result
	{
		auto [handle, callable] =
		    callbacks::Connection<int, int, int>::establish(
		        [](int x, int y) { return x * y; });

		// Note: using .value_or() to avoid exceptions by passing a value that
		// would definitely fail when compared to the expected value.
		EXPECT_EQ(callable(6, 7).value_or(0), 42);
		EXPECT_EQ(callable(-3, 11).value_or(0), -33);
	}

	// Prepend a known string to the front of a passed parameter
	{
		auto [handle, callable] =
		    callbacks::Connection<std::string, const std::string_view>::
		        establish([](const std::string_view who) {
			        std::string message("Hello, ");
			        message += who;
			        return message;
		        });

		// Note: using .value_or() to avoid exceptions by passing a value that
		// would definitely fail when compared to the expected value.
		EXPECT_EQ(callable("Alice").value_or("FAIL"), "Hello, Alice");
		EXPECT_EQ(callable("BOB").value_or("FAIL"), "Hello, BOB");
	}
}

// Values returned by the callback should be moved to the caller instead of
// copying. The callback does not need to do anything special to achieve this.
// However, it is possible for the code within the connection system to
// inadvertently introduce a copy. We can detect this by a) using a non-
// copyable type as the return and b) checking container objects for changes
// in their data pointers.
TEST_F(CallbackTest, ReturnValuesAreMoved) {
	using namespace uprotocol::utils;

	// Checking with a non-copyable object (in this case, std::unique_ptr)
	{
		void* original_location{nullptr};

		auto [handle, callable] =
		    callbacks::Connection<std::unique_ptr<int>>::establish(
		        [&original_location]() {
			        auto p = std::make_unique<int>(71);
			        original_location = p.get();
			        return p;
		        });

		auto result = callable();
		EXPECT_TRUE(result);
		EXPECT_EQ(original_location, (*result).get());
	}

	// Checking with a container object (in this case, std::string)
	{
		void* original_string_location{nullptr};
		constexpr size_t expected_capacity = 9000;
		std::string a_copy;

		auto [handle, callable] =
		    callbacks::Connection<std::string>::establish([&]() {
			    std::string s("This string must be long enough to avoid SSO");
			    // Changing the capacity provides a canary we can check
			    // to see if a copy or a move has occurred. Copies will
			    // generally only reserve what is needed to hold the
			    // current content of the original string.
			    s.reserve(expected_capacity);
			    original_string_location = s.data();
			    a_copy = s;
			    return s;
		    });

		auto result = callable();
		EXPECT_TRUE(result);
		EXPECT_EQ(original_string_location, (*result).data());
		EXPECT_EQ(expected_capacity, (*result).capacity());
		// Just to be safe, check our assumptions about copies vs moves. The
		// a_copy variable should hold a copy of the original string, this time
		// with a different pointer and capacity.
		EXPECT_NE(original_string_location, a_copy.data());
		EXPECT_NE(expected_capacity, a_copy.capacity());
	}
}

// When the conection is broken from the callee end, the caller handle will
// still exist. It is safe to call - nothing will happen. However, when the
// callback is a returning callback, an empty optional will be returned to
// indicate that the connection is not active.
TEST_F(CallbackTest, DisconnectedCallablesReturnNothing) {
	using namespace uprotocol::utils;

	{
		auto [handle, callable] =
		    callbacks::Connection<float>::establish([]() { return 1.0f; });

		static_assert(
		    std::is_same_v<std::optional<float>, decltype(callable())>);

		EXPECT_TRUE(callable);
		EXPECT_TRUE(callable());
		handle.reset();
		EXPECT_FALSE(callable);
		EXPECT_FALSE(callable());
	}

	// Just to reassure ourselves that it's still possible to return a
	// std::optional from a callback.
	{
		auto [handle, callable] =
		    callbacks::Connection<std::optional<int>>::establish(
		        []() { return 1; });

		static_assert(std::is_same_v<std::optional<std::optional<int>>,
		                             decltype(callable())>);

		EXPECT_TRUE(callable);
		EXPECT_TRUE(callable());
		handle.reset();
		EXPECT_FALSE(callable);
		EXPECT_FALSE(callable());
	}
}

// The typical use case for these callbacks is to pass asynchronous events.
// It should work with the caller executing from a separate context.
TEST_F(CallbackTest, CanCallFromAnotherThread) {
	using namespace uprotocol::utils;

	std::atomic<int> call_count{0};

	auto [handle, callable] = callbacks::Connection<void>::establish(
	    [&call_count]() { ++call_count; });

	constexpr int expected = 21;

	// Note: C++20 could use jthread instead of std::thread
	std::thread other([c = std::move(callable)]() mutable {
		for (int i = 0; i < expected; ++i) {
			c();
		}
	});

	EXPECT_TRUE(handle);
	EXPECT_FALSE(callable);
	other.join();
	EXPECT_FALSE(handle);
	EXPECT_EQ(call_count, expected);
}

////////////////////////////////////////////////////////////////////////////
// NOTE: Test was originally written with C++20 semaphores.
// This is a good enough approximation to get the test working without C++20
struct SemaphoreLike {
	void release() {
		{
			std::lock_guard l(cv_m_);
			++ctr_;
		}
		cv_.notify_one();
	}

	template <typename Rep, typename Period>
	bool try_acquire_for(const std::chrono::duration<Rep, Period>& t) {
		std::unique_lock l(cv_m_);
		if (ctr_ > 0) {
			--ctr_;
			return true;
		} else {
			return cv_.wait_for(l, t, [this]() {
				if (ctr_ > 0) {
					--ctr_;
					return true;
				}
				return false;
			});
		}
	}

private:
	size_t ctr_{0};
	std::mutex cv_m_;
	std::condition_variable cv_;
};
////////////////////////////////////////////////////

// In the scenario where callbacks are executing at the moment where the callee
// tries to disconnect, we expect that disconnecting operation to block until
// the callbacks have completed. This test sets up the scenario using multiple
// threads all blocking to acquire resources (via semaphore-like object). These
// blocks are released one-by-one to check that the connection states remain
// valid and the callee remains blocked throughout the process.
TEST_F(CallbackTest, HandleResetBlocksWhileCallbacksRunning) {
	using namespace uprotocol::utils;
	using namespace std::chrono_literals;

	std::atomic<bool> disconnect_done{false};
	std::atomic<int> callbacks_pending{0};
	std::atomic<int> callbacks_released{0};

	// NOTE: This can be replaced with std::counting_semaphore in C++20.
	// This blocks the caller threads until the main test releases them
	SemaphoreLike fake_blocking_op;
	// NOTE: This can be replaced with std::binary_semaphore in C++20.
	// This blocks the main test thread until some expected synchronization
	// event has occurred. For example, the main test may need to wait until
	// a thread has started.
	SemaphoreLike main_task_sync;
	// NOTE: This can be replaced with std::counting_semaphore in C++20.
	// This blocks the callee thread until the main test releases it. Only used
	// for startup synchronization.
	SemaphoreLike callee_sync;

	auto [handle, callable] = callbacks::Connection<bool>::establish(
	    [&fake_blocking_op]() { return fake_blocking_op.try_acquire_for(1s); });

	auto caller_fn = ([callable, &callbacks_pending, &callbacks_released,
	                   &main_task_sync]() mutable {
		++callbacks_pending;
		main_task_sync.release();
		auto did_not_expire = callable();
		--callbacks_pending;
		if (did_not_expire) {
			++callbacks_released;
		}
		main_task_sync.release();
	});

	// Start one of the callers and wait for it to signal us that it is ready
	std::thread caller_a(caller_fn);
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));

	// Start the other caller and wait for it to signal us that it is ready
	std::thread caller_b(caller_fn);
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));

	// Launch a thread for the callee. It will attempt to reset the connection,
	// then block because the two callers have executing callbacks.
	std::thread callee([handle = std::move(handle), &main_task_sync,
	                    &disconnect_done, &callee_sync]() mutable {
		// Wait for the main test to tell us its ready to start
		if (!callee_sync.try_acquire_for(1s)) {
			return;
		}
		// Tell the main test task that we are running now
		main_task_sync.release();
		// This is expected to block until all the fake blocking operations
		// are released
		handle.reset();
		disconnect_done = true;
		main_task_sync.release();
	});

	EXPECT_TRUE(callable);
	// avoid a race condition checking the validity of the callable above
	callee_sync.release();
	// Wait until the callee thread has started, at which point it will have
	// called reset() on its handle.
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));
	// The CallerHandle immediatly reports the connection has been broken to
	// prevent additional calls from starting.
	EXPECT_FALSE(callable);

	// Validate the state: two callers are blocked and the callee has not
	// completed the disconnect operation.
	EXPECT_FALSE(disconnect_done);
	EXPECT_EQ(callbacks_pending, 2);
	EXPECT_EQ(callbacks_released, 0);

	// Advance the state by releasing one of the blocked callbacks, then wait
	// for the newly unblocked caller thread to signal that it has exited.
	fake_blocking_op.release();
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));

	// Validate the state: one caller is blocked, one caller has returned, and
	// the callee has not completed the disconnect operation.
	EXPECT_FALSE(disconnect_done);
	EXPECT_EQ(callbacks_pending, 1);
	EXPECT_EQ(callbacks_released, 1);

	// Advance the state by releasing one of the blocked callbacks, then wait
	// for the newly unblocked caller thread to signal that it has exited.
	fake_blocking_op.release();
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));

	// This also immediately unblocks the waiting callee since all active
	// callbacks have now returned. Wait for it to signal that it is done to
	// avoid a race condition checking the results.
	EXPECT_TRUE(main_task_sync.try_acquire_for(1s));

	// Validate the state: no callers are blocked, two callers have returned,
	// and the callee has completed the disconnect operation.
	EXPECT_TRUE(disconnect_done);
	EXPECT_EQ(callbacks_pending, 0);
	EXPECT_EQ(callbacks_released, 2);

	// Note: C++20 could use jthread instead of std::thread
	caller_a.join();
	caller_b.join();
	callee.join();
}

// Sometimes there might be a reason for a class or function to default
// construct a CallerHandle then initialize it later. Check this works, that
// the default-constructed object reports as disconnected, and that no
// exception is thrown.
TEST_F(CallbackTest, CallerHandleCanDefaultConstruct) {
	using namespace uprotocol::utils;

	EXPECT_NO_THROW({
		callbacks::CallerHandle<void> x;
		EXPECT_FALSE(x);
	});
}

// Sometimes there might be a reason for a class or function to default
// construct a CalleeHandle then initialize it later. Check this works, that
// the default-constructed object reports as disconnected, and that no
// exception is thrown.
TEST_F(CallbackTest, CalleeHandleCanDefaultConstruct) {
	using namespace uprotocol::utils;

	EXPECT_NO_THROW({
		callbacks::CalleeHandle<void> x;
		EXPECT_FALSE(x);
	});
}

///////////////////////////////////////////////////////////////////////////////
// It is possible to create std::function objects with no target function. When
// they are invoked, they throw std::bad_function_call. This is not desireable,
// so the callback connections modules are required to check the validity of
// function objects they receive

// Tests invalid callback function objects
TEST_F(CallbackTest, EstablishWithNonCallableCallback) {
	using namespace uprotocol::utils;

	callbacks::Connection<bool>::ConnectedPair conn;

	EXPECT_THROW(conn = callbacks::Connection<bool>::establish({}),
	             callbacks::EmptyFunctionObject);

	auto& [handle, callable] = conn;

	// Ordering is important here. If handle.reset() tries blindly to call the
	// cleanup callback, the exception could be thrown before the connection
	// is broken. When that happens, the destructor will try to reset again.
	// By resetting the callable second, there is no need to try the cleanup
	// funciton again, so the destructor won't throw.
	EXPECT_NO_THROW(handle.reset());
	EXPECT_NO_THROW(callable.reset());
}

// Tests invalid cleanup function objects
TEST_F(CallbackTest, EstablishWithNonCallableCleanup) {
	using namespace uprotocol::utils;

	auto cb = []() -> bool { return true; };
	callbacks::Connection<bool>::Cleanup empty;
	callbacks::Connection<bool>::ConnectedPair conn;

	EXPECT_THROW(conn = callbacks::Connection<bool>::establish(cb, empty),
	             callbacks::EmptyFunctionObject);

	auto& [handle, callable] = conn;

	// Ordering is important here. If handle.reset() tries blindly to call the
	// cleanup callback, the exception could be thrown before the connection
	// is broken. When that happens, the destructor will try to reset again.
	// By resetting the callable second, there is no need to try the cleanup
	// funciton again, so the destructor won't throw.
	EXPECT_NO_THROW(handle.reset());
	EXPECT_NO_THROW(callable.reset());
}

// Tests both invalid cleanup and invalid callback function objects
TEST_F(CallbackTest, EstablishWithNonCallableCallbackAndCleanup) {
	using namespace uprotocol::utils;

	callbacks::Connection<bool>::Cleanup empty;
	callbacks::Connection<bool>::ConnectedPair conn;

	EXPECT_THROW(conn = callbacks::Connection<bool>::establish({}, empty),
	             callbacks::EmptyFunctionObject);

	auto& [handle, callable] = conn;

	// Ordering is important here. If handle.reset() tries blindly to call the
	// cleanup callback, the exception could be thrown before the connection
	// is broken. When that happens, the destructor will try to reset again.
	// By resetting the callable second, there is no need to try the cleanup
	// funciton again, so the destructor won't throw.
	EXPECT_NO_THROW(handle.reset());
	EXPECT_NO_THROW(callable.reset());
}

}  // namespace
