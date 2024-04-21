#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CLevel;

    class LevelCreateListener {
        public:
            LevelCreateListener(Callback<void, CLevel*>& callback);
            virtual ~LevelCreateListener();

            void execute(CLevel* level);

        protected:
            friend class LevelCreateEventType;

            u32 m_id;
            LevelCreateListener* m_next;
            LevelCreateListener* m_last;
            Callback<void, CLevel*>* m_callback;
    };
    
    class LevelCreateEventType;
    class LevelCreateEvent : public IEvent {
        public:
            LevelCreateEvent(LevelCreateEventType* tp, CLevel* level);
            virtual ~LevelCreateEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CLevel* m_level;
    };
    
    class LevelCreateEventType : public IEventType {
        public:
            LevelCreateEventType();
            virtual ~LevelCreateEventType();

            u32 createListener(Callback<void, CLevel*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CLevel* level);

            virtual bool canProduceEvents();
            LevelCreateEvent* createEvent(CLevel* level);
            void destroyEvent(LevelCreateEvent* event);
        
        protected:
            u32 m_nextListenerId;
            LevelCreateListener* m_listeners;
            LevelCreateListener* m_lastListener;
            robin_hood::unordered_map<u32, LevelCreateListener*> m_listenerMap;
    };
};