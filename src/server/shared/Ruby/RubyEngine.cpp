#include "RubyEngine.h"

void RubyEngine::Initialize()
{
    if(running)
        return;

    RUBY_INIT_STACK;
#ifdef _WIN32
    int argc = 1;
    char** argv = { NULL };
    NtInitialize(&argc, &argv);
#endif
    ruby_init();
    if(!require("./all.rb"))
    {
        sLog->outString("Error occurred when loading all.rb, please make sure that the file exists");
        return;
    }
    running = true;
    Rice::define_global_function("method_missing", &method_missing, (Arg("method"), Arg("args") = Qnil, Arg("block") = Qnil)); /// Define this here so we avoid crashes, Comma operator YAY!
    Rice::define_global_function("AddSC", &AddSC);
    SetupRuby();
    call_function<VALUE>(Qnil, "test1");
    sLog->outString("Ruby engine initialized correctly");
}

void RubyEngine::Finalize()
{
    ruby_finalize();
    running = false;
}

RubyEngine::RubyEngine()
{
    running = false;
    //Initialize();
}

RubyEngine::~RubyEngine()
{
    if(running)
        Finalize();
}

void RubyEngine::SetupRuby()
{
    typeid_enum_type = Rice::define_enum<TypeID>("TypeID")
        .define_value("TYPEID_OBJECT", TYPEID_OBJECT)
        .define_value("TYPEID_ITEM", TYPEID_ITEM)
        .define_value("TYPEID_CONTAINER", TYPEID_CONTAINER)
        .define_value("TYPEID_UNIT", TYPEID_UNIT)
        .define_value("TYPEID_PLAYER", TYPEID_PLAYER)
        .define_value("TYPEID_GAMEOBJECT", TYPEID_GAMEOBJECT)
        .define_value("TYPEID_DYNAMICOBJECT", TYPEID_DYNAMICOBJECT)
        .define_value("TYPEID_CORPSE", TYPEID_CORPSE);
        
        
    Rice::define_class<ByteBuffer>("ByteBuffer") // This definition is kinda stupid
        .define_constructor(Rice::Constructor<ByteBuffer>())
        .define_constructor(Rice::Constructor<ByteBuffer, size_t>())
        .define_method("clear",      &ByteBuffer::clear)
        .define_method("ReadUInt8",  &ByteBuffer::ReadUInt8)
        .define_method("ReadUInt16", &ByteBuffer::ReadUInt16)
        .define_method("ReadUInt32", &ByteBuffer::ReadUInt32)
        .define_method("ReadUInt64", &ByteBuffer::ReadUInt64)
        .define_method("ReadInt8",   &ByteBuffer::ReadInt8)
        .define_method("ReadInt16",  &ByteBuffer::ReadInt16)
        .define_method("ReadInt32",  &ByteBuffer::ReadInt32)
        .define_method("ReadInt64",  &ByteBuffer::ReadInt64)
        .define_method("WriteUInt8", &ByteBuffer::WriteUInt8)
        .define_method("WriteUInt16",&ByteBuffer::WriteUInt16)
        .define_method("WriteUInt32",&ByteBuffer::WriteUInt32)
        .define_method("WriteUInt64",&ByteBuffer::WriteUInt64)
        .define_method("WriteInt8",  &ByteBuffer::WriteInt8)
        .define_method("WriteInt16", &ByteBuffer::WriteInt16)
        .define_method("WriteInt32", &ByteBuffer::WriteInt32)
        .define_method("WriteInt64", &ByteBuffer::WriteInt64);
    
    Rice::define_class<Object>("Object")
        .define_method("IsInWorld", &Object::IsInWorld)
        .define_method("AddToWorld", &Object::AddToWorld)
        .define_method("GetGUID", &Object::GetGUID)
        .define_method("GetGUIDLow", &Object::GetGUIDLow)
        .define_method("GetGUIDMid", &Object::GetGUIDMid)
        .define_method("GetGUIDHigh", &Object::GetGUIDHigh)
        .define_method("GetPackGUID", &Object::GetPackGUID)
        .define_method("GetEntry", &Object::GetEntry)
        .define_method("SetEntry", &Object::SetEntry)
        .define_method("GetTypeId", &Object::GetTypeId)
        .define_method("isType", &Object::isType)
        .define_method("BuildCreateUpdateBlockForPlayer", &Object::BuildCreateUpdateBlockForPlayer)
        .define_method("SendUpdateToPlayer", &Object::SendUpdateToPlayer)
        .define_method("BuildValuesUpdateBlockForPlayer", &Object::BuildValuesUpdateBlockForPlayer)
        .define_method("BuildOutOfRangeUpdateBlock", &Object::BuildOutOfRangeUpdateBlock);
    
    Rice::define_class<ServerScript>("ServerScript")
        .define_director<ServerScriptDirector>()
        .define_constructor(Rice::Constructor<ServerScriptDirector, Rice::Object, std::string>())
        .define_method("OnNetworkStart", &ServerScriptDirector::default_OnNetworkStart)
        .define_method("OnNetworkStop", &ServerScriptDirector::default_OnNetworkStop);
}