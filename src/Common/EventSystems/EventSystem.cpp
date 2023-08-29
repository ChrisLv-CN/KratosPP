#include <typeinfo>

#include <Common/EventSystems/EventSystem.h>
#include <Utilities/Debug.h>

Event::Event(const char* Name, const char* Dest)
{
    this->Name = Name;
    this->Dest = Dest;
}

EventSystem::EventSystem(const char* name)
{
    this->Name = name;
}

void EventSystem::AddHandler(Event e, HandleEvent func)
{
    this->_handlers[e] += newDelegate(func);
    const char* log = "Event [%s]%s add handler: %s\n";
    Debug::Log(log, this->Name, e.Name, typeid(&func).name());
}

void EventSystem::RemoveHandler(Event e, HandleEvent func)
{
    this->_handlers[e] -= newDelegate(func);
}

void EventSystem::Broadcast(Event e, void* args)
{
    this->_handlers[e](this, e, args);
}

void* EventArgsLate = (void*)true;
void* EventArgsEmpty = nullptr;

// 事件管理器
EventSystem EventSystems::General("General"); // 全局事件
EventSystem EventSystems::Render("Render"); // 单位渲染事件
EventSystem EventSystems::Logic("Logic"); // 单位逻辑事件
EventSystem EventSystems::SaveLoad("SaveLoad"); // 存档事件

// 程序启动事件
Event Events::ExeRun = Event("ExeRun", "Raised when YR run");
// 程序终止事件
Event Events::ExeTerminate = Event("ExeTerminate", "Raised when YR terminate");
// 读取命令参数
Event Events::CmdLineParse = Event("CmdLineParse", "Raised when YR load cmd");
// 游戏事件
Event Events::PointerExpireEvent = Event("AnnounceExpiredPointer", "Raised when an AbstractClass pointer expired");
Event Events::ScenarioClearClassesEvent = Event("ScenarioClearClasses", "Raised when scenario is cleaning classes");
Event Events::ScenarioStartEvent = Event("ScenarioStart", "Raised when scenario start");
// 渲染事件
Event Events::GScreenRenderEvent = Event("GScreenClass_Render", "Raised when GScreen is Render");
Event Events::SidebarRenderEvent = Event("SidebarClass_Draw_It", "Raised when Sidebar is Render");
// 逻辑事件
Event Events::LogicUpdateEvent = Event("LogicClassUpdate", "Raised when LogicClass update");
// 存档事件
Event Events::SaveGameEvent = Event("SaveGame", "Raised when saving game");
Event Events::LoadGameEvent = Event("LoadGame", "Raised when loading game");

// 事件参数
SaveLoadEventArgs::SaveLoadEventArgs(const char* fileName, bool isStart)
{
    this->FileName = fileName;
    this->Stream = nullptr;
    this->isStart = isStart;
    this->isStartInStream = false;
}

SaveLoadEventArgs::SaveLoadEventArgs(IStream* stream, bool isStart)
{
    this->FileName = nullptr;
    this->Stream = stream;
    this->isStart = false;
    this->isStartInStream = isStart;
}

bool SaveLoadEventArgs::InStream()
{
    return Stream != nullptr;
}

bool SaveLoadEventArgs::IsStart()
{
    return isStart;
}

bool SaveLoadEventArgs::IsEnd()
{
    return !IsStart() && !InStream();
}

bool SaveLoadEventArgs::IsStartInStream()
{
    return isStartInStream;
}

bool SaveLoadEventArgs::IsEndInStream()
{
    return !IsStartInStream() && InStream();
}
