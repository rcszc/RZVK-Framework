// framework_vulkan. RCSZ.
// rz_vulkan_framework version 1.0.0 IPF
// [2023_11_22] vulkan 1.3.268.0
// [2024_08_28] restart. VkFramework => PSAGame2D.

#ifndef _FRAMEWORK_VULKAN_HPP
#define _FRAMEWORK_VULKAN_HPP
// psag_framework.
#ifndef POMELO_STAR_GAME2D_DEF
#include <iostream>
#include <cstdarg>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#endif
#include "framework_vulkan_define.h"

namespace RZ_VULKAN { struct RzVkLog; }

extern std::function<void(const RZ_VULKAN::RzVkLog&)> RZVK_FUNC_LOGGER;
extern std::function<void(int)> RZVK_FUNC_EXIT;
extern std::function<int64_t()> RZVK_FUNC_TIMENOW;

namespace VkFunctionLoader {
	// => [CALLBACK_FUNC]
	VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallbackPrint(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* ptr_callback_data,
		void* ptr_user_data
	);

	// => 'vkCreateDebugUtilsMessengerEXT'
	VkResult VKCreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* ptr_create_info,
		const VkAllocationCallbacks* ptr_allocator,
		VkDebugUtilsMessengerEXT* ptr_debug_messenger
	);

	// => 'vkDestroyDebugUtilsMessengerEXT'
	void VKDestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* ptr_allocator
	);
}

#define RZVK_NULL_FP32 0.0f
namespace RZ_VULKAN {
	namespace RenderCommand { struct CommandSubmit; }

	namespace CONTROLLER {
		// vk_framework state, resource state.
		struct ControllerStateQuery {
			// str_name key: "VAR", "VBR", "UBR", "CTR", "DTR", "SLR", "SPR".
			std::unordered_map<std::string, size_t> OnlineResourceItems = {};

			size_t ObjectsRenderPass    = NULL;
			size_t ObjectsRenderCommand = NULL;

			FmtVector2T<uint32_t> RzFrameworkWindowSize = {};
			std::string           RzFrameworkVersion    = {};
		};

		// vk_framework resource manager ptr_family.
		struct ControllerPointerFamily {
			RZ_VULKAN_RESOURCE::ResourceInterfaceVA* PointerVAR = nullptr; // vertex attribute res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceVB* PointerVBR = nullptr; // vertex buffer res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceUB* PointerUBR = nullptr; // uniform buffer res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceSL* PointerSLR = nullptr; // shader(pipeline) layout res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceSP* PointerSPR = nullptr; // shader(pipeline) res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceCT* PointerCTR = nullptr; // color texture res.
			RZ_VULKAN_RESOURCE::ResourceInterfaceDT* PointerDTR = nullptr; // depth texture res.
		};

		// manager objects counter. new[++], delete[--].
		class VulkanCoreControllerCounter {
		protected:
			static std::atomic<size_t> ObjectsRenderPass;
			static std::atomic<size_t> ObjectsRenderCommand;
		};

		// rz_vulkan core framework controller. 
		class VulkanCoreController :public VulkanCoreControllerCounter {
		private:
			ControllerPointerFamily VulkanFrameworkResource = {};
			bool VulkanFrameworkResourceValidFlag = false;
		protected:
			void CreateDynamicResourceManager(size_t init_map);
			// true: delete pointer_family success.
			bool DeleteDynamicResourceManager();

			ControllerStateQuery QueryRzVkFrameworkInfo();
			ControllerPointerFamily* GetRzVkFrameworkResourcePtr();
		};
	}

