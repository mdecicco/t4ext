#include <events/Keyboard.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    KeyboardListener::KeyboardListener(Callback<void, KeyboardEvent*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    KeyboardListener::~KeyboardListener() {
        delete m_callback;
    }

    void KeyboardListener::execute(KeyboardEvent* log) {
        m_callback->call(log);
    }

    KeyboardEventType::KeyboardEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    KeyboardEventType::~KeyboardEventType() {
        KeyboardListener* n = m_listeners;
        while (n) {
            KeyboardListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 KeyboardEventType::createListener(Callback<void, KeyboardEvent*>& callback) {
        KeyboardListener* listener = new KeyboardListener(callback);
        listener->m_id = m_nextListenerId;

        if (m_lastListener) {
            m_lastListener->m_next = listener;
            listener->m_last = m_lastListener;
            m_lastListener = listener;
        } else {
            m_listeners = m_lastListener = listener;
        }

        m_listenerMap[m_nextListenerId] = listener;
        return m_nextListenerId++;
    }

    void KeyboardEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        KeyboardListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = nullptr;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void KeyboardEventType::notifyListeners(KeyboardEvent* log) {
        KeyboardListener* n = m_listeners;
        while (n) {
            n->execute(log);
            n = n->m_next;
        }
    }

    void KeyboardEventType::bind() {
        IScriptAPI* api = gClient::Get()->getAPI();
        api->beginNamespace("t4");

        DataType* k = api->bind<utils::KeyboardKey>("Key");
        k->bindEnumValue("None", utils::KeyboardKey::KEY_NONE);
        k->bindEnumValue("Num0", utils::KeyboardKey::KEY_0);
        k->bindEnumValue("Num1", utils::KeyboardKey::KEY_1);
        k->bindEnumValue("Num2", utils::KeyboardKey::KEY_2);
        k->bindEnumValue("Num3", utils::KeyboardKey::KEY_3);
        k->bindEnumValue("Num4", utils::KeyboardKey::KEY_4);
        k->bindEnumValue("Num5", utils::KeyboardKey::KEY_5);
        k->bindEnumValue("Num6", utils::KeyboardKey::KEY_6);
        k->bindEnumValue("Num7", utils::KeyboardKey::KEY_7);
        k->bindEnumValue("Num8", utils::KeyboardKey::KEY_8);
        k->bindEnumValue("Num9", utils::KeyboardKey::KEY_9);
        k->bindEnumValue("A", utils::KeyboardKey::KEY_A);
        k->bindEnumValue("B", utils::KeyboardKey::KEY_B);
        k->bindEnumValue("C", utils::KeyboardKey::KEY_C);
        k->bindEnumValue("D", utils::KeyboardKey::KEY_D);
        k->bindEnumValue("E", utils::KeyboardKey::KEY_E);
        k->bindEnumValue("F", utils::KeyboardKey::KEY_F);
        k->bindEnumValue("G", utils::KeyboardKey::KEY_G);
        k->bindEnumValue("H", utils::KeyboardKey::KEY_H);
        k->bindEnumValue("I", utils::KeyboardKey::KEY_I);
        k->bindEnumValue("J", utils::KeyboardKey::KEY_J);
        k->bindEnumValue("K", utils::KeyboardKey::KEY_K);
        k->bindEnumValue("L", utils::KeyboardKey::KEY_L);
        k->bindEnumValue("M", utils::KeyboardKey::KEY_M);
        k->bindEnumValue("N", utils::KeyboardKey::KEY_N);
        k->bindEnumValue("O", utils::KeyboardKey::KEY_O);
        k->bindEnumValue("P", utils::KeyboardKey::KEY_P);
        k->bindEnumValue("Q", utils::KeyboardKey::KEY_Q);
        k->bindEnumValue("R", utils::KeyboardKey::KEY_R);
        k->bindEnumValue("S", utils::KeyboardKey::KEY_S);
        k->bindEnumValue("T", utils::KeyboardKey::KEY_T);
        k->bindEnumValue("U", utils::KeyboardKey::KEY_U);
        k->bindEnumValue("V", utils::KeyboardKey::KEY_V);
        k->bindEnumValue("W", utils::KeyboardKey::KEY_W);
        k->bindEnumValue("X", utils::KeyboardKey::KEY_X);
        k->bindEnumValue("Y", utils::KeyboardKey::KEY_Y);
        k->bindEnumValue("Z", utils::KeyboardKey::KEY_Z);
        k->bindEnumValue("SingleQuote", utils::KeyboardKey::KEY_SINGLE_QUOTE);
        k->bindEnumValue("Backslash", utils::KeyboardKey::KEY_BACKSLASH);
        k->bindEnumValue("Comma", utils::KeyboardKey::KEY_COMMA);
        k->bindEnumValue("Equal", utils::KeyboardKey::KEY_EQUAL);
        k->bindEnumValue("Backtick", utils::KeyboardKey::KEY_BACKTICK);
        k->bindEnumValue("LeftBracket", utils::KeyboardKey::KEY_LEFT_BRACKET);
        k->bindEnumValue("Minus", utils::KeyboardKey::KEY_MINUS);
        k->bindEnumValue("Period", utils::KeyboardKey::KEY_PERIOD);
        k->bindEnumValue("RightBracket", utils::KeyboardKey::KEY_RIGHT_BRACKET);
        k->bindEnumValue("Semicolon", utils::KeyboardKey::KEY_SEMICOLON);
        k->bindEnumValue("Slash", utils::KeyboardKey::KEY_SLASH);
        k->bindEnumValue("Backspace", utils::KeyboardKey::KEY_BACKSPACE);
        k->bindEnumValue("Delete", utils::KeyboardKey::KEY_DELETE);
        k->bindEnumValue("End", utils::KeyboardKey::KEY_END);
        k->bindEnumValue("Enter", utils::KeyboardKey::KEY_ENTER);
        k->bindEnumValue("Escape", utils::KeyboardKey::KEY_ESCAPE);
        k->bindEnumValue("GraveAccent", utils::KeyboardKey::KEY_GRAVE_ACCENT);
        k->bindEnumValue("Home", utils::KeyboardKey::KEY_HOME);
        k->bindEnumValue("Insert", utils::KeyboardKey::KEY_INSERT);
        k->bindEnumValue("Menu", utils::KeyboardKey::KEY_MENU);
        k->bindEnumValue("PageDown", utils::KeyboardKey::KEY_PAGE_DOWN);
        k->bindEnumValue("PageUp", utils::KeyboardKey::KEY_PAGE_UP);
        k->bindEnumValue("Pause", utils::KeyboardKey::KEY_PAUSE);
        k->bindEnumValue("Space", utils::KeyboardKey::KEY_SPACE);
        k->bindEnumValue("Tab", utils::KeyboardKey::KEY_TAB);
        k->bindEnumValue("CapLock", utils::KeyboardKey::KEY_CAP_LOCK);
        k->bindEnumValue("NumLock", utils::KeyboardKey::KEY_NUM_LOCK);
        k->bindEnumValue("ScrollLock", utils::KeyboardKey::KEY_SCROLL_LOCK);
        k->bindEnumValue("F1", utils::KeyboardKey::KEY_F1);
        k->bindEnumValue("F2", utils::KeyboardKey::KEY_F2);
        k->bindEnumValue("F3", utils::KeyboardKey::KEY_F3);
        k->bindEnumValue("F4", utils::KeyboardKey::KEY_F4);
        k->bindEnumValue("F5", utils::KeyboardKey::KEY_F5);
        k->bindEnumValue("F6", utils::KeyboardKey::KEY_F6);
        k->bindEnumValue("F7", utils::KeyboardKey::KEY_F7);
        k->bindEnumValue("F8", utils::KeyboardKey::KEY_F8);
        k->bindEnumValue("F9", utils::KeyboardKey::KEY_F9);
        k->bindEnumValue("F10", utils::KeyboardKey::KEY_F10);
        k->bindEnumValue("F11", utils::KeyboardKey::KEY_F11);
        k->bindEnumValue("F12", utils::KeyboardKey::KEY_F12);
        k->bindEnumValue("F13", utils::KeyboardKey::KEY_F13);
        k->bindEnumValue("F14", utils::KeyboardKey::KEY_F14);
        k->bindEnumValue("F15", utils::KeyboardKey::KEY_F15);
        k->bindEnumValue("F16", utils::KeyboardKey::KEY_F16);
        k->bindEnumValue("F17", utils::KeyboardKey::KEY_F17);
        k->bindEnumValue("F18", utils::KeyboardKey::KEY_F18);
        k->bindEnumValue("F19", utils::KeyboardKey::KEY_F19);
        k->bindEnumValue("F20", utils::KeyboardKey::KEY_F20);
        k->bindEnumValue("F21", utils::KeyboardKey::KEY_F21);
        k->bindEnumValue("F22", utils::KeyboardKey::KEY_F22);
        k->bindEnumValue("F23", utils::KeyboardKey::KEY_F23);
        k->bindEnumValue("F24", utils::KeyboardKey::KEY_F24);
        k->bindEnumValue("LeftAlt", utils::KeyboardKey::KEY_LEFT_ALT);
        k->bindEnumValue("LeftControl", utils::KeyboardKey::KEY_LEFT_CONTROL);
        k->bindEnumValue("LeftShift", utils::KeyboardKey::KEY_LEFT_SHIFT);
        k->bindEnumValue("LeftSuper", utils::KeyboardKey::KEY_LEFT_SUPER);
        k->bindEnumValue("PrintScreen", utils::KeyboardKey::KEY_PRINT_SCREEN);
        k->bindEnumValue("RightAlt", utils::KeyboardKey::KEY_RIGHT_ALT);
        k->bindEnumValue("RightControl", utils::KeyboardKey::KEY_RIGHT_CONTROL);
        k->bindEnumValue("RightShift", utils::KeyboardKey::KEY_RIGHT_SHIFT);
        k->bindEnumValue("RightSuper", utils::KeyboardKey::KEY_RIGHT_SUPER);
        k->bindEnumValue("Down", utils::KeyboardKey::KEY_DOWN);
        k->bindEnumValue("Left", utils::KeyboardKey::KEY_LEFT);
        k->bindEnumValue("Right", utils::KeyboardKey::KEY_RIGHT);
        k->bindEnumValue("Up", utils::KeyboardKey::KEY_UP);
        k->bindEnumValue("Numpad0", utils::KeyboardKey::KEY_NUMPAD_0);
        k->bindEnumValue("Numpad1", utils::KeyboardKey::KEY_NUMPAD_1);
        k->bindEnumValue("Numpad2", utils::KeyboardKey::KEY_NUMPAD_2);
        k->bindEnumValue("Numpad3", utils::KeyboardKey::KEY_NUMPAD_3);
        k->bindEnumValue("Numpad4", utils::KeyboardKey::KEY_NUMPAD_4);
        k->bindEnumValue("Numpad5", utils::KeyboardKey::KEY_NUMPAD_5);
        k->bindEnumValue("Numpad6", utils::KeyboardKey::KEY_NUMPAD_6);
        k->bindEnumValue("Numpad7", utils::KeyboardKey::KEY_NUMPAD_7);
        k->bindEnumValue("Numpad8", utils::KeyboardKey::KEY_NUMPAD_8);
        k->bindEnumValue("Numpad9", utils::KeyboardKey::KEY_NUMPAD_9);
        k->bindEnumValue("NumpadAdd", utils::KeyboardKey::KEY_NUMPAD_ADD);
        k->bindEnumValue("NumpadDecimal", utils::KeyboardKey::KEY_NUMPAD_DECIMAL);
        k->bindEnumValue("NumpadDivide", utils::KeyboardKey::KEY_NUMPAD_DIVIDE);
        k->bindEnumValue("NumpadEnter", utils::KeyboardKey::KEY_NUMPAD_ENTER);
        k->bindEnumValue("NumpadEqual", utils::KeyboardKey::KEY_NUMPAD_EQUAL);
        k->bindEnumValue("NumpadMultiply", utils::KeyboardKey::KEY_NUMPAD_MULTIPLY);
        k->bindEnumValue("NumpadSubtract", utils::KeyboardKey::KEY_NUMPAD_SUBTRACT);

        DataType* ks = api->bind<KeyboardEvent::KeyState>("KeyState");
        ks->bindEnumValue("Pressed", KeyboardEvent::KeyState::Pressed);
        ks->bindEnumValue("Held", KeyboardEvent::KeyState::Held);
        ks->bindEnumValue("Released", KeyboardEvent::KeyState::Released);

        DataType* evt = api->bind<KeyboardEvent>("KeyboardEvent");
        evt->bind("key", &KeyboardEvent::m_key)->setFlags(DataTypeField::Flags::IsReadOnly);
        evt->bind("state", &KeyboardEvent::m_state)->setFlags(DataTypeField::Flags::IsReadOnly);

        api->bind("addKeyboardListener", +[](Callback<void, KeyboardEvent*>& cb) {
            return utils::Singleton<KeyboardEventType>::Get()->createListener(cb);
        })->setArgNames({ "listener" });

        api->bind("removeKeyboardListener", +[](u32 listenerId) {
            utils::Singleton<KeyboardEventType>::Get()->removeListener(listenerId);
        })->setArgNames({ "listenerId" });
        api->endNamespace();
    }

    bool KeyboardEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    KeyboardEvent* KeyboardEventType::createEvent(utils::KeyboardKey key, KeyboardEvent::KeyState state) {
        return new KeyboardEvent(this, key, state);
    }
    
    void KeyboardEventType::destroyEvent(KeyboardEvent* event) {
        delete event;
    }
    
    KeyboardEvent::KeyboardEvent(KeyboardEventType* tp, utils::KeyboardKey key, KeyState state) : IEvent(tp) {
        m_key = key;
        m_state = state;
    }

    KeyboardEvent::~KeyboardEvent() {
    }

    void KeyboardEvent::process(IScriptAPI* api) {
        ((KeyboardEventType*)m_type)->notifyListeners(this);
        ((KeyboardEventType*)m_type)->destroyEvent(this);
    }
};