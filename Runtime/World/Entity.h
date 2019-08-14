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

//= INCLUDES =====================
#include <vector>
#include "../Core/EventSystem.h"
#include "World.h"
#include "../Core/Context.h"
//================================

namespace Spartan
{
	class Context;
	class Transform;
	class Renderable;
	#define VALIDATE_COMPONENT_TYPE(T) static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent")

	class SPARTAN_CLASS Entity : public Spartan_Object, public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(Context* context);
		~Entity();

		void Initialize(Transform* transform);
		void Clone();

		//==========================
		void Start();
		void Stop();
		void Tick(float delta_time);
		//==========================

		void Serialize(FileStream* stream);
		void Deserialize(FileStream* stream, Transform* parent);

		//= PROPERTIES ===================================================================================================
		const std::string& GetName() const								{ return m_name; }
		void SetName(const std::string& name)							{ m_name = name; }

		bool IsActive() const											{ return m_is_active; }
        void SetActive(const bool active);

		bool IsVisibleInHierarchy() const								{ return m_hierarchy_visibility; }
		void SetHierarchyVisibility(const bool hierarchy_visibility)	{ m_hierarchy_visibility = hierarchy_visibility; }
		//================================================================================================================

		// Adds a component of ComponentType 
		std::shared_ptr<IComponent> AddComponent(ComponentType type);

		// Adds a component of type T
		template <class T>
		constexpr std::shared_ptr<T> AddComponent()
		{
			VALIDATE_COMPONENT_TYPE(T);
			const ComponentType type = IComponent::TypeToEnum<T>();

			// Return component in case it already exists while ignoring Script components (they can exist multiple times)
			if (HasComponent(type) && type != ComponentType_Script)
				return GetComponent<T>();

			// Add component
            m_component_mask.AddComponent(type);
            m_components.emplace_back
			(	
				std::make_shared<T>
				(
					m_context,
					this,
					GetTransform_PtrRaw()
				)
			);

			auto new_component = std::static_pointer_cast<T>(m_components.back());
			new_component->SetType(IComponent::TypeToEnum<T>());
			new_component->OnInitialize();

            m_id_to_type[new_component->GetId()] = type;

			// Caching of rendering performance critical components
			if constexpr (std::is_same<T, Renderable>::value)
			{
				m_renderable = static_cast<Renderable*>(new_component.get());
			}

            auto scene = m_context->GetSubsystem<World>();
            auto manager = scene->GetComponentManager<T>();
            manager->AddComponent(this->GetId(), new_component);

			// Make the scene resolve
			FIRE_EVENT(Event_World_Resolve);

			return new_component;
		}

		// Returns a component of type T (if it exists)
		template <class T>
		constexpr std::shared_ptr<T> GetComponent()
		{
			VALIDATE_COMPONENT_TYPE(T);
			const ComponentType type = IComponent::TypeToEnum<T>();

            auto scene = m_context->GetSubsystem<World>();
            return scene->GetComponentManager<T>()->GetComponent(GetId());
		}

		// Returns any components of type T (if they exist)
		template <class T>
		constexpr std::vector<std::shared_ptr<T>> GetComponents()
		{
			VALIDATE_COMPONENT_TYPE(T);
			const ComponentType type = IComponent::TypeToEnum<T>();

            auto scene = m_context->GetSubsystem<World>();
            auto components = scene->GetComponentManager<T>()->GetComponents(GetId());

            return components;
		}
		
		// Checks if a component of ComponentType exists
		bool HasComponent(const ComponentType type) 
		{ 
            return m_component_mask.mask & (1 << (int)type);
		}

		// Checks if a component of type T exists
		template <class T>
		constexpr bool HasComponent() 
		{ 
			VALIDATE_COMPONENT_TYPE(T);
			return HasComponent(IComponent::TypeToEnum<T>()); 
		}

		// Removes a component of type T (if it exists)
		template <class T>
		constexpr void RemoveComponent()
		{
			VALIDATE_COMPONENT_TYPE(T);
			const ComponentType type = IComponent::TypeToEnum<T>();

			for (auto it = m_components.begin(); it != m_components.end();)
			{
				auto component = *it;
				if (component->GetType() == type)
				{
					component->OnRemove();
					component.reset();
					it = m_components.erase(it);
				}
				else
				{
					++it;
				}
			}

            auto scene = m_context->GetSubsystem<World>();
            scene->GetComponentManager<T>()->RemoveComponent(GetId());

			// Make the scene resolve
			FIRE_EVENT(Event_World_Resolve);
		}

		void RemoveComponentById(uint32_t id);
        const auto& Entity::GetAllComponents() const
        {
            auto scene = m_context->GetSubsystem<World>();
            std::vector<std::shared_ptr<IComponent>> components;

            scene->IterateManagers([&](auto& manager)
            {
                auto mcomponents = manager->GetComponents(GetId());
                components.insert(components.end(), mcomponents.begin(), mcomponents.end());
            });

            return components;
        }

		// Direct access for performance critical usage (not safe)
		Transform* GetTransform_PtrRaw() const		{ return m_transform; }
		Renderable* GetRenderable_PtrRaw() const	{ return m_renderable; }
		std::shared_ptr<Entity> GetPtrShared()		{ return shared_from_this(); }

	private:
		std::string m_name			= "Entity";
		bool m_is_active			= true;
		bool m_hierarchy_visibility	= true;
		// Caching of performance critical components
		Transform* m_transform		= nullptr;
		Renderable* m_renderable	= nullptr;

		// Components
		std::vector<std::shared_ptr<IComponent>> m_components;
		std::shared_ptr<Entity> m_component_empty;

        // Component Managment
        ComponentMask m_component_mask;
        std::unordered_map<uint32_t, ComponentType> m_id_to_type;

		// Misc
		Context* m_context;
	};
}
