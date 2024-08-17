
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include "Context_factory.hpp"
#include "Engine.hpp"

int InitializeAndRunClient() {
    auto ctx = CreateBasicClientContext();
    const int status = ctx->runFor(-1);
    HG_LOG_INFO(LOG_ID, "Main context stopped (status = {}).", status);
    return status;
}

int InitializeAndRunServer(int argc, char* argv[]) {
    // clang-format off
    const ServerGameParams params{
        .playerCount = 5,
        .portNumber  = 8888
    };
    auto ctx = CreateServerContext(params);
    const int status = ctx->runFor(-1);
    HG_LOG_INFO(LOG_ID, "Main context stopped (status = {}).", status);
    return status;
    // clang-format on
}

int main(int argc, char* argv[]) try {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);
    RN_IndexHandlers();

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
