#pragma once

#include <null_engine/generic/generic_scene.hpp>
#include <null_engine/util/interface/helpers/window.hpp>
#include <null_engine/util/interface/interface_holder.hpp>

namespace null_engine::testlib {

class BaseApplication {
public:
    BaseApplication() = default;

    util::WindowPtr GetWindow() const;

    void Run();

protected:
    virtual void SetUp();
    virtual void Draw();

protected:
    virtual util::WindowPtr CreateWindow() = 0;
    virtual generic::Scene CreateScene() = 0;
    virtual util::InterfaceHolder CreateInterface() = 0;

private:
    util::WindowPtr window_;
    generic::Scene scene_;
    util::InterfaceHolder interface_;
};

}  // namespace null_engine::testlib
