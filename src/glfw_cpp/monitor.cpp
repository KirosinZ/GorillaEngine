#include "monitor.hpp"

#include <error_handling/assert.hpp>

namespace glfw
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
	const u32 raw_size = raw_gr.size;
	for (int i = 0; i < raw_size; ++i)
	{
		red[i] = raw_gr.red[i];
		green[i] = raw_gr.green[i];
		blue[i] = raw_gr.blue[i];
	}
}

monitor::monitor(GLFWmonitor* raw_handle)
	: m_handle(raw_handle)
{
	err::asserts(raw_handle != nullptr, "Null handle");
	glfwSetMonitorUserPointer(m_handle, this);
}

monitor::monitor(glfw::monitor&& move) noexcept
{
	m_handle = std::exchange(move.m_handle, nullptr);
	glfwSetMonitorUserPointer(m_handle, this);
}

monitor& monitor::operator=(glfw::monitor&& move) noexcept
{
	if (this == &move)
		return *this;

	m_handle = std::exchange(move.m_handle, nullptr);
	glfwSetMonitorUserPointer(m_handle, this);

	return *this;
}

monitor::~monitor() noexcept
{
	if (m_handle)
		glfwSetMonitorUserPointer(m_handle, nullptr);
}

void monitor::initialize()
{
	glfwSetMonitorCallback(config_callback_raw);
}

std::vector<monitor> monitor::get_monitors()
{
	std::vector<monitor> res;

	i32 size = 0;
	GLFWmonitor** array = glfwGetMonitors(&size);
	res.reserve(size);
	for (i32 m_ind = 0; m_ind < size; ++m_ind)
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

std::pair<i32, i32> monitor::position() const
{
	i32 xpos = 0;
	i32 ypos = 0;
	glfwGetMonitorPos(m_handle, &xpos, &ypos);

	return { xpos, ypos };
}

monitor::work_area_t monitor::work_area() const
{
	i32 xpos = 0;
	i32 ypos = 0;
	i32 width = 0;
	i32 height = 0;
	glfwGetMonitorWorkarea(m_handle, &xpos, &ypos, &width, &height);

	return
	{
		.xpos = xpos,
		.ypos = ypos,
		.width = width,
		.height = height
	};
}

std::pair<i32, i32> monitor::physical_size() const
{
	i32 width = 0;
	i32 height = 0;
	glfwGetMonitorPhysicalSize(m_handle, &width, &height);

	return { width, height };
}

std::pair<f32, f32> monitor::content_scale() const
{
	f32 xscale = 0.0f;
	f32 yscale = 0.0f;
	glfwGetMonitorContentScale(m_handle, &xscale, &yscale);

	return { xscale, yscale };
}

std::string monitor::name() const
{
	return glfwGetMonitorName(m_handle);
}

std::vector<monitor::video_mode_t> monitor::video_modes() const
{
	std::vector<monitor::video_mode_t> res;
	int size = 0;
	const GLFWvidmode* array = glfwGetVideoModes(m_handle, &size);
	res.resize(size);
	for (int vm_ind = 0; vm_ind < size; ++vm_ind)
	{
		res[vm_ind] = array[vm_ind];
	}

	return res;
}

monitor::video_mode_t monitor::video_mode() const
{
	return *glfwGetVideoMode(m_handle);
}

void monitor::set_gamma(f32 gamma)
{
	glfwSetGamma(m_handle, gamma);
}

monitor::gamma_ramp_t monitor::gamma_ramp() const
{
	return *glfwGetGammaRamp(m_handle);
}

void monitor::set_gamma_ramp(const glfw::monitor::gamma_ramp_t& gamma_ramp)
{
	GLFWgammaramp raw_gr{};
	raw_gr.size = gamma_ramp_t::size;
	raw_gr.red = new u16[gamma_ramp_t::size];
	raw_gr.green = new u16[gamma_ramp_t::size];
	raw_gr.blue = new u16[gamma_ramp_t::size];
	for (int i = 0; i < gamma_ramp_t::size; ++i)
	{
		raw_gr.red[i] = gamma_ramp.red[i];
		raw_gr.green[i] = gamma_ramp.green[i];
		raw_gr.blue[i] = gamma_ramp.blue[i];
	}
	glfwSetGammaRamp(m_handle, &raw_gr);
	delete [] raw_gr.red;
	delete [] raw_gr.green;
	delete [] raw_gr.blue;
}

void monitor::config_callback_raw(GLFWmonitor* handle, i32 event)
{
	void* user_ptr = glfwGetMonitorUserPointer(handle);
	if (user_ptr)
	{
		monitor& self = *static_cast<monitor*>(user_ptr);

		monitor::m_config_callback(self, config_callback_event(event));
	}
	else
	{
		monitor tmp_self(handle);
		monitor::m_config_callback(tmp_self, config_callback_event(event));
	}
}

} // glfw