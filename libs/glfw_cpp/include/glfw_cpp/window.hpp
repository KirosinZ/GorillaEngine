#ifndef GORILLA_WINDOW_HPP
#define GORILLA_WINDOW_HPP

#include <functional>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "library.hpp"
#include "windowhints.hpp"
#include "monitor.hpp"

#include <error_handling/exceptions.hpp>


namespace gorilla::glfw
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


	static gorilla::error_handling::result<window> ctor(int32_t width, int32_t height, const std::string& title, const window_init_info& init_info = { });

	static gorilla::error_handling::result<window> ctor(int32_t width, int32_t height, const std::string& title, const monitor& mnt, const window_init_info& init_info = { });

	static gorilla::error_handling::result<window> ctor(int32_t width, int32_t height, const std::string& title, const window& share, const window_init_info& init_info = { });

	static gorilla::error_handling::result<window> ctor(int32_t width, int32_t height, const std::string& title, const monitor& mnt, const window& share, const window_init_info& init_info = { });

	// set_icons

	[[nodiscard]] glm::ivec2 position() const;

	void set_position(int xpos, int ypos);

	void set_position(const glm::ivec2& pos);

	[[nodiscard]] glm::ivec2 size() const;

	void set_size(int width, int height);

	void set_size(const glm::ivec2& size);

	void set_size_limits(int min_width, int min_height, int max_width, int max_height);

	void set_aspect_ratio(int numer, int denom);

	[[nodiscard]] glm::ivec2 framebuffer_size() const;
	// frame_size

	[[nodiscard]] glm::vec2 content_scale() const;

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

	[[nodiscard]] inline GLFWwindow* handle() const
	{ return handle_.get(); }

	[[nodiscard]] inline const std::any& user_data() const
	{ return user_data_; }

	inline std::any& user_data()
	{ return user_data_; }

	[[nodiscard]]
	inline const std::function<position_callback_sgn>& position_callback() const
	{ return position_callback_; }

	inline std::function<position_callback_sgn>& position_callback()
	{ return position_callback_; }

	[[nodiscard]]
	inline const std::function<size_callback_sgn>& size_callback() const
	{ return size_callback_; }

	inline std::function<size_callback_sgn>& size_callback()
	{ return size_callback_; }

	[[nodiscard]]
	inline const std::function<close_callback_sgn>& close_callback() const
	{ return close_callback_; }

	inline std::function<close_callback_sgn>& close_callback()
	{ return close_callback_; }

	[[nodiscard]]
	inline const std::function<refresh_callback_sgn>& refresh_callback() const
	{ return refresh_callback_; }

	inline std::function<refresh_callback_sgn>& refresh_callback()
	{ return refresh_callback_; }

	[[nodiscard]]
	inline const std::function<focus_callback_sgn>& focus_callback() const
	{ return focus_callback_; }

	inline std::function<focus_callback_sgn>& focus_callback()
	{ return focus_callback_; }

	[[nodiscard]]
	inline const std::function<minimize_callback_sgn>& minimize_callback() const
	{ return minimize_callback_; }

	inline std::function<minimize_callback_sgn>& minimize_callback()
	{ return minimize_callback_; }

	[[nodiscard]]
	inline const std::function<maximize_callback_sgn>& maximize_callback() const
	{ return maximize_callback_; }

	inline std::function<maximize_callback_sgn>& maximize_callback()
	{ return maximize_callback_; }

	[[nodiscard]]
	inline const std::function<contentscale_callback_sgn>& contentscale_callback() const
	{ return contentscale_callback_; }

	inline std::function<contentscale_callback_sgn>& contentscale_callback()
	{ return contentscale_callback_; }

	[[nodiscard]]
	inline const std::function<framebuffersize_callback_sgn>& framebuffersize_callback() const
	{ return framebuffersize_callback_; }

	inline std::function<framebuffersize_callback_sgn>& framebuffersize_callback()
	{ return framebuffersize_callback_; }

private:
	window(std::string title, GLFWwindow* handle, std::any user_data = { }) noexcept;

	std::string title_;
	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> handle_;
	std::any user_data_;

	std::function<position_callback_sgn> position_callback_;
	std::function<size_callback_sgn> size_callback_;
	std::function<close_callback_sgn> close_callback_;
	std::function<refresh_callback_sgn> refresh_callback_;
	std::function<focus_callback_sgn> focus_callback_;
	std::function<minimize_callback_sgn> minimize_callback_;
	std::function<maximize_callback_sgn> maximize_callback_;
	std::function<contentscale_callback_sgn> contentscale_callback_;
	std::function<framebuffersize_callback_sgn> framebuffersize_callback_;

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
