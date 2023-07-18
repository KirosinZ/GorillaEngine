#ifndef GORILLA_WINDOW_HPP
#define GORILLA_WINDOW_HPP

#include <functional>
#include <memory>
#include <string>

#include "library.hpp"
#include "windowhints.hpp"
#include "monitor.hpp"

#include <error_handling/exceptions.hpp>

namespace glfw
{

struct image
{
	int width = 0;
	int height = 0;
	std::vector<unsigned char> pixels;
};

class window
{
public:
	using position_callback_sgn = void(window&, int xpos, int ypos);
	using size_callback_sgn = void(window&, int width, int height);
	using close_callback_sgn = void(window&);
	using refresh_callback_sgn = void(window&);
	using focus_callback_sgn = void(window&, bool focused);
	using minimize_callback_sgn = void(window&, bool minimized);
	using maximize_callback_sgn = void(window&, bool maximized);
	using contentscale_callback_sgn = void(window&, float xscale, float yscale);
	using framebuffersize_callback_sgn = void(window&, int width, int height);


	static err::result<window> ctor(i32 width, i32 height, const std::string& title, const window_init_info& init_info = {});
	static err::result<window> ctor(i32 width, i32 height, const std::string& title, const monitor& mnt, const window_init_info& init_info = {});
	static err::result<window> ctor(i32 width, i32 height, const std::string& title, const window& share, const window_init_info& init_info = {});
	static err::result<window> ctor(i32 width, i32 height, const std::string& title, const monitor& mnt, const window& share, const window_init_info& init_info = {});

	window(window&&) noexcept;
	window& operator=(window&&) noexcept;

	window(const window&) = delete;
	window& operator=(const window&) = delete;

	~window() noexcept;

	// set_icons

	[[nodiscard]] std::pair<int, int> position() const;
	void set_position(int xpos, int ypos);

	[[nodiscard]] std::pair<int, int> size() const;
	void set_size(int width, int height);

	void set_size_limits(int min_width, int min_height, int max_width, int max_height);
	void set_aspect_ratio(int numer, int denom);

	[[nodiscard]] std::pair<int, int> framebuffer_size() const;
	// frame_size

	[[nodiscard]] std::pair<float, float> content_scale() const;
	[[nodiscard]] float opacity() const;
	void set_opacity(float value);

	void minimize();
	void restore();
	void maximize();
	void show();
	void hide();
	void focus();
	void request_attention();
//	monitor current_monitor() const;
//	void set_current_monitor(const monitor& monitor);

	// attrib
	[[nodiscard]] bool focused() const;
	[[nodiscard]] bool minimized() const;
	[[nodiscard]] bool maximized() const;
	[[nodiscard]] bool visible() const;
	[[nodiscard]] bool resizable() const;
	[[nodiscard]] bool decorated() const;
	[[nodiscard]] bool top_most() const;

	void swap_buffers();

	[[nodiscard]] std::string title() const;
	void set_title(const std::string& title);

	[[nodiscard]] bool should_close() const;
	void set_should_close(bool value);

	[[nodiscard]] inline GLFWwindow* handle() const { return m_handle; }

	[[nodiscard]] inline const std::any& user_data() const { return m_user_data; }
	inline std::any& user_data() { return m_user_data; }

	[[nodiscard]]
	inline const std::function<position_callback_sgn>& position_callback() const { return m_position_callback; }
	inline std::function<position_callback_sgn>& position_callback() { return m_position_callback; }

	[[nodiscard]]
	inline const std::function<size_callback_sgn>& size_callback() const { return m_size_callback; }
	inline std::function<size_callback_sgn>& size_callback() { return m_size_callback; }

	[[nodiscard]]
	inline const std::function<close_callback_sgn>& close_callback() const { return m_close_callback; }
	inline std::function<close_callback_sgn>& close_callback() { return m_close_callback; }

	[[nodiscard]]
	inline const std::function<refresh_callback_sgn>& refresh_callback() const { return m_refresh_callback; }
	inline std::function<refresh_callback_sgn>& refresh_callback() { return m_refresh_callback; }

	[[nodiscard]]
	inline const std::function<focus_callback_sgn>& focus_callback() const { return m_focus_callback; }
	inline std::function<focus_callback_sgn>& focus_callback() { return m_focus_callback; }

	[[nodiscard]]
	inline const std::function<minimize_callback_sgn>& minimize_callback() const { return m_minimize_callback; }
	inline std::function<minimize_callback_sgn>& minimize_callback() { return m_minimize_callback; }

	[[nodiscard]]
	inline const std::function<maximize_callback_sgn>& maximize_callback() const { return m_maximize_callback; }
	inline std::function<maximize_callback_sgn>& maximize_callback() { return m_maximize_callback; }

	[[nodiscard]]
	inline const std::function<contentscale_callback_sgn>& contentscale_callback() const { return m_contentscale_callback; }
	inline std::function<contentscale_callback_sgn>& contentscale_callback() { return m_contentscale_callback; }

	[[nodiscard]]
	inline const std::function<framebuffersize_callback_sgn>& framebuffersize_callback() const { return m_framebuffersize_callback; }
	inline std::function<framebuffersize_callback_sgn>& framebuffersize_callback() { return m_framebuffersize_callback; }

private:
	window(std::string title, GLFWwindow* handle, std::any user_data = {}) noexcept;

	std::string m_title;
	GLFWwindow* m_handle = nullptr;
	std::any m_user_data;

	std::function<position_callback_sgn> m_position_callback;
	std::function<size_callback_sgn> m_size_callback;
	std::function<close_callback_sgn> m_close_callback;
	std::function<refresh_callback_sgn> m_refresh_callback;
	std::function<focus_callback_sgn> m_focus_callback;
	std::function<minimize_callback_sgn> m_minimize_callback;
	std::function<maximize_callback_sgn> m_maximize_callback;
	std::function<contentscale_callback_sgn> m_contentscale_callback;
	std::function<framebuffersize_callback_sgn> m_framebuffersize_callback;

	static void position_callback_raw(GLFWwindow*, int, int);
	static void size_callback_raw(GLFWwindow*, int, int);
	static void close_callback_raw(GLFWwindow*);
	static void refresh_callback_raw(GLFWwindow*);
	static void focus_callback_raw(GLFWwindow*, int);
	static void minimize_callback_raw(GLFWwindow*, int);
	static void maximize_callback_raw(GLFWwindow*, int);
	static void contentscale_callback_raw(GLFWwindow*, float, float);
	static void framebuffersize_callback_raw(GLFWwindow*, int, int);
};

}

#endif //GORILLA_WINDOW_HPP
