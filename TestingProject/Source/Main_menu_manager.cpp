#include "Main_menu_manager.hpp"

#include "Context_factory.hpp"

#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <RmlUi/Core.h>

#include <string>

namespace {
template <class T>
bool RegisterModel(Rml::DataModelConstructor& aDataModelCtor) {
    return false;
}

struct MainMenuModel {
    bool debugMode = false;
};

template <>
bool RegisterModel<MainMenuModel>(Rml::DataModelConstructor& aDataModelCtor) {
    auto handle = aDataModelCtor.RegisterStruct<MainMenuModel>();
    if (handle) {
        handle.RegisterMember<bool>("debugMode", &MainMenuModel::debugMode);
    }
    return static_cast<bool>(handle);
}

class FormSubmitEventListener {
public:
    virtual void onFormSubmitted(const Rml::String& aPlayerName,
                                 const Rml::String& aServerIp,
                                 const Rml::String& aServerPort,
                                 const Rml::String& aLocalPort) = 0;
};

class RmlUiEventListener : public Rml::EventListener {
public:
    RmlUiEventListener(const Rml::String&       aValue,
                       Rml::Element*            aElement,
                       FormSubmitEventListener& aFormSubmitEventListener)
        : _value{aValue}
        , _element{aElement}
        , _formSubmitEventListener{aFormSubmitEventListener} {}

    void ProcessEvent(Rml::Event& aEvent) override {
        if (_value == "submit_form") {
            const auto playerName = aEvent.GetParameter<Rml::String>("playerName", "");
            const auto serverIp   = aEvent.GetParameter<Rml::String>("serverIp", "");
            const auto serverPort = aEvent.GetParameter<Rml::String>("serverPort", "");
            const auto localPort  = aEvent.GetParameter<Rml::String>("localPort", "");
            _formSubmitEventListener.onFormSubmitted(playerName, serverIp, serverPort, localPort);
        }
    }

    void OnDetach(Rml::Element* element) override {
        delete this;
    }

private:
    Rml::String              _value;
    Rml::Element*            _element;
    FormSubmitEventListener& _formSubmitEventListener;
};

class EventListenerInstancer : public Rml::EventListenerInstancer {
public:
    EventListenerInstancer(FormSubmitEventListener& aFormSubmitEventListener)
        : _formSubmitEventListener{aFormSubmitEventListener} {}

    Rml::EventListener* InstanceEventListener(const Rml::String& value, Rml::Element* element) override {
        return new RmlUiEventListener(value, element, _formSubmitEventListener);
    }

private:
    FormSubmitEventListener& _formSubmitEventListener;
};
} // namespace

class MainMenuManager::Impl
    : hg::util::NonCopyable
    , hg::util::NonMoveable
    , FormSubmitEventListener {
public:
#define CTX   _super.ctx
#define CCOMP _super.ccomp

    Impl(MainMenuManager& aSuper)
        : _super{aSuper}
        , _eventListenerInstancer{*this} {
        _init();
    }

    ~Impl() {
        Rml::Factory::RegisterEventListenerInstancer(nullptr);
        if (_document) {
            CCOMP<MWindow>().getGUIContext().UnloadDocument(_document);
            _document = nullptr;
        }
    }

    void onFormSubmitted(const Rml::String& aPlayerName,
                         const Rml::String& aServerIp,
                         const Rml::String& aServerPort,
                         const Rml::String& aLocalPort) override {
        HG_LOG_INFO(
            LOG_ID,
            "Connect form submitted: playerName: {}, serverIp: {}, serverPort: {}, localPort: {}",
            aPlayerName,
            aServerIp,
            aServerPort,
            aLocalPort);

        ClientGameParams params{.playerName      = aPlayerName,
                                .hostIpAddress   = aServerIp,
                                .hostPortNumber  = static_cast<std::uint16_t>(std::stoi(aServerPort)),
                                .localPortNumber = static_cast<std::uint16_t>(
                                    (aLocalPort == "auto") ? 0 : std::stoi(aLocalPort))};

        _super._clientGameParams = std::make_unique<ClientGameParams>(std::move(params));
        _super._timeToDie        = true;
    }

    void eventDrawGUI() {
        const bool debugMode = CTX().hasChildContext();
        if (_mainMenuModel.debugMode != debugMode) {
            _mainMenuModel.debugMode = debugMode;
            _dataModelHandle.DirtyAllVariables();
        }
    }

private:
    MainMenuManager& _super;

    EventListenerInstancer _eventListenerInstancer;

    Rml::ElementDocument* _document = nullptr;
    MainMenuModel         _mainMenuModel;
    Rml::DataModelHandle  _dataModelHandle;

    bool _documentVisible = false;

    std::optional<Rml::DataModelHandle> _setUpDataBinding(Rml::Context& aContext) {
        Rml::DataModelConstructor constructor = aContext.CreateDataModel("mainmenu-model");
        if (!constructor) {
            return {};
        }

        try {
            // clang-format off
            #define THROW_IF_FALSE(_val_)                                                              \
                do {                                                                                   \
                    if (!(_val_)) {                                                                    \
                        HG_THROW_TRACED(hg::TracedRuntimeError, 0, "Expression failed: '{}'", #_val_); \
                    }                                                                                  \
                } while (0)

            THROW_IF_FALSE(RegisterModel<MainMenuModel>(constructor));
        
            THROW_IF_FALSE(constructor.Bind("debugMode", &_mainMenuModel.debugMode));
            // THROW_IF_FALSE(constructor.Bind("localName",    &_lobbyModel.localName));
            // THROW_IF_FALSE(constructor.Bind("isAuthorized", &_lobbyModel.isAuthorized));
            //THROW_IF_FALSE(constructor.BindEventCallback("LockIn",   &Impl::_onEvent,   this));
            // clang-format on
        } catch (const hg::TracedRuntimeError& ex) {
            HG_LOG_ERROR(LOG_ID, "Could not bind data model: {}", ex.getErrorMessage());
            return {};
        }

        return constructor.GetModelHandle();
    }

    void _init() {
        SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(CTX(), headless == false);

        auto& guiContext = CCOMP<MWindow>().getGUIContext();
        auto  handle     = _setUpDataBinding(guiContext);
        HG_HARD_ASSERT(handle.has_value()); // TODO
        _dataModelHandle = *handle;

        hg::rml::PreprocessRcssFile("assets/main_menu.rcss.fp");
        Rml::Factory::RegisterEventListenerInstancer(&_eventListenerInstancer);
        _document = guiContext.LoadDocument("assets/main_menu.rml");

        if (_document) {
            _document->Show();
            _documentVisible = true;
            HG_LOG_INFO(LOG_ID, "RMLUI Document loaded successfully.");
        } else {
            HG_LOG_ERROR(LOG_ID, "RMLUI Document could not be loaded.");
        }
    }

#undef CCOMP
#undef CTX
};

MainMenuManager::MainMenuManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : spe::NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "MainMenuManager"}
    , _impl{std::make_unique<Impl>(*this)} {}

MainMenuManager::~MainMenuManager() = default;

void MainMenuManager::_eventPreUpdate() {
    if (_timeToDie && _clientGameParams) {
        _impl.reset();
        AttachGameplayManagers(ctx(), *_clientGameParams);

        spe::DetachStatus detachStatus;
        auto self = ctx().detachComponent<MainMenuManagerInterface>(&detachStatus);
        HG_HARD_ASSERT(detachStatus == spe::DetachStatus::OK && self != nullptr);
        self.reset(); // WARNING: `this` will be deleted!
    }
}

void MainMenuManager::_eventDrawGUI() {
    _impl->eventDrawGUI();
}
