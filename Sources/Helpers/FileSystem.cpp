#include "FileSystem.hpp"

#include <cassert>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef ACID_BUILD_WINDOWS
#include <io.h>
#include <direct.h>
typedef struct _stat STAT;
#define stat _stat
#define S_IFREG _S_IFREG
#define S_IFDIR _S_IFDIR
#define access _access
#define mkdir _mkdir
#define rmdir _rmdir
#define GetCurrentDir _getcwd
#else
typedef struct stat STAT;
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace acid
{
#ifdef ACID_BUILD_WINDOWS
	const std::string FileSystem::SEPARATOR = "\\";
#else
	const std::string FileSystem::SEPARATOR = "/";
#endif

	bool FileSystem::Exists(const std::string &path)
	{
		STAT st;

		if (stat(path.c_str(), & st) == -1)
		{
			return false;
		}

#ifdef WIN32
		return (st.st_mode & S_IFREG) == S_IFREG || (st.st_mode & S_IFDIR) == S_IFDIR;
#else
		return S_ISREG(st.st_mode) || S_ISDIR(st.st_mode);
#endif
	}

	bool FileSystem::IsFile(const std::string &path)
	{
		STAT st;

		if (stat(path.c_str(), & st) == -1)
		{
			return false;
		}

#ifdef WIN32
		return (st.st_mode & S_IFREG) == S_IFREG;
#else
		return S_ISREG(st.st_mode);
#endif
	}

	bool FileSystem::IsDirectory(const std::string &path)
	{
		STAT st;

		if (stat(path.c_str(), & st) == -1)
		{
			return false;
		}

#ifdef ACID_BUILD_WINDOWS
		return (st.st_mode & S_IFDIR) == S_IFDIR;
#else
		return S_ISDIR(st.st_mode);
#endif
	}

	bool FileSystem::IsReadable(const std::string &path)
	{
		return access(path.c_str(), 0x4) == 0;
	}

	bool FileSystem::IsWriteable(const std::string &path)
	{
		return access(path.c_str(), 0x2) == 0;
	}

	std::vector<std::string> FileSystem::FilesInPath(const std::string &path)
	{
		std::vector<std::string> result = {};

		struct dirent *de;
		DIR *dr = opendir(path.c_str());

		if (dr == nullptr)
		{
			Log::Error("Could not open current directory: '%s'!\n", path.c_str());
			return result;
		}

		while ((de = readdir(dr)) != nullptr)
		{
			result.emplace_back(de->d_name);
		}

		closedir(dr);
		return result;
	}

	bool FileSystem::DeletePath(const std::string &path)
	{
		if (IsDirectory(path))
		{
			return rmdir(path.c_str()) == 0;
		}
		else if (IsFile(path))
		{
#ifdef ACID_BUILD_WINDOWS
			return ::remove(path.c_str()) == 0;
#else
			return ::remove(path.c_str()) == 0;
#endif
		}

		return false;
	}

	bool FileSystem::CreateFile(const std::string &filename, const bool &createFolders)
	{
		if (Exists(filename))
		{
			return false;
		}

		if (createFolders)
		{
			auto lastPos = filename.find_last_of("\\/");

			if (lastPos != std::string::npos)
			{
				CreateFolder(filename.substr(0, lastPos));
			}
		}

		FILE *file = fopen(filename.c_str(), "rb+");

		if (file == nullptr)
		{
			file = fopen(filename.c_str(), "wb");
		}

		fclose(file);
		return true;
	}

	bool FileSystem::ClearFile(const std::string &filename)
	{
		DeletePath(filename);
		bool created = CreateFile(filename);
		return created;
	}

	bool FileSystem::CreateFolder(const std::string &path)
	{
		int32_t nError = 0;

#ifdef ACID_BUILD_WINDOWS
		nError = _mkdir(path.c_str());
#else
		mode_t nMode = 0733;
		nError = mkdir(path.c_str(), nMode);
#endif

	//	assert(nError != 0 && "Could not create folder!");
		return nError == 0;
	}

	std::optional<std::string> FileSystem::ReadTextFile(const std::string &filename)
	{
		if (!FileSystem::Exists(filename))
		{
			Log::Error("File does not exist: '%s'\n", filename.c_str());
			return {};
		}

		FILE *file = fopen(filename.c_str(), "rb");

		if (file == nullptr)
		{
			Log::Error("Could not open file: '%s'\n", filename.c_str());
			return {};
		}

		fseek(file, 0, SEEK_END);
		int32_t length = ftell(file);
		assert(length < 100 * 1024 * 1024);
		std::string result(length, 0);
		fseek(file, 0, SEEK_SET);
		fread(&result[0], 1, length, file);
		fclose(file);

		result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
		return result;
	}

	bool FileSystem::WriteTextFile(const std::string &filename, const std::string &data)
	{
		FILE *file = fopen(filename.c_str(), "ab");

		if (file == nullptr)
		{
			return false;
		}

		fputs(data.c_str(), file);
		fclose(file);
		return true;
	}

	std::optional<std::vector<char>> FileSystem::ReadBinaryFile(const std::string &filename, const std::string &mode)
	{
		std::vector<char> data = {};

		const int32_t bufferSize = 1024;
		const bool useFile = filename.c_str() && strcmp("-", filename.c_str());

		if (FILE *fp = (useFile ? fopen(filename.c_str(), mode.c_str()) : stdin))
		{
			char buf[bufferSize];

			while (size_t len = fread(buf, sizeof(char), bufferSize, fp))
			{
				data.insert(data.end(), buf, buf + len);
			}

			if (ftell(fp) == -1L)
			{
				if (ferror(fp))
				{
					Log::Error("Error reading file: '%s'\n", filename.c_str());
					return {};
				}
			}
			else
			{
				if (ftell(fp) % sizeof(char))
				{
					Log::Error("Corrupted word found in file: '%s'\n", filename.c_str());
					return {};
				}
			}

			if (useFile)
			{
				fclose(fp);
			}
		}
		else
		{
			Log::Error("File does not exist: '%s'\n", filename.c_str());
			return {};
		}

		return data;
	}

	bool FileSystem::WriteBinaryFile(const std::string &filename, const std::vector<char> &data, const std::string &mode)
	{
		const bool useStdout = !filename.c_str() || (filename.c_str()[0] == '-' && filename.c_str()[1] == '\0');

		if (FILE *fp = (useStdout ? stdout : fopen(filename.c_str(), mode.c_str())))
		{
			size_t written = fwrite(data.data(), sizeof(char), data.size(), fp);

			if (data.size() != written)
			{
				Log::Error("Could not write to file: '%s'\n", filename.c_str());
				return false;
			}

			if (!useStdout)
			{
				fclose(fp);
			}
		}
		else
		{
			Log::Error("File could not be opened: '%s'\n", filename.c_str());
			return false;
		}

		return true;
	}

	std::string FileSystem::GetWorkingDirectory()
	{
		char buff[FILENAME_MAX];
		GetCurrentDir(buff, FILENAME_MAX);
		return buff;
	}

	std::string FileSystem::ParentDirectory(const std::string &path)
	{
		if (path.empty())
		{
			return path;
		}

#ifdef ACID_BUILD_WINDOWS
		std::string::size_type end = path.find_last_of(SEPARATOR + "/");
#else
		std::string::size_type end = path.find_last_of(SEPARATOR);
#endif

		if (end == path.length() - 1)
		{
#ifdef ACID_BUILD_WINDOWS
			end = path.find_last_of(SEPARATOR + "/", end);
#else
			end = path.find_last_of(SEPARATOR, end);
#endif
		}

		if (end == std::string::npos)
		{
			return "";
		}

		return path.substr(0, end);
	}

	std::string FileSystem::FileName(const std::string &path)
	{
		std::string::size_type start = path.find_last_of(SEPARATOR);

#ifdef ACID_BUILD_WINDOWS
		// WIN32 also understands '/' as the separator.
		if (start == std::string::npos)
		{
			start = path.find_last_of("/");
		}
#endif

		if (start == std::string::npos)
		{
			start = 0;
		}
		else
		{
			start++; // We do not want the separator.
		}

		return path.substr(start);
	}

	std::string FileSystem::FileSuffix(const std::string &path)
	{
		std::string::size_type start = path.find_last_of(SEPARATOR);

#ifdef ACID_BUILD_WINDOWS
		// WIN32 also understands '/' as the separator.
		if (start == std::string::npos)
		{
			start = path.find_last_of("/");
		}
#endif

		if (start == std::string::npos)
		{
			start = 0;
		}

		else
		{
			start++; // We do not want the separator.
		}

		std::string::size_type end = path.find_last_of(".");

		if (end == std::string::npos || end < start)
		{
			return "";
		}
		else
		{
			return path.substr(end);
		}
	}
}