	// rzvk_system logger format.
	struct RzVkLog {
		std::string LOG_INFORMATION = {};
		std::string LOG_LABEL = {};
		LOGLABEL    LOG_LEVEL = {};

#define LOGGER_BUFFER_LEN 2048
		// log_format.
		RzVkLog(LOGLABEL LEVEL, const char* LABEL, const char* INFO_WAHT, ...) {
			char LogInfoTemp[LOGGER_BUFFER_LEN] = {};

			va_list ParamsArgs;
			va_start(ParamsArgs, INFO_WAHT);
			vsnprintf(LogInfoTemp, LOGGER_BUFFER_LEN, INFO_WAHT, ParamsArgs);
			va_end(ParamsArgs);

			LOG_LABEL = LABEL;
			LOG_LEVEL = LEVEL;
			LOG_INFORMATION = LogInfoTemp;
		}
		RzVkLog() : LOG_INFORMATION({}), LOG_LABEL({}) {}
	};

	// rzvk_system error_list format.
	struct RzVkError {
		RzVkLog Information; // what.
		int64_t ResultCode;  // vulkan result.

		RzVkError() : Information({}), ResultCode(NULL) {}
		RzVkError(const RzVkLog& logger, int64_t code) : Information(logger), ResultCode(code) {}
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily = {};
		std::optional<uint32_t> PresentFamily  = {};

		bool FamilyComplete() {
			// values valid, graphics_pamily & present_family.
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR        SCSD_Capabilities = {};
		std::vector<VkSurfaceFormatKHR> SCSD_Formats      = {};
		std::vector<VkPresentModeKHR>   SCSD_PresentModes = {};
	};

	class VulkanCoreError {
	protected:
		std::unordered_map<int64_t, RzVkError> RzVkResultErrorList = {};
		// get vulkan function_result error information.
		void ResultInfoGet(VkResult value, RzVkLog succ_info, RzVkLog info);
	};

	class VulkanCoreExtensions {
	protected:
		static std::vector<RzVkExtName> ExtensionEnableInstance;
		static std::vector<RzVkExtName> ExtensionEnableDevice;
	public:
		VulkanCoreExtensions();
	};

	// core static vulkan handles_resource.
	class VulkanCoreStaticResource {
	protected:
		static VkInstance HANDLE_VK_INSTANCE;

		static VkPhysicalDevice HANDLE_VK_DEVICE_PHYSICAL;
		static VkDevice         HANDLE_VK_DEVICE_LOGIC;

		static VkSurfaceKHR   HANDLE_VK_SURFACE;
		static VkSwapchainKHR HANDLE_VK_SWAPCHAIN;

		static VkQueue HANDLE_VK_QUEUE_GRAPH;
		static VkQueue HANDLE_VK_QUEUE_PRESENT;

		static VkCommandPool    HANDLE_VK_POOL_COMMAND;
		static VkDescriptorPool HANDLE_VK_POOL_DESC;
	};

	namespace TextureSamplers {
		// core vulkan texture_sampler(s) manager system.
		// manager: threads-safe.
		class TextureSamplersManager :public VulkanCoreError, protected VulkanCoreStaticResource,
			public CONTROLLER::VulkanCoreControllerCounter
		{
		private:
			static std::vector<VkSampler> FrameworkConstSampler;
			static std::mutex             FrameworkConstSamplerMutex;
			// create vulkan texture_sampler, [filetr,mode].
			void CreateTextureSampler(VkSampler* sampler, VkFilter filter, VkSamplerAddressMode mode);
		public:
			// core framework call.
			void VKFC_TextureSamplersCreate();
			void VKFC_TextureSamplersDelete();

			VkSampler GetPresetTextureSampler(RZVK_SAMPLE_MODE mode);
		};
	}
	
