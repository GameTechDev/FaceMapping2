[RasterizerStateDX11]
CullMode = D3D11_CULL_BACK
FillMode = D3D11_FILL_WIREFRAME
AntialiasedLineEnable = true
DepthBias = 10000
DepthBiasClamp = 10000

[DepthStencilStateDX11]
DepthEnable = true
DepthFunc   = D3D11_COMPARISON_GREATER_EQUAL

[SamplerDX11_1]
Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR
AddressU       = D3D11_TEXTURE_ADDRESS_BORDER
AddressV       = D3D11_TEXTURE_ADDRESS_BORDER
BorderColor0   = 0.3
BorderColor1   = 0.3
BorderColor2   = 0.3
BorderColor3   = 1.0

[SamplerDX11_2]
Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT
AddressU       = D3D11_TEXTURE_ADDRESS_BORDER
AddressV       = D3D11_TEXTURE_ADDRESS_BORDER
ComparisonFunc = D3D11_COMPARISON_GREATER

