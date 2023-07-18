#ifndef GORILLA_MONITOR_HPP
#define GORILLA_MONITOR_HPP

#include "platform/common.hpp"

#include <any>
#include <array>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "library.hpp"

namespace glfw
{

class monitor
{
public:
	enum class config_callback_event
	{
		connected = GLFW_CONNECTED,
		disconnected = GLFW_DISCONNECTED,
	};

	using config_callback_sgn = void(monitor&, config_callback_event);

	struct video_mode_t
	{
		i32 width = 0;
		i32 height = 0;
		i32 red_bits = 0;
		i32 green_bits = 0;
		i32 blue_bits = 0;
		i32 refresh_rate = 0;

		video_mode_t() = default;
		video_mode_t(GLFWvidmode raw_vm);
	};

	struct gamma_ramp_t
	{
		static constexpr u32 size = 256;
		std::array<u16, size> red{};
		std::array<u16, size> blue{};
		std::array<u16, size> green{};

		gamma_ramp_t() = default;
		gamma_ramp_t(GLFWgammaramp raw_gr);
	};

	struct work_area_t
	{
		i32 xpos = 0;
		i32 ypos = 0;
		i32 width = 0;
		i32 height = 0;
	};

	monitor() = delete;

	monitor(monitor&&) noexcept;
	monitor& operator=(monitor&&) noexcept;

	monitor(const monitor&) = delete;
	monitor& operator=(const monitor&) = delete;

	~monitor() noexcept;

	static void initialize();

	static std::vector<monitor> get_monitors();
	static std::optional<monitor> get_primary_monitor();

	[[nodiscard]] std::pair<i32, i32> position() const;
	[[nodiscard]] work_area_t work_area() const;
	[[nodiscard]] std::pair<i32, i32> physical_size() const;
	[[nodiscard]] std::pair<f32, f32> content_scale() const;

	[[nodiscard]] std::string name() const;

	inline static void set_config_callback(const std::function<config_callback_sgn>& callback) { monitor::m_config_callback = callback; }

	[[nodiscard]] std::vector<video_mode_t> video_modes() const;
	[[nodiscard]] video_mode_t video_mode() const;

	void set_gamma(f32 gamma);
	[[nodiscard]] gamma_ramp_t gamma_ramp() const;
	void set_gamma_ramp(const gamma_ramp_t& gamma_ramp);

	[[nodiscard]] inline GLFWmonitor* handle() const { return m_handle; };

	[[nodiscard]] inline const std::any& user_data() const { return m_user_data; };
	inline std::any& user_data() { return m_user_data; };
private:
	explicit monitor(GLFWmonitor* raw_handle);

	static void config_callback_raw(GLFWmonitor*, i32 event);
	inline static std::function<config_callback_sgn> m_config_callback{};

	GLFWmonitor* m_handle = nullptr;
	std::any m_user_data{};
};

} // glfw

#endif //GORILLA_MONITOR_HPP
