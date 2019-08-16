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
#include "../Core/EngineDefs.h"
#include "Components/IComponent.h"
#include <memory>
#include <vector>
#include <iostream>
#include <functional>
//================================

namespace Spartan
{
    template <typename T>
	struct SPARTAN_CLASS ComponentData
	{
		unsigned int mSize = 1;
		std::array<std::shared_ptr<T>, 1024> mData;
    };

	// Simple unsigned int (syntactical sugar) that refers to the index of a component in the componentData array
	typedef unsigned int ComponentInstance;

	class BaseComponentManager 
	{
	public:
		BaseComponentManager() = default;
		virtual ~BaseComponentManager() = default;
			
		BaseComponentManager(const BaseComponentManager&) = default;
		BaseComponentManager& operator=(const BaseComponentManager&) = default;
			
		BaseComponentManager(BaseComponentManager&&) = default;
		BaseComponentManager& operator=(BaseComponentManager&&) = default;
	};

	template<typename T>
	class SPARTAN_CLASS ComponentManager : public BaseComponentManager
	{
	public:
		ComponentType m_type = IComponent::TypeToEnum<T>();

	public:
        ComponentManager() = default;

		void AddComponent(uint32_t entityID, std::shared_ptr<T>& component);

        std::shared_ptr<T> GetComponent(uint32_t entityID);
        std::shared_ptr<T> GetComponentByID(uint32_t entityID, uint32_t componentID);
        std::vector<std::shared_ptr<T>> GetComponents(uint32_t entityID);

	    void RemoveComponent(uint32_t entityID);
        void RemoveComponentByID(uint32_t entituID, uint32_t componentID);

		void Iterate(std::function<void(std::shared_ptr<T>)> func);

	private:
		ComponentData<T> mComponentData;
			
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, ComponentInstance>> mEntityMap;
		std::unordered_map<ComponentInstance, uint32_t> mInstanceMap;
	};
		
	template<typename T>
	inline void ComponentManager<T>::AddComponent(uint32_t entityID, std::shared_ptr<T>& component)
	{
		ComponentInstance instance = mComponentData.mSize;
		mComponentData.mData[instance] = component;
			
		mEntityMap[entityID][component->GetId()] = instance;
		mInstanceMap[instance] = entityID;
			
        mComponentData.mSize++;
	}
		
	template<typename T>
	inline std::shared_ptr<T> ComponentManager<T>::GetComponent(uint32_t entityID)
	{
        std::unordered_map<uint32_t, ComponentInstance> inst = mEntityMap[entityID];
			
		if (!inst.empty())
		{
			return mComponentData.mData[inst.begin()->second];
		}

        return nullptr;
	}

    template<typename T>
    inline std::shared_ptr<T> ComponentManager<T>::GetComponentByID(uint32_t entityID, uint32_t componentID)
    {
        auto inst = mEntityMap[entityID];

        if (!inst.count(componentID) > 0)
        {
            ComponentInstance i = inst[componentID];
            return mComponentData.mData[i];
        }

        return nullptr;
    }

    template<typename T>
    inline std::vector<std::shared_ptr<T>> ComponentManager<T>::GetComponents(uint32_t entityID)
    {
        auto inst = mEntityMap[entityID];

        std::vector<std::shared_ptr<T>> components;
        if (!inst.empty())
        {
            for (auto& c : inst)
            {
                components.push_back(mComponentData.mData[c.second]);
            }
        }

        return components;
    }
		
	template<typename T>
	inline void ComponentManager<T>::RemoveComponent(uint32_t entityID)
	{
       std::unordered_map<uint32_t, ComponentInstance> inst = mEntityMap[entityID];

        if (!inst.empty())
        {
            ComponentInstance lastComponent = mComponentData.mSize - 1;
            mComponentData.mData[mEntityMap[entityID].begin()->second] = mComponentData.mData[lastComponent];
            mEntityMap[mInstanceMap[lastComponent]][mComponentData.mData[lastComponent]->GetId()] = componentID;
            mComponentData.mData[lastComponent] = nullptr;
            mEntityMap[entityID].erase(mEntityMap[entityID].begin());

            mComponentData.mSize--;
        }
    }

    template<typename T>
    inline void ComponentManager<T>::RemoveComponentByID(uint32_t entityID, uint32_t componentID)
    {
        std::unordered_map<uint32_t, ComponentInstance> inst = mEntityMap[entityID];

        if (inst.count(componentID) > 0)
        {
            ComponentInstance lastComponent = mComponentData.mSize - 1;
            mComponentData.mData[inst[componentID]] = mComponentData.mData[lastComponent];
            mEntityMap[mInstanceMap[lastComponent]][mComponentData.mData[lastComponent]->GetId()] = componentID;
            mComponentData.mData[lastComponent] = nullptr;
            mEntityMap[entityID].erase(componentID);

            mComponentData.mSize--;
        }
    }
		
	template<typename T>
    inline void ComponentManager<T>::Iterate(std::function<void(std::shared_ptr<T>)> func)
	{
		for (uint32_t i = 1; i < mComponentData.mSize; i++)
		{
			func(mComponentData.mData[i]);
		}
	}

	class SPARTAN_CLASS ComponentMask 
	{
	public:
		unsigned int mask = 0;

	public:
		void AddComponent(ComponentType type) 
		{
			mask |= (1 << (int)type);
		}

		void RemoveComponent(ComponentType type) 
		{
			mask &= ~(1 << (int)type);
		}
	};
}
