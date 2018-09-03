#include "GeometryLoader.hpp"

#include "Animations/MeshAnimated.hpp"

namespace acid
{
	GeometryLoader::GeometryLoader(const std::shared_ptr<Node> &libraryGeometries, const std::vector<VertexSkinData *> &vertexWeights) :
		m_meshData(libraryGeometries->FindChild("geometry")->FindChild("mesh")),
		m_vertexWeights(vertexWeights),
		m_positionsList(std::vector<VertexAnimatedData *>()),
		m_uvsList(std::vector<Vector2>()),
		m_normalsList(std::vector<Vector3>()),
		m_vertices(std::vector<IVertex *>()),
		m_indices(std::vector<uint32_t>())
	{
		LoadVertices();
		LoadUvs();
		LoadNormals();
		AssembleVertices();
		RemoveUnusedVertices();

		for (auto &current : m_positionsList)
		{
			Vector3 position = current->GetPosition();
			Vector2 textures = m_uvsList.at(current->GetUvIndex());
			Vector3 normal = m_normalsList.at(current->GetNormalIndex());
			Vector3 tangent = current->GetAverageTangent();

			VertexSkinData *skin = current->GetSkinData();
			Vector3 jointIds = Vector3(skin->GetJointIds()[0], skin->GetJointIds()[1], skin->GetJointIds()[2]);
			Vector3 weights = Vector3(skin->GetWeights()[0], skin->GetWeights()[1], skin->GetWeights()[2]);

			VertexAnimated *vertex = new VertexAnimated(position, textures, normal, tangent, jointIds, weights);

			m_vertices.emplace_back(vertex);

			delete current;
		}
	}

	GeometryLoader::~GeometryLoader()
	{
	}

	void GeometryLoader::LoadVertices()
	{
		std::string positionsSource = m_meshData->FindChild("vertices")->FindChild("input")->FindAttribute("source").substr(1);
		auto positionsData = m_meshData->FindChildWithAttribute("source", "id", positionsSource)->FindChild(
			"float_array");
		uint32_t positionsCount = std::stoi(positionsData->FindAttribute("count"));
		auto positionsRawData = FormatString::Split(positionsData->GetValue(), " ");

		for (uint32_t i = 0; i < positionsCount / 3; i++)
		{
			Vector4 position = Vector4(std::stof(positionsRawData[i * 3]), std::stof(positionsRawData[i * 3 + 1]), std::stof(positionsRawData[i * 3 + 2]), 1.0f);
			position = MeshAnimated::CORRECTION.Transform(position);
			VertexAnimatedData *newVertex = new VertexAnimatedData(m_positionsList.size(), position);
			newVertex->SetSkinData(m_vertexWeights[m_positionsList.size()]);
			m_positionsList.emplace_back(newVertex);
		}
	}

	void GeometryLoader::LoadUvs()
	{
		std::string uvsSource = m_meshData->FindChild("polylist")->FindChildWithAttribute("input", "semantic",
		                                                                                  "TEXCOORD")->FindAttribute(
			"source").substr(1);
		auto uvsData = m_meshData->FindChildWithAttribute("source", "id", uvsSource)->FindChild("float_array");
		uint32_t uvsCount = std::stoi(uvsData->FindAttribute("count"));
		auto uvsRawData = FormatString::Split(uvsData->GetValue(), " ");

		for (uint32_t i = 0; i < uvsCount / 2; i++)
		{
			Vector2 uv = Vector2(std::stof(uvsRawData[i * 2]), 1.0f - std::stof(uvsRawData[i * 2 + 1]));
			m_uvsList.emplace_back(uv);
		}
	}

	void GeometryLoader::LoadNormals()
	{
		std::string normalsSource = m_meshData->FindChild("polylist")->FindChildWithAttribute("input", "semantic",
		                                                                                      "NORMAL")->FindAttribute(
			"source").substr(1);
		auto normalsData = m_meshData->FindChildWithAttribute("source", "id", normalsSource)->FindChild("float_array");
		uint32_t normalsCount = std::stoi(normalsData->FindAttribute("count"));
		auto normalsRawData = FormatString::Split(normalsData->GetValue(), " ");

		for (uint32_t i = 0; i < normalsCount / 3; i++)
		{
			Vector3 normal = Vector3(std::stof(normalsRawData[i * 3]), std::stof(normalsRawData[i * 3 + 1]), std::stof(normalsRawData[i * 3 + 2]));
			normal = MeshAnimated::CORRECTION.Transform(normal);
			m_normalsList.emplace_back(normal);
		}
	}

	void GeometryLoader::AssembleVertices()
	{
		int32_t indexCount = m_meshData->FindChild("polylist")->FindChildren("input").size();
		auto indexRawData = FormatString::Split(m_meshData->FindChild("polylist")->FindChild("p")->GetValue(), " ");

		for (uint32_t i = 0; i < indexRawData.size() / indexCount; i++)
		{
			int32_t positionIndex = std::stoi(indexRawData[i * indexCount]);
			int32_t normalIndex = std::stoi(indexRawData[i * indexCount + 1]);
			int32_t uvIndex = std::stoi(indexRawData[i * indexCount + 2]);
			ProcessVertex(positionIndex, normalIndex, uvIndex);
		}
	}

	VertexAnimatedData *GeometryLoader::ProcessVertex(const int32_t &positionIndex, const int32_t &normalIndex, const int32_t &uvIndex)
	{
		auto currentVertex = m_positionsList[positionIndex];

		if (!currentVertex->IsSet())
		{
			currentVertex->SetUvIndex(uvIndex);
			currentVertex->SetNormalIndex(normalIndex);
			m_indices.emplace_back(positionIndex);
			return currentVertex;
		}
		else
		{
			return DealWithAlreadyProcessedVertex(currentVertex, uvIndex, normalIndex);
		}
	}

	VertexAnimatedData *GeometryLoader::DealWithAlreadyProcessedVertex(VertexAnimatedData *previousVertex, const int32_t &newUvIndex, const int32_t &newNormalIndex)
	{
		if (previousVertex->HasSameTextureAndNormal(newUvIndex, newNormalIndex))
		{
			m_indices.emplace_back(previousVertex->GetIndex());
			return previousVertex;
		}
		else
		{
			VertexAnimatedData *anotherVertex = nullptr;

			for (auto &position : m_positionsList)
			{
				if (position == previousVertex->GetDuplicateVertex())
				{
					anotherVertex = position;
					break;
				}
			}

			if (anotherVertex != nullptr)
			{
				return DealWithAlreadyProcessedVertex(anotherVertex, newUvIndex, newNormalIndex);
			}
			else
			{
				auto duplicateVertex = new VertexAnimatedData(m_positionsList.size(), previousVertex->GetPosition());
				duplicateVertex->SetUvIndex(newUvIndex);
				duplicateVertex->SetNormalIndex(newNormalIndex);
				duplicateVertex->SetSkinData(previousVertex->GetSkinData());
				previousVertex->SetDuplicateVertex(duplicateVertex);
				m_positionsList.emplace_back(duplicateVertex);
				m_indices.emplace_back(duplicateVertex->GetIndex());
				return duplicateVertex;
			}
		}
	}

	void GeometryLoader::RemoveUnusedVertices()
	{
		for (auto &vertex : m_positionsList)
		{
			vertex->AverageTangents();

			if (!vertex->IsSet())
			{
				vertex->SetUvIndex(0);
				vertex->SetNormalIndex(0);
			}
		}
	}
}
