/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef UP_CPP_SAFE_MAP_H
#define UP_CPP_SAFE_MAP_H

#include <any>
#include <functional>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace uprotocol::utils {

template<typename MapT>
class BaseSafeMap;

template<class Key, class T,
    class Compare = typename std::map<Key, T>::key_compare,
    class Allocator = typename std::map<Key, T, Compare>::allocator_type>
using SafeMap = BaseSafeMap<std::map<Key, T, Compare, Allocator>>;

template<class Key, class T,
    class Hash = typename std::unordered_map<Key, T>::hasher,
    class KeyEqual = typename std::unordered_map<Key, T, Hash>::key_equal,
    class Allocator = typename std::unordered_map<Key, T, Hash, KeyEqual>::allocator_type>
using SafeUnorderedMap = BaseSafeMap<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>;

/// Internal details for BaseSafeMap implementation
namespace detail {
/// @brief An ugly hack to lock at initialization time during copy and move construction
///
/// Used to lock the "other" object when copying/moving. Requires an explicit call
/// to unlock() after the initializer list is done (in the ctor body).
struct CopyMoveCtorLocker {
    CopyMoveCtorLocker() = default;

    enum class LockMode {
        None, Unique, Shared
    };

    CopyMoveCtorLocker(LockMode mode, std::shared_mutex &m) noexcept;

    LockMode unlock() noexcept;

private:
    LockMode cmclock_mode{LockMode::None};
    std::unique_lock<std::shared_mutex> ulock;
    std::shared_lock<std::shared_mutex> slock;
};
} // namespace detail

/// @brief Wraps std::map in a std::shared_mutex for thread-safe access
///
/// By using std::shared_mutex, we can avoid serialization of const access to
/// the map. While there is still _some_ performance hit from adding the lock,
/// this mitigates the worst of it.
///
/// Aside from the locking, all interfaces are passthroughs to the underlying
/// std::map class.
///
/// @remarks We use protected inheritance from std::map so that any attempt to
///          use an interface that has not been protected by a mutex will fail
///          to compile with an error that the method is protected.
///
/// @remarks On the subject of iterators and transactions:<br>
///          This wrapper *does not* allow direct access to interfaces that
///          take or return iterators. This is because iterators already can
///          be easily invalidated by many operators, so adding concurrent
///          access makes it nearly impossible to use them. While locking could
///          be added to the iterators themselves, that would introduce new
///          risks (e.g. locks accidentally being held because an iterator
///          was held).
///          <br>
///          The solution is to provide transact() methods that allow for bulk
///          actions while holding the lock. For operations where iterators
///          or atomic operations need to occur on map data, a callable
///          (function pointer, lambda, etc) can be passed to transact(). The
///          lock will be held while the callable is running.
template<typename MapT>
class BaseSafeMap : private detail::CopyMoveCtorLocker, protected MapT {
    mutable std::shared_mutex map_lock_{};
    using UnsafeMapT = MapT;
    using SafeMapT = BaseSafeMap<MapT>;
    using CMCLock = detail::CopyMoveCtorLocker;

public:
    ///////////////////////////////////////////////////////////////////////////
    /////////////// New interfaces for safe bulk transactions /////////////////
    ///////////////////////////////////////////////////////////////////////////
    using ModifyTxn = std::function<std::any(MapT&)>;
    std::any transact(ModifyTxn &&f) {
        std::unique_lock lock(map_lock_);
        return f(*this);
    }

