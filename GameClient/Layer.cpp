#include "pch.h"
#include "Layer.h"


Layer::Layer()
{
}

Layer::~Layer()
{
}



void Layer::AddObject(Ptr<GameObject> _Object)
{
	m_vecParents.push_back(_Object);
	
	list<GameObject*> queue;

	queue.push_back(_Object.Get());

	while (!queue.empty())
	{
		GameObject* pObject = queue.front();
		queue.pop_front();

		pObject->m_LayerIdx = m_LayerIdx;

		for (size_t i = 0; i < pObject->m_vecChild.size(); ++i)
		{
			queue.push_back(pObject->m_vecChild[i].Get());
		}
	}
}

void Layer::Begin()
{
	for (size_t i = 0; i < m_vecParents.size(); ++i)
	{
		m_vecParents[i]->Begin();
	}
}

void Layer::Tick()
{
	for (size_t i = 0; i < m_vecParents.size(); ++i)
	{
		m_vecParents[i]->Tick();
	}
}

void Layer::FinalTick()
{
	vector<Ptr<GameObject>>::iterator iter = m_vecParents.begin();

	for (; iter != m_vecParents.end(); )
	{
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
			iter = m_vecParents.erase(iter);
		else
			++iter;
	}
}

void Layer::Render()
{
	for (size_t i = 0; i < m_vecParents.size(); ++i)
	{
		m_vecParents[i]->Render();
	}
}
