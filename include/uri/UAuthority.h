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
#ifndef UAUTHORITY_H_
#define UAUTHORITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

#include "UriFormat.h"

namespace uprotocol::uri {

/**
 * An Authority represents the deployment location of a specific Software Entity.
 * Data representation of an <b>Authority</b>.<br> An Authority consists of a device, a domain and a micro version in the form of an IP Address.<br>
 * Device and domain names are used as part of the URI for device and service discovery. Optimized micro versions of the UUri will use the IP Address.<br>
 * Devices will be grouped together into realms of Zone of Authority.<br>
 */
class UAuthority : public UriFormat {
public:
    /**
     * Constructor for building a UAuthority with empty contents.
     */
    UAuthority() { UAuthority::empty(); }

    /**
     * Static factory method for creating a local uAuthority.<br>
     * A local uri does not contain an authority and looks like this:
     * <pre> :&lt;service&gt;/&lt;version&gt;/&lt;resource&gt;#&lt;Message&gt; </pre>
     * @return Returns a local uAuthority that has no domain, device, or ip address information, indicating to uProtocol
     *      that the uAuthority part in the UUri is relative to the sender/receiver deployment environment.
     */
    static UAuthority local() { return empty(); }

    /**
     * Static factory method for creating a remote authority supporting the long serialization information representation of a UUri.<br>
     * Building a UAuthority with this method will create an unresolved uAuthority that can only be serialised in long UUri format.
     * An uri with a long representation of uAUthority can be serialised as follows:
     * <pre> //&lt;device&gt;.&lt;domain&gt;/&lt;service&gt;/&lt;version&gt;/&lt;resource&gt;#&lt;Message&gt; </pre>
     * @param device The device a software entity is deployed on, such as the VCU, CCU or cloud provider.
     * @param domain The domain a software entity is deployed on, such as vehicle or Cloud (PaaS).
     * Vehicle Domain name <b>MUST</b> be that of the vehicle VIN.
     * @return Returns a uAuthority that contains the device and the domain and can only be serialized in long UUri format.
     */
    static UAuthority longRemote(const std::string& device,
                                const std::string& domain) {
      return UAuthority{device, domain, "", true, false};
    }

    /**
     * Static factory method for creating a remote authority supporting the micro serialization information representation of a UUri.<br>
     * Building a UAuthority with this method will create an unresolved uAuthority that can only be serialised in micro UUri format.
     * @param address The ip address of the device a software entity is deployed on.
     * @return Returns a uAuthority that contains only the internet address of the device, and can only be serialized in micro UUri format.
     */
    static UAuthority microRemote(const std::string& address) {
      return UAuthority{"", "", address, true, false};
    }

    /**
     * Static factory method for creating a remote authority that is completely resolved with name, device and ip address of the device.<br>
     * Building a UAuthority with this method will enable serialisation in both UUri formats, long UUri format and micro UUri format.
     * Note that in the case of missing data, this will not fail, but simply create a UAuthority that is not resolved.
     * @param device The device name for long serialization of UUri.
     * @param domain The domain name for long serialization of UUri.
     * @param address the IP address for the device, for micro serialization of UUri.
     * @return Returns a uAuthority that contains all resolved data and so can be serialized into a long UUri or a micro UUri.
     */
    static UAuthority resolvedRemote(const std::string& device,
                                    const std::string& domain,
                                    const std::string& address) {
      bool isResolved = !isBlank(device) && !address.empty();
      return UAuthority{device, domain, address, true, isResolved};
    }

    /**
     * Static factory method for creating an empty uAuthority, to avoid working with null<br>
     * Empty uAuthority is still serializable in both long and micro UUri formats, and will be as local to the current deployment environment.
     * @return Returns an empty authority that has no domain, device, or device ip address information.
     */
    static UAuthority empty() {
      static const auto EMPTY = UAuthority("", "", "", false, true);
      return EMPTY;
    }

    /**
     * Accessing an optional device of the uAuthority.
     * @return Returns the device a software entity is deployed on, such as the VCU, CCU or cloud provider.
     */
    [[nodiscard]] std::optional<std::string> getDevice() const {
      return isBlank(device_) ? std::nullopt : std::optional<std::string>(device_);
    }

    /**
     * Accessing an optional domain of the uAuthority.
     * @return Returns the domain a software entity is deployed on, such as vehicle or backoffice.
     */
    [[nodiscard]] std::optional<std::string> getDomain() const {
      return isBlank(domain_) ? std::nullopt : std::optional<std::string>(domain_);
    }

    /**
     * Accessing an optional IP address configuration of the uAuthority.
     * @return Returns the device IP address.
     */
    [[nodiscard]] std::optional<std::string> getAddress() const {
      return isBlank(address_) ? std::nullopt : std::optional<std::string>(address_);
    }

    /**
     * Support for determining if this uAuthority is defined for a local deployment.
     * @return returns true if this uAuthority is local, meaning does not contain a device/domain for long UUri or information for micro UUri.
     */
    [[nodiscard]] bool isLocal() const {
      return isEmpty() && !isMarkedRemote();
    }