	using RZVK_VKDUMCIEXT = VkDebugUtilsMessengerCreateInfoEXT;
	using RZVK_RESOURCE   = CONTROLLER::ControllerPointerFamily;
	// rz_vulkan framework core. RCSZ.
	// version 0.1.1 - 20240828
	// vulkan core resource,handle,config...
	class VulkanCore :public VulkanCoreError,
		public VulkanCoreExtensions, public VulkanCoreStaticResource, public CONTROLLER::VulkanCoreController
	{
	private:
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static FmtVector2T<uint32_t> SystemWindowSize;
	protected:
		std::vector<VkFramebuffer> SwapChainFrameBuffers = {};
		std::vector<VkImage>       SwapChainImages       = {};
		std::vector<VkImageView>   SwapChainImageViews   = {};

		bool SwapChainDepthTexFlag = false;
		// managed by core_framework (create & delete).
		VulkanTextureDepth SwapChainDepthTex = {};

		VkSemaphore SemaphoreImageAvailable = VK_NULL_HANDLE;
		VkSemaphore SemaphoreRenderFinished = VK_NULL_HANDLE;

		VkFence FenceInFlight = VK_NULL_HANDLE;

		std::vector<VkPhysicalDevice>      ArrayPhysicalDevice = {};
		std::vector<VkExtensionProperties> ArrayExtension      = {};

		// call: "VulkanCreatePresentation" => storage.
		QueueFamilyIndices StorageFamilyIndices = {};
		// physical device(index), default_type[0].
		size_t PHYD_INDEX = 0;
		VkDebugUtilsMessengerEXT DebugInstance = {};

		bool FlagEnableVerificationLayer = true;
		bool FlagEnableDeviceTest        = true;

		void VulkanCreateValidation(RZVK_VKDUMCIEXT* debug_info);
		void VulkanCreateInstance();
		void VulkanCreateWindowSurface();

		bool VulkanDeviceExtensions();

		// create device: return: enable all extensions.
		VkDeviceCreateInfo DeviceCreateConfig = {};
		bool VulkanCreateDevice();
		void VulkanCreateDeviceQueue();

		SwapChainSupportDetails SwapChainDetails = {};
		VkExtent2D              SwapChainExtent  = {};

		bool VulkanSwapChainSupport(bool swapchain);
		void VulkanCreateSwapchain();
		void VulkanCreateSwapchainImages();
		void VulkanCreateSwapchainImageViews();

		void VulkanCreateDescriptorPool(uint32_t max_desc);

		// swapchain frame_buffer.
		void VulkanCreateFrameBuffer(VkRenderPass render_pass, bool enable_depth);
		void VulkanCreateCommandPool();
		void VulkanCreateSyncObjects();

		VkRenderPass PresentationRenderPass = VK_NULL_HANDLE;
		bool RecreateSwapChainState = false;
		void DynamicRecreateSwapChain();

		void VulkanFreeDeleteSwapChain();
		void VulkanFreeDelete();

		VkWin32SurfaceCreateInfoKHR VulkanWindowConfig = {};
#if defined(_WIN32) && defined(GLFW_VERSION_MAJOR)
		GLFWwindow* WindowObjectPtr = nullptr;
#endif
		uint32_t DrawImageIndexTemp = NULL;
	public:
		// ******************************** CORE OPER ********************************
		~VulkanCore() { VulkanFreeDelete(); }

		// vulkan get window window_handle & module_handle.
		void RzVulkanGetWindowHD(HWND window_hd, HINSTANCE module_hd);

		// windows and glfw_library => get window_info.
		// register frame_resize callback_func.
#if defined(_WIN32) && defined(GLFW_VERSION_MAJOR)
		void RzVulkanWin32GLFWwindowHD(GLFWwindow* window);
		// resize => render_err: error-20240903, RCSZ.
		void RzVulkanWin32FLFWwindowResize(FmtVector2T<uint32_t> new_size);
#endif
		// rz_vk framework init, max_descriptor:
		// uniform(type)desc_max num, image_sampler(type)desc_max num / 2.
		// return display_fbuffer_format: x: color_fmt, y: depth_fmt.
		FmtVector2T<VkFormat> RzVulkanFrameworkInit(bool depth = true, uint32_t max_desc = 128);
		// 在初始化创建 shader 之后 load out_render_pass => start rz_vk 完整框架.
		bool RzVulkanFrameworkPassLoad(VkRenderPass presentation_pass);
		void RzVulkanFrameworkStart();

		// get vulkan out_window render_pass (draw)command submit config(info).
		RenderCommand::CommandSubmit RzVulkanGetOutRenderCmdSubmitConfig();
		RZVK_RESOURCE* RzVulkanGetResourcePtr();

		VkFramebuffer RZVK_DrawFrameContextBegin();
		void RZVK_DrawFrameContextEnd();
	};

