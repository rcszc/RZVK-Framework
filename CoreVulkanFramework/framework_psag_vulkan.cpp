// framework_psag_vulkan.
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <random>
#include "framework_psag_vulkan.h"

using namespace std;
using namespace RZ_VULKAN;

namespace PSAGameGraphicSPIRV {
	RZ_VULKAN_DEFLABEL RZVK_PSAGLOW_SPIRV = "rzvk_psaglow_spirv";

	string PasgLowFileLoader::StringFileLoader(const string& file_path, bool* flag) {
		ifstream TextFileRead(file_path);
		// read_file status.
		if (!TextFileRead.is_open()) {
			*flag = false;
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "file(str)_loader failed open: %s",
				file_path.c_str()
			));
			return string();
		}
		// get file size.
		TextFileRead.seekg(0, ios::end);
		size_t ReadFileSize = (size_t)TextFileRead.tellg();
		TextFileRead.seekg(0, ios::beg);

		// read string data.
		string FileContent((istreambuf_iterator<char>(TextFileRead)), istreambuf_iterator<char>());
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "file(str)_loader read file: %s read_size: %u",
			file_path.c_str(), ReadFileSize
		));
		TextFileRead.close();
		*flag = true;
		return FileContent;
	}

	vector<char> PasgLowFileLoader::BinaryFileLoader(const string& file_path, bool* flag) {
		vector<char> BinaryLoader = {};
		ifstream BinaryFileRead(file_path, ios::binary);
		// read_file status.
		if (!BinaryFileRead.is_open()) {
			*flag = false;
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "file(bin)_loader failed open: %s",
				file_path.c_str()
			));
			return BinaryLoader;
		}
		// get file size.
		BinaryFileRead.seekg(0, ios::end);
		size_t ReadFileSize = (size_t)BinaryFileRead.tellg();
		BinaryFileRead.seekg(0, ios::beg);

		// read binary data.
		BinaryLoader.resize(ReadFileSize);
		BinaryFileRead.read(BinaryLoader.data(), ReadFileSize);

		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "file(bin)_loader read file: %s read_size: %u",
			file_path.c_str(), ReadFileSize
		));
		*flag = true;
		BinaryFileRead.close();
		return BinaryLoader;
	}

	bool PasgLowFileLoader::StringFileWrite(const string& file_path, const string& str) {
		fstream WriteFile(file_path, ios::out);
		// write_file status.
		if (!WriteFile.is_open()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "file(str)_write failed open: %s",
				file_path.c_str()
			));
			return false;
		}
		// write string data. 
		WriteFile.write(str.data(), str.size());
		WriteFile.close();
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "file(str)_write file: %s", file_path.c_str()));
		return true;
	}

#if defined(RZVK_DEFFLAG_ENABLE_SHADERC)
	vector<uint32_t> ShaderProgramResourceSPIRV::CompileShaderSourceCode(
		const string& source, string* compile_msg, shaderc_shader_kind kind
	) {
		shaderc::Compiler ToolCompiler = {};
		shaderc::SpvCompilationResult ProgramModule = ToolCompiler.CompileGlslToSpv(source, kind, "shader");

		vector<uint32_t> ShaderProgramBinary = {};
		if (ProgramModule.GetCompilationStatus() != shaderc_compilation_status_success) {
			*compile_msg = ProgramModule.GetErrorMessage();
			return ShaderProgramBinary;
		}
		ShaderProgramBinary.insert(ShaderProgramBinary.begin(), ProgramModule.cbegin(), ProgramModule.cend());
		return ShaderProgramBinary;
	}

	bool ShaderProgramResourceSPIRV::ShaderSourceCodeLoad(
		const string& code, ShaderProgramType shader_type, const string& tag_name
	) {
		if (code.empty()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader source_code empty."));
			return false;
		}
		auto it = ShaderResourceMap.find(tag_name);
		if (it != ShaderResourceMap.end()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader invaild_key(key_name)."));
			return false;
		}
		RzVkShaderSPIRV BinaryProgramTemp = {};
		string CompilerErrorMessage = {};

		switch (shader_type) {
		case(ShaderProgramVert): {
			auto BinaryOut = CompileShaderSourceCode(code, &CompilerErrorMessage, shaderc_glsl_vertex_shader);
			// check compile vertex error_message => print.
			if (!CompilerErrorMessage.empty()) {
				RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader compile(vert) msg:"));
				RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "%s", CompilerErrorMessage.c_str()));
				return false;
			}
			// copy_data: compiler => vulkan_module read_format.
			memcpy(BinaryProgramTemp.data(), BinaryOut.data(), BinaryOut.size() * sizeof(uint32_t));
			RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader vert compilation_successful."));
			break;
		}
		case(ShaderProgramFrag): {
			auto BinaryOut = CompileShaderSourceCode(code, &CompilerErrorMessage, shaderc_glsl_fragment_shader);
			// check compile fragment error_message => print.
			if (!CompilerErrorMessage.empty()) {
				RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader compile(frag) msg:"));
				RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "%s", CompilerErrorMessage.c_str()));
				return false;
			}
			// copy_data: compiler => vulkan_module read_format.
			memcpy(BinaryProgramTemp.data(), BinaryOut.data(), BinaryOut.size() * sizeof(uint32_t));
			RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader frag compilation_successful."));
			break;
		}}
		ResourceCacheINFO CacheInfoTemp = {};
		CacheInfoTemp.ProgramSourceCode = code;
		CacheInfoTemp.ProgramBinaryCode = BinaryProgramTemp;
		// storage shader program.
		ShaderResourceMap[tag_name] = CacheInfoTemp;
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader program storage: %s", tag_name.c_str()));
		return true;
	}

	bool ShaderProgramResourceSPIRV::ShaderSourceCodeFileLoad(
		const string& file, ShaderProgramType shader_type, const string& tag_name
	) {
		bool FileState = false;
		string Source = StringFileLoader(file, &FileState);
		if (!FileState) {
			RZVK_FUNC_LOGGER(RzVkLog(LogWarning, RZVK_PSAGLOW_SPIRV, "shader failed read_file."));
			return false;
		}
		return ShaderSourceCodeLoad(Source, shader_type, tag_name);
	}

	RZ_VULKAN::RzVkShaderSPIRV ShaderProgramResourceSPIRV::FindShaderProgram(const string& tag_name) {
		auto it = ShaderResourceMap.find(tag_name);
		if (it == ShaderResourceMap.end()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogWarning, RZVK_PSAGLOW_SPIRV, "shader program_find: not key."));
			return RZ_VULKAN::RzVkShaderSPIRV();
		}
		return ShaderResourceMap[tag_name].ProgramBinaryCode;
	}

	size_t ShaderProgramResourceSPIRV::ShaderResourceMapSize() {
		return ShaderResourceMap.size();
	}
