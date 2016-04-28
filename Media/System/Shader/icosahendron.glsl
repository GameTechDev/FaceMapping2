

// -------------------------------------
layout (std140, row_major) uniform cbPerModelValues
{
    mat4 World;
    mat4 WorldViewProjection;
    mat4 InverseWorld;
    mat4 LightWorldViewProjection;
    vec3 BoundingBoxCenterWorldSpace;
    vec3 BoundingBoxHalfWorldSpace;
    vec3 BoundingBoxCenterObjectSpace;
    vec3 BoundingBoxHalfObjectSpace;
};

// -------------------------------------
layout (std140, row_major) uniform cbPerFrameValues
{
    mat4  View;
    mat4  Projection;
    vec3  AmbientColor;
    vec3  LightColor;
    float TotalTimeInSeconds;
    mat4  InverseView;
    mat4  ViewProjection;
    vec3  LightDirection;
    vec3  EyePosition;
};


#ifdef GLSL_VERTEX_SHADER

in vec4 Position;
out vec3 vPosition;

void main()
{
    vPosition = Position.xyz;
}
#endif

#ifdef GLSL_TESS_CONTROL_SHADER

layout(vertices = 3) out;
in vec3 vPosition[3];
out vec3 tcPosition[3];
//uniform float TessLevelInner = 1;
//uniform float TessLevelOuter = 1;

//#define ID gl_InvocationID

void main()
{
	float TessLevelInner = 1;
	float TessLevelOuter = 1;

    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}

#endif

#ifdef GLSL_TESS_EVALUATION_SHADER

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[3];
out vec3 tePosition;
out vec3 tePatchDistance;
//uniform mat4 Projection;
//uniform mat4 Modelview;

void main()
{
//    vec3 p0 = vec3(gl_TessCoord.x);// * tcPosition[0];
//    vec3 p1 = vec3(gl_TessCoord.y);// * tcPosition[1];
//    vec3 p2 = vec3(gl_TessCoord.z);// * tcPosition[2];
    vec3 p0 = vec3(gl_TessCoord.x) * tcPosition[0];
    vec3 p1 = vec3(gl_TessCoord.y) * tcPosition[1];
    vec3 p2 = vec3(gl_TessCoord.z) * tcPosition[2];
    tePatchDistance = gl_TessCoord;
    tePosition = normalize(p0 + p1 + p2);
	tePosition = (p0 + p1 + p2)/3;
    gl_Position = vec4(tePosition, 1) * WorldViewProjection;
	//gl_Position = Projection * Modelview * vec4(tePosition, 1);
}

#endif

#ifdef GLSL_GEOMETRY_SHADER

precision highp float;
uniform mat4 Modelview;
uniform mat3 NormalMatrix;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 tePosition[3];
in vec3 tePatchDistance[3];
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

void main()
{
    vec3 A = tePosition[2] - tePosition[0];
    vec3 B = tePosition[1] - tePosition[0];
    gFacetNormal = NormalMatrix * normalize(cross(A, B));

    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0.0, 0.0, 1.0);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0.0, 1.0, 0.0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1.0, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    EndPrimitive();
}

#endif

#ifdef GLSL_FRAGMENT_SHADER
precision highp float;
out vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;
//uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial = vec3(1, 1, 1);
uniform vec3 AmbientMaterial = vec3(.05, .05, .1);

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0.0, 1.0);
    d = 1.0 - exp2(-2.0*d*d);
    return d;
}

void main()
{
    vec3 N = normalize(gFacetNormal);
    vec3 L = LightDirection;
    float df = abs(dot(N, L));
    vec3 color = AmbientMaterial + df * DiffuseMaterial;

    float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
    color = amplify(d1, 40.0, -0.5) * amplify(d2, 60.0, -0.5) * color;

    FragColor = vec4(color, 1.0);
}

#endif