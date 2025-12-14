#pragma once

namespace Engine::UI::Managed {
    class UiManagedDrop {
        public:
        virtual ~UiManagedDrop(void) = default;
        virtual void Drop(void) = 0;
    };

    class UiManagedInit {
        public:
        virtual ~UiManagedInit(void) = default;
        virtual UiManagedInit& Init(void) = 0;
    };

    class UiManagedObject : public UiManagedDrop, public UiManagedInit {
        public:
        virtual ~UiManagedObject(void) = default;
        virtual UiManagedObject& Init(void) override = 0;
    };
}
