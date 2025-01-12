#pragma once

#include <folly/Poly.h>

namespace null_engine::util {

template <class Client>
struct IProvider {
    template <class Base>
    struct Interface : Base {
        void Subscribe(folly::Poly<Client&> client) {
            folly::poly_call<0>(*this, client);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::Subscribe>;
};

template <class Client>
class ProviderBase {
public:
    ProviderBase() = default;

    virtual ~ProviderBase() = default;

    void Subscribe(folly::Poly<Client&> client) {
        clients_.emplace_back(std::move(client));
    }

protected:
    std::vector<folly::Poly<Client&>> clients_;
};

}  // namespace null_engine::util
