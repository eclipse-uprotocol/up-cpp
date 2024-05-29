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

#ifndef UP_CPP_TEST_UTRANSPORTMOCK_H
#define UP_CPP_TEST_UTRANSPORTMOCK_H

#include <gmock/gmock.h>
#include <up-cpp/transport/UTransport.h>

namespace uprotocol::test {

struct UTransportMockInfo {
	bool init_passed;
	bool deinit_passed;
	size_t send_count;
	uprotocol::v1::UStatus send_status;
	std::optional<uprotocol::utils::callbacks::CallerHandle<
	    void, uprotocol::v1::UMessage const&>>
	    listener;
	std::optional<uprotocol::utils::callbacks::CallerHandle<
	    void, uprotocol::v1::UMessage const&>>
	    cleanupListener;
	std::optional<uprotocol::v1::UUri> source_filter;
	v1::UUri sink_filter;
	v1::UMessage message;

	void reset() {
		init_passed = false;
		deinit_passed = false;
		send_count = 0;
		listener.reset();
		cleanupListener.reset();
	}

	UTransportMockInfo() { reset(); }

	void mock_message(const uprotocol::v1::UMessage& msg) {
		using namespace std;
		ASSERT_TRUE(listener &&
		            "registerListener must be set before calling mock_packet");
		(*listener)(msg);
	}
};

class UTransportMock : public uprotocol::transport::UTransport {
public:
	static UTransportMockInfo mock_info;

	explicit UTransportMock(const v1::UUri& uuri)
	    : uprotocol::transport::UTransport(uuri) {
		mock_info.init_passed = true;
	}

	~UTransportMock() { mock_info.deinit_passed = true; }

private:
	[[nodiscard]] v1::UStatus sendImpl(const v1::UMessage& message) override {
		v1::UStatus retval;
		mock_info.message = message;
		mock_info.send_count++;
		return mock_info.send_status;
	}

	[[nodiscard]] v1::UStatus registerListenerImpl(
	    const v1::UUri& sink_filter, CallableConn&& listener,
	    std::optional<v1::UUri>&& source_filter) override {
		mock_info.listener = listener;
		mock_info.source_filter = source_filter;
		mock_info.sink_filter = sink_filter;
		v1::UStatus retval;
		retval.set_code(v1::UCode::OK);
		return retval;
	}

	void cleanupListener(CallableConn listener) override {
		mock_info.cleanupListener = listener;
	}
};

};  // namespace uprotocol::test

#endif  // UP_CPP_TEST_UTRANSPORTMOCK_H
