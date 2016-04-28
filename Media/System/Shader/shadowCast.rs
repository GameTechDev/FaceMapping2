
[DepthStencilStateDX11]
DepthEnable = true
DepthFunc = D3D11_COMPARISON_GREATER_EQUAL

[RasterizerStateDX11]
CullMode = D3D11_CULL_BACK
SlopeScaledDepthBias = -1.0f;

[SamplerDX11_1]

[SamplerDX11_2]
ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL
Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT

[DepthStencilStateOGL]
DepthEnable = true
depthfunc = gl_gequal

[RasterizerStateOGL]
DepthClipEnable = true
ScissorEnable = false
MultisampleEnable = false
AntialiasedLineEnable = false

[SamplerOGL_2]
ComparisonFunc = gl_gequal
MinFilter = GL_NEAREST
MagFilter = GL_NEAREST
AddressU = GL_CLAMP_TO_EDGE
AddressV = GL_CLAMP_TO_EDGE
AddressW = GL_CLAMP_TO_EDGE