	namespace RenderPass {
		// render_pass params: handle_ptr, max_sub_pass.
		struct VulkanRenderPassData {
			uint32_t     RenderMaxSubPass;
			VkRenderPass RenderPass;

			VulkanRenderPassData() : RenderMaxSubPass(NULL), RenderPass(VK_NULL_HANDLE) {}
			VulkanRenderPassData(uint32_t max, VkRenderPass ptr) : RenderMaxSubPass(max), RenderPass(ptr) {}
		};

		constexpr size_t PassAttachmentsMax = 128;
		// core vulkan render_pass manager system.
		// create + (storage)resource.
		class RenderPassManger :public VulkanCoreError, protected VulkanCoreStaticResource,
			public CONTROLLER::VulkanCoreControllerCounter 
		{
		protected:
			// render_pass pointer: manager create + delete.
			VkRenderPass VulkanRenderPass = VK_NULL_HANDLE;
			const char* RenderPassTagName = {};

			// index = size_t_max(ui64): non-depth.
			size_t DepthAttachmentIndex = SIZE_MAX;

			VkAttachmentReference DepthAttachmentRef = {};
			// attachment ref => pass create config.
			size_t AttachmentsRefCount = 0;
			VkAttachmentReference DatasetAttachmentsRef[PassAttachmentsMax] = {};

			std::vector<VkAttachmentDescription> DatasetAttachments    = {};
			std::vector<VkSubpassDependency>     DatasetSubDependency  = {};
			std::vector<VkSubpassDescription>    DatasetSubRenderPass  = {};
			// color & depth clear_vaules.
			VulkanClearValues AttachmentsClear = {};
		public:
			RenderPassManger() { ++ObjectsRenderPass; }
			~RenderPassManger();

			// render_pass unique.
			// [optional], warning: after the color_attachment.
			void CreateDepthAttachment(
				VkFormat dep_format, const FmtVector2T<float>& clear = RZVK_CLEAR_DEPTH
			);
			// push config... return storage(index).
			size_t PushCreateAttachment(
				VkFormat img_format, RZVK_CONFIG_OPERATE_MODE oper, const FmtVector4T<float>& clear = RZVK_CLEAR_COLOR
			);
			size_t PushCreateSubRenderPass(size_t attchment_index);
			// subpass: vector_x: sub_src => vector_y: sub_dst.
			size_t PushCreateSubDependency(const FmtVector2T<size_t>& subpass, RZVK_CONFIG_DEPEND_MODE oper);

			void CreateRenderPass(const char* tag_name = "default");

			VulkanRenderPassData GetRenderPassData();
			VulkanClearValues    GetRenderPassClearValues();
		};
	}

	namespace RenderCommand {
		// buffer offset,size: points_coord begin - end.
		struct RectangleRange {
			FmtVector2T<float> BeginCoord, EndCoord;

			RectangleRange() : BeginCoord({}), EndCoord({}) {}
			RectangleRange(const FmtVector2T<float>& begin, const FmtVector2T<float>& end) : BeginCoord(begin), EndCoord(end) {}
		};

		struct CommandSubmit {
			VkFence      CommandSubmitFence  = VK_NULL_HANDLE;
			VkSubmitInfo CommandSubmitConfig = {};
		};

		struct CMDparams_VertexBufferBind {
			VkBuffer UniformBuffer = VK_NULL_HANDLE;
			// vertex_buffer data params.
			uint32_t BindingFirst = NULL, BindingCount = NULL;
			uint64_t BufferOffsetBytes = 0;
		};

		struct CMDparams_BeginRenderPass {
			VkFramebuffer  RenderPassBuffer = VK_NULL_HANDLE;
			VkRenderPass   RenderPass       = VK_NULL_HANDLE;
			RectangleRange RenderRangeSize  = {};
			// clear color: rgba(fp32) [0.0f,1.0f], clear depth: stencil.
			VulkanClearValues ClearValues = {};
		};