#endif

	bool ShaderProgramCacheResourceSPIRV::CheckCacheFolder(const string* path) {
		if (path == nullptr)
			return false;
		filesystem::path FolderPath(*path);
		return filesystem::exists(FolderPath) && filesystem::is_directory(FolderPath);
	}

	constexpr uint32_t CONST_GEN_CODE_DIGIT = 8;
	string ShaderProgramCacheResourceSPIRV::FormatUnqiueCode(uint32_t code, uint32_t fmt) {
		ostringstream OSSFMT;
		OSSFMT << setw(fmt) << setfill('0') << code;
		return OSSFMT.str();
	}

	uint32_t ShaderProgramCacheResourceSPIRV::UnqiueIndexesAlloc() {
		bool RandomGenFlag = true;
		uint32_t UniqueCodeTemp = NULL;

		while (RandomGenFlag) {
			mt19937_64 MtGenerator(++GenRandomSeedCount);
			uniform_int_distribution<uint32_t> Distribution(1, 33554432);
			UniqueCodeTemp = Distribution(MtGenerator);
			// check unique_code valid.
			if (UniqueIndexesCode.find(UniqueCodeTemp) == UniqueIndexesCode.end())
				RandomGenFlag = false;
		}
		UniqueIndexesCode.insert(UniqueCodeTemp);
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "cache unique_index: gen_key: %u", UniqueCodeTemp));
		return UniqueCodeTemp;
	}

	bool ShaderProgramCacheResourceSPIRV::UnqiueIndexesDelete(uint32_t code) {
		if (UniqueIndexesCode.find(code) == UniqueIndexesCode.end()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogWarning, RZVK_PSAGLOW_SPIRV, "cache unique_index: failed delete, not key."));
			return false;
		}
		UniqueIndexesCode.erase(code);
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "cache unique_index: delete_key: %u", code));
		return true;
	}

	inline string CONVERT_FOLDER_PATH(const string& folder) {
		// file path: relative => absolute.
		filesystem::path AbsolutePath = filesystem::absolute(folder);
		return AbsolutePath.string();
	}

	inline void FORMAT_FOLDER_SEGMENT(string* folder) {
		// add folder path_segment '\'.
		if (folder->back() != '\\') folder->push_back('\\');
	}

	ShaderProgramCacheResourceSPIRV::ShaderProgramCacheResourceSPIRV(const FmtVector2T<string>& path) {
		// check cache filepath.
		if (!CheckCacheFolder(&path.vector_y)) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "cache_folder path invalid."));
			return;
		}
		CacheFolderpath = path.vector_y;
		// check compiler filepath.
		if (!CheckCacheFolder(&path.vector_x)) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "compiler_folder path invalid."));
			return;
		}
		CompilerFolderpath = path.vector_x;
		// convert folder_path, relative => absolute.
		CacheFolderpath    = CONVERT_FOLDER_PATH(CacheFolderpath);
		CompilerFolderpath = CONVERT_FOLDER_PATH(CompilerFolderpath);
		// processing folder_path.
		FORMAT_FOLDER_SEGMENT(&CacheFolderpath);
		FORMAT_FOLDER_SEGMENT(&CompilerFolderpath);
	}

	ShaderProgramCacheResourceSPIRV::~ShaderProgramCacheResourceSPIRV() {
		for (auto InfoItem : ShaderIndexesMap) {
			// delete cache_files.
			remove(InfoItem.second.ProgramBinaryFile.c_str());
			remove(InfoItem.second.ProgramSourceFile.c_str());
		}
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader cache_files clear: %s", CacheFolderpath.c_str()));
	}

	bool ShaderProgramCacheResourceSPIRV::ProcessingSourceCode(
		ResourceIndexCacheINFO* config, const FmtVector2T<string>& params
	) {
		string UniqueFileName = FormatUnqiueCode(UnqiueIndexesAlloc(), CONST_GEN_CODE_DIGIT);
		string PATH_STRING = CacheFolderpath + UniqueFileName + "." + params.vector_y;
		string PATH_BINARY = CacheFolderpath + UniqueFileName + ".spv";
		// write string_file => check.
		if (!StringFileWrite(PATH_STRING, params.vector_x)) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader failed write cache_file."));
			return false;
		}
		config->ProgramSourceFile = PATH_STRING;
		config->ProgramBinaryFile = PATH_BINARY;
		// execute spirv compilation...
		string COMP_CMD = CompilerFolderpath + "glslc " + PATH_STRING + " -o " + PATH_BINARY;
		if(system(COMP_CMD.c_str()) != 0)
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader %s compilation_failed.", params.vector_y.c_str()));
		// get binary file_size bytes.
		config->CacheFilesSizeBytes = (size_t)filesystem::file_size(PATH_BINARY);
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader %s compilation_success.", params.vector_y.c_str()));
		return true;
	}

	bool ShaderProgramCacheResourceSPIRV::ShaderSourceCodeLoad(
		const string& code, ShaderProgramType shader_type, const string& tag_name
	) {
		if (code.empty()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader source_code empty."));
			return false;
		}
		auto it = ShaderIndexesMap.find(tag_name);
		if (it != ShaderIndexesMap.end()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogError, RZVK_PSAGLOW_SPIRV, "shader invaild_key(key_name)."));
			return false;
		}
		ResourceIndexCacheINFO IndexInfoTemp = {};

		switch (shader_type) {
		case(ShaderProgramVert): {
			if (!ProcessingSourceCode(&IndexInfoTemp, FmtVector2T<string>(code, "vert")))
				return false;
			break;
		}
		case(ShaderProgramFrag): {
			if (!ProcessingSourceCode(&IndexInfoTemp, FmtVector2T<string>(code, "frag")))
				return false;
			break;
		}}
		// storage shader index.
		ShaderIndexesMap[tag_name] = IndexInfoTemp;
		RZVK_FUNC_LOGGER(RzVkLog(LogInfo, RZVK_PSAGLOW_SPIRV, "shader program(idx) storage: %s", tag_name.c_str()));
		return true;
	}

	bool ShaderProgramCacheResourceSPIRV::ShaderSourceCodeFileLoad(
		const string& file, ShaderProgramType shader_type, const string& tag_name
	) {
		bool FileState = false;
		string Source = StringFileLoader(file, &FileState);
		if (!FileState) {
			RZVK_FUNC_LOGGER(RzVkLog(LogWarning, RZVK_PSAGLOW_SPIRV, "shader failed read_file."));
			return false;
		}
		return ShaderSourceCodeLoad(Source, shader_type, tag_name);
	}

	RzVkShaderSPIRV ShaderProgramCacheResourceSPIRV::FindShaderProgram(const string& tag_name) {
		auto it = ShaderIndexesMap.find(tag_name);
		if (it == ShaderIndexesMap.end()) {
			RZVK_FUNC_LOGGER(RzVkLog(LogWarning, RZVK_PSAGLOW_SPIRV, "shader program(idx)_find: not key."));
			return RzVkShaderSPIRV();
		}
		bool READ_BIN_FLAG = false;
		return (RzVkShaderSPIRV)BinaryFileLoader(ShaderIndexesMap[tag_name].ProgramBinaryFile, &READ_BIN_FLAG);
	}

	size_t ShaderProgramCacheResourceSPIRV::ShaderResourceMapSize() {
		return ShaderIndexesMap.size();
	}

	size_t ShaderProgramCacheResourceSPIRV::ShaderResourceBinFilesSize() {
		size_t FilesBytes = NULL;
		for (auto& Items : ShaderIndexesMap)
			FilesBytes += Items.second.CacheFilesSizeBytes;
		return FilesBytes;
	}
}