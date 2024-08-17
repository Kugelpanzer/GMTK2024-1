
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include "Context_factory.hpp"
#include "Engine.hpp"

int InitializeAndRunDebug() {
    auto ctx = CreateBasicClientContext();

    const ServerGameParams serverParams{
        .playerCount = 2,
        .portNumber  = 8888
    };
    ctx->attachChildContext(CreateServerContext(serverParams));
    ctx->startChildContext(-1);

    const int status = ctx->runFor(-1);
    HG_LOG_INFO(LOG_ID, "Main context stopped with exit code {}.", status);
    return status;
}

int InitializeAndRunClient() {
    auto ctx = CreateBasicClientContext();
    const int status = ctx->runFor(-1);
    HG_LOG_INFO(LOG_ID, "Main context stopped with exit code {}.", status);
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
    HG_LOG_INFO(LOG_ID, "Main context stopped with exit code {}.", status);
    return status;
    // clang-format on
}

int main(int argc, char* argv[]) try {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);
    RN_IndexHandlers();

    if (argc <= 1) {
        return InitializeAndRunDebug();
    } else {
        const std::string mode = argv[1];
        if (mode == "client") {
            return InitializeAndRunClient();
        } else if (mode == "server") {
            return InitializeAndRunServer(argc, argv);
        } else {
           HG_LOG_ERROR(LOG_ID, "Unknown mode provided ({}), exiting.", mode);
           return EXIT_FAILURE;
        }
    }
} catch (const hg::TracedException& ex) {
    HG_LOG_FATAL(LOG_ID, "Traced exception caught: {}", ex.getFormattedDescription());
    return EXIT_FAILURE;
} catch (const std::exception& ex) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: {}", ex.what());
    return EXIT_FAILURE;
}