		// core vulkan render_command manager system.
		class RenderCommandManager :public VulkanCoreError, protected VulkanCoreStaticResource,
			public CONTROLLER::VulkanCoreControllerCounter 
		{
		protected:
			VkCommandBuffer VulkanCommandBuffer = VK_NULL_HANDLE;
			const char* CommandBufferTagName = {};
		public:
			RenderCommandManager() { ++ObjectsRenderCommand; }
			~RenderCommandManager();

			void ClearCommand();
			// record command context_begin.
			bool RecordCommandBegin();
			// record command context_end.
			bool RecordCommandEnd();

			void CreateRenderCommandBuffer(const char* tag_name = "default");
			bool SubmitRenderCommandBuffer(const CommandSubmit& params);

			void CommandViewportScissor(const VkViewport& viewport);
			void CommandGraphDraw(uint32_t ver, uint32_t off_ver = 0, uint32_t inst = 1, uint32_t off_inst = 0);
			void CommandNextSubpass();

			void CommandBindShader(VkPipeline shader);
			void CommandBindDescriptorSet(VkDescriptorSet desc, VkPipelineLayout layout);
			void CommandBindVertexBuffer(const CMDparams_VertexBufferBind& params);

			void CommandBeginRenderPass(const CMDparams_BeginRenderPass& params);
			void CommandEndRenderPass();

			void CommandConvertPresent(VkImage image);
		};
	}

	// vertex_attribute [ONLY], pipeline_layout [ONLY], uniform_buffer[ONLY].
	// image_textures [MULT].
	// uniform_buffer binding: 0, texture_sampler binding: 1. RCSZ. 20240905.
	struct VulkanCreateShaderConfig {
		std::vector<VulkanTextureColor> ConfigImageTextures = {};

		RzVkBindingUi32 SubRenderPassBinding = 0;
		RenderPass::VulkanRenderPassData RenderPass = {};

		// true: shader lines_render_mode.
		bool EnableLinesMode = false;

		VkViewport            ConfigRenderViewport = {};
		VulkanVertexAttribute ConfigVertAttribute  = {}; // [RES].
		VulkanShaderLayout    ConfigPipelineLayout = {}; // [RES].
		VulkanUniformBuffer   ConfigUniformBuffer  = {}; // [RES].
	};

	enum VulkanVectorType {
		AttributeVector1 = sizeof(float) * 1,
		AttributeVector2 = sizeof(float) * 2,
		AttributeVector3 = sizeof(float) * 3,
		AttributeVector4 = sizeof(float) * 4,
	};

	// vertex attributes is config_info(non-vk_destroy).
	class VulkanCreateVertexAttribute :public VulkanCoreError, public CreateInterfaceVA {
	protected:
		VulkanVertexAttribute VertexAttrubutesTemp = {};

		uint32_t VertexBufferBinding = NULL;
		uint32_t VertexBytesCount    = NULL;
		uint32_t VertexLocationCount = NULL;
	public:
		void ConstSettingBinding(uint32_t bind_count) {
			VertexBufferBinding = bind_count;
		}
		size_t PushSettingAttribute(VulkanVectorType type, const char* tag_name);

		// non_attrib true: items = empty.
		void CreateAttributes(bool non_attrib = false);

		VulkanVertexAttribute __MS_STORAGE(bool* storage_state) override;
	};

	// vulkan craete buffer & buffer_(device)memory.
	class CREATE_BUFFER_MEMORY :public virtual VulkanCoreError, public virtual VulkanCoreStaticResource {
	protected:
		// device => find buffer_memory type.
		uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
		// create buffer & buffer_(device)memory.
		void CreateVulkanBufferMemory(
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
			VkBuffer& buffer, VkDeviceMemory& bufferMemory
		);
		const char* LOG_NAME_MODULE     = {}; 
		const char* LOG_NAME_BUFFER_TAG = {};
	};

