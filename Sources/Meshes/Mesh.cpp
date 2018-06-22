#include "Mesh.hpp"

#include "Models/Shapes/ShapeCube.hpp"
#include "Models/Shapes/ShapeCylinder.hpp"
#include "Models/Shapes/ShapeDisk.hpp"
#include "Models/Shapes/ShapeRectangle.hpp"
#include "Models/Shapes/ShapeSphere.hpp"

namespace fl
{
	Mesh::Mesh(std::shared_ptr<Model> model) :
		IComponent(),
		m_model(model)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Update()
	{
		// TODO
		/*if (m_model == nullptr || m_model->GetAabb() == nullptr)
		{
			return;
		}

		auto aabb = GetGameObject()->GetComponent<ColliderAabb>();

		if (aabb != m_model->GetAabb())
		{
			GetGameObject()->RemoveComponent(aabb);
			GetGameObject()->AddComponent(m_model->GetAabb());
		}*/
	}

	void Mesh::Load(std::shared_ptr<LoadedValue> value)
	{
		TrySetModel(value->GetChild("Model")->GetString());
	}

	void Mesh::Write(std::shared_ptr<LoadedValue> destination)
	{
		destination->GetChild("Model", true)->SetString(m_model == nullptr ? "" : m_model->GetFilename());
	}

	void Mesh::TrySetModel(const std::string &filename)
	{
		if (filename.empty())
		{
			return;
		}

		// TODO: Modularize.
		auto split = FormatString::Split(filename, "_");

		if (!split.empty() && split[0] == "Cube")
		{
			m_model = ShapeCube::Resource(filename);
			return;
		}

		if (!split.empty() && split[0] == "Cylinder")
		{
			m_model = ShapeCylinder::Resource(filename);
			return;
		}

		if (!split.empty() && split[0] == "Disk")
		{
			m_model = ShapeDisk::Resource(filename);
			return;
		}

		if (!split.empty() && split[0] == "Rectangle")
		{
			m_model = ShapeRectangle::Resource(filename);
			return;
		}

		if (!split.empty() && split[0] == "Sphere")
		{
			m_model = ShapeSphere::Resource(filename);
			return;
		}

		m_model = Model::Resource(filename);
	}
}