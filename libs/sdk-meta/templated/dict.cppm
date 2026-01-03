module;

export module sdk:dict;

import :hash;
import :opt;
import :rc;
import :tuple;
import :utility;
import :vec;
import :math;

export namespace Meta {

template <typename K, typename V>
    requires(MoveConstructible<K> and requires(K const& key) {
        { hash(key) } -> Same<u64>;
    })
struct Dict {
    static inline usize const defaultCapacity    = 16;
    static inline usize const collisionThreshold = 8;

    using E = Pair<K, V>;

    struct Entry {
        i32 _hashCode;
        i32 _next;
        K   _key;
        V   _value;
    };
    Vec<i32>   _buckets;
    Vec<Entry> _entries;
    usize      _count;
    usize      _version; // Used for iterators
    i32        _releaseIndex;
    usize      _released;

    struct Subscript {
        K const& key;
        Dict&    table;
        Opt<V&>  value;

        Subscript& operator=(V const& val) {
            if (value) {
                *value = val;
            } else {
                table.put(key, val);
                value = table.get(key);
            }
            return *this;
        }

        operator Opt<V&>() { return value; }
    };

    Dict(usize cap = defaultCapacity)
        : _buckets(nextPrime(cap)),
          _entries(_buckets.len()),
          _count(0),
          _version(0),
          _released(0) {
        for (int i = 0; i < _buckets.len(); i++) {
            _buckets[i] = -1; // Initialize buckets to -1 (empty)
        }
        _releaseIndex = -1;
    }

    Dict(Items<E> list) : Dict(list.size()) {
        for (E const& pair : list) { }
    }

    Opt<i32> find(K const& key) const {
        i32 hashCode = ::hash(key) & 0x7FFF'FFFF;
        for (i32 i = _buckets[hashCode % _buckets.len()]; //
             i >= 0;
             i = _entries[i]._next) {
            if ((_entries[i]._hashCode == hashCode)
                and (key == _entries[i]._key)) {
                return i;
            }
        }
        return None {};
    }

    Subscript operator[](K const& key) {
        auto opt = find(key);
        if (opt) {
            return Subscript { .key   = key,
                               .table = *this,
                               .value = &_entries[*opt]._value };
        }
        return Subscript { .key = key, .table = *this, .value = {} };
    }

    Subscript const operator[](K const& key) const {
        auto opt = find(key);
        if (opt) {
            return Subscript { .key   = key,
                               .table = *(Dict*) this,
                               .value = &_entries[*opt]._value };
        }
        return Subscript { .key = key, .table = *(Dict*) this, .value = {} };
    }

    bool contains(K const& key) const { return find(key); }

    bool contains(Pair<K, V> const& pair) const {
        auto opt = find(pair.v1);
        return opt and (_entries[*opt]._value == pair.v2);
    }

    void resize() { resize(nextPrime(_buckets.len() * 2), true); }

    void resize(usize cap, bool newHashCodes) {
        if (cap < _buckets.len()) {
            return; // No need to shrink
        }

        Vec<i32>   buckets(nextPrime(cap));
        Vec<Entry> entries(buckets.len());
        for (i32 i = 0; i < buckets.len(); i++) {
            buckets[i] = -1; // Initialize new buckets to -1 (empty)
        }

        // TODO: copy entries array
        if (newHashCodes) {
            for (i32 i = 0; i < _count; i++) {
                if (entries[i]._hashCode != -1)
                    entries[i]._hashCode
                        = ::hash(entries[i]._key) & 0x7FFF'FFFF;
            }
        }
        for (i32 i = 0; i < _count; i++) {
            if (entries[i]._hashCode >= 0) {
                i32 bucket       = entries[i]._hashCode % buckets.len();
                entries[i]._next = buckets[bucket];
                buckets[bucket]  = i;
            }
        }
        ::exchange(_buckets, buckets);
        ::exchange(_entries, entries);
    }

    [[nodiscard]] Opt<i32> next(bool force = true) {
        i32 i;
        if (_released > 0) {
            i             = _releaseIndex;
            _releaseIndex = _entries[i]._next;
            _released--;
            return i;
        }
        if (count() == _entries.len()) {
            if (not force) {
                return {};
            }
            resize();
            i = _count;
        }
        i = _count;
        _count++;
        return i;
    }