	// single_time command context. (比 RenderCommand 更加轻量).
	class CREATE_SINGLE_TIME_COMMAND :public virtual VulkanCoreError, public virtual VulkanCoreStaticResource {
	private:
		VkCommandBuffer SingleTimeCommandBuffer = VK_NULL_HANDLE;
	protected:
		void ContextSingleTimeCommandsBegin();
		void ContextSingleTimeCommandsEnd();
		// get command_buffer handle.
		VkCommandBuffer GetCommandBuffer() { return SingleTimeCommandBuffer; }
	};

	// vulkan craete vertex_data buffer, create + upload_data.
	class VulkanCreateVertexBuffer :public CREATE_BUFFER_MEMORY, public CreateInterfaceVB {
	protected:
		VulkanVertexBuffer VulkanVertexBufferTemp = {};
	public:
		bool CreateVertexBuffer(const std::vector<float>& vertex_data, uint32_t binding);

		VulkanVertexBuffer __MS_STORAGE(bool* storage_state) override;
	};

	// vulkan craete uniform_data buffer.
	class VulkanCreateUniformBuffer :public CREATE_BUFFER_MEMORY, public CreateInterfaceUB {
	protected:
		VulkanUniformBuffer VulkanUniformBufferTemp = {};
		// gpu uniform memory bytes_alignment.
		size_t UniformMemoryBytes = NULL;
	public:
		template<typename TU, typename = std::enable_if_t<std::is_class<TU>::value>>
		void UniformStructMemory() {
			UniformMemoryBytes = sizeof(TU);
		}
		// uniform buffer binding: layout setting.
		bool CreateUniformBuffer();

		VulkanUniformBuffer __MS_STORAGE(bool* storage_state) override;
	};
	
	class CREATE_TEXTURE_VIEW :public CREATE_BUFFER_MEMORY, public CREATE_SINGLE_TIME_COMMAND {
	protected:
		void CreateTextureImage(
			uint32_t width, uint32_t height, VkFormat format,
			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage* image, VkDeviceMemory* image_memory
		);
		void CreateTextureImageView(
			VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView* image_view
		);
		const char* LOG_TXETURE_TYPE   = {};
		const char* LOG_TEXTURE_MODULE = {};
	};

	// create color_texture & upload image_src_data (cpu => gpu). [manager_tex]
	class VulkanCreateColorTexture :public CREATE_TEXTURE_VIEW, public CreateInterfaceCT {
	protected:
		VulkanTextureColor VulkanTextureImageTemp = {};
		// buffer & buffer_memory upload_temp.
		VkBuffer       TextureUploadBuffer    = VK_NULL_HANDLE;
		VkDeviceMemory TextureUploadBufferMem = VK_NULL_HANDLE;

		FmtVector2T<uint32_t> TextureSizeTemp = {};
		// create buffer => upload image data => gpu_memory.
		void TextureSourceDataUpload(const RzVkImageSRC& load_image);

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	public:
		// create color_texture(upload_image data).
		bool CreateImageTexture(const RzVkImageSRC& image_data, RZVK_SAMPLE_MODE sampler = TEX_NEAREST_AND_REPEAT);
		// create color_texture shader_to_shader use => attachment.
		bool CreateColorTexture(const FmtVector2T<uint32_t>& tex_size, RZVK_SAMPLE_MODE sampler);

		VulkanTextureColor __MS_STORAGE(bool* storage_state) override;
	};

	// create depth_texture. [manager_tex]
	// "VulkanCore" main framework usage(1).
	class VulkanCreateDepthTexture :public CREATE_TEXTURE_VIEW, public CreateInterfaceDT {
	protected:
		VulkanTextureDepth VulkanTextureDepthTemp = {};
		// depth texture support_format.
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	public:
		bool CreateDepthTexture(const FmtVector2T<uint32_t>& tex_size);

		VulkanTextureDepth __MS_STORAGE(bool* storage_state) override;
	};

