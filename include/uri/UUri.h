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
#ifndef UURI_H_
#define UURI_H_

#include <string>
#include <utility>

#include "UriFormat.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"

namespace uprotocol::uri {

/**
 * Data representation of uProtocol <b>URI</b>.
 * This class will be used to represent the source and sink (destination) parts of the Packet, for example in a CloudEvent Packet. <br>
 * UUri is used as a method to uniquely identify devices, services, and resources on the  network.<br>
 * Where software is deployed, what the service is called along with a version and the resources in the service.
 * Defining a common URI for the system allows applications and/or services to publish and discover each other
 * as well as maintain a database/repository of microservices in the various vehicles.<br>
 * Example for long format serialization:
 * <pre>
 *     //&lt;device&gt;.&lt;domain&gt;/&lt;service&gt;/&lt;version&gt;/&lt;resource&gt;#&lt;message&gt;
 * </pre>
 *
 */
class UUri : public UriFormat {
public:
    /**
     * Create a full  URI.
     * @param uAuthority The uAuthority represents the deployment location of a specific Software Entity .
     * @param uEntity The uEntity in the role of a service or in the role of an application is the software and version.
     * @param uResource The uResource is something that is manipulated by a service such as a Door.
     */
    UUri(const UAuthority&  uAuthority,
         const UEntity&     uEntity,
         const UResource&   uResource)
        : uAuthority_(uAuthority), uEntity_(uEntity), uResource_(uResource) {}

    /**
     * Create a URI for a resource. This will match all the specific instances of the resource,
     *      for example all the instances of the vehicle doors.
     * @param uAuthority The  Authority represents the deployment location of a specific  Software Entity.
     * @param uEntity The USE in the role of a service or in the role of an application.
     * @param uResource The resource is something that is manipulated by a service such as a Door.
     */
    UUri(const UAuthority&  uAuthority,
        const UEntity&     uEntity,
        const std::string& uResource)
        : UUri(uAuthority, uEntity, UResource::longFormat(uResource)) {}

    /**
     * Create an RPC Response UUri passing the Authority and Entity information.
     * @param uAuthority The uAuthority represents the deployment location of a specific Software Entity.
     * @param uEntity The SW entity information.
     * @return Returns a UUri of a constructed RPC Response.
     */
    static UUri rpcResponse(UAuthority const& UAuthority,
                            UEntity const&    UEntity) {
      return UUri(UAuthority, UEntity, UResource::forRpcResponse());
    }

    /**
     * Static factory method for creating an empty  uri, to avoid working with null<br>
     * @return Returns an empty uri to avoid working with null.
     */
    static UUri empty() {
      static const auto EMPTY = UUri(UAuthority::empty(), UEntity::empty(), UResource::empty());
      return EMPTY;
    }

    /**
     * Indicates that this  URI is an empty container and has no valuable information in building uProtocol sinks or sources.
     * @return Returns true if this  URI is an empty container and has no valuable information in building uProtocol sinks or sources.
     */
    [[nodiscard]] bool isEmpty() const override {
      return uAuthority_.isEmpty() && uEntity_.isEmpty() && uResource_.isEmpty();
    }

    /**
     * Returns true if URI contains both names and numeric representations of the names inside its belly.
     * Meaning that this UUri can be serialized to long or micro formats.
     * @return Returns true if URI contains both names and numeric representations of the names inside its belly.
     *      Meaning that this UUri can buree serialized to long or micro formats.
     */
    [[nodiscard]] bool isResolved() const override {
      return uAuthority_.isResolved() && uEntity_.isResolved() && uResource_.isResolved();
    }

    /**
     * Determines if this UUri can be serialized into a long form UUri.
     * @return Returns true if this UUri can be serialized into a long form UUri.
     */
    [[nodiscard]] bool isLongForm() const override {
      return uAuthority_.isLongForm() &&
            (uEntity_.isLongForm() || uEntity_.isEmpty()) &&
            (uResource_.isLongForm() || uResource_.isEmpty());
    }

    /**
     * Determines if this UUri can be serialized into a micro form UUri.
     * @return Returns true if this UUri can be serialized into a micro form UUri.
     */
    [[nodiscard]] bool isMicroForm() const override {
      return uAuthority_.isMicroForm() && uEntity_.isMicroForm() && uResource_.isMicroForm();
    }

    /**
     * @return Returns the Authority represents the deployment location of a specific Software Entity.
     */
    [[nodiscard]] UAuthority getUAuthority() const { return uAuthority_; }

    /**
     * @return Returns the USE in the role of a service or in the role of an application.
     */
    [[nodiscard]] UEntity getUEntity() const { return uEntity_; }

    /**
     * @return Returns the  resource, something that is manipulated by a service such as a Door.
     */
    [[nodiscard]] UResource getUResource() const { return uResource_; }

    /**
     * Compares this UUri to the specified object.
     * @param o The object to compare this UUri against.
     * @return Returns true if and only if the specified object is a UUri whose components are equal to the components of this UUri.
     */
    bool operator==(const UUri& o) const {
      if (this == &o) {
        return true;
      }
      return (uAuthority_ == o.uAuthority_) &&
            (uEntity_ == o.uEntity_) &&
            (uResource_ == o.uResource_);
    }

    /**
     * Converts this UUri to a String.
     * @return Returns a string representation of this UUri.
     */
    [[nodiscard]] std::string tostring() const {
      return std::string("UriPart{") +
                        "uAuthority=" + uAuthority_.tostring() +
                        ", uEntity=" + uEntity_.tostring() +
                        ", uResource=" + uResource_.tostring() + '}';
    }

private:
    /**
     * UAuthority represents the deployment location of a specific Software Entity.
     */
    const UAuthority uAuthority_;
    /**
     * UEntity represents the role of a service or in the role of an application.
     */
    const UEntity uEntity_;
    /**
     * UResource represents something that is manipulated by a service such as a Door.
     */
    const UResource uResource_;
}; // class UUri

}  // namespace uprotocol::uri

#endif  // UURI_H_
