#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CLevel;

    class LevelDestroyListener {
        public:
            LevelDestroyListener(Callback<void, CLevel*>& callback);
            virtual ~LevelDestroyListener();

            void execute(CLevel* level);

        protected:
            friend class LevelDestroyEventType;

            u32 m_id;
            LevelDestroyListener* m_next;
            LevelDestroyListener* m_last;
            Callback<void, CLevel*>* m_callback;
    };
    
    class LevelDestroyEventType;
    class LevelDestroyEvent : public IEvent {
        public:
            LevelDestroyEvent(LevelDestroyEventType* tp, CLevel* level);
            virtual ~LevelDestroyEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CLevel* m_level;
    };
    
    class LevelDestroyEventType : public IEventType {
        public:
            LevelDestroyEventType();
            virtual ~LevelDestroyEventType();

            u32 createListener(Callback<void, CLevel*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CLevel* level);

            virtual bool canProduceEvents();
            LevelDestroyEvent* createEvent(CLevel* level);
            void destroyEvent(LevelDestroyEvent* event);
        
        protected:
            u32 m_nextListenerId;
            LevelDestroyListener* m_listeners;
            LevelDestroyListener* m_lastListener;
            robin_hood::unordered_map<u32, LevelDestroyListener*> m_listenerMap;
    };
};