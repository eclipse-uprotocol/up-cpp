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
#ifndef UP_URI_H_
#define UP_URI_H_

#include <string>
#include <utility>

#include "uri_authority.h"
#include "uri_entity.h"
#include "uri_resource.h"

namespace uri_datamodel {
class UUri {
 public:
  static const std::string SCHEME;

  UUri(const uri_authority& uAuthority, uri_entity uEntity,
         const uri_resource& uResource)
      : uAuthority(uAuthority),
        uEntity(std::move(uEntity)),
        uResource(uResource) {}

  UUri(const uri_authority& uAuthority, const uri_entity& uEntity,
         const std::string& uResource)
      : UUri(uAuthority, uEntity, uri_resource::fromName(uResource)) {}

  static UUri empty() {
    static const auto EMPTY =
        uri_datamodel::UUri(uri_datamodel::uri_authority::empty(),
                              uri_datamodel::uri_entity::empty(),
                              uri_datamodel::uri_resource::empty());
    return EMPTY;
  }

  [[nodiscard]] bool isEmpty() const {
    return uAuthority.isLocal() && getUEntity().isEmpty() &&
           uResource.isEmpty();
  }

  [[nodiscard]] uri_authority getUAuthority() const { return uAuthority; }

  [[nodiscard]] uri_entity getUEntity() const { return uEntity; }

  [[nodiscard]] uri_resource getUResource() const { return uResource; }

  bool operator==(const UUri& o) const {
    if (this == &o) {
      return true;
    }

    return ((uAuthority == o.uAuthority) && (uEntity == o.uEntity) &&
            (uResource == o.uResource));
  }

  [[nodiscard]] std::string tostring() const {
    return std::string("upUri{") + "uAuthority=" + uAuthority.tostring() +
           ", uEntity=" + uEntity.tostring() +
           ", uResource=" + uResource.tostring() + '}';
  }

 private:
  const uri_authority uAuthority;
  const uri_entity uEntity;
  const uri_resource uResource;
};

const std::string UUri::SCHEME = std::string("up:");
}  // namespace uri_datamodel

#endif  // up_URI_H_