    void put(K const& key, V const& value) {
        i32 hashCode = ::hash(key) & 0x7FFF'FFFF;
        u32 bucketId = hashCode % _buckets.len();

        u32 coll = 0;
        for (i32 i = _buckets[bucketId]; i >= 0; i = _entries[i]._next) {
            if ((_entries[i]._hashCode == hashCode)
                and (key == _entries[i]._key)) {
                new (&_entries[i]._value) V(::move(value));
                _version++;
                return;
            }
            coll++;
        }

        auto index = next();
        if (not index or index == -1) [[unlikely]] {
            panic("Dict::putIfAbsent(): No available index for new entry");
        }
        bucketId = hashCode % _buckets.len();

        new (&_entries[*index]) Entry {
            ._hashCode = hashCode,
            ._next     = _buckets[bucketId],
            ._key      = ::move(key),
            ._value    = ::move(value),
        };
        _buckets[bucketId] = *index;
        _version++;

        if (coll > collisionThreshold) {
            resize();
        }
    }

    void putIfAbsent(K const& key, V const& value) {
        if (find(key)) {
            return; // Key already exists, do not overwrite
        }
        i32 hashCode = ::hash(key) & 0x7FFF'FFFF;

        auto index = next();
        if (not index) [[unlikely]] {
            panic("Dict::putIfAbsent(): No available index for new entry");
        }
        u32 bucketId = hashCode % _buckets.len();

        new (&_entries[index]) Entry {
            ._hashCode = hashCode,
            ._next     = _buckets[bucketId],
            ._key      = ::move(key),
            ._value    = ::move(value),
        };
        _buckets[bucketId] = index;
        _version++;
    }

    template <typename... Args>
    void emplace(K const& key, Args&&... args) {
        i32 hashCode = ::hash(key) & 0x7FFF'FFFF;
        u32 bucketId = hashCode % _buckets.len();

        u32 coll = 0;
        for (i32 i = _buckets[bucketId]; i >= 0; i = _entries[i]._next) {
            if ((_entries[i]._hashCode == hashCode)
                and (key == _entries[i]._key)) {
                new (&_entries[i]._value) V(::forward<Args>(args)...);
                _version++;
                return;
            }
            coll++;
        }

        auto index = next();
        if (not index) [[unlikely]] {
            panic("Dict::putIfAbsent(): No available index for new entry");
        }
        bucketId = hashCode % _buckets.len();

        new (&_entries[index]) Entry {
            ._hashCode = hashCode,
            ._next     = _buckets[bucketId],
            ._key      = ::move(key),
            ._value    = V(::forward<Args>(args)...),
        };
        _buckets[bucketId] = index;
        _version++;

        if (coll > collisionThreshold) {
            resize();
        }
    }

    template <typename... Args>
    void emplaceIfAbsent(K const& key, Args&&... args) {
        if (find(key)) {
            return; // Key already exists, do not overwrite
        }
        i32 hashCode = ::hash(key) & 0x7FFF'FFFF;

        auto index = next();
        if (not index) [[unlikely]] {
            panic("Dict::putIfAbsent(): No available index for new entry");
        }
        u32 bucketId = hashCode % _buckets.len();

        new (&_entries[index]) Entry {
            ._hashCode = hashCode,
            ._next     = _buckets[bucketId],
            ._key      = ::move(key),
            ._value    = V(::forward<Args>(args)...),
        };
        _buckets[bucketId] = index;
        _version++;
    }

    Opt<V&> get(K const& key) const {
        auto elem = find(key);

        if (not elem) {
            return {};
        }
        return &(_entries[elem.unwrap()]._value);
    }

    Opt<V> take(K const& key) {
        auto elem = find(key);

        if (not elem) {
            return {};
        }

        Opt<V> value = ::move(_entries[elem]._value);
        remove(key);
        return value;
    }

    bool remove(K const& key) {
        // TODO: implement
        return false;
    }

    bool remove(K const& key, V const& value) {
        // TODO: implement
        return false;
    }

    usize count() const { return _count - _released; }

    void clear() {
        if (count() not_eq 0) {
            for (int i = 0; i < _buckets.len(); i++) {
                _buckets[i] = -1;
            }
            _entries.clear();
            _releaseIndex = -1;
            _count        = 0;
            _released     = 0;
            _version++;
        }
    }

    // constexpr auto begin() { return iter(); }

    // constexpr auto end() { return NONE; }
};

} // namespace Meta

using Meta::Dict;
