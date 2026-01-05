#ifndef VDE__CORE__WINDOW_H
#define VDE__CORE__WINDOW_H
#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace vde::core
{
	struct WindowDescriptor
	{
		glm::uvec2  size        = { 1600, 900 };
		std::string title      = "LNX VDE window";
		bool        fullscreen = false;
	};

	class Window
	{
		struct Impl;

		std::unique_ptr<Impl> m_pImpl;
		WindowDescriptor      m_desc;

	public:
		explicit Window(const WindowDescriptor& desc);
		~Window() noexcept;

		const WindowDescriptor& GetDesc() const;
		Impl& GetImpl();

		bool ShouldClose() const;
		void RaiseShouldClose();

		void PollEvents();
	};
}

#endif /* VDE__CORE__WINDOW_H */