	// vulkan create pipeline_layout => vulkan shader(pipeline).
	// pipeline_layout(desc): uniform, sampler, buffer, constant ...
	class VulkanCreateLayout :public VulkanCoreError, protected VulkanCoreStaticResource, 
		public CreateInterfaceSL
	{
	protected:
		static std::atomic<uint32_t> VulkanShaderLayoutCount;
		VulkanShaderLayout           VulkanShaderLayoutTemp = {};
		// descriptor_set layout(s), config.
		std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutConfig = {};
	public:
		size_t CreateLayoutUniformBuffer(VulkanShaderType visib); // [optional]
		size_t CreateLayoutUniformSampler();                      // [optional]

		// vulkan shader(pipeline) layout.
		bool CreatePipelineShaderLayout();

		VulkanShaderLayout __MS_STORAGE(bool* storage_state) override;
	};

	constexpr RzVkBindingUi32 ShaderUniformBinding = 0;
	constexpr RzVkBindingUi32 ShaderSamplerBinding = 1;

	// vulkan create shader(pipline), code_src: 'SPIRV'.
	// config: vertex_attributes, pipeline_layout 
	class VulkanCreateShader :public VulkanCoreError, protected VulkanCoreStaticResource, 
		public CreateInterfaceSP
	{
	protected:
		VulkanShader VulkanShaderTemp = {};

		VulkanShaderModule ShaderModuleVertex   = {};
		VulkanShaderModule ShaderModuleFragment = {};

		std::vector<VkPipelineShaderStageCreateInfo> DatasetShadersStage = {};
		VulkanShaderModule CreateShaderModule(const RzVkShaderSPIRV& code, VulkanShaderType type);
	public:
		void ShaderLoaderVert(const RzVkShaderSPIRV& code);
		void ShaderLoaderFrag(const RzVkShaderSPIRV& code);

		// vulkan shader(pipeline).
		bool CreatePipelineShader(const VulkanCreateShaderConfig& config);

		VulkanShader __MS_STORAGE(bool* storage_state) override;
	};

	// 销毁非 "VulkanCoreHandlesResource" 成员的 vulkan 句柄(ptr).
	class VulkanDeleteHandle :protected VulkanCoreStaticResource {
	public:
		bool UnmapVkDeviceMemory(VkDeviceMemory ptr) {
			if (ptr == VK_NULL_HANDLE)
				return false;
			vkUnmapMemory(HANDLE_VK_DEVICE_LOGIC, ptr);
			return true;
		}