    /**
     * Support for determining if this uAuthority defines a deployment that is defined as remote.
     * @return Returns true if this uAuthority is remote, meaning it contains information for serialising a long UUri or a micro UUri.
     */
    [[nodiscard]] bool isRemote() const {
      return isMarkedRemote();
    }

    /**
     * Support for determining if this uAuthority was configured to be remote.
     * @return Returns true if this uAuthority is explicitly configured remote deployment.
     */
    [[nodiscard]] bool isMarkedRemote() const { return markedRemote_; }

    /**
     * Indicates that this UAuthority has already been resolved.
     * A resolved UAuthority means that it has all the information needed to be serialised in the long format or the micro format of a UUri.
     * @return Returns true if this UAuthority is resolved with all the information needed to be serialised in the long format or the micro format of a UUri.
     */
    [[nodiscard]] bool isResolved() const override { return markedResolved_; }

    /**
     * Determine if the UAuthority can be used to serialize a UUri in long format.
     * @return Returns true if the UAuthority can be used to serialize a UUri in long format.
     */
    [[nodiscard]] bool isLongForm() const override {
      return isLocal() || getDevice().has_value();
    }

    /**
     * Determine if the UAuthority can be used to serialize a UUri in micro format.
     * @return Returns true if the uAuthority can be serialized a UUri in micro format.
     */
    [[nodiscard]] bool isMicroForm() const override {
      return isLocal() || getAddress().has_value();
    }

    /**
     * Indicates that the UAuthority is an empty container.
     * @return Returns true if this UAuthority is empty.
     */
    [[nodiscard]] bool isEmpty() const override {
      return !getDevice().has_value() && !getDomain().has_value() && !getAddress().has_value();
    }

    /**
     * Equality operator for comparing two UAuthorities.
     * @param o The UAuthority to compare with.
     * @return Returns true if the UAuthorities are equal.
     */
    [[nodiscard]] bool operator==(const UAuthority& o) const {
      if (this == &o) {
        return true;
      }
      return (markedRemote_ == o.markedRemote_) &&
            (markedResolved_ == o.markedResolved_) &&
            (device_ == o.device_) &&
            (domain_ == o.domain_) &&
            (address_ == o.address_);
    }

    /**
     * Utility method to convert the UAuthority to a string.
     * @return Returns a string representation of the UAuthority.
     */
    [[nodiscard]] std::string tostring() const {
      return std::string("uAuthority{") +
                        "device='" + (device_.empty() ? "null" : device_) + '\'' +
                        ", domain='" + (domain_.empty() ? "null" : domain_) + '\'' +
                        ", address='" + (address_.empty() ? "null" : address_) + '\'' +
                        ", markedRemote=" + (markedRemote_ ? "true" : "false") + '\'' +
                        ", markedResolved=" + (markedResolved_ ? "true" : "false") + '}';
    }

 private:
    /**
     * Constructor for building a UAuthority.
     * @param device        The device a software entity is deployed on, such as the VCU, CCU or cloud provider.
     * @param domain        The domain a software entity is deployed on, such as vehicle or Cloud (PaaS).
     * @param address      The device IP address of the device.
     * @param markedRemote  Indicates if this UAuthority was implicitly marked as remote.
     * @param markedResolved Indicates that this uAuthority has all the information needed to be serialised
     *                          in the long format or the micro format of a UUri.
     */
    UAuthority(const std::string& device,
              const std::string& domain,
              const std::string& address,
              const bool         markedRemote,
              const bool         markedResolved)
        : address_(address), markedRemote_(markedRemote), markedResolved_(markedResolved) {
      if (!device.empty()) {
        auto current = device;
        std::transform(current.begin(), current.end(), current.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        this->device_ = std::move(current);
      }
      if (!domain.empty()) {
        auto current = domain;
        std::transform(current.begin(), current.end(), current.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        this->domain_ = std::move(current);
      }
    }

    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static bool isBlank(std::string_view str) {
      return std::all_of(str.begin(), str.end(), isspace);
    }

    /**
     * A device is a logical independent representation of a service bus in different execution environments.<br>
     * Devices will be grouped together into realms of Zone of Authority.
     */
    std::string device_;
    /**
     * The domain a software entity is deployed on, such as vehicle or backoffice.<br>
     * Vehicle Domain name <b>MUST</b> be that of the vehicle VIN.<br>
     * A domain name is an identification string that defines a realm of administrative autonomy, authority or control within the Internet.
     */
    std::string domain_;
    /**
     * The device IP address. Represents the micro version of a UAuthority.
     */
    std::string address_;
    /**
     * An UAuthority starting with // is a remote configuration of a URI, and we mark the uAuthority implicitly as remote.
     * This is never exposed externally and is used internally to indicate remote or local deployments.
     */
    bool markedRemote_;
    /**
     * Indicates that this UAuthority has already been resolved.
     * A resolved UAuthority means that it has all the information needed to be serialised in the long format or the micro format of a UUri.
     */
    bool markedResolved_;
}; // class UAuthority

} // namespace uprotocol::uri

#endif // UAUTHORITY_H_
