#pragma once

#include <folly/Poly.h>

#include <cassert>
#include <cstdint>
#include <unordered_map>

namespace null_engine {

template <class Client>
struct IProvider {
    template <class Base>
    struct Interface : Base {
        uint64_t Subscribe(folly::Poly<Client&> client) {
            return folly::poly_call<0>(*this, client);
        }

        void Unsubscribe(uint64_t subscribtion_id) {
            folly::poly_call<1>(*this, subscribtion_id);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::Subscribe, &T::Unsubscribe>;
};

template <class Client>
class ProviderBase {
public:
    ProviderBase() = default;

    virtual ~ProviderBase() {
        assert(clients_.empty() && "All clients should unsubscribe before provider destruction");
    }

    uint64_t Subscribe(folly::Poly<Client&> client) {
        clients_.emplace(subscribtion_id_++, std::move(client));
        return subscribtion_id_ - 1;
    }

    void Unsubscribe(uint64_t subscribtion_id) {
        clients_.erase(subscribtion_id);
    }

protected:
    uint64_t subscribtion_id_ = 0;
    std::unordered_map<uint64_t, folly::Poly<Client&>> clients_;
};

template <typename Derived, class Client>
class ClientBase {
    friend struct IClient;

public:
    explicit ClientBase(folly::Poly<IProvider<Client>&> provider)
        : provider_(provider)
        , subscribtion_id_(provider_->Subscribe(*static_cast<Derived*>(this))) {
    }

    ClientBase(const ClientBase& other)
        : provider_(other.provider_)
        , subscribtion_id_(provider_->Subscribe(*static_cast<Derived*>(this))) {
    }

    ClientBase(ClientBase&& other)
        : provider_(other.provider_)
        , subscribtion_id_(provider_->Subscribe(*static_cast<Derived*>(this))) {
    }

    virtual ~ClientBase() {
        provider_->Unsubscribe(subscribtion_id_);
    }

private:
    const folly::Poly<IProvider<Client>&> provider_;
    const uint64_t subscribtion_id_;
};

}  // namespace null_engine
