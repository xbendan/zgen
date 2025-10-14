#pragma once

#include <sdk-meta/cursor.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename T>
struct Lnode : Meta::Pinned {
    T* prev { nullptr };
    T* next { nullptr };
};

template <typename T>
concept HasLnode = requires(T& t) {
    requires Meta::Same<Meta::RemoveCvRef<decltype(T::lnode)>, Lnode<T>>;
};

template <typename>
struct List;

// MARK: - Intrusive List

template <typename T>
    requires(HasLnode<T>)
struct List<T> {
    T*    _head;
    T*    _tail;
    usize _count;
    usize _version;

    constexpr static auto T::* Lnode = &T::lnode;

    static auto& item(T* value) { return value->*Lnode; }

    static T*& prev(T* value) { return item(value).prev; }

    static T*& next(T* value) { return item(value).next; }

    List() noexcept = default;

    List(InitializerList<T> initials);

    template <typename U = T>
        requires(Meta::CopyConstructible<T, U>)
    List(List<U> const& other) {
        clear();
        for (auto& item : other) {
            append(item);
        }
    }

    template <typename U = T>
        requires(Meta::Convertible<U, T>)
    List(List<U>&& other) noexcept
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
        return at(index).unwrap();
    }

    T& operator[](usize index) {
        if (index >= _count) {
            panic("List::operator[]: index out of bounds.");
        }
        return at(index).unwrap();
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

        if (_head == NONE)
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

        if (_head == NONE)
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

    Opt<T&> at(usize index) {
        if (index >= _count) {
            return NONE;
        }
        // Traverse the list to find the node at the given index
        auto* current = _head;
        for (usize i = 0; i < index; ++i) {
            current = current->next;
        }
        return &current->value;
    }

    void remove(usize index) {
        // TODO: check correctness because AI generated this part
        if (index >= _count) {
            panic("List::remove: index out of bounds.");
        }

        auto* current = _head;
        for (usize i = 0; i < index; ++i)
            current = next(current);

        if (prev(current))
            next(prev(current)) = next(current);
        else
            _head = next(current);

        if (next(current))
            prev(next(current)) = prev(current);
        else
            _tail = prev(current);

        next(current) = nullptr;
        prev(current) = nullptr;

        _count--;
        _version++;
    }

    void remove(T const& value) {
        // TODO: check correctness because AI generated this part
        auto* current = _head;
        while (current) {
            if (current->value == value) {
                // Found the node to remove
                if (prev(current))
                    next(prev(current)) = next(current);
                else
                    _head = next(current);

                if (next(current))
                    prev(next(current)) = prev(current);
                else
                    _tail = prev(current);

                delete current;
                _count--;
                _version++;
                return;
            }
            current = next(current);
        }
    }

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
            return NONE;
        });
    }
};

template <typename T>
struct List {
    struct Node {
        T           value;
        Lnode<Node> lnode;
    };
    List<Node> _list;

    // Other member functions...
};

} // namespace Meta

using Meta::List;
