﻿#pragma once

#include <vector>

#include "../devices/Display.hpp"
#include "../maths/vector2.hpp"
#include "../maths/vector4.hpp"
#include "../engine/Engine.hpp"
#include "../visual/idriver.hpp"
#include "../visual/driverconstant.hpp"

namespace Flounder
{
	/// <summary>
	/// A representation of a object this is rendered to a screen. This object is contained in a parent and has children.
	/// The screen object has a few values that allow for it to be positioned and scaled, along with other variables that are used when rendering.
	/// This class can be extended to create a representation for GUI textures, fonts, etc.
	/// </summary>
	class uiobject
	{
	private:
		uiobject *m_parent;
		std::vector<uiobject*> *m_children;

		bool m_visible;
		vector2 *m_position;
		vector2 *m_dimensions;
		vector2 *m_meshSize;
		vector4 *m_scissor;

		bool m_inScreenCoords;

		vector2 *m_screenPosition;
		vector2 *m_screenDimensions;
		vector2 *m_positionOffsets;

		idriver *m_rotationDriver;
		float m_rotation;

		idriver *m_alphaDriver;
		float m_alpha;

		idriver *m_scaleDriver;
		float m_scale;
	public:
		/// <summary>
		/// Creates a new screen object.
		/// </summary>
		/// <param name="parent"> The parent screen object. </param>
		/// <param name="position"> The position in relative space (can be changed to screen space be changing {@code #inScreenCoords} to true.) </param>
		/// <param name="dimensions"> The dimensions of the object, its width is scaled with the aspect ratio so it remains in proportion to the original values. </param>
		uiobject(uiobject *parent, const vector2 &position, const vector2 &dimensions);

		/// <summary>
		/// Deconstructor for the screen object.
		/// </summary>
		virtual ~uiobject();

		/// <summary>
		/// Updates this screen object and the extended object.
		/// </summary>
		void update();

		/// <summary>
		/// Updates the implementation.
		/// </summary>
		virtual void updateObject() = 0;

		/// <summary>
		/// Disowns a child from this screen objects children list.
		/// </summary>
		/// <param name="child"> The child to disown. </param>
		void removeChild(uiobject *child);

		/// <summary>
		/// Adds this object and its children to a list.
		/// </summary>
		/// <param name="list"> The list to add to.
		/// </param>
		/// <returns> The list that has been added to. </returns>
		std::vector<uiobject*> *getAll(std::vector<uiobject*> *list);

		/// <summary>
		/// Gets the positions relative in screen space.
		/// </summary>
		/// <returns> The screen positions. </returns>
		vector2 *getScreenPosition() const { return m_screenPosition; }

		/// <summary>
		/// Gets the dimensions relative in screen space.
		/// </summary>
		/// <returns> The screen dimensions. </returns>
		vector2 *getScreenDimensions() const { return m_screenDimensions; }

		/// <summary>
		/// Gets the parent object.
		/// </summary>
		/// <returns> The parent object. </returns>
		uiobject *getParent() const { return m_parent; }

		/// <summary>
		/// Removes this object from the previous parent and attaches it to another parent.
		/// </summary>
		/// <param name="parent"> The new parent object. </param>
		void setParent(uiobject *parent);

		std::vector<uiobject*> *getChildren() const { return m_children; }

		bool isVisible();

		void setVisible(const bool &visible) { m_visible = visible; }

		vector2 *getPosition() const { return m_position; }

		void setPosition(const vector2 &position) const { m_position->set(position); }

		vector2 *getDimensions() const { return m_dimensions; }

		void setDimensions(const vector2 &dimensions) const { m_dimensions->set(dimensions); }

		vector2 *getMeshSize() const { return m_meshSize; }

		void setMeshSize(const vector2 &meshSize) const { m_meshSize->set(meshSize); }

		vector4 *getScissor() const { return m_scissor; }

		void setScissor(const vector4 &scissor) const { m_scissor->set(scissor); }

		bool getInScreenCoords() const { return m_inScreenCoords; }

		void setInScreenCoords(const bool &inScreenCoords) { m_inScreenCoords = inScreenCoords; }

		vector2 *getPositionOffsets() const { return m_positionOffsets; }

		void setPositionOffsets(const vector2 &positionOffsets) const { m_positionOffsets->set(positionOffsets); }

		void setRotationDriver(idriver *rotationDriver);

		float getRotation() const { return m_rotation; }

		void setAlphaDriver(idriver *alphaDriver);

		float getAlpha();

		void setScaleDriver(idriver *scaleDriver);

		float getScale() const { return m_scale; }
	};
}
