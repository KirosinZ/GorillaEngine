#ifndef GORILLA_MONITOR_HPP
#define GORILLA_MONITOR_HPP

#include <any>
#include <array>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "library.hpp"


namespace gorilla::glfw
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
		int32_t width = 0;
		int32_t height = 0;
		int32_t red_bits = 0;
		int32_t green_bits = 0;
		int32_t blue_bits = 0;
		int32_t refresh_rate = 0;

		video_mode_t() = default;

		video_mode_t(GLFWvidmode raw_vm);
	};

	struct gamma_ramp_t
	{
		static constexpr uint32_t size = 256;
		std::array<uint16_t, size> red{ };
		std::array<uint16_t, size> blue{ };
		std::array<uint16_t, size> green{ };

		gamma_ramp_t() = default;

		gamma_ramp_t(GLFWgammaramp raw_gr);
	};

	struct work_area_t
	{
		int32_t xpos = 0;
		int32_t ypos = 0;
		int32_t width = 0;
		int32_t height = 0;
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

	[[nodiscard]] glm::ivec2 position() const;

	[[nodiscard]] work_area_t work_area() const;

	[[nodiscard]] glm::ivec2 physical_size() const;

	[[nodiscard]] glm::vec2 content_scale() const;

	[[nodiscard]] std::string name() const;

	inline static void set_config_callback(const std::function<config_callback_sgn>& callback)
	{ monitor::config_callback_ = callback; }

	[[nodiscard]] std::vector<video_mode_t> video_modes() const;

	[[nodiscard]] video_mode_t video_mode() const;

	void set_gamma(float gamma);

	[[nodiscard]] gamma_ramp_t gamma_ramp() const;

	void set_gamma_ramp(const gamma_ramp_t& gamma_ramp);

	[[nodiscard]] inline GLFWmonitor* handle() const
	{ return handle_; };

	[[nodiscard]] inline const std::any& user_data() const
	{ return user_data_; };

	inline std::any& user_data()
	{ return user_data_; };
private:
	explicit monitor(GLFWmonitor* raw_handle);

	static void config_callback_raw(GLFWmonitor*, int32_t event);

	inline static std::function<config_callback_sgn> config_callback_{ };

	GLFWmonitor* handle_ = nullptr;
	std::any user_data_{ };
};

} // glfw

#endif //GORILLA_MONITOR_HPP
