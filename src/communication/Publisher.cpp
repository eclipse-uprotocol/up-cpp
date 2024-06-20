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

#include "up-cpp/communication/Publisher.h"
#include <up-cpp/datamodel/builder/UMessage.h>

namespace uprotocol::communication {
using namespace uprotocol::datamodel::builder;

Publisher::Publisher(std::shared_ptr<transport::UTransport> transport,
          v1::UUri& topic, v1::UPayloadFormat format,
          std::optional<v1::UPriority> priority,
          std::optional<std::chrono::milliseconds> ttl) :
    transport_(std::move(transport)) {

    auto messageBuilder = UMessageBuilder::publish(std::move(topic))
        .withPayloadFormat(format)
        .withPriority(priority.value_or(v1::UPriority::UPRIORITY_CS1))
        .withTtl(ttl.value());

    publish_builder_ = std::make_shared<UMessageBuilder>(std::move(messageBuilder));

}

v1::UStatus Publisher::publish(datamodel::builder::Payload&& payload) const {
    auto message = publish_builder_->build(std::move(payload));
    return transport_->send(std::move(message));
}




} // namespace uprotocol::communication