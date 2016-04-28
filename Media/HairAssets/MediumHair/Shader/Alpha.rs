[SamplerOGL_shadow]
MinFilter = GL_NEAREST
MagFilter = GL_NEAREST
AddressU = GL_REPEAT
AddressV = GL_REPEAT
AddressW = GL_CLAMP_TO_EDGE
ComparisonMode = GL_COMPARE_REF_TO_TEXTURE
ComparisonFunc = GL_LESS

[SamplerOGL_default]
MinFilter = GL_LINEAR_MIPMAP_LINEAR
MagFilter = GL_LINEAR
AddressU = GL_REPEAT
AddressV = GL_REPEAT
AddressW = GL_CLAMP_TO_EDGE
ComparisonMode = GL_COMPARE_REF_TO_TEXTURE
ComparisonFunc = GL_LESS

[SamplerDX11_default]

[SamplerDX11_shadow]
ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL
Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT
AddressU       = D3D11_TEXTURE_ADDRESS_BORDER
AddressV       = D3D11_TEXTURE_ADDRESS_BORDER

[RenderTargetBlendStateOGL]
blendenable = true
srcblend = GL_SRC_ALPHA
destblend = GL_ONE_MINUS_SRC_ALPHA
srcblendalpha = GL_ONE
destblendalpha = GL_ONE
blendop = gl_func_add
blendopalpha = gl_func_add

[DepthStencilStateOGL]
depthenable = true
depthwritemask = false



[RasterizerStateOGL]
CullingEnabled = gl_false


[RenderTargetBlendStateDX11_1]
BlendEnable = true
SrcBlend = D3D11_BLEND_SRC_ALPHA
DestBlend = D3D11_BLEND_INV_SRC_ALPHA

[DepthStencilStateDX11]
DepthEnable = true
DepthFunc = D3D11_COMPARISON_GREATER_EQUAL
depthwritemask = D3D11_DEPTH_WRITE_MASK_ALL

[RasterizerStateDX11]
CullMode = D3D11_CULL_NONE
