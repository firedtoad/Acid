#pragma once

#include "Animations/Joint/Joint.hpp"
#include "Serialized/Node.hpp"

namespace acid
{
	class ACID_EXPORT SkeletonLoader
	{
	private:
		std::shared_ptr<Node> m_armatureData;

		std::vector<std::string> m_boneOrder;

		int m_jointCount;
		JointData *m_headJoint;
	public:
		SkeletonLoader(const std::shared_ptr<Node> &libraryControllers, const std::vector<std::string> &boneOrder);

		~SkeletonLoader();

		int GetJointCount() const { return m_jointCount; }

		JointData *GetHeadJoint() const { return m_headJoint; }
	private:
		JointData *LoadJointData(std::shared_ptr<Node> &jointNode, const bool &isRoot);

		JointData *ExtractMainJointData(std::shared_ptr<Node> &jointNode, const bool &isRoot);

		int GetBoneIndex(const std::string &name);
	};
}
