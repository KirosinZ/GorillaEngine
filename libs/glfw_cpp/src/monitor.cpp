#include "glfw_cpp/monitor.hpp"

#include <error_handling/assert.hpp>


namespace gorilla::glfw
{

monitor::video_mode_t::video_mode_t(GLFWvidmode raw_vm)
	: width(raw_vm.width),
	  height(raw_vm.height),
	  red_bits(raw_vm.redBits),
	  green_bits(raw_vm.greenBits),
	  blue_bits(raw_vm.blueBits),
	  refresh_rate(raw_vm.refreshRate)
{}

monitor::gamma_ramp_t::gamma_ramp_t(GLFWgammaramp raw_gr)
{
	const uint32_t raw_size = raw_gr.size;
	for (int i = 0; i < raw_size; ++i)
	{
		red[i] = raw_gr.red[i];
		green[i] = raw_gr.green[i];
		blue[i] = raw_gr.blue[i];
	}
}

monitor::monitor(GLFWmonitor* raw_handle)
	: handle_(raw_handle)
{
	gorilla::error_handling::asserts(raw_handle != nullptr, "Null handle");
	glfwSetMonitorUserPointer(handle_, this);
}

monitor::monitor(glfw::monitor&& move) noexcept
{
	handle_ = std::exchange(move.handle_, nullptr);
	glfwSetMonitorUserPointer(handle_, this);
}

monitor& monitor::operator=(glfw::monitor&& move) noexcept
{
	if (this == &move)
		return *this;

	handle_ = std::exchange(move.handle_, nullptr);
	glfwSetMonitorUserPointer(handle_, this);

	return *this;
}

monitor::~monitor() noexcept
{
	if (handle_)
		glfwSetMonitorUserPointer(handle_, nullptr);
}

void monitor::initialize()
{
	glfwSetMonitorCallback(config_callback_raw);
}

std::vector<monitor> monitor::get_monitors()
{
	std::vector<monitor> res;

	int32_t size = 0;
	GLFWmonitor** array = glfwGetMonitors(&size);
	res.reserve(size);
	for (int32_t m_ind = 0; m_ind < size; ++m_ind)
	{
		res.push_back(monitor(array[m_ind]));
	}

	return res;
}

std::optional<monitor> monitor::get_primary_monitor()
{
	GLFWmonitor* handle_raw = glfwGetPrimaryMonitor();
	if (!handle_raw)
		return std::nullopt;

	return monitor(handle_raw);
}

glm::ivec2 monitor::position() const
{
	int32_t xpos = 0;
	int32_t ypos = 0;
	glfwGetMonitorPos(handle_, &xpos, &ypos);

	return { xpos, ypos };
}

monitor::work_area_t monitor::work_area() const
{
	int32_t xpos = 0;
	int32_t ypos = 0;
	int32_t width = 0;
	int32_t height = 0;
	glfwGetMonitorWorkarea(handle_, &xpos, &ypos, &width, &height);

	return
		{
			.xpos = xpos,
			.ypos = ypos,
			.width = width,
			.height = height
		};
}

glm::ivec2 monitor::physical_size() const
{
	int32_t width = 0;
	int32_t height = 0;
	glfwGetMonitorPhysicalSize(handle_, &width, &height);

	return { width, height };
}

glm::vec2 monitor::content_scale() const
{
	float xscale = 0.0f;
	float yscale = 0.0f;
	glfwGetMonitorContentScale(handle_, &xscale, &yscale);

	return { xscale, yscale };
}

std::string monitor::name() const
{
	return glfwGetMonitorName(handle_);
}

std::vector<monitor::video_mode_t> monitor::video_modes() const
{
	std::vector<monitor::video_mode_t> res;
	int size = 0;
	const GLFWvidmode* array = glfwGetVideoModes(handle_, &size);
	res.resize(size);
	for (int vm_ind = 0; vm_ind < size; ++vm_ind)
	{
		res[vm_ind] = array[vm_ind];
	}

	return res;
}

monitor::video_mode_t monitor::video_mode() const
{
	return *glfwGetVideoMode(handle_);
}

void monitor::set_gamma(float gamma)
{
	glfwSetGamma(handle_, gamma);
}

monitor::gamma_ramp_t monitor::gamma_ramp() const
{
	return *glfwGetGammaRamp(handle_);
}

void monitor::set_gamma_ramp(const glfw::monitor::gamma_ramp_t& gamma_ramp)
{
	GLFWgammaramp raw_gr{ };
	raw_gr.size = gamma_ramp_t::size;
	raw_gr.red = new uint16_t[gamma_ramp_t::size];
	raw_gr.green = new uint16_t[gamma_ramp_t::size];
	raw_gr.blue = new uint16_t[gamma_ramp_t::size];
	for (int i = 0; i < gamma_ramp_t::size; ++i)
	{
		raw_gr.red[i] = gamma_ramp.red[i];
		raw_gr.green[i] = gamma_ramp.green[i];
		raw_gr.blue[i] = gamma_ramp.blue[i];
	}
	glfwSetGammaRamp(handle_, &raw_gr);
	delete[] raw_gr.red;
	delete[] raw_gr.green;
	delete[] raw_gr.blue;
}

void monitor::config_callback_raw(GLFWmonitor* handle, int32_t event)
{
	void* user_ptr = glfwGetMonitorUserPointer(handle);
	if (user_ptr)
	{
		monitor& self = *static_cast<monitor*>(user_ptr);

		monitor::config_callback_(self, config_callback_event(event));
	}
	else
	{
		monitor tmp_self(handle);
		monitor::config_callback_(tmp_self, config_callback_event(event));
	}
}

} // glfw