#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <cstdlib>
#include <memory>

using namespace jbatnozic;
namespace hg = jbatnozic::hobgoblin;
namespace spe = jbatnozic::spempe;

constexpr auto WINDOW_WIDTH  = 640;
constexpr auto WINDOW_HEIGHT = 320;
constexpr auto FRAME_RATE = 60;

std::unique_ptr<spe::GameContext> CreateGameContext() {
    auto ctx = std::make_unique<spe::GameContext>(spe::GameContext::RuntimeConfig{});
    ctx->setToMode(spe::GameContext::Mode::GameMaster);

    auto winMgr = std::make_unique<spe::DefaultWindowManager>(ctx->getQAORuntime(), 0);
    winMgr->setToNormalMode(
        spe::WindowManagerInterface::WindowConfig{
            hg::win::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT},
            "SPeMPE Multiplayer Foundation",
            hg::win::WindowStyle::Default
        },
        spe::WindowManagerInterface::MainRenderTextureConfig{ {WINDOW_WIDTH, WINDOW_HEIGHT} },
        spe::WindowManagerInterface::TimingConfig{
            spe::FrameRate{FRAME_RATE},
            spe::PREVENT_BUSY_WAIT_ON,
            spe::VSYNC_OFF
        });
    winMgr->setStopIfCloseClicked(true);
    ctx->attachAndOwnComponent(std::move(winMgr));

    return ctx;
}

int main(int argc, char* argv[]) try {
    auto ctx = CreateGameContext();
    
    ctx->runFor(-1);

    return EXIT_SUCCESS;
}
catch (const std::exception& ex) {
    HG_LOG_ERROR("asdf", "Error: {}", ex.what());
}
