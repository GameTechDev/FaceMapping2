[RasterizerStateDX11]

FillMode = D3D11_FILL_SOLID
CullMode = D3D11_CULL_BACK  

[DepthStencilStateDX11]
DepthEnable = true
DepthFunc   = D3D11_COMPARISON_GREATER_EQUAL

[SamplerDX11_1]
Filter         = D3D11_FILTER_ANISOTROPIC
AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP
AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP
BorderColor0   = 1.0
BorderColor1   = 1.0
BorderColor2   = 1.0
BorderColor3   = 1.0

[SamplerDX11_2]
Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT
AddressU       = D3D11_TEXTURE_ADDRESS_BORDER
AddressV       = D3D11_TEXTURE_ADDRESS_BORDER
ComparisonFunc = D3D11_COMPARISON_GREATER

[SamplerDX11_3]
Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR
AddressU       = D3D11_TEXTURE_ADDRESS_WRAP
AddressV       = D3D11_TEXTURE_ADDRESS_WRAP
BorderColor0   = 1.0
BorderColor1   = 1.0
BorderColor2   = 1.0
BorderColor3   = 1.0