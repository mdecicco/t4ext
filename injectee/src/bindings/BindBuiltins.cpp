#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>
#include <Client.h>

#include <utils/Math.hpp>
#include <utils/Buffer.h>
#include <utils/Buffer.hpp>
#include <utils/Singleton.h>

#include <direct.h>
#include <sys/stat.h>
#include <filesystem>

namespace t4ext {
    enum class FileOpenMode {
        Text,
        Binary
    };

    class BufferWrapper : public utils::Buffer {
        public:
            template <typename T>
            T typedRead() {
                u64 available = m_capacity - m_pos;
                if (sizeof(T) > available) {
                    gClient::Get()->getAPI()->scriptExceptionf(
                        "Attempted to read %llu byte%s from Buffer, but only %llu byte%s remain to be read",
                        sizeof(T), sizeof(T) == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return 0;
                }

                T r;
                if (!read(r)) {
                    gClient::Get()->getAPI()->scriptExceptionf("Failed to read %d byte%s from buffer", sizeof(T), sizeof(T) == 1 ? "" : "s");
                    return 0;
                }

                return r;
            }

            template <typename T>
            T typedWrite(T value) {
                u64 available = m_capacity - m_pos;
                if (sizeof(T) > available) {
                    gClient::Get()->getAPI()->scriptExceptionf(
                        "Attempt0ed to write %llu byte%s to Buffer, but only %llu byte%s remain to be read",
                        sizeof(T), sizeof(T) == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return 0;
                }

                T r;
                if (!read(r)) {
                    gClient::Get()->getAPI()->scriptExceptionf("Failed to write %d byte%s to buffer", sizeof(T), sizeof(T) == 1 ? "" : "s");
                    return 0;
                }

                return r;
            }

            void seek(u64 offset) {
                position(offset);
            }

            u64 getPosition() {
                return position();
            }

            Buffer* subBuffer(u64 size) {
                u64 available = m_capacity - m_pos;
                if (size > available) {
                    gClient::Get()->getAPI()->scriptExceptionf(
                        "Attempted to read %llu byte%s from Buffer, but only %llu byte%s remain to be read",
                        size, size == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return nullptr;
                }

                utils::Buffer* b = new utils::Buffer(size);
                if (!read(b, size)) {
                    gClient::Get()->getAPI()->scriptExceptionf("Failed to read %d byte%s from buffer", size, size == 1 ? "" : "s");
                    delete b;
                    return nullptr;
                }

                b->position(0);
                return b;
            }

            char* readNullTerminatedString() {
                if (at_end()) {
                    gClient::Get()->getAPI()->scriptException(
                        "Attempted to read null-terminated string from Buffer, but the buffer read position was already at the end of the buffer"
                    );
                    return nullptr;
                }

                u32 len = strlen((char*)(m_data + m_pos));
                if (len == 0) return nullptr;

                u64 available = m_capacity - m_pos;
                if (len > available) {
                    gClient::Get()->getAPI()->scriptExceptionf(
                        "Attempted read of null-terminated string would result in reading %llu byte%s from Buffer, but only %llu byte%s remain to be read. Are you sure it's null-terminated?",
                        len, len == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return nullptr;
                }

                u8* result = new u8[len];
                if (!read(result, len)) {
                    gClient::Get()->getAPI()->scriptExceptionf("Failed to read %d byte%s from buffer", len, len == 1 ? "" : "s");
                    delete [] result;
                    return nullptr;
                }

                return (char*)result;
            }

            char* readString(u32 len) {
                u64 available = m_capacity - m_pos;
                if (len > available) {
                    gClient::Get()->getAPI()->scriptExceptionf(
                        "Attempted to read %llu byte%s from Buffer, but only %llu byte%s remain to be read",
                        len, len == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return nullptr;
                }

                u8* result = new u8[len];
                if (!read(result, len)) {
                    gClient::Get()->getAPI()->scriptExceptionf("Failed to read %d byte%s from buffer", len, len == 1 ? "" : "s");
                    delete [] result;
                    return nullptr;
                }

                return (char*)result;
            }

            bool writeString(const char* str) {
                u32 len = strlen(str);
                if (len == 0) return false;

                u64 available = m_capacity - m_pos;
                if (len > available) {
                    gClient::Get()->error(
                        "Attempted write of string would result in writing %llu byte%s to Buffer, but only %llu byte%s remain in the buffer",
                        len, len == 1 ? "" : "s", available, available == 1 ? "" : "s"
                    );
                    return false;
                }

                return write(str, len);
            }
    
            bool saveToFile(const char* path) {
                return save(path);
            }
    };

    struct FileStatus {
        enum class Type {
            Unknown,
            File,
            Directory,
            Pipe,
            Socket,
            CharacterDevice,
            BlockDevice,
            SymLink
        };

        Type type;
        u64 size;
        u64 lastWriteTime;
    };

    struct DirectoryEntry {
        char* path;
        char* name;
        char* extension;
        FileStatus status;
    };

    utils::Array<DirectoryEntry>* listDir(const char* path) {
        struct stat dirStat;
        if (stat(path, &dirStat) != 0) {
            gClient::Get()->getAPI()->scriptExceptionf("Could not find specified directory '%s'", path);
            return new utils::Array<DirectoryEntry>();
        }

        try {
            std::filesystem::directory_iterator dir(path);
            utils::Array<DirectoryEntry>* entries = new utils::Array<DirectoryEntry>();
            for (const std::filesystem::directory_entry& entry : dir) {
                utils::String path = entry.path().string();
                utils::String name = entry.path().filename().string();
                utils::String ext = entry.path().extension().string();

                path.replaceAll('\\', '/');

                char* sPath = new char[path.size() + 1];
                strncpy(sPath, path.c_str(), path.size());
                sPath[path.size()] = 0;

                char* sName = nullptr;
                sName = new char[name.size() + 1];
                strncpy(sName, name.c_str(), name.size());
                sName[name.size()] = 0;

                char* sExt = nullptr;
                if (ext.size() > 0) {
                    sExt = new char[ext.size() + 1];
                    strncpy(sExt, ext.c_str(), ext.size());
                    sExt[ext.size()] = 0;
                }

                FileStatus::Type type;
                if (entry.is_other()) type = FileStatus::Type::Unknown;
                else if (entry.is_regular_file()) type = FileStatus::Type::File;
                else if (entry.is_directory()) type = FileStatus::Type::Directory;
                else if (entry.is_fifo()) type = FileStatus::Type::Pipe;
                else if (entry.is_socket()) type = FileStatus::Type::Socket;
                else if (entry.is_character_file()) type = FileStatus::Type::CharacterDevice;
                else if (entry.is_block_file()) type = FileStatus::Type::BlockDevice;
                else if (entry.is_symlink()) type = FileStatus::Type::SymLink;

                entries->push({
                    sPath,
                    sName,
                    sExt,
                    {
                        type,
                        u64(entry.file_size()),
                        u64(std::chrono::duration_cast<std::chrono::milliseconds>(entry.last_write_time().time_since_epoch()).count())
                    }
                });
            }
            return entries;
        } catch (const std::exception& e) {
            gClient::Get()->getAPI()->scriptException(e.what());
        }

        return nullptr;
    }

    void BindBuffer(IScriptAPI* api) {
        api->beginNamespace("fs");

        DataType* buf = api->bind<utils::Buffer>("Buffer");
        DataType* fs = api->bind<FileStatus>("FileStatus");
        DataType* det = api->bind<FileStatus::Type>("FileType");
        DataType* de = api->bind<DirectoryEntry>("DirectoryEntry");
        DataType* om = api->bind<FileOpenMode>("OpenMode");
        
        //
        // Buffer
        //
        buf->bind("readUInt8", &BufferWrapper::typedRead<u8>);
        buf->bind("writeUInt8", &BufferWrapper::typedWrite<u8>)->setArgNames({ "value" });
        buf->bind("readUInt16", &BufferWrapper::typedRead<u16>);
        buf->bind("writeUInt16", &BufferWrapper::typedWrite<u16>)->setArgNames({ "value" });
        buf->bind("readUInt32", &BufferWrapper::typedRead<u32>);
        buf->bind("writeUInt32", &BufferWrapper::typedWrite<u32>)->setArgNames({ "value" });
        buf->bind("readUInt64", &BufferWrapper::typedRead<u64>);
        buf->bind("writeUInt64", &BufferWrapper::typedWrite<u64>)->setArgNames({ "value" });
        buf->bind("readInt8", &BufferWrapper::typedRead<i8>);
        buf->bind("writeInt8", &BufferWrapper::typedWrite<i8>)->setArgNames({ "value" });
        buf->bind("readInt16", &BufferWrapper::typedRead<i16>);
        buf->bind("writeInt16", &BufferWrapper::typedWrite<i16>)->setArgNames({ "value" });
        buf->bind("readInt32", &BufferWrapper::typedRead<i32>);
        buf->bind("writeInt32", &BufferWrapper::typedWrite<i32>)->setArgNames({ "value" });
        buf->bind("readInt64", &BufferWrapper::typedRead<i64>);
        buf->bind("writeInt64", &BufferWrapper::typedWrite<i64>)->setArgNames({ "value" });
        buf->bind("readFloat32", &BufferWrapper::typedRead<f32>);
        buf->bind("writeFloat32", &BufferWrapper::typedWrite<f32>)->setArgNames({ "value" });
        buf->bind("readFloat64", &BufferWrapper::typedRead<f64>);
        buf->bind("writeFloat64", &BufferWrapper::typedWrite<f64>)->setArgNames({ "value" });
        buf->bind("getPosition", &BufferWrapper::getPosition);
        buf->bind("getSize", &BufferWrapper::size);
        buf->bind("getCapacity", &BufferWrapper::capacity);
        buf->bind("getRemaining", &BufferWrapper::remaining);
        buf->bind("isAtEnd", &BufferWrapper::at_end);
        buf->bind("seek", &BufferWrapper::seek)->setArgNames({ "offsetFromStart" });
        buf->bind("subBuffer", &BufferWrapper::subBuffer)->setArgNames({ "size" });
        buf->bind("readNullTerminatedString", &BufferWrapper::readNullTerminatedString)->setSignatureFlags(FunctionSignature::DeallocateReturnAfterCall);
        buf->bind("readString", &BufferWrapper::readString)->setArgNames({ "length" })->setSignatureFlags(FunctionSignature::DeallocateReturnAfterCall);
        buf->bind("writeString", &BufferWrapper::writeString)->setArgNames({ "str" });
        buf->bind("saveToFile", &BufferWrapper::saveToFile)->setArgNames({ "path" });

        //
        // FileStatus
        //
        fs->bind("type", &FileStatus::type);
        fs->bind("size", &FileStatus::size);
        fs->bind("lastWriteTime", &FileStatus::lastWriteTime);

        //
        // DirectoryEntry
        //
        de->bind("path", &DirectoryEntry::path)->flags.is_nullable = 0;
        de->bind("name", &DirectoryEntry::name)->flags.is_nullable = 0;
        de->bind("extension", &DirectoryEntry::extension);
        de->bind("status", &DirectoryEntry::status)->flags.use_v8_accessors = 0;

        //
        // DirectoryEntryType
        //
        det->bindEnumValue("Unknown", u32(FileStatus::Type::Unknown));
        det->bindEnumValue("File", u32(FileStatus::Type::File));
        det->bindEnumValue("Directory", u32(FileStatus::Type::Directory));
        det->bindEnumValue("Pipe", u32(FileStatus::Type::Pipe));
        det->bindEnumValue("Socket", u32(FileStatus::Type::Socket));
        det->bindEnumValue("CharacterDevice", u32(FileStatus::Type::CharacterDevice));
        det->bindEnumValue("BlockDevice", u32(FileStatus::Type::BlockDevice));
        det->bindEnumValue("SymLink", u32(FileStatus::Type::SymLink));

        //
        // OpenMode
        //
        om->bindEnumValue("Text", u32(FileOpenMode::Text));
        om->bindEnumValue("Binary", u32(FileOpenMode::Binary));

        //
        // 'fs' functions
        //
        api->bind("bufferFromFile", +[](const char* path, FileOpenMode mode){
            return utils::Buffer::FromFile(path, mode == FileOpenMode::Text);
        })->setArgNames({ "filePath" });
        
        api->bind("createBuffer", +[](u32 capacity) {
            return new utils::Buffer(capacity);
        })->setArgNames({ "capacity" })->setReturnNullable(false);
        
        api->bind("destroyBuffer", +[](utils::Buffer* buf){
            delete buf;
        })->setArgNames({ "buffer" });

        api->bind("listDirectoryEntries", &listDir)
        ->setArgNames({ "dirPath" })
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnValueDeallocationCallback(+[](u8* ptr) {
            utils::Array<DirectoryEntry>* arr = (utils::Array<DirectoryEntry>*)ptr;

            arr->each([](const DirectoryEntry& ent) {
                if (ent.path) delete [] ent.path;
                if (ent.name) delete [] ent.name;
                if (ent.extension) delete [] ent.extension;
            });

            delete (utils::Array<DirectoryEntry>*)ptr;
        });

        api->bind("stat", +[](const char* path) {
            struct stat fstat;
            if (stat(path, &fstat) != 0) {
                return (FileStatus*)nullptr;
            }

            FileStatus::Type type;
            if (fstat.st_mode == _S_IFDIR) type = FileStatus::Type::Unknown;
            else if (fstat.st_mode == _S_IFREG) type = FileStatus::Type::File;
            else if (fstat.st_mode == _S_IFDIR) type = FileStatus::Type::Directory;
            else if (fstat.st_mode == _S_IFIFO) type = FileStatus::Type::Pipe;
            else if (fstat.st_mode == _S_IFCHR) type = FileStatus::Type::CharacterDevice;
            // other types apparently don't exist to stat...

            u8* data = new u8[sizeof(FileStatus)];

            ((FileStatus*)data)->size = fstat.st_size;
            ((FileStatus*)data)->lastWriteTime = fstat.st_mtime * 1000;
            ((FileStatus*)data)->type = type;
            
            return (FileStatus*)data;
        })
        ->setArgNames({ "path" })
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall);

        api->bind("relativePath", +[](const char* input, const char* relativeTo) {
            char* result = nullptr;

            try {
                utils::String str = std::filesystem::relative(
                    std::filesystem::path(input),
                    relativeTo
                ).string();

                if (str.size() > 0) {
                    str.replaceAll('\\', '/');
                    result = (char*)new u8[str.size() + 1];
                    strncpy(result, str.c_str(), str.size());
                    result[str.size()] = 0;
                }
            } catch (const std::exception& e) {
                gClient::Get()->getAPI()->scriptException(e.what());
            }

            return result;
        })
        ->setArgNames({ "inputPath", "relativeTo" })
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall);

        api->bind("createDirectory", +[](const char* path) {
            try {
                if (std::filesystem::create_directories(path)) return true;
            } catch (const std::exception& e) {
                gClient::Get()->getAPI()->scriptException(e.what());
            }

            return false;
        })->setArgNames({ "path" });

        api->bind("exists", +[](const char* path) {
            try {
                if (std::filesystem::exists(path)) return true;
            } catch (const std::exception& e) {
                gClient::Get()->getAPI()->scriptException(e.what());
            }

            return false;
        })->setArgNames({ "path" });

        api->endNamespace();
    }

    void BindMath(IScriptAPI* api) {
        DataType* v2f = api->bind<utils::vec2f>("basic_vec2f");
        DataType* v3f = api->bind<utils::vec3f>("basic_vec3f");
        DataType* v4f = api->bind<utils::vec4f>("basic_vec4f");
        DataType* qf = api->bind<utils::quatf>("basic_quatf");
        DataType* v2i = api->bind<utils::vec2i>("basic_vec2i");
        DataType* v3i = api->bind<utils::vec3i>("basic_vec3i");
        DataType* v4i = api->bind<utils::vec4i>("basic_vec4i");
        DataType* m3f = api->bind<utils::mat3f>("basic_mat3f");
        DataType* m4f = api->bind<utils::mat4f>("basic_mat4f");

        v2f->setFlags(DataType::Flags::HostConstructionNotRequired);
        v2f->bind("x", &utils::vec2f::x);
        v2f->bind("y", &utils::vec2f::y);

        v3f->setFlags(DataType::Flags::HostConstructionNotRequired);
        v3f->bind("x", &utils::vec3f::x);
        v3f->bind("y", &utils::vec3f::y);
        v3f->bind("z", &utils::vec3f::z);

        v4f->setFlags(DataType::Flags::HostConstructionNotRequired);
        v4f->bind("x", &utils::vec4f::x);
        v4f->bind("y", &utils::vec4f::y);
        v4f->bind("z", &utils::vec4f::z);
        v4f->bind("w", &utils::vec4f::w);

        qf->setFlags(DataType::Flags::HostConstructionNotRequired);
        qf->bind("axis", &utils::quatf::axis);
        qf->bind("angle", &utils::quatf::angle);

        v2i->setFlags(DataType::Flags::HostConstructionNotRequired);
        v2i->bind("x", &utils::vec2i::x);
        v2i->bind("y", &utils::vec2i::y);

        v3i->setFlags(DataType::Flags::HostConstructionNotRequired);
        v3i->bind("x", &utils::vec3i::x);
        v3i->bind("y", &utils::vec3i::y);
        v3i->bind("z", &utils::vec3i::z);

        v4i->setFlags(DataType::Flags::HostConstructionNotRequired);
        v4i->bind("x", &utils::vec4i::x);
        v4i->bind("y", &utils::vec4i::y);
        v4i->bind("z", &utils::vec4i::z);
        v4i->bind("w", &utils::vec4i::w);

        m3f->setFlags(DataType::Flags::HostConstructionNotRequired);
        m3f->bind("x", &utils::mat3f::x);
        m3f->bind("y", &utils::mat3f::y);
        m3f->bind("z", &utils::mat3f::z);

        m4f->setFlags(DataType::Flags::HostConstructionNotRequired);
        m4f->bind("x", &utils::mat4f::x);
        m4f->bind("y", &utils::mat4f::y);
        m4f->bind("z", &utils::mat4f::z);
        m4f->bind("w", &utils::mat4f::w);
    }

    void BindBuiltins(IScriptAPI* api) {
        api->bind<bool>("boolean")->setFlags(DataType::Flags::IsHidden);
        api->bind<char>("char");
        api->bind<i8>("i8");
        api->bind<i16>("i16");
        api->bind<i32>("i32");
        api->bind<i64>("i64");
        api->bind<u8>("u8");
        api->bind<u16>("u16");
        api->bind<u32>("u32");
        api->bind<u64>("u64");
        api->bind<f32>("f32");
        api->bind<f64>("f64");
        api->bind<pointer>("pointer");

        BindMath(api);
        BindBuffer(api);
    }
};