    using ConstTxn = std::function<std::any(MapT const&)>;
    std::any transact(ConstTxn &&f) const {
        std::shared_lock lock(map_lock_);
        return f(*this);
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////// Pass-throughs for std::map //////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Pass through all the type definitions (except those specific to
    // situations where iterators can / must be used.
    using key_type = typename MapT::key_type;
    using mapped_type = typename MapT::mapped_type;
    using value_type = typename MapT::value_type;
    using size_type = typename MapT::size_type;
    using difference_type = typename MapT::difference_type;
    //using key_compare = typename MapT::key_compare;
    //using value_compare = typename MapT::value_compare;
    using allocator_type = typename MapT::allocator_type;
    using reference = typename MapT::reference;
    using const_reference = typename MapT::const_reference;
    using pointer = typename MapT::pointer;
    using const_pointer = typename MapT::const_pointer;
    //using iterator = typename MapT::iterator;
    //using const_iterator = typename MapT::const_iterator;
    //using reverse_iterator = typename MapT::reverse_iterator;
    //using const_reverse_iterator = typename MapT::const_reverse_iterator;
    using node_type = typename MapT::node_type;
    //using insert_return_type = typename MapT::insert_return_type;
    ////TODO Base-class-specific using statements (e.g. hasher)

    // Pass through the base map class constructors to allow construction
    // from an unsafe map. Also covers some of the generic iterator
    // constructors.
    using MapT::MapT;

    // Define the constructors we need for copy and move
    BaseSafeMap() = default;

    BaseSafeMap(const SafeMapT &other)
    : CMCLock(LockMode::Shared, other.map_lock_), MapT(other) {
        CMCLock::unlock();
    }

    BaseSafeMap(const SafeMapT &other, const allocator_type &alloc)
    : CMCLock(LockMode::Shared, other.map_lock_), MapT(other, alloc) {
        CMCLock::unlock();
    }

    BaseSafeMap(SafeMapT &&other)
    : CMCLock(LockMode::Unique, other.map_lock_), MapT(std::move(other)) {
        CMCLock::unlock();
    }

    BaseSafeMap(SafeMapT &&other, const allocator_type &alloc)
    : CMCLock(LockMode::Unique, other.map_lock_), MapT(std::move(other), alloc) {
        CMCLock::unlock();
    }

    ///////////////////////////////////////////////////////////////////////////
    /////////// Locking wrappers for existing std::map interfaces /////////////
    ///////////////////////////////////////////////////////////////////////////
    SafeMapT& operator=(const SafeMapT &other) {
        std::scoped_lock lock(map_lock_, other.map_lock_);
        MapT::operator=(other);
        return *this;
    }

    SafeMapT& operator=(SafeMapT &&other) noexcept {
        std::scoped_lock lock(map_lock_, other.map_lock_);
        MapT::operator=(std::move(other));
        return *this;
    }

    SafeMapT& operator=(std::initializer_list<value_type> ilist) {
        std::unique_lock lock(map_lock_);
        MapT::operator=(ilist);
        return *this;
    }

    // skip get_allocator()

    mapped_type& at(const key_type &key) {
        std::unique_lock lock(map_lock_);
        return MapT::at(key);
    }

    const mapped_type& at(const key_type &key) const {
        std::shared_lock lock(map_lock_);
        return MapT::at(key);
    }

    mapped_type& operator[](const key_type &key) {
        std::unique_lock lock(map_lock_);
        return MapT::operator[](key);
    }

    mapped_type& operator[](key_type &&key) {
        std::unique_lock lock(map_lock_);
        return MapT::operator[](std::move(key));
    }

    // skip iterators - those should only be used through the transact interfaces
    // skip begin() and cbegin()
    // skip end() and cend()
    // skip rbegin() and crbegin()
    // skip rend() and crend()

    [[nodiscard]] bool empty() const noexcept {
        std::shared_lock lock(map_lock_);
        return MapT::empty();
    }

    size_type size() const noexcept {
        std::shared_lock lock(map_lock_);
        return MapT::size();
    }

    size_type max_size() const noexcept {
        std::shared_lock lock(map_lock_);
        return MapT::max_size();
    }

    void clear() noexcept {
        std::unique_lock lock(map_lock_);
        MapT::clear();
    }

    // skip modifiers that rely on iterators - see "skip iterators" above
    // TODO Provide static methods that can be passed to the txn interfaces
    // insert()
    // insert_range()
    // insert_or_assign()
    // emplace()
    // emplace_hint()
    // try_emplace()
    // erase()
    // extract()
    
    void swap(MapT &other) noexcept {
        std::unique_lock lock(map_lock_);
        MapT::swap(other);
    }

    // skip merge - I think it would work fine, but I'm also not sure anyone
    // is using it right now, so we'll wait until it is needed

    size_type count(const key_type &key) const {
        std::shared_lock lock(map_lock_);
        return MapT::count(key);
    }

    template<class K>
    size_type count(const K &x) const {
        std::shared_lock lock(map_lock_);
        return MapT::count(x);
    }

    // skip find - it requires iterators, so must be in a transaction

    bool contains(const key_type &key) const {
        std::shared_lock lock(map_lock_);
        return MapT::contains(key);
    }

    template<class K>
    bool contains(const K &x) const {
        std::shared_lock lock(map_lock_);
        return MapT::contains(x);
    }

    // skip equal_range - iterators
    // skip lower_bound and upper_bound - iterators
    // skip key_comp and value_comp - they can interact with iterators
};

// TODO comparison operator, std::swap(), and erase_if()
// Should these be implemented with locking, or should we just dump that into
// transactions?

namespace detail {
CopyMoveCtorLocker::LockMode CopyMoveCtorLocker::unlock() noexcept {
    if (cmclock_mode == LockMode::Unique) {
        ulock.unlock();
        ulock.release();
    } else if (cmclock_mode == LockMode::Shared) {
        slock.unlock();
        slock.release();
    }
    auto original_mode = cmclock_mode;
    cmclock_mode = LockMode::None;
    return original_mode;
}

CopyMoveCtorLocker::CopyMoveCtorLocker(LockMode mode, std::shared_mutex &m) noexcept
: cmclock_mode(mode) {
    if (cmclock_mode == LockMode::Unique) {
        ulock = std::unique_lock(m);
    } else if (cmclock_mode == LockMode::Shared) {
        slock = std::shared_lock(m);
    }
}
} // namespace detail
} // namespace uprotocol::utils

#endif //UP_CPP_SAFE_MAP_H
