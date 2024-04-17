#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class UpdateListener {
        public:
            UpdateListener(ICallback<void>& callback);
            virtual ~UpdateListener();

            void execute();

        protected:
            friend class UpdateEventType;

            u32 m_id;
            UpdateListener* m_next;
            UpdateListener* m_last;
            ICallback<void>* m_callback;
    };
    
    class UpdateEventType;
    class UpdateEvent : public IEvent {
        public:
            UpdateEvent(UpdateEventType* tp);
            virtual ~UpdateEvent();

            virtual void process(IScriptAPI* api);
    };
    
    class UpdateEventType : public IEventType {
        public:
            UpdateEventType();
            virtual ~UpdateEventType();

            u32 createListener(ICallback<void>& callback);
            void removeListener(u32 id);
            void notifyListeners();

            virtual void bind(DataType* eventTp);
            virtual bool canProduceEvents();
            virtual IEvent* createEvent();
            virtual void destroyEvent(IEvent* event);
        
        protected:
            u32 m_nextListenerId;
            UpdateListener* m_listeners;
            UpdateListener* m_lastListener;
            robin_hood::unordered_map<u32, UpdateListener*> m_listenerMap;
            utils::PagedAllocator<UpdateEvent> m_allocator;
    };

    class EngineUpdateEventType : public UpdateEventType {};
    class EngineRenderEventType : public UpdateEventType {};
};