		void DeleteVkDescriptorSetLayout(VkDescriptorSetLayout ptr) {
			vkDestroyDescriptorSetLayout(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkBuffer(VkBuffer ptr) {
			vkDestroyBuffer(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkDeviceMemory(VkDeviceMemory ptr) {
			vkFreeMemory(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkPipeline(VkPipeline ptr) {
			vkDestroyPipeline(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkPipelineLayout(VkPipelineLayout ptr) {
			vkDestroyPipelineLayout(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkSampler(VkSampler ptr) {
			vkDestroySampler(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkTextureImage(VkImage ptr) {
			vkDestroyImage(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
		void DeleteVkTextureImageView(VkImageView ptr) {
			vkDestroyImageView(HANDLE_VK_DEVICE_LOGIC, ptr, nullptr);
			ptr = VK_NULL_HANDLE;
		}
	};
}

namespace RZ_VULKAN_RESOURCE {
	RZ_VULKAN_DEFLABEL RZVK_RESOURCE_TAG = "rzvk_resource";

	// [T-SAFE]
	class ResourceVkVertexAttribute :public ResourceInterfaceVA, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanVertexAttribute> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkVertexAttribute(size_t init_map) : 
			ResourceHashMap(init_map) 
		{}
		RZ_VULKAN::VulkanVertexAttribute ResourceFind(VKLRES_KEY key)            override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceVA* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkVertexAttribute() {
			// vertex_attribute: not vk_handle(ptr).
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(va): %u items.", ResourceSize()));
		}
	};

	class ResourceVkVertexBuffer :public ResourceInterfaceVB, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanVertexBuffer> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkVertexBuffer(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanVertexBuffer ResourceFind(VKLRES_KEY key)               override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceVB* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkVertexBuffer() {
			for (auto& item : ResourceHashMap) {
				// vertex_buffer: free_buffer => free_device_mem.
				DeleteVkBuffer(item.second.VertexBuffer);
				DeleteVkDeviceMemory(item.second.VertexBufferMemory);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(vb): %u items.", ResourceSize()));
		}
	};

	class ResourceVkUniformBuffer :public ResourceInterfaceUB, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanUniformBuffer> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkUniformBuffer(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanUniformBuffer ResourceFind(VKLRES_KEY key)              override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceUB* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkUniformBuffer() {
			for (auto& item : ResourceHashMap) {
				// uniform_buffer: unmap_mem => free_buffer => free_device_mem.
				UnmapVkDeviceMemory(item.second.UniformBufferMemory);
				DeleteVkBuffer(item.second.UniformBuffer);
				DeleteVkDeviceMemory(item.second.UniformBufferMemory);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(ub): %u items.", ResourceSize()));
		}
	};

	class ResourceVkShaderLayout :public ResourceInterfaceSL, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanShaderLayout> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkShaderLayout(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanShaderLayout ResourceFind(VKLRES_KEY key)               override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceSL* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkShaderLayout() {
			for (auto& item : ResourceHashMap) {
				// shader(pipeline)_layout: free_desc_set_layout => free_shader_layout.
				DeleteVkDescriptorSetLayout(item.second.LayoutDescriptorSet);
				DeleteVkPipelineLayout(item.second.LayoutPipeline);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(sl): %u items.", ResourceSize()));
		}
	};

	class ResourceVkShader :public ResourceInterfaceSP, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanShader> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkShader(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanShader ResourceFind(VKLRES_KEY key)                     override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceSP* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkShader() {
			for (auto& item : ResourceHashMap) {
				// shader(pipeline): free_desc_set_layout => free_shader_layout.
				// "item.second.ShaderDescriptorSet" => desc_pool.
				DeleteVkPipeline(item.second.ShaderPipeline);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(sp): %u items.", ResourceSize()));
		}
	};

	class ResourceVkColorTexture :public ResourceInterfaceCT, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanTextureColor> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkColorTexture(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanTextureColor ResourceFind(VKLRES_KEY key)               override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceCT* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkColorTexture() {
			for (auto& item : ResourceHashMap) {
				// texture_image: free_texview => free_tex => free_device_mem.
				// sampler: manager_free.
				DeleteVkTextureImageView(item.second.ColorTextureView);
				DeleteVkTextureImage(item.second.ColorTexture);
				DeleteVkDeviceMemory(item.second.TextureBufferMemory);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(ti): %u items.", ResourceSize()));
		}
	};

	class ResourceVkDepthTexture :public ResourceInterfaceDT, public RZ_VULKAN::VulkanDeleteHandle {
	protected:
		std::unordered_map<VKLRES_KEY, RZ_VULKAN::VulkanTextureDepth> ResourceHashMap = {};
		std::mutex ResourceThreadMutex = {};
		std::atomic<VKLRES_KEY> ResourceCounter = 1;
	public:
		ResourceVkDepthTexture(size_t init_map) :
			ResourceHashMap(init_map)
		{}
		RZ_VULKAN::VulkanTextureDepth ResourceFind(VKLRES_KEY key)               override;
		bool ResourceStorage(VKLRES_KEY* key, RZ_VULKAN::CreateInterfaceDT* res) override;
		bool ResourceDelete(VKLRES_KEY key)                                      override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceThreadMutex);
			return ResourceHashMap.size();
		}

		~ResourceVkDepthTexture() {
			for (auto& item : ResourceHashMap) {
				// texture_depthe: free_texview => free_tex => free_device_mem.
				// sampler: manager_free.
				DeleteVkTextureImageView(item.second.DepthTextureView);
				DeleteVkTextureImage(item.second.DepthTexture);
				DeleteVkDeviceMemory(item.second.TextureBufferMemory);
			}
			RZVK_FUNC_LOGGER(RZ_VULKAN::RzVkLog(LogTrace, RZVK_RESOURCE_TAG, "free hashmap(td): %u items.", ResourceSize()));
		}
	};
}

#endif