#pragma once

#include <memory>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "IButton.hpp"

namespace acid
{
	/// <summary>
	/// Handles multiple buttons at once.
	/// </summary>
	class ACID_EXPORT ButtonCompound :
		public IButton,
		public NonCopyable
	{
	private:
		std::vector<std::unique_ptr<IButton>> m_buttons;
		bool m_useAnd;
		bool m_wasDown;
	public:
		/// <summary>
		/// Creates a new compound button.
		/// </summary>
		/// <param name="buttons"> The buttons on the being added. </param>
		/// <param name="useAnd"> If <seealso cref="#IsDown()"/> will check if all buttons are down instead of just one. </param>
		explicit ButtonCompound(const std::vector<IButton *> &buttons, const bool &useAnd = false);

		bool IsDown() const override;

		bool WasDown() override;
	};
}
