#ifndef GORILLA_WINDOW_HPP
#define GORILLA_WINDOW_HPP

#include <memory>
#include <string>

#include "glfw.hpp"
#include "windowhints.hpp"
#include "monitor.hpp"

namespace glfw
{

class window
{
public:

	window(i32 width, i32  height, const std::string& title, const window_init_info& init_info = {});
	window(i32 width, i32  height, const std::string& title, const monitor& mnt, const window_init_info& init_info = {});
	window(i32 width, i32  height, const std::string& title, const window& share, const window_init_info& init_info = {});
	window(i32 width, i32  height, const std::string& title, const monitor& mnt, const window& share, const window_init_info& init_info = {});

	window(window&&) noexcept;
	window& operator=(window&&) noexcept;

	window(const window&) = delete;
	window& operator=(const window&) = delete;

	~window() noexcept;

	[[nodiscard]] bool focused() const;
	[[nodiscard]] bool minimized() const;
	[[nodiscard]] bool maximized() const;
	[[nodiscard]] bool visible() const;
	[[nodiscard]] bool resizable() const;
	[[nodiscard]] bool decorated() const;
	[[nodiscard]] bool top_most() const;

	void minimize();
	void maximize();
	void show();
	void hide();

	i32 width() const;
	i32  height() const;
	std::string title() const;

	void set_size(i32 width, i32 height);
	void set_title(const std::string& title);

	bool should_close() const;
	void set_should_close(bool value);

	[[nodiscard]] inline GLFWwindow* handle() const { return m_handle; }

private:
	i32 m_width = 0;
	i32 m_height = 0;
	std::string m_title;
	GLFWwindow* m_handle = nullptr;
};

}

#endif //GORILLA_WINDOW_HPP
