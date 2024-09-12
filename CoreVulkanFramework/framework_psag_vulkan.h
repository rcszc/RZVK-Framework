// framework_psag_vulkan. RCSZ.
// rzvk_framework => pomelostar_2d_framework. 
#ifndef _FRAMEWORK_PSAG_VULKAN_H
#define _FRAMEWORK_PSAG_VULKAN_H
#if defined(RZVK_DEFFLAG_ENABLE_SHADERC)
#include <shaderc/shaderc.hpp>
#endif
#include "framework_vulkan.hpp"

enum ShaderProgramType {
	ShaderProgramVert = 1 << 1,
	ShaderProgramFrag = 1 << 2
};
// rzvk => psag_framework spirv.
namespace PSAGameGraphicSPIRV {
	// psagame2d low: [GraphicSPIRV] module file_loader.
	class PasgLowFileLoader {
	protected:
		std::vector<char> BinaryFileLoader(const std::string& file_path, bool* flag);
		std::string       StringFileLoader(const std::string& file_path, bool* flag);

		bool StringFileWrite(const std::string& file_path, const std::string& str);
	};

#if defined(RZVK_DEFFLAG_ENABLE_SHADERC)
	// source_code: string, binary_code: spir_v.
	struct ResourceCacheINFO {
		std::string ProgramSourceCode = {};
		RZ_VULKAN::RzVkShaderSPIRV ProgramBinaryCode = {};
	};
	// shader_program [spir_v] processing & storage.
	// libray: vulkan_sdk shaderc.
	class ShaderProgramResourceSPIRV :public PasgLowFileLoader {
	protected:
		std::unordered_map<std::string, ResourceCacheINFO> ShaderResourceMap = {};

		std::vector<uint32_t> CompileShaderSourceCode(
			const std::string& source, std::string* compile_msg, shaderc_shader_kind kind
		);
	public:
		// load shader => compile => storage => find: key = tag_name.
		bool ShaderSourceCodeLoad(const std::string& code, ShaderProgramType shader_type, const std::string& tag_name);
		bool ShaderSourceCodeFileLoad(const std::string& file, ShaderProgramType shader_type, const std::string& tag_name);

		RZ_VULKAN::RzVkShaderSPIRV FindShaderProgram(const std::string& tag_name);
		size_t ShaderResourceMapSize();
	};
#endif

	struct ResourceIndexCacheINFO {
		std::string ProgramSourceFile = {};
		std::string ProgramBinaryFile = {};

		size_t CacheFilesSizeBytes = NULL;
	};
	// shader_program [spir_v] processing & cache_files & storage.
	// cache: src_file: text, spv_file: binary.
	class ShaderProgramCacheResourceSPIRV :public PasgLowFileLoader {
	private:
		int64_t GenRandomSeedCount = 1;
	protected:
		std::unordered_map<std::string, ResourceIndexCacheINFO> ShaderIndexesMap = {};
		std::unordered_set<uint32_t> UniqueIndexesCode = {};

		std::string CompilerFolderpath = {};
		std::string CacheFolderpath    = {};

		bool CheckCacheFolder(const std::string* path);
		std::string FormatUnqiueCode(uint32_t code, uint32_t fmt);

		uint32_t UnqiueIndexesAlloc();
		bool     UnqiueIndexesDelete(uint32_t code);

		bool ProcessingSourceCode(ResourceIndexCacheINFO* config, const FmtVector2T<std::string>& params);
	public:
		ShaderProgramCacheResourceSPIRV(const FmtVector2T<std::string>& path);
		~ShaderProgramCacheResourceSPIRV();

		bool ShaderSourceCodeLoad(const std::string& code, ShaderProgramType shader_type, const std::string& tag_name);
		bool ShaderSourceCodeFileLoad(const std::string& file, ShaderProgramType shader_type, const std::string& tag_name);

		RZ_VULKAN::RzVkShaderSPIRV FindShaderProgram(const std::string& tag_name);

		size_t ShaderResourceMapSize();
		size_t ShaderResourceBinFilesSize();
	};
}

// psagame2d render_module & commands. 
namespace PSAGameGraphicRender {

}

#endif