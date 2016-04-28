
[BlendStateDX11]

[RasterizerStateDX11]
CullMode = D3D11_CULL_NONE

[RenderTargetBlendStateDX11_1]
BlendEnable = false
SrcBlend = D3D11_BLEND_SRC_ALPHA
DestBlend = D3D11_BLEND_INV_SRC_ALPHA

[DepthStencilStateDX11]
DepthEnable = false
DepthFunc = D3D11_COMPARISON_GREATER_EQUAL

[SamplerDX11_1]
MipLODBias = 0

[SamplerDX11_2]
MipLODBias = 0

[SamplerDX11_3]
MipLODBias = 0


[DepthStencilStateOGL]
DepthEnable = false

[RasterizerStateOGL]
DepthClipEnable = true
ScissorEnable = false
MultisampleEnable = false
AntialiasedLineEnable = false

[RenderTargetBlendStateOGL]
blendenable = true
srcblend = GL_SRC_ALPHA
destblend = GL_ONE_MINUS_SRC_ALPHA
srcblendalpha = GL_ONE
destblendalpha = GL_ONE
blendop = gl_func_add
blendopalpha = gl_func_add


[SamplerOGL_1]
MinFilter = GL_NEAREST
MagFilter = GL_NEAREST
AddressU = GL_CLAMP_TO_EDGE
AddressV = GL_CLAMP_TO_EDGE
AddressW = GL_CLAMP_TO_EDGE