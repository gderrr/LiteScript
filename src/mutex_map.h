#pragma once
#include <map>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

template <typename Key, typename Value>
class mutex_map {

    public:

        mutex_map() = default;

        // [] equivalent
        void add_or_set (const Key& key, const Value& value) {
            std::unique_lock lock(mutex_);
            auto& entry = map_[key];
            std::unique_lock entry_lock(entry.mtx);
            entry.value = value;
        }

        void add_or_set (const Key& key, Value&& value) {
            std::unique_lock lock(mutex_);
            auto& entry = map_[key];
            std::unique_lock entry_lock(entry.mtx);
            entry.value = std::move(value);
        }

        // .at() equivalent for writing
        void set (const Key& key, const Value& value) {
            std::unique_lock lock(mutex_);
            auto& entry = map_.at(key);
            std::unique_lock entry_lock(entry.mtx);
            entry.value = value;
        }

        void set (const Key& key, Value&& value) {
            std::unique_lock lock(mutex_);
            auto& entry = map_[key];
            std::unique_lock entry_lock(entry.mtx);
            entry.value = std::move(value);
        }

        // .at() equivalent for reading
        Value at (const Key& key) const {
            std::shared_lock lock(mutex_);
            auto& entry = map_.at(key);
            std::shared_lock entry_lock(entry.mtx);
            return entry.value;
        }

        void erase (const Key& key) {
            std::unique_lock lock(mutex_);
            map_.erase(key);
        }

        size_t size () const {
            std::shared_lock lock(mutex_);
            return map_.size();
        }

        bool contains (const Key& key) const {
            std::shared_lock lock(mutex_);
            return map_.find(key) != map_.end();
        }

        struct LockedRef {
            std::unique_lock<std::shared_mutex> guard;
            Value& ref;
            LockedRef(std::unique_lock<std::shared_mutex>&& g, Value& r) : guard(std::move(g)), ref(r) {}
        };

        LockedRef get (const Key& key) {
            std::unique_lock lock(mutex_);
            auto& entry = map_.at(key);
            std::unique_lock entry_lock(entry.mtx);
            return LockedRef(std::move(entry_lock), entry.value);
        }

        std::vector<std::pair<Key,Value>> snapshot() const {
            std::vector<std::pair<Key,Value>> out;
            std::shared_lock lock(mutex_);
            for (auto& [key, entry] : map_) {
                std::shared_lock entry_lock(entry.mtx);
                out.emplace_back(key, entry.value);
            }
            return out;
        }

        void merge (const mutex_map& other) {
            auto other_snapshot = other.snapshot();
            for (auto& [key, value] : other_snapshot) {
                add_or_set(key, value);
            }
        }

    private:

        struct Entry {
            Value value;
            mutable std::shared_mutex mtx;
        };

        std::map<Key, Entry> map_;
        mutable std::shared_mutex mutex_;

};
