#include "stdafx.h"
#include "Utilities/Log.h"
#include "Emu/System.h"
#include "Emu/SysCalls/Modules.h"
#include "Crypto/sha1.h"
#include <mutex>
#include "ModuleManager.h"

u32 getFunctionId(const std::string& name)
{
	const char* suffix = "\x67\x59\x65\x99\x04\x25\x04\x90\x56\x64\x27\x49\x94\x89\x74\x1A"; // Symbol name suffix
	std::string input = name + suffix;
	unsigned char output[20];

	sha1((unsigned char*)input.c_str(), input.length(), output); // Compute SHA-1 hash
	return (u32&)output[0];
}

Module::Module(u16 id, const char* name)
	: m_is_loaded(false)
	, m_name(name)
	, m_id(id)
	, m_load_func(nullptr)
	, m_unload_func(nullptr)
{
	Emu.GetModuleManager().SetModule(m_id, this, false);
}

Module::Module(const char* name, void (*init)(), void (*load)(), void (*unload)())
	: m_is_loaded(false)
	, m_name(name)
	, m_id(-1)
	, m_load_func(load)
	, m_unload_func(unload)
{
	Emu.GetModuleManager().SetModule(m_id, this, init != nullptr);
	if(init) init();
}

Module::Module(u16 id, void (*init)(), void (*load)(), void (*unload)())
	: m_is_loaded(false)
	, m_id(id)
	, m_load_func(load)
	, m_unload_func(unload)
{
	Emu.GetModuleManager().SetModule(m_id, this, init != nullptr);
	if(init) init();
}

Module::Module(Module &&other)
	: m_is_loaded(false)
	, m_id(0)
	, m_load_func(nullptr)
	, m_unload_func(nullptr)
{
	std::swap(this->m_name,other.m_name);
	std::swap(this->m_id, other.m_id);
	std::swap(this->m_is_loaded, other.m_is_loaded);
	std::swap(this->m_load_func, other.m_load_func);
	std::swap(this->m_unload_func, other.m_unload_func);
	std::swap(this->m_funcs_list, other.m_funcs_list);
}

Module &Module::operator =(Module &&other)
{
	std::swap(this->m_name, other.m_name);
	std::swap(this->m_id, other.m_id);
	std::swap(this->m_is_loaded, other.m_is_loaded);
	std::swap(this->m_load_func, other.m_load_func);
	std::swap(this->m_unload_func, other.m_unload_func);
	std::swap(this->m_funcs_list, other.m_funcs_list);
	return *this;
}

Module::~Module()
{
	UnLoad();

	for (int i = 0; i < m_funcs_list.size(); i++)
	{
		delete m_funcs_list[i];
	}
}

void Module::Load()
{
	if(IsLoaded())
		return;

	if(m_load_func) m_load_func();

	for(u32 i=0; i<m_funcs_list.size(); ++i)
	{
		Emu.GetModuleManager().AddFunc(m_funcs_list[i]);
	}

	SetLoaded(true);
}

void Module::UnLoad()
{
	if(!IsLoaded())
		return;

	if(m_unload_func) m_unload_func();

	for(u32 i=0; i<m_funcs_list.size(); ++i)
	{
		Emu.GetModuleManager().UnloadFunc(m_funcs_list[i]->id);
	}

	SetLoaded(false);
}

bool Module::Load(u32 id)
{
	if(Emu.GetModuleManager().IsLoadedFunc(id)) return false;

	for(u32 i=0; i<m_funcs_list.size(); ++i)
	{
		if(m_funcs_list[i]->id == id)
		{
			Emu.GetModuleManager().AddFunc(m_funcs_list[i]);
			return true;
		}
	}

	return false;
}

bool Module::UnLoad(u32 id)
{
	return Emu.GetModuleManager().UnloadFunc(id);
}

void Module::SetLoaded(bool loaded)
{
	m_is_loaded = loaded;
}

bool Module::IsLoaded() const
{
	return m_is_loaded;
}

u16 Module::GetID() const
{
	return m_id;
}

std::string Module::GetName() const
{
	return m_name;
}

void Module::SetName(const std::string& name)
{
	m_name = name;
}

void Module::Log(const u32 id, std::string fmt, ...)
{
	if(Ini.HLELogging.GetValue())
	{
		va_list list;
		va_start(list, fmt);
		LOG_NOTICE(HLE, GetName() + fmt::Format("[%d]: ", id) + fmt::FormatV(fmt, list));
		va_end(list);
	}
}

void Module::Log(std::string fmt, ...)
{
	if(Ini.HLELogging.GetValue())
	{
		va_list list;
		va_start(list, fmt);
		LOG_NOTICE(HLE, GetName() + ": " + fmt::FormatV(fmt, list));
		va_end(list);
	}
}

void Module::Warning(const u32 id, std::string fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	LOG_WARNING(HLE, GetName() + fmt::Format("[%d] warning: ", id) + fmt::FormatV(fmt, list));
	va_end(list);
}

void Module::Warning(std::string fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	LOG_WARNING(HLE, GetName() + " warning: " + fmt::FormatV(fmt, list));
	va_end(list);
}

void Module::Error(const u32 id, std::string fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	LOG_ERROR(HLE, GetName() + fmt::Format("[%d] error: ", id) + fmt::FormatV(fmt, list));
	va_end(list);
}

void Module::Error(std::string fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	LOG_ERROR(HLE, GetName() + " error: " + fmt::FormatV(fmt, list));
	va_end(list);
}

bool Module::CheckID(u32 id) const
{
	return Emu.GetIdManager().CheckID(id) && Emu.GetIdManager().GetID(id).m_name == GetName();
}

bool Module::CheckID(u32 id, ID*& _id) const
{
	return Emu.GetIdManager().CheckID(id) && (_id = &Emu.GetIdManager().GetID(id))->m_name == GetName();
}
