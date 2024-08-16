
#include <Hobgoblin/HGExcept.hpp>

#include "Context_factory.hpp"
#include "Engine.hpp"

int InitializeAndRunClient() {
    auto ctx = CreateBasicClientContext();
    return ctx->runFor(-1);
}

int InitializeAndRunServer(int argc, char* argv[]) {
    // clang-format off
    const ServerGameParams params{
        .playerCount = 5,
        .portNumber  = 8888
    };
    auto ctx = CreateServerContext(params);
    return ctx->runFor(-1);
    // clang-format on
}

int main(int argc, char* argv[]) try {
    if (argc <= 1) {
        return InitializeAndRunClient();
    } else {
        return InitializeAndRunServer(argc, argv);
    }
} catch (const hg::TracedException& ex) {
    HG_LOG_FATAL(LOG_ID, "Traced exception caught: {}", ex.getFormattedDescription());
    return EXIT_FAILURE;
} catch (const std::exception& ex) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: {}", ex.what());
    return EXIT_FAILURE;
}
