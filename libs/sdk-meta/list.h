#pragma once

#include <sdk-meta/cursor.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/std.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename T>
struct Lnode : Meta::Pinned {
    T* prev { nullptr };
    T* next { nullptr };
};

template <typename>
struct List;

// MARK: - Intrusive List

template <typename T>
    requires requires(T& t) {
        { t.item } -> Meta::Same<Lnode<T>>;
    }
struct List<T> {
    T*    _head;
    T*    _tail;
    usize _count;
    usize _version;

    constexpr static T::Item* Item = &T::item;

    static auto& item(T* value) { return value->*Item; }

    static T*& prev(T* value) { return item(value).prev; }

    static T*& next(T* value) { return item(value).next; }

    List() = default;

    List(InitializerList<T> initials);

    List(List const& other) {
        // clear()
        for (auto& item : other) {
            append(item);
        }
    }

    List(List&& other) noexcept
        : _head(other._head),
          _tail(other._tail),
          _count(other._count) {
        other._head  = nullptr;
        other._tail  = nullptr;
        other._count = 0;

        other._version = 0;
    }

    T const& operator[](usize index) const {
        if (index >= _count) {
            panic("List::operator[]: index out of bounds.");
        }
        return *at(index).unwrap();
    }

    T& operator[](usize index) {
        if (index >= _count) {
            panic("List::operator[]: index out of bounds.");
        }
        return *at(index).unwrap();
    }

    T* head() { return _head; }

    T const* head() const { return _head; }

    T* tail() { return _tail; }

    T const* tail() const { return _tail; }

    usize count() const { return _count; }

    void insert(usize index, T const& value) {
        if (index > _count) {
            panic("List::insert: index out of bounds.");
        }

        auto* p = &value;
        if (index == 0)
            prepend(value);
        else if (index == _count)
            append(value);
        else {
            // Traverse to the node before the insertion point
            auto* current = _head;
            for (usize i = 0; i < index - 1; ++i)
                current = current->item.next;
            next(p) = next(current);
            prev(p) = current;
            if (next(current) != nullptr)
                prev(next(current)) = p;
            else {
                _tail = p;
            }
            next(current) = p;
        }
    }

    void prepend(T const& value) {
        auto* p = &value;

        if (_head == EMPTY)
            _head = _tail = p;
        else {
            next(p)     = _head;
            prev(_head) = p;
            _head       = p;
        }
        prev(p) = nullptr;

        _count++;
        _version++;
    }

    void append(T const& value) {
        auto* p = &value;

        if (_head == EMPTY)
            _head = _tail = p;
        else {
            next(_tail) = p;
            prev(p)     = _tail;
            _tail       = p;
        }
        next(p) = nullptr;

        _count++;
        _version++;
    }

    Opt<Cursor<T>> at(usize index) {
        if (index >= _count) {
            return {};
        }
        // Traverse the list to find the node at the given index
        auto* current = _head;
        for (usize i = 0; i < index; ++i) {
            current = current->next;
        }
        return { &current->value };
    }

    void remove(usize index) { }

    void remove(T const& value) { }

    void clear() {
        while (_head) {
            auto* next = _head->next;
            delete _head;
            _head = next;
        }
        _tail  = nullptr;
        _count = 0;

        _version = 0;
    }

    bool contains(T const& value) const {
        for (auto* current = _head; current; current = current->next) {
            if (current->value == value) {
                return true;
            }
        }
        return false;
    }

    auto iter() {
        return Iter(
            [this, curr = head(), ver = _version] mutable -> Opt<Cursor<T>> {
            if (ver != _version) {
                panic("List::iter(): List modified during iteration");
            }

            if (curr) {
                auto* val = curr;
                curr      = next(curr);
                return Cursor(val);
            }
            return EMPTY;
        });
    }
};

// MARK: - Non-intrusive List

template <typename T>
    requires(not requires(T& t) {
        { t.item } -> Meta::Same<Lnode<T>>;
    })
struct List<T> {
    struct Item {
        T           value;
        Lnode<Item> item;
    };

    List<Item> _list;

    List() = default;

    List(InitializerList<T> initials) {
        // clear()
        // Append all initial values
    }

    List(List const& other) : _list(other._list) { }

    List(List&& other) noexcept : _list(::move(other._list)) { }

    ~List() { clear(); }

    T* head() { return _list.head() ? &_list.head()->value : nullptr; }

    T const* head() const {
        return _list.head() ? &_list.head()->value : nullptr;
    }

    T* tail() { return _list.tail() ? &_list.tail()->value : nullptr; }

    T const* tail() const {
        return _list.tail() ? &_list.tail()->value : nullptr;
    }

    usize count() const { return _list.count(); }

    void insert(usize index, T const& value) {
        _list.insert(index, { ::move(value), {} });
    }

    void prepend(T const& value) { _list.prepend({ ::move(value), {} }); }

    void append(T const& value) { _list.append({ ::move(value), {} }); }

    template <typename... Args>
    void emplace(usize index, Args&&... args) {
        if (index > _list.count()) {
            panic("List::emplace: index out of bounds.");
        }

        auto* n = new Item { T(::forward<Args>(args)...), {} };
    }

    Opt<Cursor<T>> at(usize index) {
        return _list.at(index).map(
            [](Item* item) { return Cursor(&item->value); });
    }

    void remove(usize index) {
        auto opt = at(index);
        if (opt) {
            remove(*opt.unwrap());
        }
    }

    void remove(T const& value) { }

    void clear() { _list.clear(); }

    bool contains(T const& value) const {
        for (auto& item : _list) {
            if (item.value == value) {
                return true;
            }
        }
        return false;
    }

    auto iter() {
        return Iter([_list = &_list,
                     head  = _list.head(),
                     ver   = _list._version] mutable -> Opt<Cursor<T>> {
            if (ver != _list->_version) {
                panic("List::iter(): List modified during iteration");
            }

            if (head) {
                auto* val = head;
                head      = List<Item>::next(head);
                return Cursor(&val->value);
            }
            return EMPTY;
        });
    }
};

} // namespace Meta

using Meta::List;
