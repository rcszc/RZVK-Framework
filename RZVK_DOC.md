### 主要缺陷/问题(1):

```cpp
size_t RenderPassManger::PushCreateAttachment(VkFormat img_format, RZVK_CONFIG_OPERATE_MODE oper, const FmtVector4T<float>& clear);
```
```RZVK_CONFIG_OPERATE_MODE``` 在内部颜色附件依赖存在问题.

### 主要缺陷/问题(2):

```cpp
size_t RenderPassManger::PushCreateSubDependency(const FmtVector2T<size_t>& subpass, RZVK_CONFIG_DEPEND_MODE oper);
```
```RZVK_CONFIG_DEPEND_MODE``` 在内部Subpass依赖存在问题.

### 主要缺陷/问题(3):

目前缺少FrameBuffer的创建和依赖管理.

---

```END``` ```20240912```