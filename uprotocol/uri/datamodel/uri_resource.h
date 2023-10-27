/**
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef URI_RESOURCE_H_
#define URI_RESOURCE_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace uri_datamodel {
class uri_resource {
 public:
  uri_resource() : uri_resource("", "", "") {}

  uri_resource(const std::string& name, const std::string& instance,
               const std::string& message)
      : name(name), instance(instance), message(message) {}

  static uri_resource fromName(const std::string& name) {
    return uri_resource{name, "", ""};
  }

  static uri_resource fromNameWithInstance(const std::string& name,
                                           const std::string& instance) {
    return uri_resource{name, instance, ""};
  }

  static uri_resource forRpc(const std::string& commandName) {
    return uri_resource{"rpc", commandName, ""};
  }

  [[nodiscard]] bool isRPCMethod() const { return name == std::string("rpc"); }

  static uri_resource empty() {
    static const auto EMPTY = uri_datamodel::uri_resource("", "", "");
    return EMPTY;
  }

  [[nodiscard]] bool isEmpty() const {
    bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
    return whiteSpacesOnly && !getInstance().has_value() &&
           !getMessage().has_value();
  }

  [[nodiscard]] std::string getName() const { return name; }

  [[nodiscard]] std::string nameWithInstance() const {
    return getInstance().has_value() ? (getName() + "." + getInstance().value())
                                     : getName();
  }

  [[nodiscard]] std::optional<std::string> getInstance() const {
    bool whiteSpacesOnly =
        std::all_of(instance.begin(), instance.end(), isspace);
    return whiteSpacesOnly ? std::optional<std::string>()
                           : std::optional<std::string>(instance);
  }

  [[nodiscard]] std::optional<std::string> getMessage() const {
    bool whiteSpacesOnly = std::all_of(message.begin(), message.end(), isspace);
    return whiteSpacesOnly ? std::optional<std::string>()
                           : std::optional<std::string>(message);
  }

  bool operator==(const uri_resource& o) const {
    if (this == &o) {
      return true;
    }

    return ((name == o.name) && (instance == o.instance) &&
            (message == o.message));
  }

  [[nodiscard]] std::string tostring() const {
    return std::string("uResource{") + "name='" + name + '\'' + ", instance='" +
           (instance.empty() ? "null" : instance) + '\'' + ", message='" +
           (message.empty() ? "null" : message) + '\'' + '}';
  }

 private:
  std::string name;
  std::string instance;
  std::string message;
};
}  // namespace uri_datamodel

#endif  // URI_RESOURCE_H_
