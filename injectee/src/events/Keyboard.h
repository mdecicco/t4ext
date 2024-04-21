#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <utils/Input.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class KeyboardEvent;

    class KeyboardListener {
        public:
            KeyboardListener(Callback<void, KeyboardEvent*>& callback);
            virtual ~KeyboardListener();

            void execute(KeyboardEvent* log);

        protected:
            friend class KeyboardEventType;

            u32 m_id;
            KeyboardListener* m_next;
            KeyboardListener* m_last;
            Callback<void, KeyboardEvent*>* m_callback;
    };
    
    class KeyboardEventType;
    class KeyboardEvent : public IEvent {
        public:
            enum KeyState {
                Pressed,
                Held,
                Released
            };

            KeyboardEvent(KeyboardEventType* tp, utils::KeyboardKey key, KeyState state);
            virtual ~KeyboardEvent();

            virtual void process(IScriptAPI* api);         

        protected:
            friend class KeyboardEventType;
            utils::KeyboardKey m_key;
            KeyState m_state;
    };
    
    class KeyboardEventType : public IEventType {
        public:
            KeyboardEventType();
            virtual ~KeyboardEventType();

            u32 createListener(Callback<void, KeyboardEvent*>& callback);
            void removeListener(u32 id);
            void notifyListeners(KeyboardEvent* log);

            void bind();
            virtual bool canProduceEvents();
            KeyboardEvent* createEvent(utils::KeyboardKey key, KeyboardEvent::KeyState state);
            void destroyEvent(KeyboardEvent* event);
        
        protected:
            u32 m_nextListenerId;
            KeyboardListener* m_listeners;
            KeyboardListener* m_lastListener;
            robin_hood::unordered_map<u32, KeyboardListener*> m_listenerMap;
    };
};