/*
Copyright(c) 2016-2019 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ==================
#include <vector>
#include <memory>
#include <string>
#include "../Core/EngineDefs.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Light.h"
#include "Components/AudioSource.h"
#include "Components/Collider.h"
#include "Components/Constraint.h"
#include "Components/RigidBody.h"
#include "Components/Renderable.h"
#include "Components/Script.h"
#include "Components/Skybox.h"
#include "Components/AudioListener.h"
#include "Component.h"
#include "Iteration.h"
#include "../Core/ISubsystem.h"
//=============================

namespace Spartan
{
	class Entity;
	class Light;
	class Input;
	class Profiler;

	enum Scene_State
	{
		Ticking,
		Idle,
		Request_Loading,
		Loading
	};

	class SPARTAN_CLASS World : public ISubsystem
	{
	public:
		World(Context* context);
		~World();

		//= ISubsystem ======================
		bool Initialize() override;
		void Tick(float delta_time) override;
		//===================================
		
		void Unload();
		bool SaveToFile(const std::string& filePath);
		bool LoadFromFile(const std::string& file_path);
		const auto& GetName() { return m_name; }

		//= Entities ===============================================================================
		std::shared_ptr<Entity>& EntityCreate();
		std::shared_ptr<Entity>& EntityAdd(const std::shared_ptr<Entity>& entity);
		bool EntityExists(const std::shared_ptr<Entity>& entity);
		void EntityRemove(const std::shared_ptr<Entity>& entity);	
		std::vector<std::shared_ptr<Entity>> EntityGetRoots();
		const std::shared_ptr<Entity>& EntityGetByName(const std::string& name);
		const std::shared_ptr<Entity>& EntityGetById(uint32_t id);
		const auto& EntityGetAll()	{ return m_entities_primary; }
		auto EntityGetCount()		{ return static_cast<uint32_t>(m_entities_primary.size()); }
		//==========================================================================================

        //= COMPONENT MANAGERS ========================
        template <typename T>
        std::shared_ptr<ComponentManager<T>> GetComponentManager();
        
        template <typename Func>
        void IterateManagers(Func f);
        //===================================

	private:
		//= COMMON ENTITY CREATION ========================
		std::shared_ptr<Entity>& CreateSkybox();
		std::shared_ptr<Entity> CreateCamera();
		std::shared_ptr<Entity>& CreateDirectionalLight();
		//================================================

		// Double-buffered actors
		std::vector<std::shared_ptr<Entity>> m_entities_primary;
		std::vector<std::shared_ptr<Entity>> m_entities_secondary;

        std::unordered_map<ComponentType, std::shared_ptr<BaseComponentManager>> m_components_managers;

		std::shared_ptr<Entity> m_entity_empty;
		Input* m_input;
		Profiler* m_profiler;
		bool m_wasInEditorMode;
		bool m_isDirty;
		Scene_State m_state;
		std::string m_name;
	};

    template<typename T>
    inline std::shared_ptr<ComponentManager<T>> World::GetComponentManager()
    {
        ComponentType type = IComponent::TypeToEnum<T>();

        std::shared_ptr<BaseComponentManager> manager = m_components_managers[type];

        return std::static_pointer_cast<ComponentManager<T>>(manager);
    }

    template<typename Func>
    inline void World::IterateManagers(Func f)
    {
        Spartan::for_each(std::tie(Transform(), AudioSource(), AudioListener(),
            Constraint(), Collider(), RigidBody(), Light(),
            Renderable(), Script(), Skybox(), Camera()), [&](auto type)
        {
              auto manager = this->GetComponentManager<decltype(type)>();
              f(manager);
        });
    }
}
