#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CLevel;

    class LevelSpawnListener {
        public:
            LevelSpawnListener(Callback<void, CLevel*>& callback);
            virtual ~LevelSpawnListener();

            void execute(CLevel* level);

        protected:
            friend class LevelSpawnEventType;

            u32 m_id;
            LevelSpawnListener* m_next;
            LevelSpawnListener* m_last;
            Callback<void, CLevel*>* m_callback;
    };
    
    class LevelSpawnEventType;
    class LevelSpawnEvent : public IEvent {
        public:
            LevelSpawnEvent(LevelSpawnEventType* tp, CLevel* level);
            virtual ~LevelSpawnEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CLevel* m_level;
    };
    
    class LevelSpawnEventType : public IEventType {
        public:
            LevelSpawnEventType();
            virtual ~LevelSpawnEventType();

            u32 createListener(Callback<void, CLevel*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CLevel* level);

            virtual bool canProduceEvents();
            LevelSpawnEvent* createEvent(CLevel* level);
            void destroyEvent(LevelSpawnEvent* event);
        
        protected:
            u32 m_nextListenerId;
            LevelSpawnListener* m_listeners;
            LevelSpawnListener* m_lastListener;
            robin_hood::unordered_map<u32, LevelSpawnListener*> m_listenerMap;
    };
};